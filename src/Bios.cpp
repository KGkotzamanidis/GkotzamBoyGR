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
#include "Bios.h"
Bios::Bios() {
    std::printf("Initiallize BIOS Sector\n");
}

void Bios::loadBios(std::string BiosPath) {
    std::ifstream ifs(BiosPath, std::ios::ate | std::ios::binary | std::ios::in);
    std::printf("Locate the BOOT Rom\n");
    if (ifs.is_open()) {
        std::printf("Reading...");
        ifs.seekg(0, std::ios::end);
        Biosize = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        BIOS.resize(Biosize);
        ifs.read(reinterpret_cast<char *>(BIOS.data()), Biosize);
        ifs.close();
        BootROM = true;
        std::printf("%i KiB OK\n", (int)Biosize);
    } else {
        std::printf("Error while Reading BOOT Rom\n");
        BootROM = false;
    }
}

uint8_t Bios::receivingData(uint16_t address) {
    return BIOS[address];
}

void Bios::sendingData(uint16_t address, uint8_t data) {
    BIOS[address] = data;
}