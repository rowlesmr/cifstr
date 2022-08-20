#include "ciffile.hpp"



constexpr void row::cif::Datavalue::swap(Datavalue& other)
{
	m_strs.swap(other.m_strs);
	m_dbls.swap(other.m_dbls);
	m_errs.swap(other.m_errs);
	std::swap(m_isConverted, other.m_isConverted);
	return;
}

void row::cif::Datavalue::push_back(std::string&& value)
{
	m_isConverted = false;
	m_strs.push_back(std::forward<std::string>(value));
	return;
}

void row::cif::Datavalue::push_back(const std::string& value)
{
	m_isConverted = false;
	m_strs.push_back(value);
	return;
}

constexpr void row::cif::Datavalue::clear() noexcept
{
	m_strs.clear();
	m_dbls.clear();
	m_errs.clear();
	m_isConverted = false;
	return;
}

constexpr void row::cif::Datavalue::shrink_to_fit()
{
	m_strs.shrink_to_fit();
	m_dbls.shrink_to_fit();
	m_errs.shrink_to_fit();
	return;
}

constexpr row::cif::Datavalue::size_type row::cif::Datavalue::capacity() const noexcept
{
	return m_strs.capacity();
}

constexpr void row::cif::Datavalue::reserve(size_type new_cap)
{
	m_strs.reserve(new_cap);
	m_dbls.reserve(new_cap);
	m_errs.reserve(new_cap);
	return;
}

constexpr row::cif::Datavalue::size_type row::cif::Datavalue::size() const noexcept
{
	return m_strs.size();
}

constexpr bool row::cif::Datavalue::isEmpty() const noexcept
{
	return empty();
}

constexpr bool row::cif::Datavalue::empty() const noexcept
{
	return m_strs.empty();
}

row::cif::Datavalue::const_reverse_iterator_double row::cif::Datavalue::err_crend() const
{
	convert(); 
	return m_errs.crend();
}

row::cif::Datavalue::const_reverse_iterator_double row::cif::Datavalue::err_crbegin() const
{
	convert(); 
	return m_errs.crbegin();
}

row::cif::Datavalue::const_iterator_double row::cif::Datavalue::err_cend() const
{
	convert(); 
	return m_errs.cend();
}

row::cif::Datavalue::const_iterator_double row::cif::Datavalue::err_cbegin() const
{
	convert(); 
	return m_errs.cbegin();
}

row::cif::Datavalue::const_reverse_iterator_double row::cif::Datavalue::err_rend() const
{
	convert(); 
	return m_errs.rend();
}

row::cif::Datavalue::const_reverse_iterator_double row::cif::Datavalue::err_rbegin() const
{
	convert(); 
	return m_errs.rbegin();
}

row::cif::Datavalue::const_iterator_double row::cif::Datavalue::err_end() const
{
	convert(); 
	return m_errs.end();
}

row::cif::Datavalue::const_iterator_double row::cif::Datavalue::err_begin() const
{
	convert(); 
	return m_errs.begin();
}

row::cif::Datavalue::const_reverse_iterator_double row::cif::Datavalue::dbl_crend() const
{
	convert(); 
	return m_dbls.crend();
}

row::cif::Datavalue::const_reverse_iterator_double row::cif::Datavalue::dbl_crbegin() const
{
	convert(); 
	return m_dbls.crbegin();
}

row::cif::Datavalue::const_iterator_double row::cif::Datavalue::dbl_cend() const
{
	convert(); 
	return m_dbls.cend();
}

row::cif::Datavalue::const_iterator_double row::cif::Datavalue::dbl_cbegin() const
{
	convert(); 
	return m_dbls.cbegin();
}

row::cif::Datavalue::const_reverse_iterator_double row::cif::Datavalue::dbl_rend() const
{
	convert(); 
	return m_dbls.rend();
}

row::cif::Datavalue::const_reverse_iterator_double row::cif::Datavalue::dbl_rbegin() const
{
	convert(); 
	return m_dbls.rbegin();
}

row::cif::Datavalue::const_iterator_double row::cif::Datavalue::dbl_end() const
{
	convert(); 
	return m_dbls.end();
}

row::cif::Datavalue::const_iterator_double row::cif::Datavalue::dbl_begin() const
{
	convert(); 
	return m_dbls.begin();
}

row::cif::Datavalue::const_reverse_iterator row::cif::Datavalue::str_crend() const
{
	return crend();
}

row::cif::Datavalue::const_reverse_iterator row::cif::Datavalue::str_crbegin() const
{
	return crbegin();
}

row::cif::Datavalue::const_iterator row::cif::Datavalue::str_cend() const
{
	return cend();
}

row::cif::Datavalue::const_iterator row::cif::Datavalue::str_cbegin() const
{
	return cbegin();
}

row::cif::Datavalue::const_reverse_iterator row::cif::Datavalue::str_rend() const
{
	return rend();
}

row::cif::Datavalue::const_reverse_iterator row::cif::Datavalue::str_rbegin() const
{
	return rbegin();
}

row::cif::Datavalue::const_iterator row::cif::Datavalue::str_end() const
{
	return end();
}

row::cif::Datavalue::const_iterator row::cif::Datavalue::str_begin() const
{
	return begin();
}

row::cif::Datavalue::const_reverse_iterator row::cif::Datavalue::crend() const
{
	return m_strs.crend();
}

row::cif::Datavalue::const_reverse_iterator row::cif::Datavalue::crbegin() const
{
	return m_strs.crbegin();
}

row::cif::Datavalue::const_iterator row::cif::Datavalue::cend() const
{
	return m_strs.cend();
}

row::cif::Datavalue::const_iterator row::cif::Datavalue::cbegin() const
{
	return m_strs.cbegin();
}

row::cif::Datavalue::const_reverse_iterator row::cif::Datavalue::rend() const
{
	return m_strs.rend();
}

row::cif::Datavalue::const_reverse_iterator row::cif::Datavalue::rbegin() const
{
	return m_strs.rbegin();
}

row::cif::Datavalue::const_iterator row::cif::Datavalue::end() const
{
	return m_strs.end();
}

row::cif::Datavalue::const_iterator row::cif::Datavalue::begin() const
{
	return m_strs.begin();
}

const double* row::cif::Datavalue::err_data() const noexcept
{
	convert();
	return m_errs.data();
}

const double* row::cif::Datavalue::dbl_data() const noexcept
{
	convert();
	return m_dbls.data();
}

constexpr const std::string* row::cif::Datavalue::str_data() const noexcept
{
	return data();
}

constexpr const std::string* row::cif::Datavalue::data() const noexcept
{
	return m_strs.data();
}

row::cif::Datavalue::const_reference_double row::cif::Datavalue::back_err() const
{
	convert(); return m_errs.back();
}

row::cif::Datavalue::const_reference_double row::cif::Datavalue::back_dbl() const
{
	convert(); return m_dbls.back();
}

constexpr row::cif::Datavalue::const_reference row::cif::Datavalue::back_str() const
{
	return back();
}

constexpr row::cif::Datavalue::const_reference row::cif::Datavalue::back() const
{
	return m_strs.back();
}

row::cif::Datavalue::const_reference_double row::cif::Datavalue::front_err() const
{
	convert();  
	return m_errs.front();
}

row::cif::Datavalue::const_reference_double row::cif::Datavalue::front_dbl() const
{
	convert(); 
	return m_dbls.front();
}

constexpr row::cif::Datavalue::const_reference row::cif::Datavalue::front_str() const
{
	return front();
}

constexpr row::cif::Datavalue::const_reference row::cif::Datavalue::front() const
{
	return m_strs.front();
}

row::cif::Datavalue::const_reference_double row::cif::Datavalue::err_at(size_type pos) const noexcept(false)
{
	convert();
	return m_errs.at(pos);
}

row::cif::Datavalue::const_reference_double row::cif::Datavalue::dbl_at(size_type pos) const noexcept(false)
{
	convert();
	return m_dbls.at(pos);
}

constexpr row::cif::Datavalue::const_reference row::cif::Datavalue::str_at(size_type pos) const noexcept(false)
{
	return at(pos);
}

constexpr row::cif::Datavalue::const_reference row::cif::Datavalue::at(size_type pos) const noexcept(false)
{
	return m_strs.at(pos);
}

const std::vector<double>& row::cif::Datavalue::getErrors() const
{
	convert();
	return m_errs;
}

const std::vector<double>& row::cif::Datavalue::getDoubles() const
{
	convert();
	return m_dbls;
}

const std::vector<std::string>& row::cif::Datavalue::getStrings() const
{
	return m_strs;
}

bool row::cif::Datavalue::isConverted() const
{
	return m_isConverted;
}

void row::cif::Datavalue::reconvert() const
{
	m_isConverted = false;
	return;
}

bool row::cif::Datavalue::convert() const
{
	if (m_isConverted) {
		return m_isConverted;
	}

	//test the first one. If it passes, assume the rest will.
	// a fully validating parser would test everyone, as well
	// as knowing if the tag associated with the values could
	// be numeric, or a list, etc...
	if (m_strs.size()) {
		auto [val, err] = row::util::stode(m_strs[0]);
		if (val == row::util::NaN && err == row::util::NaN) {
			m_isConverted = false;
			m_dbls.clear();
			m_errs.clear();
			return m_isConverted;
		}
	}
	else {
		m_isConverted = false;
		m_dbls.clear();
		m_errs.clear();
		return m_isConverted;
	}

	m_dbls.clear();
	m_errs.clear();
	m_dbls.reserve(m_strs.size());
	m_errs.reserve(m_strs.size());

	for (const auto& s : m_strs) {
		auto [val, err] = row::util::stode(s);
		m_dbls.push_back(val);
		m_errs.push_back(err);
	}

	m_isConverted = true;
	return m_isConverted;
}

row::cif::Datavalue::Datavalue(std::initializer_list<std::string> in) : m_strs{ std::move(in) }
{

}

row::cif::Datavalue::Datavalue(std::vector<std::string>&& in) : m_strs(std::move(in))
{

}

row::cif::Datavalue::Datavalue(const std::vector<std::string>& in) : m_strs(in)
{

}

row::cif::Datavalue::Datavalue(std::string&& in) : m_strs({ std::move(in) })
{

}

row::cif::Datavalue::Datavalue(const std::string& in) : m_strs({ in })
{

}

row::cif::Datavalue::Datavalue()
{

}

std::pair<const row::cif::dataname, row::cif::Datavalue>* row::cif::Block::constptrToFirstItem() const
{
	itemorder firstItem = m_item_order[0];
	dataname firstTag{};
	if (firstItem.index() == 0) {
		int loopNum = std::get<int>(firstItem);
		firstTag = m_loops.at(loopNum)[0];
	}
	else {
		firstTag = std::get<dataname>(firstItem);
	}
	std::pair<const dataname, Datavalue>* ptr = const_cast<std::pair<const dataname, Datavalue>*>(&(*m_block.find(firstTag)));
	return ptr;
}

std::pair<const row::cif::dataname, row::cif::Datavalue>* row::cif::Block::ptrToFirstItem()
{
	itemorder firstItem = m_item_order[0];
	dataname firstTag{};
	if (firstItem.index() == 0) {
		int loopNum = std::get<int>(firstItem);
		firstTag = m_loops.at(loopNum)[0];
	}
	else {
		firstTag = std::get<dataname>(firstItem);
	}
	return &(*m_block.find(firstTag));
}

bool row::cif::Block::contains(const dataname& tag) const
{
	dataname lowerTag{ row::util::toLower(tag) };
	return m_block.contains(lowerTag);
}

row::cif::Block::const_iterator row::cif::Block::find(const dataname& tag) const
{
	dataname lowerTag{ row::util::toLower(tag) };

	auto it = m_block.find(lowerTag);
	if (it == m_block.end()) {
		return cend();
	}

	return const_iterator(&(*it), this);
}

size_t row::cif::Block::count(const dataname& tag) const
{
	dataname lowerTag{ row::util::toLower(tag) };
	return m_block.count(lowerTag);
}

const row::cif::Datavalue& row::cif::Block::at(const dataname& tag) const
{
	dataname lowerTag{ row::util::toLower(tag) };
	return m_block.at(lowerTag);
}

size_t row::cif::Block::erase(const dataname& tag)
{
	const_iterator r = removeItem(tag);
	if (r == cend())
		return 0;
	else
		return 1;
}

void row::cif::Block::clear() noexcept
{
	m_block.clear();
	m_loops.clear();
	m_item_order.clear();
	m_true_case.clear();
	overwrite = true;
}

size_t row::cif::Block::max_size() const noexcept
{
	return std::min({ m_block.max_size(), m_item_order.max_size(), m_loops.max_size(), m_true_case.max_size() });
}

int row::cif::Block::size(const dataname& tag) const noexcept
{
	if (contains(tag)) {
		return static_cast<int>(m_block.at(tag).size());
	}
	return -1;
}

size_t row::cif::Block::size() const noexcept
{
	return m_block.size();
}

bool row::cif::Block::isEmpty() const noexcept
{
	return empty();
}

bool row::cif::Block::empty() const noexcept
{
	return m_block.empty();
}

row::cif::Block::const_iterator row::cif::Block::cend() const noexcept
{
	return const_iterator(nullptr, this);
}

row::cif::Block::const_iterator row::cif::Block::cbegin() const noexcept
{
	return const_iterator(constptrToFirstItem(), this);
}

row::cif::Block::const_iterator row::cif::Block::end() const noexcept
{
	return const_iterator(nullptr, this);
}

row::cif::Block::const_iterator row::cif::Block::begin() const noexcept
{
	return const_iterator(constptrToFirstItem(), this);
}

std::string row::cif::Block::to_string(bool pretty/*=true*/) const
{
	std::string block{};
	size_t maxTagLen{ 1 }; //must not be zero or std::format throws.

	if (pretty) {
		for (const auto& [k, _] : m_block) {
			maxTagLen = std::max(maxTagLen, k.size());
		}
	}

	for (const auto& item : m_item_order) {
		if (item.index() == 0) { // it's a loop
			int loopNum{ std::get<int>(item) };
			size_t loopLen{ m_block.at(m_loops.at(loopNum)[0]).size() };
			size_t loopWidth{ m_loops.at(loopNum).size() };

			block += "loop_\n";
			for (const auto& tag : m_loops.at(loopNum)) {
				block += "  " + tag + '\n';
			}
			std::vector<size_t> colWidths(m_loops.at(loopNum).size(), 1); //must not be zero or std::format throws.

			if (pretty) {
				for (size_t j{ 0 }; j < loopWidth; ++j) {
					for (size_t i{ 0 }; i < loopLen; ++i) {
						const std::string& value{ m_block.at(m_loops.at(loopNum)[j]).at(i) };

						if (value.find('\n') != std::string::npos) [[unlikely]] {
							continue; //it's a semicolon textfield, and I don't want to mess with it's length.
						}
						colWidths[j] = std::max(colWidths[j], value.size());
					}
				}
			}

			for (size_t i{ 0 }; i < loopLen; ++i) {
				for (size_t j{ 0 }; j < m_loops.at(loopNum).size(); ++j) {
					const std::string& tag{ m_loops.at(loopNum)[j] };
					block += '\t' + makeStringLength(m_block.at(tag).at(i), colWidths[j]);
				}
				block += '\n';
			}
		}
		else { // it's a plain dataitem
			const std::string& loopTag = std::get<std::string>(item);
			block += makeStringLength(loopTag, maxTagLen) + '\t' + formatValue(m_block.at(loopTag).at(0)) + '\n';
		}
	}
	return block;
}

void row::cif::Block::print(bool pretty /*= true*/) const
{
	std::cout << to_string(pretty);
}

std::string row::cif::Block::formatValue(std::string value) const
{
	if (value.find('\n') != std::string::npos) {
		value = "\n;\n" + value + "\n;"; //its a semicolon textfield
	}
	else if (value.find(' ') != std::string::npos || value[0] == '_') {
		value = "\"" + value + "\""; //it's a string that needs delimiting
	}
	return value;
}

std::string row::cif::Block::makeStringLength(dataname tag, size_t len) const
{
	return std::format("{1:{0}}", len, std::move(tag));
}

std::vector<row::cif::Datavalue> row::cif::Block::values() const
{
	return getValues();
}

std::vector<row::cif::Datavalue> row::cif::Block::getValues() const
{
	std::vector<Datavalue> values{};
	for (const auto& [_, v] : *this) {
		values.push_back(v);
	}
	return values;
}

std::vector<row::cif::dataname> row::cif::Block::tags() const
{
	return getNames();
}

std::vector<row::cif::dataname> row::cif::Block::getNames() const
{
	std::vector<dataname> names{};
	for (const auto& [k, _] : *this) {
		names.push_back(k);
	}
	return names;
}

const row::cif::dataname& row::cif::Block::getName() const
{
	return name;
}

const row::cif::Datavalue& row::cif::Block::get_value(const dataname& tag) const
{
	return get(tag);
}

const row::cif::Datavalue& row::cif::Block::get(const dataname& tag) const
{
	dataname lowerTag{ row::util::toLower(tag) };
	if (contains(lowerTag)) {
		return m_block.at(lowerTag);
	}
	throw no_such_tag_error(std::format("{} does not exist.", lowerTag));
}

row::cif::Block::const_iterator row::cif::Block::put(const dataname& tag, const Datavalue& value)
{
	return set(tag, value);
}

row::cif::Block::const_iterator row::cif::Block::set(const dataname& tag, const Datavalue& value)
{
	return addItem(tag, value);
}

std::vector<std::string> row::cif::Block::getInvalidLoopLengths() const
{
	std::vector<std::string> r{};
	for (const auto& [k, vs] : m_loops) {
		bool invalidLoopLength{ false };
		size_t loopLen{ vs[0].size() };
		for (const auto& v : vs) {
			if (v.size() != loopLen) {
				invalidLoopLength = true;
				break;
			}
		}
		if (invalidLoopLength) {
			r.insert(r.end(), vs.cbegin(), vs.cend());
		}
	}
	return r;
}

row::cif::Block::const_iterator row::cif::Block::changeLoopPosition(const dataname& tag, const size_t newPosn)
{
	/*Move the printout order of the loop containing `tag` to `newpos`.*/
	dataname lowerTag{ row::util::toLower(tag) };
	int loopNum = getLoopNum(tag);
	if (loopNum < 0) {
		throw no_such_tag_error(std::format("{} does not exist in a loop.", tag));
	}

	itemorder tmp{ loopNum };
	row::util::move_element(m_item_order, row::util::getIndexOf(m_item_order, tmp), newPosn);

	return find(lowerTag);
}

row::cif::Block::const_iterator row::cif::Block::changeItemPosition(const dataname& tag, const size_t newPosn)
{
	/*Move the printout order of `tag` to `newpos`. If `tag` is
		in a loop, `newpos` refers to the order within the loop.*/
	dataname lowerTag{ row::util::toLower(tag) };
	if (!contains(lowerTag)) {
		throw no_such_tag_error(std::format("{} does not exist.", tag));
	}
	auto [loopNum, oldPosn] = getItemPosition(tag);
	if (loopNum < 0) {
		row::util::move_element(m_item_order, oldPosn, newPosn);
	}
	else {
		row::util::move_element(m_loops[loopNum], oldPosn, newPosn);
	}
	return find(lowerTag);
}

std::tuple<int, int> row::cif::Block::getItemPosition(const dataname& tag) const
{
	/*A utility function to get the numerical order in the printout
of `tag`.  An item has coordinate `(loop_no,pos)` with
the top level having a `loop_no` of -1.
Return -1, -1, indicates doesn't exist.
*/
	dataname lowerTag{ row::util::toLower(tag) };

	if (!contains(lowerTag)) {
		return std::tuple<int, int> {-1, -1};
	}
	itemorder tmp{ lowerTag };
	if (row::util::contains(m_item_order, tmp)) {
		return std::tuple<int, int> {-1, row::util::getIndexOf(m_item_order, tmp)};
	}
	else {
		int loopNum = getLoopNum(lowerTag);
		auto it = std::find(m_loops.at(loopNum).cbegin(), m_loops.at(loopNum).cend(), lowerTag);
		int loopPos = static_cast<int>(it - m_loops.at(loopNum).cbegin());
		return std::tuple<int, int> {loopNum, loopPos};
	}
}

row::cif::Datavalue row::cif::Block::getAssociatedValue(const dataname& tag, const std::string& value, const dataname& associatedTag)
{
	dataname lowerTag{ row::util::toLower(tag) };
	dataname lowerOther{ row::util::toLower(associatedTag) };
	if (!contains(lowerTag)) {
		throw no_such_tag_error(std::format("{} does not exist.", lowerTag));
	}
	if (!contains(lowerOther)) {
		throw no_such_tag_error(std::format("{} does not exist.", lowerOther));
	}
	if (!isInLoop(lowerTag)) {
		throw no_such_tag_error(std::format("{} does not exist in a loop.", lowerTag));
	}
	if (!isInLoop(lowerOther)) {
		throw no_such_tag_error(std::format("{} does not exist in a loop.", lowerOther));
	}
	if (getLoopNum(lowerTag) != getLoopNum(lowerOther)) {
		throw no_such_tag_error(std::format("{} does not exist in a loop with {}.", lowerOther, lowerTag));
	}

	int i{ row::util::getIndexOf(get(tag).getStrings(), value) };

	return Datavalue{ get(associatedTag).getStrings()[i] };
}

row::cif::Block::const_iterator row::cif::Block::removeItem(const dataname& tag)
{
	dataname lowerTag{ row::util::toLower(tag) };

	if (!contains(lowerTag)) {
		return cend();
	}

	const_iterator returnMe{ find(lowerTag) };
	if (returnMe != cbegin()) {
		--returnMe;
	}

	int loopNum{ getLoopNum(tag) };
	m_block.erase(lowerTag);
	m_true_case.erase(lowerTag);

	if (loopNum > 0) {
		std::erase(m_loops[loopNum], lowerTag);

		if (m_loops[loopNum].size() == 0) {
			m_loops.erase(loopNum);
			std::erase_if(m_item_order, [loopNum](const auto& thing) { if (thing.index() == 0) { return std::get<int>(thing) == loopNum; } else { return false; }});
		}
	}
	else {
		std::erase_if(m_item_order, [lowerTag](const auto& thing) { if (thing.index() == 1) { return std::get<std::string>(thing) == lowerTag; } else { return false; }});
	}
	return returnMe;
}

bool row::cif::Block::isInLoop(const dataname& tag) const
{
	return getLoopNum(tag) != -1;
}

const std::vector<row::cif::dataname>& row::cif::Block::getLoopNames(const dataname& tag) const noexcept(false)
{
	for (auto& [_, vs] : m_loops) {
		if (row::util::contains(vs, tag)) {
			return vs;
		}
	}
	throw no_such_tag_error(std::format("{} does not exist in a loop.", tag));
}

int row::cif::Block::getLoopNum(const std::string& tag) const
{
	dataname lowerTag{ row::util::toLower(tag) };

	for (auto& [k, v] : m_loops) {
		if (row::util::contains(v, lowerTag)) {
			return k;
		}
	}
	return -1;
}

row::cif::Block::const_iterator row::cif::Block::addNameToLoop(const dataname& newName, const dataname& oldName) noexcept(false)
{
	dataname lowerNew{ row::util::toLower(newName) };
	dataname lowerOld{ row::util::toLower(oldName) };

	if (!contains(lowerNew)) { throw no_such_tag_error(std::format("{} does not exist.", lowerNew)); }
	if (m_block.at(lowerNew).size() != m_block.at(lowerOld).size()) { throw loop_length_mismatch_error(std::format("{} new values and {} existing values.", m_block.at(lowerNew).size(), m_block.at(lowerOld).size())); }

	int loopNum{ getLoopNum(oldName) };
	if (loopNum < 0) { throw no_such_tag_error(std::format("{} does not exist in a loop.", oldName)); }
	if (row::util::contains(m_loops.at(loopNum), lowerNew)) { return find(lowerNew); }

	// if we get to here, the newName exists, the oldName is in a loop,  newName isn't in the oldName loop, and newName length is the same as oldName's

	//remove from other loops
	for (auto& [_, v] : m_loops) {
		std::erase(v, lowerNew);
	}
	m_loops[loopNum].push_back(lowerNew);
	std::erase(m_item_order, itemorder{ lowerNew });

	return find(lowerNew);
}

row::cif::Block::const_iterator row::cif::Block::createLoop(const std::vector<dataname>& tags) noexcept(false)
{
	std::vector<dataname> lowerTags{ row::util::toLower(tags) };

	//check that all tags exist, and have all the same length values
	size_t len{ m_block[lowerTags[0]].size() };
	for (const auto& tag : lowerTags) {
		if (!m_block.contains(tag)) {
			throw no_such_tag_error(std::format("{} does not exist.", tag));
		}
		if (m_block.at(tag).size() != len) {
			throw loop_length_mismatch_error("Different number of values per tag in loop");
		}
	}

	//remove all tags from any existing loops
	for (auto& [_, loopTags] : m_loops) {
		for (auto& tag : lowerTags) {
			if (row::util::contains(loopTags, tag)) {
				auto it = std::find(loopTags.cbegin(), loopTags.cend(), tag);
				loopTags.erase(it);
			}
		}
	}

	//remove empty loops
	for (auto& [loopNum, loopTags] : m_loops) {
		if (loopTags.size() == 0) {
			m_loops.erase(loopNum);
		}
	}

	//get the loop number to use for the loop we're about to insert
	int loopNum{ 1 };
	if (m_loops.size() > 0) {
		for (auto& [k, v] : m_loops) {
			if (k >= loopNum)
				loopNum = k + 1;
		}
	}
	m_loops[loopNum] = lowerTags;
	m_item_order.emplace_back(loopNum);

	//remove tags from item order
	for (auto& tag : m_loops[loopNum]) {
		auto it = m_item_order.begin();
		while (it != m_item_order.end()) {
			if (it->index() == 0) {
				++it;
			}
			else if (std::get<std::string>(*it) == tag) {
				it = m_item_order.erase(it);
			}
			else {
				++it;
			}
		}
	}
	return find(lowerTags[0]);
}

row::cif::Block::const_iterator row::cif::Block::addItemsAsLoop(const std::vector<dataname>& tags, const std::vector<Datavalue>& values) noexcept(false)
{
	size_t len{ values[0].size() };
	if (!(std::all_of(values.cbegin(), values.cend(), [len](const auto& value) { return value.size() == len; }))) {
		throw loop_length_mismatch_error("Different number of values per tag in loop");
	}

	addItems(tags, values);
	return createLoop(tags);
}

row::cif::Block::const_iterator row::cif::Block::addItems(const std::vector<dataname>& tags, const std::vector<Datavalue>& values) noexcept(false)
{
	if (tags.size() != values.size()) {
		throw tag_value_mismatch_error(std::format("{} tags and {} values", tags.size(), values.size()));
	}

	const_iterator it = addItem(tags[0], values[0]);
	for (size_t i = 1; i < tags.size(); ++i) {
		addItem(tags[i], values[i]);
	}
	return it;
}

row::cif::Block::const_iterator row::cif::Block::addItem(dataname tag, Datavalue value) noexcept(false)
{
	if (tag.size() < 2 || tag[0] != '_') {
		throw illegal_tag_error(tag);
	}

	dataname lowerTag{ row::util::toLower(tag) };
	if (!overwrite && contains(lowerTag)) {
		throw tag_already_exists_error(lowerTag);
	}


	if (!contains(lowerTag) && !isInLoop(lowerTag)) {
		m_item_order.push_back(lowerTag);
	}

	if (m_true_case.contains(lowerTag)) {
		m_true_case.erase(lowerTag);
	}
	m_true_case.insert({ lowerTag, std::move(tag) });
	const auto& [it, _] = m_block.insert({ lowerTag, std::move(value) });
	// need to convert the iterator to one of mine
	return const_iterator(&(*it), this);
}

row::cif::Block::Block(dataname name, bool ow) : name{ name }, overwrite{ ow }
{

}

row::cif::Block::Block(dataname name) : name{ name }
{

}

row::cif::Block::Block(bool ow) : overwrite(ow)
{

}

void row::cif::Block::print_true_case() const
{
	std::cout << "---\nTrue case:\n";
	for (auto& [k, v] : m_true_case) {
		std::cout << k << " : " << v << ", ";
	}
	std::cout << "---\n";
}

void row::cif::Block::print_item_order() const
{
	std::cout << "---\nItem order:\n";
	for (auto& item : m_item_order) {
		if (item.index() == 0) {
			std::cout << std::get<int>(item) << '\t';
		}
		else {
			std::cout << std::get<std::string>(item) << '\t';
		}
	}
	std::cout << "\n---\n";
}

void row::cif::Block::print_loops() const
{
	std::cout << "---\nLoops:\n";
	for (auto& [k, vs] : m_loops) {
		std::cout << k << ':';
		for (auto& v : vs) {
			std::cout << '\t' << v;
		}
		std::cout << '\n';
	}
	std::cout << "---\n";
}

void row::cif::Block::print_block() const
{
	std::cout << "---\nBlock:\n";
	for (auto& [k, vs] : m_block) {
		std::cout << k << ':';
		for (auto& v : vs) {
			std::cout << '\t' << v;
		}
		std::cout << '\n';
	}
	std::cout << "---\n";
}

row::cif::Block::const_iterator::const_pointer row::cif::Block::const_iterator::nextPtr(size_t currIndex, dataname& currentTag)
{
	return getPtr(++currIndex, currentTag);
}

row::cif::Block::const_iterator::const_pointer row::cif::Block::const_iterator::prevPtr(size_t currIndex, dataname& currentTag)
{
	return getPtr(--currIndex, currentTag);
}

row::cif::Block::const_iterator::const_pointer row::cif::Block::const_iterator::getPtr(size_t idx, dataname& currentTag)
{
	itemorder item{ block->m_item_order[idx] };
	if (item.index() == 0) { // the prevTag is the start of a loop
		size_t numTags{ block->m_loops.at(std::get<int>(item)).size() };
		currentTag = block->m_loops.at(std::get<int>(item))[numTags - 1];//update the tagname
	}
	else { //the next tag is just a tag
		currentTag = std::get<dataname>(item); //update the tagname
	}
	return &(*block->m_block.find(currentTag));
}

row::cif::Block::const_iterator row::cif::Block::const_iterator::operator--(int)
{
	const_iterator tmp = *this; --(*this); return tmp;
}

row::cif::Block::const_iterator row::cif::Block::const_iterator::operator++(int)
{
	const_iterator tmp = *this; ++(*this); return tmp;
}

row::cif::Block::const_iterator& row::cif::Block::const_iterator::operator++()
{
	dataname currentTag{  };

	//what is ++end()? It should be begin();
	if (!m_ptr) { //ie, increment end();
		size_t numItems{ block->m_item_order.size() };
		itemorder lastItem{ block->m_item_order[numItems - 1] };
		if (lastItem.index() == 0) { // the lastItem is the start of a loop
			size_t numTags{ block->m_loops.at(std::get<int>(lastItem)).size() };
			currentTag = block->m_loops.at(std::get<int>(lastItem))[numTags - 1];//update the tagname
		}
		else { //the next tag is just a tag
			currentTag = std::get<dataname>(lastItem); //update the tagname
		}
		m_ptr = &(*block->m_block.find(currentTag));
		return *this;
	}


	currentTag = m_ptr->first;
	auto [loopNum, posn] = block->getItemPosition(currentTag);
	itemorder nextItem{};
	if (loopNum < 0) { // currentTag is not a loop
		if (posn < block->m_item_order.size() - 1) { // not at the end of the item_order
			m_ptr = nextPtr(posn, currentTag);
		}
		else {
			m_ptr = nullptr;
		}
	}
	else {  //we're in a loop
		if (posn < block->m_loops.at(loopNum).size() - 1) { // not at the end of the loop
			currentTag = block->m_loops.at(loopNum)[++posn];
			m_ptr = const_cast<pointer>(&(*block->m_block.find(currentTag)));
		}
		else { //we're at the end of a loop and need to look at the next element
			int currentIndex = row::util::getIndexOf(block->m_item_order, std::variant<int, dataname>{ loopNum });
			if (currentIndex < block->m_item_order.size() - 1) { // we can go to the next element
				m_ptr = nextPtr(currentIndex, currentTag);
			}
			else {
				m_ptr = nullptr;
			}
		}
	}
	return *this;
}

row::cif::Block::const_iterator& row::cif::Block::const_iterator::operator--()
{
	dataname currentTag{};
	// what is --end()? It should be begin();
	if (!m_ptr) { //ie, decrement end();
		itemorder firstItem{ block->m_item_order[0] };
		if (firstItem.index() == 0) { // the firstTag is the start of a loop
			currentTag = block->m_loops.at(std::get<int>(firstItem))[0];//update the tagname
		}
		else { //the next tag is just a tag
			currentTag = std::get<dataname>(firstItem); //update the tagname
		}
		m_ptr = &(*block->m_block.find(currentTag));
		return *this;
	}

	currentTag = m_ptr->first;
	auto [loopNum, posn] = block->getItemPosition(currentTag);
	if (loopNum < 0) { // currentTag is not a loop
		if (posn > 0) { // not at the beginning of the item_order
			m_ptr = prevPtr(posn, currentTag);
		}
		else {
			m_ptr = nullptr;
		}
	}
	else {  //we're in a loop
		if (posn > 0) { // not at the beginning of the loop
			currentTag = block->m_loops.at(loopNum)[--posn];
			m_ptr = &(*block->m_block.find(currentTag));
		}
		else { //we're at the beginning of a loop and need to look at the next element
			int currentIndex{ row::util::getIndexOf(block->m_item_order, std::variant<int, dataname>{ loopNum }) };
			if (currentIndex < 0) { // we can go to the previous element
				m_ptr = prevPtr(currentIndex, currentTag);
			}
			else {
				m_ptr = nullptr;
			}
		}
	}
	return *this;
}

row::cif::Block::const_iterator::const_pointer row::cif::Block::const_iterator::operator->()
{
	return m_ptr;
}

row::cif::Block::const_iterator::const_reference row::cif::Block::const_iterator::operator*() const
{
	return *m_ptr;
}

row::cif::Block::const_iterator::const_iterator(const_pointer m_ptr, const Block* blk) : m_ptr{ m_ptr }, block{ blk }
{

}

std::pair<const row::cif::Cif::blockname, row::cif::Block>* row::cif::Cif::constptrToFirstBlock() const
{
	std::pair<const blockname, Block>* ptr = const_cast<std::pair<const blockname, Block>*>(&(*m_cif.find(m_block_order[0])));
	return ptr;
}

std::pair<const row::cif::Cif::blockname, row::cif::Block>* row::cif::Cif::ptrToFirstBlock()
{
	return &(*m_cif.find(m_block_order[0]));
}

bool row::cif::Cif::contains(const blockname& name) const
{
	dataname lowerTag{ row::util::toLower(name) };
	return m_cif.contains(lowerTag);
}

row::cif::Cif::const_iterator row::cif::Cif::find(const blockname& name) const
{
	dataname lowerTag{ row::util::toLower(name) };
	std::pair<const blockname, Block>* m_ptr = const_cast<std::pair<const blockname, Block>*>(&(*m_cif.find(lowerTag)));
	return const_iterator(m_ptr, this);
}

size_t row::cif::Cif::count(const blockname& name) const
{
	dataname lowerTag{ row::util::toLower(name) };
	return m_cif.count(lowerTag);
}

const row::cif::Block& row::cif::Cif::at(const blockname& name) const
{
	dataname lowerTag{ row::util::toLower(name) };
	return m_cif.at(lowerTag);
}

const row::cif::Block& row::cif::Cif::get(const blockname& name) const
{
	dataname lowerTag{ row::util::toLower(name) };
	if (contains(name)) {
		return m_cif.at(name);
	}
	throw no_such_tag_error(std::format("{} does not exist.", lowerTag));
}

row::cif::Cif::const_iterator row::cif::Cif::put(const blockname& name, const Block& value)
{
	return set(name, value);
}

row::cif::Cif::const_iterator row::cif::Cif::set(const blockname& name, const Block& value)
{
	return addBlock(name, value);
}

size_t row::cif::Cif::erase(const blockname& name)
{
	dataname lowerTag{ row::util::toLower(name) };
	const_iterator r{ removeBlock(lowerTag) };
	if (r != cend()) { return 1; }
	else { return 0; }
}

void row::cif::Cif::clear() noexcept
{
	m_cif.clear();
	m_block_order.clear();
	m_true_case.clear();
	m_overwrite = true;
}

size_t row::cif::Cif::max_size() const noexcept
{
	return std::min({ m_cif.max_size(), m_block_order.max_size(), m_true_case.max_size() });
}

int row::cif::Cif::size(const std::string& name) const noexcept
{
	dataname lowerTag{ row::util::toLower(name) };
	if (contains(lowerTag)) {
		return static_cast<int>(m_cif.at(lowerTag).size());
	}
	return -1;
}

size_t row::cif::Cif::size() const noexcept
{
	return m_cif.size();
}

bool row::cif::Cif::isEmpty() const noexcept
{
	return empty();
}

bool row::cif::Cif::empty() const noexcept
{
	return m_cif.empty();
}

row::cif::Cif::const_iterator row::cif::Cif::cend() const noexcept
{
	return const_iterator(nullptr, this);
}

row::cif::Cif::const_iterator row::cif::Cif::cbegin() const noexcept
{
	return const_iterator(constptrToFirstBlock(), this);
}

row::cif::Cif::const_iterator row::cif::Cif::end() const noexcept
{
	return const_iterator(nullptr, this);
}

row::cif::Cif::const_iterator row::cif::Cif::begin() const noexcept
{
	return const_iterator(constptrToFirstBlock(), this);
}

std::string row::cif::Cif::to_string(bool pretty/*=true*/) const
{
	std::string cif{};
	for (const auto& block : m_block_order) {
		cif += "\ndata_" + block + '\n';
		cif += m_cif.at(block).to_string(pretty);
	}
	return cif;
}

void row::cif::Cif::print(bool pretty /*= true*/) const
{
	std::cout << to_string(pretty);
}

bool row::cif::Cif::canOverwrite() const
{
	return m_overwrite;
}

bool row::cif::Cif::overwrite(bool ow)
{
	if (m_overwrite == ow) {
		return m_overwrite == ow;
	}
	m_overwrite = ow;

	for (auto& [_, block] : m_cif) {
		block.overwrite = m_overwrite;
	}
	return m_overwrite;
}

row::cif::Cif::const_iterator row::cif::Cif::changeBlockPosition(const dataname& name, const size_t newPosn)
{
	/*Move the printout order of `name` to `newpos`. If `name` is
		in a loop, `newpos` refers to the order within the loop.*/
	dataname lowerTag{ row::util::toLower(name) };
	if (!contains(lowerTag)) {
		throw no_such_tag_error(std::format("{} does not exist.", lowerTag));
	}

	int oldPosn = getBlockPosition(name);
	row::util::move_element(m_block_order, oldPosn, newPosn);

	return find(lowerTag);
}

int row::cif::Cif::getBlockPosition(const dataname& name) const
{
	/*A utility function to get the numerical order in the printout
of `name`.  An item has coordinate `(loop_no,pos)` with
the top level having a `loop_no` of -1.
Return -1, -1, indicates doesn't exist.
*/
	dataname lowerTag{ row::util::toLower(name) };

	if (!contains(lowerTag)) {
		return -1;
	}
	return row::util::getIndexOf(m_block_order, lowerTag);
}

row::cif::Cif::const_iterator row::cif::Cif::removeBlock(const blockname& name)
{
	dataname lowerTag{ row::util::toLower(name) };

	if (!contains(lowerTag)) {
		return cend();
	}
	const_iterator r = ++find(lowerTag);
	m_cif.erase(lowerTag);
	m_true_case.erase(lowerTag);
	std::erase_if(m_block_order, [lowerTag](const auto& thing) { return thing == lowerTag; });

	return r;
}

row::cif::Cif::const_iterator row::cif::Cif::addBlocks(const std::vector<blockname>& names, const std::vector<Block>& blocks)
{
	if (names.size() != blocks.size()) {
		throw tag_value_mismatch_error(std::format("{} names and {} blocks", names.size(), blocks.size()));
	}

	for (size_t i = 0; i < names.size(); ++i) {
		addBlock(names[i], blocks[i]);
	}
	return find(row::util::toLower(names[0]));
}

row::cif::Cif::const_iterator row::cif::Cif::addBlock(const blockname& name, const Block& block) noexcept(false)
{
	dataname lowerTag{ row::util::toLower(name) };

	if (!canOverwrite() && contains(lowerTag)) {
		throw tag_already_exists_error(lowerTag);
	}

	if (!contains(lowerTag)) {
		m_block_order.push_back(lowerTag);
	}

	m_true_case.erase(lowerTag);
	m_true_case[lowerTag] = name;
	m_cif[lowerTag] = block;
	return find(lowerTag);
}

row::cif::Block& row::cif::Cif::addBlock(const blockname& name) noexcept(false)
{
	return addName(name);
}

row::cif::Block& row::cif::Cif::addName(const blockname& name) noexcept(false)
{
	dataname lowerTag{ row::util::toLower(name) };

	if (!canOverwrite() && contains(lowerTag)) {
		throw tag_already_exists_error(lowerTag);
	}

	if (!contains(lowerTag)) {
		m_block_order.push_back(lowerTag);
	}

	m_true_case.erase(lowerTag);
	m_true_case[lowerTag] = name;
	m_cif[lowerTag];
	m_cif[lowerTag].overwrite = m_overwrite;
	return m_cif[lowerTag];
}

const std::string& row::cif::Cif::getSource() const
{
	return m_source;
}

const row::cif::Cif::blockname& row::cif::Cif::getLastBlockName() const
{
	return m_block_order.back();
}

const row::cif::Block& row::cif::Cif::getLastBlock() const
{
	return m_cif.at(m_block_order.back());
}

row::cif::Block& row::cif::Cif::getLastBlock()
{
	return m_cif.at(m_block_order.back());
}

row::cif::Cif::Cif(std::string source) : m_source(std::move(source))
{

}

row::cif::Cif::Cif()
{

}

void row::cif::Cif::print_true_case() const
{
	std::cout << "---\nTrue case:\n";
	for (const auto& [k, v] : m_true_case) {
		std::cout << k << " : " << v << ", ";
	}
	std::cout << "\n---\n";
}

void row::cif::Cif::print_block_order() const
{
	std::cout << "---\nBlock order:\n";
	for (const auto& block : m_block_order) {
		std::cout << block << '\n';
	}
	std::cout << "---\n";
}

void row::cif::Cif::print_cif() const
{
	std::cout << "---\nCIF tags:\n";
	for (const auto& [k, _] : m_cif) {
		std::cout << k << '\n';
	}
	std::cout << "---\n";
}

row::cif::Cif::const_iterator row::cif::Cif::const_iterator::operator++(int)
{
	const_iterator tmp = *this; ++(*this); 
	return tmp;
}

row::cif::Cif::const_iterator& row::cif::Cif::const_iterator::operator++()
{
	blockname currentBlock{ m_ptr->first };
	int posn = cif->getBlockPosition(currentBlock);
	if (posn < cif->m_block_order.size() - 1) { // not at the end of the block_order
		currentBlock = cif->m_block_order[++posn]; //update the current block to the next one
		m_ptr = &(*cif->m_cif.find(currentBlock));
	}
	else {
		m_ptr = nullptr;
	}
	return *this;
}

row::cif::Cif::const_iterator::const_pointer row::cif::Cif::const_iterator::operator->()
{
	return m_ptr;
}

row::cif::Cif::const_iterator::const_reference row::cif::Cif::const_iterator::operator*() const
{
	return *m_ptr;
}

row::cif::Cif::const_iterator::const_iterator(const_pointer m_ptr, const Cif* cif) 
	: m_ptr{ m_ptr }, cif{ cif }
{

}
