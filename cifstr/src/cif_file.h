

#ifndef ROW_CIF_FILE_
#define ROW_CIF_FILE_

#define _USE_MATH_DEFINES
#include <cmath>
// leave ^ include up there. Moving it down breaks the M_PI value

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <vector>
//#include <stdexcept>

#include <limits>
#include <assert.h>




namespace row {
   namespace cif {

      struct ItemIndex {
         size_t item{ SIZE_MAX };
         size_t loop{ SIZE_MAX };

         ItemIndex(size_t i = SIZE_MAX, size_t j = SIZE_MAX)
            : item{ i }, loop{ j } {}
      };

      struct LoopPair {
         std::string tag{};
         std::vector<std::string> values{};
         bool all_empty{ true };

         LoopPair() 
            : tag{ }, values{  } {}

         LoopPair(std::string t) 
            : tag{ std::move(t) }, values{  } {}

         LoopPair(std::string t, std::vector<std::string> v)
            : tag{ std::move(t) }, values{ std::move(v) } {}

         const std::vector<std::string>& get_values() const {
            return values;
         }
         const std::vector<std::string>& get_values(const std::string& t) const noexcept(false) {
            if (t == tag) {
               return values;
            }
            else {
               throw std::out_of_range("Tag does not exist.");
            }
         }

         bool has_tag(const std::string& t) const {
            return t == tag;
         }
      };

      struct Loop {
         std::vector<LoopPair> lpairs;
         size_t currentAppend{ 0 };
         size_t totalValues{ 0 };

         Loop() = default;
            
         Loop(std::vector<LoopPair> lps)
            : lpairs{ std::move(lps) } {}

         bool has_tag(const std::string& t, size_t& ti) const {
            for (size_t i{ 0 }; i < lpairs.size(); ++i) {
               if (lpairs[i].has_tag(t)) {
                  ti = i;
                  return true;
               }
            }
            ti = SIZE_MAX;
            return false;
         }

         bool has_tag(const std::string& t) const {
            size_t i;
            return has_tag(t, i);
         }

         size_t find_tag(const std::string& t) const {
            size_t i;
            has_tag(t, i);
            return i;
         }

         const std::vector<std::string>& get_values(const std::string& t) const noexcept(false) {
            size_t i = find_tag(t);
            if (i != SIZE_MAX)
               return lpairs[i].values;
            throw std::out_of_range("Tag does not exist.");
         }

         const std::vector<std::string>& get_value(const std::string& t) const noexcept(false) {
            return get_values(t);
         }

         bool remove_item(const std::string& t) {
            return std::erase_if(lpairs, [&t](row::cif::LoopPair& lp) { return lp.has_tag(t); }) != 0;
         }

      };

      struct Pair {
         std::string tag{};
         // if the value is stored in a vector, then the return type for all values is a vector<string>
         //  if it is only one element, then just use that one. More than one, then you have a loop.
         //Trust me. It's easier this way.
         std::vector<std::string> value{};

         Pair() = default;

         Pair(std::string t)
            : tag{ std::move(t) }, value{ } {}

         Pair(std::string t, std::string v) 
            : tag{ std::move(t) }, value{ std::move(v) } {}

         const std::vector<std::string>& get_value() const {
            return value;
         }

         const std::vector<std::string>& get_value(const std::string& t) const noexcept(false) {
            if (has_tag(t)) {
               return value;
            }
            else {
               throw std::out_of_range("Tag not found.");
            }
         }

         bool has_tag(const std::string& t) const {
            return t == tag;
         }
      };

      struct LoopArg {}; // used only as arguments when creating Item

      enum class ItemType {
         Pair,
         Loop
      };


      struct Item {
         ItemType type;
         union {
            Pair pair;
            Loop loop;
         };

         explicit Item(LoopArg)
            : type{ ItemType::Loop }, loop{} {}

         explicit Item(std::string t)
            : type{ ItemType::Pair }, pair{ std::move(t) } {}
         
         Item(std::string t, std::string v)
            : type{ ItemType::Pair }, pair{ std::move(t), std::move(v) } {}


         bool has_tag(const std::string& t) const {
            if (is_pair()) {
               return pair.has_tag(t);
            }
            else if (is_loop()) {
               return loop.has_tag(t);
            }
            else {
               return false;
            }
         }

         const std::vector<std::string>& get_value(const std::string& t) const noexcept(false) {
            if (is_pair()) {
               return pair.get_value(t);
            }
            else if (is_loop()) {
               return loop.get_value(t);
            }
            throw std::out_of_range("Tag not found.");
         }


         bool is_loop() const {
            return type == ItemType::Loop;
         }

         bool is_pair() const {
            return type == ItemType::Pair;
         }

         //Rule of five - I'm using a union, so I need to do all the manual memory management
         // I'm using a union because I want to be able to pass references to the data, rather than do it by value, as in std::variant.
         // see https://en.cppreference.com/w/cpp/language/rule_of_three, https://riptutorial.com/cplusplus/example/5421/rule-of-five
         ~Item() { //destructor
            destroy();
         }
         //copy
         Item(const Item& o) // constructor
            : type{ o.type } {
            copyItem(o);
         }
         Item& operator=(const Item& o) { // assignment
            if (o.type == type) {
               copyItem(o);
            }
            else {
               destroy();
               type = o.type;
               copyItem(o);
            }
            return *this;
         }
         //move
         Item(Item&& o) noexcept
            : type{ o.type }  { // constructor
            moveItem(std::move(o));
         }
         Item& operator=(Item&& o) noexcept { // assignement
            if (o.type == type) {
               moveItem(std::move(o));
            }
            else {
               destroy();
               type = o.type;
               moveItem(std::move(o));
            }
            return *this;
         }

      private:
         void destroy() {
            switch (type) {
            case ItemType::Pair: pair.~Pair(); break;
            case ItemType::Loop: loop.~Loop(); break;
            }
         }

         void copyItem(const Item& o) {
            switch (type) {
            case ItemType::Pair: new (&pair) Pair(o.pair); break; //placement new operator. It reads as:
            case ItemType::Loop: new (&loop) Loop(o.loop); break; // at the address of loop, make a copy of o.loop using the copy constructor
            }
         }

         void moveItem(Item&& o) {
            switch (type) {
            case ItemType::Pair: new (&pair) Pair(std::move(o.pair)); break; //placement new operator. It reads as:
            case ItemType::Loop: new (&loop) Loop(std::move(o.loop)); break; // at the address of loop, move o.loop using the move constructor
            }
         }

      };

      struct Block {
         std::string name{};
         std::string pd_block_id{};
         std::vector<Item> items{};

         Block(std::string in_name) 
            : name{ std::move(in_name) }, pd_block_id{ name }, items{} {}

         bool has_tag(const std::string& t) const {
            ItemIndex idx{ find_tag(t) };
            return idx.item != SIZE_MAX;
         }

         ItemIndex find_tag(const std::string& t) const {
            ItemIndex idx{};
            for (size_t i{ 0 }; i < items.size(); ++i) {
               const Item& item = items[i];
               if (item.is_loop()) {
                  idx.loop = items[i].loop.find_tag(t);
                  if (idx.loop != SIZE_MAX) {
                     idx.item = i;
                     return idx;
                  }
               }
               else if (item.is_pair()) {
                  if (items[i].pair.has_tag(t)) {
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
               return items[idx.item].pair.value;
            }
            else { //it's a loop
               return items[idx.item].loop.lpairs[idx.loop].values;
            }
         }

         const std::vector<std::string>& get_value(const std::string& t) const  noexcept(false) {
            ItemIndex idx = find_tag(t);
            return get_value(idx);
         }

         bool is_loop(const std::string& t) const  {
            ItemIndex idx = find_tag(t);
            return  idx.loop != SIZE_MAX && idx.item != SIZE_MAX ;
         }
        
         bool is_pair(const std::string& t) const  {
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
               Loop& loop = items[idx.item].loop;
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
      };

      struct Cif {
         std::string source{};
         std::vector<Block> blocks{};
         std::vector<Item>* items_ = nullptr; //this is used to point to the items in the current block for when I'm doing the initial parsing.
         std::string* blockcode_ = nullptr; // this points to the name of the current block for when I'm doing the initial parsing

         const Block& get_block(std::string name) const noexcept(false) {
            for (const Block& block : blocks) {
               if (block.name == name)
                  return block;
            }
            throw std::out_of_range("Block not found.");
         }

         const Block& get_block_by_id(std::string id) const noexcept(false) {
            for (const Block& block : blocks) {
               if(block.pd_block_id == id)
                  return block;
            }
            throw std::out_of_range("pd_block_id not found.");
         }
      };



      /************************************************************************************
      *
      * Helper functions
      *
      *************************************************************************************/
      //void print(const std::string& s);
      //void print(const int s);
      //void print(const std::vector<std::string>& v);
      inline void print(const Pair& p) {
         std::cout << p.tag << '\t' << p.value[0] << std::endl;
      }
      inline void print(const LoopPair& lp) {
         std::cout << lp.tag << '\n';
         for (const std::string& v : lp.values)
            std::cout << '\t' << v << '\n';
         std::cout << std::endl;
      }
      inline void print(const Loop& loop) {
         size_t total_values = loop.lpairs.size() * loop.lpairs[0].values.size();
         size_t total_tags = loop.lpairs.size();
         //tags
         for (const LoopPair& p : loop.lpairs) {
            std::cout << p.tag << '\t';
         }
         //values
         for (size_t k = 0; k < total_values; ++k) {
            size_t col = k % total_tags;
            size_t row = k / total_tags;
            if (col == 0)
               std::cout << std::endl;
            std::cout << loop.lpairs[col].values[row] << '\t';
         }
         std::cout << std::endl;
      }
      inline void print(const Item& item) {
         if (item.is_pair())
            print(item.pair);
         else //should just be a Loop
            print(item.loop);
      }
      inline void print(const Block& block, const bool print_all_values) {
         std::cout << "---------------\nBlock:\t";
         std::cout << block.name << std::endl;
         if (print_all_values) {
            for (const Item& item : block.items) {
               std::cout << "---------------\n";
               print(item);
            }
         }
         std::cout << "---------------\n" << std::endl;
      }
      inline void print(const Cif& cif, const bool print_all_values) {
         std::cout << "###############\nCIF:\t";
         std::cout << cif.source << std::endl;
         for (const Block& block : cif.blocks) {
            std::cout << "###############\n";
            print(block, print_all_values);
         }
         std::cout << "###############\n" << std::endl;
      }




   } //end namespace cif
} // end namespace row


#endif