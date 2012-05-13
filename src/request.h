// -*- C++ -*-
// Copyright 2012, Evan Klitzke <evan@eklitzke.org>

#ifndef SRC_REQUEST_H_
#define SRC_REQUEST_H_

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <string>
#include <utility>

#include "./headers.h"

namespace garfield {

class Connection;

class Request {
 public:
  explicit Request();
  std::string method;
  std::string path;
  std::pair<int, int> version;

  // Get the connection time.
  const boost::posix_time::microsec_clock& connection_time() const {
    return connection_time_;
  }

  HeadersDict* headers() { return &headers_; }

  boost::asio::streambuf streambuf;

 private:
  boost::posix_time::microsec_clock connection_time_;
  HeadersDict headers_;
};
}

#endif  // SRC_REQUEST_H_
