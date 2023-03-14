#include "input_reader.h"

#include <string>
#include <vector>
#include <cmath>

void FillTransportCatalogue(std::istream& in, TransportCatalogue& catalogue) {
    std::vector<std::string> query_bus;
    size_t requests_count;

    in >> requests_count;
    for (size_t i = 0; i < requests_count; ++i) {
        std::string keyword, line;
        in >> keyword; 
        if (keyword == "Stop") {
            std::getline(in, line);
            std::string stop_name = line.substr(1, line.find_first_of(':') - line.find_first_of(' ') - 1);
            double lat = std::stod(line.substr(line.find_first_of(':') + 2, 9));
            double lng = std::stod(line.substr(line.find_first_of(',') + 2, 9));
            Coordinates stop_coordinates = { lat, lng };
            catalogue.AddStop(stop_name, stop_coordinates);
        }
        if (keyword == "Bus") {
            std::getline(in, line);
            query_bus.push_back(line);
        }
    }

    for (auto& bus_ : query_bus) {
        std::string route_number = bus_.substr(1, bus_.find_first_of(':') - 1);
        bus_.erase(0, bus_.find_first_of(':') + 2);
        auto [route_stops, circular_route] = FillRoute(bus_);
        catalogue.AddRoute(route_number, route_stops, circular_route);
        route_stops.clear();
    }
}

std::pair<std::vector<std::string>, bool> FillRoute(std::string& line) {
    std::vector<std::string> route_stops;
    bool circular_route = false;
    std::string stop_name;
    auto pos = line.find('>') != line.npos ? '>' : '-';
    while (line.find(pos) != line.npos) {
        stop_name = line.substr(0, line.find_first_of(pos) - 1);
        route_stops.push_back(stop_name);
        line.erase(0, line.find_first_of(pos) + 2);
    }
    stop_name = line.substr(0, line.npos - 1);
    route_stops.push_back(stop_name);
    if (pos == '>') circular_route = true;

    return std::make_pair(route_stops, circular_route);
}