// Copyright 2012, Evan Klitzke <evan@eklitzke.org>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include <functional>
#include <iostream>
#include <string>

#include "./src/garfield.h"

namespace po = boost::program_options;

void MapIndexFile(const std::string &index, garfield::Request *req) {
  if (req->path == "/") {
    req->path = index;
  }
}

int main(int argc, char **argv) {
  po::options_description desc("Allowed options");
  desc.add_options()
      ("help", "produce help message")
      ("log-to-stderr", "log to stderr instead of files")
      ("index-file", po::value<std::string>(),
       "the index file to use when mapping /")
      ("port,p", po::value<int>()->default_value(8000),
       "the port to bind on")
      ("daemon", "run as a daemon")
      ("user", po::value<std::string>(), "the port to bind on")
      ("chdir", po::value<std::string>(), "the directory to change to");
      ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc;
    return 1;
  }

  boost::asio::io_service io;

  if (!vm["user"].empty()) {
    io.post(std::bind(garfield::DropPrivileges, vm["user"].as<std::string>()));
  }

  std::string dir = ".";
  if (!vm["chdir"].empty()) {
    dir = vm["chdir"].as<std::string>();
  }

  if (vm.count("log-to-stderr")) {
    garfield::SetLogger(garfield::StdErrLogger);
  } else {
    garfield::SetLogger(garfield::FileLogger);
  }
  garfield::HTTPServer server(&io);
  if (!vm["index-file"].empty()) {
    server.AddRequestTransform(std::bind(MapIndexFile,
                                         vm["index-file"].as<std::string>(),
                                         std::placeholders::_1));
  }
  server.AddRoute("GET", ".*", garfield::StaticFileHandler);
  server.Bind(vm["port"].as<int>());
  if (vm.count("daemon")) {
    garfield::Daemonize(&io, dir);
  } else {
    chdir(dir.c_str());
    io.run();  // start the main loop
  }
  return 0;
}
