// -*- C++ -*-
// Copyright 2012, Evan Klitzke <evan@eklitzke.org>

#ifndef SRC_HANDLERS_H_
#define SRC_HANDLERS_H_

#include <functional>

#include "./request.h"
#include "./response.h"

namespace garfield {
typedef std::function<void(Request *, Response *)> Handler;

// Implements HTTP 404 behavior
void NotFoundHandler(Request *req, Response *resp);
}

#endif  // SRC_HANDLERS_H_
