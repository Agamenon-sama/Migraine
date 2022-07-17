#include <iostream>

#include "Emulator.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <rom file>\n";
        return 1;
    }
    Emulator vm(argv[1]);

    vm.run();

    return 0;
}
