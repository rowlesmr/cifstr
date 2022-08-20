
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

#include "ctre/ctre.hpp"

#include "row/pdqciflib.hpp"


using namespace row;
using namespace row::util;


static Logger logger{};

static constexpr double as_B{ 8 * std::numbers::pi * std::numbers::pi };

static constexpr std::array NA_values{ ".", "?" };

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



std::string& fix_atom_type_i(std::string& atom) {
    auto m = ctre::match<"([A-Za-z]{1,2})(\\d{0,}\\.?\\d{0,})([+\\-]{0,1})(\\d{0,2})">(atom);

    std::string symbol { m.get<1>().to_string() }; 
    std::string charge { m.get<2>().to_string() }; 
    std::string sign   { m.get<3>().to_string() }; 
    std::string digit  { m.get<4>().to_string() }; 

    if (charge == "0") { // eg  "Si0+" is a valid symbol, but it is a neutral atom
        charge = "";
        sign = "";
    }

    if (sign.size() == 1 && charge.size() == 0) { //check for a single sign with no charge. eg F-. Needs to return F-1
        if (digit.size() == 1)
            charge = "1";
        else //the atom was probably the right way around to begin with
            return atom;
    }

    std::string new_atom = symbol + sign + charge;

    if (contains(allowed_atoms, new_atom)) {
        atom = new_atom;
        return atom;
    }

    logger.log(Logger::Verbosity::SOME, std::format("{0} is not a legal TOPAS scattering factor. Atom replaced with {1}.", new_atom, symbol));
    atom = symbol;

    return atom;
}

std::vector<std::string>& fix_atom_types_i(std::vector<std::string>& atoms) {
    std::for_each(atoms.begin(), atoms.end(), [](std::string& a) { fix_atom_type_i(a); });
    return atoms;
}

std::vector<std::string> fix_atom_types(std::vector<std::string> atoms) {
    fix_atom_types_i(atoms);
    return atoms;
}

std::string& label_to_atom_i(std::string& label) {
    if (contains(water, label.substr(0, 3))) {
        logger.log(Logger::Verbosity::SOME, std::format("Site label '{0}' probably means 'water'. Please check that this atom really is oxygen.", label));
        label = "O";
        return label;
    }
    else if (contains(elements, label.substr(0, 2))) {
        label = label.substr(0, 2);
        return label;
    }
    else if (contains(elements, label.substr(0, 1))) {
        std::string tmp = label.substr(0, 1);
        if (tmp == "W")
            logger.log(Logger::Verbosity::SOME, std::format("W detected for site '{0}'. Do you mean oxygen from a water molecule or tungsten? Please check.", label));
        label = tmp;
        return label;
    }

    logger.log(Logger::Verbosity::SOME, std::format("Can't decide what atom the site label '{0}' should be. Please check it.", label));
    return label;
}

std::vector<std::string>& labels_to_atoms_i(std::vector<std::string>& labels) {
    std::for_each(labels.begin(), labels.end(), [](std::string& label) { label_to_atom_i(label); });
    return labels;
}

std::vector<std::string> labels_to_atoms(std::vector<std::string> labels) {
    labels_to_atoms_i(labels);
    return labels;
}

std::string& make_frac_i(std::string& coord, const std::string_view label="") {
    
    std::string r{};
    if (auto m1 = ctre::match<"^([+\\-]?)0?\\.16{2,}[67]$">(coord)) {
        r = std::format("={0}1/6;", m1.get<1>().to_string());
    }
    else if (auto m2 = ctre::match<"^([+\\-]?)0?\\.3{4,}$">(coord)) {
        r = std::format("={0}1/3;", m2.get<1>().to_string());
    }
    else if (auto m3 = ctre::match<"^([+\\-]?)0?\\.6{3,}[67]$">(coord)) {
        r = std::format("={0}2/3;", m3.get<1>().to_string());
    }
    else if (auto m4 = ctre::match<"^([+\\-]?)0?\\.83{3,}$">(coord)) {
        r = std::format("={0}5/6;", m4.get<1>().to_string());
    }

    if (r.size() != 0) {
        if (label.size() == 0) {
            logger.log(Logger::Verbosity::ALL, std::format("Atomic site coordinate '{0}' replaced by '{1}'.", coord, r));
        }
        else {
            logger.log(Logger::Verbosity::ALL, std::format("Atomic fractional coordinate '{0}' replaced by '{1}' for site {2}.", coord, r, label));
        }
        coord = r;
    }

    return coord;
}

std::vector<std::string>& make_frac_i(std::vector<std::string>& v) {
    std::for_each(v.begin(), v.end(), [](std::string& s) { make_frac_i(s); });
    return v;
}
std::vector<std::string>& make_frac_i(std::vector<std::string>& coords, const std::vector<std::string>& labels) {
    for (size_t i{ 0 }; i < coords.size(); ++i) {
        make_frac_i(coords[i], labels[i]);
    }
    return coords;
}

std::vector<std::string> make_frac(std::vector<std::string> v) {
    make_frac_i(v);
    return v;
}
std::vector<std::string> make_frac(std::vector<std::string> coords, const std::vector<std::string>& labels) {
    make_frac_i(coords, labels);
    return coords;
}


struct Vec3 {
public:
    double x;
    double y;
    double z;

    Vec3(double u = 0.0, double v = 0.0, double w = 0.0)
        : x{ u }, y{ v }, z{ w }{}


    Vec3 cross_product(const Vec3& other) const {
        return Vec3(y * other.z - z * other.y,
                    z * other.x - x * other.z,
                    x * other.y - y * other.x);
    }
    Vec3 cross(const Vec3& other) const {
        return cross_product(other);
    }
    double dot_product(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    double dot(const Vec3& other) const {
        return dot_product(other);
    }

    double square_magnitude() const {
        return x * x + y * y + z * z;
    }

    double magnitude() const {
        return std::sqrt(square_magnitude());
    }



    const Vec3 operator+(const Vec3& other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }
    const Vec3 operator-(const Vec3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }
};
double triple_product(Vec3 a, Vec3 b, Vec3 c) {
    return a.dot_product(b.cross_product(c));
}
const Vec3 operator*(double scalar, const Vec3& vec) {
    return Vec3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
}
const Vec3 operator*(const Vec3& vec, double scalar) {
    return scalar * vec;
}
const Vec3 operator/(const Vec3& vec, double scalar) {
    return Vec3(vec.x / scalar, vec.y / scalar, vec.z / scalar);
}


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

    UnitCellVectors(double av, double bv, double cv, double alv, double bev, double gav) {
        a = Vec3(av);
        b = Vec3(bv * std::cos(gav * deg), bv * std::sin(gav * deg));
        double n = (std::cos(alv * deg) - (std::cos(gav * deg) * std::cos(bev * deg))) / std::sin(gav * deg);
        c = Vec3(cv * std::cos(bev * deg), cv * n, std::sqrt(std::sin(alv * deg) * std::sin(alv * deg) - n * n));

        double inv_vol{ 1 / triple_product(a, b, c) };

        Vec3 cas = b.cross_product(c);
        Vec3 cbs = c.cross_product(a);
        Vec3 ccs = a.cross_product(b);

        as = Vec3(cas.x, cas.y, cas.z) * inv_vol;
        bs = Vec3(cbs.x, cbs.y, cbs.z) * inv_vol;
        cs = Vec3(ccs.x, ccs.y, ccs.z) * inv_vol;
    }
};


enum class CrystalSystem
{
    Triclinic, Monoclinic_al, Monoclinic_be, Monoclinic_ga, Orthorhombic, Tetragonal, Hexagonal, Rhombohedral, Cubic
};


class UnitCell {
public:
    UnitCell(std::string av, std::string bv, std::string cv, std::string alv, std::string bev, std::string gav)
        : a_s{ strip_brackets(std::move(av)) }, b_s{ strip_brackets(std::move(bv)) }, c_s{ strip_brackets(std::move(cv)) },
        al_s{ strip_brackets(std::move(alv)) }, be_s{ strip_brackets(std::move(bev)) }, ga_s{ strip_brackets(std::move(gav)) },
        a{ stode(a_s).first }, b{ stode(b_s).first }, c{ stode(c_s).first }, al{ stode(al_s).first }, be{ stode(be_s).first }, ga{ stode(ga_s).first },
        crystal_system{ deduce_symmetry() }, usv{ UnitCellVectors(a,b,c,al,be,ga) } {}

    UnitCell(const row::cif::Block& block) {
        a_s = strip_brackets(block.get_value("_cell_length_a").getStrings()[0]);
        b_s = strip_brackets(block.get_value("_cell_length_b").getStrings()[0]);
        c_s = strip_brackets(block.get_value("_cell_length_c").getStrings()[0]);
        al_s = strip_brackets(block.get_value("_cell_angle_alpha").getStrings()[0]);
        be_s = strip_brackets(block.get_value("_cell_angle_beta").getStrings()[0]);
        ga_s = strip_brackets(block.get_value("_cell_angle_gamma").getStrings()[0]);
    
        a = block.get_value("_cell_length_a").getDoubles()[0];
        b = block.get_value("_cell_length_b").getDoubles()[0];
        c = block.get_value("_cell_length_c").getDoubles()[0];
        al = block.get_value("_cell_angle_alpha").getDoubles()[0];
        be = block.get_value("_cell_angle_beta").getDoubles()[0];
        ga = block.get_value("_cell_angle_gamma").getDoubles()[0];

        crystal_system = deduce_symmetry();
        usv = UnitCellVectors(a,b,c,al,be,ga);
    
    }

    const UnitCellVectors& get_unitcellvectors() const {
        return usv;
    }

    std::string to_string(size_t indent = 2) const {
        std::string tabs(indent, '\t');

        switch (crystal_system)
        {
        case CrystalSystem::Triclinic:
            return std::format("{0}a  {1}\t' {1}\n{0}b  {2}\t' {2}\n{0}c  {3}\t' {3}\n{0}al {4}\t' {4}\n{0}be {5}\t' {5}\n{0}ga {6}\t' {6}", tabs, a_s, b_s, c_s, al_s, be_s, ga_s);
        case CrystalSystem::Monoclinic_al:
            return std::format("{0}a  {1}\t' {1}\n{0}b  {2}\t' {2}\n{0}c  {3}\t' {3}\n{0}al {4}\t' {4}", tabs, a_s, b_s, c_s, al_s);
        case CrystalSystem::Monoclinic_be:
            return std::format("{0}a  {1}\t' {1}\n{0}b  {2}\t' {2}\n{0}c  {3}\t' {3}\n{0}be {4}\t' {4}", tabs, a_s, b_s, c_s, be_s);
        case CrystalSystem::Monoclinic_ga:
            return std::format("{0}a  {1}\t' {1}\n{0}b  {2}\t' {2}\n{0}c  {3}\t' {3}\n{0}ga {4}\t' {4}", tabs, a_s, b_s, c_s, ga_s);
        case CrystalSystem::Orthorhombic:
            return std::format("{0}a  {1}\t' {1}\n{0}b  {2}\t' {2}\n{0}c  {3}\t' {3}", tabs, a_s, b_s, c_s);
        case CrystalSystem::Tetragonal:
            return std::format("{0}Tetragonal({1}, {2}) ' {1}, {2}", tabs, a_s, c_s);
        case CrystalSystem::Hexagonal:
            return std::format("{0}Hexagonal({1}, {2}) ' {1}, {2}", tabs, a_s, c_s);
        case CrystalSystem::Rhombohedral:
            return std::format("{0}Rhombohedral({1}, {2}) ' {1}, {2}", tabs, a_s, al_s);
        case CrystalSystem::Cubic:
            return std::format("{0}Cubic({1}) ' {1}", tabs, a_s);
        default:
            return std::format("{0}a  {1}\t' {1}\n{0}b  {2}\t' {2}\n{0}c  {3}\t' {3}\n{0}al {4}\t' {4}\n{0}be {5}\t' {5}\n{0}ga {6}\t' {6}", tabs, a_s, b_s, c_s, al_s, be_s, ga_s);
        }
    }

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

    CrystalSystem deduce_symmetry() const {
        if (util::all_equal({ a,b,c }) && util::all_equal({ al,be,ga })) {
            if (util::are_equal(al, 90.0)) {
                return CrystalSystem::Cubic;
            }
            else {
                return CrystalSystem::Rhombohedral;
            }
        }
        else if (util::all_equal({ a,b }) && util::all_equal({ al,be,90.0 })) {
            if (util::are_equal(ga, 90.0)) {
                return CrystalSystem::Tetragonal;
            }
            if (util::are_equal(ga, 120.0)) {
                return CrystalSystem::Hexagonal; //can't tell hexagonal vs trigonal from unit cell prms, and the TOPAS macro is the same anyway, soooo...
            }
        }
        else if (!util::all_equal({ a,b,c }) && util::all_equal({ al,be,ga,90.0 })) {
            return CrystalSystem::Orthorhombic;
        }

        if (util::all_equal({ al,ga,90.0 }) && !util::all_equal({ be,90.0 })) {
            return CrystalSystem::Monoclinic_be;
        }
        else if (util::all_equal({ al,be,90.0 }) && !util::all_equal({ ga,90.0 })) {
            return CrystalSystem::Monoclinic_ga;
        }
        else if (util::all_equal({ ga,be,90.0 }) && !util::all_equal({ al,90.0 })) {
            return CrystalSystem::Monoclinic_al;
        }
            return CrystalSystem::Triclinic;
    }


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

    Site(std::string label, std::string x, std::string y, std::string z, std::string atom, std::string occ, std::string beq)
        : label{ std::move(label) }, x{ std::move(x) }, y{ std::move(y) }, z{ std::move(z) }, 
        atom{ std::move(atom) }, occ{ std::move(occ) }, beq{ std::move(beq) } {
    
        std::replace(label.begin(), label.end(), '\'', 'p'); //can't contain a "'", as this is a comment character in TOPAS
    }

    std::string to_string(size_t indent = 2) const {
        std::string tabs(indent, '\t');
        return std::format("{0}site {1} num_posns 0\tx {2} y {3} z {4} occ {5} {6} beq {7}", tabs, label, x, y, z, atom, occ, beq);
    }
};


class Sites {
private:
    std::vector<Site> m_sites{};
    std::string m_ss{};

public:
    Sites(const row::cif::Block& block) {

        std::vector<std::string> labels{ block.get_value("_atom_site_label").getStrings() };
        std::vector<std::string> xs{ block.get_value("_atom_site_fract_x").getStrings() };
        std::vector<std::string> ys{ block.get_value("_atom_site_fract_y").getStrings() };
        std::vector<std::string> zs{ block.get_value("_atom_site_fract_z").getStrings() };

        
        pad_column_i(strip_brackets_i(make_frac_i(xs, labels)));
        pad_column_i(strip_brackets_i(make_frac_i(ys, labels)));
        pad_column_i(strip_brackets_i(make_frac_i(zs, labels)));
        pad_column_i(labels);

        std::vector<std::string> atoms { get_atoms(block) };
        std::vector<std::string> occs  { get_occs(block) };
        std::vector<std::string> beqs  { get_Beqs(block) };

        
        m_sites.reserve(labels.size());
        for (size_t i{ 0 }; i < labels.size(); ++i) {
            m_sites.emplace_back(labels[i], xs[i], ys[i], zs[i], atoms[i], occs[i], beqs[i]);
        }

        m_ss = create_string();
    }

    const std::string& to_string() const {
        return m_ss;
    }

public:
    static std::optional<std::vector<std::string>> get_Biso(const row::cif::Block& block) {
        if(!block.contains("_atom_site_B_iso_or_equiv"))
            return std::nullopt;

        std::vector<std::string> beq{ block.get_value("_atom_site_B_iso_or_equiv").getStrings() };
        strip_brackets_i(beq);

        size_t NAs{ 0 };
        std::for_each(beq.begin(), beq.end(), [&NAs](const std::string& b) {if (contains(NA_values, b)) ++NAs; });

        if (NAs > 0) {
            logger.log(Logger::Verbosity::ALL, std::format("{0} missing Biso values.", NAs));
        }
        return beq;
    }

    static std::optional<std::vector<std::string>> get_Uiso_as_B(const row::cif::Block& block) {
        if (!block.contains("_atom_site_U_iso_or_equiv"))
            return std::nullopt;

        size_t NAs{ 0 };       
        const std::vector<std::string>& ueq{ block.get_value("_atom_site_U_iso_or_equiv").getStrings() };
        std::for_each(ueq.begin(), ueq.end(), [&NAs](const std::string& u) { if (contains(NA_values, u)) ++NAs;  });

        if (NAs > 0) {
            logger.log(Logger::Verbosity::ALL, std::format("{0} missing Uiso values.", NAs));
        }

        const std::vector<double>& ueq_dbl{ block.get_value("_atom_site_U_iso_or_equiv").getDoubles() };
        std::vector<std::string> r{};
        r.resize(ueq_dbl.size());
        std::transform(ueq_dbl.cbegin(), ueq_dbl.cend(), r.begin(), [](double d) { return std::format("{:.3f}", d * as_B); });

        return r;
    }

    static std::optional<std::vector<std::string>> get_Baniso_as_B(const row::cif::Block& block) {
        if (!block.contains("_atom_site_aniso_B_11"))
            return std::nullopt;

        std::vector<double> B11{ block.get_value("_atom_site_aniso_B_11").getDoubles() };
        std::vector<double> B22{ block.get_value("_atom_site_aniso_B_22").getDoubles() };
        std::vector<double> B33{ block.get_value("_atom_site_aniso_B_33").getDoubles() };
        std::vector<double> beq;
        beq.reserve(B11.size());

        for (size_t i{ 0 }; i < B11.size(); ++i) {
            beq.push_back((B11[i] + B22[i] + B33[i]) / 3.0);
        }

        std::vector<std::string> r{};
        r.resize(beq.size());
        std::transform(beq.cbegin(), beq.cend(), r.begin(), [](double d) { return std::format("{:.3f}", d); });

        return r;
    }

    static std::optional<std::vector<std::string>> get_Uaniso_as_B(const row::cif::Block& block) {
        if (!block.contains("_atom_site_aniso_U_11"))
            return std::nullopt;

        std::vector<double> U11{ block.get_value("_atom_site_aniso_U_11").getDoubles() };
        std::vector<double> U22{ block.get_value("_atom_site_aniso_U_22").getDoubles() };
        std::vector<double> U33{ block.get_value("_atom_site_aniso_U_33").getDoubles() };
        std::vector<double> ueq;
        ueq.reserve(U11.size());

        for (size_t i{ 0 }; i < U11.size(); ++i) {
            ueq.push_back((U11[i] + U22[i] + U33[i]) / 3.0);
        }

        std::vector<std::string> r{};
        r.resize(ueq.size());
        std::transform(ueq.cbegin(), ueq.cend(), r.begin(), [](double d) { return std::format("{:.3f}", d * as_B); });

        return r;
    }

    static std::optional<std::vector<std::string>> get_BEaniso_as_B(const row::cif::Block& block) {
        if (!block.contains("_atom_site_aniso_beta_11"))
            return std::nullopt;

        std::vector<double> BE11{ block.get_value("_atom_site_aniso_beta_11").getDoubles() };
        std::vector<double> BE22{ block.get_value("_atom_site_aniso_beta_22").getDoubles() };
        std::vector<double> BE33{ block.get_value("_atom_site_aniso_beta_33").getDoubles() };

        const UnitCellVectors usv = UnitCellVectors(block.get_value("_cell_length_a").getDoubles()[0], 
                                                    block.get_value("_cell_length_b").getDoubles()[0],
                                                    block.get_value("_cell_length_c").getDoubles()[0],
                                                    block.get_value("_cell_angle_alpha").getDoubles()[0],
                                                    block.get_value("_cell_angle_beta").getDoubles()[0],
                                                    block.get_value("_cell_angle_gamma").getDoubles()[0]);

        const double mas{ usv.as.square_magnitude() };
        const double mbs{ usv.bs.square_magnitude() };
        const double mcs{ usv.cs.square_magnitude() };

        std::transform(BE11.begin(), BE11.end(), BE11.begin(), [&mas](double be) { return 4.0 * be / mas; });
        std::transform(BE22.begin(), BE22.end(), BE22.begin(), [&mbs](double be) { return 4.0 * be / mbs; });
        std::transform(BE33.begin(), BE33.end(), BE33.begin(), [&mcs](double be) { return 4.0 * be / mcs; });

        std::vector<double> beeq;
        beeq.reserve(BE11.size());
        for (size_t i{ 0 }; i < BE11.size(); ++i) {
            beeq.push_back((BE11[i] + BE22[i] + BE33[i]) / 3.0);
        }
        
        std::vector<std::string> r{};
        r.resize(beeq.size());
        std::transform(beeq.cbegin(), beeq.cend(), r.begin(), [](double d) { return std::format("{:.3f}", d); });

        return r;
    }

    static std::unordered_map<std::string, std::string> make_beq_dict(const row::cif::Block& block, const std::string& b_type) {
        bool iso = true;

        auto f{ get_Biso }; // if you ask for the wrong thing, just go for the base case.

        if (b_type == "b_iso")
            f = get_Biso;
        else if (b_type == "u_iso")
            f = get_Uiso_as_B;
        else if (b_type == "b_aniso") {
            f = get_Baniso_as_B;
            iso = false;
        }
        else if (b_type == "u_aniso") {
            f = get_Uaniso_as_B;
            iso = false;
        }
        else if (b_type == "be_aniso") {
            f = get_BEaniso_as_B;
            iso = false;
        }

        std::unordered_map<std::string, std::string> dict{};
        std::optional<std::vector<std::string>> b_values{ f(block) };

        //need to be a bunch of checks to ensure that I return an empty dict in the case that it shouuld be.
        bool all_good{ true };

        all_good = all_good && b_values.has_value();
        all_good = all_good && (iso ? block.contains("_atom_site_label") : block.contains("_atom_site_aniso_label"));

        if (!all_good)
            return dict;

        std::vector<std::string> b_labels = iso ? block.get_value("_atom_site_label").getStrings() : block.get_value("_atom_site_aniso_label").getStrings();

        all_good = all_good && (b_values.value().size() == b_labels.size());

        if (!all_good)
            return dict;

        dict.reserve(b_labels.size());
        std::transform(b_labels.cbegin(), b_labels.cend(), b_values.value().begin(), std::inserter(dict, dict.end()), 
            [](const std::string& k, const std::string& v) {  return std::make_pair(k, v); });
        
        static const std::array bad_vals{ "nan", "0.000", ".", "?" };
        std::erase_if(dict, [](const auto& kv) { auto const& [_, val] = kv; return contains(bad_vals, val); });

        return dict;
    }

    static std::vector<std::string> get_atoms(const row::cif::Block& block) {
        std::vector<std::string> atoms{};
        atoms.reserve(block.get_value("_atom_site_label").size());

        if (block.contains("_atom_site_type_symbol")) {
            atoms = fix_atom_types(block.get_value("_atom_site_type_symbol").getStrings());
        }
        else {
            logger.log(Logger::Verbosity::SOME, "Atom types inferred from site labels. Please check for correctness.");
            atoms = labels_to_atoms(block.get_value("_atom_site_label").getStrings());
        }

        return pad_column_i(atoms);
    }

    static std::vector<std::string> get_occs(const row::cif::Block& block) {
        std::vector<std::string> occs{};
        occs.reserve(block.get_value("_atom_site_label").size());

        if (block.contains("_atom_site_occupancy")) {
            occs = block.get_value("_atom_site_occupancy").getStrings();
        }
        else {
            logger.log(Logger::Verbosity::SOME, "No occupancies found. All set to 1.");
            occs = std::vector<std::string>(block.get_value("_atom_site_label").size(), "1.");
        }
        return pad_column_i(strip_brackets_i(occs));
    }

    static std::vector<std::string> get_Beqs(const row::cif::Block& block) noexcept(false) {
        static const std::array<std::string, 5> beq_types{ "b_iso", "u_iso", "b_aniso", "u_aniso", "be_aniso" };
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> all_beqs{};
        for (const std::string& beq : beq_types) {
            all_beqs[beq] = make_beq_dict(block, beq);
        }

        const std::vector<std::string>& labels = block.get_value("_atom_site_label").getStrings();
        std::vector<std::string> beqs{};
        beqs.reserve(labels.size());

        for (const std::string& label : labels) {
            bool found = false;
            for (const std::string& beq : beq_types) {
                auto it = all_beqs[beq].find(label);
                if (it == all_beqs[beq].end())
                    continue;

                found = true;
                if (beq == beq_types[1])
                    logger.log(Logger::Verbosity::ALL, std::format("beq value for site {0} calculated from isotropic U value.", label));
                else if (beq == beq_types[2])
                    logger.log(Logger::Verbosity::ALL, std::format("beq value for site {0} calculated from anisotropic B values", label));
                else if (beq == beq_types[3])
                    logger.log(Logger::Verbosity::ALL, std::format("beq value for site {0} calculated from anisotropic U values", label));
                else if (beq == beq_types[4])
                    logger.log(Logger::Verbosity::ALL, std::format("beq value for site {0} calculated from anisotropic beta values", label));

                if ((it->second).starts_with('-')) {
                    logger.log(Logger::Verbosity::SOME, std::format("Negative atomic displacement parameter detected for site {0}! Please check.", label));
                }
                beqs.push_back(it->second);
                break;
            }
            if (!found) {
                logger.log(Logger::Verbosity::SOME, std::format("beq value missing or zero for site {0}! Default value of '1.' entered.", label));
                beqs.push_back("1.");
            }
        }
        return pad_column_i(beqs);
    }

    std::string create_string() const {
        std::string s{};
        for (const Site& site : m_sites) {
            s += site.to_string() + '\n';
        }
        return s;
    }
};


export class CrystalStructure {
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
    CrystalStructure(const row::cif::Block& block, std::string block_name, std::string source = std::string(), int verbosity = 1, bool add_stuff = true)
        : is_good{ check_block(block, verbosity) }, block_name{ std::move(block_name) }, source { std::move(source) },
        phase_name{ make_phase_name(block) }, space_group{ make_space_group(block) }, sites{ block }, unitcell{ block }, 
        m_ss{ create_string(add_stuff) } {
    
        
    }

    const std::string& to_string() const {
        return m_ss;
    }

    const std::string& get_source() const {
        return source;
    }

    std::string create_string(bool add_stuff, size_t indent = 1) const {
        std::string tab(indent, '\t');
        std::string tabs(indent+1, '\t');

        std::string s{ "\tstr '" };
        s += source + '\n';
        s += "\t\tphase_name \"" + phase_name + "\"\n";
        if (add_stuff) {
            s += "\t\tPhase_Density_g_on_cm3( 0)\n";
            s += "\t\tCS_L( ,200)\n";
            s += "\t\tMVW(0,0,0)\n";
            s += "\t\tscale @ 0.0001\n";
        }
        s += unitcell.to_string() + '\n';
        s += "\t\tspace_group \"" + space_group + "\"\n";
        s += sites.to_string() + '\n';
        return s;
    }


private:
    bool check_block(const row::cif::Block& block, int verbosity) const {
        logger.verbosity = static_cast<Logger::Verbosity>(verbosity);
        if (std::all_of(must_have_tags.cbegin(), must_have_tags.cend(), [&block](const std::string& tag) { return block.contains(tag); }) &&
            std::any_of(space_group_tags.cbegin(), space_group_tags.cend(), [&block](const std::string& tag) { return block.contains(tag); }))
            return true;
        else
            throw std::out_of_range(std::format("Block \"{0}\" doesn't contain sufficient information to make a structure.", block_name));
     }


    std::string make_phase_name(const row::cif::Block& block) const {
        std::string name{};
        auto it = std::find_if(phase_name_tags.begin(), phase_name_tags.end(), [&block](const std::string& tag) {return block.contains(tag); });
        if (it != phase_name_tags.end()) {
            name = block.get_value(*it).getStrings()[0];
        }

        replace_i(name, '\'', ' ');
        replace_i(name, '\"', ' ');
        trim_i(name);
        replace_i(name, '\n', '_');
        replace_i(name, '\r', '_');
        
        return name + "_" + block_name;
    }

    std::string make_space_group(const row::cif::Block& block) const {
        std::string sg{};
        auto it = std::find_if(space_group_tags.begin(), space_group_tags.end(), [&block](const std::string& tag) {return block.contains(tag); });
        if (it != space_group_tags.end()) {
            sg = block.get_value(*it).getStrings()[0];
        }

        if (std::all_of(sg.begin(), sg.end(), [](const char& c) { return std::isdigit(c) != 0; })) {
            logger.log(Logger::Verbosity::SOME, "Spacegroup given by number. Check that the SG setting matches that of the atom coordinates.");
        }

        return sg;
    }

};

#endif
