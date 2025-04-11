#include <iostream>
#include <cstdlib>

#include "include/VLSI.h"

// accept command line arguments
int main(int argc, char* argv[]) {
    Circuit circuit;
    int dump_level = 0;
    std::string aux_file_dir = "datasets/superblue1/superblue1.aux";
    // check if there is an argument like "--dump 1 --dir datasets/superblue1/superblue1.aux"
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--dump") {
            dump_level = std::stoi(argv[i + 1]);
            i++;
        }
        else if (std::string(argv[i]) == "--dir" || std::string(argv[i]) == "-d") {
            aux_file_dir = argv[i + 1];
            i++;
        }
        else if (std::string(argv[i]) == "--help" || std::string(argv[i]) == "-h") {
            std::cout << "Usage: " << argv[0] << " [--dump <level>] [--dir <aux_file_dir>]" << std::endl;
            std::cout << "  --dump <level> : dump level (0, 1)" << std::endl;
            std::cout << "  --dir <aux_file_dir> : directory of the .aux file" << std::endl;
            return 0;
        }
    }
    circuit.load(aux_file_dir, dump_level);
    // circuit.dump(dump_level);

    // run Fiduccia-Mattheyses bipartition
    int area_constraint = 1;
    int max_unbalanced_nodes = 500;
    Circuit partition1, partition2;
    std::vector<CircuitNet> cut;
    std::tie(partition1, partition2, cut) = circuit.Fiduccia_Mattheyses_bipartition(area_constraint, max_unbalanced_nodes, dump_level);
    std::cout << "Partition 1:" << std::endl;
    partition1.dump(dump_level);
    std::cout << "Partition 2:" << std::endl;
    partition2.dump(dump_level);
    std::cout << "Total cut size: " << cut.size() << std::endl;

    return 0;
}