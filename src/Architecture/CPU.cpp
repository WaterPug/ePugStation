#include "CPU.h"
#include <Utilities/OpUtilities.h>
#include <Utilities/Utils.h>
#include <iostream>

namespace ePugStation
{
	CPU::CPU() :
		m_ip(BIOS_ROM_LOGICAL),
		m_loadPair(std::make_pair(0, 0)),
		m_HI(0xdeadbeaf),
		m_LO(0xdeadbeaf),
		m_isBranching(false),
		m_delaySlot(false)
	{
		// Make constructor for cop0
		m_cop0.sr = 0;
		m_cop0.cause = 0;
		m_cop0.epc = 0;

		m_nextIp = m_ip + 4;

		m_registers.fill(0xdeadbeef);
		m_outputRegisters.fill(0xdeadbeef);
		m_registers[0] = 0; // Constant R0
	}

	void CPU::runNextInstruction()
	{
		m_instruction = Instruction(load32(m_ip));
		++debugLineCounter;

		m_delaySlot = m_isBranching;
		m_isBranching = false;

		m_currentIp = m_ip;
		m_ip = m_nextIp;

		if (!checkIfAlignedBy<ALIGNED_FOR_32_BITS>(m_currentIp))
		{
			exception(Exception::LoadAddressError);
			return;
		}

		//std::cout << ". Line counter : " << std::to_string(debugLineCounter) << "\n";
		// Point IP to next instruction
		m_nextIp += 4;
		setReg(m_loadPair);
		m_loadPair = std::make_pair(0, 0);
		decodeAndExecuteCurrentOp();
		m_registers = m_outputRegisters;
	}

	uint8_t CPU::load8(uint32_t address) const
	{
		return m_interconnect.load8(address);
	}

	void CPU::store8(uint32_t address, uint8_t value)
	{
		m_interconnect.store8(address, value);
	}

	uint16_t CPU::load16(uint32_t address) const
	{
		return m_interconnect.load16(address);
	}

	void CPU::store16(uint32_t address, uint16_t value)
	{
		m_interconnect.store16(address, value);
	}

	uint32_t CPU::load32(uint32_t address) const
	{
		return m_interconnect.load32(address);
	}

	void CPU::store32(uint32_t address, uint32_t value)
	{
		m_interconnect.store32(address, value);
	}

	void CPU::decodeAndExecuteCurrentOp()
	{
		switch (m_instruction.function)
		{
			// Sub Op
		case 0b000000: matchSubOp(); break;
		case 0b000001: matchSubBranchOp(); break;

			// Jump
		case 0b000010: opJ(); break;
		case 0b000011: opJAL(); break;

			// Branch
		case 0b000100: opBEQ(); break;
		case 0b000101: opBNE(); break;
		case 0b000110: opBLEZ(); break;
		case 0b000111: opBGTZ(); break;

			// Add
		case 0b001000: opADDI(); break;
		case 0b001001: opADDIU(); break;

			// Set
		case 0b001010: opSLTI(); break;
		case 0b001011: opSLTIU(); break;

			// And
		case 0b001100: opANDI(); break;

			// OR
		case 0b001101: opORI(); break;

			// XOR
		case 0b001110: opXORI(); break;

			// Cop
		case 0b010000: opCop0(); break;
		case 0b010001: opCop1(); break;
		case 0b010010: opCop2(); break;
		case 0b010011: opCop3(); break;

			// Load
		case 0b001111: opLUI(); break;
		case 0b100000: opLB(); break;
		case 0b100100: opLBU(); break;
		case 0b100001: opLH(); break;
		case 0b100010: opLWL(); break;
		case 0b100011: opLW(); break;
		case 0b100101: opLHU(); break;
		case 0b100110: opLWR(); break;

			// Store
		case 0b101000: opSB(); break;
		case 0b101001: opSH(); break;
		case 0b101010: opSWL(); break;
		case 0b101011: opSW(); break;
		case 0b101110: opSWR(); break;

			// LWCx
		case 0b110000: opLWC0(); break;
		case 0b110001: opLWC1(); break;
		case 0b110010: opLWC2(); break;
		case 0b110011: opLWC3(); break;

			// SWCx
		case 0b111000: opSWC0(); break;
		case 0b111001: opSWC1(); break;
		case 0b111010: opSWC2(); break;
		case 0b111011: opSWC3(); break;

		default:
			throw std::runtime_error("Instruction function not implemented with function : " + std::to_string(m_instruction.function));
		}
	}

	void CPU::matchSubBranchOp()
	{
		switch (m_instruction.t)
		{
		case 0b00000: opBLTZ(); break;
		case 0b00001: opBGEZ(); break;
		case 0b10000: opBLTZAL(); break;
		case 0b10001: opBGEZAL(); break;
		default:
			throw std::runtime_error("Unsupported t value at address : " + std::to_string(m_instruction.function));
		}
	}

	void CPU::matchSubOp()
	{
		switch (m_instruction.sub)
		{
			// Shifts
		case 0b000000: opSLL();  break;
		case 0b000010: opSRL();  break;
		case 0b000011: opSRA();  break;
		case 0b000100: opSLLV(); break;
		case 0b000110: opSRLV();  break;
		case 0b000111: opSRAV();  break;

			// Jump
		case 0b001000: opJR(); break;
		case 0b001001: opJALR(); break;

			// SYSCALL
		case 0b001100: opSYSCALL(); break;
		case 0b001101: opBREAK(); break;

			// Move
		case 0b010000: opMFHI(); break;
		case 0b010001: opMTHI(); break;
		case 0b010010: opMFLO(); break;
		case 0b010011: opMTLO(); break;

			// Mult
		case 0b011000: opMULT(); break;
		case 0b011001: opMULTU(); break;

			// Div
		case 0b011010: opDIV(); break;
		case 0b011011: opDIVU(); break;

			// Add
		case 0b100000: opADD(); break;
		case 0b100001: opADDU(); break;

			// Sub
		case 0b100010: opSUB(); break;
		case 0b100011: opSUBU(); break;

			// And
		case 0b100100: opAND(); break;

			// Or
		case 0b100101: opOR(); break;

			// XOR
		case 0b100110: opXOR(); break;
		case 0b100111: opNOR(); break;

			// Set
		case 0b101010: opSLT(); break;
		case 0b101011: opSLTU();  break;

		default:
			throw std::runtime_error("SubOperation not implemented...");
		}
	}

	void CPU::setReg(uint32_t index, uint32_t value)
	{
		m_outputRegisters[index] = value;
		m_outputRegisters[0] = 0;
	}

	void CPU::setReg(std::pair<uint32_t, uint32_t> setRegPair)
	{
		setReg(setRegPair.first, setRegPair.second);
	}

	void CPU::branch(uint32_t offset)
	{
		m_nextIp += (offset << 2);
		m_nextIp -= 4;
		m_isBranching = true;
	}

	void CPU::opCop0()
	{
		switch (m_instruction.s)
		{
		case 0b00000:
			opMFC();
			break;
		case 0b00100:
			opMTC();
			break;
		case 0b10000:
			opRFE();
			break;
		default:
			throw std::runtime_error("Unhandled COP0 opcode...");
		}
	}

	void CPU::opCop1()
	{
		exception(Exception::CoprocessorError); // Doesn't exist on Playstation
	}

	// GTE
	void CPU::opCop2()
	{
		throw std::runtime_error("Unhandled GTE instruction");
	}

	void CPU::opCop3()
	{
		exception(Exception::CoprocessorError); // Doesn't exist on Playstation

	}

	// Move From Cop0
	void CPU::opMFC()
	{
		const uint32_t copIndex = m_instruction.d;
		const uint32_t cpuIndex = m_instruction.t;
		uint32_t loadValue = 0;

		switch (copIndex)
		{
		case 12:
			loadValue = m_cop0.sr.value;
			break;
		case 13:
			loadValue = m_cop0.cause.value;
			break;
		case 14:
			loadValue = m_cop0.epc;
			break;
		default:
			throw std::runtime_error("Unhandled Move from COP0 index value...");
		}

		// fill load
		m_loadPair = std::make_pair(cpuIndex, loadValue);
	}

	// Move to Cop0
	void CPU::opMTC()
	{
		auto regValue = m_registers[m_instruction.t];

		// To be used when Cop0 registers are implemented
		// auto copRegIndex = m_instruction.d;
		switch (m_instruction.d)
		{
		case 3:
		case 5:
		case 6:
		case 7:
		case 9:
		case 11:
			if (regValue != 0)
				throw std::runtime_error("Unhandled write to cop0r");
			break;
		case 12:
			m_cop0.sr = regValue;
			break;
		case 13:
			if (regValue != 0)
				m_cop0.cause = regValue;
			break;
		case 14:
			if (regValue != 0)
				m_cop0.epc = regValue;
			break;
		default:
			throw std::runtime_error("Unhandled move to Cop0 index value...");
		}
	}

	void CPU::opLWC0()
	{
		exception(Exception::CoprocessorError);
	}

	void CPU::opLWC1()
	{
		exception(Exception::CoprocessorError);
	}

	void CPU::opLWC2()
	{
		throw std::runtime_error("Unhandled GTE LWC2 operation...");
	}

	void CPU::opLWC3()
	{
		exception(Exception::CoprocessorError);
	}

	void CPU::opSWC0()
	{
		exception(Exception::CoprocessorError);
	}

	void CPU::opSWC1()
	{
		exception(Exception::CoprocessorError);
	}

	void CPU::opSWC2()
	{
		throw std::runtime_error("Unhandled GTE SWC2 operation...");
	}

	void CPU::opSWC3()
	{
		exception(Exception::CoprocessorError);
	}

	void CPU::opIllegal()
	{
		std::cout << "Illegal instruction...\n";
		exception(Exception::IllegalInstruction);
	}

	void CPU::opRFE()
	{
		if ((m_instruction.op & 0x3f) != 0b010000)
		{
			throw std::runtime_error("Invalid cop0 instrcution...");
		}

		// Previous moved to current and old to previous
		m_cop0.sr.IEc = m_cop0.sr.IEp;
		m_cop0.sr.KUc = m_cop0.sr.KUp;

		m_cop0.sr.IEp = m_cop0.sr.IEo;
		m_cop0.sr.KUp = m_cop0.sr.KUo;
	}

	void CPU::opSLT()
	{
		auto s = static_cast<int32_t>(m_registers[m_instruction.s]);
		auto t = static_cast<int32_t>(m_registers[m_instruction.t]);

		setReg(m_instruction.d, s < t);
	}

	void CPU::opSLTU()
	{
		setReg(m_instruction.d, m_registers[m_instruction.s] < m_registers[m_instruction.t]);
	}

	void CPU::opSLTI()
	{
		auto s = static_cast<int32_t>(m_registers[m_instruction.s]);

		setReg(m_instruction.t, s < m_instruction.imm_se);
	}

	void CPU::opSLTIU()
	{
		setReg(m_instruction.t, m_registers[m_instruction.s] < m_instruction.imm);
	}

	void CPU::opSRA()
	{
		// For arithmetic shift : Signed integer
		int32_t signedValue = static_cast<int32_t>(m_registers[m_instruction.t]);

		// Here need to keep signed bit
		setReg(m_instruction.d, signedValue >> m_instruction.h);
	}

	void CPU::opSRAV()
	{
		// For arithmetic shift : Signed integer
		int32_t signedValue = static_cast<int32_t>(m_registers[m_instruction.t]);

		// Here need to keep signed bit
		setReg(m_instruction.d, signedValue >> (m_registers[m_instruction.s] & 0x1f));

	}

	void CPU::opSRL()
	{
		// For logical shift : Unsigned integer
		setReg(m_instruction.d, m_registers[m_instruction.t] >> m_instruction.h);
	}

	void CPU::opSRLV()
	{
		setReg(m_instruction.d, m_registers[m_instruction.t] >> (m_registers[m_instruction.s] & 0x1f));
	}

	void CPU::opSUB()
	{
		auto s = static_cast<int32_t>(m_registers[m_instruction.s]);
		auto t = static_cast<int32_t>(m_registers[m_instruction.t]);
		auto result = s - t;

		if (isSubOverflow(s, t, result))
		{
			exception(Exception::Overflow);
		}
		else
		{
			setReg(m_instruction.d, result);
		}
	}

	void CPU::opSUBU()
	{
		setReg(m_instruction.d, m_registers[m_instruction.s] - m_registers[m_instruction.t]);
	}

	void CPU::opXOR()
	{
		setReg(m_instruction.d, m_registers[m_instruction.s] ^ m_registers[m_instruction.t]);
	}

	void CPU::opXORI()
	{
		setReg(m_instruction.t, m_registers[m_instruction.s] ^ m_registers[m_instruction.imm]);
	}

	void CPU::opSYSCALL()
	{
		exception(Exception::SysCall);
	}

	void CPU::opBREAK()
	{
		exception(Exception::Break);
	}

	void CPU::opDIV()
	{
		int32_t numerator = static_cast<int32_t>(m_registers[m_instruction.s]);
		int32_t denominator = static_cast<int32_t>(m_registers[m_instruction.t]);

		if (denominator == 0) // Div by 0
		{
			m_HI = static_cast<uint32_t>(numerator);
			if (numerator >= 0)
			{
				m_LO = 0xFFFFFFFF;
			}
			else
			{
				m_LO = 0x1;
			}
		}
		else if (static_cast<uint32_t>(numerator) == 0x80000000 && denominator == -1)
		{
			m_LO = 0x80000000;
			m_HI = 0;
		}
		else
		{
			// Normal case
			m_LO = static_cast<uint32_t>(numerator / denominator);
			m_HI = static_cast<uint32_t>(numerator % denominator);
		}
	}

	void CPU::opDIVU()
	{
		uint32_t numerator = m_registers[m_instruction.s];
		uint32_t denominator = m_registers[m_instruction.t];

		if (denominator == 0) // Div by 0
		{
			m_HI = numerator;
			m_LO = 0xFFFFFFFF;
		}
		else
		{
			// Normal case
			m_LO = numerator / denominator;
			m_HI = numerator % denominator;
		}
	}

	void CPU::opOR()
	{
		setReg(m_instruction.d, m_registers[m_instruction.s] | m_registers[m_instruction.t]);
	}

	void CPU::opORI()
	{
		setReg(m_instruction.t, m_registers[m_instruction.s] | m_instruction.imm);
	}

	void CPU::opNOR()
	{
		setReg(m_instruction.d, !(m_registers[m_instruction.s] | m_registers[m_instruction.t]));
	}


	void CPU::opJ()
	{
		m_nextIp = (m_nextIp & 0xF0000000) | (m_instruction.immJump << 2);
		m_isBranching = true;
	}

	void CPU::opJAL()
	{
		setReg(31, m_nextIp);
		opJ();
	}

	void CPU::opJR()
	{
		m_nextIp = m_registers[m_instruction.s];
		m_isBranching = true;
	}

	void CPU::opJALR()
	{
		setReg(m_instruction.d, m_nextIp);
		m_nextIp = m_registers[m_instruction.s];
		m_isBranching = true;
	}

	void CPU::opADDU()
	{
		setReg(m_instruction.d, m_registers[m_instruction.s] + m_registers[m_instruction.t]);
	}

	void CPU::opAND()
	{
		setReg(m_instruction.d, m_registers[m_instruction.s] & m_registers[m_instruction.t]);
	}

	void CPU::opANDI()
	{
		setReg(m_instruction.t, m_registers[m_instruction.s] & m_instruction.imm);
	}

	void CPU::opBNE()
	{
		if (m_registers[m_instruction.s] != m_registers[m_instruction.t])
		{
			branch(m_instruction.imm_se);
		}
	}

	void CPU::opBEQ()
	{
		if (m_registers[m_instruction.s] == m_registers[m_instruction.t])
		{
			branch(m_instruction.imm_se);
		}
	}

	void CPU::opBGEZ()
	{
		int32_t s = static_cast<int32_t>(m_registers[m_instruction.s]);
		if (s >= 0)
		{
			branch(m_instruction.imm_se);
		}
	}

	void CPU::opBGEZAL()
	{
		int32_t s = static_cast<int32_t>(m_registers[m_instruction.s]);
		if (s >= 0)
		{
			setReg(31, m_nextIp);
			branch(m_instruction.imm_se);
		}
	}

	void CPU::opBGTZ()
	{
		int32_t s = static_cast<int32_t>(m_registers[m_instruction.s]);
		if (s > 0)
		{
			branch(m_instruction.imm_se);
		}
	}

	void CPU::opBLEZ()
	{
		int32_t s = static_cast<int32_t>(m_registers[m_instruction.s]);
		if (s <= 0)
		{
			branch(m_instruction.imm_se);
		}
	}

	void CPU::opBLTZ()
	{
		int32_t s = static_cast<int32_t>(m_registers[m_instruction.s]);
		if (s < 0)
		{
			branch(m_instruction.imm_se);
		}
	}

	void CPU::opBLTZAL()
	{
		int32_t s = static_cast<int32_t>(m_registers[m_instruction.s]);
		if (s < 0)
		{
			setReg(31, m_nextIp);
			branch(m_instruction.imm_se);
		}
	}

	void CPU::opADDIU()
	{
		setReg(m_instruction.t, m_registers[m_instruction.s] + m_instruction.imm_se);
	}

	void CPU::opADD()
	{
		int32_t s = static_cast<int32_t>(m_registers[m_instruction.s]);
		int32_t t = static_cast<int32_t>(m_registers[m_instruction.t]);
		int32_t result = s + t;

		if (isAddOverflow(s, t, result))
		{
			exception(Exception::Overflow);
		}
		else
		{
			setReg(m_instruction.d, result);
		}
	}

	void CPU::opADDI()
	{
		int32_t s = static_cast<int32_t>(m_registers[m_instruction.s]);
		int32_t result = s + m_instruction.imm_se;

		if (isAddOverflow(s, m_instruction.imm_se, result))
		{
			exception(Exception::Overflow);
		}
		else
		{
			setReg(m_instruction.t, result);
		}
	}

	void CPU::opSLL()
	{
		setReg(m_instruction.d, m_registers[m_instruction.t] << m_instruction.h);
	}

	void CPU::opSLLV()
	{
		auto shiftCount = m_registers[m_instruction.s] & 0x1f;
		setReg(m_instruction.d, m_registers[m_instruction.t] << shiftCount);
	}

	void CPU::opMFHI()
	{
		setReg(m_instruction.d, m_HI);
	}

	void CPU::opMFLO()
	{
		setReg(m_instruction.d, m_LO);
	}

	void CPU::opMTHI()
	{
		m_HI = m_registers[m_instruction.s];
	}

	void CPU::opMTLO()
	{
		m_LO = m_registers[m_instruction.s];
	}

	void CPU::opMULT()
	{
		int64_t s = static_cast<int32_t>(m_registers[m_instruction.s]);
		int64_t t = static_cast<int32_t>(m_registers[m_instruction.t]);
		uint64_t result = s * t;

		m_LO = static_cast<uint32_t>(result);
		m_HI = static_cast<uint32_t>(result >> 32);
	}

	void CPU::opMULTU()
	{
		uint64_t s = m_registers[m_instruction.s];
		uint64_t t = m_registers[m_instruction.t];
		uint64_t result = s * t;

		m_HI = static_cast<uint32_t>(result >> 32);
		m_LO = static_cast<uint32_t>(result);
	}

	void CPU::opSH()
	{
		if (m_cop0.sr.Isc == 1)
		{
			return;
		}
		uint32_t address = m_registers[m_instruction.s] + m_instruction.imm_se;
		if (checkIfAlignedBy<ALIGNED_FOR_16_BITS>(address))
		{
			store16(address, m_registers[m_instruction.t]);
		}
		else
		{
			exception(Exception::StoreAddressError);
		}
	}

	void CPU::opSB()
	{
		if (m_cop0.sr.Isc == 1)
		{
			return;
		}
		uint32_t address = m_registers[m_instruction.s] + m_instruction.imm_se;
		store8(address, 0xff & m_registers[m_instruction.t]);
	}

	void CPU::opSW()
	{
		if (m_cop0.sr.Isc == 1)
		{
			return;
		}
		uint32_t address = m_registers[m_instruction.s] + m_instruction.imm_se;
		if (checkIfAlignedBy<ALIGNED_FOR_32_BITS>(address))
		{
			store32(address, m_registers[m_instruction.t]);
		}
		else
		{
			exception(Exception::StoreAddressError);
		}
	}

	void CPU::opLW()
	{
		if (m_cop0.sr.Isc == 1)
		{
			return;
		}
		uint32_t address = m_registers[m_instruction.s] + m_instruction.imm_se;
		if (checkIfAlignedBy<ALIGNED_FOR_32_BITS>(address))
		{
			uint32_t index = m_instruction.t;
			m_loadPair = std::make_pair(index, load32(address));
		}
		else
		{
			exception(Exception::LoadAddressError);
		}
	}

	// Store Word left
	void CPU::opSWL()
	{
		auto address = m_registers[m_instruction.s] + m_instruction.imm_se;
		auto t = m_outputRegisters[m_instruction.t];

		auto alignedAddress = address & !3;
		auto currentMem = load32(alignedAddress);

		uint32_t newMem = 0;
		switch (address & 3)
		{
		case 0: newMem = (currentMem & 0xffffff00) | (t >> 24); break;
		case 1: newMem = (currentMem & 0xffff0000) | (t >> 16); break;
		case 2: newMem = (currentMem & 0xff000000) | (t >> 8); break;
		case 3: newMem = (currentMem & 0x00000000) | t; break;
		}
		store32(address, newMem);
	}

	// Store Word right
	void CPU::opSWR()
	{
		auto address = m_registers[m_instruction.s] + m_instruction.imm_se;
		auto t = m_outputRegisters[m_instruction.t];

		auto alignedAddress = address & !3;
		auto currentMem = load32(alignedAddress);

		uint32_t newMem = 0;
		switch (address & 3)
		{
		case 0: newMem = (currentMem & 0x00000000) | t; break;
		case 1: newMem = (currentMem & 0x000000ff) | (t << 8); break;
		case 2: newMem = (currentMem & 0x0000ffff) | (t << 16); break;
		case 3: newMem = (currentMem & 0x00ffffff) | (t << 24); break;
		}
		store32(address, newMem);
	}

	// Load Word left
	void CPU::opLWL()
	{
		auto address = m_registers[m_instruction.s] + m_instruction.imm_se;
		auto currentT = m_outputRegisters[m_instruction.t];

		auto alignedAddress = address & !3;
		auto alignedWord = load32(alignedAddress);
		uint32_t result = 0;
		switch (address & 3)
		{
		case 0: result = (currentT & 0x00ffffff) | (alignedWord << 24); break;
		case 1: result = (currentT & 0x0000ffff) | (alignedWord << 16); break;
		case 2: result = (currentT & 0x000000ff) | (alignedWord << 8); break;
		case 3: result = (currentT & 0x00000000) | alignedWord; break;
		}
		uint32_t index = m_instruction.t;
		m_loadPair = std::make_pair(index, result);
	}

	// Load Word right
	void CPU::opLWR()
	{
		auto address = m_registers[m_instruction.s] + m_instruction.imm_se;

		auto curV = m_outputRegisters[m_instruction.t];

		auto alignedAddress = address & !3;
		auto alignedWord = load32(alignedAddress);
		uint32_t result = 0;
		switch (address & 3)
		{
		case 0: result = (curV & 0x00000000) | alignedWord; break;
		case 1: result = (curV & 0xff000000) | (alignedWord >> 8); break;
		case 2: result = (curV & 0xffff0000) | (alignedWord >> 16); break;
		case 3: result = (curV & 0xffffff00) | (alignedWord >> 24); break;
		}
		uint32_t index = m_instruction.t;
		m_loadPair = std::make_pair(index, result);
	}

	void CPU::opLB()
	{
		if (m_cop0.sr.Isc == 1)
		{
			return;
		}
		uint32_t address = m_registers[m_instruction.s] + m_instruction.imm_se;
		int8_t loadValue = load8(address);

		uint32_t index = m_instruction.t;
		m_loadPair = std::make_pair(index, static_cast<uint32_t>(loadValue));
	}

	void CPU::opLBU()
	{
		if (m_cop0.sr.Isc == 1)
		{
			return;
		}
		uint32_t address = m_registers[m_instruction.s] + m_instruction.imm_se;

		uint32_t index = m_instruction.t;
		m_loadPair = std::make_pair(index, load8(address));
	}

	void CPU::opLH()
	{
		if (m_cop0.sr.Isc == 1)
		{
			return;
		}
		uint32_t address = m_registers[m_instruction.s] + m_instruction.imm_se;
		if (checkIfAlignedBy<ALIGNED_FOR_16_BITS>(address))
		{
			int16_t loadValue = load16(address);
			uint32_t index = m_instruction.t;
			m_loadPair = std::make_pair(index, static_cast<uint32_t>(loadValue));
		}
		else
		{
			exception(Exception::LoadAddressError);
		}
	}

	void CPU::opLHU()
	{
		if (m_cop0.sr.Isc == 1)
		{
			return;
		}
		uint32_t address = m_registers[m_instruction.s] + m_instruction.imm_se;
		if (checkIfAlignedBy<ALIGNED_FOR_16_BITS>(address))
		{
			uint32_t index = m_instruction.t;
			m_loadPair = std::make_pair(index, load16(address));
		}
		else
		{
			exception(Exception::LoadAddressError);
		}
	}

	void CPU::opLUI()
	{
		setReg(m_instruction.t, (m_instruction.imm << 16));
	}

	void CPU::exception(Exception exception)
	{
		uint32_t handler = m_cop0.sr.BEV == 1 ? 0xbfc00180 : 0x80000080;

		// Update Interrupt and Kernel/User mode bits
		m_cop0.sr.IEo = m_cop0.sr.IEp;
		m_cop0.sr.KUo = m_cop0.sr.KUp;
		m_cop0.sr.IEp = m_cop0.sr.IEc;
		m_cop0.sr.KUp = m_cop0.sr.KUc;

		m_cop0.cause.excode = static_cast<uint32_t>(exception);

		m_cop0.epc = m_currentIp;

		if (m_delaySlot)
		{
			m_cop0.epc -= 4;
			m_cop0.cause.BD = 1;
		}

		m_ip = handler;
		m_nextIp = m_ip + 4;
	}
}
