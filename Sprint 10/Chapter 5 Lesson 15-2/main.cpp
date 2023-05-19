/*
 * Примерная структура программы:
 *
 * Считать JSON из stdin
 * Построить на его основе JSON базу данных транспортного справочника
 * Выполнить запросы к справочнику, находящиеся в массиве "stat_requests", построив JSON-массив
 * с ответами.
 * Вывести в stdout ответы в виде JSON
 */

#include "json_reader.h"
#include "request_handler.h"

int main() {
    transport::Catalogue catalogue;
    JsonReader json_doc(std::cin);
    
    json_doc.FillCatalogue(catalogue);
    
    //const auto& stat_requests = json_doc.GetStatRequests();
    const auto& render_settings = json_doc.GetRenderSettings().AsMap();
    const auto& renderer = json_doc.FillRenderSettings(render_settings);
    
    RequestHandler rh(catalogue, renderer);
    //rh.ProcessRequests(stat_requests);
    rh.RenderMap().Render(std::cout);
}
