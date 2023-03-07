#include "ini.h"

using namespace std;

namespace ini {

Section& Document::AddSection(string name) {
	return sections_[name];
}

const Section& Document::GetSection(const string& name) const {
	static Section empty = {};
	if (sections_.count(name) == 0 || sections_.at(name).empty()) return empty;
	else {
		return sections_.at(name);
	}
}

std::size_t Document::GetSectionCount() const {
	return sections_.size();
}

void ClearString(string& str) {
	if (!str.empty()) str = str.substr(str.find_first_not_of(' '), str.find_last_not_of(' ') - str.find_first_not_of(' ') + 1);
}

Document Load(istream& input) {
	Document result;
	string line;
	Section* section = nullptr;

	while (getline(input, line)) {
		ClearString(line);

		if (line[0] == '[') {
			string section_name = line.substr(1, line.find(']') - 1);
			section = &result.AddSection(section_name);
		}
		else if (line.find('=') && !line.empty()) {
			string section_key = line.substr(0, line.find_first_of('='));
			string section_value = line.substr(line.find_first_of('=') + 1, line.size() - line.find_first_of('='));
			ClearString(section_key);
			ClearString(section_value);
			section->insert({ section_key, section_value });
		}
		else {
			continue;
		}
	}

	return result;
}

} // namespace ini
