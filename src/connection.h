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
  MALFORMED_HEADER_LINE  = 3,
  TOO_MUCH_DATA          = 4   // client sent too much data
};

class Connection;

typedef std::function<void(Connection*, Request*, RequestError)> RequestCallback;

class Connection {
 public:
  explicit Connection(boost::asio::ip::tcp::socket *sock,
                      RequestCallback callback);
  ~Connection();
  void NotifyConnected();
  // Getters and Setters
  boost::asio::ip::tcp::socket* sock() const { return sock_; }

  State state() const { return state_; }
  void set_state(State state) { state_ = state; }

  bool keep_alive() const { return keep_alive_; }
  void set_keep_alive(bool keep_alive) { keep_alive_ = keep_alive; }

  std::size_t content_length() const { return content_length_; }
  const std::string& body() const { return body_; }

 private:
  State state_;
  boost::asio::ip::tcp::socket *sock_;
  RequestCallback callback_;
  bool keep_alive_;
  std::size_t content_length_;
  std::string body_;

  // Check the boost error code and handle it appropriately. What this means is
  // logging the error unless it's a client-side thing (e.g. if the client
  // prematurely closes their connection, we'll get a boost error, but it's not
  // our fault -- so that isn't loged).
  //
  // Returns true if there was an error and the caller should "abort" what it's
  // doing, false otherwise.
  bool CheckBoostError(Request *req, const boost::system::error_code &err);

  void OnHeaders(Request *req,
                 const boost::system::error_code &err,
                 std::size_t bytes_transferred);

  void OnBody(Request *req,
              const boost::system::error_code &err,
              std::size_t bytes_transferred);
};
}

#endif  // SRC_CONNECTION_H_
