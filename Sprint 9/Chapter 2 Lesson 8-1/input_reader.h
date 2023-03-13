#pragma once

#include "geo.h"
#include "transport_catalogue.h"

#include <iostream>

void FillTransportCatalogue(std::istream& in, TransportCatalogue& catalogue);
std::pair<std::vector<std::string>, bool> FillRoute(std::string& line);