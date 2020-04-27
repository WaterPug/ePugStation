#ifndef E_PUG_STATION_DMA
#define E_PUG_STATION_DMA

#include "Utilities/Constants.h"

namespace ePugStation
{
	enum class SyncMode : uint8_t
	{
		Manual = 0,
		Sync = 1,
		LinkedList = 2,
		Other = 3
	};

	enum class StepDirection : uint8_t
	{
		Forward = 0,
		Backward = 1
	};

	enum class DMATransferDirection : uint8_t
	{
		ToRam = 0,
		FromRam = 1
	};

	enum class StartTrigger : uint8_t
	{
		Normal = 0,
		Manual = 1
	};

	struct DMABlockChannel
	{
		union
		{
			uint32_t value;
			struct
			{
				uint16_t numberOfWords : 16;
				uint16_t unused : 16;
			}SyncMode0;

			struct
			{
				uint16_t blockSize : 16;
				uint16_t amountOfBlocks : 16;
			}SyncMode1;
		};
	};

	struct DMAInterrupt
	{
		DMAInterrupt() = default;
		DMAInterrupt(uint32_t in_value) : value(in_value) {}
		union
		{
			uint32_t value;
			struct
			{
				uint8_t unknown : 6;
				uint16_t unused : 9;
				uint8_t forceIRQ : 1;
				uint8_t IRQEnable : 7;
				uint8_t IRQMasterEnable : 1;
				uint8_t IRQFlags : 7;
				uint8_t IRQMasterFlag : 1;
			};
		};
	};

	struct DMAChannelControl
	{
		DMAChannelControl() = default;
		DMAChannelControl(uint32_t in_value) : value(in_value) {}
		union
		{
			uint32_t value;
			struct // See http://problemkaputt.de/psx-spx.htm#dmachannels for definitions
			{
				DMATransferDirection isFromRam : 1;
				StepDirection memoryAddressStep : 1;
				uint8_t unused1 : 6;
				bool choppingEnable : 1;
				SyncMode syncMode : 2;
				uint8_t unused2 : 5;
				uint8_t choppingDMAWindowSize : 3;
				bool unused3 : 1;
				uint8_t choppingCPUWindowSize : 3;
				bool unused4 : 1;
				bool enable : 1;
				uint8_t unused5 : 3;
				StartTrigger startTrigger : 1; // 0 == normal, 1 == manual start
				uint8_t unknown : 3;
			};
		};
	};

	struct DMABaseAddress
	{
		union {
			uint32_t value;
			uint32_t address : 24;
		};
	};

	struct DMAChannel
	{
		uint32_t baseAddress;
		DMAChannelControl control;
		DMABlockChannel blockChannel;

		uint32_t getTransferSize()
		{
			if (control.syncMode == SyncMode::Manual)
			{
				return blockChannel.SyncMode0.numberOfWords;
			}
			else if (control.syncMode == SyncMode::Sync)
			{
				return blockChannel.SyncMode1.amountOfBlocks * blockChannel.SyncMode1.blockSize;
			}
			else if (control.syncMode == SyncMode::LinkedList)
			{
				throw std::runtime_error("Linked not supported");
				//return 0xffffff;
			}
		}
	};

	class DMA
	{
	public:
		DMA() : m_control(DMA_RESET) {};
		~DMA() = default;

		uint32_t getControl() const { return m_control; }
		void setControl(uint32_t value) { m_control = value; }

		uint32_t getInterrupt() const { return m_interrupt.value; }
		void setInterrupt(uint32_t value) { m_interrupt.value = value; }

		uint32_t getChannelControl(uint32_t index) const { return m_channels[index].control.value; }
		void setChannelControl(uint32_t index, uint32_t value) { m_channels[index].control.value = value; }

		uint32_t getChannelBaseAddress(uint32_t index) const { return m_channels[index].baseAddress; }
		void setChannelBaseAddress(uint32_t index, uint32_t value) { m_channels[index].baseAddress = value; }

		uint32_t getChannelBlockControl(uint32_t index) const { return m_channels[index].blockChannel.value; }
		void setChannelBlockControl(uint32_t index, uint32_t value) { m_channels[index].blockChannel.value = value; }

		bool isChannelActive(uint32_t index) const 
		{ 
			bool trigger = true;
			if (m_channels[index].control.syncMode == SyncMode::Manual)
			{
				trigger = m_channels[index].control.startTrigger == StartTrigger::Manual;
			}
			return trigger && m_channels[index].control.enable;
		}

		void finalizeCopy(uint32_t index)
		{
			m_channels[index].control.enable = false;
			m_channels[index].control.startTrigger = StartTrigger::Normal;
		}

		DMAChannel getChannel(uint32_t index) { return m_channels[index]; };

	private:
		uint32_t m_control;
		DMAInterrupt m_interrupt;
		DMAChannel m_channels[7];
	};
}
#endif