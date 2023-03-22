#pragma once

#include "transport_catalogue.h"

void ProcessRequests(TransportCatalogue& catalogue);
void PrintRoute(std::string& line, TransportCatalogue& catalogue);
void PrintStop(std::string& line, TransportCatalogue& catalogue);