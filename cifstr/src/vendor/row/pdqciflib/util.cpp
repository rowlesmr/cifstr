#include "util.hpp"



void row::util::Logger::log(Verbosity lev, const std::string_view message) const
{
	if (lev <= verbosity) {
		std::cout << message << '\n';
	}
}

row::util::Logger::~Logger()
{
	if (verbosity == EVERYTHING) {
		std::cout << " LOGGER destroyed" << std::endl;
	}
}

row::util::Logger::Logger(Verbosity lev) :verbosity{ lev }
{
	if (verbosity == EVERYTHING) {
		std::cout << "LOGGER set to: " << level_names[verbosity] << '\n';
	}
}

std::string& row::util::toLower_i(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str;
}

std::vector<std::string>& row::util::toLower_i(std::vector<std::string>& strs)
{
	for (std::string& str : strs) {
		toLower_i(str);
	}
	return strs;
}

std::string row::util::toLower(std::string str)
{
	toLower_i(str);
	return str;
}

std::vector<std::string> row::util::toLower(std::vector<std::string> strs)
{
	toLower_i(strs);
	return strs;
}

std::pair<double, double> row::util::stode(const char* s, const size_t len)
{
	double v{ 0.0 }; //value
	double e{ 0.0 }; //the error in the value

	int sgn{ 1 }; // what is sign of the double?
	int p{ 0 }; // what is the effective power for the value and error terms?
	char c{ *s };
	bool hasDigits{ false };

	//this is the address of the start of the const char*. I need it as a long, so I can
	// do the same when I get to the end. If the difference is the length of the string, 
	// then I know I've converted the entire string, and it is valid.
	// I need to cast to void* to get around interpreting the char* as a string. I then reinterpret 
	// the address as a long, so I can do maths with it.
	unsigned long long beginning{ reinterpret_cast<unsigned long long>(static_cast<const void*>(s)) };
	size_t extraChar{ 0 };

	//get the sign of the double
	if (c == '-') {
		sgn = -1;
		++s;
	}
	else if (c == '+') {
		++s;
	}
	//get the digits before the decimal point
	while ((c = *s++) != '\0' && std::isdigit(c)) {
		v = v * 10.0 + (c - '0');
		hasDigits = true;
	}
	//get the digits after the decimal point
	if (c == '.' || c == '?') {
		if (len == 1) { // then it is a value that represent missing or unknown
			return std::pair<double, double>({ NaN, 0 });
		}
		while ((c = *s++) != '\0' && std::isdigit(c)) {
			v = v * 10.0 + (c - '0');
			p--;
			hasDigits = true;
		}
	}
	//get the digits that belong to the exponent
	if ((c == 'e' || c == 'E') && hasDigits) {
		int sign = 1;
		int m = 0;
		c = *s++;
		if (c == '+') {
			c = *s++;
		}
		else if (c == '-') {
			c = *s++;
			sign = -1;
		}
		while (isdigit(c)) {
			m = m * 10 + (c - '0');
			c = *s++;
		}
		p += sign * m;
	}
	// get the digits that belong to the error
	if (c == '(' && hasDigits) {
		while ((c = *s++) != '\0' && std::isdigit(c)) { //implicitly breaks out of loop on the trailing ')'
			e = e * 10.0 + (c - '0');
		}
		extraChar = 1;
	}
	//scale the value and error
	while (p > 0) {
		v *= 10.0;
		e *= 10.0;
		p--;
	}
	while (p < 0) {
		v *= 0.1;
		e *= 0.1;
		p++;
	}
	//apply the correct sign to the value
	v *= sgn;

	unsigned long long end{ reinterpret_cast<unsigned long long>(static_cast<const void*>(s)) };
	size_t used_len{ static_cast<size_t>((end - beginning) / sizeof(char)) };
	size_t numChars{ extraChar + used_len - 1 };

	if (numChars != len) {
		return std::pair<double, double>({ NaN, NaN });
	}

	return std::pair<double, double>({ v, e });;
}

std::pair<double, double> row::util::stode(const std::string& s)
{
	return stode(s.c_str(), s.size());
}

std::string& row::util::strip_brackets_i(std::string& s)
{
	size_t n{ s.find("(") };
	if (n != std::string::npos)
		s = s.substr(0, n);
	return s;
}

std::vector<std::string>& row::util::strip_brackets_i(std::vector<std::string>& v)
{
	std::for_each(v.begin(), v.end(), [](std::string& s) { strip_brackets_i(s); });
	return v;
}

std::string row::util::strip_brackets(std::string s)
{
	strip_brackets_i(s);
	return s;
}

std::vector<std::string> row::util::strip_brackets(std::vector<std::string> v)
{
	strip_brackets_i(v);
	return v;
}

bool row::util::are_equal(double q, double w)
{
	return std::fabs(q - w) < 0.00000001;
}

bool row::util::all_equal(std::initializer_list<double> list)
{
	auto it = list.begin();
	return std::all_of(++it, list.end(), [&list](double val) { return are_equal(*list.begin(), val); });
}

std::string& row::util::pad_right_i(std::string& s, size_t len)
{
	s = std::format("{0:{1}}", s, len);
	return s;
}

std::string& row::util::pad_left_i(std::string& s, size_t len)
{
	s = std::format("{0:>{1}}", s, len);
	return s;
}

std::vector<std::string>& row::util::pad_column_i(std::vector<std::string>& v)
{
	if (std::any_of(v.begin(), v.end(), [](const std::string& s) { return s[0] == '-'; })) {
		std::for_each(v.begin(), v.end(), [](std::string& s) { if (s[0] != '-') pad_left_i(s, s.size() + 1); });
	}
	size_t max_len{ 0 };
	std::for_each(v.begin(), v.end(), [&max_len](const std::string& s) { if (s.size() > max_len) max_len = s.size(); });
	std::for_each(v.begin(), v.end(), [&max_len](std::string& s) { pad_right_i(s, max_len); });
	return v;
}

std::vector<std::string> row::util::pad_column(std::vector<std::string> v)
{
	pad_column_i(v);
	return v;
}

std::string& row::util::trim_i(std::string& s)
{
	s.erase(s.find_last_not_of(' ') + 1);
	s.erase(0, s.find_first_not_of(' '));
	return s;
}

std::string row::util::trim(std::string s)
{
	trim_i(s);
	return s;
}

std::string& row::util::replace_i(std::string& s, const char match, const char replace)
{
	std::replace(s.begin(), s.end(), match, replace);
	return s;
}

std::string row::util::replace(std::string s, const char match, const char replace)
{
	replace_i(s, match, replace);
	return s;
}
