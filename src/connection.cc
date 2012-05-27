// Copyright 2012, Evan Klitzke <evan@eklitzke.org>

#include "./connection.h"

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include <cassert>
#include <iterator>
#include <fstream>
#include <functional>

#include "./logging.h"

namespace {
const boost::regex request_line("^(\\u+) (.*) HTTP/1\\.([01])$");
const boost::regex header_line("^([-a-zA-Z0-9_]+):\\s+(.*?)$");

class ContentLengthMatcher {
 public:
  explicit ContentLengthMatcher(std::size_t expected)
      :expected_(expected), seen_(0) {}

  template <typename Iterator>
  std::pair<Iterator, bool> operator() (Iterator begin, Iterator end) {
    seen_ += end - begin;
    if (seen_ >= expected_) {
      return std::make_pair(end, true);
    }
    return std::make_pair(end, false);
  }

 private:
  std::size_t expected_;
  std::size_t seen_;
};

namespace boost {
namespace asio {
template <> struct is_match_condition<
  decltype(std::bind(&ContentLengthMatcher::match_version, shared_ptr<TcpMimeConnection>(), _1, _2)
                    )>
  : public boost::true_type {};
  ContentLengthMatcher>
    : public boost::true_type {};
}
}

namespace garfield {
Connection::Connection(boost::asio::ip::tcp::socket *sock,
                       RequestCallback callback)
    :state_(UNCONNECTED), sock_(sock), callback_(callback),
     keep_alive_(true), content_length_(0) {
}

void Connection::NotifyConnected() {
  assert(state_ == UNCONNECTED);
  state_ = WAITING_FOR_HEADERS;
  Request *req = new Request();

  boost::asio::ip::tcp::endpoint remote_ep = sock_->remote_endpoint();
  boost::asio::ip::address remote_ad = remote_ep.address();
  req->peername = remote_ad.to_string();

  boost::asio::async_read_until(
      *sock_, req->streambuf, "\r\n\r\n",
      std::bind(
          &Connection::OnHeaders, this, req,
          std::placeholders::_1,
          std::placeholders::_2));
}

void Connection::OnHeaders(Request *req,
                           const boost::system::error_code &err,
                           std::size_t bytes_transferred) {
  assert(state_ == WAITING_FOR_HEADERS);
  if (CheckBoostError(req, err)) {
    return;
  }
  std::string data(
      (std::istreambuf_iterator<char>(&req->streambuf)),
      std::istreambuf_iterator<char>());

  std::string::size_type offset = 0;
  while (true) {
    std::string::size_type newline = data.find("\r\n", offset);
    if (newline == std::string::npos) {
      Log(ERROR, "malformed header line!");
      callback_(this, req, MALFORMED_HEADER_LINE);
      return;
    }
    std::string line = data.substr(offset, newline - offset);
    boost::smatch what;
    if (offset == 0) {
      // we're reading the first line of the request
      if (!boost::regex_match(line, what, request_line)) {
        Log(ERROR, "malformed first line!");
        callback_(this, req, MALFORMED_FIRST_LINE);
        return;
      }
      req->method = what[1];
      req->path = what[2];
      req->version = std::make_pair(1, what[3] == "0" ? 0 : 1);
      if (what[3] == "0") {
        keep_alive_ = false;  // Force non-keep alive for HTTP/1.0
      }
    } else if (line == "") {
      break;
    } else {
      if (!boost::regex_match(line, what, header_line)) {
        Log(ERROR, "malformed header line!");
        callback_(this, req, MALFORMED_HEADER_LINE);
        return;
      }
      HeaderKey hdr_key(what[1]);
      std::string hdr_val = what[2];
      req->headers()->SetHeader(hdr_key, hdr_val);
      if (hdr_key.norm_key == "connection" && hdr_val == "close") {
        keep_alive_ = false;
      } else if (hdr_key.norm_key == "content-length") {
        content_length_ = boost::lexical_cast<std::size_t>(hdr_val);
      }
    }
    offset = newline + 2;
  }
  if (content_length_) {
    state_ = WAITING_FOR_BODY;
    boost::asio::async_read_until(
        *sock_,
        req->streambuf,
        ContentLengthMatcher(content_length_),
        std::bind(&Connection::OnBody, this, req,
                  std::placeholders::_1,
                  std::placeholders::_2));
  } else {
    state_ = PROCESSING;
    callback_(this, req, OK);
  }
}

void Connection::OnBody(Request *req,
                        const boost::system::error_code &err,
                        std::size_t bytes_transferred) {
  assert(state_ == WAITING_FOR_BODY);
  body_ = std::string((std::istreambuf_iterator<char>(&req->streambuf)),
                      std::istreambuf_iterator<char>());

  if (bytes_transferred > content_length_) {
    callback_(this, req, TOO_MUCH_DATA);
  } else {
    state_ = PROCESSING;
    callback_(this, req, OK);
  }
}

Connection::~Connection() {
  sock_->cancel();
  delete sock_;
}

bool Connection::CheckBoostError(Request *req,
                                 const boost::system::error_code &err) {
  if (err) {
    // If the error is because the client closed its connection, it's not our
    // fault, so do nothing at all. If the error is something else, log it.
    if (err != boost::asio::error::connection_reset &&
        err != boost::asio::error::eof) {
      std::string err_name = boost::lexical_cast<std::string>(err);
      Log(ERROR, "system error in OnBody, %s", err_name.c_str());
      
      callback_(this, req, SYSTEM_ERROR);
    }
    return true;
  }
  return false;
}
}
