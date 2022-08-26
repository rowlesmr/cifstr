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
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
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




auto initialise_pow10_lookup = [] {
	const int max_pow_10{ 309 };
	std::array<double, max_pow_10> pows{};
	pows[0] = 1;
	for (int i{ 1 }; i < max_pow_10; ++i) {
		pows[i] = pows[i - 1] * 10.0;
	}
	return pows;
};

static constexpr std::array pow_10{ initialise_pow10_lookup() };



std::pair<double, double> row::util::stode(const char* p, const char* pend) //inclusive begin, exclusive end
{
	if ((*p == '?' || *p == '.') && pend - p == 1) {
		return std::make_pair(NaN, 0);
	}

	uint64_t vi{ 0 }; //value
	uint64_t ei{ 0 }; //the error in the value
	bool isNeg{ *p == '-' };
	int64_t exponent{ 0 }; // what is the effective power for the value and error terms?

	bool hasDigits{ false };

	//get the sign of the double
	if (*p == '-' || *p == '+') {
		++p;
	}
	//get the digits before the decimal point
	while (p != pend && std::isdigit(*p)) {
		vi = vi * 10 + uint64_t(*p - '0');
		hasDigits = true;
		++p;
	}
	//get the digits after the decimal point
	if (*p == '.') {
		++p;
		while (p != pend && std::isdigit(*p)) {
			vi = vi * 10 + uint64_t(*p - '0');
			exponent--;
			hasDigits = true;
			++p;
		}
	}
	//get the digits that belong to the exponent
	if ((*p == 'e' || *p == 'E') && hasDigits) {
		++p;
		bool sign = (*p == '-');
		int64_t m{ 0 };
		if (*p == '-' || *p == '+') {
			++p;
		}
		while (p != pend && std::isdigit(*p)) {
			m = m * 10 + uint64_t(*p - '0');
			++p;
		}
		m = sign ? -m : m;
		exponent += m;
	}
	// get the digits that belong to the error
	if (*p == '(' && hasDigits) {
		++p;
		while (p != pend && std::isdigit(*p)) { 
			ei = ei * 10 + uint64_t(*p - '0');
			++p;
		}
		++p; //special case to get over the final bracket
	}
	//if everything is well-formed, we should now be at the end of the string.
	if (p != pend) {
		return std::make_pair(NaN, NaN);
	}

	double v, e;
	//scale the value and error
	if (exponent > 0) {
		v = static_cast<double>(vi) * pow_10[exponent];
		e = static_cast<double>(ei) * pow_10[exponent];
	}
	else if (exponent < 0) {
		v = static_cast<double>(vi) / pow_10[-exponent];
		e = static_cast<double>(ei) / pow_10[-exponent];
	}
	else
	{
		v = static_cast<double>(vi);
		e = static_cast<double>(ei);
	}

	//apply the correct sign to the value
	v = isNeg ? -v : v;

	return std::make_pair(v, e);
}

std::pair<double, double> row::util::stode(const std::string& s)
{
	return stode(s.c_str(), s.c_str() + s.size());
}
std::pair<double, double> row::util::stode(const std::string_view s)
{
	return stode(s.data(), s.data() + s.size());
}
std::string& row::util::strip_brackets_i(std::string& s)
{
	size_t n{ s.find('(') };
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

bool row::util::icompare(const std::string_view sva, const std::string_view svb)
{
	if (sva.size() == svb.size()) {
		return std::equal(svb.begin(), svb.end(), sva.begin(), [](unsigned char b, unsigned char a) { return std::tolower(a) == std::tolower(b); });
	}
	return false;
}








// below this is gratuitously copied from the fast_float library
bool row::util::is_integer(char c)  noexcept { return c >= '0' && c <= '9'; }

row::util::parsed_number_string row::util::parse_number_string(const char* p, const char* pend)
{
	parsed_number_string answer;

	if ((*p == '?' || *p == '.') && pend - p == 1) {
		return answer;
	}

	answer.negative = (*p == '-');

	if (*p == '-' || *p == '+') {
		++p;
		if (p == pend) {
			return answer;
		}
		if (!is_integer(*p) && (*p != '.')) { // a sign must be followed by an integer or the dot
			return answer;
		}
	}
	const char* const start_digits = p;

	uint64_t vi{ 0 }; // an unsigned int avoids signed overflows (which are bad) "value integer"
	while ((p != pend) && is_integer(*p)) {  //this is getting the digits before the decimal point
		vi = 10 * vi + uint64_t(*p - '0'); // might overflow
		++p;
	}
	const char* const end_of_integer_part = p;
	int64_t digit_count = int64_t(end_of_integer_part - start_digits);
	answer.integer = byte_span(start_digits, size_t(digit_count));
	
	int64_t exponent{ 0 };	
	if ((p != pend) && (*p == '.')) { //this is getting the digits after the decimal point
		++p;
		const char* before = p;

		while ((p != pend) && is_integer(*p)) {
			uint8_t digit = uint8_t(*p - '0');
			vi = vi * 10 + digit; // might overflow
			++p;
		}

		exponent = before - p;
		answer.fraction = byte_span(before, size_t(p - before));
		digit_count -= exponent;
	}
	// we must have encountered at least one integer!
	if (digit_count == 0) {
		return answer;
	}

	int64_t exp_number{ 0 };            // explicit exponential part
	if ((p != pend) && ((*p == 'e') || (*p == 'E'))) {
		const char* location_of_e{ p };
		++p;
		bool neg_exp{ false };
		if ((p != pend) && (*p=='-')) {
			neg_exp = true;
			++p;
		}
		else if ((p != pend) && (*p=='+')) { 
			++p;
		}

		if ((p == pend) || !is_integer(*p)) {
			p = location_of_e;
		}
		else {
			while ((p != pend) && is_integer(*p)) {
				uint8_t digit = uint8_t(*p - '0');
				if (exp_number < 0x10000000) {
					exp_number = 10 * exp_number + digit;
				}
				++p;
			}
			if (neg_exp) { exp_number = -exp_number; }
			exponent += exp_number;
		}
	}

	int64_t ei{ 0 };            // explicit uncertainty term: 1.234e-04(3) <- the bit in brackets. It cannot contain a sign, decimal point, or explicit exponent   ei == "error integer"
	if ((p != pend) && (*p == '(')) {
		++p;
		const char* before = p;
		while ((p != pend) && is_integer(*p)) {
			ei = ei * 10 + uint64_t(*p - '0'); // might overflow
			++p;
		}
		if(*p == ')'){
			answer.errint = byte_span(before, size_t(p - before));
			++p; //to point past the bracket
		}
		else{
			return answer;
		}	
	}

	answer.lastmatch = p;
	answer.valid = true;
	answer.exponent = exponent;
	answer.mantissa = vi;
	answer.error_mantissa = ei;
	return answer;
}

