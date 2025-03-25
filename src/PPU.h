/*
 *An Gameboy and GameboyColor emulation with project name GkotzamBoyGR
 *Copyright (C) <Mon Mar 24 2025>  <KGkotzamanidis>
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef PPU_H
#define PPU_H

#include <vector>

#include "EmulationUtils.h"
#include "Interrupts.h"

class PPU {
public:
    PPU(Interrupts &interrupts, bool CGBmode);

    uint8_t receivingData(uint16_t address);
    void sendingData(uint16_t address, uint8_t data);

    void reset(void);

    void syncPPU(int lastCycleCount);

private:
    Interrupts *interrupts = nullptr;

    uint8_t LCDC, STAT, SCY, SCX, LY, LYC, DMA, BGP, OBP0, OBP1, WY, WX;

    /* 8KiB Video RAM */
    std::vector<uint8_t> VRAM = std::vector<uint8_t>(0x2000, 0);
    /* 160 bytes (60/4=40 object) OAM Buffer   */
    std::vector<uint8_t> OAM = std::vector<uint8_t>(0xA0, 0);

    int PPUDotsCount = 0;

    bool CGBmode;
};
#endif