#include <numbers>
#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <algorithm>
#include <sstream>
#include <format>

export module util;


export namespace row {

    template<typename T>
    void println(const T& t) {
        std::cout << t << '\n';
    }
    void println() {
        std::cout << '\n';
    }
    template<typename T>
    void print(const T& t) {
        std::cout << t << '\t';
    }

    template<typename T>
    void print(const std::vector<T>& v) {
        std::for_each(v.begin(), v.end(), [](const T& t) { print(t); });
    }

    template<typename item, typename collection>
    bool is_in(const item& i, const collection& c) {
        return std::any_of(c.begin(), c.end(), [&i](const item& val) { return val == i; });
    }

    constexpr double NaN{ std::numeric_limits<double>::quiet_NaN() };
    constexpr double deg{ std::numbers::pi / 180.0 };
    constexpr double rad{ 1.0 / deg };

    double stod(const char* s, const size_t len)
    {
        double v{ 0.0 }; //value

        int sgn = 1; // what is sign of the double?
        int p = 0; // what is the effective power for the value and error terms?
        int c = *s;

        //get the sign of the double
        if (c == '-') {
            sgn = -1;
            s++;
        }
        else if (c == '+') {
            s++;
        }
        //get the digits before the decimal point
        while ((c = *s++) != '\0' && std::isdigit(c)) {
            v = v * 10.0 + (c - '0');
        }
        //get the digits after the decimal point. Also checks if the string represent a missing or unknown value
        if (c == '.' || c == '?') {
            if (len == 1) {
                v = NaN;
                return false;
            }
            while ((c = *s++) != '\0' && std::isdigit(c)) {
                v = v * 10.0 + (c - '0');
                p--;
            }
        }
        //get the digits that belong to the exponent
        if (c == 'e' || c == 'E') {
            int sign = 1;
            int m = 0;
            c = *s++;
            if (c == '+')
                c = *s++;
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
        ////scale the value and error
        while (p > 0) {
            v *= 10.0;
            p--;
        }
        while (p < 0) {
            v *= 0.1;
            p++;
        }
        //apply the correct sign to the value
        v *= sgn;

        return v;
    }

    double stod(const char* s) {
        return stod(s, strlen(s));
    }

    double str_to_v(const std::string& s)
    {
        return stod(s.c_str(), s.size());
    }

    double str_to_v(const char* s)
    {
        return stod(s, strlen(s));
    }

    std::vector<double> str_to_v(const std::vector<std::string>& v)
    {
        std::vector<double> d;
        d.reserve(v.size());
        std::transform(v.begin(), v.end(), std::back_inserter(d), [](const std::string& s) { return str_to_v(s); });
        return d;
    }

    std::string v_to_str(const double d, const int dp = 3) {
        return std::format("{0:.{1}f}", d, dp);
    }

    std::vector<std::string> v_to_str(const std::vector<double>& d, const int dp = 3) {
        std::vector<std::string> v;
        v.reserve(d.size());
        std::transform(d.begin(), d.end(), std::back_inserter(v), [dp](const double d) { if (!std::isnan(d)) [[likely]] { return v_to_str(d, dp); } else { return std::string{"."}; } });
        return v;
    }

    std::string& strip_brackets_i(std::string& s) {
        size_t n{ s.find("(") };
        if (n != std::string::npos)
            s = s.substr(0, n);
        return s;
    }

    std::string strip_brackets(std::string s) {
        strip_brackets_i(s);
        return s;
    }

    std::vector<std::string>& strip_brackets_i(std::vector<std::string>& v) {
        std::for_each(v.begin(), v.end(), [](std::string& s) { strip_brackets_i(s); });
        return v;
    }

    std::vector<std::string> strip_brackets(std::vector<std::string> v) {
        strip_brackets_i(v);
        return v;
    }

    //for the magnitude of values I'm dealing with, this is fine.
    bool are_equal(double q, double w) {
        return std::fabs(q - w) < 0.0000001;
    }

    void pad_right_i(std::string& s, size_t len)
    {
        if (s.size() < len)
            s = s + std::string(len - s.size(), ' ');
        return;
    }

    void pad_left_i(std::string& s, size_t len)
    {
        if (s.size() < len)
            s = std::string(len - s.size(), ' ') + s;
        return;
    }

    std::vector<std::string>& pad_column_i(std::vector<std::string>& v) {
        if (std::any_of(v.begin(), v.end(), [](const std::string& s) { return s[0] == '-'; })) {
            std::for_each(v.begin(), v.end(), [](std::string& s) { if (s[0] != '-') pad_left_i(s, s.size() + 1); });
        }
        size_t max_len{ 0 };
        std::for_each(v.begin(), v.end(), [&max_len](const std::string& s) { if (s.size() > max_len) max_len = s.size(); });
        std::for_each(v.begin(), v.end(), [&max_len](std::string& s) { pad_right_i(s, max_len); });
        return v;
    }

    std::vector<std::string> pad_column(std::vector<std::string> v) {
        pad_column_i(v);
        return v;
    }

    std::string& trim_i(std::string& s) {
        s.erase(s.find_last_not_of(' ') + 1);
        s.erase(0, s.find_first_not_of(' '));
        return s;
    }

    std::string trim(std::string s) {
        trim_i(s);
        return s;
    }

    std::string& replace_i(std::string& s, const char match, const char replace) {
        std::replace(s.begin(), s.end(), match, replace);
        return s;
    }

    std::string replace(std::string s, const char match, const char replace) {
        replace_i(s, match, replace);
        return s;
    }
}