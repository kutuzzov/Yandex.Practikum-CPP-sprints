#include "json_reader.h"
#include "request_handler.h"

int main() {
    transport::Catalogue catalogue;
    JsonReader json_doc(std::cin);

    json_doc.FillCatalogue(catalogue);

    const auto& stat_requests = json_doc.GetStatRequests();
    const auto& render_settings = json_doc.GetRenderSettings();
    const auto& renderer = json_doc.FillRenderSettings(render_settings);
    const auto& routing_settings = json_doc.FillRoutingSettings(json_doc.GetRoutingSettings());
    const transport::Router router = { routing_settings, catalogue };

    RequestHandler rh(catalogue, renderer, router);
    json_doc.ProcessRequests(stat_requests, rh);
}
