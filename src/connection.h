// -*- C++ -*-
// Copyright 2012, Evan Klitzke <evan@eklitzke.org>

#ifndef SRC_CONNECTION_H_
#define SRC_CONNECTION_H_

#include <boost/asio.hpp>
#include <functional>

#include "./request.h"

namespace garfield {

enum State {
  UNCONNECTED          = 0,
  WAITING_FOR_HEADERS  = 1,
  WAITING_FOR_BODY     = 2,
  PROCESSING           = 3,
  WRITING_RESPONSE     = 4
};

enum RequestError {
  OK                     = 0,
  SYSTEM_ERROR           = 1,
  MALFORMED_FIRST_LINE   = 2,
  MALFORMED_HEADER_LINE  = 3
};

class Connection;

typedef std::function<void(Connection*, Request*, RequestError)> RequestCallback;

class Connection {
 public:
  explicit Connection(boost::asio::ip::tcp::socket *sock,
                      RequestCallback callback);
  ~Connection();
  void NotifyConnected();
  void OnHeaders(Request *req,
                 const boost::system::error_code &err,
                 std::size_t bytes_transferred);

  boost::asio::ip::tcp::socket* sock() const { return sock_; }

 private:
  State state_;
  boost::asio::ip::tcp::socket *sock_;
  RequestCallback callback_;
};
}

#endif  // SRC_CONNECTION_H_
