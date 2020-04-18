#include "CPU.h"
#include <Utilities/OpUtilities.h>
#include <iostream>

namespace ePugStation
{
	CPU::CPU() :
		m_ip(BIOS_ROM_LOGICAL),
		m_sr(0),
		m_loadPair({ 0, 0 })
	{
		m_nextInstruction.op = 0x0;

		m_registers.fill(0xdeadbeef);
		m_registers[0] = 0; // Constant R0
	}

	void CPU::runNextInstruction()
	{
		Instruction currentInstruction(m_nextInstruction);
		m_nextInstruction = Instruction(load32(m_ip));
		// Point IP to next instruction
		std::cout << "m_ip : " + std::to_string(m_ip) + "\n";
		m_ip += 4;
		setReg(m_loadPair);
		m_loadPair = { 0,0 };
		decodeAndExecute(currentInstruction.op);
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

	void CPU::decodeAndExecute(uint32_t instruction)
	{
		Instruction formatedInstruction;
		formatedInstruction.op = instruction;

		matchOp(formatedInstruction);
	}

	void CPU::matchOp(Instruction instruction)
	{
		switch (instruction.function)
		{
		case 0b000000:
			if (instruction.ShiftOperation.sub == 0b000000)
				opSLL(instruction);
			else if (instruction.ShiftOperation.sub == 0b100001)
				opADDU(instruction);
			else if (instruction.ShiftOperation.sub == 0b100101)
				opOR(instruction);
			else if (instruction.ShiftOperation.sub == 0b101011)
				opSLTU(instruction);
			else
				throw std::runtime_error("SubOperation not implemented...");
			break;
		case 0b000010:
			opJ(instruction);
			break;
		case 0b001000:
			opADDI(instruction);
			break;
		case 0b001001:
			opADDIU(instruction);
			break;
		case 0b000101:
			opBNE(instruction);
			break;
		case 0b001101:
			opORI(instruction);
			break;
		case 0b001111:
			opLUI(instruction);
			break;
		case 0b010000:
			opCop0(instruction);
			break;
		case 0b100011:
			opLW(instruction);
			break;
		case 0b101001:
			opSH(instruction);
			break;
		case 0b101011:
			opSW(instruction);
			break;

		default:
			throw std::runtime_error("Instruction function not implemented with function : " + std::to_string(instruction.function));
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
		m_ip -= 4;
		m_ip += (offset << 2);
	}

	void CPU::opCop0(Instruction instruction)
	{
		auto regValue = m_registers[instruction.t];
		switch (instruction.s)
		{
		case 0b00100:
			// To be used when Cop0 registers are implemented
			//auto copRegIndex = instruction.ShiftOperation.d;
			switch (instruction.ShiftOperation.d)
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
				m_sr = regValue;
				break;
			case 13:
				if (regValue != 0)
					throw std::runtime_error("Unhandled write to CAUSE register!");
				break;
			default:
				throw std::runtime_error("Unhandled cop Reg value...");
			}
			break;
		default:
			throw std::runtime_error("Unhandled cop0 instruction...");
		}
	}

	void CPU::opSLTI(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opSLTIU(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opSLTU(Instruction instruction)
	{
		setReg(instruction.ShiftOperation.d, m_registers[instruction.s] < m_registers[instruction.t]);
	}

	void CPU::opSRA(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opSRL(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opSRLV(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opSUB(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opSUBU(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opXOR(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opXORI(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opBNE(Instruction instruction)
	{
		if (m_registers[instruction.s] != m_registers[instruction.t])
		{
			branch(instruction.imm);
		}
	}

	void CPU::opDIV(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opDIVU(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opOR(Instruction instruction)
	{
		setReg(instruction.ShiftOperation.d, m_registers[instruction.s] | m_registers[instruction.ShiftOperation.t]);
	}

	void CPU::opORI(Instruction instruction)
	{
		setReg(instruction.t, m_registers[instruction.s] | instruction.imm);
	}

	void CPU::opJ(Instruction instruction)
	{
		m_ip = (m_ip & 0xF0000000) | (instruction.immJump() << 2);
	}

	void CPU::opJAL(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opJR(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opADDU(Instruction instruction)
	{
		setReg(instruction.ShiftOperation.d, m_registers[instruction.s] + m_registers[instruction.t]);
	}

	void CPU::opAND(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opANDI(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opBEQ(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opBGEZ(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opBGEZAL(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opBGTZ(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opBLEZ(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opBLTZ(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opBLTZAL(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opADDIU(Instruction instruction)
	{
		setReg(instruction.t, m_registers[instruction.s] + instruction.imm);
	}

	void CPU::opADD(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opADDI(Instruction instruction)
	{
		uint32_t result = 0;
		if (!safeAdd(m_registers[instruction.s], instruction.imm, result))
		{
			// TODO: Exception handling here
		}
		else
		{
			setReg(instruction.t, result);
		}
	}

	void CPU::opSLL(Instruction instruction)
	{
		setReg(instruction.ShiftOperation.d, m_registers[instruction.t] << instruction.ShiftOperation.h);
	}

	void CPU::opSLLV(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opLW(Instruction instruction)
	{
		if ((m_sr & 0x10000) != 0)
		{
			std::cout << "Ignoring load while cache is isolated...\n";
			return;
		}
		uint32_t address = m_registers[instruction.s] + instruction.imm;
		m_loadPair = { instruction.t, load32(address) };
	}

	void CPU::opMFHI(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opMFLO(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opMULT(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opMULTU(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opSH(Instruction instruction)
	{
		if ((m_sr & 0x10000) != 0)
		{
			std::cout << "Ignoring store while cache is isolated...\n";
			return;
		}
		uint32_t address = m_registers[instruction.s] + instruction.imm;
		store16(address, m_registers[instruction.t]);
	}

	void CPU::opSB(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opSW(Instruction instruction)
	{
		if ((m_sr & 0x10000) != 0)
		{
			std::cout << "Ignoring store while cache is isolated...\n";
			return;
		}
		uint32_t address = m_registers[instruction.s] + instruction.imm;
		store32(address, m_registers[instruction.t]);
	}

	void CPU::opLB(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opLH(Instruction instruction)
	{
		throw std::runtime_error("NOT IMPLEMENTED");
	}

	void CPU::opLUI(Instruction instruction)
	{
		setReg(instruction.t, (instruction.imm << 16));
	}
}
