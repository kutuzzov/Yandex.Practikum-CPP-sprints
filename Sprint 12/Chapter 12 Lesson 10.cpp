#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>

using namespace std;

class Domain {
public:
    Domain(const string& line)
        : domain_(line) {
        reverse(domain_.begin(), domain_.end());
        domain_.push_back('.');
    }

    bool operator==(const Domain& other) {
        return domain_ == other.domain_;
    }

    // разработайте метод IsSubdomain, принимающий другой домен и возвращающий true, если this его поддомен
    bool IsSubdomain(const Domain& other) const {
        if (domain_.size() < other.domain_.size()) return false;
        auto res = std::mismatch(other.domain_.begin(), other.domain_.end(), domain_.begin());
        return res.first == other.domain_.end();
    }
    
    string GetDomain() const {
        return domain_;
    }

private:
    string domain_;
};

class DomainChecker {
public:
public:
    template<typename Iterator>
    DomainChecker(Iterator first, Iterator last)
        : forbidden_domains_(first, last) {
        sort(forbidden_domains_.begin(), forbidden_domains_.end(), [](const Domain& lhs, const Domain& rhs){
            return lhs.GetDomain() < rhs.GetDomain();
        });
        last = std::unique(forbidden_domains_.begin(), forbidden_domains_.end(), [](const Domain& lhs, const Domain& rhs){
            return rhs.IsSubdomain(lhs);
        });
        forbidden_domains_.erase(last, forbidden_domains_.end());
    }

    bool IsForbidden(const Domain& domain) const {
        auto it = upper_bound(forbidden_domains_.begin(), forbidden_domains_.end(), domain, [](const Domain& lhs, const Domain& rhs){
                    return lhs.GetDomain() < rhs.GetDomain();
                });
        if (it == forbidden_domains_.begin()) return false;
        else return domain.IsSubdomain(*prev(it));
    }

private:
    vector<Domain> forbidden_domains_;
};

// разработайте функцию ReadDomains, читающую заданное количество доменов из стандартного входа
template <typename Number>
vector<Domain> ReadDomains(istream& in, Number num) {
    vector<Domain> domains;
    domains.reserve(num);
    string domain;
    for (size_t i = 0; i < num; ++i) {
        getline(in, domain);
        domains.push_back(domain);
    }
    return domains;
}

template <typename Number>
Number ReadNumberOnLine(istream& input) {
    string line;
    getline(input, line);

    Number num;
    std::istringstream(line) >> num;

    return num;
}

int main() {
    const std::vector<Domain> forbidden_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

    const std::vector<Domain> test_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    for (const Domain& domain : test_domains) {
        cout << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << endl;
    }
}
