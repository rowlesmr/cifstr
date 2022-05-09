


#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <variant>
#include <format>

export module ciffile;



namespace row {
    namespace cif {

        struct ItemIndex {
            size_t item{ SIZE_MAX };
            size_t loop{ SIZE_MAX };

            ItemIndex(size_t i = SIZE_MAX, size_t j = SIZE_MAX)
                : item{ i }, loop{ j } {}
        };

        export struct LoopPair {
            std::string tag{};
            std::vector<std::string> values{};

            LoopPair() = default;

            explicit LoopPair(std::string t)
                : tag{ std::move(t) }, values{} {}
           
            LoopPair(std::string t, std::vector<std::string> v)
                : tag{ std::move(t) }, values{ std::move(v) } {}


            const std::vector<std::string>& get_values() const noexcept {
                return values;
            }
            const std::vector<std::string>& get_values(const std::string& t) const noexcept(false) {
                if (t == tag) {
                    return values;
                }
                else {
                    throw std::out_of_range(std::format("Tag {0} not found.", t));
                }
            }

            bool has_tag(const std::string& t) const noexcept {
                return t == tag;
            }

            const std::string& get_tag() const noexcept {
                return tag;
            }

            friend std::ostream& operator<<(std::ostream& o, LoopPair const& lp) {
                o << lp.tag << '\n';
                for (const std::string& v : lp.values)
                    o << '\t' << v << '\n';
                return o;
            }
        };

        export struct Loop {
            std::vector<LoopPair> lpairs;
            size_t currentAppend{ 0 };
            size_t totalValues{ 0 };

            Loop() = default;

            Loop(std::vector<LoopPair> lps)
                : lpairs{ std::move(lps) } {}

            bool has_tag(const std::string& t, size_t& ti) const noexcept {
                for (size_t i{ 0 }; i < lpairs.size(); ++i) {
                    if (lpairs[i].has_tag(t)) {
                        ti = i;
                        return true;
                    }
                }
                ti = SIZE_MAX;
                return false;
            }

            bool has_tag(const std::string& t) const noexcept {
                size_t i;
                return has_tag(t, i);
            }

            size_t find_tag(const std::string& t) const noexcept {
                size_t i;
                has_tag(t, i);
                return i;
            }

            const std::vector<std::string>& get_values(const std::string& t) const noexcept(false) {
                size_t i = find_tag(t);
                if (i != SIZE_MAX)
                    return lpairs[i].get_values();
                throw std::out_of_range(std::format("Tag {0} not found.", t));
            }

            const std::vector<std::string>& get_value(const std::string& t) const noexcept(false) {
                return get_values(t);
            }

            bool remove_item(const std::string& t) {
                return std::erase_if(lpairs, [&t](row::cif::LoopPair& lp) { return lp.has_tag(t); }) == 1;
            }

            friend std::ostream& operator<<(std::ostream& o, Loop const& loop) {
                size_t total_values = loop.lpairs.size() * loop.lpairs[0].values.size();
                size_t total_tags = loop.lpairs.size();
                //tags
                for (const LoopPair& p : loop.lpairs) {
                    o << p.tag << '\t';
                }
                //values
                for (size_t k = 0; k < total_values; ++k) {
                    size_t col = k % total_tags;
                    size_t row = k / total_tags;
                    if (col == 0)
                        o << '\n';
                    o << loop.lpairs[col].values[row] << '\t';
                }
                return o;
            }
        };

        export struct Pair {
            // if the value is stored in a vector, then the return type for all values is a vector<string>
            //  if it is only one element, then just use that one. More than one, then you have a loop.
            //Trust me. It's easier this way.
            std::string tag{};
            std::vector<std::string> value{};

            Pair() = default;

            explicit Pair(std::string t)
                : tag{ std::move(t) }, value{} {}

            Pair(std::string t, std::string v)
                : tag{ std::move(t) }, value{ std::move(v) } {}

            const std::vector<std::string>& get_value() const noexcept{
                return value;
            }

            const std::vector<std::string>& get_value(const std::string& t) const noexcept(false) {
                if (has_tag(t)) {
                    return value;
                }
                else {
                    throw std::out_of_range(std::format("Tag {0} not found.", t));
                }
            }

            bool has_tag(const std::string& t) const noexcept {
                return t == tag;
            }

            const std::string& get_tag() const noexcept {
                return tag;
            }

            friend std::ostream& operator<<(std::ostream& o, Pair const& p) {
                o << p.tag << '\t' << p.value[0];
                return o;
            }
        };

        export struct LoopArg {}; // used only as arguments when creating Item

        enum class ItemType {
            Pair,
            Loop
        };


        export struct Item {
            std::variant<Pair, Loop> data;

            explicit Item(LoopArg)
                : data{ Loop{} } {}

            explicit Item(std::string t)
                : data{ Pair(t) } {}

            Item(std::string t, std::string v)
                : data{ Pair(t,v) } {}

            bool has_tag(const std::string& t) const noexcept {
                if (is_pair()) {
                    return std::get_if<Pair>(&data)->has_tag(t);
                }
                else if (is_loop()) {
                    return std::get_if<Loop>(&data)->has_tag(t);
                }
                return false;
            }

            const std::vector<std::string>& get_value(const std::string& t) const noexcept(false) {
                if (is_pair()) {
                    return std::get_if<Pair>(&data)->get_value(t);
                }
                else if (is_loop()) {
                    return std::get_if<Loop>(&data)->get_values(t);
                }
                throw std::out_of_range(std::format("Tag {0} not found.", t));
            }

            bool is_loop() const noexcept {
                return std::holds_alternative<Loop>(data);
            }

            bool is_pair() const noexcept {
                return std::holds_alternative<Pair>(data);
            }

            friend std::ostream& operator<<(std::ostream& o, const Item& item) {
                if (item.is_pair())
                    o << *std::get_if<Pair>(&(item.data)) << '\n';
                else //should just be a Loop
                    o << *std::get_if<Loop>(&(item.data)) << '\n';
                return o;
            }
        };


        export struct Block {
            std::string name{};
            std::string pd_block_id{};
            std::vector<Item> items{};

            explicit Block(std::string in_name)
                : name{ std::move(in_name) }, pd_block_id{ name }, items{} {}

            bool has_tag(const std::string& t) const noexcept {
                ItemIndex idx{ find_tag(t) };
                return idx.item != SIZE_MAX;
            }

            ItemIndex find_tag(const std::string& t) const noexcept {
                ItemIndex idx{};
                for (size_t i{ 0 }; i < items.size(); ++i) {
                    const Item& item = items[i];
                    if (item.is_loop()) {
                        idx.loop = std::get_if<Loop>(&items[i].data)->find_tag(t);
                        if (idx.loop != SIZE_MAX) {
                            idx.item = i;
                            return idx;
                        }
                    }
                    else if (item.is_pair()) {
                        if (std::get_if<Pair>(&items[i].data)->has_tag(t)) {
                            idx.item = i;
                            return idx;
                        }
                    }
                }
                return idx;
            }

            const std::vector<std::string>& get_value(const ItemIndex& idx) const noexcept(false) {
                if (idx.item == SIZE_MAX) {
                    throw std::out_of_range("Tag not found.");
                }
                else if (idx.loop == SIZE_MAX) { //it's a pair
                    return std::get_if<Pair>(&items[idx.item].data)->get_value();
                }
                else { //it's a loop
                    return std::get_if<Loop>(&items[idx.item].data)->lpairs[idx.loop].get_values();
                }
            }

            const std::vector<std::string>& get_value(const std::string& t) const noexcept(false) {
                ItemIndex idx = find_tag(t);
                try {
                    return get_value(idx);
                }
                catch ([[maybe_unused]] std::runtime_error& e) {
                    throw std::out_of_range(std::format("Tag \"{0}\" not found.", t));
                }
            }

            bool is_loop(const std::string& t) const noexcept {
                ItemIndex idx = find_tag(t);
                return idx.loop != SIZE_MAX && idx.item != SIZE_MAX;
            }

            bool is_pair(const std::string& t) const noexcept {
                ItemIndex idx = find_tag(t);
                return idx.loop == SIZE_MAX && idx.item != SIZE_MAX;
            }

            bool remove_tag(const std::string& t) {
                ItemIndex idx = find_tag(t);
                if (idx.item == SIZE_MAX) {
                    return false;
                }
                else if (idx.loop == SIZE_MAX) { //it's a Pair
                    items.erase(items.begin() + idx.item);
                    return true;
                }
                else { //it's a loop
                    Loop& loop = *std::get_if<Loop>(&items[idx.item].data);
                    loop.lpairs.erase(loop.lpairs.begin() + idx.loop);
                    //also need to check if the entire loop is empty!
                    if (loop.lpairs.empty()) {
                        items.erase(items.begin() + idx.item);
                    }
                    return true;
                }
            }

            bool remove_loop_containing_tag(const std::string& t) {
                ItemIndex idx = find_tag(t);
                if (idx.item == SIZE_MAX || idx.loop == SIZE_MAX) {
                    return false;
                }
                else { //it's a loop
                    items.erase(items.begin() + idx.item);
                    return true;
                }
            }

            friend std::ostream& operator<<(std::ostream& o, const Block& block) {
                o << "---------------\nBlock:\t" << block.name << '\n';
                for (const Item& item : block.items) {
                    o << "---------------\n" << item;
                }
                o << "---------------\n";
                return o;
            }

        };

        export struct Cif {
            std::string source{};
            std::vector<Block> blocks{};
            std::vector<Item>* items_{ nullptr }; //this is used to point to the items in the current block for when I'm doing the initial parsing.
            std::string* blockcode_{ nullptr }; // this points to the name of the current block for when I'm doing the initial parsing

            const Block& get_block(std::string name) const noexcept(false) {
                for (const Block& block : blocks) {
                    if (block.name == name)
                        return block;
                }
                throw std::out_of_range(std::format("Block \"{0}\" not found.", name));
            }

            const Block& get_block_by_id(std::string id) const noexcept(false) {
                for (const Block& block : blocks) {
                    if (block.pd_block_id == id)
                        return block;
                }
                throw std::out_of_range(std::format("pd_block_id \"{0}\" not found.", id));
            }

            friend std::ostream& operator<<(std::ostream& o, const Cif& cif) {
                o << "###############\nCIF:\t";
                o << cif.source << '\n';
                for (const Block& block : cif.blocks) {
                    o << "###############\n";
                    o << block;
                }
                o << "###############\n";
                return o;
            }
        };


    } //end namespace cif
} // end namespace row




