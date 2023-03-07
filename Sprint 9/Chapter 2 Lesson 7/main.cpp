#include "libstat.h"

#include <iostream>

using namespace statistics::tests;

int main() {
    AggregSum();
    AggregMax();
    AggregMean();
    AggregStandardDeviation();
    AggregMode();
    AggregPrinter();

    std::cout << "Test passed!"sv << std::endl;
}
