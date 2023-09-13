#include "json_reader.h"
#include "json_builder.h"

using namespace std::literals;

const json::Node& JsonReader::GetBaseRequests() const {
    if (!input_.GetRoot().AsDict().count("base_requests"s)) return dummy_;
    return input_.GetRoot().AsDict().at("base_requests"s);
}

const json::Node& JsonReader::GetStatRequests() const {
    if (!input_.GetRoot().AsDict().count("stat_requests"s)) return dummy_;
    return input_.GetRoot().AsDict().at("stat_requests"s);
}

const json::Node& JsonReader::GetRenderSettings() const {
    if (!input_.GetRoot().AsDict().count("render_settings"s)) return dummy_;
    return input_.GetRoot().AsDict().at("render_settings"s);
}

const json::Node& JsonReader::GetRoutingSettings() const {
    if (!input_.GetRoot().AsDict().count("routing_settings"s)) return dummy_;
    return input_.GetRoot().AsDict().at("routing_settings"s);
}

const json::Node& JsonReader::GetSerializationSettings() const {
    if (!input_.GetRoot().AsDict().count("serialization_settings"s)) return dummy_;
    return input_.GetRoot().AsDict().at("serialization_settings"s);
}

void JsonReader::ProcessRequests(const json::Node& stat_requests, RequestHandler& rh) const {
    json::Array result;
    for (auto& request : stat_requests.AsArray()) {
        const auto& request_map = request.AsDict();
        const auto& type = request_map.at("type"s).AsString();
        if (type == "Stop"s) result.push_back(PrintStop(request_map, rh).AsDict());
        if (type == "Bus"s) result.push_back(PrintRoute(request_map, rh).AsDict());
        if (type == "Map"s) result.push_back(PrintMap(request_map, rh).AsDict());
        if (type == "Route"s) result.push_back(PrintRouting(request_map, rh).AsDict());
    }

    json::Print(json::Document{ result }, std::cout);
}

void JsonReader::FillCatalogue(transport::Catalogue& catalogue) {
    const json::Array& arr = GetBaseRequests().AsArray();
    for (auto& request_stops : arr) {
        const auto& request_stops_map = request_stops.AsDict();
        const auto& type = request_stops_map.at("type"s).AsString();
        if (type == "Stop"s) {
            auto [stop_name, coordinates, stop_distances] = FillStop(request_stops_map);
            catalogue.AddStop(stop_name, coordinates);
        }
    }
    FillStopDistances(catalogue);

    for (auto& request_bus : arr) {
        const auto& request_bus_map = request_bus.AsDict();
        const auto& type = request_bus_map.at("type"s).AsString();
        if (type == "Bus"s) {
            auto [bus_number, stops, circular_route] = FillRoute(request_bus_map, catalogue);
            catalogue.AddRoute(bus_number, stops, circular_route);
        }
    }
}

std::tuple<std::string_view, geo::Coordinates, std::map<std::string_view, int>> JsonReader::FillStop(const json::Dict& request_map) const {
    std::string_view stop_name = request_map.at("name"s).AsString();
    geo::Coordinates coordinates = { request_map.at("latitude"s).AsDouble(), request_map.at("longitude"s).AsDouble() };
    std::map<std::string_view, int> stop_distances;
    auto& distances = request_map.at("road_distances"s).AsDict();
    for (auto& [stop_name, dist] : distances) {
        stop_distances.emplace(stop_name, dist.AsInt());
    }
    return std::make_tuple(stop_name, coordinates, stop_distances);
}

void JsonReader::FillStopDistances(transport::Catalogue& catalogue) const {
    const json::Array& arr = GetBaseRequests().AsArray();
    for (auto& request_stops : arr) {
        const auto& request_stops_map = request_stops.AsDict();
        const auto& type = request_stops_map.at("type"s).AsString();
        if (type == "Stop"s) {
            auto [stop_name, coordinates, stop_distances] = FillStop(request_stops_map);
            for (auto& [to_name, dist] : stop_distances) {
                auto from = catalogue.FindStop(stop_name);
                auto to = catalogue.FindStop(to_name);
                catalogue.SetDistance(from, to, dist);
            }
        }
    }
}

std::tuple<std::string_view, std::vector<const transport::Stop*>, bool> JsonReader::FillRoute(const json::Dict& request_map, transport::Catalogue& catalogue) const {
    std::string_view bus_number = request_map.at("name"s).AsString();
    std::vector<const transport::Stop*> stops;
    for (auto& stop : request_map.at("stops"s).AsArray()) {
        stops.push_back(catalogue.FindStop(stop.AsString()));
    }
    bool circular_route = request_map.at("is_roundtrip"s).AsBool();

    return std::make_tuple(bus_number, stops, circular_route);
}

renderer::MapRenderer JsonReader::FillRenderSettings(const json::Node& settings) const {
    json::Dict request_map = settings.AsDict();
    renderer::RenderSettings render_settings;
    render_settings.width = request_map.at("width"s).AsDouble();
    render_settings.height = request_map.at("height"s).AsDouble();
    render_settings.padding = request_map.at("padding"s).AsDouble();
    render_settings.stop_radius = request_map.at("stop_radius"s).AsDouble();
    render_settings.line_width = request_map.at("line_width"s).AsDouble();
    render_settings.bus_label_font_size = request_map.at("bus_label_font_size"s).AsInt();
    const json::Array& bus_label_offset = request_map.at("bus_label_offset"s).AsArray();
    render_settings.bus_label_offset = { bus_label_offset[0].AsDouble(), bus_label_offset[1].AsDouble() };
    render_settings.stop_label_font_size = request_map.at("stop_label_font_size"s).AsInt();
    const json::Array& stop_label_offset = request_map.at("stop_label_offset"s).AsArray();
    render_settings.stop_label_offset = { stop_label_offset[0].AsDouble(), stop_label_offset[1].AsDouble() };

    if (request_map.at("underlayer_color"s).IsString()) render_settings.underlayer_color = request_map.at("underlayer_color"s).AsString();
    else if (request_map.at("underlayer_color"s).IsArray()) {
        const json::Array& underlayer_color = request_map.at("underlayer_color"s).AsArray();
        if (underlayer_color.size() == 3) {
            render_settings.underlayer_color = svg::Rgb(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt());
        }
        else if (underlayer_color.size() == 4) {
            render_settings.underlayer_color = svg::Rgba(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt(), underlayer_color[3].AsDouble());
        }
        else throw std::logic_error("wrong underlayer colortype"s);
    }
    else throw std::logic_error("wrong underlayer color"s);

    render_settings.underlayer_width = request_map.at("underlayer_width"s).AsDouble();

    const json::Array& color_palette = request_map.at("color_palette"s).AsArray();
    for (const auto& color_element : color_palette) {
        if (color_element.IsString()) render_settings.color_palette.push_back(color_element.AsString());
        else if (color_element.IsArray()) {
            const json::Array& color_type = color_element.AsArray();
            if (color_type.size() == 3) {
                render_settings.color_palette.push_back(svg::Rgb(color_type[0].AsInt(), color_type[1].AsInt(), color_type[2].AsInt()));
            }
            else if (color_type.size() == 4) {
                render_settings.color_palette.push_back(svg::Rgba(color_type[0].AsInt(), color_type[1].AsInt(), color_type[2].AsInt(), color_type[3].AsDouble()));
            }
            else throw std::logic_error("wrong color_palette type"s);
        }
        else throw std::logic_error("wrong color_palette"s);
    }

    return render_settings;
}

transport::Router JsonReader::FillRoutingSettings(const json::Node& settings) const {
    //transport::Router routing_settings;
    return transport::Router{ settings.AsDict().at("bus_wait_time"s).AsInt(), settings.AsDict().at("bus_velocity"s).AsDouble() };
}

const json::Node JsonReader::PrintRoute(const json::Dict& request_map, RequestHandler& rh) const {
    json::Node result;
    const std::string& route_number = request_map.at("name"s).AsString();
    const int id = request_map.at("id"s).AsInt();

    if (!rh.IsBusNumber(route_number)) {
        result = json::Builder{}
            .StartDict()
                .Key("request_id"s).Value(id)
                .Key("error_message"s).Value("not found"s)
            .EndDict()
        .Build();
    }
    else {
        const auto& route_info = rh.GetBusStat(route_number);
        result = json::Builder{}
            .StartDict()
                .Key("request_id"s).Value(id)
                .Key("curvature"s).Value(route_info->curvature)
                .Key("route_length"s).Value(route_info->route_length)
                .Key("stop_count"s).Value(static_cast<int>(route_info->stops_count))
                .Key("unique_stop_count"s).Value(static_cast<int>(route_info->unique_stops_count))
            .EndDict()
        .Build();
    }
    return result;
}

const json::Node JsonReader::PrintStop(const json::Dict& request_map, RequestHandler& rh) const {
    json::Node result;
    const std::string& stop_name = request_map.at("name"s).AsString();
    const int id = request_map.at("id"s).AsInt();

    if (!rh.IsStopName(stop_name)) {
        result = json::Builder{}
            .StartDict()
                .Key("request_id"s).Value(id)
                .Key("error_message"s).Value("not found"s)
            .EndDict()
        .Build();
    }
    else {
        json::Array buses;
        for (const auto& bus : rh.GetBusesByStop(stop_name)) {
            buses.push_back(bus);
        }
        result = json::Builder{}
            .StartDict()
                .Key("request_id"s).Value(id)
                .Key("buses"s).Value(buses)
            .EndDict()
        .Build();
    }
    return result;
}

const json::Node JsonReader::PrintMap(const json::Dict& request_map, RequestHandler& rh) const {
    json::Node result;
    const int id = request_map.at("id"s).AsInt();
    std::ostringstream strm;
    svg::Document map = rh.RenderMap();
    map.Render(strm);

    result = json::Builder{}
        .StartDict()
            .Key("request_id"s).Value(id)
            .Key("map"s).Value(strm.str())
        .EndDict()
    .Build();

    return result;
}

const json::Node JsonReader::PrintRouting(const json::Dict& request_map, RequestHandler& rh) const {
    json::Node result;
    const int id = request_map.at("id"s).AsInt();
    const std::string_view stop_from = request_map.at("from"s).AsString();
    const std::string_view stop_to = request_map.at("to"s).AsString();
    const auto& routing = rh.GetOptimalRoute(stop_from, stop_to);
    
    if (!routing) {
        result = json::Builder{}
            .StartDict()
                .Key("request_id"s).Value(id)
                .Key("error_message"s).Value("not found"s)
            .EndDict()
        .Build();
    }
    else {
        json::Array items;
        double total_time = 0.0;
        items.reserve(routing.value().edges.size());
        for (auto& edge_id : routing.value().edges) {
            const graph::Edge<double> edge = rh.GetRouterGraph().GetEdge(edge_id);
            if (edge.quality == 0) {
                items.emplace_back(json::Node(json::Builder{}
                    .StartDict()
                        .Key("stop_name"s).Value(edge.name)
                        .Key("time"s).Value(edge.weight)
                        .Key("type"s).Value("Wait"s)
                    .EndDict()
                .Build()));

                total_time += edge.weight;
            }
            else {
                items.emplace_back(json::Node(json::Builder{}
                    .StartDict()
                        .Key("bus"s).Value(edge.name)
                        .Key("span_count"s).Value(static_cast<int>(edge.quality))
                        .Key("time"s).Value(edge.weight)
                        .Key("type"s).Value("Bus"s)
                    .EndDict()
                .Build()));

                total_time += edge.weight;
            }
        }

        result = json::Builder{}
            .StartDict()
                .Key("request_id"s).Value(id)
                .Key("total_time"s).Value(total_time)
                .Key("items"s).Value(items)
            .EndDict()
        .Build();
    }

    return result;
}
