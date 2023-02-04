#include "unitcell.hpp"


UnitCellVectors::UnitCellVectors(double av, double bv, double cv, double alv, double bev, double gav)
{
	// taken from http://gisaxs.com/index.php/Unit_cell
	a = Vec3(1.0) * av;
	b = Vec3(std::cos(gav * deg), std::sin(gav * deg)) * bv;
	double n = (std::cos(alv * deg) - (std::cos(bev * deg) * std::cos(gav * deg))) / std::sin(gav * deg);
	c = Vec3(std::cos(bev * deg), n, std::sqrt(1 - pow(std::cos(bev * deg), 2) - pow(n, 2))) * cv;

	double inv_vol{ 1 / triple_product(a, b, c) };

	as = b.cross_product(c) * inv_vol;
	bs = c.cross_product(a) * inv_vol;
	cs = a.cross_product(b) * inv_vol;
}


UnitCell::UnitCell(std::string av, std::string bv, std::string cv, std::string alv, std::string bev, std::string gav)
{
	cell_lengths_s[0] = std::move(av);
	cell_lengths_s[1] = std::move(bv);
	cell_lengths_s[2] = std::move(cv);
	cell_angles_s[0] = std::move(alv);
	cell_angles_s[1] = std::move(bev);
	cell_angles_s[2] = std::move(gav);

	crystal_system = deduce_symmetry();
	usv = UnitCellVectors(stod(cell_lengths_s[0]), stod(cell_lengths_s[1]), stod(cell_lengths_s[2]),
		stod(cell_angles_s[0]), stod(cell_angles_s[1]), stod(cell_angles_s[2]));
}

UnitCell::UnitCell(const row::cif::Block& block)
{
	UnitCell(row::util::strip_brackets(block.getValue("_cell_length_a").getStrings()[0]),
		row::util::strip_brackets(block.getValue("_cell_length_b").getStrings()[0]),
		row::util::strip_brackets(block.getValue("_cell_length_c").getStrings()[0]),
		row::util::strip_brackets(block.getValue("_cell_angle_alpha").getStrings()[0]),
		row::util::strip_brackets(block.getValue("_cell_angle_beta").getStrings()[0]),
		row::util::strip_brackets(block.getValue("_cell_angle_gamma").getStrings()[0]));
}
const UnitCellVectors& UnitCell::get_unitcellvectors() const
{
	return usv;
}

std::string UnitCell::to_string(size_t indent /*= 2*/) const
{
	std::string tabs(indent, '\t');

	switch (crystal_system)
	{
	case CrystalSystem::Triclinic:
		return std::format("{0}a  {1}\t' {1}\n{0}b  {2}\t' {2}\n{0}c  {3}\t' {3}\n{0}al {4}\t' {4}\n{0}be {5}\t' {5}\n{0}ga {6}\t' {6}", tabs, cell_lengths_s[0], cell_lengths_s[1], cell_lengths_s[2], cell_angles_s[0], cell_angles_s[1], cell_angles_s[2]);
	case CrystalSystem::Monoclinic_al:
		return std::format("{0}a  {1}\t' {1}\n{0}b  {2}\t' {2}\n{0}c  {3}\t' {3}\n{0}al {4}\t' {4}", tabs, cell_lengths_s[0], cell_lengths_s[1], cell_lengths_s[2], cell_angles_s[0]);
	case CrystalSystem::Monoclinic_be:
		return std::format("{0}a  {1}\t' {1}\n{0}b  {2}\t' {2}\n{0}c  {3}\t' {3}\n{0}be {4}\t' {4}", tabs, cell_lengths_s[0], cell_lengths_s[1], cell_lengths_s[2], cell_angles_s[1]);
	case CrystalSystem::Monoclinic_ga:
		return std::format("{0}a  {1}\t' {1}\n{0}b  {2}\t' {2}\n{0}c  {3}\t' {3}\n{0}ga {4}\t' {4}", tabs, cell_lengths_s[0], cell_lengths_s[1], cell_lengths_s[2], cell_angles_s[2]);
	case CrystalSystem::Orthorhombic:
		return std::format("{0}a  {1}\t' {1}\n{0}b  {2}\t' {2}\n{0}c  {3}\t' {3}", tabs, cell_lengths_s[0], cell_lengths_s[1], cell_lengths_s[2]);
	case CrystalSystem::Tetragonal:
		return std::format("{0}Tetragonal({1}, {2}) ' {1}, {2}", tabs, cell_lengths_s[0], cell_lengths_s[2]);
	case CrystalSystem::Hexagonal:
		return std::format("{0}Hexagonal({1}, {2}) ' {1}, {2}", tabs, cell_lengths_s[0], cell_lengths_s[2]);
	case CrystalSystem::Rhombohedral:
		return std::format("{0}Rhombohedral({1}, {2}) ' {1}, {2}", tabs, cell_lengths_s[0], cell_angles_s[0]);
	case CrystalSystem::Cubic:
		return std::format("{0}Cubic({1}) ' {1}", tabs, cell_lengths_s[0]);
	}
	return std::format("{0}a  {1}\t' {1}\n{0}b  {2}\t' {2}\n{0}c  {3}\t' {3}\n{0}al {4}\t' {4}\n{0}be {5}\t' {5}\n{0}ga {6}\t' {6}", tabs, cell_lengths_s[0], cell_lengths_s[1], cell_lengths_s[2], cell_angles_s[0], cell_angles_s[1], cell_angles_s[2]);
}

CrystalSystem UnitCell::deduce_symmetry() const
{
	if (all_equal(cell_lengths_s) && all_equal(cell_angles_s))
	{
		if (is_string_90(cell_angles_s[0]))
		{
			return CrystalSystem::Cubic;
		}
		return CrystalSystem::Rhombohedral;
	}
	if (cell_lengths_s[0] == cell_lengths_s[1] && is_string_90(cell_angles_s[0]) && cell_angles_s[0] == cell_angles_s[1])
	{
		if (is_string_90(cell_angles_s[2]))
		{
			return CrystalSystem::Tetragonal;
		}
		if (is_string_120(cell_angles_s[2]))
		{
			return CrystalSystem::Hexagonal; //can't tell hexagonal vs trigonal from unit cell prms, and the TOPAS macro is the same anyway, soooo...
		}
	}
	else if (!all_equal(cell_lengths_s) && is_string_90(cell_angles_s[0]) && all_equal(cell_angles_s))
	{
		return CrystalSystem::Orthorhombic;
	}

	if (is_string_90(cell_angles_s[0]) && !is_string_90(cell_angles_s[1]) && is_string_90(cell_angles_s[2]))
	{
		return CrystalSystem::Monoclinic_be;
	}
	if (is_string_90(cell_angles_s[0]) && is_string_90(cell_angles_s[1]) && !is_string_90(cell_angles_s[2]))
	{
		return CrystalSystem::Monoclinic_ga;
	}
	if (!is_string_90(cell_angles_s[0]) && is_string_90(cell_angles_s[1]) && is_string_90(cell_angles_s[2]))
	{
		return CrystalSystem::Monoclinic_al;
	}

	return CrystalSystem::Triclinic; //no test for this so that it can capture anything that falls through gaps.
	// eg an orthorhombic cell with two edges the same length.
}


