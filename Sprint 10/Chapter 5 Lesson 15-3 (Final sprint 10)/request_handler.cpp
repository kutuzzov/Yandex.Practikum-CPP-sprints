/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

#include "request_handler.h"

void RequestHandler::ProcessRequests(const json::Node& stat_requests) const {
    json::Array result;
    for (auto& request : stat_requests.AsArray()) {
        const auto& request_map = request.AsMap();
        const auto& type = request_map.at("type").AsString();
        if (type == "Stop") result.push_back(PrintStop(request_map).AsMap());
        if (type == "Bus") result.push_back(PrintRoute(request_map).AsMap());
        if (type == "Map") result.push_back(PrintMap(request_map).AsMap());
    }
    
    json::Print(json::Document{ result }, std::cout);
}

const json::Node RequestHandler::PrintRoute(const json::Dict& request_map) const {
    json::Dict result;
    const std::string& route_number = request_map.at("name").AsString();
    result["request_id"] = request_map.at("id").AsInt();
    if (!catalogue_.FindRoute(route_number)) {
        result["error_message"] = json::Node{ static_cast<std::string>("not found") };
    }
    else {
        result["curvature"] = GetBusStat(route_number)->curvature;
        result["route_length"] = GetBusStat(route_number)->route_length;
        result["stop_count"] = static_cast<int>(GetBusStat(route_number)->stops_count);
        result["unique_stop_count"] = static_cast<int>(GetBusStat(route_number)->unique_stops_count);
    }
    
    return json::Node{ result };
}

const json::Node RequestHandler::PrintStop(const json::Dict& request_map) const {
    json::Dict result;
    const std::string& stop_name = request_map.at("name").AsString();
    result["request_id"] = request_map.at("id").AsInt();
    if (!catalogue_.FindStop(stop_name)) {
        result["error_message"] = json::Node{ static_cast<std::string>("not found") };
    }
    else {
        json::Array buses;
        for (auto& bus : GetBusesByStop(stop_name)) {
            buses.push_back(bus);
        }
        result["buses"] = buses;
    }

    return json::Node{ result };
}

const json::Node RequestHandler::PrintMap(const json::Dict& request_map) const {
    json::Dict result;
    result["request_id"] = request_map.at("id").AsInt();
    std::ostringstream strm;
    svg::Document map = RenderMap();
    map.Render(strm);
    result["map"] = strm.str();
    
    return json::Node{ result };
}

std::optional<transport::BusStat> RequestHandler::GetBusStat(const std::string_view bus_number) const {
    transport::BusStat bus_stat{};
    const transport::Bus* bus = catalogue_.FindRoute(bus_number);

    if (!bus) throw std::invalid_argument("bus not found");
    if (bus->is_circle) bus_stat.stops_count = bus->stops.size();
    else bus_stat.stops_count = bus->stops.size() * 2 - 1;

    int route_length = 0;
    double geographic_length = 0.0;

    for (size_t i = 0; i < bus->stops.size() - 1; ++i) {
        auto from = bus->stops[i];
        auto to = bus->stops[i + 1];
        if (bus->is_circle) {
            route_length += catalogue_.GetDistance(from, to);
            geographic_length += geo::ComputeDistance(from->coordinates,
                to->coordinates);
        }
        else {
            route_length += catalogue_.GetDistance(from, to) + catalogue_.GetDistance(to, from);
            geographic_length += geo::ComputeDistance(from->coordinates,
                to->coordinates) * 2;
        }
    }

    bus_stat.unique_stops_count = catalogue_.UniqueStopsCount(bus_number);
    bus_stat.route_length = route_length;
    bus_stat.curvature = route_length / geographic_length;

    return bus_stat;
}

const std::set<std::string> RequestHandler::GetBusesByStop(std::string_view stop_name) const {
    return catalogue_.FindStop(stop_name)->buses_by_stop;
}

svg::Document RequestHandler::RenderMap() const {
    return renderer_.GetSVG(catalogue_.GetSortedAllBuses());
}
