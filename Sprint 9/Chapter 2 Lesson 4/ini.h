#include <iostream>
#include <string>
#include <unordered_map>

namespace ini {

using Section = std::unordered_map<std::string, std::string>;

class Document {
public:
    Section& AddSection(std::string name);
    const Section& GetSection(const std::string& name) const;
    std::size_t GetSectionCount() const;

private:
    std::unordered_map<std::string, Section> sections_;
};

void ClearString(std::string& str);

Document Load(std::istream& input);

} //namespace ini
