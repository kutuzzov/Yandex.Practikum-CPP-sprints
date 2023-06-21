#pragma once
#include "date.h"

// напишите в этом классе код, ответственный за чтение запросов
class Parser {
public:
	virtual void ParseQuery(std::string_view query) = 0;

	void SetDateFrom(const Date from);
	void SetDateTo(const Date to);
	void SetEarnings(const double earnings);

	const Date GetDateFrom() const;
	const Date GetDateTo() const;
	double GetEarnings() const;

private:
	Date from_;
	Date to_;
	double earnings_ = 0.0;
};

class Parser_read : public Parser {
public:
	Parser_read(std::string_view query) {
		ParseQuery(query);
	}
	void ParseQuery(std::string_view query) override;
};

class Parser_modify : public Parser {
public:
	Parser_modify(std::string_view query) {
		ParseQuery(query);
	}
	void ParseQuery(std::string_view query) override;
};