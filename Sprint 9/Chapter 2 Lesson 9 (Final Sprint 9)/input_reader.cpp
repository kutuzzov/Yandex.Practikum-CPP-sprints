#include "input_reader.h"

namespace transport {

void FillCatalogue(std::istream& in, Catalogue& catalogue) {
    std::vector<std::string> query_bus;
    std::vector<std::string> query_stop;
    std::vector<std::string> query_stop_distances;
    size_t requests_count;

    in >> requests_count;
    for (size_t i = 0; i < requests_count; ++i) {
        std::string keyword, line;
        in >> keyword;
        std::getline(in, line);
        if (keyword == "Stop") {
            query_stop.push_back(line);
        }
        if (keyword == "Bus") {
            query_bus.push_back(line);
        }
    }
    query_stop_distances = query_stop;
    for (auto& stop : query_stop) {
        auto [stop_name, coordinates] = FillStop(stop);
        catalogue.AddStop(stop_name, coordinates);
    }
    for (auto& stop : query_stop_distances) {
        FillStopDistances(stop, catalogue);
    }
    for (auto& bus : query_bus) {
        auto [bus_number, stops, is_circle] = FillRoute(bus, catalogue);
        catalogue.AddRoute(bus_number, stops, is_circle);
        bus = {};
    }
}

std::pair<std::string, geo::Coordinates> FillStop(std::string& line) {
    std::string stop_name = line.substr(1, line.find_first_of(':') - line.find_first_of(' ') - 1);
    double lat = std::stod(line.substr(line.find_first_of(':') + 2, line.find_first_of(',') - 1));
    double lng;
    line.erase(0, line.find_first_of(',') + 2);
    if (line.find_last_of(',') == line.npos) {
        lng = std::stod(line.substr(0, line.npos - 1));
        line.clear();
    }
    else {
        lng = std::stod(line.substr(0, line.find_first_of(',')));
        line.erase(0, line.find_first_of(',') + 2);
    }
    geo::Coordinates stop_coordinates = { lat, lng };

    return { stop_name, stop_coordinates };
}

void FillStopDistances(std::string& line, Catalogue& catalogue) {
    if (!line.empty()) {
        std::string stop_from_name = FillStop(line).first;
        const Stop* from = catalogue.FindStop(stop_from_name);

        while (!line.empty()) {
            int distanse = 0;
            std::string stop_to_name;
            distanse = std::stoi(line.substr(0, line.find_first_of("m to ")));
            line.erase(0, line.find_first_of("m to ") + 5);
            if (line.find("m to ") == line.npos) {
                stop_to_name = line.substr(0, line.npos - 1);
                const Stop* to = catalogue.FindStop(stop_to_name);
                catalogue.SetDistance(from, to, distanse);
                if (!catalogue.GetDistance(from, to)) {
                    catalogue.SetDistance(to, from, distanse);
                }
                line.clear();
            }
            else {
                stop_to_name = line.substr(0, line.find_first_of(','));
                const Stop* to = catalogue.FindStop(stop_to_name);
                catalogue.SetDistance(from, to, distanse);
                if (!catalogue.GetDistance(from, to)) {
                    catalogue.SetDistance(to, from, distanse);
                }
                line.erase(0, line.find_first_of(',') + 2);
            }
        }
    }
}

std::tuple<std::string, std::vector<const Stop*>, bool> FillRoute(std::string& line, transport::Catalogue& catalogue) {
    std::vector<const Stop*> route_stops;
    std::string route_number = line.substr(1, line.find_first_of(':') - 1);
    line.erase(0, line.find_first_of(':') + 2);
    bool is_circle = false;
    std::string stop_name;
    auto pos = line.find('>') != line.npos ? '>' : '-';
    while (line.find(pos) != line.npos) {
        stop_name = line.substr(0, line.find_first_of(pos) - 1);
        const Stop* stop_name_pointer = catalogue.FindStop(stop_name);
        route_stops.push_back(stop_name_pointer);
        line.erase(0, line.find_first_of(pos) + 2);
    }
    stop_name = line.substr(0, line.npos - 1);
    const Stop* stop_name_pointer = catalogue.FindStop(stop_name);
    route_stops.push_back(stop_name_pointer);
    if (pos == '>') is_circle = true;

    return { route_number, route_stops, is_circle };
}

} // namespace transport
