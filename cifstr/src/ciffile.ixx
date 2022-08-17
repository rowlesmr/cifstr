


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

import util;
import cifexcept;

export module ciffile;

export namespace row::cif {

	using dataname = std::string;
	using datanameview = std::string_view;
	template< typename K, typename V>
	using dict = std::unordered_map<K, V>;


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
		Datavalue() {}
		Datavalue(const std::string& in) : m_strs({ in }) {}
		Datavalue(std::string&& in) : m_strs({ std::move(in) }) {}
		Datavalue(const std::vector<std::string>& in) : m_strs( in ) {}
		Datavalue(std::vector<std::string>&& in) : m_strs(std::move(in)) {}
		Datavalue(std::initializer_list<std::string> in) : m_strs{ std::move(in) } {}

		bool convert() const {
			if (m_isConverted){
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

		void reconvert() {
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
		const_reference_double err_at(size_type pos) const noexcept(false){
			convert();
			return m_errs.at(pos);
		}

		// extereme iterators
		constexpr const_reference front() const { return m_strs.front(); }
		constexpr const_reference front_str() const { return front(); }
		const_reference_double front_dbl() const { convert(); return m_dbls.front(); }
		const_reference_double front_err() const { convert();  return m_errs.front(); }

		constexpr const_reference back() const { return m_strs.back(); }
		constexpr const_reference back_str() const { return back(); }
		const_reference_double back_dbl() const { convert(); return m_dbls.back(); }
		const_reference_double back_err() const { convert(); return m_errs.back(); }


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
		const_iterator begin() const { return m_strs.begin(); };
		const_iterator end() const { return m_strs.end(); };
		const_reverse_iterator rbegin() const { return m_strs.rbegin(); };
		const_reverse_iterator rend() const { return m_strs.rend(); };
		const_iterator cbegin() const { return m_strs.cbegin(); };
		const_iterator cend() const { return m_strs.cend(); };
		const_reverse_iterator crbegin() const { return m_strs.crbegin(); };
		const_reverse_iterator crend() const { return m_strs.crend(); };

		const_iterator str_begin() const { return begin(); };
		const_iterator str_end() const { return end(); };
		const_reverse_iterator str_rbegin() const { return rbegin(); };
		const_reverse_iterator str_rend() const { return rend(); };
		const_iterator str_cbegin() const { return cbegin(); };
		const_iterator str_cend() const { return cend(); };
		const_reverse_iterator str_crbegin() const { return crbegin(); };
		const_reverse_iterator str_crend() const { return crend(); };

		const_iterator_double dbl_begin() const { convert(); return m_dbls.begin(); };
		const_iterator_double dbl_end() const { convert(); return m_dbls.end(); };
		const_reverse_iterator_double dbl_rbegin() const { convert(); return m_dbls.rbegin(); };
		const_reverse_iterator_double dbl_rend() const { convert(); return m_dbls.rend(); };
		const_iterator_double dbl_cbegin() const { convert(); return m_dbls.cbegin(); };
		const_iterator_double dbl_cend() const { convert(); return m_dbls.cend(); };
		const_reverse_iterator_double dbl_crbegin() const { convert(); return m_dbls.crbegin(); };
		const_reverse_iterator_double dbl_crend() const { convert(); return m_dbls.crend(); };

		const_iterator_double err_begin() const { convert(); return m_errs.begin(); };
		const_iterator_double err_end() const { convert(); return m_errs.end(); };
		const_reverse_iterator_double err_rbegin() const { convert(); return m_errs.rbegin(); };
		const_reverse_iterator_double err_rend() const { convert(); return m_errs.rend(); };
		const_iterator_double err_cbegin() const { convert(); return m_errs.cbegin(); };
		const_iterator_double err_cend() const { convert(); return m_errs.cend(); };
		const_reverse_iterator_double err_crbegin() const { convert(); return m_errs.crbegin(); };
		const_reverse_iterator_double err_crend() const { convert(); return m_errs.crend(); };


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
		//friend constexpr void swap(Datavalue& lhs, Datavalue& rhs) noexcept(noexcept(lhs.swap(rhs))) {
		//	lhs.swap(rhs);
		//}
	};



	//class ItemOrder {
	//public:
	//	using group_iterator = std::vector<std::vector<dataname>>::iterator;
	//	using const_group_iterator = const std::vector<std::vector<dataname>>::iterator;
	//	using tag_iterator = std::vector<dataname>::iterator;
	//
	//private:
	//	//each element in m_tags is a set of tags. If there is one tag, then it is by itself, if there ane
	//	//  many tags, then they are a group, which implies some semantic meaning exists between them.
	//	std::vector<std::vector<dataname>> m_tags{};
	//	bool m_overwrite{ false };
	//
	//	
	//public:
	//	//these print_* methods are for debugging.
	//	void print_tags() const {
	//		for (const auto& tags : m_tags) {
	//			for (const auto& tag : tags) {
	//				std::cout << tag << '\t';
	//			}
	//			std::cout << '\n';
	//		}
	//	}
	//
	//
	//public:
	//
	//	ItemOrder() = default;
	//	explicit ItemOrder(bool overwrite) : m_overwrite{ overwrite } {}
	//
	//	void /*const_iterator*/ addTag(dataname tag) {
	//		bool exists = contains(tag);
	//		if (!canOverwrite() && exists) {
	//			throw tag_already_exists_error(tag);
	//		}
	//		if (!exists) {
	//			m_tags.push_back({ std::move(tag) });
	//		}
	//
	//		return /*const_iterator*/;
	//	}
	//
	//	void addTags(const std::vector<dataname>& tags) {
	//		//todo: add checks so this is atomic: it either works for all or none.
	//
	//		std::for_each(tags.cbegin(), tags.cend(), [this](auto& tag) { this->addTag(tag); });
	//	}
	//
	//	void addTagsAsGroup(std::vector<dataname> tags) {
	//		//check each one for existance, taking into account the ability to overwrite tags
	//		std::vector<bool> exists{};
	//		bool doRemove{ false };
	//		std::for_each(tags.cbegin(), tags.cend(), [this, &exists, &doRemove](const dataname& tag) {
	//			exists.push_back(this->contains(tag)); 
	//			doRemove = doRemove || exists.back();
	//			if (!this->canOverwrite() && exists.back()) {
	//				throw tag_already_exists_error(tag);
	//			}
	//		});
	//
	//		//if I am overwriting tags, then remove them
	//		if (doRemove) {
	//			for (size_t i{ 0 }; i < exists.size(); ++i) {
	//				if (exists[i]) {
	//					removeTag(tags[i]);
	//				}
	//			}
	//		}
	//
	//		//now I can add my tags
	//		m_tags.push_back({ std::move(tags) });
	//	}
	//
	//	void addTagToGroup(dataname newTag, const dataname& existingTag) {
	//		//get an iterator to the vector containing the vector containing the existing tag.
	//		auto it{ findGroupContaining(existingTag) };
	//
	//		if (it == m_tags.end()) {
	//			throw no_such_tag_error(existingTag);
	//		}
	//		else {
	//			it->push_back(std::move(newTag));
	//		}
	//	}
	//
	//	void removeTag(const dataname& tag) {
	//		//remove the tag - getting the return value of the find_if only because it is [[nodiscard]]
	//		//  I want this function because of the side-effects
	//		auto it{ std::find_if(m_tags.begin(), m_tags.end(), [&tag](std::vector<dataname>& tags) {
	//			auto it = std::find(tags.begin(), tags.end(), tag);
	//			if (it != tags.end()) {
	//				tags.erase(it);
	//				return true;
	//			}
	//			return false;
	//		}) };
	//
	//		//remove zero-length entries
	//		std::erase_if(m_tags, [](std::vector<dataname>& tags) { return tags.size() == 0; });
	//	}
	//
	//	bool isInGroup(const dataname& tag) const {
	//		return findGroupContaining(tag)->size() > 1;
	//	}
	//
	//	bool areInSameGroup(const dataname& tag, const dataname& otherTag) const {
	//		return row::util::contains(findGroupContaining(tag), otherTag);
	//	}
	//
	//	int getLoopNum(const dataname& tag) const {
	//		return -1;
	//	}
	//
	//	vo/d /*const std::vector<dataname>&*/ getGroupTags(const dataname& tag) const {
	//
	//	}
	//
	//	std::tuple<int, int> getTagPosition(const dataname& tag) const {
	//
	//		int x{ -1 };
	//		int y{ -1 };
	//
	//		if (!contains(tag)) {
	//			return std::tuple<int, int> {x, y};
	//		}
	//		auto grp_it = findGroupContaining(tag);
	//		auto tag_it = find(grp_it->cbegin(), grp_it->cend(), tag);
	//
	//		x = grp_it - m_tags.begin();
	//		y = (grp_it->size() > 1) ? tag_it - grp_it->begin() : -1;
	//
	//		return std::tuple<int, int> {x, y};
	//	}
	//
	//	void changeTagPosition(const dataname& tag, size_t newPosn) {
	//		/*Move the printout order of `tag` to `newpos`. If `tag` is
	//			in a loop, `newpos` refers to the order within the loop.*/
	//
	//		auto [x, y] = getTagPosition(tag);
	//
	//		if (y == -1) {
	//			row::util::makeInRange<size_t>(newPosn, 0, m_tags.size() - 1);
	//			auto it = m_tags.begin();
	//			std::rotate(it + newPosn, it + x, it + x + 1);
	//		}
	//		else {
	//			row::util::makeInRange<size_t>(newPosn, 0, m_tags[y].size() - 1);
	//			auto it = m_tags[y].begin();
	//			std::rotate(it + newPosn, it + x, it + x + 1);
	//		}
	//	}
	//
	//	void changeGroupPosition(const dataname& tag, size_t newPosn) {
	//		/*Move the printout order of the group containing `tag` to `newpos`.*/
	//
	//		auto [x, _] = getTagPosition(tag);
	//
	//		row::util::makeInRange<size_t>(newPosn, 0, m_tags.size() - 1);
	//		auto it = m_tags.begin();
	//		std::rotate(it + newPosn, it + x, it + x + 1);
	//}
	//
	//	bool canOverwrite() {
	//		return m_overwrite;
	//	}
	//	void overwrite(bool ow) {
	//		m_overwrite = ow;
	//	}
	//
	//
	//	bool contains(const dataname& findMe) const {
	//		return findGroupContaining(findMe) != m_tags.end();
	//	}
	//
	//	group_iterator findGroupContaining(const dataname& tag) {
	//		return std::find_if(m_tags.begin(), m_tags.end(), [&tag](auto& tags) {
	//			return std::find(tags.begin(), tags.end(), tag) != tags.end();
	//			});
	//	}
	//
	//	const_group_iterator findGroupContaining(const dataname& tag) const {
	//		return findGroupContaining(tag);
	//	}
	//};



	class Block {
	public:
		using itemorder = std::variant<int, dataname>;
		//using item = typename dict<dataname, Datavalue>::value_type;

	private:
		dict<dataname, Datavalue> m_block{}; // this is the actual data
		dict<int, std::vector<dataname>> m_loops{}; // keeps track of datanames that are looped together
		std::vector<itemorder> m_item_order{}; // keeps the insertion order
		dict<dataname, dataname> m_true_case{}; // keeps the actual case of the tags used.

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
		}

		void print_item_order() const {
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

		void print_true_case() const {
			std::cout << "---\nTrue case:\n";
			for (auto& [k, v] : m_true_case) {
				std::cout << k << " : " << v << ", ";
			}
			std::cout << "---\n";
		}

		struct const_iterator;

	public:
		Block() = default;
		explicit Block(bool ow) : overwrite(ow) {}


		const_iterator addItem(dataname tag, Datavalue value) noexcept(false) {
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
			const auto&[it, _] = m_block.insert({ lowerTag, std::move(value) });
			// need to convert the iterator to one of mine
			return const_iterator(&(*it), this);
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
			size_t len{ values[0].size()};
			if (!(std::all_of(values.cbegin(), values.cend(), [len](const auto& value) { return value.size() == len; }))) {
				throw loop_length_mismatch_error("Different number of values per tag in loop");
			}

			addItems(tags, values);
			return createLoop(tags);
		}

		const_iterator createLoop(const std::vector<dataname>& tags) noexcept(false) {
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

		const_iterator addNameToLoop(const dataname& newName, const dataname& oldName) noexcept(false) {
			dataname lowerNew{ row::util::toLower(newName) };
			dataname lowerOld{ row::util::toLower(oldName) };

			if (!contains(lowerNew)) { throw no_such_tag_error(std::format("{} does not exist.", lowerNew)); }
			if (m_block.at(lowerNew).size() != m_block.at(lowerOld).size()) { throw loop_length_mismatch_error(std::format("{} new values and {} existing values.", m_block.at(lowerNew).size(), m_block.at(lowerOld).size()));; }

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

		int getLoopNum(const std::string& tag) const {
			dataname lowerTag{ row::util::toLower(tag) };

			for (auto& [k, v] : m_loops) {
				if (row::util::contains(v, lowerTag)) {
					return k;
				}
			}
			return -1;
		}

		const std::vector<dataname>& getLoopNames(const dataname& tag) const noexcept(false) {
			for (auto& [_, vs] : m_loops) {
				if (row::util::contains(vs, tag)) {
					return vs;
				}
			}
			throw no_such_tag_error(std::format("{} does not exist in a loop.", tag));
		}

		bool isInLoop(const dataname& tag) const {
			return getLoopNum(tag) != -1;
		}

		const_iterator removeItem(const dataname& tag) {
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

		Datavalue getAssociatedValue(const dataname& tag, const std::string& value, const dataname& associatedTag) {
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
			
			int i{ row::util::getIndexOf(get(tag).getStrings(), value)};

			return Datavalue{ get(associatedTag).getStrings()[i] };
		}

		std::tuple<int, int> getItemPosition(const dataname& tag) const {
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

		const_iterator changeItemPosition(const dataname& tag, const size_t newPosn) {
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

		const_iterator changeLoopPosition(const dataname& tag, const size_t newPosn) {
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

		//why is this here?
		std::vector<std::string> getInvalidLoopLengths() const {
			std::vector<std::string> r{};
			for (const auto& [k, vs] : m_loops) {
				bool invalidLoopLength{ false };
				size_t loopLen{vs[0].size()};
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

		const_iterator set(const dataname& tag, const Datavalue& value) {
			return addItem(tag, value);
		}
		const_iterator put(const dataname& tag, const Datavalue& value) {
			return set(tag, value);
		}
		const Datavalue& get(const dataname& tag) const {
			dataname lowerTag{ row::util::toLower(tag) };
			if (contains(lowerTag)) {
				return m_block.at(lowerTag);
			}
			throw no_such_tag_error(std::format("{} does not exist.", lowerTag));
		}

		std::vector<dataname> getNames() const {
			std::vector<dataname> names{};			
			for (const auto& [k, _] : *this) {
				names.push_back(k);
			}
			return names;
		}
		std::vector<dataname> tags() const {
			return getNames();
		}

		std::vector<Datavalue> getValues() const {
			std::vector<Datavalue> values{};
			for (const auto& [_, v] : *this) {
				values.push_back(v);
			}
			return values;
		}
		std::vector<Datavalue> values() const {
			return getValues();
		}

		std::string makeStringLength(dataname tag, size_t len) const {
			return std::format("{1:{0}}", len, std::move(tag));
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
		}

		std::string to_string(bool pretty=true) const {
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
					size_t loopWidth{ m_loops.at(loopNum).size()};

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
			const_iterator(const_pointer m_ptr, const Block* blk)
				: m_ptr{ m_ptr }, block{ blk } { }

			const_reference operator*() const { return *m_ptr; }
			const_pointer operator->() { return m_ptr; }

			// Prefix increment
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

			// Prefix decrement
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

			// Postfix increment/decrement
			const_iterator operator++(int) { const_iterator tmp = *this; ++(*this); return tmp; }
			const_iterator operator--(int) { const_iterator tmp = *this; --(*this); return tmp; }

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

		//Iterators
		const_iterator begin() const noexcept {
			return const_iterator(constptrToFirstItem(), this);
		}
		const_iterator end() const noexcept {
			return const_iterator(nullptr, this);
		}

		const_iterator cbegin() const noexcept {
			return const_iterator(constptrToFirstItem(), this);
		}
		const_iterator cend() const noexcept {
			return const_iterator(nullptr, this);
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
		int size(const dataname& tag) const noexcept {
			if (contains(tag)) {
				return static_cast<int>(m_block.at(tag).size());
			}
			return -1;
		}
		size_t max_size() const noexcept {
			return std::min({ m_block.max_size(), m_item_order.max_size(), m_loops.max_size(), m_true_case.max_size() });
		}

		// Modifiers
		void clear() noexcept {
			m_block.clear();
			m_loops.clear();
			m_item_order.clear();
			m_true_case.clear();
			overwrite = true;
		}
		size_t erase(const dataname& tag) {
			const_iterator r = removeItem(tag);
			if (r == this->cend())
				return 0;
			else
				return 1;
		}

		// Lookup
		const Datavalue& at(const dataname& tag) const {
			dataname lowerTag{ row::util::toLower(tag) };
			return m_block.at(lowerTag);
		}
		
		size_t count(const dataname& tag) const {
			dataname lowerTag{ row::util::toLower(tag) };
			return m_block.count(lowerTag);
		}
		
		const_iterator find(const dataname& tag) const {
			dataname lowerTag{ row::util::toLower(tag) };

			auto it = m_block.find(lowerTag);
			if (it == m_block.end()) {
				return cend();
			}

			return const_iterator(&(*it), this);
		}
		
		bool contains(const dataname& tag) const {
			dataname lowerTag{ row::util::toLower(tag) };
			return m_block.contains(lowerTag);
		}



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

	private:
		dict<blockname, Block> m_cif{}; // this is the actual data
		std::vector<blockname> m_block_order{}; // keeps the insertion order
		dict<blockname, blockname> m_true_case{}; // keeps the actual case of the blocknames used.

		std::string m_source{};
		bool m_overwrite{ true };
	public:

		//these print_* functions are for debugging purposes
		void print_cif() const {
			std::cout << "---\nCIF tags:\n";
			for (const auto& [k, _] : m_cif) {
				std::cout << k << '\n';
			}
			std::cout << "---\n";
		}

		void print_block_order() const {
			std::cout << "---\nBlock order:\n";
			for (const auto& block : m_block_order) {
				std::cout << block << '\n';
			}
			std::cout << "---\n";
		}

		void print_true_case() const {
			std::cout << "---\nTrue case:\n";
			for (const auto& [k, v] : m_true_case) {
				std::cout << k << " : " << v << ", ";
			}
			std::cout << "\n---\n";
		}

		struct const_iterator;
	public:
		Cif() {}
		explicit Cif(std::string source) : m_source(std::move(source)) {}

		Block& getLastBlock() {
			return m_cif.at(m_block_order.back());
		}

		const Block& getLastBlock() const {
			return m_cif.at(m_block_order.back());
		}

		const std::string& getSource() const {
			return m_source;
		}


		Block& addName(const blockname& name) noexcept(false) {
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

		Block& addBlock(const blockname& name) noexcept(false) {
			return addName(name);
		}

		const_iterator addBlock(const blockname& name, const Block& block) noexcept(false) {
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

		const_iterator addBlocks(const std::vector<blockname>& names, const std::vector<Block>& blocks) {
			if (names.size() != blocks.size()) {
				throw tag_value_mismatch_error(std::format("{} names and {} blocks", names.size(), blocks.size()));
			}

			for (size_t i = 0; i < names.size(); ++i) {
				addBlock(names[i], blocks[i]);
			}
			return find(row::util::toLower(names[0]));
		}

		const_iterator removeBlock(const blockname& name) {
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

		int getBlockPosition(const dataname& name) const {
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

		const_iterator changeBlockPosition(const dataname& name, const size_t newPosn) {
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

		bool canOverwrite() const {
			return m_overwrite;
		}

		void print(bool pretty = true) const {
			std::cout << to_string(pretty);
		}

		std::string to_string(bool pretty=true) const {
			std::string cif{};
			for (const auto& block : m_block_order) {
				cif += "\ndata_" + block + '\n';
				cif += m_cif.at(block).to_string(pretty);
			}
			return cif;
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
				: m_ptr{ m_ptr }, cif{ cif } { }

			const_reference operator*() const { return *m_ptr; }
			const_pointer operator->() { return m_ptr; }

			// Prefix increment
			const_iterator& operator++() {
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

			// Postfix increment
			const_iterator operator++(int) { const_iterator tmp = *this; ++(*this); return tmp; }

			friend bool operator== (const const_iterator& a, const const_iterator& b) { return a.m_ptr == b.m_ptr; };
			friend bool operator!= (const const_iterator& a, const const_iterator& b) { return a.m_ptr != b.m_ptr; };


		private:
			const_pointer m_ptr;
			const Cif* cif;
		};


		const_iterator begin() const noexcept {
			return const_iterator(constptrToFirstBlock(), this);
		}
		const_iterator end() const noexcept {
			return const_iterator(nullptr, this);
		}

		const_iterator cbegin() const noexcept {
			return const_iterator(constptrToFirstBlock(), this);
		}
		const_iterator cend() const noexcept {
			return const_iterator(nullptr, this);
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
		int size(const std::string& name) const noexcept {
			dataname lowerTag{ row::util::toLower(name) };
			if (contains(lowerTag)) {
				return static_cast<int>(m_cif.at(lowerTag).size());
			}
			return -1;
		}
		size_t max_size() const noexcept {
			return std::min({ m_cif.max_size(), m_block_order.max_size(), m_true_case.max_size() });
		}

		// Modifiers
		void clear() noexcept {
			m_cif.clear();
			m_block_order.clear();
			m_true_case.clear();
			m_overwrite = true;
		}
		size_t erase(const blockname& name) {
			dataname lowerTag{ row::util::toLower(name) };
			const_iterator r{ removeBlock(lowerTag) };
			if (r != cend()) { return 1; }
			else { return 0; }
		}
		const_iterator set(const blockname& name, const Block& value) {
			return addBlock(name, value);
		}
		const_iterator put(const blockname& name, const Block& value) {
			return set(name, value);
		}


		// Lookup
		const Block& get(const blockname& name) const {
			dataname lowerTag{ row::util::toLower(name) };
			if (contains(name)) {
				return m_cif.at(name);
			}
			throw no_such_tag_error(std::format("{} does not exist.", lowerTag));
		}
		const Block& at(const blockname& name) const {
			dataname lowerTag{ row::util::toLower(name) };
			return m_cif.at(lowerTag);
		}
		size_t count(const blockname& name) const {
			dataname lowerTag{ row::util::toLower(name) };
			return m_cif.count(lowerTag);
		}
		const_iterator find(const blockname& name) const {
			dataname lowerTag{ row::util::toLower(name) };
			std::pair<const blockname, Block>* m_ptr = const_cast<std::pair<const blockname, Block>*>(&(*m_cif.find(lowerTag)));
			return const_iterator(m_ptr, this);
		}
		bool contains(const blockname& name) const {
			dataname lowerTag{ row::util::toLower(name) };
			return m_cif.contains(lowerTag);
		}


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

