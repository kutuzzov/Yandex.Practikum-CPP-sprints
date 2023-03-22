#include "input_reader.h"
#include "stat_reader.h"

int main() {
    TransportCatalogue catalogue;
    FillTransportCatalogue(catalogue);
    ProcessRequests(catalogue);
}
