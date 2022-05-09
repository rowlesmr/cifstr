

#ifndef ROW_CIF_
#define ROW_CIF_

#include <string>
#include <stdexcept>
#include <unordered_map>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include "tao/pegtl.hpp"

#include "cif_file.h"


namespace row {
   namespace cif {
      
      namespace pegtl = tao::pegtl;

      namespace rules {

         //#define DEBUG_RULES

         //reserved words
         struct DATA : TAO_PEGTL_ISTRING("data_") {};
         struct LOOP : TAO_PEGTL_ISTRING("loop_") {};
         struct GLOBAL : TAO_PEGTL_ISTRING("global_") {};
         struct SAVE : TAO_PEGTL_ISTRING("save_") {};
         struct STOP : TAO_PEGTL_ISTRING("stop_") {};
         struct reserved : pegtl::sor<DATA, LOOP, SAVE, GLOBAL, STOP> {};

         //character sets
         //OrdinaryChar:	{ '!' | '%' | '&' | '(' | ')' | '*' | '+' | ',' | '-' | '.' | '/' | '0' 
         //              | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' | ':' | '<' | '=' 
         //              | '>' | '?' | '@' | 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' 
         //              | 'J' | 'K' | 'L' | 'M' | 'N' | 'O' | 'P' | 'Q' | 'R' | 'S' | 'T' | 'U' 
         //              | 'V' | 'W' | 'X' | 'Y' | 'Z' | '\' | '^' | '`' | 'a' | 'b' | 'c' | 'd' 
         //              | 'e' | 'f' | 'g' | 'h' | 'i' | 'j' | 'k' | 'l' | 'm' | 'n' | 'o' | 'p' 
         //              | 'q' | 'r' | 's' | 't' | 'u' | 'v' | 'w' | 'x' | 'y' | 'z' | '{' | '|' 
         //              | '}' | '~' }	
         //NonBlankChar:	OrdinaryChar | dquote | '#' | '$' | squote | '_' |           ';' | '[' | ']'
         //TextLeadChar:	OrdinaryChar | dquote | '#' | '$' | squote | '_' | SP | HT |       '[' | ']'
         //AnyPrintChar:	OrdinaryChar | dquote | '#' | '$' | squote | '_' | SP | HT | ';' | '[' | ']'
         struct ordinarychar : pegtl::ranges<'!', '!', '%', '&', '(', ':', '<', 'Z', '\\', '\\', '^', '^', '`', '~'> {}; //not: ' " # $ _ ; [ ] SP HT
         struct nonblankchar : pegtl::range<'!', '~'> {};  //not: SP HT
         struct textleadchar : pegtl::ranges<' ', ':', '<', '~', '\t'> {}; //all chars except ';'
         struct anyprintchar : pegtl::ranges<' ', '~', '\t'> {}; //ALL THE CHARS!!
         struct ws : pegtl::one<' ', '\t'> {};
         struct wschar : pegtl::one<' ', '\t', '\n', '\r'> {};

         //Whitespace and comments
         //WhiteSpace        : { SP | HT | EOL | TokenizedComments }+ 
         //Comments          : { '#' AnyPrintChar* EOL }+
         //TokenizedComments	 : { SP | HT | EOL }+ Comments
         struct comment : pegtl::if_must<pegtl::one<'#'>, pegtl::until<pegtl::eolf>> {};
         struct whitespace : pegtl::plus<pegtl::sor<wschar, comment>> {};
         struct ws_or_eof : pegtl::sor<whitespace, pegtl::eof> {};

         //character strings  and text fields
         //CharString                   : UnquotedString | SingleQuotedString | DoubleQuotedString
         //unquotedstring               : eolunquotedstring | noteolunquotedstring
         //eol UnquotedString           : eol OrdinaryChar NonBlankChar*  --  this is to match a string from the beginning of a line
         //noteol UnquotedString        : noteol {OrdinaryChar | ';'} NonBlankChar*  --  this is to match a string to from the beginning of a line
         //SingleQuotedString WhiteSpace: single_quote AnyPrintChar* single_quote WhiteSpace
         //DoubleQuotedString WhiteSpace: double_quote AnyPrintChar* double_quote WhiteSpace
         //TextField                    : SemiColonTextField	
         //eol SemiColonTextField       : eol ';' { AnyPrintChar* eol { {TextLeadChar AnyPrintChar* } ? eol }* } ';'
        

         struct field_sep : pegtl::seq<pegtl::bol, pegtl::one<';'>> {};
         struct end_field_sep : pegtl::seq<pegtl::plus<pegtl::eol>, field_sep> {};
         struct semicolontextfield : pegtl::if_must<field_sep, pegtl::until<end_field_sep>> {};
         struct textfield : semicolontextfield {}; 

         template<typename Q> //what is the end of a quoted string
         struct endq : pegtl::seq<Q, pegtl::at<pegtl::sor<pegtl::one<' ', '\n', '\r', '\t', '#'>, pegtl::eof>>> {};
         template <typename Q> //the entire tail of a quoted string
         struct quoted_tail : pegtl::until<endq<Q>, anyprintchar> {};
         template<typename Q>
         struct quoted : pegtl::if_must<Q, quoted_tail<Q>> {};

         struct singlequotedstring : quoted<pegtl::one<'\''>> {};
         struct doublequotedstring : quoted<pegtl::one<'"'>> {};
         struct unquotedstring : pegtl::seq<pegtl::not_at<reserved>, pegtl::not_at<pegtl::one<'_', '$', '#'>>, pegtl::plus<nonblankchar>> {};
         struct charstring : pegtl::sor<singlequotedstring, doublequotedstring, unquotedstring> {};

         // for pdCIF, most values will be numeric. But we don't care about their specific type, we just suck up their
         //  string representation, and worry about it later.
         struct numeric : pegtl::seq<pegtl::plus<ordinarychar>, pegtl::at<wschar>> {};

         //Tags and values
         //Tag   :  '_' NonBlankChar+ 
         //Value :  '.' | '?' | Numeric | TextField | CharString
         struct tag : pegtl::seq<pegtl::one<'_'>, pegtl::plus<nonblankchar>> {};
         struct value : pegtl::sor<numeric, textfield, charstring> {};
         struct itemtag : tag {};
         struct itemvalue : value {};
         struct looptag : tag {};
         struct loopvalue : value {};

         //CIF Strucure
         //cif               :  Comments? WhiteSpace? { DataBlock { WhiteSpace DataBlock }* WhiteSpace? }?
         //DataBlock         :  DataBlockHeading { WhiteSpace { DataItem | SaveFrame } }*
         //DataBlockHeading  :  DATA NonBlankChar+
         //SaveFrame         :  SaveFrameHeading { WhiteSpace DataItem }+ WhiteSpace SAVE
         //SaveFrameHeading  :  SAVE NonBlankChar+
         //DataItems         :	{ Tag WhiteSpace Value } | { LoopHeader WhiteSpace LoopBody }
         //LoopHeader        :  LOOP { WhiteSpace Tag }+
         //LoopBody          :  Value { WhiteSpace Value }*  , 
         struct pair : pegtl::if_must<itemtag, whitespace, pegtl::if_then_else<itemvalue, ws_or_eof, TAO_PEGTL_RAISE_MESSAGE("Malformed or missing value.")>, pegtl::discard> {};
         struct blockframecode : pegtl::plus<nonblankchar> {};

         //loop
         struct loopend : pegtl::opt<STOP, ws_or_eof> {};
         struct looptags : pegtl::plus<pegtl::seq<looptag, whitespace, pegtl::discard>> {};
         struct loopvalues : pegtl::sor<pegtl::plus<pegtl::seq<loopvalue, ws_or_eof, pegtl::discard>>, /* handle incorrect CIF with empty loop -->*/ pegtl::at<pegtl::sor<reserved, pegtl::eof>>> {};
         struct loopstart : pegtl::seq<LOOP, whitespace> {};
         struct loop : pegtl::if_must<loopstart, looptags, loopvalues, loopend> {};

         struct dataitem : pegtl::sor<pair, loop> {};

         struct saveframeend : SAVE {};
         struct saveframeheading : pegtl::seq<SAVE, blockframecode> {};
         struct saveframe : pegtl::if_must<saveframeheading, whitespace, pegtl::star<dataitem>, saveframeend, ws_or_eof> {};

         struct datablockheading : pegtl::seq<DATA, blockframecode> {};
         struct datablock : pegtl::seq<datablockheading, ws_or_eof, pegtl::star<pegtl::sor<dataitem, saveframe>>> {};

         struct content : pegtl::plus<pegtl::seq<datablock, pegtl::opt<whitespace>>> {};
         struct file : pegtl::seq<pegtl::opt<comment>, pegtl::opt<whitespace>, pegtl::if_must<pegtl::not_at<pegtl::eof>, content, pegtl::eof>> {};
      
      } //end namespace rules


      //********************
      // Parsing Actions to populate the values in the ciffile
      //********************
      template<typename Rule> 
      struct Action : pegtl::nothing<Rule> {};

      template<> 
      struct Action<rules::blockframecode> {
         template<typename Input> 
         static void apply(const Input& in, Cif& out) {
            Block& block = out.blocks.emplace_back(in.string());
            out.items_ = &block.items;
            out.blockcode_ = &block.name;
         }
      };

      template<> struct Action<rules::saveframeheading> {
         template<typename Input> static void apply(const Input& in, Cif& out) {
            throw pegtl::parse_error("Saveframes are not yet supported by this parser. ", in);
            out.items_; //to get rid of compiler warnings, as I'm not smart enough to figure out how to use a single-argument `apply`
         }
      };

      template<> struct Action<rules::itemtag> {
         template<typename Input> static void apply(const Input& in, Cif& out) {
            out.items_->emplace_back(in.string());
         }
      };

      template<> struct Action<rules::itemvalue> {
         template<typename Input> static void apply(const Input& in, Cif& out) {
            Item& item = out.items_->back();
            assert(item.is_pair());
            item.pair.value.emplace_back(in.string());
         }
      };

      template<> struct Action<rules::loopstart> {
         template<typename Input> static void apply(const Input& in, Cif& out) {
            out.items_->emplace_back(LoopArg{}); //construct a new Item containing a Loop
            std::string tmp{ in.string() };  //to get rid of compiler warnings, as I'm not smart enough to figure out how to use a single-argument `apply`
         }
      };

      template<> struct Action<rules::looptag> {
         template<typename Input> static void apply(const Input& in, Cif& out) {
            Item& item = out.items_->back();
            assert(item.is_loop());
            item.loop.lpairs.emplace_back(in.string());
         }
      };

      template<> struct Action<rules::loopvalue> {
         template<typename Input> static void apply(const Input& in, Cif& out) {
            Item& item = out.items_->back();
            assert(item.is_loop());
            Loop& loop = item.loop;
            loop.lpairs[loop.currentAppend].values.emplace_back(in.string());
            loop.currentAppend = ++loop.currentAppend % loop.lpairs.size();
            ++loop.totalValues;
         }
      };

      template<> struct Action<rules::loop> {
         template<typename Input> static void apply(const Input& in, Cif& out) {
            Item& item = out.items_->back();
            assert(item.is_loop());
            Loop& loop = item.loop;
            size_t should_be_zero = loop.totalValues % loop.lpairs.size();
            if (should_be_zero != 0) {
               std::string too_many{ std::to_string(should_be_zero) };
               std::string too_few{ std::to_string(loop.lpairs.size() - should_be_zero) };
               throw pegtl::parse_error(too_few + " too few, or " + too_many + " too many values in loop.", in);
            }
         }
      };

      inline std::string check_duplicates(const Block& block) {
         std::unordered_map<std::string, size_t> tagCount;
         std::string tag{};

         auto count_tags = [&tagCount, &tag]() {
            if (tagCount.find(tag) == tagCount.end()) {
               tagCount.insert(std::make_pair(tag, 1));
            }
            else {
               tagCount[tag]++;
            }
         };

         //collect all the tags in the entire block.
         for (const Item& item : block.items) {
            if (item.is_pair()) {
               tag = item.pair.tag;
               count_tags();
            }
            else if (item.is_loop()) {
               for (const LoopPair& lp : item.loop.lpairs) {
                  tag = lp.tag;
                  count_tags();
               }
            }
         }

         std::string duplicates{};
         for (const auto& [key, value] : tagCount) {
            if (value > 1)
               duplicates += key + " ";
         }
         return duplicates;
      }

      inline void check_duplicates(const Cif& cif) noexcept(false) {
         std::string duplicates{};
         std::string blockDuplicates{};
         for (const Block& block : cif.blocks) {
            blockDuplicates = check_duplicates(block);
            if (blockDuplicates.size() > 0) {
               duplicates += block.name + ": " + blockDuplicates + "\n";
            }
         }
         if (duplicates.size() > 0) {
            throw std::runtime_error("Duplicate tags encountered: " + duplicates);
         }
      }

      template<typename Input> void parse_input(Cif& d, Input&& in) noexcept(false) {
         try {
            pegtl::parse<rules::file, Action>(in, d);
         }
         catch (pegtl::parse_error& e) {
            const auto p = e.positions().front();
            //pretty-print the error msg and the line that caused it, with an indicator at the token that done it.
            std::cerr << e.what() << '\n'
               << in.line_at(p) << '\n'
               << std::setw(p.column) << '^' << std::endl;
            throw std::runtime_error("Parsing error.");
         }       
      }

      template<typename Input> Cif read_input(Input&& in)  noexcept(false) {
         Cif cif;
         cif.source = in.source();
         parse_input(cif, in);
         check_duplicates(cif);
         return cif;
      }

      //read in a file into a Cif. Will throw std::runtime_error if it encounters problems
      inline Cif read_file(const std::string& filename) noexcept(false) {
         pegtl::file_input in(filename);
         return read_input(in);
      }

   } //end namespace cif
} // end namespace row


#endif