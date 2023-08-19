#include "saveload.h"

#include <cassert>
#include <map>
#include <sstream>
#include <string>

using namespace std;

void TestSaveLoad() {
    map<uint32_t, string> m = {
        {1, "hello"},
        {2, "bye"},
    };
    stringstream ss;
    Serialize(m, ss);
    auto s = ss.str();

    assert(s.size() == 40);

    // Размер map (8 байт, так как это size_t)
    assert(s[0] == 0x02);
    assert(s[1] == 0x00);
    assert(s[2] == 0x00);
    assert(s[3] == 0x00);
    assert(s[4] == 0x00);
    assert(s[5] == 0x00);
    assert(s[6] == 0x00);
    assert(s[7] == 0x00);

    // Ключ 1 (4 байта, так как это uint32_t)
    assert(s[8] == 0x01);
    assert(s[9] == 0x00);
    assert(s[10] == 0x00);
    assert(s[11] == 0x00);

    // Значение: сначала размер строки (size_t)
    assert(s[12] == 0x05);
    assert(s[13] == 0x00);
    assert(s[14] == 0x00);
    assert(s[15] == 0x00);
    assert(s[16] == 0x00);
    assert(s[17] == 0x00);
    assert(s[18] == 0x00);
    assert(s[19] == 0x00);

    // Теперь сама строка (ASCII-коды):
    assert(s[20] == 0x68);  // 'h'
    assert(s[21] == 0x65);  // 'e'
    assert(s[22] == 0x6c);  // 'l'
    assert(s[23] == 0x6c);  // 'l'
    assert(s[24] == 0x6f);  // 'o'

    // Ключ 2
    assert(s[25] == 0x02);
    assert(s[26] == 0x00);
    assert(s[27] == 0x00);
    assert(s[28] == 0x00);

    // Значение ключа 2: размер
    assert(s[29] == 0x03);
    assert(s[30] == 0x00);
    assert(s[31] == 0x00);
    assert(s[32] == 0x00);
    assert(s[33] == 0x00);
    assert(s[34] == 0x00);
    assert(s[35] == 0x00);
    assert(s[36] == 0x00);

    // Значение ключа 2: строка
    assert(s[37] == 0x62);  // 'b'
    assert(s[38] == 0x79);  // 'y'
    assert(s[39] == 0x65);  // 'e'

    map<uint32_t, string> m2;
    Deserialize(ss, m2);

    assert(m == m2);
}

int main() {
    TestSaveLoad();
}
