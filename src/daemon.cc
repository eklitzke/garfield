// -*- C++ -*-
// Copyright 2012, Evan Klitzke <evan@eklitzke.org>

#include "./daemon.h"

#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

#include <boost/asio.hpp>
#include <csignal>
#include <string>

namespace garfield {
void DropPrivileges(const std::string &user) {
  passwd *pwent = getpwnam(user.c_str());
  setgid(pwent->pw_gid);
  setuid(pwent->pw_uid);
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

}
