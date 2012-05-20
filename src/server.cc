// Copyright 2012, Evan Klitzke <evan@eklitzke.org>

#include "./server.h"

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <sstream>
#include <utility>

#include "./connection.h"
#include "./handlers.h"
#include "./logging.h"
#include "./request.h"
#include "./response.h"
#include "./time.h"

namespace {
const char* FmtLogField(const std::string &field) {
  if (field.empty()) {
    return "-";
  } else {
    return field.c_str();
  }
}
}

namespace garfield {
RouteSpec::RouteSpec(const std::string &verb, const std::string &path,
                     Handler handler)
    :verb_(verb), path_(boost::regex(path)), handler_(handler) {
}

Handler RouteSpec::Match(const std::string &verb, const std::string &path) {
  if (verb == verb_ && boost::regex_match(path, path_)) {
    return handler_;
  }
  return nullptr;
}

HTTPServer::HTTPServer(boost::asio::io_service *io_service)
    :io_service_(*io_service), acceptor_(*io_service) {
}
void HTTPServer::AddRoute(const std::string &verb, const std::string &path, Handler handler) {
  routes_.push_back(RouteSpec(verb, path, handler));
}

void HTTPServer::Bind(int port) {
  boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();
  ScheduleAccept();
}

void HTTPServer::ScheduleAccept() {
  Connection *conn = new Connection(
      new boost::asio::ip::tcp::socket(io_service_),
      std::bind(&HTTPServer::OnRequest, this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3));

  acceptor_.async_accept(
      *conn->sock(),
      std::bind(&HTTPServer::OnNewConnection, this, conn,
                std::placeholders::_1));
}

void HTTPServer::OnNewConnection(Connection *conn,
                                 const boost::system::error_code& error) {
  conn->NotifyConnected();
  ScheduleAccept();
}

void HTTPServer::OnRequest(Connection *conn, Request *req, RequestError err) {
  if (err != OK) {
    delete conn;
    delete req;
    return;
  }
  conn->set_state(PROCESSING);

  for (RequestTransform transform : request_transforms_) {
    transform(req);
  }

  Handler handler = nullptr;
  for (auto route : routes_) {
    handler = route.Match(req->method, req->path);
    if (handler != nullptr) {
      break;
    }
  }
  if (handler == nullptr) {
    handler = NotFoundHandler;
  }

  Response *resp = new Response();
  resp->headers()->AddHeader(
      "Server", "garfield/0.1 (+https://github.com/eklitzke/garfield)");
  if (!conn->keep_alive()) {
    // Only send the Connection: header for "Connection: close", since
    // "Connection: keep-alive" is the default/assumed behavior.
    resp->headers()->AddHeader("Connection", "close");
  }
  resp->headers()->AddHeader("Content-Type", "text/html");
  handler(req, resp);

  std::string response_header = "HTTP/1.1 ";
  response_header += boost::lexical_cast<std::string>(resp->status());
  response_header += " ";
  response_header += resp->GetStatusName();
  response_header += "\r\n";

  std::size_t bytes = 0;
  std::vector<boost::asio::const_buffer> send_bufs;
  send_bufs.push_back(boost::asio::buffer(response_header, response_header.size()));
  send_bufs.push_back(boost::asio::buffer("\r\n", 2));
  for (const std::string chunk : resp->output()) {
    send_bufs.push_back(boost::asio::buffer(chunk, chunk.size()));
    bytes += chunk.size();
  }
  resp->headers()->AddHeader("Date", GetRFC1123Time());
  if (!resp->suppress_content_length()) {
    resp->headers()->SetHeader("Content-Length", boost::lexical_cast<std::string>(bytes));
  }
  std::string hdrs = resp->headers()->GetHeadersAsString();
  send_bufs.insert(send_bufs.begin() + 1, boost::asio::buffer(hdrs, hdrs.size()));

  std::size_t expected_size = 0;
  for (const boost::asio::const_buffer &buf : send_bufs) {
    expected_size += boost::asio::buffer_size(buf);
  }

  conn->set_state(WRITING_RESPONSE);
  boost::asio::async_write(*conn->sock(), send_bufs,
                           std::bind(&HTTPServer::OnWrite, this, conn, req,
                                     resp, expected_size,
                                     std::placeholders::_1,
                                     std::placeholders::_2));

  boost::posix_time::time_duration td = (
      boost::posix_time::microsec_clock::universal_time()- req->connection_time());
  Log(ACCESS, "%s [%s] \"%s %s HTTP/1.%d\" %d %zd %ld \"%s\" \"%s\"",
      req->peername.c_str(),
      GetLogTime().c_str(),
      req->method.c_str(),
      req->path.c_str(),
      req->version.second,
      resp->status(),
      expected_size,
      td.total_microseconds(),
      FmtLogField(req->headers()->GetHeader("Referer")),
      FmtLogField(req->headers()->GetHeader("User-Agent")));
}

void HTTPServer::OnWrite(Connection *conn, Request *req, Response *resp,
                         std::size_t expected_size,
                         const boost::system::error_code& error,
                         std::size_t bytes_transferred) {
  if (error) {
	std::string errstring = boost::lexical_cast<std::string>(error);
	Log(ERROR, "got boost::asio error %s during write", errstring.c_str());
  }
  if (expected_size != bytes_transferred) {
	Log(ERROR, "expected to write %zd bytes during request, but only wrote %zd",
		expected_size, bytes_transferred);
  }
  delete req;
  delete resp;

  if (!conn->keep_alive()) {
    delete conn;
  } else {
    conn->set_state(UNCONNECTED);
    conn->NotifyConnected();
  }
}
}
