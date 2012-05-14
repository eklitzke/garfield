// Copyright 2012, Evan Klitzke <evan@eklitzke.org>

#include <iostream>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include "./src/garfield.h"

namespace po = boost::program_options;

void HelloWorldHandler(garfield::Request *req, garfield::Response *resp) {
  resp->headers()->SetHeader("Content-Type", "text/plain");
  resp->Write("Hello, world!\n");
}

int main(int argc, char **argv) {
  po::options_description desc("Allowed options");
  desc.add_options()
      ("help", "produce help message")
      ("port,p", po::value<int>()->default_value(8000),
       "the port to bind on")
      ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc;
    return 1;
  }

  boost::asio::io_service ios;
  garfield::HTTPServer server(&ios);
  server.Bind(vm["port"].as<int>());
  server.AddRoute("/", HelloWorldHandler);
  server.AddRoute(".*", garfield::StaticFileHandler);
  ios.run();  // start the main loop
  return 0;
}
