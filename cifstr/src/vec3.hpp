#pragma once

#include <iostream>

class Vec3
{
public:
	double x{ 0.0 };
	double y{ 0.0 };
	double z{ 0.0 };

	[[nodiscard]] Vec3(double u = 0.0, double v = 0.0, double w = 0.0);

	[[nodiscard]] Vec3 cross_product(const Vec3& other) const;
	[[nodiscard]] Vec3 cross(const Vec3& other) const;
	[[nodiscard]] double dot_product(const Vec3& other) const;
	[[nodiscard]] double dot(const Vec3& other) const;
	[[nodiscard]] double square_magnitude() const;
	[[nodiscard]] double magnitude() const;
	[[nodiscard]] bool is_close_to(const Vec3& other, const double rel_tol = 1e-9, const double abs_tol = 0.0) const;

	Vec3& operator*=(const double other);
	Vec3& operator/=(const double other);
	Vec3& operator+=(const Vec3& other);
	Vec3& operator-=(const Vec3& other);
};

[[nodiscard]] double triple_product(const Vec3& a, const Vec3& b, const Vec3& c);

[[nodiscard]] Vec3 operator*(const double scalar, Vec3 vec);
[[nodiscard]] Vec3 operator*(Vec3 vec, const double scalar);

[[nodiscard]] Vec3 operator/(Vec3 vec, const double scalar);

[[nodiscard]] Vec3 operator+(Vec3 vec, const Vec3& other);

[[nodiscard]] Vec3 operator-(Vec3 vec, const Vec3& other);

[[nodiscard]] bool operator==(const Vec3& vec, const Vec3& other);

[[nodiscard]] bool operator!=(const Vec3& vec, const Vec3& other);

std::ostream& operator<< (std::ostream& stream, const Vec3& vec);

