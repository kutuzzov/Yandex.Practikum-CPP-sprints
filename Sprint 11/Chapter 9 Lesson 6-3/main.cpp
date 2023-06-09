#include "json_reader.h"
#include "request_handler.h"

int main() {
#ifdef __APPLE__
    freopen("input.json", "r", stdin);
    freopen("output.json", "w", stdout);
    //freopen("error-output.txt", "w", stderr);
#endif
    
    transport::Catalogue catalogue;
    JsonReader json_doc(std::cin);
    
    json_doc.FillCatalogue(catalogue);
    
    const auto& stat_requests = json_doc.GetStatRequests();
    const auto& render_settings = json_doc.GetRenderSettings().AsDict();
    const auto& renderer = json_doc.FillRenderSettings(render_settings);

    RequestHandler rh(catalogue, renderer);
    json_doc.ProcessRequests(stat_requests, rh);
}
