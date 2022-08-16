#pragma once

#include <string>

class Disassembler {
    public:
    Disassembler() = default;

    std::string disassemble(const uint8_t *rom);
    const std::string& getDisassembled() const { return _disassembled; }
    
    private:
    std::string _disassembled;
};
