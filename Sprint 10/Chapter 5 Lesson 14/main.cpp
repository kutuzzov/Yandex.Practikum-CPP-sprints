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
    setlocale(LC_ALL, "Russian");

    transport::Catalogue catalogue;
    JsonReader requests(std::cin);
    requests.FillCatalogue(catalogue);
    RequestHandler(requests, catalogue);
}

/*
{
    "base_requests": [
    {
        "type": "Bus",
        "name": "114",
        "stops": ["Морской вокзал", "Ривьерский мост"],
        "is_roundtrip": false
    },
    {
        "type": "Stop",
        "name": "Ривьерский мост",
        "latitude": 43.587795,
        "longitude": 39.716901,
        "road_distances": {"Морской вокзал": 850}
    },
    {
        "type": "Stop",
        "name": "Морской вокзал",
        "latitude": 43.581969,
        "longitude": 39.719848,
        "road_distances": {"Ривьерский мост": 850}
    }
    ],
    "stat_requests": [
    { "id": 1, "type": "Stop", "name": "Ривьерский мост" },
    { "id": 2, "type": "Bus", "name": "114" }
    ]
}
--------------------------
Ожидаемый вывод:
[
   {
       "buses": [
           "114"
       ],
       "request_id": 1
   },
   {
       "curvature": 1.23199,
       "request_id": 2,
       "route_length": 1700,
       "stop_count": 3,
       "unique_stop_count": 2
   }
]
--------------------------
*/
