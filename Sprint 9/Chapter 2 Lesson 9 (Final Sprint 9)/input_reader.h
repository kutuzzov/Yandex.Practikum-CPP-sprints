#pragma once

#include "transport_catalogue.h"

#include <iostream>

namespace transport {

void FillCatalogue(std::istream& in, Catalogue& catalogue);

std::pair<std::string, geo::Coordinates> FillStop(std::string& line);
void FillStopDistances(std::string& line, Catalogue& catalogue);
std::tuple<std::string, std::vector<const Stop*>, bool> FillRoute(std::string& line, transport::Catalogue& catalogue);

} // namespace transport
