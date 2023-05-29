#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>

using namespace std;
using filesystem::path;

path operator""_p(const char* data, std::size_t sz) {
    return path(data, data + sz);
}

void PrintTree(ostream& dst, const path& p, const filesystem::file_status& status, int offset) {
    dst << string(offset, ' ') << p.filename().string() << endl;
    if (status.type() == filesystem::file_type::directory) {
        vector<filesystem::directory_entry> objects;
        for (const auto& dir_entry : filesystem::directory_iterator(p)) {
            objects.push_back(dir_entry);
        }
        sort(objects.begin(), objects.end(), [](const auto& lhs, const auto& rhs) {
                return lhs.path().filename() > rhs.path().filename();
            });
        for (const auto& dir_entry : objects) {
            PrintTree(dst, dir_entry.path(), dir_entry.status(), offset + 2);
        }
    }
}

void PrintTree(ostream& dst, const path& p) {
    error_code err;
    auto status = filesystem::status(p, err);

    if (err) {
        return;
    }

    PrintTree(dst, p, status, 0);
}

int main() {
    error_code err;
    filesystem::remove_all("test_dir", err);
    filesystem::create_directories("test_dir"_p / "a"_p, err);
    filesystem::create_directories("test_dir"_p / "b"_p, err);

    ofstream("test_dir"_p / "b"_p / "f1.txt"_p);
    ofstream("test_dir"_p / "b"_p / "f2.txt"_p);
    ofstream("test_dir"_p / "a"_p / "f3.txt"_p);

    ostringstream out;
    PrintTree(out, "test_dir"_p);
    assert(out.str() ==
        "test_dir\n"
        "  b\n"
        "    f2.txt\n"
        "    f1.txt\n"
        "  a\n"
        "    f3.txt\n"sv
    );
}
