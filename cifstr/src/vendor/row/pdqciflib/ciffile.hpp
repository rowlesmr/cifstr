
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
		Datavalue();
		Datavalue(const std::string& in);
		Datavalue(std::string&& in);
		Datavalue(const std::vector<std::string>& in);
		Datavalue(std::vector<std::string>&& in);
		Datavalue(std::initializer_list<std::string> in);

		bool convert() const;
		void reconvert() const;
		bool isConverted() const;

		//vector access
		const std::vector<std::string>& getStrings() const;
		const std::vector<double>& getDoubles() const;
		const std::vector<double>& getErrors() const;

		//element access
		constexpr const_reference at(size_type pos) const noexcept(false);
		constexpr const_reference str_at(size_type pos) const noexcept(false);
		const_reference_double dbl_at(size_type pos) const noexcept(false);
		const_reference_double err_at(size_type pos) const noexcept(false);

		// extreme iterators
		constexpr const_reference front() const;
		constexpr const_reference front_str() const;
		const_reference_double front_dbl() const;
		const_reference_double front_err() const;

		constexpr const_reference back() const;
		constexpr const_reference back_str() const;
		const_reference_double back_dbl() const;
		const_reference_double back_err() const;

		//data pointer
		constexpr const std::string* data() const noexcept;
		constexpr const std::string* str_data() const noexcept;
		const double* dbl_data() const noexcept;
		const double* err_data() const noexcept;

		//iterators
		const_iterator begin() const;
		const_iterator end() const;
		const_reverse_iterator rbegin() const;
		const_reverse_iterator rend() const;
		const_iterator cbegin() const;
		const_iterator cend() const;
		const_reverse_iterator crbegin() const;
		const_reverse_iterator crend() const;

		const_iterator str_begin() const;
		const_iterator str_end() const;
		const_reverse_iterator str_rbegin() const;
		const_reverse_iterator str_rend() const;
		const_iterator str_cbegin() const;
		const_iterator str_cend() const;
		const_reverse_iterator str_crbegin() const;
		const_reverse_iterator str_crend() const;

		const_iterator_double dbl_begin() const;
		const_iterator_double dbl_end() const;
		const_reverse_iterator_double dbl_rbegin() const;
		const_reverse_iterator_double dbl_rend() const;
		const_iterator_double dbl_cbegin() const;
		const_iterator_double dbl_cend() const;
		const_reverse_iterator_double dbl_crbegin() const;
		const_reverse_iterator_double dbl_crend() const;

		const_iterator_double err_begin() const;
		const_iterator_double err_end() const;
		const_reverse_iterator_double err_rbegin() const;
		const_reverse_iterator_double err_rend() const;
		const_iterator_double err_cbegin() const;
		const_iterator_double err_cend() const;
		const_reverse_iterator_double err_crbegin() const;
		const_reverse_iterator_double err_crend() const;


		//capacity
		[[nodiscard]] constexpr bool empty() const noexcept;
		[[nodiscard]] constexpr bool isEmpty() const noexcept;
		constexpr size_type size() const noexcept;
		constexpr void reserve(size_type new_cap);
		constexpr size_type capacity() const noexcept;
		constexpr void shrink_to_fit();

		//modifiers
		constexpr void clear() noexcept;
		void push_back(const std::string& value);
		void push_back(std::string&& value);
		constexpr void swap(Datavalue& other);



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
		dataname name{}; //the name of the block to which this data belongs -> must be set through the Cif which this Block belongs.

	public:
		bool overwrite{ true };

		//these print_* functions are for debugging purposes
		void print_block() const;
		void print_loops() const;
		void print_item_order() const;
		void print_true_case() const;

		struct const_iterator;

	public:
		Block() = default;
		explicit Block(bool ow);
		explicit Block(dataname name);
		Block(dataname name, bool ow);

		const_iterator addItem(dataname tag, Datavalue value) noexcept(false);
		const_iterator addItems(const std::vector<dataname>& tags, const std::vector<Datavalue>& values) noexcept(false);
		const_iterator addItemsAsLoop(const std::vector<dataname>& tags, const std::vector<Datavalue>& values) noexcept(false);
		const_iterator createLoop(const std::vector<dataname>& tags) noexcept(false);
		const_iterator addNameToLoop(const dataname& newName, const dataname& oldName) noexcept(false);

		int getLoopNum(const std::string& tag) const;

		const std::vector<dataname>& getLoopNames(const dataname& tag) const noexcept(false);

		bool isInLoop(const dataname& tag) const;

		const_iterator removeItem(const dataname& tag);
		Datavalue getAssociatedValue(const dataname& tag, const std::string& value, const dataname& associatedTag);
		std::tuple<int, int> getItemPosition(const dataname& tag) const;
		const_iterator changeItemPosition(const dataname& tag, const size_t newPosn);
		const_iterator changeLoopPosition(const dataname& tag, const size_t newPosn);

		//why is this here?
		std::vector<std::string> getInvalidLoopLengths() const;

		const_iterator set(const dataname& tag, const Datavalue& value);
		const_iterator put(const dataname& tag, const Datavalue& value);
		const Datavalue& get(const dataname& tag) const;
		const Datavalue& get_value(const dataname& tag) const;

		const dataname& getName() const;

		std::vector<dataname> getNames() const;
		std::vector<dataname> tags() const;

		std::vector<Datavalue> getValues() const;
		std::vector<Datavalue> values() const;

		std::string makeStringLength(dataname tag, size_t len) const;

		std::string formatValue(std::string value) const;

		void print(bool pretty = true) const;

		std::string to_string(bool pretty=true) const;


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
			const_iterator(const_pointer m_ptr, const Block* blk);

			const_reference operator*() const;
			const_pointer operator->();

			// Prefix increment
			const_iterator& operator++();

			// Prefix decrement
			const_iterator& operator--();

			// Postfix increment/decrement
			const_iterator operator++(int);
			const_iterator operator--(int);

			friend bool operator== (const const_iterator& a, const const_iterator& b) { return a.m_ptr == b.m_ptr; };
			friend bool operator!= (const const_iterator& a, const const_iterator& b) { return a.m_ptr != b.m_ptr; };

		private:
			const_pointer getPtr(size_t idx, dataname& currentTag);
			const_pointer prevPtr(size_t currIndex, dataname& currentTag);
			const_pointer nextPtr(size_t currIndex, dataname& currentTag);
		};

		//Iterators
		const_iterator begin() const noexcept;
		const_iterator end() const noexcept;

		const_iterator cbegin() const noexcept;
		const_iterator cend() const noexcept;

		//capacity
		[[nodiscard]] bool empty() const noexcept;
		[[nodiscard]] bool isEmpty() const noexcept;
		size_t size() const noexcept;
		int size(const dataname& tag) const noexcept;
		size_t max_size() const noexcept;

		// Modifiers
		void clear() noexcept;
		size_t erase(const dataname& tag);

		// Lookup
		const Datavalue& at(const dataname& tag) const;
		size_t count(const dataname& tag) const;
		const_iterator find(const dataname& tag) const;
		bool contains(const dataname& tag) const;

	private:
		std::pair<const dataname, Datavalue>* ptrToFirstItem();
		std::pair<const dataname, Datavalue>* constptrToFirstItem() const;
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
		void print_cif() const;
		void print_block_order() const;
		void print_true_case() const;

		struct const_iterator;

	public:
		Cif();
		explicit Cif(std::string source);
		Block& getLastBlock();

		const Block& getLastBlock() const;
		const blockname& getLastBlockName() const;
		const std::string& getSource() const;


		Block& addName(const blockname& name) noexcept(false);
		Block& addBlock(const blockname& name) noexcept(false);
		const_iterator addBlock(const blockname& name, const Block& block) noexcept(false);
		const_iterator addBlocks(const std::vector<blockname>& names, const std::vector<Block>& blocks);
		const_iterator removeBlock(const blockname& name);
		int getBlockPosition(const dataname& name) const;
		const_iterator changeBlockPosition(const dataname& name, const size_t newPosn);
		bool overwrite(bool ow);
		bool canOverwrite() const;

		void print(bool pretty = true) const;
		std::string to_string(bool pretty=true) const;

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

			const_iterator(const_pointer m_ptr, const Cif* cif);

			const_reference operator*() const;
			const_pointer operator->();
			const_iterator& operator++(); // Prefix increment
			const_iterator operator++(int); // Postfix increment
			friend bool operator== (const const_iterator& a, const const_iterator& b) { return a.m_ptr == b.m_ptr; };
			friend bool operator!= (const const_iterator& a, const const_iterator& b) { return a.m_ptr != b.m_ptr; };

		private:
			const_pointer m_ptr;
			const Cif* cif;
		};


		const_iterator begin() const noexcept;
		const_iterator end() const noexcept;

		const_iterator cbegin() const noexcept;
		const_iterator cend() const noexcept;

		//capacity
		[[nodiscard]] bool empty() const noexcept;
		[[nodiscard]] bool isEmpty() const noexcept;
		size_t size() const noexcept;
		int size(const std::string& name) const noexcept;
		size_t max_size() const noexcept;

		// Modifiers
		void clear() noexcept;
		size_t erase(const blockname& name);
		const_iterator set(const blockname& name, const Block& value);
		const_iterator put(const blockname& name, const Block& value);


		// Lookup
		const Block& get(const blockname& name) const;
		const Block& at(const blockname& name) const;
		size_t count(const blockname& name) const;
		const_iterator find(const blockname& name) const;
		bool contains(const blockname& name) const;

	private:
		std::pair<const blockname, Block>* ptrToFirstBlock();
		std::pair<const blockname, Block>* constptrToFirstBlock() const;
	};
}

#endif // !ROW_CIFFILE_HPP
