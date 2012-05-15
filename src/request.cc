// Copyright 2012, Evan Klitzke <evan@eklitzke.org>

#include "./request.h"

#include <boost/date_time/posix_time/posix_time.hpp>

namespace garfield {
Request::Request()
    :connection_time_(boost::posix_time::microsec_clock::universal_time()) {
}
}
