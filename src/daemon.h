// -*- C++ -*-
// Copyright 2012, Evan Klitzke <evan@eklitzke.org>

#ifndef SRC_DAEMON_H_
#define SRC_DAEMON_H_

#include <boost/asio.hpp>
#include <string>

namespace garfield {
void DropPrivileges(const std::string &user);
void Daemonize(boost::asio::io_service *io_service, const std::string &dir);
}

#endif  // SRC_DAEMON_H_
