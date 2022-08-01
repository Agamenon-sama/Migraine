#include <iostream>

#include "Emulator.h"

static void usage(char *prog) {
    std::cerr << "Usage: " << prog << " [-d] <rom file>\n";
}

int main(int argc, char *argv[]) {
    if (argc < 2 || argc >= 4) {
        usage(argv[0]);
        return 1;
    }
    else if (argc == 3 && std::string(argv[1]) == "-d") {
        Emulator vm(argv[2], true);
        vm.run();
    }
    else if (argc == 2 && std::string(argv[1]) != "-d") {
        Emulator vm(argv[1]);
        vm.run();
    }
    // todo: implement this when I implement drag & drop
    // else if (argc == 2 && argv[1] == "-d") {}
    else {
        usage(argv[0]);
        return 1;
    }


    return 0;
}
