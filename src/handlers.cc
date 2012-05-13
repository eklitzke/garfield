// Copyright 2012, Evan Klitzke <evan@eklitzke.org>

#include "./handlers.h"

namespace garfield {
void NotFoundHandler(Request *req, Response *resp) {
  resp->set_status(404);
  resp->headers()->SetHeader("Content-Type", "text/plain");
  resp->Write("The page you were looking for could not be found.\n");
}
}
