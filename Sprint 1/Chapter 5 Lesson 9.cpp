#include <iostream>

using namespace std;

class SearchServer {
public:
    // перенесите сюда DocumentStatus
    enum class DocumentStatus {
        ACTUAL,
        IRRELEVANT,
        BANNED,
        REMOVED,
    };
};

int main() {
    // выведите численное значение статуса BANNED
    cout << static_cast<int>(SearchServer::DocumentStatus::BANNED) << endl;
}
