#ifndef E_PUG_STATION_GPU
#define E_PUG_STATION_GPU

#include "Utilities/Constants.h"
#include <cstdint>

namespace ePugStation
{
	enum class TextureDepth : unsigned
	{
		T4bit = 0,
		T8bit = 1,
		T15bit = 2,
		Reserved = 3
	};

	enum class HorizontalResolution : unsigned
	{
		H256 = 0,
		H368 = 1,
		H320 = 2,
		H512 = 4,
		H640 = 6
	};

	// utility : 
	inline HorizontalResolution hResFromFields(uint8_t hRes1, uint8_t hRes2)
	{
		return HorizontalResolution((hRes1 << 1) | hRes2);
	}

	enum class VerticalResolution : unsigned
	{
		V240 = 0,
		V480 = 1
	};

	enum class VideoMode : unsigned
	{
		NTSC = 0, // 480i60Hz
		PAL = 1   // 576i50Hz
	};

	enum class Field : unsigned
	{
		Bottom = 0,
		Top = 1
	};

	enum class DisplayDepth : unsigned
	{
		D15bit = 0,
		D24bit = 1
	};

	enum class DMADirection : unsigned
	{
		Off = 0,
		Fifo = 1,
		CpuToGpu = 2,
		VRamToCpu = 3
	};

	// Whole file needs some nice refactoring once things are cleared up
	struct VRAMDisplay
	{
		VRAMDisplay() : value(0) {}
		VRAMDisplay(uint32_t reg) : value(reg) {}
		VRAMDisplay(uint16_t startX, uint16_t startY) : value(0), xStart(startX), yStart(startY) {}
		VRAMDisplay(const VRAMDisplay& vramDisplay) : value(0), xStart(vramDisplay.xStart), yStart(vramDisplay.yStart) {}
		union
		{
			unsigned value;
			struct {
				unsigned xStart : 10;
				unsigned yStart : 9;
			};
		};
	};

	struct HSyncDisplay
	{
		HSyncDisplay() : value(0) {}
		HSyncDisplay(uint32_t reg) : value(reg) {}
		HSyncDisplay(uint16_t hStart, uint16_t hEnd) : value(0), start(hStart), end(hEnd) {}
		HSyncDisplay(const HSyncDisplay& hSyncDisplay) : value(0), start(hSyncDisplay.start), end(hSyncDisplay.end) {}
		union
		{
			unsigned value;
			struct {
				unsigned start : 12;
				unsigned end : 12;
			};
		};
	};

	struct VSyncDisplay
	{
		VSyncDisplay() : value(0) {}
		VSyncDisplay(uint32_t reg) : value(reg) {}
		VSyncDisplay(uint16_t vStart, uint16_t vEnd) : value(0), start(vStart), end(vEnd) {}
		VSyncDisplay(const VSyncDisplay& vSyncDisplay) : value(0), start(vSyncDisplay.start), end(vSyncDisplay.end) {}
		union
		{
			unsigned value;
			struct {
				unsigned start : 10;
				unsigned end : 10;
			};
		};
	};

	struct DrawingCoordinate
	{
		DrawingCoordinate() : value(0) {}
		DrawingCoordinate(uint32_t reg) : value(reg) {}
		DrawingCoordinate(uint16_t x, uint16_t y) : value(0), xValue(x), yValue(y) {}
		DrawingCoordinate(const DrawingCoordinate& drawingCoordinate) : value(0), xValue(drawingCoordinate.xValue), yValue(drawingCoordinate.yValue) {}
		union
		{
			unsigned value;
			struct {
				unsigned xValue : 10;
				unsigned yValue : 9;
			};
		};
	};

	struct DrawingOffset
	{
		DrawingOffset() : value(0) {}
		DrawingOffset(uint32_t reg) : value(reg) {}
		DrawingOffset(uint16_t x, uint16_t y) : value(0), xOffset(x), yOffset(y) {}
		DrawingOffset(const DrawingOffset& drawingOffset) : value(0), xOffset(drawingOffset.xOffset), yOffset(drawingOffset.yOffset) {}
		union
		{
			unsigned value;
			struct {
				signed xOffset : 11;
				signed yOffset : 11;
			};
		};
	};

	struct TextureWindowSettings
	{
		TextureWindowSettings() : value(0) {}
		TextureWindowSettings(uint32_t reg) : value(reg) {}
		TextureWindowSettings(uint8_t maskX, uint8_t maskY, uint8_t offsetX, uint8_t offsetY) :
			value(0), textureWindowMaskX(maskX), textureWindowMaskY(maskY), textureWindowOffsetX(offsetX), textureWindowOffsetY(offsetY) {}
		TextureWindowSettings(const TextureWindowSettings& drawingOffset) : 
			value(0), 
			textureWindowMaskX(drawingOffset.textureWindowMaskX), 
			textureWindowMaskY(drawingOffset.textureWindowMaskY), 
			textureWindowOffsetX(drawingOffset.textureWindowOffsetX),
			textureWindowOffsetY(drawingOffset.textureWindowOffsetY) {}
		union
		{
			uint32_t value;
			struct
			{
				unsigned textureWindowMaskX : 5;
				unsigned textureWindowMaskY : 5;
				unsigned textureWindowOffsetX : 5;
				unsigned textureWindowOffsetY : 5;
			};
		};
	};

	struct GPUStat
	{
		GPUStat() : value(0) {}
		GPUStat(uint32_t inValue) :value(inValue) {}
		union
		{
			uint32_t value;
			struct
			{
				unsigned texturePageXBase : 4; // 0-3
				unsigned texturePageYBase : 1; // 4
				unsigned semiTransparency : 2; // 5-6
				TextureDepth textureDepth : 2; // 7-8
				unsigned dithering : 1;			   // 9
				unsigned drawToDisplay : 1;		   // 10
				unsigned useMaskBit : 1;		   // 11
				unsigned preserveMaskedPixels : 1; // 12
				Field field : 1;			   // 13
				unsigned reverseFlag : 1;		   // 14
				unsigned textureDisabled : 1;	   // 15
				HorizontalResolution hRes : 3; // 16-18
				VerticalResolution vRes : 1;   // 19
				VideoMode videoMode : 1;	   // 20
				DisplayDepth displayDepth : 1; // 21
				unsigned isInterlaced : 1;		   // 22
				unsigned isDisplayDisabled : 1;	   // 23
				unsigned interruptRequest : 1;	   // 24
				unsigned dataRequest : 1;			// 25 -- Not clear what this is
				unsigned readyToReceiveCmd : 1;	   // 26
				unsigned readyToSendVramToCpu : 1;  // 27
				unsigned readyToReceiveDMABlock : 1; // 28
				DMADirection dmaDirection : 2; // 29-30
				unsigned drawOddLines : 1;		   // 31
			};
		};
	};

	struct GP0
	{
		GP0() : value(0) {}
		~GP0() = default;
		GP0(uint32_t inValue) : value(inValue) {}

		union
		{
			uint32_t value;
			TextureWindowSettings windowSettings; // Verify if this works...
			DrawingCoordinate drawingCoordinate;
			DrawingOffset drawingOffset;

			struct {
				unsigned : 24;
				unsigned command : 8;
			};
			struct {
				unsigned texturePageXBase : 4; // 0-3 (GPU STAT 0-3)
				unsigned texturePageYBase : 1; // 4   (GPU STAT 4)
				unsigned semiTransparency : 2; // 5-6 (GPU STAT 5-6)
				TextureDepth textureDepth : 2; // 7-8 (GPU STAT 7-8)
				unsigned dithering : 1;			   // 9   (GPU STAT 0)
				unsigned drawToDisplay : 1;		   // 10  (GPU STAT 10)
				unsigned textureDisabled : 1;	   // 11  (GPU STAT 15)
				unsigned textureRectXFlip : 1;	   // 12 
				unsigned textureRectYFlip : 1;	   // 13 (GPU STAT 13?) TBD
				unsigned unused1 : 10;
				unsigned cmd : 8;
			}CMD_E1;
			struct {
				unsigned useMaskBit : 1;
				unsigned preserveMaskedPixels : 1;
			}CMD_E6;

		};
	};

	struct GP1
	{
		GP1() : value(0) {};
		~GP1() = default;
		GP1(uint32_t inValue) : value(inValue) {}

		union
		{
			uint32_t value;
			struct
			{
				unsigned : 24;
				unsigned command : 8;
			};
			unsigned isDisplayDisabled : 1; // 3
			DMADirection dmaDirection : 2; // 4
			VRAMDisplay vramDisplay; // 5
			HSyncDisplay hSyncDisplay; // 6
			VSyncDisplay vSyncDisplay; // 7
			struct {
				unsigned hRes1 : 2;
				VerticalResolution vRes : 1;
				VideoMode videoMode : 1;
				DisplayDepth displayDepth : 1;
				unsigned isInterlace : 1;
				unsigned hRes2 : 1;
				unsigned reverseFlag : 1;
			}CMD_08;
		};
	};

	class GPU
	{
	public:
		GPU()
		{
			m_stat.isDisplayDisabled = true;
			m_stat.readyToReceiveCmd = 1;
			m_stat.readyToReceiveDMABlock = 1;
			m_stat.readyToSendVramToCpu = 1;
		};
		~GPU() = default;

		GPUStat getGPUStat() const { return m_stat; }

		void setGP0Command(uint32_t value)
		{
			m_gp0 = GP0(value);
			decodeAndExecuteGP0();
		}

		void setGP1Command(uint32_t value)
		{
			m_gp1 = GP1(value);
			decodeAndExecuteGP1();
		}

	private:
		// Probably better to couple these once I understand their use (Display rectangle ?)
		GPUStat m_stat;
		GP0 m_gp0;
		GP1 m_gp1;
		VRAMDisplay m_vramDisplay;
		VSyncDisplay m_vSyncDisplay;
		HSyncDisplay m_hSyncDisplay;
		TextureWindowSettings m_textureWindowSettings;
		DrawingCoordinate m_drawingAreaTopLeft;
		DrawingCoordinate m_drawingAreaBottomRight;
		DrawingOffset m_drawingOffset;

		void decodeAndExecuteGP1()
		{
			switch (m_gp1.command)
			{
			case 0x00: gp1SoftReset();  break;
			case 0x01: resetCommandBuffer(); break;
			case 0x02: acknowledgeGPUInterrupt(); break;
			case 0x03: displayEnable(m_gp1.isDisplayDisabled); break;
			case 0x04: dmaDirection(m_gp1.dmaDirection); break;
			case 0x05: startOfDisplayArea(m_gp1.vramDisplay); break;
			case 0x06: horizontalDisplayRange(m_gp1.hSyncDisplay); break;
			case 0x07: verticalDisplayRange(m_gp1.vSyncDisplay); break;
			case 0x08: displayMode(hResFromFields(m_gp1.CMD_08.hRes1, m_gp1.CMD_08.hRes2), m_gp1.CMD_08.vRes, m_gp1.CMD_08.videoMode, m_gp1.CMD_08.displayDepth, m_gp1.CMD_08.isInterlace, m_gp1.CMD_08.reverseFlag); break;
			default: throw("Unhandled GP1 command");
			}
		}

		void gp1SoftReset()
		{
			// TODO: Clear FIFO...

			// GP1 resets
			resetCommandBuffer();
			acknowledgeGPUInterrupt();
			displayEnable(true);
			dmaDirection(DMADirection::Off);
			startOfDisplayArea(VRAMDisplay(0, 0));
			horizontalDisplayRange(HSyncDisplay(0x200, 0xc00));
			verticalDisplayRange(VSyncDisplay(0x10, 0x100));
			displayMode(hResFromFields(0, 0), VerticalResolution::V240, VideoMode::NTSC, DisplayDepth::D15bit, true, false);

			// GP0 resets
			drawMode(0, 0, 0, TextureDepth::T4bit, false, false, false);
			textureWindowSettings(TextureWindowSettings(0, 0, 0, 0));
			setDrawingAreaTopLeft(DrawingCoordinate(0, 0));
			setDrawingAreaBottomRight(DrawingCoordinate(0, 0));
			setDrawingOffset(DrawingOffset(0, 0));
			setMaskBitSettings(0, 0);
		}

		// gp1 : 0x01
		void resetCommandBuffer()
		{
			// ??
		}

		// gp1 : 0x02
		void acknowledgeGPUInterrupt()
		{
			m_stat.interruptRequest = 0;
		}

		// gp1 : 0x03
		void displayEnable(bool isDisplayDisabled)
		{
			m_stat.isDisplayDisabled = isDisplayDisabled;
		}

		// gp1 : 0x04
		void dmaDirection(DMADirection direction)
		{
			m_stat.dmaDirection = direction;
		}

		// gp1 : 0x05
		void startOfDisplayArea(VRAMDisplay vramDisplay)
		{
			m_vramDisplay = vramDisplay;
		}

		// gp1 : 0x06
		void horizontalDisplayRange(HSyncDisplay hSyncDisplay)
		{
			m_hSyncDisplay = hSyncDisplay;
		}

		// gp1 : 0x07
		void verticalDisplayRange(VSyncDisplay vSyncDisplay)
		{
			m_vSyncDisplay = vSyncDisplay;
		}

		// gp1 : 0x08
		void displayMode(HorizontalResolution hRes, VerticalResolution vRes, VideoMode videoMode, DisplayDepth displayDepth, bool isInterlace, bool reverseFlag)
		{
			m_stat.hRes = hRes;
			m_stat.vRes = vRes;
			m_stat.videoMode = videoMode;
			m_stat.displayDepth = displayDepth;
			m_stat.isInterlaced = isInterlace;
			m_stat.reverseFlag = reverseFlag;
			if (reverseFlag == 1)
			{
				throw std::runtime_error("Reverse flag was enabled! TODO: Handle it");
			}
		}

		void decodeAndExecuteGP0()
		{
			switch (m_gp0.command)
			{
			case 0x00: break; // NOOP
			case 0xE1: drawMode(m_gp0.CMD_E1.texturePageXBase, m_gp0.CMD_E1.texturePageYBase, m_gp0.CMD_E1.semiTransparency, m_gp0.CMD_E1.textureDepth, m_gp0.CMD_E1.dithering, m_gp0.CMD_E1.drawToDisplay, m_gp0.CMD_E1.textureDisabled); break;
			case 0xE2: textureWindowSettings(m_gp0.windowSettings); break;
			case 0xE3: setDrawingAreaTopLeft(m_gp0.drawingCoordinate); break;
			case 0xE4: setDrawingAreaBottomRight(m_gp0.drawingCoordinate); break;
			case 0xE5: setDrawingOffset(m_gp0.drawingOffset); break;
			case 0xE6: setMaskBitSettings(m_gp0.CMD_E6.useMaskBit, m_gp0.CMD_E6.preserveMaskedPixels);  break;
			default: throw("Unhandled GP0 command");
			}
		}

		// gp0 : 0xE1
		void drawMode(uint8_t pageXBase, uint8_t pageYBase, uint8_t semiTransparency, TextureDepth textureDepth, bool dithering, bool drawToDisplay, bool textureDisabled)
		{
			// Update stats (Maybe fx for this)
			m_stat.texturePageXBase = pageXBase;
			m_stat.texturePageYBase = pageYBase;
			m_stat.semiTransparency = semiTransparency;
			m_stat.textureDepth = textureDepth;
			m_stat.dithering = dithering;
			m_stat.drawToDisplay = drawToDisplay;
			m_stat.textureDisabled = textureDisabled;
		}

		// gp0 : 0xE2
		void textureWindowSettings(TextureWindowSettings textureWindowSettings)
		{
			m_textureWindowSettings = textureWindowSettings;
		}

		// gp0 : 0XE3 --> Not stored in FIFO ? ** Probably executed immediately ** Use only when FIFO is empty?
		void setDrawingAreaTopLeft(DrawingCoordinate topLeftCoord)
		{
			m_drawingAreaTopLeft = topLeftCoord;
		}

		// gp0 : 0XE4 --> Not stored in FIFO ? ** Probably executed immediately ** 
		void setDrawingAreaBottomRight(DrawingCoordinate bottomRightCoord)
		{
			m_drawingAreaBottomRight = bottomRightCoord;
		}

		// gp0 : 0xE5 --> Not stored in FIFO ? ** Probably executed immediately ** 
		void setDrawingOffset(DrawingOffset offset)
		{
			m_drawingOffset = offset;
		}

		// gp0 : 0xE6
		void setMaskBitSettings(uint8_t useMaskBit, uint8_t preserveMaskedPixels)
		{
			m_stat.useMaskBit = useMaskBit;
			m_stat.preserveMaskedPixels = preserveMaskedPixels;
		}
	};
}
#endif