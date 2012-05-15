// -*- C++ -*-
// Copyright 2012, Evan Klitzke <evan@eklitzke.org>

#ifndef SRC_SERVER_H_
#define SRC_SERVER_H_

#include "./connection.h"

#include <functional>
#include <string>
#include <utility>
#include <vector>

#include <boost/asio.hpp>
#include <boost/regex.hpp>

#include "./handlers.h"
#include "./request.h"
#include "./response.h"

namespace garfield {

typedef std::function<void(Request *)> RequestTransform;

class HTTPServer {
 public:
  HTTPServer(boost::asio::io_service *io_service);

  // Add a route to the server
  void AddRoute(const std::string &route, Handler handler);

  // Add a Request transform; this transform will be run on the request before
  // the request is mapped.
  void AddRequestTransform(RequestTransform t) {
    request_transforms_.push_back(t);
  }

  // Bind the server to a given port.
  void Bind(int port);

 private:
  std::vector<std::pair<boost::regex, Handler> > routes_;
  std::vector<RequestTransform> request_transforms_;
  boost::asio::io_service &io_service_;
  boost::asio::ip::tcp::acceptor acceptor_;
  boost::asio::streambuf request_data_;

  // Callback called when a new connection is made from a client to our server.
  void OnNewConnection(Connection *conn,
                       const boost::system::error_code& error);

  // Callback called when data comes in on a client connection (note: this will
  // also be called when there's an "error" on the socket, including when our
  // peer closes its connection).
  void OnConnectionRead(Connection *conn,
                        const boost::system::error_code& error,
                        std::size_t bytes_transferred);

  // Called when we have a fully formed request object.
  void OnRequest(Connection *conn, Request *req, RequestError err);

  // Called when data is written during a response.
  void OnWrite(Connection *conn, Request *req, Response *resp,
               std::size_t expected_size,
               const boost::system::error_code& error,
               std::size_t bytes_transferred);

  // Schedule an accept to happen in the event loop; this happens after the
  // socket is initially bound, and subsequently after each new connection.
  void ScheduleAccept();

  // Schedule a read to happen on a connection from a peer; this happens after a
  // new socket is initially accepted, and subsequently after each request is
  // handled on the connection.
  void ScheduleConnectionRead(Connection *conn);
};
}

#endif  // SRC_SERVER_H_
