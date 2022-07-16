#include <iostream>

#include "Emulator.h"

int main(int argc, char *argv[]) {
    std::cout << "Started the program\n";
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <rom file>\n";
        return 1;
    }
    Emulator vm(argv[1]);
    std::cout << "constracted the vm\n";

    vm.run();

    return 0;
}
