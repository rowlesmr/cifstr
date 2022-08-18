

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <format>
#include <string>
#include <filesystem>


import pdqciflib;
import cifstr;



int main(int argc, char* argv[])
{

    if (argc < 2) {
        std::cout << "Command line arguments should be as follows :\n";
        std::cout << "\tcifstr input_file [input_file ...] output_file\n";
        return 0;
    }

    std::ofstream fout(argv[argc-1]);
    for (int i{ 1 }; i < argc-1; ++i) { 
        std::string file{ argv[i] };
        try {
            std::cout << std::format("--------------------\nNow reading {0}. Block(s):\n", file);
            row::cif::Cif cif = row::cif::read_file(file);
            for (const auto& [name, block] : cif) {
                try {
                    std::cout << name << '\n';
                    CrystalStructure str(block, name, cif.getSource());
                    fout << str.to_string() << '\n';
                }
                catch (std::exception& e) {
                    std::cout << e.what() << '\n';
                    std::cout << "Continuing with next block...\n";
                }
            }
        }
        catch (std::runtime_error& e) {
            std::cout << e.what() << '\n';
            std::cout << "Probable parse error. Continuing with next file...\n";
        }   
    }


    //row::cif::Block blk{};

    //for (const auto& [name, value] : blk) {

    //}



}


