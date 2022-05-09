

#ifndef ROW_STR_FILE_
#define ROW_STR_FILE_
#include "util.h"
#include "cif.h"


struct Vec3 {
public:
    double x;
    double y;
    double z;

    Vec3(double u = 0.0, double v = 0.0, double w = 0.0);
};

Vec3 cross_product(Vec3 a, Vec3 b);
double dot_product(Vec3 a, Vec3 b);
double triple_product(Vec3 a, Vec3 b, Vec3 c);
double square_magnitude(Vec3 a);
double magnitude(Vec3 a);


struct UnitCellVectors {
public:
    Vec3 a;
    Vec3 b;
    Vec3 c;
    Vec3 as;
    Vec3 bs;
    Vec3 cs;

    UnitCellVectors();
    UnitCellVectors(double av, double bv, double cv, double alv, double bev, double gav);
    UnitCellVectors(std::string av, std::string bv, std::string cv, std::string alv, std::string bev, std::string gav);
};


enum class CrystalSystem
{
    Triclinic, Monoclinic_al, Monoclinic_be, Monoclinic_ga, Orthorhombic, Tetragonal, Hexagonal, Rhombohedral, Cubic
};


class UnitCell {
public:
    UnitCell(std::string av, std::string bv, std::string cv, std::string alv, std::string bev, std::string gav);
    UnitCell(const row::cif::Block& block);

    const std::stringstream& to_stringstream() const;
    const UnitCellVectors& get_unitcellvectors() const;

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

    std::stringstream m_ss{};

    std::stringstream create_stringstream();
    CrystalSystem deduce_crystal_system();
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
    std::stringstream ss{};

    Site(std::string label, std::string x, std::string y, std::string z, std::string atom, std::string occ, std::string beq);

private:
    std::stringstream create_stringstream();
};


class Sites {

public:
    Sites(const row::cif::Block& block);

    const std::stringstream& to_stringstream() const;

private:
    std::vector<Site> m_sites{};
    std::stringstream m_ss{};

    static std::vector<std::string> get_Biso(const row::cif::Block& block)  noexcept(false);
    static std::vector<std::string> get_Uiso_as_B(const row::cif::Block& block)  noexcept(false);
    static std::vector<std::string> get_Baniso_as_B(const row::cif::Block& block)  noexcept(false);
    static std::vector<std::string> get_Uaniso_as_B(const row::cif::Block& block)  noexcept(false);
    static std::vector<std::string> get_BEaniso_as_B(const row::cif::Block& block)  noexcept(false);
    static std::unordered_map<std::string, std::string> make_beq_dict(const row::cif::Block& block, const std::string& b_type) noexcept(false);
    static std::vector<std::string> get_atoms(const row::cif::Block& block) noexcept;
    static std::vector<std::string> get_occs(const row::cif::Block& block) noexcept;
    static std::vector<std::string> get_Beqs(const row::cif::Block& block) noexcept(false);
    void create_stringstream();
};


class CrystalStructure {
public:
    CrystalStructure(const row::cif::Block& block);
    const std::stringstream& to_stringstream() const;

private:
    std::string phase_name{};
    std::string space_group{};
    Sites sites;
    UnitCell unitcell;
    std::stringstream m_ss{};

    static constexpr std::array phase_name_tags{ "_pd_phase_name", "_chemical_name_mineral", "_chemical_name_common", "_chemical_name_systematic", "_chemical_name_structure_type" };
    static constexpr std::array space_group_tags{ "_symmetry_space_group_name_H-M", "_space_group_name_H-M_alt", "_symmetry_Int_Tables_number", "_space_group_IT_number" };

    std::string make_phase_name(const row::cif::Block& block) const;
    std::string make_space_group(const row::cif::Block& block) const;
    std::stringstream create_stringstream();
};

#endif