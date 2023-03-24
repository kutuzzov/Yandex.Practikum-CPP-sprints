#pragma once

#include "transport_catalogue.h"

namespace transport {

void ProcessRequests(Catalogue& catalogue);

namespace detail {

void PrintRoute(std::string& line, Catalogue& catalogue);
void PrintStop(std::string& line, Catalogue& catalogue);

} // namespace detail

} // namespace transport
