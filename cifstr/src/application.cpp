

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <format>
#include <string>
#include <filesystem>

#ifdef DOTHIS
import cif;
import cifstr;
import util;


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
            row::println(std::format("--------------------\nNow reading {0}. Block(s):", file));
            row::cif::Cif cif = row::cif::read_file(file);
            for (const row::cif::Block& block : cif.blocks) {
                try {
                    row::println(block.name);
                    CrystalStructure str(block, cif.source);
                    fout << str.to_string() << '\n';
                }
                catch (std::exception& e) {
                    row::println(e.what());
                    row::println("Continuing...");
                }
            }
        }
        catch (std::runtime_error& e) {
            row::println(e.what());
            row::println("Probable parse error. Continuing...");
        }   
    }


}
#endif


int main(int argc, char* argv[]) {

    return 0;
}

