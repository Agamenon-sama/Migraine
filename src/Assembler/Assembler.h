#pragma once

#include <string>
#include <unordered_map>
#include <fstream>

class Assembler {
    public:
    Assembler();
    ~Assembler();

    bool assemble(std::string filename);

    private:
    std::string _filename;
    std::string _src;
    std::unordered_map<std::string, uint32_t> _labelTable;

    uint8_t *_output;
    int _opcodesWritten;

    void _preparse();
    bool _readFile();
    inline void _writeOpcode(uint16_t opcode);
    bool _writeOutput();
};
