#include "sites.hpp"


Site::Site(std::string t_label, std::string t_x, std::string t_y, std::string t_z, std::string t_atom, std::string t_occ, std::string t_beq)
	: label{ std::move(t_label) }, x{ std::move(t_x) }, y{ std::move(t_y) }, z{ std::move(t_z) },
	atom{ std::move(t_atom) }, occ{ std::move(t_occ) }, beq{ std::move(t_beq) }
{
	std::replace(label.begin(), label.end(), '\'', 'p'); //can't contain a ', as this is a comment character in TOPAS
	std::replace(label.begin(), label.end(), '"', 'q'); //can't contain a ", as this is a quote character in TOPAS
}


std::string Site::to_string(size_t indent /*= 2*/) const
{
	std::string tabs(indent, '\t');
	return std::format("{0}site {1} num_posns 0\tx {2} y {3} z {4} occ {5} {6} beq {7}", tabs, label, x, y, z, atom, occ, beq);
}

Sites::Sites(const row::cif::Block& block)
{
	std::vector<std::string> labels{ block.getValue("_atom_site_label").getStrings() };
	std::vector<std::string> xs{ block.getValue("_atom_site_fract_x").getStrings() };
	std::vector<std::string> ys{ block.getValue("_atom_site_fract_y").getStrings() };
	std::vector<std::string> zs{ block.getValue("_atom_site_fract_z").getStrings() };

	row::util::pad_column_i(row::util::strip_brackets_i(make_frac_i(xs, labels)));
	row::util::pad_column_i(row::util::strip_brackets_i(make_frac_i(ys, labels)));
	row::util::pad_column_i(row::util::strip_brackets_i(make_frac_i(zs, labels)));
	row::util::pad_column_i(labels);

	std::vector<std::string> atoms{ get_atoms(block) };
	std::vector<std::string> occs{ get_occs(block) };
	std::vector<std::string> beqs{ get_Beqs(block) };

	m_sites.reserve(labels.size());
	for (size_t i{ 0 }; i < labels.size(); ++i)
	{
		m_sites.emplace_back(labels[i], xs[i], ys[i], zs[i], atoms[i], occs[i], beqs[i]);
	}

	m_ss = create_string();

	usv = block.contains("_atom_site_aniso_beta_11") ? UnitCellVectors(block.getValue("_cell_length_a").getDoubles()[0],
		block.getValue("_cell_length_b").getDoubles()[0],
		block.getValue("_cell_length_c").getDoubles()[0],
		block.getValue("_cell_angle_alpha").getDoubles()[0],
		block.getValue("_cell_angle_beta").getDoubles()[0],
		block.getValue("_cell_angle_gamma").getDoubles()[0]) : UnitCellVectors();

}

const std::string& Sites::to_string() const
{
	return m_ss;
}


std::vector<std::string> Sites::get_atoms(const row::cif::Block& block)
{
	auto initialiser = [&]
	{
		if (block.contains("_atom_site_type_symbol"))
		{
			return fix_atom_types(block.getValue("_atom_site_type_symbol").getStrings());
		}
		std::cout << "Atom types inferred from site labels. Please check for correctness.\n";
		return labels_to_atoms(block.getValue("_atom_site_label").getStrings());
	};

	std::vector<std::string> atoms{ initialiser() };
	return row::util::pad_column_i(atoms);
}

std::vector<std::string> Sites::get_occs(const row::cif::Block& block)
{
	auto initialiser = [&]
	{
		if (block.contains("_atom_site_occupancy"))
		{
			return block.getValue("_atom_site_occupancy").getStrings();
		}
		std::cout << "No occupancies found. All set to 1.\n";
		return std::vector<std::string>(block.getValue("_atom_site_label").size(), std::string{ "1." });
	};

	std::vector<std::string> occs{ initialiser() };
	return row::util::pad_column_i(row::util::strip_brackets_i(occs));
}


bool is_valid_value(const std::string& value)
{
	return !(value == "." || value == "?");
}


size_t valid_aniso_info_index(const row::cif::Block& block, const std::string_view tag, const std::vector<std::string>& atom_labels, const int atom_label_num)
{
	if (!block.contains(tag))
		return std::numeric_limits<size_t>::max();

	const auto& aniso_labels{ block.getValue("_atom_site_aniso_label").getStrings() };
	auto it = find(aniso_labels.begin(), aniso_labels.end(), atom_labels[atom_label_num]);
	size_t r{ it - aniso_labels.begin() };

	if (r == aniso_labels.size())
		return std::numeric_limits<size_t>::max();

	if (is_valid_value(block.getValue(tag).getStrings()[r]))
		return r;

	return std::numeric_limits<size_t>::max();
}


std::vector<std::string> Sites::get_Beqs(const row::cif::Block& block)
{
	const std::vector<std::string>& labels = block.getValue("_atom_site_label").getStrings();
	std::vector<std::string> beqs{};
	beqs.reserve(labels.size());
	std::string r{ "1." };

	auto it{ labels.begin() };
	for (size_t i{ 0 }; i < labels.size(); ++i)
	{
		size_t aniso_label_idx{ 0 };

		if (block.contains("_atom_site_B_iso_or_equiv") && is_valid_value(block.getValue("_atom_site_B_iso_or_equiv").getStrings()[i]))
		{
			r = row::util::strip_brackets(block.getValue("_atom_site_B_iso_or_equiv").getStrings()[i]);
		}
		else if (block.contains("_atom_site_U_iso_or_equiv") && is_valid_value(block.getValue("_atom_site_U_iso_or_equiv").getStrings()[i]))
		{
			double d = block.getValue("_atom_site_U_iso_or_equiv").getDoubles()[i];
			r = std::format("{:.3f}", d * as_B);
			std::cout << std::format("beq value for site {0} calculated from isotropic U value.\n", labels[i]);
		}
		else if ((aniso_label_idx = valid_aniso_info_index(block, "_atom_site_aniso_B_11", labels, i)) != std::numeric_limits<size_t>::max())
		{
			double B11{ block.getValue("_atom_site_aniso_B_11").getDoubles()[aniso_label_idx] };
			double B22{ block.getValue("_atom_site_aniso_B_22").getDoubles()[aniso_label_idx] };
			double B33{ block.getValue("_atom_site_aniso_B_33").getDoubles()[aniso_label_idx] };
			double d{ (B11 + B22 + B33) / 3.0 };
			r = std::format("{:.3f}", d);
			std::cout << std::format("beq value for site {0} calculated from anisotropic B values.\n", labels[i]);
		}
		else if ((aniso_label_idx = valid_aniso_info_index(block, "_atom_site_aniso_U_11", labels, i)) != std::numeric_limits<size_t>::max())
		{
			const size_t aniso_label_idx = it - block.getValue("_atom_site_aniso_label").getStrings().begin();
			double B11{ block.getValue("_atom_site_aniso_U_11").getDoubles()[aniso_label_idx] };
			double B22{ block.getValue("_atom_site_aniso_U_22").getDoubles()[aniso_label_idx] };
			double B33{ block.getValue("_atom_site_aniso_U_33").getDoubles()[aniso_label_idx] };
			double d{ (B11 + B22 + B33) / 3.0 };
			r = std::format("{:.3f}", d * as_B);
			std::cout << std::format("beq value for site {0} calculated from anisotropic U values.\n", labels[i]);
		}
		else if ((aniso_label_idx = valid_aniso_info_index(block, "_atom_site_aniso_beta_11", labels, i)) != std::numeric_limits<size_t>::max())
		{
			const size_t aniso_label_idx = it - block.getValue("_atom_site_aniso_label").getStrings().begin();
			const double mas{ usv.as.square_magnitude() };
			const double mbs{ usv.bs.square_magnitude() };
			const double mcs{ usv.cs.square_magnitude() };

			double B11{ 4.0 * block.getValue("_atom_site_aniso_beta_11").getDoubles()[aniso_label_idx] / mas };
			double B22{ 4.0 * block.getValue("_atom_site_aniso_beta_22").getDoubles()[aniso_label_idx] / mbs };
			double B33{ 4.0 * block.getValue("_atom_site_aniso_beta_33").getDoubles()[aniso_label_idx] / mcs };
			double d{ (B11 + B22 + B33) / 3.0 };
			r = std::format("{:.3f}", d);
			std::cout << std::format("beq value for site {0} calculated from anisotropic beta values.\n", labels[i]);
		}
		else
		{
			std::cout << std::format("beq value missing or zero for site {0}! Default value of '1.' entered.\n", labels[i]);
		}

		if (r[0] == '-')
			std::cout << std::format("Negative atomic displacement parameter detected for site {0}! Please check.\n", labels[i]);
		beqs.push_back(r);
	}

	return row::util::pad_column_i(beqs);
}


std::string Sites::create_string() const
{
	std::string s{};
	for (const Site& site : m_sites)
	{
		s += site.to_string() + '\n';
	}
	return s;
}


