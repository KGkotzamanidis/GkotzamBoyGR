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
#include "Motherboard.h"

Motherboard::Motherboard(Bios &bios, Timers &timers, Interrupts &Interrupts, bool CGBmode) : bios(&bios), timers(&timers), interrupts(&Interrupts), CGBmode(CGBmode) {
    std::printf("Initiallize MOTHERBOARD Sector\n");
    this->CGBmode = CGBmode;
}

/*Read/Write 8Bit Signals*/
uint8_t Motherboard::readByte(uint16_t address) {
    uint8_t data = 0xFF;
    /* 16 KiB ROM bank 00-NN*/
    if (address >= 0x0000 && address <= 0x7FFF) {
        if (bios->BootROM && address <= 0x100) {
            data = bios->receivingData(address);
        } else {
        }
    }
    /*8 KiB Video RAM (VRAM)*/
    else if (address >= 0x8000 && address <= 0x9FFF) {
    }
    /*8 KiB External RAM*/
    else if (address >= 0xA000 && address <= 0xBFFF) {
    }
    /*4+4 KiB Work RAM with ECHO (WRAM)*/
    else if (address >= 0xC000 && address <= 0xFDFF) {
    }
    /*Object attribute memory (OAM)*/
    else if (address >= 0xFE00 && address <= 0xFE9F) {
    }
    /*Not Usable*/
    else if (address >= 0xFEA0 && address <= 0xFEFF) {
    }
    /*I/O Registers*/
    else if (address >= 0xFF00 && address <= 0xFF7F) {
        /*Joypad input*/
        if (address == 0xFF00) {
        }
        /*Serial transfer*/
        else if (address >= 0xFF01 && address <= 0xFF02) {
        }
        /*Timer and divider*/
        else if (address >= 0xFF04 && address <= 0xFF07) {
            data = timers->receivingData(address);
        }
        /*Interrupts*/
        else if (address == 0xFF0F) {
            data = interrupts->IF;
        }
        /*Audio*/
        else if (address >= 0xFF10 && address <= 0xFF26) {
        }
        /*Wave pattern*/
        else if (address >= 0xFF30 && address <= 0xFF3F) {
        }
        /*LCD Control, Status, Position, Scrolling, and Palettes*/
        else if (address >= 0xFF40 && address <= 0xFF4B) {
        }
        /*KEY1 CGB*/
        else if (address == 0xFF4D) {
            data = KEY_1;
        }
        /*VRAM Bank Select CGB*/
        else if (address == 0xFF4F && CGBmode) {
        }
        /*Set to non-zero to disable boot ROM*/
        else if (address == 0xFF50) {
        }
        /*VRAM DMA*/
        else if ((address >= 0xFF51 && address <= 0xFF55) && CGBmode) {
        }
        /*BG / OBJ Palettes CGB*/
        else if ((address >= 0xFF68 && address <= 0xFF6B) && CGBmode) {
        }
        /*WRAM Bank Select CGB*/
        else if (address == 0xFF70 && CGBmode) {
        }
    }
    /*High RAM (HRAM)*/
    else if (address >= 0xFF80 && address <= 0xFFFE) {
        data = HRAM[address & 0x7F];
    }
    /*Interrupt Enable register (IE)*/
    else if (address == 0xFFFF) {
        data = interrupts->IE;
    }

    return data;
}
void Motherboard::writeByte(uint16_t address, uint8_t data) {
    /* 16 KiB ROM bank 00-NN*/
    if (address >= 0x0000 && address <= 0x7FFF) {
    }
    /*8 KiB Video RAM (VRAM)*/
    else if (address >= 0x8000 && address <= 0x9FFF) {
    }
    /*8 KiB External RAM*/
    else if (address >= 0xA000 && address <= 0xBFFF) {
    }
    /*4+4 KiB Work RAM with ECHO (WRAM)*/
    else if (address >= 0xC000 && address <= 0xFDFF) {
    }
    /*Object attribute memory (OAM)*/
    else if (address >= 0xFE00 && address <= 0xFE9F) {
    }
    /*Not Usable*/
    else if (address >= 0xFEA0 && address <= 0xFEFF) {
    }
    /*I/O Registers*/
    else if (address >= 0xFF00 && address <= 0xFF7F) {
        /*Joypad input*/
        if (address == 0xFF00) {
        }
        /*Serial transfer*/
        else if (address >= 0xFF01 && address <= 0xFF02) {
        }
        /*Timer and divider*/
        else if (address >= 0xFF04 && address <= 0xFF07) {
            timers->sendingData(address, data);
        }
        /*Interrupts*/
        else if (address == 0xFF0F) {
            interrupts->IF = data;
        }
        /*Audio*/
        else if (address >= 0xFF10 && address <= 0xFF26) {
        }
        /*Wave pattern*/
        else if (address >= 0xFF30 && address <= 0xFF3F) {
        }
        /*LCD Control, Status, Position, Scrolling, and Palettes*/
        else if (address >= 0xFF40 && address <= 0xFF4B) {
        } else if (address == 0xFF4D) {
            KEY_1 = data;
        }
        /*VRAM Bank Select CGB*/
        else if (address == 0xFF4F && CGBmode) {
        }
        /*Set to non-zero to disable boot ROM*/
        else if (address == 0xFF50) {
            if (data == 0x1) {
                bios->BootROM = false;
            }
        }
        /*VRAM DMA CGB*/
        else if ((address >= 0xFF51 && address <= 0xFF55) && CGBmode) {
        }
        /*BG / OBJ Palettes CGB*/
        else if ((address >= 0xFF68 && address <= 0xFF6B) && CGBmode) {
        }
        /*WRAM Bank Select CGB*/
        else if (address == 0xFF70 && CGBmode) {
        }
    }
    /*High RAM (HRAM)*/
    else if (address >= 0xFF80 && address <= 0xFFFE) {
        HRAM[address & 0x7F] = data;
    }
    /*Interrupt Enable register (IE)*/
    else if (address == 0xFFFF) {
        interrupts->IE = data;
    }
}
/*Read/Write 16Bit Signals*/
uint16_t Motherboard::readWord(uint16_t address) {
    uint16_t data = readByte(address);
    data |= readByte(address + 1) << 8;
    return data;
}
void Motherboard::writeWord(uint16_t address, uint16_t data) {
    writeByte(address, data & 0xFF);
    writeByte(address + 1, (data >> 8) & 0xFF);
}