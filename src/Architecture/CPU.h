#ifndef E_PUG_STATION_CPU
#define E_PUG_STATION_CPU

#include "Utilities/Constants.h"
#include "Interconnect.h"
#include "Utilities/Instruction.h"
#include <cstdint>

namespace ePugStation
{
	class CPU
	{
	public:
		CPU();
		~CPU() = default;

		uint32_t load8(uint32_t address) const;
		uint32_t load16(uint32_t address) const;
		uint32_t load32(uint32_t address) const;
		void store8(uint32_t address, uint32_t value);
		void store16(uint32_t address, uint32_t value);
		void store32(uint32_t address, uint32_t value);

		void runNextInstruction();
		void decodeAndExecute(uint32_t instruction);

	private:
		uint32_t m_ip;
		Instruction m_nextInstruction;
		std::array<uint32_t, CPU_REGISTERS> m_registers;
		std::array<uint32_t, CPU_REGISTERS> m_outputRegisters;
		std::pair<uint32_t, uint32_t> m_loadPair;
		Interconnect m_interconnect;

		uint32_t m_sr; // Temporary Cop0 register value. Sr = register #12 -- System status register (R/W)

		void matchOp(Instruction instruction);

		void setReg(uint32_t index, uint32_t value);
		void setReg(std::pair<uint32_t, uint32_t> setRegPair);

		// Branch 
		void branch(uint32_t offset);

		// Operations
		void opSLTU(Instruction instruction);
		void opBNE(Instruction instruction);
		void opLUI(Instruction instruction);
		void opRUI(Instruction instruction);
		void opLW(Instruction instruction);
		void opSW(Instruction instruction);
		void opSH(Instruction instruction);
		void opSLL(Instruction instruction);
		void opADDIU(Instruction instruction);
		void opADDU(Instruction instruction);
		void opADDI(Instruction instruction);
		void opJ(Instruction instruction);
		void opOR(Instruction instruction);
		void opORI(Instruction instruction);

		// Coprocessor operations
		void opCop0(Instruction instruction);

	};
}
#endif