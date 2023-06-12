#include <iostream>
#include <iomanip>
#include <algorithm>
#include <set>

using namespace std;

class ReadingManager {
public:
    ReadingManager()
        : people_(MAX_USER_COUNT_ + 1, -1)
        , pages_(MAX_PAGES_, 0)
    {}
    
    void Read(int user_id, int page_num) {
        for (int i = people_[user_id] + 1; i <= page_num; ++i) {
            ++pages_[i];
        }
        people_[user_id] = page_num;
    }
    
    double Cheer(int user_id) {
        if (people_[user_id] == -1) return 0;
        if (pages_[0] == 1) return 1;
        return (pages_[0] - pages_[people_[user_id]]) * 1.0 / (pages_[0] - 1) * 1.0;
    }
    
private:
    static const int MAX_USER_COUNT_ = 100000;
    static const int MAX_PAGES_ = 1000;
    
    vector<int> people_;
    vector<int> pages_;
};

int main() {
    ReadingManager manager;
    int query_count;
    cin >> query_count;
    
    for (size_t i = 0; i < query_count; ++i) {
        string query;
        int user_id;
        cin >> query >> user_id;
        
        if (query == "READ") {
            int page_num;
            cin >> page_num;
            manager.Read(user_id, page_num);
        }
        if (query == "CHEER") {
            cout << setprecision(6) << manager.Cheer(user_id) << "\n";
        }
    }
    
    return 0;
}
