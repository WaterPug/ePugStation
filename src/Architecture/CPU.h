#ifndef E_PUG_STATION_CPU
#define E_PUG_STATION_CPU

#include "Utilities/Constants.h"
#include "Interconnect.h"
#include "Utilities/Instruction.h"
#include <cstdint>

namespace ePugStation
{
	// TODO: Put these structs in appropriate place
	struct Cop0_Cause
	{
		Cop0_Cause() = default;
		Cop0_Cause(uint32_t causeValue) : value(causeValue) {}

		union
		{
			uint32_t value;
			struct
			{
				unsigned unused1 : 2;
				unsigned excode : 5;
				unsigned unused2 : 1;
				unsigned ip : 8;
				unsigned unused3 : 12;
				unsigned CE : 2;
				unsigned unused4 : 1;
				unsigned BD : 1;
			};
		};
	};

	struct Cop0_SR
	{
		Cop0_SR() = default;
		Cop0_SR(uint32_t causeValue) : value(causeValue) {}

		union
		{
			uint32_t value;
			struct
			{
				unsigned IEc : 1;
				unsigned KUc : 1;
				unsigned IEp : 1;
				unsigned KUp : 1;
				unsigned IEo : 1;
				unsigned KUo : 1;
				unsigned unused1 : 2;
				unsigned Im : 8;
				unsigned Isc : 1;
				unsigned Swc : 1;
				unsigned Pz : 1;
				unsigned CM : 1;
				unsigned PE : 1;
				unsigned TS : 1;
				unsigned BEV : 1;
				unsigned unused2 : 2;
				unsigned RE : 1;
				unsigned unused3 : 2;
				unsigned CU0 : 1;
				unsigned CU1 : 1;
				unsigned CU2 : 1;
				unsigned CU3 : 1;
			};
		};
	};

	struct Cop0
	{
		Cop0_SR sr;		  // Reg 12 : System status register (R/W)
		Cop0_Cause cause; // Reg 13 : Cause register
		uint32_t epc;	  // Reg 14 : EPC
	};

	enum Exception
	{
		Break = 0x9, // Break operation
		SysCall = 0x8, // Syscall operation
		Overflow = 0xc, // Arithmetic overflow
		LoadAddressError = 0x4,
		StoreAddressError = 0x5,
		CoprocessorError = 0xb,
		IllegalInstruction = 0xa
	};

	class CPU
	{
	public:
		CPU();
		~CPU() = default;

		void runNextInstruction();
	private:

		// Debugging purposes
		int debugLineCounter = 0;

		Cop0 m_cop0;

		Instruction m_instruction;

		uint32_t m_ip;
		uint32_t m_currentIp;
		uint32_t m_nextIp;

		uint32_t m_HI;
		uint32_t m_LO;

		bool m_isBranching; // If branch occured in current instruction
		bool m_delaySlot;   // If last op was branch, we are in delay slot

		std::array<uint32_t, CPU_REGISTERS> m_registers;
		std::array<uint32_t, CPU_REGISTERS> m_outputRegisters;
		std::pair<uint32_t, uint32_t> m_loadPair;
		Interconnect m_interconnect;

		void decodeAndExecuteCurrentOp();

		void matchSubOp();
		void matchSubBranchOp();

		void setReg(uint32_t index, uint32_t value);
		void setReg(std::pair<uint32_t, uint32_t> setRegPair);

		// Loads
		uint8_t load8(uint32_t address) const;
		uint16_t load16(uint32_t address) const;
		uint32_t load32(uint32_t address) const;

		// Stores
		void store8(uint32_t address, uint8_t value);
		void store16(uint32_t address, uint16_t value);
		void store32(uint32_t address, uint32_t value);

		// Branch 
		void branch(uint32_t offset);

		// ADD
		void opADD();
		void opADDI();
		void opADDIU();
		void opADDU();

		// AND 
		void opAND();
		void opANDI();

		// Branch
		void opBEQ();
		void opBGEZ();
		void opBGEZAL();
		void opBGTZ();
		void opBLEZ();
		void opBLTZ();
		void opBLTZAL();
		void opBNE();

		// Division
		void opDIV();
		void opDIVU();

		// Jump
		void opJ();
		void opJAL();
		void opJR();
		void opJALR();

		// Loads
		void opLB();
		void opLBU();
		void opLH();
		void opLHU();
		void opLUI();
		void opLW();
		void opLWL();
		void opLWR();

		// Move
		void opMFHI();
		void opMFLO();
		void opMTHI();
		void opMTLO();

		// Mult
		void opMULT();
		void opMULTU();

		// Or
		void opOR();
		void opORI();

		// Store
		void opSB();
		void opSW();
		void opSH();
		void opSWL();
		void opSWR();

		// Shift
		void opSLL();
		void opSLLV();

		// Set
		void opSLT();
		void opSLTI();
		void opSLTIU();
		void opSLTU();

		// Shift right
		void opSRA();
		void opSRAV();
		void opSRL();
		void opSRLV();

		// SUB
		void opSUB();
		void opSUBU();

		// XOR
		void opXOR();
		void opXORI();
		void opNOR();

		// SYSCALL
		void opSYSCALL();
		void opBREAK();

		// Coprocessor operations
		void opCop0();
		void opCop1();
		void opCop2();
		void opCop3();
		void opMFC();
		void opMTC();
		void opRFE();
		void opLWC0();
		void opLWC1();
		void opLWC2();
		void opLWC3();
		void opSWC0();
		void opSWC1();
		void opSWC2();
		void opSWC3();

		// Illegal
		void opIllegal();

		void exception(Exception exception);
	};
}
#endif