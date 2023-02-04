#include "crystal_structure.hpp"

CrystalStructure::CrystalStructure(const row::cif::Block& block, std::string block_name, std::string source /*= std::string()*/, bool add_stuff /*= true*/) 
	: is_good{ check_block(block) }, block_name{ std::move(block_name) }, source{ std::move(source) },
	phase_name{ make_phase_name(block) }, space_group{ make_space_group(block) }, sites{ block }, unitcell{ block },
	m_ss{ create_string(add_stuff) }
{

}

const std::string& CrystalStructure::to_string() const
{
	return m_ss;
}

const std::string& CrystalStructure::get_source() const
{
	return source;
}

std::string CrystalStructure::create_string(bool add_stuff, size_t indent /*= 1*/) const
{
	std::string tab(indent, '\t');
	std::string tabs(indent + 1, '\t');

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

bool CrystalStructure::check_block(const row::cif::Block& block) const
{
	if (std::all_of(must_have_tags.cbegin(), must_have_tags.cend(), [&block](const std::string& tag) { return block.contains(tag); }) &&
		std::any_of(space_group_tags.cbegin(), space_group_tags.cend(), [&block](const std::string& tag) { return block.contains(tag); })) {
		return true;
	}
	throw std::out_of_range(std::format("Block \"{0}\" doesn't contain sufficient information to make a structure.", block_name));
}

std::string CrystalStructure::make_phase_name(const row::cif::Block& block) const
{
	auto initialiser = [&] {
		auto it = std::find_if(phase_name_tags.begin(), phase_name_tags.end(), [&block](const std::string& tag){ return block.contains(tag); });
		if (it != phase_name_tags.end()) {
			return block.getValue(*it).getStrings()[0];
		}
		return std::string{ "" };
	};

	std::string name{ initialiser() };
	row::util::replace_i(name, '\'', ' ');
	row::util::replace_i(name, '\"', ' ');
	row::util::trim_i(name);
	row::util::replace_i(name, '\n', '_');
	row::util::replace_i(name, '\r', '_');

	return name + "_" + block_name;
}

std::string CrystalStructure::make_space_group(const row::cif::Block& block) const
{
	auto initialiser = [&] {
		auto it = std::find_if(space_group_tags.begin(), space_group_tags.end(), [&block](const std::string& tag) {return block.contains(tag); });
		if (it != space_group_tags.end()) {
			return block.getValue(*it).getStrings()[0];
		}
		return std::string{ "" };
	};

	std::string sg{ initialiser() };

	if (std::all_of(sg.begin(), sg.end(), [](const char& c) { return std::isdigit(c) != 0; })) {
		std::cout << "Space group given by number. Check that the SG setting matches that of the atom coordinates.\n";
	}

	return sg;
}


