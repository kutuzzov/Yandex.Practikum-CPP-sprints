#include "stat_reader.h"
#include "input_reader.h"

int main() {
    TransportCatalogue catalogue;
    FillTransportCatalogue(std::cin, catalogue);
    ProcessRequests(std::cout, catalogue);
}
