#pragma once

#define _SILENCE_CXX20_CISO646_REMOVED_WARNING

#include <string>

#include "sites.hpp"
#include "unitcell.hpp"


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
    CrystalStructure(const row::cif::Block& block, std::string block_name, std::string source = std::string(), bool add_stuff = true);

    const std::string& to_string() const;
    const std::string& get_source() const;
    std::string create_string(bool add_stuff, size_t indent = 1) const;


private:
    bool check_block(const row::cif::Block& block) const;
    std::string make_phase_name(const row::cif::Block& block) const;
    std::string make_space_group(const row::cif::Block& block) const;
};

