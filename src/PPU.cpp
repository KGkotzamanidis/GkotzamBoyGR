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
#include "PPU.h"
PPU::PPU(Interrupts &interrupts, bool CGBmode) : interrupts(&interrupts), CGBmode(CGBmode) {
    this->CGBmode = CGBmode;
}

uint8_t PPU::receivingData(uint16_t address) {
    uint8_t data;

    if (address >= 0x8000 && address <= 0x9FFF) {
        data = VRAM[address & 0x1FFF];
    } else if (address >= 0xC000 && address <= 0xFDFF) {
        data = OAM[address & 0xFF];
    } else if (address >= 0xFF40 && address <= 0xFF4B) {
        switch (address) {
        case LCDCaddress:
            data = LCDC;
            break;
        case STATaddress:
            data = STAT;
            break;
        case SCYaddress:
            data = SCY;
            break;
        case SCXaddress:
            data = SCX;
            break;
        case LYaddress:
            data = LY;
            break;
        case LYCaddress:
            data = LYC;
            break;
        case DMAaddress:
            break;
        case BGPaddress:
            data = BGP;
            break;
        case OBP0address:
            data = OBP0;
            break;
        case OBP1address:
            data = OBP1;
            break;
        case WYaddress:
            data = WY;
            break;
        case WXaddress:
            data = WX;
            break;
        }
    }
    return data;
}

void PPU::sendingData(uint16_t address, uint8_t data) {
    if (address >= 0x8000 && address <= 0x9FFF) {
        VRAM[address & 0x1FFF] = data;
    } else if (address >= 0xC000 && address <= 0xFDFF) {
        OAM[address & 0xFF] = data;
    } else if (address >= 0xFF40 && address <= 0xFF4B) {
        switch (address) {
        case LCDCaddress:
            LCDC = data;
            break;
        case STATaddress:
            STAT = data;
            break;
        case SCYaddress:
            SCY = data;
            break;
        case SCXaddress:
            SCX = data;
            break;
        case LYaddress:
            LY = data;
            break;
        case LYCaddress:
            LYC = data;
            break;
        case DMAaddress:
            break;
        case BGPaddress:
            BGP = data;
            break;
        case OBP0address:
            OBP0 = data;
            break;
        case OBP1address:
            OBP1 = data;
            break;
        case WYaddress:
            WY = data;
            break;
        case WXaddress:
            WX = data;
            break;
        }
    }
}

void PPU::reset() {
    LCDC = 0x91;
    STAT = SCY = SCX = LY = LYC = DMA = BGP = OBP0 = OBP1 = WY = WX = 0;
}

void PPU::syncPPU(int lastCycleCount) {
    PPUDotsCount += lastCycleCount;
    uint8_t PPUmode = STAT & 0x03;

    switch (PPUmode) {
    case 0: /*HBlank*/
        if (PPUDotsCount >= 204) {
            PPUDotsCount -= 204;
        }
        break;
    case 1: /*VBlank*/
        if (PPUDotsCount >= 456) {
            PPUDotsCount -= 456;
        }
        break;
    case 2: /*OAM Scan*/
        if (PPUDotsCount >= 80) {
            PPUDotsCount -= 80;
        }
        break;
    case 3: /*Drawing Pixels*/
        if (PPUDotsCount >= 289) {
            PPUDotsCount -= 289;
        }
        break;
    }
}