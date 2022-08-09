#include "Assembler.h"

#include <sstream>
#include <iostream>
#include <string.h>

Assembler::Assembler() {
    _output = new uint8_t[4096];
    _opcodesWritten = 0;
}

Assembler::~Assembler() {
    delete [] _output;
}

void Assembler::assemble(std::string filename) {
    _filename = filename;

    if (!_readFile()) {
        exit(1); // temp until I implement proper error handling
    }
    _preparse(); // set label Table

    std::stringstream src;
    src.str(_src);
    std::string line;

    uint16_t opcode = 0;
    std::string mnem;
    std::string op1;
    std::string op2;

    while (std::getline(src, line)) {
        std::stringstream sline;
        sline.str(line);
        sline >> mnem;
        if (mnem == "jmp") {
            sline >> op1;
            int addr = std::stoi(op1);
            opcode = 0x1000 | (addr & 0x0fff); // 0x1nnn
            _writeOpcode(opcode);
        }
        else if (mnem == "call") {
            sline >> op1;
            int addr = std::stoi(op1);
            opcode = 0x2000 | (addr & 0x0fff); // 0x2nnn
            _writeOpcode(opcode);
        }
        else if (mnem == "ld") {
            sline >> op1;
            sline >> op2;
            int reg, val;
            if (op1[0] == 'v') {
                reg = std::stoi(op1.substr(1, 1), 0, 16);
                val = std::stoi(op2);
            }
            opcode = 0x6000 | ((reg & 0x000f) << 8) | (val & 0x00ff); // 0x6xnn
            _writeOpcode(opcode);
        }
    }

    if (!_writeOutput()) {
        std::cerr << "can't write binary file\n";
    }
}

inline void Assembler::_writeOpcode(uint16_t opcode) {
    // I can't help but feel there should be an easier way
    // to do this but I don't care. It just works.
    char buff[2];
    buff[0] = (char)(opcode >> 8) & 0x00ff;
    buff[1] = (char)(opcode & 0x00ff);

    memcpy(_output + (2 * _opcodesWritten), buff, 2);
    _opcodesWritten++;
}

bool Assembler::_readFile() {
    std::fstream file(_filename, std::ios::in);
    if (!file.is_open()) {
        return false;
    }

    file.seekg(0, std::ios::end);
    auto size = file.tellg();
    file.seekg(0);

    char *buff = new char[size];
    file.read(buff, size);
    _src = buff;
    delete [] buff;

    return true;
}

bool Assembler::_writeOutput() {
    std::string filename = _filename + ".ch8";
    std::fstream file(filename, std::ios::out | std::ios::binary);

    if (!file.is_open()) {
        return false;
    }

    file.write((char *)_output, _opcodesWritten * 2);

    return true;
}

void Assembler::_preparse() {
    std::stringstream src;
    src.str(_src);
    _src = "";
    std::string line;
    int lineNum = 0;

    while (std::getline(src, line)) {
        line = line.substr(0, line.find("#")); // remove comments

        std::string firstWord = line.substr(0, line.find(" "));
        if (firstWord.size() <= 1) {
            // todo: implement unvalid line handler
        }
        else if (firstWord[firstWord.size() - 1] == ':') {
            _labelTable[firstWord.substr(0, firstWord.size() - 1)] = lineNum * 2;
        }
        else {
            lineNum++;
            _src += line + "\n";
        }
    }
}
