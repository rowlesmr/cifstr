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

std::vector<std::string> Sites::get_Beqs(const row::cif::Block& block) noexcept(false)
{
	static const std::array<std::string, 5> beq_types{ "b_iso", "u_iso", "b_aniso", "u_aniso", "be_aniso" };
	std::unordered_map<std::string, std::unordered_map<std::string, std::string>> all_beqs{};
	for (const std::string& beq : beq_types)
	{
		all_beqs[beq] = make_beq_dict(block, beq);
	}

	const std::vector<std::string>& labels = block.getValue("_atom_site_label").getStrings();
	std::vector<std::string> beqs{};
	beqs.reserve(labels.size());

	for (const std::string& label : labels)
	{
		bool found = false;
		for (const std::string& beq : beq_types)
		{
			auto it = all_beqs[beq].find(label);
			if (it == all_beqs[beq].end())
				continue;

			found = true;
			if (beq == beq_types[1])
				std::cout << std::format("beq value for site {0} calculated from isotropic U value.\n", label);
			else if (beq == beq_types[2])
				std::cout << std::format("beq value for site {0} calculated from anisotropic B values.\n", label);
			else if (beq == beq_types[3])
				std::cout << std::format("beq value for site {0} calculated from anisotropic U values.\n", label);
			else if (beq == beq_types[4])
				std::cout << std::format("beq value for site {0} calculated from anisotropic beta values.\n", label);

			if ((it->second).starts_with('-'))
			{
				std::cout << std::format("Negative atomic displacement parameter detected for site {0}! Please check.\n", label);
			}
			beqs.push_back(it->second);
			break;
		}
		if (!found)
		{
			std::cout << std::format("beq value missing or zero for site {0}! Default value of '1.' entered.\n", label);
			beqs.emplace_back("1.");
		}
	}
	return row::util::pad_column_i(beqs);
}


std::string Sites::get_Beq(const row::cif::Block& block, const std::string& label) noexcept(false)
{
	const std::vector<std::string>& labels = block.getValue("_atom_site_label").getStrings();
	auto it = find(labels.begin(), labels.end(), label);
	const size_t label_idx = (it != labels.end()) ? it - labels.begin() : -1;

	std::string r{ "1." };

	if (label_idx != -1)
		throw std::out_of_range(std::format("Site {0} not found.\n", label));

	if (block.contains("_atom_site_B_iso_or_equiv"))
	{
		const std::vector<std::string>& adps{ block.getValue("_atom_site_B_iso_or_equiv").getStrings() };
		if (adps[label_idx] != "." || adps[label_idx] != "?")
		{
			r = row::util::strip_brackets(adps[label_idx]);
			goto return_section;
		}
	}

	if (block.contains("_atom_site_U_iso_or_equiv"))
	{
		const std::vector<std::string>& adps{ block.getValue("_atom_site_U_iso_or_equiv").getStrings() };
		if (adps[label_idx] != "." || adps[label_idx] != "?")
		{
			double d = row::util::stode(adps[label_idx]).first;
			std::cout << std::format("beq value for site {0} calculated from isotropic U value.\n", label);
			r = std::format("{:.3f}", d * as_B);
			goto return_section;
		}
	}

	if (block.contains("_atom_site_aniso_B_11"))
	{
		const std::vector<std::string>& aniso_labels = block.getValue("_atom_site_aniso_label").getStrings();
		auto aniso_it = find(aniso_labels.begin(), aniso_labels.end(), label);
		const size_t aniso_label_idx = (aniso_it != aniso_labels.end()) ? aniso_it - aniso_labels.begin() : -1;
		if (aniso_label_idx != -1)
		{
			const std::vector<std::string>& adps{ block.getValue("_atom_site_aniso_B_11").getStrings() };
			if (adps[aniso_label_idx] != "." || adps[aniso_label_idx] != "?")
			{
				double B11{ block.getValue("_atom_site_aniso_B_11").getDoubles()[aniso_label_idx] };
				double B22{ block.getValue("_atom_site_aniso_B_22").getDoubles()[aniso_label_idx] };
				double B33{ block.getValue("_atom_site_aniso_B_33").getDoubles()[aniso_label_idx] };
				double d{ (B11 + B22 + B33) / 3.0 };
				std::cout << std::format("beq value for site {0} calculated from anisotropic B values.\n", label);
				r = std::format("{:.3f}", d);
				goto return_section;
			}
		}
	}

	if (block.contains("_atom_site_aniso_U_11"))
	{
		const std::vector<std::string>& aniso_labels = block.getValue("_atom_site_aniso_label").getStrings();
		auto aniso_it = find(aniso_labels.begin(), aniso_labels.end(), label);
		const size_t aniso_label_idx = (aniso_it != aniso_labels.end()) ? aniso_it - aniso_labels.begin() : -1;
		if (aniso_label_idx != -1)
		{
			const std::vector<std::string>& adps{ block.getValue("_atom_site_aniso_U_11").getStrings() };
			if (adps[aniso_label_idx] != "." || adps[aniso_label_idx] != "?")
			{
				double U11{ block.getValue("_atom_site_aniso_U_11").getDoubles()[aniso_label_idx] };
				double U22{ block.getValue("_atom_site_aniso_U_22").getDoubles()[aniso_label_idx] };
				double U33{ block.getValue("_atom_site_aniso_U_33").getDoubles()[aniso_label_idx] };
				double d{ (U11 + U22 + U33) / 3.0 };
				std::cout << std::format("beq value for site {0} calculated from anisotropic U values.\n", label);
				r = std::format("{:.3f}", d * as_B);
				goto return_section;
			}
		}
	}

	if (block.contains("_atom_site_aniso_beta_11"))
	{
		const std::vector<std::string>& aniso_labels = block.getValue("_atom_site_aniso_label").getStrings();
		auto aniso_it = find(aniso_labels.begin(), aniso_labels.end(), label);
		const size_t aniso_label_idx = (aniso_it != aniso_labels.end()) ? aniso_it - aniso_labels.begin() : -1;
		if (aniso_label_idx != -1)
		{
			const std::vector<std::string>& adps{ block.getValue("_atom_site_aniso_beta_11").getStrings() };
			if (adps[aniso_label_idx] != "." || adps[aniso_label_idx] != "?")
			{
				const double mas{ usv.as.square_magnitude() };
				const double mbs{ usv.bs.square_magnitude() };
				const double mcs{ usv.cs.square_magnitude() };

				double Be11{ 4.0 * block.getValue("_atom_site_aniso_beta_11").getDoubles()[aniso_label_idx] / mas };
				double Be22{ 4.0 * block.getValue("_atom_site_aniso_beta_22").getDoubles()[aniso_label_idx] / mbs };
				double Be33{ 4.0 * block.getValue("_atom_site_aniso_beta_33").getDoubles()[aniso_label_idx] / mcs };

				double d{ (Be11 + Be22 + Be33) / 3.0 };
				std::cout << std::format("beq value for site {0} calculated from anisotropic beta values.\n", label);
				r = std::format("{:.3f}", d);
				goto return_section;
			}
		}
	}

	std::cout << std::format("beq value missing or zero for site {0}! Default value of '1.' entered.\n", label);

return_section:

	if (r[0] == '-')
		std::cout << std::format("Negative atomic displacement parameter detected for site {0}! Please check.\n", label);

	return r;
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


