#include "runtime.h"
#include "test_runner_p.h"

using namespace std;

namespace runtime {
void RunObjectHolderTests(TestRunner& tr);
void RunObjectsTests(TestRunner& tr);
}  // namespace runtime

namespace {

void TestAll() {
    TestRunner tr;
    runtime::RunObjectHolderTests(tr);
    runtime::RunObjectsTests(tr);
}

}  // namespace

int main() {
    TestAll();

    return 0;
}
