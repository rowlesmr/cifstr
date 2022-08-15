
#include <iostream>
#include <unordered_map>
#include <stdexcept>
#include <algorithm>
#include <string>
#include <vector>
#include <variant>


namespace row::cif2 {

	using dataname = std::string;
	using datavalue = std::vector<std::string>;
	using itemorder = std::variant<int, dataname>;

	template< typename K, typename V>
	using dict = std::unordered_map<K, V>;


	dataname toLower(dataname str) {
		std::transform(str.begin(), str.end(), str.begin(), std::tolower);
		return str;
	}

	std::vector<dataname> toLower(std::vector<dataname> strs) {
		for (dataname& str : strs) {
			std::transform(str.begin(), str.end(), str.begin(), std::tolower);
		}
		return strs;
	}

	template<typename C, typename F>
	bool contains(const C& c, const F& f) {
		return std::find(c.cbegin(), c.cend(), f) != c.cend();
	}


	class Block {
	private:
		dict<dataname, datavalue> m_block{}; // this is the actual data
		dict<int, std::vector<dataname>> m_loops{}; // keeps track of datanames that are looped together
		std::vector<itemorder> m_item_order{}; // keeps the insertion order
		dict<dataname, dataname> m_true_case{}; // keeps the actual case of the keys used.

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


	public:
		bool addItem(const dataname& key, const datavalue& value) {
			dataname lowerKey{ toLower(key) };

			if (!overwrite && this->contains(lowerKey)) {
				return false;
			}

			if (!this->contains(lowerKey) && !this->isInLoop(lowerKey)) {
				m_item_order.push_back(lowerKey);
			}

			if (m_true_case.contains(lowerKey)) {
				m_true_case.erase(lowerKey);
			}
			m_true_case[lowerKey] = key;
			m_block[lowerKey] = value;
			return true;
		}

		bool addItems(std::vector<dataname>& keys, std::vector<datavalue>& values) {
			if (keys.size() != values.size()) {
				return false;
			}

			for (size_t i = 0; i < keys.size(); ++i) {
				if (!addItem(keys[i], values[i])) {
					return false;
				}
			}
			return true;
		}

		bool createLoop(const std::vector<dataname>& keys) {
			const auto& blk = m_block;
			std::vector<dataname> lowerKeys{ toLower(keys) };

			//check that all keys exist, and have all the same length values
			if (!(std::all_of(lowerKeys.cbegin(), lowerKeys.cend(), [&blk](const auto& key) { return blk.contains(key); }))) {
				return false;
			}

			size_t len{ m_block[keys[0]].size() };
			if (!(std::all_of(lowerKeys.cbegin(), lowerKeys.cend(), [&blk, len](const auto& key) { return blk.at(key).size() == len; }))) {
				return false;
			}

			//remove all keys from any existing loops
			for (auto& [_, loopKeys] : m_loops) {
				for (auto& key : lowerKeys) {
					if (row::cif2::contains(loopKeys, key)) {
						auto it = std::find(loopKeys.cbegin(), loopKeys.cend(), key);
						loopKeys.erase(it);
					}
				}
			}

			//remove empty loops
			for (auto& [loopNum, loopKeys] : m_loops) {
				if (loopKeys.size() == 0) {
					m_loops.erase(loopNum);
				}
			}

			//get the loop number to use for the loop we're about to insert
			int loopNum{ 1 };
			if (m_loops.size() > 0) {
				for (auto& [k, v] : m_loops) {
					if (k > loopNum)
						loopNum = k + 1;
				}
			}
			m_loops[loopNum] = lowerKeys;
			m_item_order.emplace_back(loopNum);

			//remove keys from item order
			for (auto& key : m_loops[loopNum]) {
				auto it = m_item_order.begin();
				while (it != m_item_order.end()) {
					if (it->index() == 0) {
						++it;
					}
					else if (std::get<std::string>(*it) == key) {
						it = m_item_order.erase(it);
					}
					else {
						++it;
					}
				}
			}
			return true;
		}


		bool addNameToLoop(const dataname& newName, const dataname& oldName) {
			dataname lowerNew{ toLower(newName) };
			dataname lowerOld{ toLower(oldName) };

			if (!contains(lowerNew)) { return false; }

			int loopNum = findLoop(oldName);
			if (loopNum < 0) { return false; }
			if (row::cif2::contains(m_loops[loopNum], lowerNew)) { return true; }
			if (m_block.at(lowerNew).size() != m_block.at(lowerOld).size()) { return false; }

			// if we get to here, the newName exists, the oldName is in a loop,  newName isn't in the oldName loop, and newName length is the same as oldName's

			//remove from other loops
			for (auto& [_, v] : m_loops) {
				std::erase(v, lowerNew);
			}
			m_loops[loopNum].push_back(lowerNew);
			std::erase(m_item_order, itemorder{ lowerNew });

			return true;

		}


		int findLoop(const std::string& key) const {
			dataname lowerKey{ toLower(key) };

			for (auto& [k, v] : m_loops) {
				if (row::cif2::contains(v, lowerKey)) {
					return k;
				}
			}
			return -1;
		}

		bool getLoopNames(const dataname& key, std::vector<dataname>& keys) const {
			for (auto& [_, vs] : m_loops) {
				if (row::cif2::contains(vs, key)) {
					keys = vs;
					return true;
				}
			}
			return false;
		}

		bool isInLoop(const dataname& key) const {
			return findLoop(key) != -1;
		}


		bool removeItem(const dataname& key) {
			dataname lowerKey{ toLower(key) };

			if (this->contains(lowerKey)) {
				int loopNum = this->findLoop(key);

				m_block.erase(lowerKey);
				m_true_case.erase(lowerKey);

				if (loopNum > 0) {
					std::erase(m_loops[loopNum], lowerKey);

					if (m_loops[loopNum].size() == 0) {
						m_loops.erase(loopNum);
						std::erase_if(m_item_order, [loopNum](const auto& thing) { if (thing.index() == 0) { return std::get<int>(thing) == loopNum; } else { return false; }});
					}
				}
				else {
					std::erase_if(m_item_order, [lowerKey](const auto& thing) { if (thing.index() == 1) { return std::get<std::string>(thing) == lowerKey; } else { return false; }});
				}
				return true;
			}
			return false;
		}


		std::tuple<int, int> getItemPosition(const dataname& key) const {
			/*A utility function to get the numerical order in the printout
		of `key`.  An item has coordinate `(loop_no,pos)` with
		the top level having a `loop_no` of -1.
		Return -1, -1, indicates doesn't exist.
		*/
			dataname lowerKey{ toLower(key) };

			if (!contains(lowerKey)) {
				return std::tuple<int, int> {-1, -1};
			}
			itemorder tmp{ lowerKey };
			if (row::cif2::contains(m_item_order, tmp)) {
				auto it = std::find(m_item_order.cbegin(), m_item_order.cend(), tmp);
				int index = it - m_item_order.cbegin();
				return std::tuple<int, int> {-1, index};
			}
			else {
				int loopNum = findLoop(lowerKey);
				auto it = std::find(m_loops.at(loopNum).cbegin(), m_loops.at(loopNum).cend(), lowerKey);
				int loopPos = it - m_loops.at(loopNum).cbegin();
				return std::tuple<int, int> {loopNum, loopPos};
			}
		}


		bool changeItemPosition(const dataname& key, const size_t newPosn) {
			/*Move the printout order of `key` to `newpos`. If `key` is
			  in a loop, `newpos` refers to the order within the loop.*/
			dataname lowerKey{ toLower(key) };
			if (!contains(lowerKey)) {
				return false;
			}
			auto [loopNum, oldPosn] = getItemPosition(key);
			if (loopNum < 0) {
				move_element(m_item_order, oldPosn, newPosn);
			}
			else {
				move_element(m_loops[loopNum], oldPosn, newPosn);
			}
			return true;
		}


		void print() {
			std::cout << "---\n";
			for (const auto& item : m_item_order) {
				if (item.index() == 0) {
					int loopNum = std::get<int>(item);
					for (const auto& key : m_loops[loopNum]) {
						std::cout << '\t' << key << '\n';
					}

					for (size_t i{ 0 }; i < m_block[m_loops[loopNum][0]].size(); ++i) {
						for (const auto& key : m_loops[loopNum]) {
							std::cout << '\t' << m_block[key][i];
						}
						std::cout << '\n';
					}
				}
				else {
					std::string loopKey = std::get<std::string>(item);
					std::cout << loopKey << '\t' << m_block[loopKey][0] << '\n';
				}
			}
			std::cout << "---\n";
		}




		//Iterators
		dict<dataname, datavalue>::const_iterator begin() const noexcept {
			return m_block.begin();
		}
		dict<dataname, datavalue>::const_iterator end() const noexcept {
			return m_block.end();
		}
		dict<dataname, datavalue>::const_iterator cbegin() const noexcept {
			return m_block.cbegin();
		}
		dict<dataname, datavalue>::const_iterator cend() const noexcept {
			return m_block.cend();
		}

		//capacity
		[[nodiscard]] bool empty() const noexcept {
			return m_block.empty();
		}
		[[nodiscard]] bool isEmpty() const noexcept {
			return m_block.empty();
		}
		size_t size() const noexcept {
			return m_block.size();
		}
		size_t size(const std::string& key) const noexcept {
			if (contains(key)) {
				return m_block.at(key).size();
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

		size_t erase(const dataname& key) {
			return removeItem(key);
		}

		bool set(const dataname& key, const datavalue& value) {
			return this->addItem(key, value);
		}
		bool put(const dataname& key, const datavalue& value) {
			return set(key, value);
		}


		// Lookup

		bool get(const dataname& key, datavalue& value) const {
			if (this->contains(key)) {
				value = m_block.at(key);
				return true;
			}
			return false;
		}


		const datavalue& at(const dataname& key) const {
			return m_block.at(key);
		}
		datavalue& operator[](const dataname& key) {
			return m_block[key];
		}
		datavalue& operator[](dataname& key) {
			return m_block[key];
		}
		size_t count(const dataname& key) const {
			return m_block.count(key);
		}
		dict<dataname, datavalue>::const_iterator find(const dataname& key) const {
			return m_block.find(key);
		}
		bool contains(const dataname& key) const {
			return m_block.contains(key);
		}


		////structured binding
		//template<std::size_t Index>
		//std::tuple_element_t<Index, Block>& get()
		//{
		//	if constexpr (Index == 0) return "name";
		//	if constexpr (Index == 1) return { "name" };
		//}

	private:
		//https://stackoverflow.com/a/57399634/36061
		template <typename T> void move_element(std::vector<T>& v, size_t oldIndex, size_t newIndex)
		{
			if (newIndex >= v.size()) {
				newIndex = v.size() - 1;
			}
			if (newIndex < 0  {
				newIndex = 0;
			}

			if (oldIndex > newIndex)
				std::rotate(v.rend() - oldIndex - 1, v.rend() - oldIndex, v.rend() - newIndex);
			else
				std::rotate(v.begin() + oldIndex, v.begin() + oldIndex + 1, v.begin() + newIndex + 1);
		}

	};








}
