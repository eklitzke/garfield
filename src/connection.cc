// Copyright 2012, Evan Klitzke <evan@eklitzke.org>

#include "./connection.h"

#include <boost/regex.hpp>

#include <cassert>
#include <iterator>
#include <fstream>
#include <functional>
#include <iostream>

namespace {
const boost::regex request_line("^(\\u+) (.*) HTTP/1\\.([01])$");
const boost::regex header_line("^([-a-zA-Z0-9_]+):\\s+(.*?)$");
}

namespace garfield {
Connection::Connection(boost::asio::ip::tcp::socket *sock,
                       RequestCallback callback)
    :state_(UNCONNECTED), sock_(sock), callback_(callback) {
}


void Connection::NotifyConnected() {
  assert(state_ == UNCONNECTED);
  state_ = WAITING_FOR_HEADERS;
  Request *req = new Request();
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
  if (err) {
    std::cerr << "system error" << std::endl;
    callback_(this, req, SYSTEM_ERROR);
    return;
  }

  std::string data(
      (std::istreambuf_iterator<char>(&req->streambuf)),
      std::istreambuf_iterator<char>());

  std::string::size_type offset = 0;
  while (true) {
    std::string::size_type newline = data.find("\r\n", offset);
    if (newline == std::string::npos) {
      std::cerr << "malformed header line" << std::endl;
      callback_(this, req, MALFORMED_HEADER_LINE);
      return;
    }
    std::string line = data.substr(offset, newline - offset);
    boost::smatch what;
    if (offset == 0) {
      // we're reading the first line of the request
      if (!boost::regex_match(line, what, request_line)) {
        std::cerr << "malformed first line" << std::endl;
        callback_(this, req, MALFORMED_FIRST_LINE);
        return;
      }
      req->method = what[1];
      req->path = what[2];
      req->version = std::make_pair(1, what[3] == "0" ? 0 : 1);
    } else if (line == "") {
      break;
    } else {
      if (!boost::regex_match(line, what, header_line)) {
        std::cerr << "malformed header line" << std::endl;
        callback_(this, req, MALFORMED_HEADER_LINE);
        return;
      }
      std::string hdr_name = what[1];
      std::string hdr_val = what[2];
      req->headers()->SetHeader(hdr_name, hdr_val);
    }
    offset = newline + 2;
  }
  callback_(this, req, OK);
}


Connection::~Connection() {
  sock_->cancel();
  delete sock_;
}
}
