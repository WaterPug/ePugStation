#include "CPU.h"
#include <Utilities/OpUtilities.h>
#include <Utilities/Utils.h>
#include <iostream>

namespace ePugStation
{
	CPU::CPU() :
		m_ip(BIOS_ROM_LOGICAL),
		m_loadPair({ 0, 0 }),
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
		m_currentInstruction = Instruction(load32(m_ip));

		m_delaySlot = m_isBranching;
		m_isBranching = false;

		m_currentIp = m_ip;
		m_ip = m_nextIp;

		if (!checkIfAlignedBy<ALIGNED_FOR_32_BITS>(m_currentIp))
		{
			exception(Exception::LoadAddressError);
			return;
		}

		std::cout << "m_ip : " + std::to_string(m_currentIp) + "\n";
		// Point IP to next instruction
		m_nextIp += 4;
		setReg(m_loadPair);
		m_loadPair = { 0,0 };
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
		switch (m_currentInstruction.function)
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

			// Load
		case 0b001111: opLUI(); break;
		case 0b100000: opLB(); break;
		case 0b100100: opLBU(); break;
		case 0b100001: opLH(); break;
		case 0b100011: opLW(); break;

			// Store
		case 0b101000: opSB(); break;
		case 0b101001: opSH(); break;
		case 0b101011: opSW(); break;

		default:
			throw std::runtime_error("Instruction function not implemented with function : " + std::to_string(m_currentInstruction.function));
		}
	}

	void CPU::matchSubBranchOp()
	{
		switch (m_currentInstruction.t)
		{
		case 0b00000: opBLTZ(); break;
		case 0b00001: opBGEZ(); break;
		case 0b10000: opBLTZAL(); break;
		case 0b10001: opBGEZAL(); break;
		default:
			throw std::runtime_error("Unsupported t value at address : " + std::to_string(m_currentInstruction.function));
		}
	}

	void CPU::matchSubOp()
	{
		switch (m_currentInstruction.SubOperation.sub)
		{
			// Shifts
		case 0b000000: opSLL();  break;
		case 0b000010: opSRL();  break;
		case 0b000011: opSRA();  break;
		case 0b000100: opSLLV(); break;
		case 0b000110: opSRLV();  break;

			// Jump
		case 0b001000: opJR(); break;
		case 0b001001: opJALR(); break;

			// SYSCALL
		case 0b001100: opSYSCALL(); break;

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
		m_outputRegisters[setRegPair.first] = setRegPair.second;
		m_outputRegisters[0] = 0;
	}

	void CPU::branch(uint32_t offset)
	{
		m_nextIp -= 4;
		m_nextIp += (offset << 2);
		m_isBranching = true;
	}

	void CPU::opCop0()
	{
		switch (m_currentInstruction.s)
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

	// Move From Cop0
	void CPU::opMFC()
	{
		const uint32_t copIndex = m_currentInstruction.SubOperation.d;
		const uint32_t cpuIndex = m_currentInstruction.t;
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
		m_loadPair = { cpuIndex, loadValue };
	}

	// Move to Cop0
	void CPU::opMTC()
	{
		auto regValue = m_registers[m_currentInstruction.t];

		// To be used when Cop0 registers are implemented
		// auto copRegIndex = m_currentInstruction.SubOperation.d;
		switch (m_currentInstruction.SubOperation.d)
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

	void CPU::opRFE()
	{
		if ((m_currentInstruction.op & 0x3f) != 0b010000)
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
		auto s = static_cast<int32_t>(m_registers[m_currentInstruction.s]);
		auto t = static_cast<int32_t>(m_registers[m_currentInstruction.t]);

		setReg(m_currentInstruction.SubOperation.d, s < t);
	}

	void CPU::opSLTU()
	{
		setReg(m_currentInstruction.SubOperation.d, m_registers[m_currentInstruction.s] < m_registers[m_currentInstruction.t]);
	}

	void CPU::opSLTI()
	{
		setReg(m_currentInstruction.t, static_cast<int32_t>(m_registers[m_currentInstruction.s]) < m_currentInstruction.imm);
	}

	void CPU::opSLTIU()
	{
		setReg(m_currentInstruction.t, m_registers[m_currentInstruction.s] < m_currentInstruction.imm);
	}

	void CPU::opSRA()
	{
		// For arithmetic shift : Signed integer
		int32_t signedValue = static_cast<int32_t>(m_registers[m_currentInstruction.t]);

		// Here need to keep signed bit
		setReg(m_currentInstruction.SubOperation.d, signedValue >> m_currentInstruction.SubOperation.h);
	}

	void CPU::opSRL()
	{
		// For logical shift : Unsigned integer
		setReg(m_currentInstruction.SubOperation.d, m_registers[m_currentInstruction.t] >> m_currentInstruction.SubOperation.h);
	}

	void CPU::opSRLV()
	{
		setReg(m_currentInstruction.SubOperation.d, m_registers[m_currentInstruction.t] >> m_registers[m_currentInstruction.s]);
	}

	void CPU::opSUB()
	{
		setReg(m_currentInstruction.SubOperation.d, m_registers[m_currentInstruction.s] - m_registers[m_currentInstruction.t]);
	}

	void CPU::opSUBU()
	{
		setReg(m_currentInstruction.SubOperation.d, m_registers[m_currentInstruction.s] - m_registers[m_currentInstruction.t]);
	}

	void CPU::opXOR()
	{
		setReg(m_currentInstruction.SubOperation.d, m_registers[m_currentInstruction.s] ^ m_registers[m_currentInstruction.t]);
	}

	void CPU::opXORI()
	{
		setReg(m_currentInstruction.t, m_registers[m_currentInstruction.s] ^ m_registers[m_currentInstruction.imm]);
	}

	void CPU::opSYSCALL()
	{
		exception(Exception::SysCall);
	}

	void CPU::opDIV()
	{
		int32_t numerator = static_cast<int32_t>(m_registers[m_currentInstruction.s]);
		int32_t denominator = static_cast<int32_t>(m_registers[m_currentInstruction.t]);

		if (denominator == 0) // Div by 0
		{
			m_HI = numerator;
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
		uint32_t numerator = m_registers[m_currentInstruction.s];
		uint32_t denominator = m_registers[m_currentInstruction.t];

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
		setReg(m_currentInstruction.SubOperation.d, m_registers[m_currentInstruction.s] | m_registers[m_currentInstruction.SubOperation.t]);
	}

	void CPU::opORI()
	{
		// TODO: Understand why imm is 32bit instead of 16 !!
		// Might be worth to make functions with types for now to be less error prone...
		setReg(m_currentInstruction.t, m_registers[m_currentInstruction.s] | (0x0000ffff & m_currentInstruction.imm));
	}

	void CPU::opJ()
	{
		m_nextIp = (m_nextIp & 0xF0000000) | (m_currentInstruction.immJump() << 2);
		m_isBranching = true;
	}

	void CPU::opJAL()
	{
		setReg(31, m_nextIp);
		opJ();
	}

	void CPU::opJR()
	{
		m_nextIp = m_registers[m_currentInstruction.s];
		m_isBranching = true;
	}

	void CPU::opJALR()
	{
		setReg(m_currentInstruction.SubOperation.d, m_nextIp);
		m_nextIp = m_registers[m_currentInstruction.s];
		m_isBranching = true;
	}

	void CPU::opADDU()
	{
		setReg(m_currentInstruction.SubOperation.d, m_registers[m_currentInstruction.s] + m_registers[m_currentInstruction.t]);
	}

	void CPU::opAND()
	{
		setReg(m_currentInstruction.SubOperation.d, m_registers[m_currentInstruction.s] & m_registers[m_currentInstruction.t]);
	}

	void CPU::opANDI()
	{
		setReg(m_currentInstruction.t, m_registers[m_currentInstruction.s] & m_currentInstruction.imm);
	}

	void CPU::opBNE()
	{
		if (m_registers[m_currentInstruction.s] != m_registers[m_currentInstruction.t])
		{
			branch(m_currentInstruction.imm);
		}
	}

	void CPU::opBEQ()
	{
		if (m_registers[m_currentInstruction.s] == m_registers[m_currentInstruction.t])
		{
			branch(m_currentInstruction.imm);
		}
	}

	void CPU::opBGEZ()
	{
		if (static_cast<int32_t>(m_registers[m_currentInstruction.s]) >= 0)
		{
			branch(m_currentInstruction.imm);
		}
	}

	void CPU::opBGEZAL()
	{
		if (static_cast<int32_t>(m_registers[m_currentInstruction.s]) >= 0)
		{
			setReg(31, m_nextIp);
			branch(m_currentInstruction.imm);
		}
	}

	void CPU::opBGTZ()
	{
		if (static_cast<int32_t>(m_registers[m_currentInstruction.s]) > 0)
		{
			branch(m_currentInstruction.imm);
		}
	}

	void CPU::opBLEZ()
	{
		if (static_cast<int32_t>(m_registers[m_currentInstruction.s]) <= 0)
		{
			branch(m_currentInstruction.imm);
		}
	}

	void CPU::opBLTZ()
	{
		if (static_cast<int32_t>(m_registers[m_currentInstruction.s]) < 0)
		{
			branch(m_currentInstruction.imm);
		}
	}

	void CPU::opBLTZAL()
	{
		if (static_cast<int32_t>(m_registers[m_currentInstruction.s]) < 0)
		{
			setReg(31, m_nextIp);
			branch(m_currentInstruction.imm);
		}
	}

	void CPU::opADDIU()
	{
		setReg(m_currentInstruction.t, m_registers[m_currentInstruction.s] + m_currentInstruction.imm);
	}

	void CPU::opADD()
	{
		uint32_t result = 0;
		if (!safeAdd(m_registers[m_currentInstruction.s], m_currentInstruction.imm, result))
		{
			exception(Exception::Overflow);
		}
		else
		{
			setReg(m_currentInstruction.SubOperation.d, m_registers[m_currentInstruction.s] + m_registers[m_currentInstruction.t]);
		}
	}

	void CPU::opADDI()
	{
		uint32_t result = 0;
		if (!safeAdd(m_registers[m_currentInstruction.s], m_currentInstruction.imm, result))
		{
			exception(Exception::Overflow);
		}
		else
		{
			setReg(m_currentInstruction.t, result);
		}
	}

	void CPU::opSLL()
	{
		setReg(m_currentInstruction.SubOperation.d, m_registers[m_currentInstruction.t] << m_currentInstruction.SubOperation.h);
	}

	void CPU::opSLLV()
	{
		setReg(m_currentInstruction.SubOperation.d, m_registers[m_currentInstruction.t] << m_registers[m_currentInstruction.s]);
	}

	void CPU::opMFHI()
	{
		setReg(m_currentInstruction.SubOperation.d, m_HI);
	}

	void CPU::opMFLO()
	{
		setReg(m_currentInstruction.SubOperation.d, m_LO);
	}

	void CPU::opMTHI()
	{
		m_HI = m_registers[m_currentInstruction.s];
	}

	void CPU::opMTLO()
	{
		m_LO = m_registers[m_currentInstruction.s];
	}

	void CPU::opMULT()
	{
		auto s = static_cast<int32_t>(m_registers[m_currentInstruction.s]);
		auto t = static_cast<int32_t>(m_registers[m_currentInstruction.t]);
		m_LO = s * t;
	}

	void CPU::opMULTU()
	{
		m_LO = m_registers[m_currentInstruction.s] * m_registers[m_currentInstruction.t];
	}

	void CPU::opSH()
	{
		if (m_cop0.sr.Isc == 1)
		{
			std::cout << "Ignoring store while cache is isolated...\n";
			return;
		}
		uint32_t address = m_registers[m_currentInstruction.s] + m_currentInstruction.imm;
		if (checkIfAlignedBy<ALIGNED_FOR_16_BITS>(address))
		{
			store16(address, m_registers[m_currentInstruction.t]);
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
			std::cout << "Ignoring store while cache is isolated...\n";
			return;
		}
		uint32_t address = m_registers[m_currentInstruction.s] + m_currentInstruction.imm;
		store8(address, 0xff & m_registers[m_currentInstruction.t]);
	}

	void CPU::opSW()
	{
		if (m_cop0.sr.Isc == 1)
		{
			std::cout << "Ignoring store while cache is isolated...\n";
			return;
		}
		uint32_t address = m_registers[m_currentInstruction.s] + m_currentInstruction.imm;
		if (checkIfAlignedBy<ALIGNED_FOR_32_BITS>(address))
		{
			store32(address, m_registers[m_currentInstruction.t]);
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
			std::cout << "Ignoring load while cache is isolated...\n";
			return;
		}
		uint32_t address = m_registers[m_currentInstruction.s] + m_currentInstruction.imm;
		if (checkIfAlignedBy<ALIGNED_FOR_32_BITS>(address))
		{
			m_loadPair = { m_currentInstruction.t, load32(address) };
		}
		else
		{
			exception(Exception::LoadAddressError);
		}
	}

	void CPU::opLB()
	{
		if (m_cop0.sr.Isc == 1)
		{
			std::cout << "Ignoring load while cache is isolated...\n";
			return;
		}
		uint32_t address = m_registers[m_currentInstruction.s] + m_currentInstruction.imm;
		int8_t loadValue = load8(address);
		m_loadPair = { m_currentInstruction.t, static_cast<uint32_t>(loadValue) };
	}

	void CPU::opLBU()
	{
		if (m_cop0.sr.Isc == 1)
		{
			std::cout << "Ignoring load while cache is isolated...\n";
			return;
		}
		uint32_t address = m_registers[m_currentInstruction.s] + m_currentInstruction.imm;
		m_loadPair = { m_currentInstruction.t, load8(address) };
	}

	void CPU::opLH()
	{
		if (m_cop0.sr.Isc == 1)
		{
			std::cout << "Ignoring load while cache is isolated...\n";
			return;
		}
		uint32_t address = m_registers[m_currentInstruction.s] + m_currentInstruction.imm;
		if (checkIfAlignedBy<ALIGNED_FOR_16_BITS>(address))
		{
			m_loadPair = { m_currentInstruction.t, load16(address) };
		}
		else
		{
			exception(Exception::LoadAddressError);
		}
	}

	void CPU::opLUI()
	{
		setReg(m_currentInstruction.t, (m_currentInstruction.imm << 16));
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
