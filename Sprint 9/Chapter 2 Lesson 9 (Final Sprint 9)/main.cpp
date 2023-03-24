#include "input_reader.h"
#include "stat_reader.h"

using namespace transport;

int main() {
    Catalogue catalogue;
    FillCatalogue(catalogue);
    ProcessRequests(catalogue);
}
