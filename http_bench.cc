// Copyright 2012, Evan Klitzke <evan@eklitzke.org>

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

namespace po = boost::program_options;

class RequestCounter {
 public:
  explicit RequestCounter(int count) :counter_(count) {}
  bool GetWork() { if (counter_) { counter_--; return true; } return false; }
 private:
  int counter_;
};

class HTTPClient {
 public:
  HTTPClient(boost::asio::io_service *io,
             std::string host, int port, std::string path,
             RequestCounter *counter)
      :port_(port), path_(path), counter_(counter), sock_(*io) {}
  void Run(boost::asio::ip::tcp::resolver::iterator endpoint);
 private:
  std::string host_;
  int port_;
  std::string path_;
  RequestCounter *counter_;
  boost::asio::ip::tcp::socket sock_;
  std::string request_;

  void OnConnect(const boost::system::error_code &error,
                 boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
  void TryGet();
  void OnWrite(const boost::system::error_code &error,
               std::size_t bytes_transferred);
};

void HTTPClient::Run(boost::asio::ip::tcp::resolver::iterator endpoint_iterator) {
  request_ = "GET ";
  request_ += path_;
  request_ += " HTTP/1.1\r\nHost: localhost\r\n\r\n";
  boost::asio::async_connect(sock_, endpoint_iterator,
                             std::bind(&HTTPClient::OnConnect, this,
                                       std::placeholders::_1,
                                       std::placeholders::_2));
}

void HTTPClient::OnConnect(const boost::system::error_code &error,
                           boost::asio::ip::tcp::resolver::iterator endpoint_iterator) {
  if (!error) {
    TryGet();
    //} else if (endpoint_iterator != boost::asio::ip::tcp::resolver::iterator()) {
  } else {
    std::cout << error << std::endl;
    boost::asio::async_connect(sock_, endpoint_iterator,
                               std::bind(&HTTPClient::OnConnect, this,
                                         std::placeholders::_1,
                                         std::placeholders::_2));
  }
}

void HTTPClient::TryGet() {
  if (counter_->GetWork()) {
    boost::asio::async_write(sock_, boost::asio::buffer(request_),
                             std::bind(&HTTPClient::OnWrite, this,
                                       std::placeholders::_1,
                                       std::placeholders::_2));
  }
}

void HTTPClient::OnWrite(const boost::system::error_code &error,
                         std::size_t bytes_transferred) {
  assert(!error);
  assert(bytes_transferred > 0);
  std::cout << this << std::endl;
  TryGet();
}

int main(int argc, char **argv) {
  po::options_description desc("Allowed options");
  desc.add_options()
      ("help", "produce help message")
      ("concurrency,c", po::value<int>()->default_value(8),
       "the number of concurrent connections to use")
      ("num-requests,n", po::value<int>()->default_value(10000),
       "the total number of request to make")
      ("port,p", po::value<int>()->default_value(8000),
       "the port to connect to")
      ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc;
    return 1;
  }

  boost::asio::io_service io;

  int port = vm["port"].as<int>();
  boost::asio::ip::tcp::resolver resolver(io);
  boost::asio::ip::tcp::resolver::query query(
      "127.0.0.1", boost::lexical_cast<std::string>(port),
      boost::asio::ip::resolver_query_base::numeric_service);
  boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(
      query);

  std::vector<HTTPClient *> clients;
  RequestCounter counter(vm["num-requests"].as<int>());
  int concurrency = vm["concurrency"].as<int>();
  for (int i = 0; i < concurrency; i++) {
    HTTPClient client(&io, "localhost", port, "/", &counter);
    client.Run(endpoint_iterator);
    clients.push_back(&client);
  }
  io.run();  // start the main loop
  return 0;
}
