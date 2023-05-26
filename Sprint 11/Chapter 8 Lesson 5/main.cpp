#include "compressor.h"
#include "decompressor.h"

#include <cassert>
#include <fstream>
#include <string>
#include <string_view>

using namespace std;

// эта функция нужна для теста
string GetFileContents(string file) {
    ifstream stream(file, ios::binary);

    stream.seekg(0, ios::end);
    size_t sz = stream.tellg();

    stream.seekg(0, ios::beg);
    string res(sz, '\0');

    if (stream.read(res.data(), sz)) {
        return res;
    }

    return {};
}

void Test() {
    ofstream("test.txt") << "abcdaaaaaabbbcdedecdec"sv << string(1000, 'x')
                         << "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\xa0"
                            "decdecdecasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdassdasdassdas"
                            "dcdecasdasdasdasdasdasdasdasdasdasdasdasdasdasdasdassdasdassdasdcde"
                            "casdasdasdasdasdasdasdasdasdasdasdasdasdasdasdassdasdassdasdcdecasd"
                            "asdasdasdasdasdasdasdasdasdasdasdasdasdasdassdasdassdasddeccccccccc"
                            "cccc"sv;
    EncodeRLE("test.txt"s, "test.rle"s);
    DecodeRLE("test.rle"s, "test2.txt"s);

    assert(GetFileContents("test.rle"s).size() < GetFileContents("test.txt"s).size() / 2);
    assert(GetFileContents("test.txt"s) == GetFileContents("test2.txt"s));
}

int main(int argc, const char** argv) {
    if (argc == 1) {
        Test();
        return 0;
    }

    if (argc != 4 || (argv[1] != "x"sv && argv[1] != "c"sv)) {
        cout << "Usage: "sv << argv[0] << " c|x <in file> <out file>"sv << endl;
        return 1;
    }

    if (argv[1] == "c"sv) {
        auto result = EncodeRLE(argv[2], argv[3]);
        if (!result.opened) {
            cout << "Error opening file"sv << endl;
            return 2;
        }

        cout << "Compressed "sv << result.src_size << " bytes => "sv << result.dst_size << " bytes"sv << endl;
    } else {
        if (!DecodeRLE(argv[2], argv[3])) {
            cout << "Error decoding file"sv << endl;
            return 3;
        }

        cout << "Successfully decoded"sv << endl;
    }

    return 0;
}
