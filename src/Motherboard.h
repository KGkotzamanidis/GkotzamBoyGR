/*
 *An Gameboy and GameboyColor emulation with project name GkotzamBoyGR
 *Copyright (C) <Mon Mar 17 2025>  <KGkotzamanidis>
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
#ifndef MOTHERBOARD_H
#define MOTHERBOARD_H

#include <cstdint>
#include <iostream>
#include <vector>

#include "Bios.h"
#include "Interrupts.h"
#include "Timers.h"

class Motherboard
{
public:
    Motherboard(Bios &bios, Timers &timers, Interrupts &Interrupts);

    /*Read/Write 8Bit Signals*/
    uint8_t readByte(uint16_t address);
    void writeByte(uint16_t address, uint8_t data);
    /*Read/Write 16Bit Signals*/
    uint16_t readWord(uint16_t address);
    void writeWord(uint16_t address, uint16_t data);

private:
    Bios *bios = nullptr;
    Timers *timers= nullptr;
    Interrupts *interrupts= nullptr;

    std::vector<uint8_t> HRAM = std::vector<uint8_t>(0x7E, 0);

private:
    uint8_t KEY_1 = 0, WRAMBank = 1;
};
#endif