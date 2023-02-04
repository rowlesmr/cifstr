#include <cmath>
#include "util.hpp"
#include "vec3.hpp"

Vec3::Vec3(double u /*= 0.0*/, double v /*= 0.0*/, double w /*= 0.0*/)
	: x{ u }, y{ v }, z{ w }
{}

Vec3 Vec3::cross_product(const Vec3& other) const
{
	return { y * other.z - z * other.y,
			 z * other.x - x * other.z,
			 x * other.y - y * other.x };
}

Vec3 Vec3::cross(const Vec3& other) const
{
	return cross_product(other);
}

double Vec3::dot_product(const Vec3& other) const
{
	return (x * other.x) + (y * other.y) + (z * other.z);
}

double Vec3::dot(const Vec3& other) const
{
	return dot_product(other);
}

double Vec3::square_magnitude() const
{
	return (x * x) + (y * y) + (z * z);
}

double Vec3::magnitude() const
{
	return std::sqrt(square_magnitude());
}

bool Vec3::is_close_to(const Vec3& other, const double rel_tol /*= 1e-9*/, const double abs_tol /*= 0.0*/) const
{
	return is_close(x, other.x, rel_tol, abs_tol) &&
		is_close(y, other.y, rel_tol, abs_tol) &&
		is_close(z, other.z, rel_tol, abs_tol);
}

Vec3& Vec3::operator*=(const double other)
{
	x *= other;
	y *= other;
	z *= other;
	return *this;
}

Vec3& Vec3::operator/=(const double other)
{
	x /= other;
	y /= other;
	z /= other;
	return *this;
}

Vec3& Vec3::operator+=(const Vec3& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

Vec3& Vec3::operator-=(const Vec3& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}





double triple_product(const Vec3& a, const Vec3& b, const Vec3& c)
{
	return a.dot_product(b.cross_product(c));
}

Vec3 operator*(const double scalar, Vec3 vec)
{
	return vec * scalar;
}

Vec3 operator*(Vec3 vec, const double scalar)
{
	vec *= scalar;
	return vec;
}

Vec3 operator/(Vec3 vec, const double scalar)
{
	vec /= scalar;
	return vec;
}

Vec3 operator+(Vec3 vec, const Vec3& other)
{
	vec += other;
	return vec;
}

Vec3 operator-(Vec3 vec, const Vec3& other)
{
	vec -= other;
	return vec;
}

bool operator==(const Vec3& vec, const Vec3& other)
{
	return vec.x == other.x && vec.y == other.y && vec.z == other.z;
}

bool operator!=(const Vec3& vec, const Vec3& other)
{
	return !(vec == other);
}


std::ostream& operator<<(std::ostream& stream, const Vec3& vec)
{
	stream << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
}


