#include "Assembler.h"

#define SLOG_IMPLEMENTATION
#include <slog/slog.h>

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

bool Assembler::assemble(std::string filename) {
    _filename = filename;

    if (!_readFile()) {
        slog::error("Failed to open the assembly file");
        return false;
    }
    _preparse(); // set label Table

    std::stringstream src;
    src.str(_src);
    std::string line;

    uint16_t opcode = 0;
    std::string mnem;
    std::string op1;
    std::string op2;
    std::string op3;

    while (std::getline(src, line)) {
        std::stringstream sline;
        sline.str(line);
        sline >> mnem;
        if (mnem == "cls") {
            opcode = 0x00e0; // 0x00e0
            _writeOpcode(opcode);
        }
        else if (mnem == "ret") {
            opcode = 0x00ee; // 0x00ee
            _writeOpcode(opcode);
        }
        else if (mnem == "jmp") {
            sline >> op1;
            int addr;
            try {
                addr = std::stoi(op1);
            }
            catch (std::exception &e) {
                if (_labelTable.find(op1) != _labelTable.end()) {
                    addr = _labelTable[op1];
                }
                else {
                    slog::error("unrecognized symbol: " + op1);
                    slog::message(sline.str());
                    return false;
                }
            }
            opcode = 0x1000 | (addr & 0x0fff); // 0x1nnn
            _writeOpcode(opcode);
        }
        else if (mnem == "call") {
            sline >> op1;
            int addr;
            try {
                addr = std::stoi(op1);
            }
            catch (std::exception &e) {
                if (_labelTable.find(op1) != _labelTable.end()) {
                    addr = _labelTable[op1];
                }
                else {
                    slog::error("unrecognized symbol: " + op1);
                    slog::message(sline.str());
                    return false;
                }
            }
            opcode = 0x2000 | (addr & 0x0fff); // 0x2nnn
            _writeOpcode(opcode);
        }
        else if (mnem == "se") {
            sline >> op1;
            sline >> op2;
            int reg, val;
            if (op1[0] != 'v') {
                slog::error("expected a register");
                slog::message(sline.str());
                return false;
            }
            try {
                reg = std::stoi(op1.substr(1, 1), 0, 16);
            }
            catch (std::exception &e) {
                slog::error("invalid register number");
                slog::message(sline.str());
                return false;
            }
            try {
                val = std::stoi(op2);
            }
            catch (std::exception &e) {
                slog::error("expected a number literal");
                slog::message(sline.str());
                return false;
            }
            opcode = 0x3000 | ((reg & 0x000f) << 8) | (val & 0x00ff); // 0x3xnn
            _writeOpcode(opcode);
        }
        else if (mnem == "sne") {
            sline >> op1;
            sline >> op2;
            int reg, val;
            if (op1[0] != 'v') {
                slog::error("expected a register");
                slog::message(sline.str());
                return false;
            }
            try {
                reg = std::stoi(op1.substr(1, 1), 0, 16);
            }
            catch (std::exception &e) {
                slog::error("invalid register number");
                slog::message(sline.str());
                return false;
            }
            try {
                val = std::stoi(op2);
            }
            catch (std::exception &e) {
                slog::error("expected a number literal");
                slog::message(sline.str());
                return false;
            }
            opcode = 0x4000 | ((reg & 0x000f) << 8) | (val & 0x00ff); // 0x4xnn
            _writeOpcode(opcode);
        }
        else if (mnem == "sev") {
            sline >> op1;
            sline >> op2;
            int reg1, reg2;
            if(op1[0] != 'v' || op2[0] != 'v') {
                slog::error("expected a register");
                slog::message(sline.str());
                return false;
            }
            try {
                reg1 = std::stoi(op1.substr(1, 1), 0, 16);
                reg2 = std::stoi(op2.substr(1, 1), 0, 16);
            }
            catch (std::exception &e) {
                slog::error("invalid register number");
                slog::message(sline.str());
                return false;
            }
            opcode = 0x5000 | ((reg1 & 0xf) << 8) | ((reg2 & 0xf) << 4); // 0x5xy0
            _writeOpcode(opcode);
        }
        else if (mnem == "ld") {
            sline >> op1;
            sline >> op2;
            int reg, val;
            if (op1[0] != 'v') {
                slog::error("expected a register");
                slog::message(sline.str());
                return false;
            }
            try {
                reg = std::stoi(op1.substr(1, 1), 0, 16);
            }
            catch (std::exception &e) {
                slog::error("invalid register number");
                slog::message(sline.str());
                return false;
            }
            try {
                val = std::stoi(op2);
            }
            catch (std::exception &e) {
                slog::error("expected a number literal");
                slog::message(sline.str());
                return false;
            }
            opcode = 0x6000 | ((reg & 0x000f) << 8) | (val & 0x00ff); // 0x6xnn
            _writeOpcode(opcode);
        }
        else if (mnem == "addv") {
            sline >> op1;
            sline >> op2;
            int reg, val;
            if (op1[0] != 'v') {
                slog::error("expected a register");
                slog::message(sline.str());
                return false;
            }
            try {
                reg = std::stoi(op1.substr(1, 1), 0, 16);
            }
            catch (std::exception &e) {
                slog::error("invalid register number");
                slog::message(sline.str());
                return false;
            }
            try {
                val = std::stoi(op2);
            }
            catch (std::exception &e) {
                slog::error("expected a number literal");
                slog::message(sline.str());
                return false;
            }
            opcode = 0x7000 | ((reg & 0x000f) << 8) | (val & 0x00ff); // 0x7xnn
            _writeOpcode(opcode);
        }
        else if (mnem == "ldv") {
            sline >> op1;
            sline >> op2;
            int reg1, reg2;
            if(op1[0] != 'v' || op2[0] != 'v') {
                slog::error("expected a register");
                slog::message(sline.str());
                return false;
            }
            try {
                reg1 = std::stoi(op1.substr(1, 1), 0, 16);
                reg2 = std::stoi(op2.substr(1, 1), 0, 16);
            }
            catch (std::exception &e) {
                slog::error("invalid register number");
                slog::message(sline.str());
                return false;
            }
            opcode = 0x8000 | ((reg1 & 0xf) << 8) | ((reg2 & 0xf) << 4); // 0x8xy0
            _writeOpcode(opcode);
        }
        else if (mnem == "or") {
            sline >> op1;
            sline >> op2;
            int reg1, reg2;
            if(op1[0] != 'v' || op2[0] != 'v') {
                slog::error("expected a register");
                slog::message(sline.str());
                return false;
            }
            try {
                reg1 = std::stoi(op1.substr(1, 1), 0, 16);
                reg2 = std::stoi(op2.substr(1, 1), 0, 16);
            }
            catch (std::exception &e) {
                slog::error("invalid register number");
                slog::message(sline.str());
                return false;
            }
            opcode = 0x8000 | ((reg1 & 0xf) << 8) | ((reg2 & 0xf) << 4) | 0x1; // 0x8xy1
            _writeOpcode(opcode);
        }
        else if (mnem == "and") {
            sline >> op1;
            sline >> op2;
            int reg1, reg2;
            if(op1[0] != 'v' || op2[0] != 'v') {
                slog::error("expected a register");
                slog::message(sline.str());
                return false;
            }
            try {
                reg1 = std::stoi(op1.substr(1, 1), 0, 16);
                reg2 = std::stoi(op2.substr(1, 1), 0, 16);
            }
            catch (std::exception &e) {
                slog::error("invalid register number");
                slog::message(sline.str());
                return false;
            }
            opcode = 0x8000 | ((reg1 & 0xf) << 8) | ((reg2 & 0xf) << 4) | 0x2; // 0x8xy2
            _writeOpcode(opcode);
        }
        else if (mnem == "xor") {
            sline >> op1;
            sline >> op2;
            int reg1, reg2;
            if(op1[0] != 'v' || op2[0] != 'v') {
                slog::error("expected a register");
                slog::message(sline.str());
                return false;
            }
            try {
                reg1 = std::stoi(op1.substr(1, 1), 0, 16);
                reg2 = std::stoi(op2.substr(1, 1), 0, 16);
            }
            catch (std::exception &e) {
                slog::error("invalid register number");
                slog::message(sline.str());
                return false;
            }
            opcode = 0x8000 | ((reg1 & 0xf) << 8) | ((reg2 & 0xf) << 4) | 0x3; // 0x8xy3
            _writeOpcode(opcode);
        }
        else if (mnem == "add") {
            sline >> op1;
            sline >> op2;
            int reg1, reg2;
            if(op1[0] != 'v' || op2[0] != 'v') {
                slog::error("expected a register");
                slog::message(sline.str());
                return false;
            }
            try {
                reg1 = std::stoi(op1.substr(1, 1), 0, 16);
                reg2 = std::stoi(op2.substr(1, 1), 0, 16);
            }
            catch (std::exception &e) {
                slog::error("invalid register number");
                slog::message(sline.str());
                return false;
            }
            opcode = 0x8000 | ((reg1 & 0xf) << 8) | ((reg2 & 0xf) << 4) | 0x4; // 0x8xy4
            _writeOpcode(opcode);
        }
        else if (mnem == "sub") {
            sline >> op1;
            sline >> op2;
            int reg1, reg2;
            if(op1[0] != 'v' || op2[0] != 'v') {
                slog::error("expected a register");
                slog::message(sline.str());
                return false;
            }
            try {
                reg1 = std::stoi(op1.substr(1, 1), 0, 16);
                reg2 = std::stoi(op2.substr(1, 1), 0, 16);
            }
            catch (std::exception &e) {
                slog::error("invalid register number");
                slog::message(sline.str());
                return false;
            }
            opcode = 0x8000 | ((reg1 & 0xf) << 8) | ((reg2 & 0xf) << 4) | 0x5; // 0x8xy5
            _writeOpcode(opcode);
        }
        else if (mnem == "shr") {
            sline >> op1;
            sline >> op2;
            int reg1, reg2;
            if(op1[0] != 'v' || op2[0] != 'v') {
                slog::error("expected a register");
                slog::message(sline.str());
                return false;
            }
            try {
                reg1 = std::stoi(op1.substr(1, 1), 0, 16);
                reg2 = std::stoi(op2.substr(1, 1), 0, 16);
            }
            catch (std::exception &e) {
                slog::error("invalid register number");
                slog::message(sline.str());
                return false;
            }
            opcode = 0x8000 | ((reg1 & 0xf) << 8) | ((reg2 & 0xf) << 4) | 0x6; // 0x8xy6
            _writeOpcode(opcode);
        }
        else if (mnem == "subn") {
            sline >> op1;
            sline >> op2;
            int reg1, reg2;
            if(op1[0] != 'v' || op2[0] != 'v') {
                slog::error("expected a register");
                slog::message(sline.str());
                return false;
            }
            try {
                reg1 = std::stoi(op1.substr(1, 1), 0, 16);
                reg2 = std::stoi(op2.substr(1, 1), 0, 16);
            }
            catch (std::exception &e) {
                slog::error("invalid register number");
                slog::message(sline.str());
                return false;
            }
            opcode = 0x8000 | ((reg1 & 0xf) << 8) | ((reg2 & 0xf) << 4) | 0x7; // 0x8xy7
            _writeOpcode(opcode);
        }
        else if (mnem == "shl") {
            sline >> op1;
            sline >> op2;
            int reg1, reg2;
            if(op1[0] != 'v' || op2[0] != 'v') {
                slog::error("expected a register");
                slog::message(sline.str());
                return false;
            }
            try {
                reg1 = std::stoi(op1.substr(1, 1), 0, 16);
                reg2 = std::stoi(op2.substr(1, 1), 0, 16);
            }
            catch (std::exception &e) {
                slog::error("invalid register number");
                slog::message(sline.str());
                return false;
            }
            opcode = 0x8000 | ((reg1 & 0xf) << 8) | ((reg2 & 0xf) << 4) | 0xe; // 0x8xye
            _writeOpcode(opcode);
        }
        else if (mnem == "sne") {
            sline >> op1;
            sline >> op2;
            int reg1, reg2;
            if(op1[0] != 'v' || op2[0] != 'v') {
                slog::error("expected a register");
                slog::message(sline.str());
                return false;
            }
            try {
                reg1 = std::stoi(op1.substr(1, 1), 0, 16);
                reg2 = std::stoi(op2.substr(1, 1), 0, 16);
            }
            catch (std::exception &e) {
                slog::error("invalid register number");
                slog::message(sline.str());
                return false;
            }
            opcode = 0x9000 | ((reg1 & 0xf) << 8) | ((reg2 & 0xf) << 4); // 0x9xy0
            _writeOpcode(opcode);
        }
        else if (mnem == "ldi") {
            sline >> op1;
            int addr;
            try {
                addr = std::stoi(op1);
            }
            catch (std::exception &e) {
                slog::error("expected a number literal");
                slog::message(sline.str());
                return false;
            }
            opcode = 0xa000 | (addr & 0x0fff); // 0xannn
            _writeOpcode(opcode);
        }
        else if (mnem == "jmpv") {
            sline >> op1;
            int addr;
            try {
                addr = std::stoi(op1);
            }
            catch (std::exception &e) {
                slog::error("expected a number literal");
                slog::message(sline.str());
                return false;
            }
            opcode = 0xb000 | (addr & 0x0fff); // 0xbnnn
            _writeOpcode(opcode);
        }
        else if (mnem == "rnd") {
            sline >> op1;
            sline >> op2;
            int reg, val;
            if(op1[0] != 'v') {
                slog::error("expected a register");
                slog::message(sline.str());
                return false;
            }
            try {
                reg = std::stoi(op1.substr(1, 1), 0, 16);
            }
            catch (std::exception &e) {
                slog::error("invalid register number");
                slog::message(sline.str());
                return false;
            }
            try {
                val = std::stoi(op2);
            }
            catch (std::exception &e) {
                slog::error("expected a number literal");
                slog::message(sline.str());
                return false;
            }
            opcode = 0xc000 | ((reg & 0xf) << 8) | val & 0xff; // 0xcxy0
            _writeOpcode(opcode);
        }
        else if (mnem == "drw") {
            sline >> op1;
            sline >> op2;
            sline >> op3;
            int reg1, reg2, val;

            if(op1[0] != 'v' || op2[0] != 'v') {
                slog::error("expected a register");
                slog::message(sline.str());
                return false;
            }
            try {
                reg1 = std::stoi(op1.substr(1, 1), 0, 16);
                reg2 = std::stoi(op2.substr(1, 1), 0, 16);
            }
            catch (std::exception &e) {
                slog::error("invalid register number");
                slog::message(sline.str());
                return false;
            }
            try {
                val = std::stoi(op3);
            }
            catch (std::exception &e) {
                slog::error("expected a number literal");
                slog::message(sline.str());
                return false;
            }

            opcode = 0xd000 | ((reg1 & 0xf) << 8) | ((reg2 & 0xf) << 4) | val & 0xf; // 0xdxyn
            _writeOpcode(opcode);
        }
        else if (mnem == "kpr") {
            sline >> op1;
            int reg;

            if (op1[0] != 'v') {
                slog::error("expected a register");
                slog::message(sline.str());
                return false;
            }
            try {
                reg = std::stoi(op1.substr(1, 1), 0, 16);
            }
            catch (std::exception &e) {
                slog::error("invalid register number");
                slog::message(sline.str());
                return false;
            }

            opcode = 0xe000 | ((reg & 0xf) << 8) | 0x009e; // 0xex9e
            _writeOpcode(opcode);
        }
        else if (mnem == "knpr") {
            sline >> op1;
            int reg;

            if (op1[0] != 'v') {
                slog::error("expected a register");
                slog::message(sline.str());
                return false;
            }
            try {
                reg = std::stoi(op1.substr(1, 1), 0, 16);
            }
            catch (std::exception &e) {
                slog::error("invalid register number");
                slog::message(sline.str());
                return false;
            }

            opcode = 0xe000 | ((reg & 0xf) << 8) | 0x00a1; // 0xexa1
            _writeOpcode(opcode);
        }
        else if (mnem == "gdt") {
            sline >> op1;
            int reg;

            if (op1[0] != 'v') {
                slog::error("expected a register");
                slog::message(sline.str());
                return false;
            }
            try {
                reg = std::stoi(op1.substr(1, 1), 0, 16);
            }
            catch (std::exception &e) {
                slog::error("invalid register number");
                slog::message(sline.str());
                return false;
            }

            opcode = 0xf000 | ((reg & 0xf) << 8) | 0x0007; // 0xfx07
            _writeOpcode(opcode);
        }
        else if (mnem == "gkpr") {
            sline >> op1;
            int reg;

            if (op1[0] != 'v') {
                slog::error("expected a register");
                slog::message(sline.str());
                return false;
            }
            try {
                reg = std::stoi(op1.substr(1, 1), 0, 16);
            }
            catch (std::exception &e) {
                slog::error("invalid register number");
                slog::message(sline.str());
                return false;
            }

            opcode = 0xf000 | ((reg & 0xf) << 8) | 0x000a; // 0xfx0a
            _writeOpcode(opcode);
        }
        else if (mnem == "sdt") {
            sline >> op1;
            int reg;

            if (op1[0] != 'v') {
                slog::error("expected a register");
                slog::message(sline.str());
                return false;
            }
            try {
                reg = std::stoi(op1.substr(1, 1), 0, 16);
            }
            catch (std::exception &e) {
                slog::error("invalid register number");
                slog::message(sline.str());
                return false;
            }

            opcode = 0xf000 | ((reg & 0xf) << 8) | 0x0015; // 0xfx15
            _writeOpcode(opcode);
        }
        else if (mnem == "sst") {
            sline >> op1;
            int reg;

            if (op1[0] != 'v') {
                slog::error("expected a register");
                slog::message(sline.str());
                return false;
            }
            try {
                reg = std::stoi(op1.substr(1, 1), 0, 16);
            }
            catch (std::exception &e) {
                slog::error("invalid register number");
                slog::message(sline.str());
                return false;
            }

            opcode = 0xf000 | ((reg & 0xf) << 8) | 0x0018; // 0xfx18
            _writeOpcode(opcode);
        }
        else if (mnem == "addi") {
            sline >> op1;
            int reg;

            if (op1[0] != 'v') {
                slog::error("expected a register");
                slog::message(sline.str());
                return false;
            }
            try {
                reg = std::stoi(op1.substr(1, 1), 0, 16);
            }
            catch (std::exception &e) {
                slog::error("invalid register number");
                slog::message(sline.str());
                return false;
            }

            opcode = 0xf000 | ((reg & 0xf) << 8) | 0x001e; // 0xfx1e
            _writeOpcode(opcode);
        }
        else if (mnem == "lds") {
            sline >> op1;
            int reg;

            if (op1[0] != 'v') {
                slog::error("expected a register");
                slog::message(sline.str());
                return false;
            }
            try {
                reg = std::stoi(op1.substr(1, 1), 0, 16);
            }
            catch (std::exception &e) {
                slog::error("invalid register number");
                slog::message(sline.str());
                return false;
            }

            opcode = 0xf000 | ((reg & 0xf) << 8) | 0x0029; // 0xfx29
            _writeOpcode(opcode);
        }
        else if (mnem == "bcd") {
            sline >> op1;
            int reg;

            if (op1[0] != 'v') {
                slog::error("expected a register");
                slog::message(sline.str());
                return false;
            }
            try {
                reg = std::stoi(op1.substr(1, 1), 0, 16);
            }
            catch (std::exception &e) {
                slog::error("invalid register number");
                slog::message(sline.str());
                return false;
            }

            opcode = 0xf000 | ((reg & 0xf) << 8) | 0x0033; // 0xfx33
            _writeOpcode(opcode);
        }
        else if (mnem == "regd") {
            sline >> op1;
            int reg;

            if (op1[0] != 'v') {
                slog::error("expected a register");
                slog::message(sline.str());
                return false;
            }
            try {
                reg = std::stoi(op1.substr(1, 1), 0, 16);
            }
            catch (std::exception &e) {
                slog::error("invalid register number");
                slog::message(sline.str());
                return false;
            }

            opcode = 0xf000 | ((reg & 0xf) << 8) | 0x0055; // 0xfx55
            _writeOpcode(opcode);
        }
        else if (mnem == "regl") {
            sline >> op1;
            int reg;

            if (op1[0] != 'v') {
                slog::error("expected a register");
                slog::message(sline.str());
                return false;
            }
            try {
                reg = std::stoi(op1.substr(1, 1), 0, 16);
            }
            catch (std::exception &e) {
                slog::error("invalid register number");
                slog::message(sline.str());
                return false;
            }

            opcode = 0xf000 | ((reg & 0xf) << 8) | 0x0065; // 0xfx65
            _writeOpcode(opcode);
        }
        else {
            slog::error("invalid mnemonic" + mnem);
            return false;
        }
    }

    if (!_writeOutput()) {
        slog::error("can't write binary file");
        return false;
    }

    return true;
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
        if (line == "") continue;
        line = line.substr(line.find_first_not_of(" \t")); // trimming leading whitespaces

        std::string firstWord = line.substr(0, line.find(" "));
        if (firstWord.size() <= 1) {
            slog::warning("invalid line");
            slog::message(line);
        }
        else if (firstWord[firstWord.size() - 1] == ':') {
            _labelTable[firstWord.substr(0, firstWord.size() - 1)] = lineNum * 2 + 0x200;
        }
        else {
            lineNum++;
            _src += line + "\n";
        }
    }
}
