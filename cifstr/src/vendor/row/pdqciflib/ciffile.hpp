
#ifndef ROW_CIFFILE_HPP
#define ROW_CIFFILE_HPP



#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <variant>  
#include <iterator> // For std::forward_iterator_tag
#include <cstddef>  // For std::ptrdiff_t
#include <format>
#include <algorithm>
#include <utility>
#include <string_view>

#include "util.hpp"
#include "cifexcept.hpp"



namespace row::cif {

	// after https://stackoverflow.com/a/8627711/36061, https://stackoverflow.com/a/53613999/36061, https://github.com/microsoft/STL/issues/683
	struct CaseInsensitiveEqual
	{
		using is_transparent = void;

		bool operator()(const std::string& left, const std::string_view right) const {
			return compare(left, right);
		}

		bool operator()(const std::string_view left, const std::string_view right) const {
			return compare(left, right);
		}

		bool compare(const std::string_view left, const std::string_view right) const {
			return left.size() == right.size() &&
				std::equal(left.begin(), left.end(), right.begin(),
					[](unsigned char b, unsigned char a) {
						return std::tolower(a) == std::tolower(b);
					}
			);
		}
	};

	//after https://stackoverflow.com/a/107657/36061, https://stackoverflow.com/a/53613999/36061, https://www.cppstories.com/2021/heterogeneous-access-cpp20/
	struct CaseInsensitiveHash
	{
		using is_transparent = void;
		using transparent_key_equal = CaseInsensitiveEqual;

		[[nodiscard]] size_t operator()(const char* txt) const {
			return makeHash(txt);
		}
		[[nodiscard]] size_t operator()(const std::string& txt) const {
			return makeHash(txt);
		}
		[[nodiscard]] size_t operator()(const std::string_view txt) const {
			return makeHash(txt);
		}

		size_t makeHash(const std::string_view key) const {
			size_t h = 0;
			std::for_each(key.begin(), key.end(), [&](char c) {
				h = h * 101 + ::tolower(c);
				}
			);
			return h;
		}
	};



	using dataname = std::string;
	using dataname_view = std::string_view;
	using datavalue_view = std::string_view;


	class Datavalue {
	public:
		using size_type = typename std::vector<std::string>::size_type;
		using const_iterator = typename std::vector<std::string>::const_iterator;
		using const_reverse_iterator = typename std::vector<std::string>::const_reverse_iterator;
		using const_reference = typename std::vector<std::string>::const_reference;

		using const_iterator_double = typename std::vector<double>::const_iterator;
		using const_reverse_iterator_double = typename std::vector<double>::const_reverse_iterator;
		using const_reference_double = typename std::vector<double>::const_reference;

	private:
		std::vector<std::string> m_strs{};
		mutable std::vector<double> m_dbls{};
		mutable std::vector<double> m_errs{};
		mutable bool m_isConverted{ false };
		
	public:
		Datavalue()=default;
		Datavalue(const std::string& in) : m_strs({ in }) {}
		Datavalue(std::string&& in) : m_strs({ std::move(in) }) {}
		Datavalue(const std::vector<std::string>& in) : m_strs(in) {}
		Datavalue(std::vector<std::string>&& in) : m_strs(std::move(in)) {}
		Datavalue(std::initializer_list<std::string> in) : m_strs{ in } {}

		bool convert() const {
			if (m_isConverted) {
				return m_isConverted;
			}

			//test the first one. If it passes, assume the rest will.
			// a fully validating parser would test every one, as well
			// as knowing if the tag associated with the values could
			// be numeric, or a list, etc...
			if (!m_strs.empty()) {
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

		void reconvert() const {
			m_isConverted = false;
			return;
		}

		bool isConverted() const {
			return m_isConverted;
		}

		//vector access
		const std::vector<std::string>& getStrings() const {
			return m_strs;
		}
		const std::vector<double>& getDoubles() const {
			convert();
			return m_dbls;
		}
		const std::vector<double>& getErrors() const {
			convert();
			return m_errs;
		}

		//element access
		constexpr const_reference at(size_type pos) const noexcept(false) {
			return m_strs.at(pos);
		}

		constexpr const_reference str_at(size_type pos) const noexcept(false) {
			return at(pos);
		}

		const_reference_double dbl_at(size_type pos) const noexcept(false) {
			convert();
			return m_dbls.at(pos);
		}

		const_reference_double err_at(size_type pos) const noexcept(false) {
			convert();
			return m_errs.at(pos);
		}


		// extreme iterators
		constexpr const_reference front() const {
			return m_strs.front();
		}

		constexpr const_reference front_str() const {
			return front();
		}

		const_reference_double front_dbl() const {
			convert();
			return m_dbls.front();
		}

		const_reference_double front_err() const {
			convert();
			return m_errs.front();
		}


		constexpr const_reference back() const {
			return m_strs.back();
		}

		constexpr const_reference back_str() const {
			return back();
		}

		const_reference_double back_dbl() const {
			convert(); 
			return m_dbls.back();
		}

		const_reference_double back_err() const {
			convert(); 
			return m_errs.back();
		}


		//data pointer
		constexpr const std::string* data() const noexcept {
			return m_strs.data();
		}

		constexpr const std::string* str_data() const noexcept {
			return data();
		}

		const double* dbl_data() const noexcept {
			convert();
			return m_dbls.data();
		}

		const double* err_data() const noexcept {
			convert();
			return m_errs.data();
		}


		//iterators
		const_iterator begin() const {
			return m_strs.begin();
		}

		const_iterator end() const {
			return m_strs.end();
		}

		const_reverse_iterator rbegin() const {
			return m_strs.rbegin();
		}

		const_reverse_iterator rend() const {
			return m_strs.rend();
		}

		const_iterator cbegin() const {
			return m_strs.cbegin();
		}

		const_iterator cend() const {
			return m_strs.cend();
		}

		const_reverse_iterator crbegin() const {
			return m_strs.crbegin();
		}

		const_reverse_iterator crend() const {
			return m_strs.crend();
		}

		const_iterator str_begin() const {
			return begin();
		}

		const_iterator str_end() const {
			return end();
		}

		const_reverse_iterator str_rbegin() const {
			return rbegin();
		}

		const_reverse_iterator str_rend() const {
			return rend();
		}

		const_iterator str_cbegin() const {
			return cbegin();
		}

		const_iterator str_cend() const {
			return cend();
		}

		const_reverse_iterator str_crbegin() const {
			return crbegin();
		}

		const_reverse_iterator str_crend() const {
			return crend();
		}

		const_iterator_double dbl_begin() const {
			convert();
			return m_dbls.begin();
		}

		const_iterator_double dbl_end() const {
			convert();
			return m_dbls.end();
		}

		const_reverse_iterator_double dbl_rbegin() const {
			convert();
			return m_dbls.rbegin();
		}

		const_reverse_iterator_double dbl_rend() const {
			convert();
			return m_dbls.rend();
		}

		const_iterator_double dbl_cbegin() const {
			convert();
			return m_dbls.cbegin();
		}

		const_iterator_double dbl_cend() const {
			convert();
			return m_dbls.cend();
		}

		const_reverse_iterator_double dbl_crbegin() const {
			convert();
			return m_dbls.crbegin();
		}

		const_reverse_iterator_double dbl_crend() const {
			convert();
			return m_dbls.crend();
		}


		const_iterator_double err_begin() const {
			convert();
			return m_errs.begin();
		}

		const_iterator_double err_end() const {
			convert();
			return m_errs.end();
		}

		const_reverse_iterator_double err_rbegin() const {
			convert();
			return m_errs.rbegin();
		}

		const_reverse_iterator_double err_rend() const {
			convert();
			return m_errs.rend();
		}

		const_iterator_double err_cbegin() const {
			convert();
			return m_errs.cbegin();
		}

		const_iterator_double err_cend() const {
			convert();
			return m_errs.cend();
		}


		const_reverse_iterator_double err_crbegin() const {
			convert();
			return m_errs.crbegin();
		}

		const_reverse_iterator_double err_crend() const {
			convert();
			return m_errs.crend();
		}



		//capacity
		[[nodiscard]] constexpr bool empty() const noexcept {
			return m_strs.empty();
		}

		[[nodiscard]] constexpr bool isEmpty() const noexcept {
			return empty();
		}

		constexpr size_type size() const noexcept {
			return m_strs.size();
		}

		constexpr void reserve(size_type new_cap) {
			m_strs.reserve(new_cap);
			m_dbls.reserve(new_cap);
			m_errs.reserve(new_cap);
			return;
		}

		constexpr size_type capacity() const noexcept {
			return m_strs.capacity();
		}

		constexpr void shrink_to_fit() {
			m_strs.shrink_to_fit();
			m_dbls.shrink_to_fit();
			m_errs.shrink_to_fit();
			return;
		}


		//modifiers
		constexpr void clear() noexcept {
			m_strs.clear();
			m_dbls.clear();
			m_errs.clear();
			m_isConverted = false;
			return;
		}

		void push_back(const std::string& value) {
			m_isConverted = false;
			m_strs.push_back(value);
			return;
		}

		void push_back(std::string&& value) {
			m_isConverted = false;
			m_strs.push_back(std::forward<std::string>(value));
			return;
		}

		constexpr void swap(Datavalue& other) {
			m_strs.swap(other.m_strs);
			m_dbls.swap(other.m_dbls);
			m_errs.swap(other.m_errs);
			std::swap(m_isConverted, other.m_isConverted);
			return;
		}



		//"non"-member functions
		friend constexpr bool operator==(const Datavalue& lhs, const Datavalue& rhs) {
			return lhs.m_strs == rhs.m_strs;
		}
		friend constexpr auto operator<=>(const Datavalue& lhs, const Datavalue& rhs) {
			return lhs.m_strs <=> rhs.m_strs;
		}
		friend constexpr void swap(Datavalue& lhs, Datavalue& rhs) noexcept(noexcept(lhs.swap(rhs))) {
			lhs.swap(rhs);
		}
	};


	class Block {
	public:
		using itemorder = std::variant<int, dataname>;
		//using item = typename dict<dataname, Datavalue>::value_type;

	private:
		std::unordered_map<dataname, Datavalue, CaseInsensitiveHash, CaseInsensitiveEqual> m_block{}; // this is the actual data
		std::unordered_map<int, std::vector<dataname>> m_loops{}; // keeps track of datanames that are looped together
		std::vector<itemorder> m_item_order{}; // keeps the insertion order
		dataname name{}; //the name of the block to which this data belongs -> must be set through the Cif which this Block belongs.

	public:
		bool overwrite{ true };

		//these print_* functions are for debugging purposes
		void print_block() const {
			std::cout << "---\nBlock:\n";
			for (auto& [k, vs] : m_block) {
				std::cout << k << ':';
				for (auto& v : vs) {
					std::cout << '\t' << v;
				}
				std::cout << '\n';
			}
			std::cout << "---\n";
			return;
		}

		void print_loops() const {
			std::cout << "---\nLoops:\n";
			for (auto& [k, vs] : m_loops) {
				std::cout << k << ':';
				for (auto& v : vs) {
					std::cout << '\t' << v;
				}
				std::cout << '\n';
			}
			std::cout << "---\n";
			return;
		}

		void print_item_order() const {
			std::cout << "---\nItem order:\n";
			for (auto& item : m_item_order) {
				if (item.index() == 0) {
					std::cout << std::get<int>(item) << '\t';
				}
				else {
					std::cout << std::get<dataname>(item) << '\t';
				}
			}
			std::cout << "\n---\n";
			return;
		}


		struct const_iterator;

	public:
		Block() = default;
		explicit Block(bool ow) : overwrite(ow) {}

		const_iterator addItem(dataname tag, Datavalue value) noexcept(false) {
			if (tag.size() < 2 || tag[0] != '_') {
				throw illegal_tag_error(tag);
			}

			row::util::toLower_i(tag);

			if (!overwrite && contains(tag)) {
				throw tag_already_exists_error(tag);
			}

			if (!contains(tag) && !isInLoop(tag)) {
				m_item_order.emplace_back(tag);
			}

			const auto& [it, _] = m_block.insert({ std::move(tag), std::move(value) });
			// need to convert the iterator to one of mine
			return { &(*it), this };
		}

		const_iterator addItems(const std::vector<dataname>& tags, const std::vector<Datavalue>& values) noexcept(false) {
			if (tags.size() != values.size()) {
				throw tag_value_mismatch_error(std::format("{} tags and {} values", tags.size(), values.size()));
			}

			const_iterator it = addItem(tags[0], values[0]);

			for (size_t i = 1; i < tags.size(); ++i) {
				addItem(tags[i], values[i]);
			}
			return it;
		}

		const_iterator addItemsAsLoop(const std::vector<dataname>& tags, const std::vector<Datavalue>& values) noexcept(false) {
			size_t len{ values[0].size() };
			if (!(std::all_of(values.cbegin(), values.cend(), [len](const auto& value) { return value.size() == len; }))) {
				throw loop_length_mismatch_error("Different number of values per tag in loop");
			}

			addItems(tags, values);
			return createLoop(tags);
		}

		const_iterator createLoop(std::vector<dataname> tags) noexcept(false) {

			//check that all tags exist, and have all the same length values
			size_t len{ m_block[tags[0]].size() };
			for (const auto& tag : tags) {
				if (!m_block.contains(tag)) {
					throw no_such_tag_error(std::format("{} does not exist.", tag));
				}
				if (m_block.at(tag).size() != len) {
					throw loop_length_mismatch_error("Different number of values per tag in loop");
				}
			}

			//remove all tags from any existing loops
			for (auto& [_, loopTags] : m_loops) {
				for (auto& tag : tags) {
					if (row::util::icontains(loopTags, tag)) {
						auto it = std::find(loopTags.cbegin(), loopTags.cend(), tag);
						loopTags.erase(it);
					}
				}
			}

			//remove empty loops
			for (auto& [loopNum, loopTags] : m_loops) {
				if (loopTags.empty()) {
					m_loops.erase(loopNum);
				}
			}

			//get the loop number to use for the loop we're about to insert
			int loopNum{ 1 };
			if (!m_loops.empty()) {
				for (auto& [k, v] : m_loops) {
					if (k >= loopNum)
						loopNum = k + 1;
				}
			}
			m_loops[loopNum] = std::move(tags);
			m_item_order.emplace_back(loopNum);

			//remove tags from item order
			for (auto& tag : m_loops[loopNum]) {
				auto it = m_item_order.begin();
				while (it != m_item_order.end()) {
					if (it->index() == 0) {
						++it;
					}
					else if (std::get<dataname>(*it) == tag) {
						it = m_item_order.erase(it);
					}
					else {
						++it;
					}
				}
			}
			return find(m_loops[loopNum][0]);
		}

		const_iterator addNameToLoop(dataname newName, const dataname_view oldName) noexcept(false) {
			row::util::toLower_i(newName);

			if (!contains(newName)) {
				throw no_such_tag_error(std::format("{} does not exist.", newName));
			}
			if (get(newName).size() != get(oldName).size()) {
				throw loop_length_mismatch_error(std::format("{} new values and {} existing values.", get(newName).size(), get(oldName).size()));
			}

			int loopNum{ getLoopNum(oldName) };
			if (loopNum < 0) {
				throw no_such_tag_error(std::format("{} does not exist in a loop.", oldName));
			}
			if (row::util::icontains(m_loops.at(loopNum), newName)) {
				return find(newName);
			}

			// if we get to here, the newName exists, the oldName is in a loop, newName isn't in the oldName loop, and newName length is the same as oldName's

			//remove from other loops
			for (auto& [_, v] : m_loops) {
				std::erase(v, newName);
			}
			m_loops[loopNum].push_back(newName);
			std::erase(m_item_order, itemorder{ newName });

			return find(newName);
		}

		const_iterator removeItem(const dataname_view tag) {
			if (!contains(tag)) {
				return cend();
			}

			const_iterator returnMe{ find(tag) };
			if (returnMe != cbegin()) {
				--returnMe;
			}

			int loopNum{ getLoopNum(tag) };
			m_block.erase(m_block.find(tag));

			if (loopNum > 0) {
				std::erase(m_loops[loopNum], tag);

				if (m_loops[loopNum].empty()) {
					m_loops.erase(loopNum);
					std::erase_if(m_item_order, [loopNum](const auto& thing) {
						if (thing.index() == 0) {
							return std::get<int>(thing) == loopNum;
						}
						return false;
						});
				}
			}
			else {
				std::erase_if(m_item_order, [tag](const auto& thing) {
					if (thing.index() == 1) {
						return row::util::icompare(std::get<dataname>(thing), tag);
					}
					return false;
					});
			}
			return returnMe;
		}

		int getLoopNum(const dataname_view tag) const {
			for (auto& [k, v] : m_loops) {
				if (row::util::icontains(v, tag)) { //contains needs to do a case insensitive comparision
					return k;
				}
			}
			return -1;
		}

		const std::vector<dataname>& getLoopNames(const dataname_view tag) const noexcept(false) {
			for (auto& [_, vs] : m_loops) {
				if (row::util::icontains(vs, tag)) {
					return vs;
				}
			}
			throw no_such_tag_error(std::format("{} does not exist in a loop.", tag));
		}


		bool isInLoop(const dataname_view tag) const {
			return getLoopNum(tag) != -1;
		}

		Datavalue getAssociatedValue(const dataname_view tag, const datavalue_view value, const dataname_view associatedTag) {
			if (!contains(tag)) {
				throw no_such_tag_error(std::format("{} does not exist.", tag));
			}
			if (!contains(associatedTag)) {
				throw no_such_tag_error(std::format("{} does not exist.", associatedTag));
			}
			if (!isInLoop(tag)) {
				throw no_such_tag_error(std::format("{} does not exist in a loop.", tag));
			}
			if (!isInLoop(associatedTag)) {
				throw no_such_tag_error(std::format("{} does not exist in a loop.", associatedTag));
			}
			if (getLoopNum(tag) != getLoopNum(associatedTag)) {
				throw no_such_tag_error(std::format("{0} does not exist in a loop with {1}.", associatedTag, tag));
			}

			//int i{ row::util::getIndexOf(getValue(tag).getStrings(), value) };

			auto& allValues = getValue(tag).getStrings();
			int i{ static_cast<int>(std::distance(allValues.begin(), std::find(allValues.begin(), allValues.end(), value))) };

			return Datavalue{ getValue(associatedTag).getStrings()[i] };
		}

		std::tuple<int, int> getItemPosition(const dataname_view tag) const {
			//A utility function to get the numerical order in the printout
			//of `tag`.  An item has coordinate `(loop_no,pos)` with
			//the top level having a `loop_no` of -1.
			//Return -1, -1, indicates doesn't exist.
			//
			if (!contains(tag)) {
				return std::tuple<int, int> {-1, -1};
			}

			const auto it = std::find_if(m_item_order.cbegin(), m_item_order.cend(), [tag](auto& item) {
				if (item.index() != 0) {
					return row::util::icompare(tag, std::get<dataname>(item));
				}
				return false;
				});


			if (it != m_item_order.end()) {
				return std::tuple<int, int> { -1, static_cast<int>(std::distance(m_item_order.cbegin(), it)) };
			}
			int loopNum{ getLoopNum(tag) };
			auto itt{ std::find(m_loops.at(loopNum).cbegin(), m_loops.at(loopNum).cend(), tag) };
			int loopPos{ static_cast<int>(std::distance(m_loops.at(loopNum).cbegin(), itt)) };
			return std::tuple<int, int> {loopNum, loopPos};
		}

		const_iterator changeItemPosition(const dataname_view tag, const size_t newPosn) {
			/*Move the printout order of `tag` to `newpos`. If `tag` is
				in a loop, `newpos` refers to the order within the loop.*/

			if (!contains(tag)) {
				throw no_such_tag_error(std::format("{} does not exist.", tag));
			}
			auto [loopNum, oldPosn] = getItemPosition(tag);
			if (loopNum < 0) {
				row::util::move_element(m_item_order, oldPosn, newPosn);
			}
			else {
				row::util::move_element(m_loops[loopNum], oldPosn, newPosn);
			}
			return find(tag);
		}

		const_iterator changeLoopPosition(const dataname_view tag, const size_t newPosn) {
			/*Move the printout order of the loop containing `tag` to `newpos`.*/
			int loopNum = getLoopNum(tag);
			if (loopNum < 0) {
				throw no_such_tag_error(std::format("{} does not exist in a loop.", tag));
			}

			itemorder tmp{ loopNum };
			row::util::move_element(m_item_order, row::util::getIndexOf(m_item_order, tmp), newPosn);

			return find(tag);
		}

		std::vector<std::string> getInvalidLoopLengths() const //Why is this here? What does it do?
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

		const Datavalue& getValue(const dataname_view tag) const {
			if (contains(tag)) {
				return m_block.find(tag)->second;
			}
			throw no_such_tag_error(std::format("{} does not exist.", tag));
		}

		const Datavalue& get(const dataname_view tag) const {
			return getValue(tag);
		}

		std::vector<dataname> getAllTags() const {
			std::vector<dataname> names{};
			names.reserve(this->size());
			for (const auto& [k, _] : *this) {
				names.push_back(k);
			}
			return names;
		}

		std::vector<Datavalue> getAllValues() const {
			std::vector<Datavalue> values{};
			values.reserve(this->size());
			for (const auto& [_, v] : *this) {
				values.push_back(v);
			}
			return values;
		}


		std::string makeStringLength(dataname_view tag, size_t len) const {
			return std::format("{1:{0}}", len, tag);
		}

		std::string formatValue(std::string value) const {
			if (value.find('\n') != std::string::npos) {
				value = "\n;\n" + value + "\n;"; //its a semicolon textfield
			}
			else if (value.find(' ') != std::string::npos || value[0] == '_') {
				value = "\"" + value + "\""; //it's a string that needs delimiting
			}
			return value;
		}

		void print(bool pretty = true) const {
			std::cout << to_string(pretty);
			return;
		}

		[[nodiscard]] std::string to_string(bool pretty = true) const {
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

		//Iterators

		//Iterators
		const_iterator begin() const noexcept {
			return { constptrToFirstItem(), this };
		}

		const_iterator end() const noexcept {
			return { nullptr, this };
		}

		const_iterator cbegin() const noexcept {
			return { constptrToFirstItem(), this };
		}

		const_iterator cend() const noexcept {
			return { nullptr, this };
		}

		//capacity
		[[nodiscard]] bool empty() const noexcept {
			return m_block.empty();
		}

		[[nodiscard]] bool isEmpty() const noexcept {
			return empty();
		}

		size_t size() const noexcept {
			return m_block.size();
		}

		int size(const dataname_view tag) const noexcept {
			if (contains(tag)) {
				return static_cast<int>(get(tag).size());
			}
			return -1;
		}


		size_t max_size() const noexcept {
			return std::min({ m_block.max_size(), m_item_order.max_size(), m_loops.max_size() });
		}

		// Modifiers

		//Modifiers
		void clear() noexcept {
			m_block.clear();
			m_loops.clear();
			m_item_order.clear();
			overwrite = true;
			return;
		}

		size_t erase(const dataname_view tag) {
			const_iterator r = removeItem(tag);
			if (r == cend())
				return 0;
			return 1;
		}

		// Lookup

		//Lookup
		const Datavalue& at(const dataname_view tag) const {
			return get(tag);
		}

		size_t count(const dataname_view tag) const {
			return m_block.count(tag);
		}

		const_iterator find(const dataname_view tag) const {
			auto it = m_block.find(tag);
			if (it == m_block.end()) {
				return cend();
			}

			return { &(*it), this };
		}

		bool contains(const dataname_view tag) const {
			return m_block.contains(tag);
		}

		//iterator implementation
		//struct Iterator
		//// taken from https://www.internalpointers.com/post/writing-custom-iterators-modern-cpp
		//{
		//	using iterator_category = std::forward_iterator_tag;
		//	using difference_type = std::ptrdiff_t;
		//	using value_type = std::pair<const dataname, datavalue>;
		//	using pointer = value_type*;
		//	using reference = value_type&;
		//
		//	Iterator(pointer m_ptr, Block* blk)
		//		: m_ptr{ m_ptr }, block{ blk } { }
		//	
		//	reference operator*() const { return *m_ptr; }
		//	pointer operator->() { return m_ptr; }
		//
		//	// Prefix increment
		//	Iterator& operator++() { 
		//		dataname currentTag{ m_ptr->first };
		//		auto [loopNum, posn] = block->getItemPosition(currentTag);
		//		itemorder nextItem{};
		//		if (loopNum < 0) { // currentTag is not a loop
		//			if (posn < block->m_item_order.size() - 1) { // not at the end of the item_order
		//				nextItem = block->m_item_order[++posn];
		//				if (nextItem.index() == 0) { // the nextkey is the start of a loop
		//					currentTag = block->m_loops.at(std::get<int>(nextItem))[0];//update the keyname
		//				}
		//				else { //the next key is just a key
		//					currentTag = std::get<dataname>(nextItem); //update the keyname
		//				}
		//				m_ptr = &(*block->m_block.find(currentTag));
		//			}
		//			else {
		//				m_ptr = nullptr;
		//			}
		//		}
		//		else {  //we're in a loop
		//			if (posn < block->m_loops[loopNum].size() - 1) { // not at the end of the loop
		//				currentTag = block->m_loops[loopNum][++posn];
		//				m_ptr = &(*block->m_block.find(currentTag));
		//			}
		//			else { //we're at the end of a loop and need to look at the next element
		//				int currentIndex = row::util::getIndexOf(block->m_item_order, std::variant<int, dataname>{loopNum});
		//				if (currentIndex < block->m_item_order.size() - 1) { // we can go to the next element
		//					nextItem = block->m_item_order[++currentIndex];
		//					if (nextItem.index() == 0) { // the nextkey is the start of a loop
		//						currentTag = block->m_loops.at(std::get<int>(nextItem))[0];//update the keyname
		//					}
		//					else { //the next key is just a key
		//						currentTag = std::get<dataname>(nextItem); //update the keyname
		//					}
		//					m_ptr = &(*block->m_block.find(currentTag));
		//				}
		//				else {
		//					m_ptr = nullptr;
		//				}
		//			}
		//		}
		//		return *this; 
		//	}
		//
		//	// Postfix increment
		//	Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
		//
		//	friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; };
		//	friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; };
		//
		//private:
		//	pointer m_ptr;
		//	Block* block;
		//};

		//// taken from https://www.internalpointers.com/post/writing-custom-iterators-modern-cpp
		struct const_iterator
		{
		public:
			using iterator_category = std::bidirectional_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = std::pair<const dataname, Datavalue>;
			using pointer = value_type*;
			using reference = value_type&;
			using const_pointer = const std::pair<const dataname, Datavalue>*;
			using const_reference = const std::pair<const dataname, Datavalue>&;

		private:
			const_pointer m_ptr;
			const Block* block;

		public:

			//iterator implementation
			const_iterator(const_pointer m_ptr, const Block* blk) : m_ptr{ m_ptr }, block{ blk }
			{

			}

			const_reference operator*() const {
				return *m_ptr;
			}

			const_pointer operator->() {
				return m_ptr;
			}

			const_iterator& operator++() {
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
			// Prefix increment
			const_iterator& operator--() {
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
			// Prefix decrement

			//postfix
			const_iterator operator++(int) {
				const_iterator tmp = *this; ++(*this); return tmp;
			}

			const_iterator operator--(int) {
				const_iterator tmp = *this; --(*this); return tmp;
			}
			

			friend bool operator== (const const_iterator& a, const const_iterator& b) { return a.m_ptr == b.m_ptr; };
			friend bool operator!= (const const_iterator& a, const const_iterator& b) { return a.m_ptr != b.m_ptr; };

		private:
			const_pointer getPtr(size_t idx, dataname& currentTag) {
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

			const_pointer prevPtr(size_t currIndex, dataname& currentTag) {
				return getPtr(--currIndex, currentTag);
			}

			const_pointer nextPtr(size_t currIndex, dataname& currentTag) {
				return getPtr(++currIndex, currentTag);
			}

		};

	private:
		std::pair<const dataname, Datavalue>* ptrToFirstItem() {
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

		std::pair<const dataname, Datavalue>* constptrToFirstItem() const {
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
	};


	class Cif {
	public:
		using blockname = std::string;
		using blockname_view = std::string_view;

	private:
		std::unordered_map<blockname, Block, CaseInsensitiveHash, CaseInsensitiveEqual> m_cif{}; // this is the actual data
		std::vector<blockname> m_block_order{}; // keeps the insertion order

		std::string m_source{};
		bool m_overwrite{ false };
	public:

		//these print_* functions are for debugging purposes

		void print_cif() const {
			std::cout << "---\nCIF tags:\n";
			for (const auto& [k, _] : m_cif) {
				std::cout << k << '\n';
			}
			std::cout << "---\n";
			return;
		}

		void print_block_order() const {
			std::cout << "---\nBlock order:\n";
			for (const auto& block : m_block_order) {
				std::cout << block << '\n';
			}
			std::cout << "---\n";
			return;
		}

		struct const_iterator;

	public:
		Cif() = default;
		explicit Cif(blockname source) : m_source(std::move(source)) {}

		Block& getLastBlock() {
			return m_cif.at(m_block_order.back());
		}

		const Block& getLastBlock() const {
			return m_cif.at(m_block_order.back());
		}

		const blockname& getLastBlockName() const {
			return m_block_order.back();
		}

		const std::string& getSource() const {
			return m_source;
		}


		Block& addName(blockname name) noexcept(false) {
			row::util::toLower_i(name);

			if (!canOverwrite() && contains(name)) {
				throw tag_already_exists_error(name);
			}

			if (!contains(name)) {
				m_block_order.push_back(name);
			}

			m_cif[name];
			m_cif[name].overwrite = m_overwrite;
			return m_cif[name];
		}

		Block& addBlock(blockname name) noexcept(false) {
			return addName(std::move(name));
		}

		const_iterator addBlock(blockname name, Block block) noexcept(false) {
			row::util::toLower_i(name);

			if (!canOverwrite() && contains(name)) {
				throw tag_already_exists_error(name);
			}

			if (!contains(name)) {
				m_block_order.push_back(name);
			}

			m_cif[name] = std::move(block);
			return find(name);
		}

		const_iterator addBlocks(const std::vector<blockname>& names, const std::vector<Block>& blocks) {
			if (names.size() != blocks.size()) {
				throw tag_value_mismatch_error(std::format("{} names and {} blocks", names.size(), blocks.size()));
			}

			for (size_t i = 0; i < names.size(); ++i) {
				addBlock(names[i], blocks[i]);
			}
			return find(names[0]);
		}

		const_iterator removeBlock(const blockname_view name) {
			if (!contains(name)) {
				return cend();
			}
			const_iterator r = ++find(name);
			m_cif.erase(m_cif.find(name));
			std::erase_if(m_block_order, [name](const auto& thing) { return row::util::icompare(thing, name); });

			return r;
		}


		int getBlockPosition(const blockname_view name) const {
			/*A utility function to get the numerical order in the printout
		of `name`.  An item has coordinate `(loop_no,pos)` with
		the top level having a `loop_no` of -1.
		Return -1, -1, indicates doesn't exist.
		*/
			const auto it = std::find_if(m_block_order.cbegin(), m_block_order.cend(), [name](auto& block) {
				return row::util::icompare(name, block);
				});

			if (it != m_block_order.end()) {
				return static_cast<int>(std::distance(m_block_order.cbegin(), it));
			}
			return -1;
		}

		const_iterator changeBlockPosition(const blockname_view name, const size_t newPosn) {
			/*Move the printout order of `name` to `newpos`. If `name` is
				in a loop, `newpos` refers to the order within the loop.*/
			if (!contains(name)) {
				throw no_such_tag_error(std::format("{} does not exist.", name));
			}

			int oldPosn = getBlockPosition(name);
			row::util::move_element(m_block_order, oldPosn, newPosn);

			return find(name);
		}

		bool overwrite(bool ow) {
			if (m_overwrite == ow) {
				return m_overwrite == ow;
			}
			m_overwrite = ow;

			for (auto& [_, block] : m_cif) {
				block.overwrite = m_overwrite;
			}
			return m_overwrite;
		}

		bool canOverwrite() const
		{
			return m_overwrite;
		}


		void print(bool pretty = true) const {
			std::cout << to_string(pretty);
			return;
		}


		std::string to_string(bool pretty = true) const {
			std::string cif{};
			for (const auto& block : m_block_order) {
				cif += "\ndata_" + block + '\n';
				cif += m_cif.at(block).to_string(pretty);
			}
			return cif;
		}

		//iterators

		//iterators
		const_iterator begin() const noexcept {
			return { constptrToFirstBlock(), this };
		}

		const_iterator end() const noexcept {
			return { nullptr, this };
		}

		const_iterator cbegin() const noexcept {
			return { constptrToFirstBlock(), this };
		}

		const_iterator cend() const noexcept {
			return { nullptr, this };
		}


		//capacity
		[[nodiscard]] bool empty() const noexcept {
			return m_cif.empty();
		}

		[[nodiscard]] bool isEmpty() const noexcept {
			return empty();
		}

		size_t size() const noexcept {
			return m_cif.size();
		}

		int size(const blockname_view name) const noexcept {
			if (contains(name)) {
				return static_cast<int>(get(name).size());
			}
			return -1;
		}

		size_t max_size() const noexcept {
			return std::min({ m_cif.max_size(), m_block_order.max_size() });
		}


		// Modifiers
		void clear() noexcept {
			m_cif.clear();
			m_block_order.clear();
			m_overwrite = false;
			return;
		}

		size_t erase(const blockname_view name) {
			const_iterator r{ removeBlock(name) };
			if (r != cend()) {
				return 1;
			}
			return 0;
		}

		const_iterator set(blockname name, Block value) {
			return addBlock(std::move(name), std::move(value));
		}

		const_iterator put(blockname name, Block value) {
			return set(std::move(name), std::move(value));
		}


		// Lookup
		const Block& get(const blockname_view name) const {
			if (contains(name)) {
				return m_cif.find(name)->second; //can't use .at() as it doesn't support the inesnitive case lookup
			}
			throw no_such_tag_error(std::format("{} does not exist.", name));
		}

		const Block& at(const blockname_view name) const {
			return get(name);
		}

		size_t count(const blockname_view name) const {
			return m_cif.count(name);
		}

		const_iterator find(const blockname_view name) const {
			const std::pair<const blockname, Block>* m_ptr = &(*m_cif.find(name));
			return { m_ptr, this };
		}

		bool contains(const blockname_view name) const {
			return m_cif.contains(name);
		}


		//struct Iterator
		//// taken from https://www.internalpointers.com/post/writing-custom-iterators-modern-cpp
		//{
		//	using iterator_category = std::forward_iterator_tag;
		//	using difference_type = std::ptrdiff_t;
		//	using value_type = std::pair<const blockname, Block>;
		//	using pointer = value_type*;
		//	using reference = value_type&;
		//
		//	Iterator(pointer m_ptr, Cif* cif)
		//		: m_ptr{ m_ptr }, cif{ cif } { }
		//
		//	reference operator*() const { return *m_ptr; }
		//	pointer operator->() { return m_ptr; }
		//
		//	// Prefix increment
		//	Iterator& operator++() {
		//		blockname currentBlock{ m_ptr->first };
		//		int posn = cif->getBlockPosition(currentBlock);
		//		if (posn < cif->m_block_order.size() - 1) { // not at the end of the block_order
		//			currentBlock = cif->m_block_order[++posn]; //update the current block to the next one
		//			m_ptr = &(*cif->m_cif.find(currentBlock));
		//		}
		//		else {
		//			m_ptr = nullptr;
		//		}
		//		return *this;
		//	}
		//
		//	// Postfix increment
		//	Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
		//
		//	friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; };
		//	friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; };
		//
		//private:
		//	pointer m_ptr;
		//	Cif* cif;
		//};

		struct const_iterator
		{
			using iterator_category = std::forward_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = std::pair<const blockname, Block>;
			using pointer = value_type*;
			using reference = value_type&;
			using const_pointer = const std::pair<const blockname, Block>*;
			using const_reference = const std::pair<const blockname, Block>&;

			const_iterator(const_pointer m_ptr, const Cif* cif) 
				: m_ptr{ m_ptr }, cif{ cif } {}

			const_reference operator*() const {
				return *m_ptr;
			}

			const_pointer operator->() {
				return m_ptr;
			}

			const_iterator& operator++() {// Prefix increment
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
			
			const_iterator operator++(int) {// Postfix increment
				const_iterator tmp = *this; ++(*this);
				return tmp;
			}
			
			friend bool operator== (const const_iterator& a, const const_iterator& b) { return a.m_ptr == b.m_ptr; };
			friend bool operator!= (const const_iterator& a, const const_iterator& b) { return a.m_ptr != b.m_ptr; };

		private:
			const_pointer m_ptr;
			const Cif* cif;
		};


	private:
		std::pair<const blockname, Block>* ptrToFirstBlock() {
			return &(*m_cif.find(m_block_order[0]));
		}

		std::pair<const blockname, Block>* constptrToFirstBlock() const {
			std::pair<const blockname, Block>* ptr = const_cast<std::pair<const blockname, Block>*>(&(*m_cif.find(m_block_order[0])));
			return ptr;
		}

	};
}

#endif // !ROW_CIFFILE_HPP
