#include "stat_reader.h"

#include <iomanip>

namespace transport {

void ProcessRequests(Catalogue& catalogue) {
    size_t requests_count;
    std::cin >> requests_count;
    for (size_t i = 0; i < requests_count; ++i) {
        std::string keyword, line;
        std::cin >> keyword;
        std::getline(std::cin, line);
        if (keyword == "Bus") {
            detail::PrintRoute(line, catalogue);
        }
        if (keyword == "Stop") {
            detail::PrintStop(line, catalogue);
        }
    }
}

namespace detail {

void PrintRoute(std::string& line, Catalogue& catalogue) {
    std::string route_number = line.substr(1, line.npos);
    if (catalogue.FindRoute(route_number)) {
        std::cout << "Bus " << route_number << ": " << catalogue.RouteInformation(route_number).stops_count << " stops on route, "
        << catalogue.RouteInformation(route_number).unique_stops_count << " unique stops, " << std::setprecision(6)
        << catalogue.RouteInformation(route_number).route_length << " route length, "
        << catalogue.RouteInformation(route_number).curvature << " curvature\n";
    }
    else {
        std::cout << "Bus " << route_number << ": not found\n";
    }
}

void PrintStop(std::string& line, Catalogue& catalogue) {
    std::string stop_name = line.substr(1, line.npos);
    if (catalogue.FindStop(stop_name)) {
        std::cout << "Stop " << stop_name << ": ";
        std::set<std::string> buses = catalogue.GetBusesOnStop(stop_name);
        if (!buses.empty()) {
            std::cout << "buses ";
            for (const auto& bus : buses) {
                std::cout << bus << " ";
            }
            std::cout << "\n";
        }
        else {
            std::cout << "no buses\n";
        }
    }
    else {
        std::cout << "Stop " << stop_name << ": not found\n";
    }
}

} // namespace detail {

} // namespace transport
