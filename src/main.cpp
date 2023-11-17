#include <iostream>

#include "Emulator.h"
#include "Assembler/Assembler.h"

static void usage(char *prog);
static void license();

int main(int argc, char *argv[]) {
    if (argc == 2 && (std::string(argv[1]) == "-l" || std::string(argv[1]) == "--license")) {
        license();
        return 0;
    }

    if (argc == 3 && (std::string(argv[1]) == "-a" || std::string(argv[1]) == "--assemble")) {
        Assembler ass;
        return (ass.assemble(argv[2]) ? 0 : 2);
    }
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

static void usage(char *prog) {
    std::cerr << "Usage: " << prog << " [OPTION] <file>\n";
    std::cerr << "\t-d: lauch emulator in debug mode\n";
    std::cerr << "\t-a, --assemble: assemble assembly file\n";
}

static void license() {
    std::cout <<
R"(Migraine - Chip8 emulator an environment
Copyright (C) 2023  Chamseddine "Agamenon" HACHANI 

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
)";
}
