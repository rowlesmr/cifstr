#pragma once

#include "ctre/ctre.hpp"
#include "vec3.hpp"
#include "row/pdqciflib.hpp"
#include "util.hpp"

class UnitCellVectors
{
public:
	//real space vectors
	Vec3 a{};
	Vec3 b{};
	Vec3 c{};

	//reciprocal space vectors
	Vec3 as{};
	Vec3 bs{};
	Vec3 cs{};

	UnitCellVectors() = default;
	UnitCellVectors(double av, double bv, double cv, double alv, double bev, double gav);
};


enum class CrystalSystem
{
	Triclinic, Monoclinic_al, Monoclinic_be, Monoclinic_ga, Orthorhombic, Tetragonal, Hexagonal, Rhombohedral, Cubic
};


class UnitCell
{
public:
	UnitCell(std::string av, std::string bv, std::string cv, std::string alv, std::string bev, std::string gav);
	UnitCell(const row::cif::Block& block);

	const UnitCellVectors& get_unitcellvectors() const;
	std::string to_string(size_t indent = 2) const;

private:
	std::array<std::string, 3> cell_lengths_s;
	std::array<std::string, 3> cell_angles_s;

	CrystalSystem crystal_system;
	UnitCellVectors usv;

	CrystalSystem deduce_symmetry() const;
};

inline bool is_string_90(const std::string& ang)
{
	return ctre::match<"90(\\.0*)?">(ang);
}

inline bool is_string_120(const std::string& ang)
{
	return ctre::match<"120(\\.0*)?">(ang);
}




