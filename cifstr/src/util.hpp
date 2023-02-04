#pragma once


inline bool is_close(const double a, const double b, const double rel_tol = 1e-9, const double abs_tol = 0.0)
{
	return std::abs(x - y) <= abs_tol * std::max(1.0, rel_tol / abs_tol * std::max(std::abs(x), std::abs(y)));
}
