
#ifndef ROW_CIFSTR_HPP
#define ROW_CIFSTR_HPP

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <array>
#include <unordered_map>
#include <format>
#include <numbers>
#include <algorithm>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <cmath>

#include "ctre/ctre.hpp"

#include "row/pdqciflib.hpp"


using namespace row;
using namespace row::util;


static Logger logger{};

static constexpr double as_B{ 8 * std::numbers::pi * std::numbers::pi };

static constexpr std::array<std::string_view, 2> NA_values{ ".", "?" };

static constexpr const std::array<std::string_view, 100> elements
{ "Ac", "Ag", "Al", "Am", "Ar", "As", "At", "Au", "Ba", "B",
  "Be", "Bi", "Br", "Ca", "Cd", "C",  "Ce", "Cl", "Co", "Cr",
  "Cs", "Cu", "Dy", "Er", "Eu", "F",  "Fe", "Fr", "Ga", "Gd",
  "Ge", "H",  "He", "Hg", "Hf", "Ho", "I",  "In", "Ir", "K",
  "Kr", "La", "Li", "Lu", "Mg", "Mn", "Mo", "N",  "Na", "Nb",
  "Nd", "Ne", "Ni", "O",  "Os", "Pb", "P",  "Pa", "Pd", "Po",
  "Pr", "Pm", "Pt", "Ra", "Rb", "Re", "Rh", "Rn", "Ru", "S",
  "Sb", "Sc", "Sm", "Se", "Si", "Sn", "Sr", "Ta", "Tb", "Tc",
  "Te", "Th", "Tl", "Ti", "Tm", "W",  "U",  "V",  "Xe", "Y",
  "Yb", "Zn", "Zr", "Np", "Pu", "Am", "Cm", "Bk", "Cf", "D" };

static constexpr const std::array<std::string_view, 3> water{ "Wat", "WAT", "wat" };

static constexpr const std::array<std::string_view, 212> allowed_atoms
{ "D",    "H",    "H-1",  "D-1",  "He",   "Li",   "Li+1", "Be",   "Be+2", "B",
  "C",    "N",    "O",    "O-1",  "O-2",  "F",    "F-1",  "Ne",   "Na",   "Na+1",
  "Mg",   "Mg+2", "Al",   "Al+3", "Si",   "Si+4", "P",    "S",    "Cl",   "Cl-1",
  "Ar",   "K",    "K+1",  "Ca",   "Ca+2", "Sc",   "Sc+3", "Ti",   "Ti+2", "Ti+3",
  "Ti+4", "V",    "V+2",  "V+3",  "V+5",  "Cr",   "Cr+2", "Cr+3", "Mn",   "Mn+2",
  "Mn+3", "Mn+4", "Fe",   "Fe+2", "Fe+3", "Co",   "Co+2", "Co+3", "Ni",   "Ni+2",
  "Ni+3", "Cu",   "Cu+1", "Cu+2", "Zn",   "Zn+2", "Ga",   "Ga+3", "Ge",   "Ge+4",
  "As",   "Se",   "Br",   "Br-1", "Kr",   "Rb",   "Rb+1", "Sr",   "Sr+2", "Y",
  "Y+3",  "Zr",   "Zr+4", "Nb",   "Nb+3", "Nb+5", "Mo",   "Mo+3", "Mo+5", "Mo+6",
  "Tc",   "Ru",   "Ru+3", "Ru+4", "Rh",   "Rh+3", "Rh+4", "Pd",   "Pd+2", "Pd+4",
  "Ag",   "Ag+1", "Ag+2", "Cd",   "Cd+2", "In",   "In+3", "Sn",   "Sn+2", "Sn+4",
  "Sb",   "Sb+3", "Sb+5", "Te",   "I",    "I-1",  "Xe",   "Cs",   "Cs+1", "Ba",
  "Ba+2", "La",   "La+3", "Ce",   "Ce+3", "Ce+4", "Pr",   "Pr+3", "Pr+4", "Nd",
  "Nd+3", "Pm",   "Pm+3", "Sm",   "Sm+3", "Eu",   "Eu+2", "Eu+3", "Gd",   "Gd+3",
  "Tb",   "Tb+3", "Dy",   "Dy+3", "Ho",   "Ho+3", "Er",   "Er+3", "Tm",   "Tm+3",
  "Yb",   "Yb+2", "Yb+3", "Lu",   "Lu+3", "Hf",   "Hf+4", "Ta",   "Ta+5", "W",
  "W+6",  "Re",   "Os",   "Os+4", "Ir",   "Ir+3", "Ir+4", "Pt",   "Pt+2", "Pt+4",
  "Au",   "Au+1", "Au+3", "Hg",   "Hg+1", "Hg+2", "Tl",   "Tl+1", "Tl+3", "Pb",
  "Pb+2", "Pb+4", "Bi",   "Bi+3", "Bi+5", "Po",   "At",   "Rn",   "Fr",   "Ra",
  "Ra+2", "Ac",   "Ac+3", "Th",   "Th+4", "Pa",   "U",    "U+3",  "U+4",  "U+6",
  "Np",   "Np+3", "Np+4", "Np+6", "Pu",   "Pu+3", "Pu+4", "Pu+6", "Am",   "Cm",
  "Bk",   "Cf" };



std::string& fix_atom_type_i(std::string& atom);
std::vector<std::string>& fix_atom_types_i(std::vector<std::string>& atoms);
std::vector<std::string> fix_atom_types(std::vector<std::string> atoms);

std::string& label_to_atom_i(std::string& label);
std::vector<std::string>& labels_to_atoms_i(std::vector<std::string>& labels);
std::vector<std::string> labels_to_atoms(std::vector<std::string> labels);

std::string& make_frac_i(std::string& coord, const std::string_view label="");

std::vector<std::string>& make_frac_i(std::vector<std::string>& v);
std::vector<std::string>& make_frac_i(std::vector<std::string>& coords, const std::vector<std::string>& labels);
std::vector<std::string> make_frac(std::vector<std::string> v);
std::vector<std::string> make_frac(std::vector<std::string> coords, const std::vector<std::string>& labels);


struct Vec3 {
public:
    double x;
    double y;
    double z;

    Vec3(double u = 0.0, double v = 0.0, double w = 0.0);

    Vec3 cross_product(const Vec3& other) const;
    Vec3 cross(const Vec3& other) const;
    double dot_product(const Vec3& other) const;
    double dot(const Vec3& other) const;
    double square_magnitude() const;
    double magnitude() const;

    const Vec3 operator+(const Vec3& other) const;
    const Vec3 operator-(const Vec3& other) const;
};
double triple_product(Vec3 a, Vec3 b, Vec3 c);
const Vec3 operator*(double scalar, const Vec3& vec);
const Vec3 operator*(const Vec3& vec, double scalar);
const Vec3 operator/(const Vec3& vec, double scalar);


struct UnitCellVectors {
public:
    //real space vectors
    Vec3 a;
    Vec3 b;
    Vec3 c;

    //reciprocal space vectors
    Vec3 as;
    Vec3 bs;
    Vec3 cs;

    UnitCellVectors() = default;
    UnitCellVectors(double av, double bv, double cv, double alv, double bev, double gav);
};


enum class CrystalSystem
{
    Triclinic, Monoclinic_al, Monoclinic_be, Monoclinic_ga, Orthorhombic, Tetragonal, Hexagonal, Rhombohedral, Cubic
};


class UnitCell {
public:
    UnitCell(std::string av, std::string bv, std::string cv, std::string alv, std::string bev, std::string gav);
    UnitCell(const row::cif::Block& block);

    const UnitCellVectors& get_unitcellvectors() const;
    std::string to_string(size_t indent = 2) const;

private:
    std::string a_s;
    std::string b_s;
    std::string c_s;
    std::string al_s;
    std::string be_s;
    std::string ga_s;

    double a;
    double b;
    double c;
    double al;
    double be;
    double ga;

    CrystalSystem crystal_system;
    UnitCellVectors usv;

    CrystalSystem deduce_symmetry() const;
};


struct Site {
public:
    std::string label{};
    std::string x{};
    std::string y{};
    std::string z{};
    std::string atom{};
    std::string occ{};
    std::string beq{};

    Site(std::string t_label, std::string t_x, std::string t_y, std::string t_z, std::string t_atom, std::string t_occ, std::string t_beq);

    std::string to_string(size_t indent = 2) const;
};


class Sites {
private:
    std::vector<Site> m_sites{};
    std::string m_ss{};

public:
    Sites(const row::cif::Block& block);

    const std::string& to_string() const;

private:
    static std::optional<std::vector<std::string>> get_Biso(const row::cif::Block& block);
    static std::optional<std::vector<std::string>> get_Uiso_as_B(const row::cif::Block& block);
    static std::optional<std::vector<std::string>> get_Baniso_as_B(const row::cif::Block& block);
    static std::optional<std::vector<std::string>> get_Uaniso_as_B(const row::cif::Block& block);
    static std::optional<std::vector<std::string>> get_BEaniso_as_B(const row::cif::Block& block);
    static std::unordered_map<std::string, std::string> make_beq_dict(const row::cif::Block& block, const std::string& b_type);
    static std::vector<std::string> get_atoms(const row::cif::Block& block);
    static std::vector<std::string> get_occs(const row::cif::Block& block);
    static std::vector<std::string> get_Beqs(const row::cif::Block& block) noexcept(false);

    std::string create_string() const;
};


class CrystalStructure {
private:
    bool is_good{ false };
    std::string block_name{};
    std::string source{};
    std::string phase_name{};
    std::string space_group{};
    Sites sites;
    UnitCell unitcell;
    std::string m_ss{};

    static constexpr std::array phase_name_tags{ "_pd_phase_name", "_chemical_name_mineral", "_chemical_name_common", "_chemical_name_systematic", "_chemical_name_structure_type" };
    static constexpr std::array space_group_tags{ "_symmetry_space_group_name_H-M", "_space_group_name_H-M_alt", "_symmetry_Int_Tables_number", "_space_group_IT_number" };

    static constexpr std::array must_have_tags { "_cell_length_a", "_cell_length_b", "_cell_length_c", 
                                                 "_cell_angle_alpha", "_cell_angle_beta", "_cell_angle_gamma", 
                                                 "_atom_site_label", "_atom_site_fract_x", "_atom_site_fract_y", "_atom_site_fract_z" };

public:
    CrystalStructure(const row::cif::Block& block, std::string block_name, std::string source = std::string(), int verbosity = 1, bool add_stuff = true);

    const std::string& to_string() const;
    const std::string& get_source() const;
    std::string create_string(bool add_stuff, size_t indent = 1) const;


private:
    bool check_block(const row::cif::Block& block, int verbosity) const;
    std::string make_phase_name(const row::cif::Block& block) const;
    std::string make_space_group(const row::cif::Block& block) const;
};

#endif
