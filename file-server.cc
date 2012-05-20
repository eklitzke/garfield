// Copyright 2012, Evan Klitzke <evan@eklitzke.org>

#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include <functional>
#include <iostream>
#include <string>

#include "./src/garfield.h"

namespace po = boost::program_options;

void DropPrivileges(const std::string &user) {
  passwd *pwent = getpwnam(user.c_str());
  setgid(pwent->pw_gid);
  setuid(pwent->pw_uid);
}

void MapIndexFile(const std::string &index, garfield::Request *req) {
  if (req->path == "/") {
    req->path = index;
  }
}

void Daemonize(boost::asio::io_service *io_service, const std::string &dir) {
  boost::asio::signal_set signals(*io_service, SIGINT, SIGTERM);
  signals.async_wait(
      std::bind(&boost::asio::io_service::stop, io_service));

  // Inform the io_service that we are about to become a daemon.
  io_service->notify_fork(boost::asio::io_service::fork_prepare);

  if (pid_t pid = fork()) {
    if (pid > 0) {
      // We're in the parent process and need to exit.
      exit(EXIT_SUCCESS);
    } else {
      perror("fork()");
      exit(EXIT_FAILURE);
    }
  }

  setsid();  // become the session leader
  chdir(dir.c_str());
  umask(0);

  if (pid_t pid = fork()) {
    if (pid > 0) {
      exit(EXIT_SUCCESS);
    } else {
      perror("fork()");
    }
  }

  close(0);
  close(1);
  close(2);

  if (open("/dev/null", O_RDONLY) < 0) {
    exit(EXIT_FAILURE);
  }

  const char* output = "/tmp/asio.daemon.out";
  const int flags = O_WRONLY | O_CREAT | O_APPEND;
  const mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  if (open(output, flags, mode) < 0) {
  }

  if (dup(1) < 0) {
    exit(EXIT_FAILURE);
  }

  io_service->notify_fork(boost::asio::io_service::fork_child);
  io_service->run();
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
    io.post(std::bind(DropPrivileges, vm["user"].as<std::string>()));
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
    Daemonize(&io, dir);
  } else {
    chdir(dir.c_str());
    io.run();  // start the main loop
  }
  return 0;
}
