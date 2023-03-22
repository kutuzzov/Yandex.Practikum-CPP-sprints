#pragma once

#include "transport_catalogue.h"

#include <iostream>

void FillTransportCatalogue(TransportCatalogue& catalogue);
Bus FillRoute(std::string& line);
Stop FillStop(std::string& line);
void AddStopDistances(std::string& line, TransportCatalogue& catalogue);