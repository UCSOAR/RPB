/*
 * StateReco.hpp
 */
#include <cstdint>
#include "main.h"

#ifndef FLIGHTCONTROL_INC_STATERECO_HPP_
#define FLIGHTCONTROL_INC_STATERECO_HPP_

// STM32G4 specific constants
#define FLASH_BASE_ADDR        (0x08000000UL)
#define STATE_RECO_PAGE_SIZE   (2048) // STM32G4 uses 2KB pages
#define STATE_RECO_PAGE_COUNT  (2)

#define STATE_RECO_START_ADDR  (0x0807E000UL)

class StateRecoverer {
public:
	static StateRecoverer& Inst() {
		static StateRecoverer s(STATE_RECO_START_ADDR, STATE_RECO_PAGE_COUNT);
		return s;
	}

	StateRecoverer(uint32_t startAddress, uint32_t pageCount);

	// Returns the saved level (e.g. 0-9), or -1 if no valid state exists
	int32_t GetMostRecentState();

	// Saves the specified integer level
	bool SaveState(int32_t level);

private:
	const uint32_t startAddress;
	const uint32_t pageCount;
	uint32_t generation;

	uint32_t savesPerPage;
	uint32_t currentPageIndex;
	uint32_t currentWriteOffset;

	struct StateSave {
		// Maintained at 32-bits to prevent uninitialized struct padding bytes
		// from breaking the hardware CRC or the 64-bit flash write boundaries.
		int32_t level;
		uint32_t tick;
		uint32_t gen;
		uint32_t checksum;
	};

	uint32_t GetChecksum(const StateSave& save) const;
	bool FindMostRecent(StateSave& outSave, uint32_t& outPage, uint32_t& outOffset);
	StateSave ReadSave(uint32_t pageIdx, uint32_t offsetIdx);
	bool ErasePage(uint32_t pageAddress);
};

#endif /* FLIGHTCONTROL_INC_STATERECO_HPP_ */
