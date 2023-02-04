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
	for (size_t i{ 0 }; i < labels.size(); ++i) {
		m_sites.emplace_back(labels[i], xs[i], ys[i], zs[i], atoms[i], occs[i], beqs[i]);
	}

	m_ss = create_string();
}

const std::string& Sites::to_string() const
{
	return m_ss;
}

std::optional<std::vector<std::string>> Sites::get_Biso(const row::cif::Block& block)
{
	if (!block.contains("_atom_site_B_iso_or_equiv"))
		return std::nullopt;

	std::vector<std::string> beq{ block.getValue("_atom_site_B_iso_or_equiv").getStrings() };
	row::util::strip_brackets_i(beq);

	size_t NAs{ 0 };
	std::for_each(beq.begin(), beq.end(), [&NAs](const std::string& b) {if (row::util::contains(NA_values, b)) ++NAs; });

	if (NAs > 0) {
		std::cout << std::format("{0} missing Biso values.\n", NAs);
	}
	return beq;
}

std::optional<std::vector<std::string>> Sites::get_Uiso_as_B(const row::cif::Block& block)
{
	if (!block.contains("_atom_site_U_iso_or_equiv"))
		return std::nullopt;

	size_t NAs{ 0 };
	const std::vector<std::string>& ueq{ block.getValue("_atom_site_U_iso_or_equiv").getStrings() };
	std::for_each(ueq.begin(), ueq.end(), [&NAs](const std::string& u) { if (row::util::contains(NA_values, u)) ++NAs;  });

	if (NAs > 0) {
		std::cout << std::format("{0} missing Uiso values.\n", NAs);
	}

	const std::vector<double>& ueq_dbl{ block.getValue("_atom_site_U_iso_or_equiv").getDoubles() };
	std::vector<std::string> r{};
	r.resize(ueq_dbl.size());
	std::transform(ueq_dbl.cbegin(), ueq_dbl.cend(), r.begin(), [](double d) { return std::format("{:.3f}", d * as_B); });

	return r;
}

std::optional<std::vector<std::string>> Sites::get_Baniso_as_B(const row::cif::Block& block)
{
	if (!block.contains("_atom_site_aniso_B_11"))
		return std::nullopt;

	std::vector<double> B11{ block.getValue("_atom_site_aniso_B_11").getDoubles() };
	std::vector<double> B22{ block.getValue("_atom_site_aniso_B_22").getDoubles() };
	std::vector<double> B33{ block.getValue("_atom_site_aniso_B_33").getDoubles() };
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

std::optional<std::vector<std::string>> Sites::get_Uaniso_as_B(const row::cif::Block& block)
{
	if (!block.contains("_atom_site_aniso_U_11"))
		return std::nullopt;

	std::vector<double> U11{ block.getValue("_atom_site_aniso_U_11").getDoubles() };
	std::vector<double> U22{ block.getValue("_atom_site_aniso_U_22").getDoubles() };
	std::vector<double> U33{ block.getValue("_atom_site_aniso_U_33").getDoubles() };
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

std::optional<std::vector<std::string>> Sites::get_BEaniso_as_B(const row::cif::Block& block)
{
	if (!block.contains("_atom_site_aniso_beta_11"))
		return std::nullopt;

	std::vector<double> BE11{ block.getValue("_atom_site_aniso_beta_11").getDoubles() };
	std::vector<double> BE22{ block.getValue("_atom_site_aniso_beta_22").getDoubles() };
	std::vector<double> BE33{ block.getValue("_atom_site_aniso_beta_33").getDoubles() };

	const UnitCellVectors usv = UnitCellVectors(block.getValue("_cell_length_a").getDoubles()[0],
		block.getValue("_cell_length_b").getDoubles()[0],
		block.getValue("_cell_length_c").getDoubles()[0],
		block.getValue("_cell_angle_alpha").getDoubles()[0],
		block.getValue("_cell_angle_beta").getDoubles()[0],
		block.getValue("_cell_angle_gamma").getDoubles()[0]);

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

std::unordered_map<std::string, std::string> Sites::make_beq_dict(const row::cif::Block& block, const std::string& b_type)
{
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

	std::vector<std::string> b_labels = iso ? block.getValue("_atom_site_label").getStrings() : block.getValue("_atom_site_aniso_label").getStrings();

	all_good = all_good && (b_values.value().size() == b_labels.size());

	if (!all_good)
		return dict;

	dict.reserve(b_labels.size());
	std::transform(b_labels.cbegin(), b_labels.cend(), b_values.value().begin(), std::inserter(dict, dict.end()),
		[](const std::string& k, const std::string& v) {  return std::make_pair(k, v); });

	static const std::array bad_vals{ "nan", "0.000", ".", "?" };
	std::erase_if(dict, [](const auto& kv) { auto const& [_, val] = kv; return row::util::contains(bad_vals, val); });

	return dict;
}

std::vector<std::string> Sites::get_atoms(const row::cif::Block& block)
{
	auto initialiser = [&] {
		if (block.contains("_atom_site_type_symbol")) {
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
	auto initialiser = [&] {
		if (block.contains("_atom_site_occupancy")) {
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
	for (const std::string& beq : beq_types) {
		all_beqs[beq] = make_beq_dict(block, beq);
	}

	const std::vector<std::string>& labels = block.getValue("_atom_site_label").getStrings();
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
				std::cout << std::format("beq value for site {0} calculated from isotropic U value.\n", label);
			else if (beq == beq_types[2])
				std::cout << std::format("beq value for site {0} calculated from anisotropic B values.\n", label);
			else if (beq == beq_types[3])
				std::cout << std::format("beq value for site {0} calculated from anisotropic U values.\n", label);
			else if (beq == beq_types[4])
				std::cout << std::format("beq value for site {0} calculated from anisotropic beta values.\n", label);

			if ((it->second).starts_with('-')) {
				std::cout << std::format("Negative atomic displacement parameter detected for site {0}! Please check.\n", label);
			}
			beqs.push_back(it->second);
			break;
		}
		if (!found) {
			std::cout << std::format("beq value missing or zero for site {0}! Default value of '1.' entered.\n", label);
			beqs.emplace_back("1.");
		}
	}
	return row::util::pad_column_i(beqs);
}

std::string Sites::create_string() const
{
	std::string s{};
	for (const Site& site : m_sites) {
		s += site.to_string() + '\n';
	}
	return s;
}


