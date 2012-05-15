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
      ("log-to-stderr", "log to stderr instead of files")
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

  boost::asio::io_service io;
  if (vm.count("log-to-stderr")) {
    garfield::SetLogger(garfield::StdErrLogger);
  } else {
    garfield::SetLogger(garfield::FileLogger);
  }
  garfield::HTTPServer server(&io);
  server.Bind(vm["port"].as<int>());
  server.AddRoute("/", HelloWorldHandler);
  server.AddRoute(".*", garfield::StaticFileHandler);
  io.run();  // start the main loop
  return 0;
}
