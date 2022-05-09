

#include <iostream>
#include <string>
#include <stdexcept>
#include <unordered_set>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <variant>
#include "tao/pegtl.hpp"


export module cif;

export import ciffile;

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

            //Whitespace and comments
            struct comment : pegtl::if_must<pegtl::one<'#'>, pegtl::until<pegtl::eolf>> {};
            struct ws : pegtl::one<' ', '\t'> {};
            struct wschar : pegtl::space {}; //pegtl::one<' ', '\n', '\r', '\t', '\v', '\f'>
            struct whitespace : pegtl::plus<pegtl::sor<wschar, comment>> {};
            struct ws_or_eof : pegtl::sor<whitespace, pegtl::eof> {};

            //character text fields and strings
            struct field_sep : pegtl::seq<pegtl::bol, pegtl::one<';'>> {};
            struct end_field_sep : pegtl::seq<pegtl::star<ws>, pegtl::plus<pegtl::eol>, field_sep> {};
            struct leading_ws : pegtl::star<pegtl::not_at<pegtl::sor<nonblankchar, end_field_sep>>, pegtl::sor<ws, pegtl::eol>> {};
            struct sctf_text : pegtl::star<pegtl::not_at<end_field_sep>, pegtl::sor<anyprintchar, pegtl::eol>> {};
            struct semicolontextfield : pegtl::if_must<field_sep, leading_ws, sctf_text, end_field_sep> {};
            struct textfield : semicolontextfield {};

            template<typename Q> struct endq : pegtl::seq<Q, pegtl::at<pegtl::sor<pegtl::one<' ', '\n', '\r', '\t', '#'>, pegtl::eof>>> {}; //what is the end of a quoted string
            template<typename Q> struct quote_text : pegtl::seq<pegtl::star<pegtl::not_at<endq<Q>>, anyprintchar>> {};
            struct unquoted_text : pegtl::plus<nonblankchar> {};
            template <typename Q> struct quoted_tail : pegtl::seq<quote_text<Q>, endq<Q>> {}; //the entire tail of a quoted string
            template<typename Q> struct quoted : pegtl::if_must<Q, quoted_tail<Q>> {};

            struct singlequotedstring : quoted<pegtl::one<'\''>> {};
            struct doublequotedstring : quoted<pegtl::one<'"'>> {};
            struct unquotedstring : pegtl::seq<pegtl::not_at<reserved>, pegtl::not_at<pegtl::one<'_', '$', '#'>>, pegtl::plus<nonblankchar>> {};
            struct charstring : pegtl::sor<singlequotedstring, doublequotedstring, unquotedstring> {};

            // for pdCIF, most values will be numeric. But we don't care about their specific type, we just suck up their
            //  string representation, and worry about it later.
            struct numeric : pegtl::seq<pegtl::plus<ordinarychar>, pegtl::at<wschar>> {};

            //Tags and values
            struct tag : pegtl::seq<pegtl::one<'_'>, pegtl::plus<nonblankchar>> {};
            struct value : pegtl::sor<numeric, textfield, charstring> {};
            struct itemtag : tag {};
            struct itemvalue : value {};
            struct looptag : tag {};
            struct loopvalue : value {};

            //CIF Structure
            //loop
            struct loopend : pegtl::opt<STOP, ws_or_eof> {};
            struct looptags : pegtl::plus<pegtl::seq<looptag, whitespace, pegtl::discard>> {};
            struct loopvalues : pegtl::sor<pegtl::plus<pegtl::seq<loopvalue, ws_or_eof, pegtl::discard>>, /* handle incorrect CIF with empty loop -->*/ pegtl::at<pegtl::sor<reserved, pegtl::eof>>> {};
            struct loopstart : pegtl::seq<LOOP, whitespace> {};
            struct loop : pegtl::if_must<loopstart, looptags, loopvalues, loopend> {};

            //pair
            struct pair : pegtl::if_must<itemtag, whitespace, pegtl::if_then_else<itemvalue, ws_or_eof, TAO_PEGTL_RAISE_MESSAGE("Malformed or missing value.")>, pegtl::discard> {};

            //item
            struct dataitem : pegtl::sor<pair, loop> {};

            struct blockframecode : pegtl::plus<nonblankchar> {};

            //saveframe
            struct saveframeend : SAVE {};
            struct saveframeheading : pegtl::seq<SAVE, blockframecode> {};
            struct saveframe : pegtl::if_must<saveframeheading, whitespace, pegtl::star<dataitem>, saveframeend, ws_or_eof> {};

            //datablock
            struct datablockheading : pegtl::seq<DATA, blockframecode> {};
            struct datablock : pegtl::seq<datablockheading, ws_or_eof, pegtl::star<pegtl::sor<dataitem, saveframe>>> {};

            //The actual CIF file
            struct content : pegtl::plus<pegtl::seq<datablock, pegtl::opt<whitespace>>> {};
            struct file : pegtl::seq<pegtl::opt<comment>, pegtl::opt<whitespace>, pegtl::if_must<pegtl::not_at<pegtl::eof>, content, pegtl::eof>> {};

        } //end namespace rules


        //to keep track of whether the quote string has been sent to file
        struct Status {
            bool is_loop{ false };
            bool is_quote{ false };
            bool is_printed{ false };

            void reset() {
                is_loop = false;
                is_quote = false;
                is_printed = false;
            }
            void get_ready_to_print() {
                is_quote = true;
                is_printed = false;
            }
            void just_printed() {
                is_printed = true;
            }
            void finished_printing() {
                is_quote = false;
                is_printed = false;
            }
            void loop() {
                is_loop = !is_loop;
            }
        };

        template<typename Input>
        void divert_action_to_value(const Input& in, Cif& out, Status& status) {
            status.get_ready_to_print();
            if (status.is_loop) {
                Action<rules::loopvalue>::apply(in, out, status);
            }
            else {
                Action<rules::itemvalue>::apply(in, out, status);
            }

        }


        //********************
        // Parsing Actions to populate the values in the ciffile
        //********************
        template<typename Rule>
        struct Action : pegtl::nothing<Rule> {};

        template<>
        struct Action<rules::blockframecode> {
            template<typename Input> static void apply(const Input& in, Cif& out, Status& status) {
                Block& block = out.blocks.emplace_back(in.string());
                out.items_ = &block.items;
                out.blockcode_ = &block.name;
                status.reset();
            }
        };

        template<> struct Action<rules::saveframeheading> {
            template<typename Input> static void apply(const Input& in, [[maybe_unused]] Cif& out, [[maybe_unused]] Status& status) {
                throw pegtl::parse_error("Saveframes are not yet supported by this parser. ", in);
            }
        };

        template<> struct Action<rules::itemtag> {
            template<typename Input> static void apply(const Input& in, Cif& out, [[maybe_unused]] Status& status) {
                out.items_->emplace_back(in.string());
            }
        };

        template<> struct Action<rules::itemvalue> {
            template<typename Input> static void apply(const Input& in, Cif& out, Status& status) {
                if (!status.is_quote || (status.is_quote && !status.is_printed)) [[likely]] {
                    Item& item = out.items_->back();
                    std::get_if<Pair>(&item.data)->value.emplace_back(in.string());
                    //item.pair.value.emplace_back(in.string());
                    status.just_printed();
                }
                else {
                    status.finished_printing();
                }
            }
        };

        template<> struct Action<rules::loopstart> {
            template<typename Input> static void apply([[maybe_unused]] const Input& in, Cif& out, [[maybe_unused]] Status& status) {
                out.items_->emplace_back(LoopArg{}); //construct a new Item containing a Loop
                status.loop();
            }
        };

        template<> struct Action<rules::looptag> {
            template<typename Input> static void apply(const Input& in, Cif& out, [[maybe_unused]] Status& status) {
                Item& item = out.items_->back();
                std::get_if<Loop>(&item.data)->lpairs.emplace_back(in.string());
                //item.loop.lpairs.emplace_back(in.string());

            }
        };

        template<> struct Action<rules::loopvalue> {
            template<typename Input> static void apply(const Input& in, Cif& out, Status& status) {
                if (!status.is_quote || (status.is_quote && !status.is_printed)) [[likely]] {
                    Item& item = out.items_->back();
                    Loop& loop = *std::get_if<Loop>(&item.data); // item.loop;
                    loop.lpairs[loop.currentAppend].values.emplace_back(in.string());
                    loop.currentAppend = ++loop.currentAppend % loop.lpairs.size();
                    ++loop.totalValues;                  
                    status.just_printed();
                }
                else {
                    status.finished_printing();
                }
            }
        };

        template<> struct Action<rules::loop> {
            template<typename Input> static void apply(const Input& in, Cif& out, Status& status) {
                Item& item = out.items_->back();
                assert(item.is_loop());
                Loop& loop = *std::get_if<Loop>(&item.data); // item.loop;
                size_t should_be_zero = loop.totalValues % loop.lpairs.size();
                if (should_be_zero != 0) {
                    std::string too_many{ std::to_string(should_be_zero) };
                    std::string too_few{ std::to_string(loop.lpairs.size() - should_be_zero) };
                    throw pegtl::parse_error(too_few + " too few, or " + too_many + " too many values in loop.", in);
                }
                status.loop();
            }
        };

        template<> struct Action<rules::quote_text<pegtl::one<'\''>>> {
            template<typename Input> static void apply(const Input& in, Cif& out, Status& status) {
                divert_action_to_value(in, out, status);
            }
        };

        template<> struct Action<rules::quote_text<pegtl::one<'\"'>>> {
            template<typename Input> static void apply(const Input& in, Cif& out, Status& status) {
                divert_action_to_value(in, out, status);
            }
        };

        template<> struct Action<rules::sctf_text> {
            template<typename Input> static void apply(const Input& in, Cif& out, Status& status) {
                divert_action_to_value(in, out, status);
            }
        };
        
        export std::string check_duplicates(const Block& block) {
            std::unordered_set<std::string> all_tags;
            //std::string tag{};
            std::string duplicates{};

            all_tags.reserve(100);

            auto check_tags = [&all_tags, &duplicates](const std::string &tag ) {
                if (all_tags.find(tag) == all_tags.end()) [[likely]] {
                    all_tags.insert(tag);
                }
                else {
                    duplicates += tag + " ";
                }
            };

            for (const Item& item : block.items) {
                if (item.is_pair()) {
                    //tag = std::get_if<Pair>(&item.data)->get_tag(); 
                    check_tags(std::get_if<Pair>(&item.data)->get_tag());
                }
                else if (item.is_loop()) {
                    for (const LoopPair& lp : std::get_if<Loop>(&item.data)->lpairs) {
                        //tag = lp.get_tag();
                        check_tags(lp.get_tag());
                    }
                }
            }
            return duplicates;
        }

        export void check_duplicates(const Cif& cif) noexcept(false) {
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
                Status status{};
                pegtl::parse<rules::file, Action>(in, d, status);
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
        export Cif read_file(const std::string& filename) noexcept(false) {
            pegtl::file_input in(filename);
            return read_input(in);
        }

    } //end namespace cif
} // end namespace row











