#ifndef E_PUG_STATION_INSTRUCTION
#define E_PUG_STATION_INSTRUCTION

#include <cstdint>

namespace ePugStation
{
    enum class PrimaryOp : uint8_t
    {
        // Special
        SubOp = 0b000000,
        BranchOp = 0b000001,

        // Jump
        opJ = 0b000010,
        opJAL = 0b000011,

        // Branch
        opBEQ = 0b000100,
        opBNE = 0b000101,
        opBLEZ = 0b000110,
        opBGTZ = 0b000111,

        // Add
        opADDI = 0b001000,
        opADDIU = 0b001001,

        // Set
        opSLTI = 0b001010,
        opSLTIU = 0b001011,

        // And
        opANDI = 0b001100,

        // OR
        opORI = 0b001101,

        // XOR
        opXORI = 0b001110,

        // Cop
        opCop0 = 0b010000,
        opCop1 = 0b010001,
        opCop2 = 0b010010,
        opCop3 = 0b010011,

        // Load
        opLUI = 0b001111,
        opLB = 0b100000,
        opLBU = 0b100100,
        opLH = 0b100001,
        opLWL = 0b100010,
        opLW = 0b100011,
        opLHU = 0b100101,
        opLWR = 0b100110,

        // Store
        opSB = 0b101000,
        opSH = 0b101001,
        opSWL = 0b101010,
        opSW = 0b101011,
        opSWR = 0b101110,

        // LWCx
        opLWC0 = 0b110000,
        opLWC1 = 0b110001,
        opLWC2 = 0b110010,
        opLWC3 = 0b110011,

        // SWCx
        opSWC0 = 0b111000,
        opSWC1 = 0b111001,
        opSWC2 = 0b111010,
        opSWC3 = 0b111011
    };

    enum class SecondaryOp : uint8_t
    {
        // Shifts
        opSLL = 0b000000,
        opSRL = 0b000010,
        opSRA = 0b000011,
        opSLLV = 0b000100,
        opSRLV = 0b000110,
        opSRAV = 0b000111,

        // Jump
        opJR = 0b001000,
        opJALR = 0b001001,

        // SYSCALL
        opSYSCALL = 0b001100,
        opBREAK = 0b001101,

        // Move
        opMFHI = 0b010000,
        opMTHI = 0b010001,
        opMFLO = 0b010010,
        opMTLO = 0b010011,

        // Mult
        opMULT = 0b011000,
        opMULTU = 0b011001,

        // Div
        opDIV = 0b011010,
        opDIVU = 0b011011,

        // Add
        opADD = 0b100000,
        opADDU = 0b100001,

        // Sub
        opSUB = 0b100010,
        opSUBU = 0b100011,

        // And
        opAND = 0b100100,

        // Or
        opOR = 0b100101,

        // XOR
        opXOR = 0b100110,
        opNOR = 0b100111,

        // Set
        opSLT = 0b101010,
        opSLTU = 0b101011
    };

    // TODO: Make type safe index
    //struct InstructionIndex
    //{
    //    uint32_t index;
    //};

    struct Instruction
    {
        Instruction() : value(0) {}
        ~Instruction() = default;

        Instruction(uint32_t operation) : value(operation) {}
        Instruction(const Instruction& instruction) : value(instruction.value) {}

        union
        {
            uint32_t value;
            int16_t imm_se : 16;
            uint16_t imm : 16;
            uint32_t immJump : 26;

            struct {
                uint32_t ignore1 : 6; // [5:0]
                uint32_t h : 5; // [10:6]
                uint32_t d : 5; // [15:11]
                uint32_t t : 5; // [20:16]
                uint32_t s : 5; // [25:21]
                uint32_t ignore2 : 6; // [31:26]
            }reg;

            struct {
                SecondaryOp seconday : 6; // [5:0]
                uint32_t ignore : 20; // [25:6]
                PrimaryOp primary : 6; // [31:26]
            }op;
        };
    };
}
#endif