// Copyright 2012, Evan Klitzke <evan@eklitzke.org>

#include "./response.h"

#include <utility>

namespace garfield {
Response::Response()
    :status_(200) {
}

void Response::Write(const std::string &msg) {
  output_.push_back(msg);
}

std::string Response::GetStatusName() {
  std::string name = "???";
  switch (status_) {
    case 100:
      name = "Continue";
      break;
    case 101:
      name = "Switching Protocols";
      break;
    case 200:
      name = "OK";
      break;
    case 201:
      name = "Created";
      break;
    case 202:
      name = "Accepted";
      break;
    case 203:
      name = "Non-Authoritative Information";
      break;
    case 204:
      name = "No Content";
      break;
    case 205:
      name = "Reset Content";
      break;
    case 206:
      name = "Partial Content";
      break;
    case 300:
      name = "Multiple Choices";
      break;
    case 301:
      name = "Moved Permanently";
      break;
    case 302:
      name = "Found";
      break;
    case 303:
      name = "See Other";
      break;
    case 304:
      name = "Not Modified";
      break;
    case 305:
      name = "Use Proxy";
      break;
    case 306:
      name = "(Unused)";
      break;
    case 307:
      name = "Temporary Redirect";
      break;
    case 400:
      name = "Bad Request";
      break;
    case 401:
      name = "Unauthorized";
      break;
    case 402:
      name = "Payment Required";
      break;
    case 403:
      name = "Forbidden";
      break;
    case 404:
      name = "Not Found";
      break;
    case 405:
      name = "Method Not Allowed";
      break;
    case 406:
      name = "Not Acceptable";
      break;
    case 407:
      name = "Proxy Authentication Required";
      break;
    case 408:
      name = "Request Timeout";
      break;
    case 409:
      name = "Conflict";
      break;
    case 410:
      name = "Gone";
      break;
    case 411:
      name = "Length Required";
      break;
    case 412:
      name = "Precondition Failed";
      break;
    case 413:
      name = "Request Entity Too Large";
      break;
    case 414:
      name = "Request-URI Too Long";
      break;
    case 415:
      name = "Unsupported Media Type";
      break;
    case 416:
      name = "Requested Range Not Satisfiable";
      break;
    case 417:
      name = "Expectation Failed";
      break;
    case 500:
      name = "Internal Server Error";
      break;
    case 501:
      name = "Not Implemented";
      break;
    case 502:
      name = "Bad Gateway";
      break;
    case 503:
      name = "Service Unavailable";
      break;
    case 504:
      name = "Gateway Timeout";
      break;
    case 505:
      name = "HTTP Version Not Supported";
      break;
  }
  return name;
}
}
