#include "cifstr.hpp"



std::string& fix_atom_type_i(std::string& atom)
{
	auto m = ctre::match<"([A-Za-z]{1,2})(\\d{0,}\\.?\\d{0,})([+\\-]{0,1})(\\d{0,2})">(atom);

	std::string symbol{ m.get<1>().to_string() };
	std::string charge{ m.get<2>().to_string() };
	std::string sign{ m.get<3>().to_string() };
	std::string digit{ m.get<4>().to_string() };

	if (charge == "0") { // eg  "Si0+" is a valid symbol, but it is a neutral atom
		charge = "";
		sign = "";
	}

	if (sign.size() == 1 && charge.empty()) { //check for a single sign with no charge. eg F-. Needs to return F-1
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

std::vector<std::string>& fix_atom_types_i(std::vector<std::string>& atoms)
{
	std::for_each(atoms.begin(), atoms.end(), [](std::string& a) { fix_atom_type_i(a); });
	return atoms;
}

std::vector<std::string> fix_atom_types(std::vector<std::string> atoms)
{
	fix_atom_types_i(atoms);
	return atoms;
}

std::string& label_to_atom_i(std::string& label)
{
	if (contains(water, label.substr(0, 3))) {
		logger.log(Logger::Verbosity::SOME, std::format("Site label '{0}' probably means 'water'. Please check that this atom really is oxygen.", label));
		label = "O";
		return label;
	}
	if (contains(elements, label.substr(0, 2))) {
		label = label.substr(0, 2);
		return label;
	}
	if (contains(elements, label.substr(0, 1))) {
		std::string tmp = label.substr(0, 1);
		if (tmp == "W")
			logger.log(Logger::Verbosity::SOME, std::format("W detected for site '{0}'. Do you mean oxygen from a water molecule or tungsten? Please check.", label));
		label = tmp;
		return label;
	}

	logger.log(Logger::Verbosity::SOME, std::format("Can't decide what atom the site label '{0}' should be. Please check it.", label));
	return label;
}

std::vector<std::string>& labels_to_atoms_i(std::vector<std::string>& labels)
{
	std::for_each(labels.begin(), labels.end(), [](std::string& label) { label_to_atom_i(label); });
	return labels;
}

std::vector<std::string> labels_to_atoms(std::vector<std::string> labels)
{
	labels_to_atoms_i(labels);
	return labels;
}

std::string& make_frac_i(std::string& coord, const std::string_view label/*=""*/)
{
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

	if (!r.empty()) {
		if (label.empty()) {
			logger.log(Logger::Verbosity::ALL, std::format("Atomic site coordinate '{0}' replaced by '{1}'.", coord, r));
		}
		else {
			logger.log(Logger::Verbosity::ALL, std::format("Atomic fractional coordinate '{0}' replaced by '{1}' for site {2}.", coord, r, label));
		}
		coord = r;
	}
	return coord;
}

std::vector<std::string>& make_frac_i(std::vector<std::string>& v)
{
	std::for_each(v.begin(), v.end(), [](std::string& s) { make_frac_i(s); });
	return v;
}

std::vector<std::string>& make_frac_i(std::vector<std::string>& coords, const std::vector<std::string>& labels)
{
	for (size_t i{ 0 }; i < coords.size(); ++i) {
		make_frac_i(coords[i], labels[i]);
	}
	return coords;
}

std::vector<std::string> make_frac(std::vector<std::string> v)
{
	make_frac_i(v);
	return v;
}

std::vector<std::string> make_frac(std::vector<std::string> coords, const std::vector<std::string>& labels)
{
	make_frac_i(coords, labels);
	return coords;
}

double triple_product(Vec3 a, Vec3 b, Vec3 c)
{
	return a.dot_product(b.cross_product(c));
}

const Vec3 operator*(double scalar, const Vec3& vec)
{
	return Vec3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
}

const Vec3 operator*(const Vec3& vec, double scalar)
{
	return scalar * vec;
}

const Vec3 operator/(const Vec3& vec, double scalar)
{
	return Vec3(vec.x / scalar, vec.y / scalar, vec.z / scalar);
}

Vec3::Vec3(double u /*= 0.0*/, double v /*= 0.0*/, double w /*= 0.0*/) : x{ u }, y{ v }, z{ w }
{

}

Vec3 Vec3::cross_product(const Vec3& other) const
{
	return {y * other.z - z * other.y,
		    z * other.x - x * other.z,
		    x * other.y - y * other.x};
}

Vec3 Vec3::cross(const Vec3& other) const
{
	return cross_product(other);
}

double Vec3::dot_product(const Vec3& other) const
{
	return x * other.x + y * other.y + z * other.z;
}

double Vec3::dot(const Vec3& other) const
{
	return dot_product(other);
}

double Vec3::square_magnitude() const
{
	return x * x + y * y + z * z;
}

double Vec3::magnitude() const
{
	return std::sqrt(square_magnitude());
}

const Vec3 Vec3::operator+(const Vec3& other) const
{
	return Vec3(x + other.x, y + other.y, z + other.z);
}

const Vec3 Vec3::operator-(const Vec3& other) const
{
	return Vec3(x - other.x, y - other.y, z - other.z);
}

UnitCellVectors::UnitCellVectors(double av, double bv, double cv, double alv, double bev, double gav)
{
	// taken from http://gisaxs.com/index.php/Unit_cell
	a = Vec3(av);
	b = Vec3(bv * std::cos(gav * deg), bv * std::sin(gav * deg));
	double n = (std::cos(alv * deg) - (std::cos(gav * deg) * std::cos(bev * deg))) / std::sin(gav * deg);
	c = Vec3(cv * std::cos(bev * deg), cv * n, cv * std::sqrt(1 - pow(std::cos(bev * deg),2) - pow(n,2)));

	double inv_vol{ 1 / triple_product(a, b, c) };

	Vec3 cas = b.cross_product(c);
	Vec3 cbs = c.cross_product(a);
	Vec3 ccs = a.cross_product(b);

	as = Vec3(cas.x, cas.y, cas.z) * inv_vol;
	bs = Vec3(cbs.x, cbs.y, cbs.z) * inv_vol;
	cs = Vec3(ccs.x, ccs.y, ccs.z) * inv_vol;
}

UnitCell::UnitCell(const row::cif::Block& block)
{
	a_s = strip_brackets(block.getValue("_cell_length_a").getStrings()[0]);
	b_s = strip_brackets(block.getValue("_cell_length_b").getStrings()[0]);
	c_s = strip_brackets(block.getValue("_cell_length_c").getStrings()[0]);
	al_s = strip_brackets(block.getValue("_cell_angle_alpha").getStrings()[0]);
	be_s = strip_brackets(block.getValue("_cell_angle_beta").getStrings()[0]);
	ga_s = strip_brackets(block.getValue("_cell_angle_gamma").getStrings()[0]);

	a = block.getValue("_cell_length_a").getDoubles()[0];
	b = block.getValue("_cell_length_b").getDoubles()[0];
	c = block.getValue("_cell_length_c").getDoubles()[0];
	al = block.getValue("_cell_angle_alpha").getDoubles()[0];
	be = block.getValue("_cell_angle_beta").getDoubles()[0];
	ga = block.getValue("_cell_angle_gamma").getDoubles()[0];

	crystal_system = deduce_symmetry();
	usv = UnitCellVectors(a, b, c, al, be, ga);
}

UnitCell::UnitCell(std::string av, std::string bv, std::string cv, std::string alv, std::string bev, std::string gav) : a_s{ strip_brackets(std::move(av)) }, b_s{ strip_brackets(std::move(bv)) }, c_s{ strip_brackets(std::move(cv)) },
al_s{ strip_brackets(std::move(alv)) }, be_s{ strip_brackets(std::move(bev)) }, ga_s{ strip_brackets(std::move(gav)) },
a{ stode(a_s).first }, b{ stode(b_s).first }, c{ stode(c_s).first }, al{ stode(al_s).first }, be{ stode(be_s).first }, ga{ stode(ga_s).first },
crystal_system{ deduce_symmetry() }, usv{ UnitCellVectors(a,b,c,al,be,ga) }
{

}

const UnitCellVectors& UnitCell::get_unitcellvectors() const
{
	return usv;
}

std::string UnitCell::to_string(size_t indent /*= 2*/) const
{
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

CrystalSystem UnitCell::deduce_symmetry() const
{
	if (util::all_equal({ a,b,c }) && util::all_equal({ al,be,ga })) {
		if (util::are_equal(al, 90.0)) {
			return CrystalSystem::Cubic;
		}
		return CrystalSystem::Rhombohedral;
	}
	if (util::all_equal({ a,b }) && util::all_equal({ al,be,90.0 })) {
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
	if (util::all_equal({ al,be,90.0 }) && !util::all_equal({ ga,90.0 })) {
		return CrystalSystem::Monoclinic_ga;
	}
	if (util::all_equal({ ga,be,90.0 }) && !util::all_equal({ al,90.0 })) {
		return CrystalSystem::Monoclinic_al;
	}
	return CrystalSystem::Triclinic;
}

Site::Site(std::string t_label, std::string t_x, std::string t_y, std::string t_z, std::string t_atom, std::string t_occ, std::string t_beq) : label{ std::move(t_label) }, x{ std::move(t_x) }, y{ std::move(t_y) }, z{ std::move(t_z) },
atom{ std::move(t_atom) }, occ{ std::move(t_occ) }, beq{ std::move(t_beq) }
{
	std::replace(label.begin(), label.end(), '\'', 'p'); //can't contain a "'", as this is a comment character in TOPAS
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

	pad_column_i(strip_brackets_i(make_frac_i(xs, labels)));
	pad_column_i(strip_brackets_i(make_frac_i(ys, labels)));
	pad_column_i(strip_brackets_i(make_frac_i(zs, labels)));
	pad_column_i(labels);

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
	strip_brackets_i(beq);

	size_t NAs{ 0 };
	std::for_each(beq.begin(), beq.end(), [&NAs](const std::string& b) {if (contains(NA_values, b)) ++NAs; });

	if (NAs > 0) {
		logger.log(Logger::Verbosity::ALL, std::format("{0} missing Biso values.", NAs));
	}
	return beq;
}

std::optional<std::vector<std::string>> Sites::get_Uiso_as_B(const row::cif::Block& block)
{
	if (!block.contains("_atom_site_U_iso_or_equiv"))
		return std::nullopt;

	size_t NAs{ 0 };
	const std::vector<std::string>& ueq{ block.getValue("_atom_site_U_iso_or_equiv").getStrings() };
	std::for_each(ueq.begin(), ueq.end(), [&NAs](const std::string& u) { if (contains(NA_values, u)) ++NAs;  });

	if (NAs > 0) {
		logger.log(Logger::Verbosity::ALL, std::format("{0} missing Uiso values.", NAs));
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
	std::erase_if(dict, [](const auto& kv) { auto const& [_, val] = kv; return contains(bad_vals, val); });

	return dict;
}

std::vector<std::string> Sites::get_atoms(const row::cif::Block& block)
{
	auto initialiser = [&] {
		if (block.contains("_atom_site_type_symbol")) {
			return fix_atom_types(block.getValue("_atom_site_type_symbol").getStrings());
		}
		logger.log(Logger::Verbosity::SOME, "Atom types inferred from site labels. Please check for correctness.");
		return labels_to_atoms(block.getValue("_atom_site_label").getStrings());
	};

	std::vector<std::string> atoms{ initialiser() };
	return pad_column_i(atoms);
}

std::vector<std::string> Sites::get_occs(const row::cif::Block& block)
{
	auto initialiser = [&] {
		if (block.contains("_atom_site_occupancy")) {
			return block.getValue("_atom_site_occupancy").getStrings();
		}
		logger.log(Logger::Verbosity::SOME, "No occupancies found. All set to 1.");
		return std::vector<std::string>(block.getValue("_atom_site_label").size(), std::string{ "1." });
	};

	std::vector<std::string> occs{ initialiser() };
	return pad_column_i(strip_brackets_i(occs));
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
			beqs.emplace_back("1.");
		}
	}
	return pad_column_i(beqs);
}

std::string Sites::create_string() const
{
	std::string s{};
	for (const Site& site : m_sites) {
		s += site.to_string() + '\n';
	}
	return s;
}

CrystalStructure::CrystalStructure(const row::cif::Block& block, std::string block_name, std::string source /*= std::string()*/, int verbosity /*= 1*/, bool add_stuff /*= true*/) 
	: is_good{ check_block(block, verbosity) }, block_name{ std::move(block_name) }, source{ std::move(source) },
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

bool CrystalStructure::check_block(const row::cif::Block& block, int verbosity) const
{
	logger.verbosity = static_cast<Logger::Verbosity>(verbosity);
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
	replace_i(name, '\'', ' ');
	replace_i(name, '\"', ' ');
	trim_i(name);
	replace_i(name, '\n', '_');
	replace_i(name, '\r', '_');

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
		logger.log(Logger::Verbosity::SOME, "Space group given by number. Check that the SG setting matches that of the atom coordinates.");
	}

	return sg;
}
