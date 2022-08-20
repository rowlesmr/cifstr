

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

	template<typename C, typename F>
	bool contains(const C& c, const F& f);

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

	std::pair<double, double> stode(const char* s, const size_t len);
	std::pair<double, double> stode(const std::string& s);

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
	return static_cast<int>(it - v.cbegin());
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

#endif