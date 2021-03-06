
//

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

#include "ctre/ctre.hpp"

import cif;
import util;
import logger;


export module cifstr;


using namespace row;

#ifdef _DEBUG
static Logger logger{ Logger::Level::DEBUG };
#else
static Logger logger{ Logger::Level::INFO };
#endif

static constexpr double as_B{ 8 * std::numbers::pi * std::numbers::pi };

static constexpr std::array NA_values{ ".", "?" };

static constexpr std::array elements
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

static constexpr std::array water{ "Wat", "WAT", "wat" };

static constexpr std::array allowed_atoms
{ "D",    "H",    "H-1",  "D-1",  "He",   "Li",   "Li+1", "Be",  "Be+2",  "B",
  "C",    "N",    "O",    "O-1",  "O-2",  "F",    "F-1",  "Ne",  "Na",   "Na+1",
  "Mg",   "Mg+2", "Al",   "Al+3", "Si",   "Si+4", "P",    "S",   "Cl",   "Cl-1",
  "Ar",   "K",    "K+1",  "Ca",   "Ca+2", "Sc",   "Sc+3", "Ti",  "Ti+2",  "Ti+3",
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

static const std::string one_sixth{ "=1/6;" };
static const std::string one_third{ "=1/3;" };
static const std::string two_third{ "=2/3;" };
static const std::string five_sixth{ "=5/6;" };
static const std::string neg_one_sixth{ "=-1/6;" };
static const std::string neg_one_third{ "=-1/3;" };
static const std::string neg_two_third{ "=-2/3;" };
static const std::string neg_five_sixth{ "=-5/6;" };

static const std::unordered_map<std::string, std::string> fractions
({
   {"0.1666", one_sixth}, {"0.16666", one_sixth}, {"0.166666", one_sixth}, {"0.1666666", one_sixth},
   {"0.1667", one_sixth}, {"0.16667", one_sixth}, {"0.166667", one_sixth}, {"0.1666667", one_sixth},
   {".1666",  one_sixth}, {".16666",  one_sixth}, {".166666",  one_sixth}, {".1666666",  one_sixth},
   {".1667",  one_sixth}, {".16667",  one_sixth}, {".166667",  one_sixth}, {".1666667",  one_sixth},

   {"-0.1666", neg_one_sixth}, {"-0.16666", neg_one_sixth}, {"-0.166666", neg_one_sixth}, {"-0.1666666", neg_one_sixth},
   {"-0.1667", neg_one_sixth}, {"-0.16667", neg_one_sixth}, {"-0.166667", neg_one_sixth}, {"-0.1666667", neg_one_sixth},
   {"-.1666",  neg_one_sixth}, {"-.16666",  neg_one_sixth}, {"-.166666",  neg_one_sixth}, {"-.1666666",  neg_one_sixth},
   {"-.1667",  neg_one_sixth}, {"-.16667",  neg_one_sixth}, {"-.166667",  neg_one_sixth}, {"-.1666667",  neg_one_sixth},

   {"0.3333",  one_third}, {"0.33333",  one_third}, {"0.333333",  one_third}, {"0.3333333",  one_third},
   {".3333",   one_third}, {".33333",   one_third}, {".333333",   one_third}, {".3333333",   one_third},

   {"-0.3333", neg_one_third}, {"-0.33333", neg_one_third}, {"-0.333333", neg_one_third}, {"-0.3333333", neg_one_third},
   {"-.3333",  neg_one_third}, {"-.33333",  neg_one_third}, {"-.333333",  neg_one_third}, {"-.3333333",  neg_one_third},

   {"0.6666", two_third}, {"0.66666", two_third}, {"0.666666", two_third}, {"0.6666666", two_third},
   {"0.6667", two_third}, {"0.66667", two_third}, {"0.666667", two_third}, {"0.6666667", two_third},
   {".6666",  two_third}, {".66666",  two_third}, {".666666",  two_third}, {".6666666",  two_third},
   {".6667",  two_third}, {".66667",  two_third}, {".666667",  two_third}, {".6666667",  two_third},

   {"-0.6666", neg_two_third}, {"-0.66666", neg_two_third}, {"-0.666666", neg_two_third}, {"-0.6666666", neg_two_third},
   {"-0.6667", neg_two_third}, {"-0.66667", neg_two_third}, {"-0.666667", neg_two_third}, {"-0.6666667", neg_two_third},
   {"-.6666",  neg_two_third}, {"-.66666",  neg_two_third}, {"-.666666",  neg_two_third}, {"-.6666666",  neg_two_third},
   {"-.6667",  neg_two_third}, {"-.66667",  neg_two_third}, {"-.666667",  neg_two_third}, {"-.6666667",  neg_two_third},

   {"0.8333",  five_sixth}, {"0.83333",  five_sixth}, {"0.833333",  five_sixth}, {"0.8333333",  five_sixth},
   {".8333",   five_sixth}, {".83333",   five_sixth}, {".833333",   five_sixth}, {".8333333",   five_sixth},

   {"-0.8333", neg_five_sixth}, {"-0.83333", neg_five_sixth}, {"-0.833333", neg_five_sixth}, {"-0.8333333",neg_five_sixth},
   {"-.8333",  neg_five_sixth}, {"-.83333",  neg_five_sixth}, {"-.833333",  neg_five_sixth}, {"-.8333333", neg_five_sixth}
    });


std::string& fix_atom_type_i(std::string& atom) {
    auto m = ctre::match<"([A-Za-z]{1,2})(\\d{0,2})([+\\-]{0,1})(\\d{0,2})">(atom);

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

    if (is_in(new_atom, allowed_atoms)) {
        atom = new_atom;
        return atom;
    }

    logger.log(Logger::Level::INFO, std::format("{0} is not a legal TOPAS scattering factor. Atom replaced with {1}.", new_atom, symbol));
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
    if (is_in(label.substr(0, 3), water)) {
        logger.log(Logger::Level::WARNING, std::format("Site label '{0}' probably means 'water'. Please check that this atom really is oxygen.", label));
        label = "O";
        return label;
    }
    else if (is_in(label.substr(0, 2), elements)) {
        label = label.substr(0, 2);
        return label;
    }
    else if (is_in(label.substr(0, 1), elements)) {
        std::string tmp = label.substr(0, 1);
        if (tmp == "W")
            logger.log(Logger::Level::WARNING, std::format("W detected for site '{0}'. Do you mean oxygen from a water molecule or tungsten? Please check.", label));
        label = tmp;
        return label;
    }

    logger.log(Logger::Level::WARNING, std::format("Can't decide what atom the site label '{0}' should be. Please check it.", label));
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

std::string& make_frac_i(std::string& s) {
    auto result = fractions.find(s);
    if (result != fractions.end()) {
        logger.log(Logger::Level::INFO, std::format("Atomic site coordinate '{0}' replaced by '{1}'.", s, result->second));
        s = result->second;
    }
    return s;
}
std::string& make_frac_i(std::string& coord, const std::string& label) {
    auto result = fractions.find(coord);
    if (result != fractions.end()) {
        logger.log(Logger::Level::INFO, std::format("Atomic fractional coordinate '{0}' replaced by '{1}' for site {2}.", coord, result->second, label));
        coord = result->second;
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
};

Vec3 cross_product(Vec3 a, Vec3 b) {
    return Vec3(a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x);
}

double dot_product(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

double triple_product(Vec3 a, Vec3 b, Vec3 c) {
    return dot_product(a, cross_product(b, c));
}

double square_magnitude(Vec3 a) {
    return a.x * a.x + a.y * a.y + a.z * a.z;
}

double magnitude(Vec3 a) {
    return std::sqrt(square_magnitude(a));
}



struct UnitCellVectors {
public:
    Vec3 a;
    Vec3 b;
    Vec3 c;
    Vec3 as;
    Vec3 bs;
    Vec3 cs;

    UnitCellVectors() = default;

    UnitCellVectors(double av, double bv, double cv, double alv, double bev, double gav) {
        a = Vec3(av);
        b = Vec3(bv * std::cos(gav * rad), bv * std::sin(gav * rad));
        double n = (std::cos(alv * rad) - (std::cos(gav * rad) * std::cos(bev * rad))) / std::sin(gav * rad);
        c = Vec3(cv * std::cos(bev * rad), cv * n, std::sqrt(std::sin(alv * rad) * std::sin(alv * rad) - n * n));

        double inv_vol = 1 / triple_product(a, b, c);

        Vec3 cas = cross_product(b, c);
        Vec3 cbs = cross_product(c, a);
        Vec3 ccs = cross_product(a, b);

        as = Vec3(cas.x * inv_vol, cas.y * inv_vol, cas.z * inv_vol);
        bs = Vec3(cbs.x * inv_vol, cbs.y * inv_vol, cbs.z * inv_vol);
        cs = Vec3(ccs.x * inv_vol, ccs.y * inv_vol, ccs.z * inv_vol);
    }

    UnitCellVectors(std::string av, std::string bv, std::string cv, std::string alv, std::string bev, std::string gav)
        : UnitCellVectors(str_to_v(std::move(av)), str_to_v(std::move(bv)), str_to_v(std::move(cv)), str_to_v(std::move(alv)), str_to_v(std::move(bev)), str_to_v(std::move(gav))) {}

    UnitCellVectors(const char* av, const char* bv, const char* cv, const char* alv, const char* bev, const char* gav)
        : UnitCellVectors(str_to_v(av), str_to_v(bv), str_to_v(cv), str_to_v(alv), str_to_v(bev), str_to_v(gav)) {}
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
        a{ str_to_v(a_s) }, b{ str_to_v(b_s) }, c{ str_to_v(c_s) }, al{ str_to_v(al_s) }, be{ str_to_v(be_s) }, ga{ str_to_v(ga_s) },
        crystal_system{ deduce_crystal_system() }, usv{ UnitCellVectors(a,b,c,al,be,ga) }, m_ss{ create_string() } {}

    UnitCell(const row::cif::Block& block)
        : UnitCell(block.get_value("_cell_length_a")[0], block.get_value("_cell_length_b")[0], block.get_value("_cell_length_c")[0],
        block.get_value("_cell_angle_alpha")[0], block.get_value("_cell_angle_beta")[0], block.get_value("_cell_angle_gamma")[0]) {}

    const std::string& to_string() const {
        return m_ss;
    }

    const UnitCellVectors& get_unitcellvectors() const {
        return usv;
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

    std::string m_ss{};

    std::string create_string() {
        switch (crystal_system)
        {
            case CrystalSystem::Triclinic:
                return std::format("\t\ta  {0}\t' {0}\n\t\tb  {1}\t' {1}\n\t\tc  {2}\t' {2}\n\t\tal {3}\t' {3}\n\t\tbe {4}\t' {4}\n\t\tga {5}\t' {5}", a_s, b_s, c_s, al_s, be_s, ga_s);
            case CrystalSystem::Monoclinic_al:
                return std::format("\t\ta  {0}\t' {0}\n\t\tb  {1}\t' {1}\n\t\tc  {2}\t' {2}\n\t\tal {3}\t' {3}", a_s, b_s, c_s, al_s);
            case CrystalSystem::Monoclinic_be:
                return std::format("\t\ta  {0}\t' {0}\n\t\tb  {1}\t' {1}\n\t\tc  {2}\t' {2}\n\t\tbe {3}\t' {3}", a_s, b_s, c_s, be_s);
            case CrystalSystem::Monoclinic_ga:
                return std::format("\t\ta  {0}\t' {0}\n\t\tb  {1}\t' {1}\n\t\tc  {2}\t' {2}\n\t\tga {3}\t' {3}", a_s, b_s, c_s, ga_s);
            case CrystalSystem::Orthorhombic:
                return std::format("\t\ta  {0}\t' {0}\n\t\tb  {1}\t' {1}\n\t\tc  {2}\t' {2}", a_s, b_s, c_s);
            case CrystalSystem::Tetragonal:
                return std::format("\t\tTetragonal({0}, {1}) ' {0}, {1}", a_s, c_s);
            case CrystalSystem::Hexagonal:
                return std::format("\t\tHexagonal({0}, {1}) ' {0}, {1}", a_s, c_s);
            case CrystalSystem::Rhombohedral:
                return std::format("\t\tRhombohedral({0}, {1}) ' {0}, {1}", a_s, al_s);
            case CrystalSystem::Cubic:
                return std::format("\t\tCubic({0}) ' {0}", a_s);
            default:
                return std::format("\t\ta  {0}\t' {0}\n\t\tb  {1}\t' {1}\n\t\tc  {2}\t' {2}\n\t\tal {3}\t' {3}\n\t\tbe {4}\t' {4}\n\t\tga {5}\t' {5}", a_s, b_s, c_s, al_s, be_s, ga_s);
        }
    }

    CrystalSystem deduce_crystal_system() {
        if (are_equal(al, 90.0) && are_equal(be, 90.0) && are_equal(ga, 90.0)) {
            if (are_equal(a, b) && are_equal(b, c))
                return CrystalSystem::Cubic;
            else if (are_equal(a, b) && !are_equal(b, c))
                return CrystalSystem::Tetragonal;
            else if (!are_equal(a, b) && !are_equal(b, c))
                return CrystalSystem::Orthorhombic;
            else
                return CrystalSystem::Triclinic;
        }
        else if (are_equal(al, 90.0) && are_equal(be, 90.0) && are_equal(ga, 120.0)) {
            if (are_equal(a, b) && !are_equal(b, c))
                return CrystalSystem::Hexagonal; //can't tell hexagonal vs trigonal from unit cell prms, and the TOPAS macro is the same anyway, soooo...
            else
                return CrystalSystem::Triclinic;
        }
        else if (are_equal(al, be) && are_equal(be, ga) && !are_equal(al, 90.0)) {
            if (are_equal(a, b) && are_equal(b, c))
                return CrystalSystem::Rhombohedral;
            else
                return CrystalSystem::Triclinic;
        }
        else if (!are_equal(a, b) && !are_equal(b, c)) {
            if (are_equal(al, be) && !are_equal(be, ga) && are_equal(al, 90.0))
                return CrystalSystem::Monoclinic_ga;
            else if (are_equal(al, ga) && !are_equal(ga, be) && are_equal(al, 90.0))
                return CrystalSystem::Monoclinic_be;
            else if (are_equal(be, ga) && !are_equal(ga, al) && are_equal(be, 90.0))
                return CrystalSystem::Monoclinic_al;
            else
                return CrystalSystem::Triclinic;
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
    std::string string{};

    Site(std::string label, std::string x, std::string y, std::string z, std::string atom, std::string occ, std::string beq)
        : label{ std::move(label) }, x{ std::move(x) }, y{ std::move(y) }, z{ std::move(z) }, 
        atom{ std::move(atom) }, occ{ std::move(occ) }, beq{ std::move(beq) }, string{ create_string() } {}

    const std::string& to_string() const {
        return string;
    }

private:
    std::string create_string() const {
        return std::format("\t\tsite {0} num_posns 0\tx {1} y {2} z {3} occ {4} {5} beq {6}", label, x, y, z, atom, occ, beq);
    }
};


class Sites {
public:
    Sites(const row::cif::Block& block) {

        std::vector<std::string> labels{ block.get_value("_atom_site_label") };
        std::vector<std::string> xs{ block.get_value("_atom_site_fract_x") };
        std::vector<std::string> ys{ block.get_value("_atom_site_fract_y") };
        std::vector<std::string> zs{ block.get_value("_atom_site_fract_z") };

        
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

private:
    std::vector<Site> m_sites{};
    std::string m_ss{};

public:
    static std::optional<std::vector<std::string>> get_Biso(const row::cif::Block& block) {
        if(!block.has_tag("_atom_site_B_iso_or_equiv"))
            return std::nullopt;

        std::vector<std::string> beq{ block.get_value("_atom_site_B_iso_or_equiv") };
        strip_brackets_i(beq);

        size_t NAs{ 0 };
        std::for_each(beq.begin(), beq.end(), [&NAs](const std::string& b) {if (is_in(b, NA_values)) ++NAs; });

        if (NAs > 0) {
            logger.log(Logger::Level::INFO, std::format("{0} missing Biso values.", NAs));
        }
        return beq;
    }

    static std::optional<std::vector<std::string>> get_Uiso_as_B(const row::cif::Block& block) {
        if (!block.has_tag("_atom_site_U_iso_or_equiv"))
            return std::nullopt;

        std::vector<std::string> ueq{ block.get_value("_atom_site_U_iso_or_equiv") };
        strip_brackets_i(ueq);

        size_t NAs{ 0 };
        std::for_each(ueq.begin(), ueq.end(), [&NAs](const std::string& u) { if (is_in(u, NA_values)) ++NAs;  });

        if (NAs > 0) {
            logger.log(Logger::Level::INFO, std::format("{0} missing Uiso values.", NAs));
        }

        std::transform(ueq.begin(), ueq.end(), ueq.begin(), [](std::string& u) { return std::format("{:.3f}", str_to_v(u) * as_B); });

        return ueq;
    }

    static std::optional<std::vector<std::string>> get_Baniso_as_B(const row::cif::Block& block) {
        if (!block.has_tag("_atom_site_aniso_B_11"))
            return std::nullopt;

        std::vector<double> B11{ str_to_v(block.get_value("_atom_site_aniso_B_11")) };
        std::vector<double> B22{ str_to_v(block.get_value("_atom_site_aniso_B_22")) };
        std::vector<double> B33{ str_to_v(block.get_value("_atom_site_aniso_B_33")) };
        std::vector<double> beq;
        beq.reserve(B11.size());

        for (size_t i{ 0 }; i < B11.size(); ++i) {
            beq.push_back((B11[i] + B22[i] + B33[i]) / 3.0);
        }

        return v_to_str(beq);
    }

    static std::optional<std::vector<std::string>> get_Uaniso_as_B(const row::cif::Block& block) {
        if (!block.has_tag("_atom_site_aniso_U_11"))
            return std::nullopt;

        std::vector<double> U11{ str_to_v(block.get_value("_atom_site_aniso_U_11")) };
        std::vector<double> U22{ str_to_v(block.get_value("_atom_site_aniso_U_22")) };
        std::vector<double> U33{ str_to_v(block.get_value("_atom_site_aniso_U_33")) };
        std::vector<double> ueq;
        ueq.reserve(U11.size());

        for (size_t i{ 0 }; i < U11.size(); ++i) {
            ueq.push_back(as_B * (U11[i] + U22[i] + U33[i]) / 3.0);
        }

        return v_to_str(ueq);
    }

    static std::optional<std::vector<std::string>> get_BEaniso_as_B(const row::cif::Block& block) {
        if (!block.has_tag("_atom_site_aniso_beta_11"))
            return std::nullopt;

        std::vector<double> BE11{ str_to_v(block.get_value("_atom_site_aniso_beta_11")) };
        std::vector<double> BE22{ str_to_v(block.get_value("_atom_site_aniso_beta_22")) };
        std::vector<double> BE33{ str_to_v(block.get_value("_atom_site_aniso_beta_33")) };

        const UnitCellVectors usv = UnitCellVectors(block.get_value("_cell_length_a")[0], 
                                                    block.get_value("_cell_length_b")[0], 
                                                    block.get_value("_cell_length_c")[0],
                                                    block.get_value("_cell_angle_alpha")[0], 
                                                    block.get_value("_cell_angle_beta")[0], 
                                                    block.get_value("_cell_angle_gamma")[0]);

        const double mas{ square_magnitude(usv.as) };
        const double mbs{ square_magnitude(usv.bs) };
        const double mcs{ square_magnitude(usv.cs) };

        std::transform(BE11.begin(), BE11.end(), BE11.begin(), [&mas](double be) { return 4.0 * be / mas; });
        std::transform(BE22.begin(), BE22.end(), BE22.begin(), [&mbs](double be) { return 4.0 * be / mbs; });
        std::transform(BE33.begin(), BE33.end(), BE33.begin(), [&mcs](double be) { return 4.0 * be / mcs; });

        std::vector<double> beeq;
        beeq.reserve(BE11.size());
        for (size_t i{ 0 }; i < BE11.size(); ++i) {
            beeq.push_back((BE11[i] + BE22[i] + BE33[i]) / 3.0);
        }
        
        return v_to_str(beeq);
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
        all_good = all_good && (iso ? block.has_tag("_atom_site_label") : block.has_tag("_atom_site_aniso_label"));

        if (!all_good)
            return dict;

        std::vector<std::string> b_labels = iso ? block.get_value("_atom_site_label") : block.get_value("_atom_site_aniso_label");

        all_good = all_good && (b_values.value().size() == b_labels.size());

        if (!all_good)
            return dict;

        dict.reserve(b_labels.size());
        std::transform(b_labels.begin(), b_labels.end(), b_values.value().begin(), std::inserter(dict, dict.end()), [](const std::string& k, const std::string& v) {  return std::make_pair(k, v); });
        
        static const std::array bad_vals{ "nan", "0.000", ".", "?" };
        std::erase_if(dict, [](const auto& kv) { auto const& [_, val] = kv; return is_in(val, bad_vals); });

        return dict;
    }

    static std::vector<std::string> get_atoms(const row::cif::Block& block) {
        std::vector<std::string> atoms{};
        atoms.reserve(block.get_value("_atom_site_label").size());

        if (block.has_tag("_atom_site_type_symbol")) {
            atoms = fix_atom_types(block.get_value("_atom_site_type_symbol"));
        }
        else {
            logger.log(Logger::Level::WARNING, "Atom types inferred from site labels. Please check for correctness.");
            atoms = labels_to_atoms(block.get_value("_atom_site_label"));
        }

        return pad_column_i(atoms);
    }

    static std::vector<std::string> get_occs(const row::cif::Block& block) {
        std::vector<std::string> occs{};
        occs.reserve(block.get_value("_atom_site_label").size());

        if (block.has_tag("_atom_site_occupancy")) {
            occs = block.get_value("_atom_site_occupancy");
        }
        else {
            logger.log(Logger::Level::WARNING, "No occupancies found. All set to 1.");
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

        const std::vector<std::string>& labels = block.get_value("_atom_site_label");     
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
                    logger.log(Logger::Level::INFO, std::format("beq value for site {0} calculated from isotropic U value.", label));
                else if (beq == beq_types[2])
                    logger.log(Logger::Level::INFO, std::format("beq value for site {0} calculated from anisotropic B values", label));
                else if (beq == beq_types[3])
                    logger.log(Logger::Level::INFO, std::format("beq value for site {0} calculated from anisotropic U values", label));
                else if (beq == beq_types[4])
                    logger.log(Logger::Level::INFO, std::format("beq value for site {0} calculated from anisotropic beta values", label));

                if ((it->second).starts_with('-')) {
                    logger.log(Logger::Level::WARNING, std::format("Negative atomic displacement parameter detected for site {0}! Please check.", label));
                }
                beqs.push_back(it->second);
                break;
            }
            if (!found) {
                logger.log(Logger::Level::WARNING, std::format("beq value missing or zero for site {0}! Default value of '1.' entered.", label));
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
public:
    CrystalStructure(const row::cif::Block& block, std::string source = std::string())
        : is_good{ check_block(block) }, source { std::move(source) }, phase_name{ make_phase_name(block) }, space_group{ make_space_group(block) }, sites{ block }, unitcell{ block }, m_ss{ create_string() } {}

    const std::string& to_string() const {
        return m_ss;
    }

    const std::string& get_source() const {
        return source;
    }

private:
    bool is_good{ false };
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
    bool check_block(const row::cif::Block& block) const {
        if (std::all_of(must_have_tags.cbegin(), must_have_tags.cend(), [&block](const std::string& tag) { return block.has_tag(tag); }))
            return true;
        else 
            throw std::out_of_range(std::format("Block \"{0}\" doesn't contain sufficient information to make a structure.", block.name));
    }


    std::string make_phase_name(const row::cif::Block& block) const {
        std::string name{};
        auto it = std::find_if(phase_name_tags.begin(), phase_name_tags.end(), [&block](const std::string& tag) {return block.has_tag(tag); });
        if (it != phase_name_tags.end()) {
        name = block.get_value(*it)[0];
        }

        replace_i(name, '\'', ' ');
        replace_i(name, '\"', ' ');
        trim_i(name);
        replace_i(name, '\n', '_');
        replace_i(name, '\r', '_');
        
        return name + "_" + block.name;
    }

    std::string make_space_group(const row::cif::Block& block) const {
        std::string sg{};
        auto it = std::find_if(space_group_tags.begin(), space_group_tags.end(), [&block](const std::string& tag) {return block.has_tag(tag); });
        if (it != space_group_tags.end()) {
            sg = block.get_value(*it)[0];
        }

        if (std::all_of(sg.begin(), sg.end(), [](const char& c) { return std::isdigit(c) != 0; })) {
            logger.log(Logger::Level::INFO, "Spacegroup given by number. Check that the SG setting matches that of the atom coordinates.");
        }

        return sg;
    }

    std::string create_string() const{
        std::string s{ "\tstr '" };
        s += source + '\n';
        s += "\t\tphase_name \"" + phase_name + "\"\n";
        s += "\t\tPhase_Density_g_on_cm3( 0)\n";
        s += "\t\tCS_L( ,200)\n";
        s += "\t\tMVW(0,0,0)\n";
        s += "\t\tscale @ 0.0001\n";
        s += unitcell.to_string() + '\n';
        s += "\t\tspace_group \"" + space_group + "\"\n";
        s += sites.to_string() + '\n';
        return s;
    }
};

