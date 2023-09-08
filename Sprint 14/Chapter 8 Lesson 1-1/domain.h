#pragma once

#include "geo.h"

#include <string>
#include <vector>
#include <set>
#include <unordered_map>

namespace transport {

struct Stop {
    std::string name;
    geo::Coordinates coordinates;
    std::set<std::string> buses_by_stop;
};

struct Bus {
    std::string number;
    std::vector<const Stop*> stops;
    bool is_circle;
};

struct BusStat {
    size_t stops_count;
    size_t unique_stops_count;
    double route_length;
    double curvature;
};

} // namespace transport