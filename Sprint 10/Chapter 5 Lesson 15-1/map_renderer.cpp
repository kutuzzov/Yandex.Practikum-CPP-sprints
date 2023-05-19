/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршрутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */

#include "map_renderer.h"

namespace renderer {

bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}

std::vector<svg::Polyline> MapRenderer::GetRouteLines(const std::map<std::string_view, const transport::Bus*>& buses, const SphereProjector& sp) const {
    std::vector<svg::Polyline> result;
    size_t color_num = 0;
    for (const auto& [bus_number, bus] : buses) {
        if (bus->stops.empty()) continue;
        std::vector<const transport::Stop*> route_stops{ bus->stops.begin(), bus->stops.end() };
        if (bus->is_circle == false) route_stops.insert(route_stops.end(), std::next(bus->stops.rbegin()), bus->stops.rend());
        svg::Polyline line;
        for (const auto& stop : route_stops) {
            line.AddPoint(sp(stop->coordinates));
        }
        line.SetStrokeColor(render_settings_.color_palette[color_num]);
        line.SetFillColor("none");
        line.SetStrokeWidth(render_settings_.line_width);
        line.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        line.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        
        if (color_num < (render_settings_.color_palette.size() - 1)) ++color_num;
        else color_num = 0;
        
        result.push_back(line);
    }
    
    return result;
}

svg::Document MapRenderer::GetSVG(const std::map<std::string_view, const transport::Bus*>& buses) const {
    svg::Document result;
    std::vector<geo::Coordinates> route_stops_coord;
    for (const auto& [bus_number, bus] : buses) {
        for (const auto& stop : bus->stops) {
            route_stops_coord.push_back(stop->coordinates);
        }
    }
    SphereProjector sp(route_stops_coord.begin(), route_stops_coord.end(), render_settings_.width, render_settings_.height, render_settings_.padding);
    for (const auto& line : GetRouteLines(buses, sp)) {
        result.Add(line);
    }

    return result;
}

} // namespace renderer
