#include "Architecture/CPU.h"

int main(int argc, char* argv[])
{
	ePugStation::CPU cpu;

	while (true)
	{
		cpu.runNextInstruction();
	}

	return -1;
}
