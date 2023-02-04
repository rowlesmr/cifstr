#pragma once

#include "vec3.hpp"
#include "row/pdqciflib.hpp"

class UnitCellVectors {
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


