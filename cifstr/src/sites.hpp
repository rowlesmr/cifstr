#pragma once
#define _SILENCE_CXX20_CISO646_REMOVED_WARNING

#include <string>

#include "ctre/ctre.hpp"

#include "row/pdqciflib.hpp"
#include "util.hpp"
#include "unitcell.hpp"




inline std::string& fix_atom_type_i(std::string& atom)
{
	auto m = ctre::match<"([A-Za-z]{1,2})(\\d{0,}\\.?\\d{0,})([+\\-]{0,1})(\\d{0,2})">(atom);

	std::string symbol{ m.get<1>().to_string() };
	std::string charge{ m.get<2>().to_string() };
	std::string sign{ m.get<3>().to_string() };
	std::string digit{ m.get<4>().to_string() };

	if (charge == "0")
	{ // eg  "Si0+" is a valid symbol, but it is a neutral atom
		charge = "";
		sign = "";
	}

	if (sign.size() == 1 && charge.empty())
	{ //check for a single sign with no charge. eg F-. Needs to return F-1
		if (digit.size() == 1)
			charge = "1";
		else //the atom was probably the right way around to begin with
			return atom;
	}

	std::string new_atom = symbol + sign + charge;

	if (row::util::contains(allowed_atoms, new_atom))
	{
		atom = new_atom;
		return atom;
	}

	std::cout << std::format("{0} is not a legal TOPAS scattering factor. Atom replaced with {1}.\n", new_atom, symbol);
	atom = symbol;

	return atom;
}

inline std::vector<std::string>& fix_atom_types_i(std::vector<std::string>& atoms)
{
	std::for_each(atoms.begin(), atoms.end(), [](std::string& a) { fix_atom_type_i(a); });
	return atoms;
}

[[nodiscard]] inline std::vector<std::string> fix_atom_types(std::vector<std::string> atoms)
{
	fix_atom_types_i(atoms);
	return atoms;
}

inline std::string& label_to_atom_i(std::string& label)
{
	if (row::util::contains(water, label.substr(0, 3)))
	{
		std::cout << std::format("Site label '{0}' probably means 'water'. Please check that this atom really is oxygen.\n", label);
		label = "O";
		return label;
	}
	if (row::util::contains(elements, label.substr(0, 2)))
	{
		label = label.substr(0, 2);
		return label;
	}
	if (row::util::contains(elements, label.substr(0, 1)))
	{
		std::string tmp = label.substr(0, 1);
		if (tmp == "W")
			std::cout << std::format("W detected for site '{0}'. Do you mean oxygen from a water molecule or tungsten? Please check.\n", label);
		label = tmp;
		return label;
	}

	std::cout << std::format("Can't decide what atom the site label '{0}' should be. Please check it.\n", label);
	return label;
}

inline std::vector<std::string>& labels_to_atoms_i(std::vector<std::string>& labels)
{
	std::for_each(labels.begin(), labels.end(), [](std::string& label) { label_to_atom_i(label); });
	return labels;
}

[[nodiscard]] inline std::vector<std::string> labels_to_atoms(std::vector<std::string> labels)
{
	labels_to_atoms_i(labels);
	return labels;
}

inline std::string& make_frac_i(std::string& coord, const std::string_view label = "")
{
	std::string r{};
	if (auto m1 = ctre::match<"^([+\\-]?)0?\\.16{2,}[67]$">(coord))
	{
		r = std::format("={0}1/6;", m1.get<1>().to_string());
	}
	else if (auto m2 = ctre::match<"^([+\\-]?)0?\\.3{4,}$">(coord))
	{
		r = std::format("={0}1/3;", m2.get<1>().to_string());
	}
	else if (auto m3 = ctre::match<"^([+\\-]?)0?\\.6{3,}[67]$">(coord))
	{
		r = std::format("={0}2/3;", m3.get<1>().to_string());
	}
	else if (auto m4 = ctre::match<"^([+\\-]?)0?\\.83{3,}$">(coord))
	{
		r = std::format("={0}5/6;", m4.get<1>().to_string());
	}

	if (!r.empty())
	{
		if (label.empty())
		{
			std::cout << std::format("Atomic site coordinate '{0}' replaced by '{1}'.\n", coord, r);
		}
		else
		{
			std::cout << std::format("Atomic fractional coordinate '{0}' replaced by '{1}' for site {2}.\n", coord, r, label);
		}
		coord = r;
	}
	return coord;
}

inline std::vector<std::string>& make_frac_i(std::vector<std::string>& v)
{
	std::for_each(v.begin(), v.end(), [](std::string& s) { make_frac_i(s); });
	return v;
}

inline std::vector<std::string>& make_frac_i(std::vector<std::string>& coords, const std::vector<std::string>& labels)
{
	for (size_t i{ 0 }; i < coords.size(); ++i)
	{
		make_frac_i(coords[i], labels[i]);
	}
	return coords;
}

[[nodiscard]] inline std::vector<std::string> make_frac(std::vector<std::string> v)
{
	make_frac_i(v);
	return v;
}

[[nodiscard]] inline std::vector<std::string> make_frac(std::vector<std::string> coords, const std::vector<std::string>& labels)
{
	make_frac_i(coords, labels);
	return coords;
}





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
public:
    Sites(const row::cif::Block& block);

    const std::string& to_string() const;

private:
    std::vector<Site> m_sites{};
    std::string m_ss{};

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


