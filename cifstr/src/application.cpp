
#define _SILENCE_CXX20_CISO646_REMOVED_WARNING

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <array>
#include <unordered_map>
#include <format>
#include <numbers>
#include <algorithm>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <cmath>



#include "argparse/argparse.hpp"

#include "row/pdqciflib.hpp"
#include "util.hpp"
#include "vec3.hpp"
#include "unitcell.hpp"
#include "sites.hpp"
#include "crystal_structure.hpp"




void info() {
	constexpr std::string_view sv
	{ "This program is designed to reformat data from CIF into the STR format suitable for use by\n"
	"the Rietveld analysis software, TOPAS. It doesn't carry out any validation checks to ensure\n"
	"the data is consistent, but simply transcribes the data as given in the CIF.\n"
	"\n"
	"Where similar or identical data could be given in several places, the values are taken in a\n"
	"specific order of precedence, as detailed in the sections below.In general, if a value exists\n"
	"in an earlier place, the later places are not looked at.\n"
	"\n"
	"The STR's phase_name is taken from '_pd_phase_name', '_chemical_name_mineral', '_chemical_name_common',\n"
	"'_chemical_name_systematic', or '_chemical_name_structure_type', in that order, appended with\n"
	"the value of the 'data_' block. If none of these keys are available, then the name of the 'data_'\n"
	"block is used. This is also used as the filename of the STR file.\n"
	"\n"
	"The unit cell parameters are taken from '_cell_length_a', '_cell_length_b', '_cell_length_c',\n"
	"'_cell_angle_alpha', '_cell_angle_beta', and '_cell_angle_gamma'. Some comparisons are made to\n"
	"enable some standard macros to be used (eg Cubic, Tetragonal...). In any of these fail, then all\n"
	"parameters are given as a fail safe.\n"
	"\n"
	"The space_group is taken from '_symmetry_space_group_name_H-M', '_space_group_name_H-M_alt',\n"
	"'_symmetry_Int_Tables_number', or '_space_group_IT_number', in that order. If none of these keys\n"
	"are available, then an empty string is written as the space group. The value of the space group is\n"
	"as exactly given in the CIF. No validation or editing is done.\n"
	"\n"
	"The atomic sites are constructed as follows: The site names are taken from '_atom_site_label',\n"
	"with the fractional x, y, and z coordinates given by '_atom_site_fract_x', '_y', and '_z'.\n"
	"If the decimal values of the fractional coordinates are consistent with the fractions 1/6, 1/3, 2/3,\n"
	"or 5/6, then the decimal value is replaced by the fractions.\n"
	"\n"
	"The site occupancy is given by '_atom_site_occupancy', or by '1', if that key is not given.\n"
	"The atom type is given by '_atom_site_type_symbol', where available, or by the first one or two\n"
	"characters of the site label. If these characters match an element symbol, then that is used,\n"
	"otherwise, the label is used in it's entirety, and the user must decide the correct atom type to use.\n"
	"If the site label starts with 'Wat', then it is assumed that this means 'water', and an oxygen atom\n"
	"is used. An attempt is also made to reorder the charge given on an atom, to ensure it is compatible with\n"
	"TOPAS ordering, eg Fe+2, not Fe2+.\n"
	"\n"
	"Isotropic Atomic Displacement Parameters(ADPs; Biso), are taken from '_atom_site_B_iso_or_equiv', or\n"
	"from '_atom_site_U_iso_or_equiv'. Uiso values are multiplied by 8*Pi^2 to give B values.\n"
	"If anisotropic ADPs are given, then '_atom_site_aniso_B_11', '_atom_site_aniso_B_22', and\n"
	"'_atom_site_aniso_B_33' are averaged to give an equivalent Biso value. Alternatively, the equivalent\n"
	"U or beta values are used to calculate Biso. As anisotropic values could be given for a subset of the\n"
	"atoms in the structure, the labels given by '_atom_site_label' and '_atom_site_aniso_label' are matched,\n"
	"and if an atom doesn't have an anisotropic value, it takes its isotropic value, or is assigned a\n"
	"value of '1'.\n"
	"\n"
	"The atomic site is also given a 'num_posns 0' entry, which will update with the multiplicity of the\n"
	"site following a refinement. This will allow the user to compare this value with the CIF or Vol A\n"
	"to help ensure that the correct symmetry is being applied.\n"
	"\n"
	"There are many command line arguments to specialise the program's behaviour. The default behaviour\n"
	"is to only output the last block of any CIF file, and to only write content that is present in the CIF.\n"
	"The '-s' option adds a fixed Lorentzian crystallite size of 200 nm, and a refinable scale factor\n"
	"of 0.0001 to allow for an easy start to a refinement. The '-a' option does all blocks present in a\n"
	"CIF file. The '-m' option writes an output file for each block. The verbosity of the output to the screen\n"
	"can be controlled with '-v'.\n"
	"\n"
	"If you have any feedback, please contact me. If you find any bugs, please provide the CIF which\n"
	"caused the error, a description of the error, and a description of how you believe the program\n"
	"should work in that instance.\n"
	"\n"
	"Thanks, Matthew Rowles.\n"
	"rowlesmr@gmail.com\n" };

	std::cout << sv;
	return;
}

struct MyArgs : public argparse::Args {
    std::vector<std::string>& src_path = arg("input_files", "CIF file(s) you wish to convert.").multi_argument();          
    std::string& dst_path = arg("output_file", "The target STR file. It will be overwritten if it already exists.");      
    bool& add_stuff = flag("s,stuff", "Add in the scale factor and other nice stuff to make the STR immediately useable."); 
    bool& do_all_blocks = flag("a,all", "Do all the blocks in all the input_files.");                                       
    bool& write_many_files = flag("m,many", "Output each block as its own STR file. Uses output_file as the basename");     
    //int& verbosity = kwarg("v,verbosity", "Verbosity of screen output: 0|1|2").set_default(1);
    bool& print_info = flag("i,info", "Print information about what the program does.");                                       

    bool& printargs = flag("print", "A flag to toggle printing the argument values. Useful for debugging.");

    void welcome() override {
        std::cout << "Welcome to " << program_name << ", a program to convert CIF files to TOPAS STR files." << '\n';
    }
};

void print_block_to_file(const std::string& name, const std::string& source, const row::cif::Block& block, bool stuff, std::ofstream& out) {
    try {
        std::cout << name << '\n';
        CrystalStructure str(block, name, source, stuff);
        out << str.to_string() << '\n';
    }
    catch (std::exception& e) {
		std::cerr << e.what() << '\n';
		std::cerr << "Continuing...\n";

    }
	return;
}


int main(int argc, char* argv[])
{
	//work around argparse not liking not having the two default positional arguments
	if (argc == 2 && (strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "--info") == 0)) {
        info();
        exit(0);
	}

    auto args = argparse::parse<MyArgs>(argc, argv);

    if (args.printargs) {
        args.print();      // prints all variables
    }
    if (args.print_info) {
        info();
    }
    
    std::ofstream fout(args.dst_path);

    for (const std::string& file : args.src_path) { 
        try {
			std::cout << std::format("--------------------\nNow reading {0}. Block(s):\n", file);

            row::cif::Cif cif = row::cif::read_file(file, false);
            if (args.do_all_blocks) {
                for (const auto& [name, block] : cif) {
                    if (args.write_many_files) {
                        fout.close();; //close the previous instance
                        fout.open(args.dst_path + name + ".str");
                    }
                    print_block_to_file(name, cif.getSource(), block, args.add_stuff, fout);
                }
            }
            else {
                if (args.write_many_files) {
                    fout.close();; //close the previous instance
                    fout.open(args.dst_path + cif.getLastBlockName() + ".str");
                }
                print_block_to_file(cif.getLastBlockName(), cif.getSource(), cif.getLastBlock(), args.add_stuff, fout);
            }
        }
        catch (std::runtime_error& e) {
			std::cerr << e.what() << '\n';
			std::cerr << "Continuing with next file...\n";
        }   
    }


	std::cout << "Thanks for using cifstr. For feedback, please contact rowlesmr@gmail.com\n";

}

