/*
 * StateReco.cpp
 */
#include "AirbrakesStateReco.hpp"

extern CRC_HandleTypeDef hcrc;

StateRecoverer::StateRecoverer(uint32_t startAddress, uint32_t pageCount)
    : startAddress(startAddress), pageCount(pageCount) {
    
    savesPerPage = STATE_RECO_PAGE_SIZE / sizeof(StateSave);

    StateSave recentSave;
    uint32_t recentPage = 0;
    uint32_t recentOffset = 0;

    if (FindMostRecent(recentSave, recentPage, recentOffset)) {
        generation = recentSave.gen + 1;
        currentPageIndex = recentPage;
        currentWriteOffset = recentOffset + 1;

        if (currentWriteOffset >= savesPerPage) {
            currentPageIndex = (currentPageIndex + 1) % pageCount;
            currentWriteOffset = 0;
        }
    } else {
        generation = 1;
        currentPageIndex = 0;
        currentWriteOffset = 0;
    }
}

bool StateRecoverer::GetMostRecentState() {
	StateSave recentSave;
	uint32_t page, off;
	if (FindMostRecent(recentSave, page, off)) {
		return recentSave.airbrakes != 0;
	}
	return false;
}

bool StateRecoverer::SaveState(bool airbrakes) {
	StateSave s;
	s.airbrakes = airbrakes ? 1 : 0;
	s.tick = HAL_GetTick();
	s.gen = generation;
	s.checksum = GetChecksum(s);

	uint32_t pageAddress = startAddress + (currentPageIndex * STATE_RECO_PAGE_SIZE);

	HAL_FLASH_Unlock();

	if (currentWriteOffset == 0) {
		if (!ErasePage(pageAddress)) {
			HAL_FLASH_Lock();
			return false;
		}
	}

	uint32_t writeAddress = pageAddress + (currentWriteOffset * sizeof(StateSave));

    uint64_t* dataChunks = (uint64_t*)&s;

	if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, writeAddress, dataChunks[0]) != HAL_OK ||
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, writeAddress + 8, dataChunks[1]) != HAL_OK) {
		HAL_FLASH_Lock();
		return false;
	}

	HAL_FLASH_Lock();

	currentWriteOffset++;
	if (currentWriteOffset >= savesPerPage) {
		currentPageIndex = (currentPageIndex + 1) % pageCount;
		currentWriteOffset = 0;
	}

	return true;
}

bool StateRecoverer::ErasePage(uint32_t pageAddress) {
    FLASH_EraseInitTypeDef eraseInit;
    uint32_t pageError = 0;

    eraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseInit.Banks = FLASH_BANK_1;

    eraseInit.Page = (pageAddress - FLASH_BASE_ADDR) / STATE_RECO_PAGE_SIZE;
    eraseInit.NbPages = 1;

    return (HAL_FLASHEx_Erase(&eraseInit, &pageError) == HAL_OK);
}

uint32_t StateRecoverer::GetChecksum(const StateSave& save) const {
	uint32_t buf[] = {save.airbrakes, save.tick, save.gen};
	return HAL_CRC_Calculate(&hcrc, &buf[0], sizeof(buf)/sizeof(uint32_t));
}

bool StateRecoverer::FindMostRecent(StateSave& outSave, uint32_t& outPage, uint32_t& outOffset) {
	bool foundValid = false;
	StateSave highestSave = {0, 0, 0, 0};
	uint32_t highestPage = 0;
	uint32_t highestOff = 0;

	for (uint32_t p = 0; p < pageCount; p++) {
		for (uint32_t off = 0; off < savesPerPage; off++) {
			StateSave thisSave = ReadSave(p, off);

			if (thisSave.tick == 0xFFFFFFFF && thisSave.gen == 0xFFFFFFFF) {
				break; 
			}

			if (thisSave.checksum == GetChecksum(thisSave)) {
				if (!foundValid ||
					thisSave.gen > highestSave.gen ||
					(thisSave.gen == highestSave.gen && thisSave.tick > highestSave.tick)) {
					
                    highestSave = thisSave;
					highestPage = p;
					highestOff = off;
					foundValid = true;
				}
			}
		}
	}

	if (foundValid) {
		outSave = highestSave;
		outPage = highestPage;
		outOffset = highestOff;
		return true;
	}
	return false;
}

StateRecoverer::StateSave StateRecoverer::ReadSave(uint32_t pageIdx, uint32_t offsetIdx) {
	uint32_t byteAddress = startAddress + (pageIdx * STATE_RECO_PAGE_SIZE)
                         + (offsetIdx * sizeof(StateSave));

	return *reinterpret_cast<StateSave*>(byteAddress);
}
