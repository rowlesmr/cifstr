

#ifndef ROW_UTIL_HPP
#define ROW_UTIL_HPP

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <numbers>
#include <format>
#include <array>
#include <string_view>
#include <cctype>
#include <utility>


namespace row::util {

	class Logger
	{
	public:
		enum Verbosity { NONE, SOME, ALL, EVERYTHING };
		Verbosity verbosity{ ALL };

	private:
		static constexpr std::array<std::string_view, 4> level_names{ "NONE", "SOME", "ALL", "EVERYTHING" };

	public:
		Logger(Verbosity lev);
		Logger() = default;

		~Logger();

		void log(Verbosity lev, const std::string_view message) const;
	};

	std::string& toLower_i(std::string& str);
	std::string toLower(std::string str);
	std::vector<std::string>& toLower_i(std::vector<std::string>& strs);
	std::vector<std::string> toLower(std::vector<std::string> strs);

	bool icompare(const std::string_view sva, const std::string_view svb);

	template<typename C, typename F>
	bool contains(const C& c, const F& f);

	template<typename C, typename F>
	bool icontains(const C& c, const F& f);
		
	template <typename T>
	void makeInRange(T& val, const T& minVal, const T& maxVal);

	//https://stackoverflow.com/a/57399634/36061
	template <typename T> 
	void move_element(std::vector<T>& v, size_t oldIndex, size_t newIndex);

	template<typename T>
	int getIndexOf(const std::vector<T>& v, const T& f);

	constexpr double NaN{ std::numeric_limits<double>::quiet_NaN() };
	constexpr double deg{ std::numbers::pi / 180.0 };
	constexpr double rad{ 1.0 / deg };



	//std::pair<double, double> row::util::stode(const char* first, const char* last);
	template <typename T>
	struct span {
		const T* ptr;
		size_t length;
		span(const T* _ptr, size_t _length) : ptr(_ptr), length(_length) {}
		span() : ptr(nullptr), length(0) {}

		constexpr size_t len() const noexcept {
			return length;
		}

		const T& operator[](size_t index) const noexcept {
			//FASTFLOAT_DEBUG_ASSERT(index < length);
			return ptr[index];
		}
	};

	typedef span<const char> byte_span;
	
	struct parsed_number_string {
		int64_t exponent{ 0 };
		uint64_t mantissa{ 0 };
		uint64_t error_mantissa{ 0 };
		const char* lastmatch{ nullptr };
		bool negative{ false };
		bool valid{ false };
		bool too_many_digits{ false };
		// contains the range of the significant digits
		byte_span integer{};  // non-nullable
		byte_span fraction{}; // nullable
		byte_span errint{};
	};

	
	bool is_integer(char c) noexcept;
	parsed_number_string parse_number_string(const char* p, const char* pend);
	std::pair<double, double> stode(const char* p, const char* pend);
	std::pair<double, double> stode(const std::string& s);
	std::pair<double, double> stode(const std::string_view s);

	std::string& strip_brackets_i(std::string& s);
	std::string strip_brackets(std::string s);
	std::vector<std::string>& strip_brackets_i(std::vector<std::string>& v);
	std::vector<std::string> strip_brackets(std::vector<std::string> v);

	//for the magnitude of values I'm dealing with, this is fine.
	bool are_equal(double q, double w);
	bool all_equal(std::initializer_list<double> list);

	std::string& pad_right_i(std::string& s, size_t len);
	std::string& pad_left_i(std::string& s, size_t len);
	std::vector<std::string>& pad_column_i(std::vector<std::string>& v);
	std::vector<std::string> pad_column(std::vector<std::string> v);

	std::string& trim_i(std::string& s);
	std::string trim(std::string s);

	std::string& replace_i(std::string& s, const char match, const char replace);
	std::string replace(std::string s, const char match, const char replace);
}

template<typename T>
int row::util::getIndexOf(const std::vector<T>& v, const T& f)
{
	auto it = std::find(v.cbegin(), v.cend(), f);
	if (it == v.cend()) {
		return -1;
	}
	return static_cast<int>(std::distance(v.cbegin(), it));
}

template <typename T>
void row::util::move_element(std::vector<T>& v, size_t oldIndex, size_t newIndex)
{
	makeInRange<size_t>(newIndex, 0, v.size() - 1);
	if (oldIndex > newIndex)
		std::rotate(v.rend() - oldIndex - 1, v.rend() - oldIndex, v.rend() - newIndex);
	else
		std::rotate(v.begin() + oldIndex, v.begin() + oldIndex + 1, v.begin() + newIndex + 1);
}

template <typename T>
void row::util::makeInRange(T& val, const T& minVal, const T& maxVal)
{
	if (val > maxVal) {
		val = maxVal;
	}
	if (val < minVal) {
		val = minVal;
	}
}

template<typename C, typename F>
bool row::util::contains(const C& c, const F& f)
{
	return std::find(c.cbegin(), c.cend(), f) != c.cend();
}

template<typename C, typename F>
bool row::util::icontains(const C& c, const F& f)
{
	auto strincomp = [&f](const std::string_view sv) {
		return sv.size() == f.size() &&
		std::equal(sv.begin(), sv.end(), f.begin(),
			[](unsigned char b, unsigned char a) {
				return std::tolower(a) == std::tolower(b);
			}
		); 
	};
	return std::find_if(c.cbegin(), c.cend(), strincomp) != c.cend();
}

#endif