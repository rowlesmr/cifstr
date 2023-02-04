#pragma once
#include <numbers>
#include <algorithm>
#include <cmath>

inline static constexpr double deg{ 180.0 / std::numbers::pi };


inline bool is_close(const double a, const double b, const double rel_tol = 1e-9, const double abs_tol = 0.0)
{
	return std::abs(a - b) <= abs_tol * std::max(1.0, rel_tol / abs_tol * std::max(std::abs(a), std::abs(b)));
}

inline bool all_equal(const auto& container)
{
	return std::adjacent_find(container.begin(), container.end(), std::not_equal_to<>()) == container.end();
}




