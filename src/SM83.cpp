/*
 *An Gameboy and GameboyColor emulation with project name GkotzamBoyGR
 *Copyright (C) <Fri Mar 21 2025>  <KGkotzamanidis>
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
#include "SM83.h"
SM83::SM83(Motherboard &mem) : mem(&mem) {
    std::printf("Initiallize CPU Sector\n");
    EIDIFlag = false;
    IME = false;
    IMEhold = false;
    ishalt = false;
}
// clang-format off
const int SM83::instructionCount[] = {
    4,12,8,8,4,4,8,4,20,8,8,8,4,4,8,4,
    4,12,8,8,4,4,8,4,12,8,8,8,4,4,8,4,
    8,12,8,8,4,4,8,4,8,8,8,8,4,4,8,4,
    8,12,8,8,12,12,12,4,8,8,8,8,4,4,8,4,
    4,4,4,4,4,4,8,4,4,4,4,4,4,4,8,4,
    4,4,4,4,4,4,8,4,4,4,4,4,4,4,8,4,
    4,4,4,4,4,4,8,4,4,4,4,4,4,4,8,4,
    8,8,8,8,8,8,4,8,4,4,4,4,4,4,8,4,
    4,4,4,4,4,4,8,4,4,4,4,4,4,4,8,4,
    4,4,4,4,4,4,8,4,4,4,4,4,4,4,8,4,
    4,4,4,4,4,4,8,4,4,4,4,4,4,4,8,4,
    4,4,4,4,4,4,8,4,4,4,4,4,4,4,8,4,
    8,12,12,16,12,16,8,16,8,16,12,4,12,24,8,16,
    8,12,12,0,12,16,8,16,8,16,12,0,12,0,8,16,
    12,12,8,0,0,16,8,16,16,4,16,0,0,0,8,16,
    12,12,8,4,0,16,8,16,12,8,16,4,0,0,8,16,
};

const int SM83::prefixedinstructionCount[] = {
    8,8,8,8,8,8,16,8,8,8,8,8,8,8,16,8,
    8,8,8,8,8,8,16,8,8,8,8,8,8,8,16,8,
    8,8,8,8,8,8,16,8,8,8,8,8,8,8,16,8,
    8,8,8,8,8,8,16,8,8,8,8,8,8,8,16,8,
    8,8,8,8,8,8,12,8,8,8,8,8,8,8,12,8,
    8,8,8,8,8,8,12,8,8,8,8,8,8,8,12,8,
    8,8,8,8,8,8,12,8,8,8,8,8,8,8,12,8,
    8,8,8,8,8,8,12,8,8,8,8,8,8,8,12,8,
    8,8,8,8,8,8,16,8,8,8,8,8,8,8,16,8,
    8,8,8,8,8,8,16,8,8,8,8,8,8,8,16,8,
    8,8,8,8,8,8,16,8,8,8,8,8,8,8,16,8,
    8,8,8,8,8,8,16,8,8,8,8,8,8,8,16,8,
    8,8,8,8,8,8,16,8,8,8,8,8,8,8,16,8,
    8,8,8,8,8,8,16,8,8,8,8,8,8,8,16,8,
    8,8,8,8,8,8,16,8,8,8,8,8,8,8,16,8,
    8,8,8,8,8,8,16,8,8,8,8,8,8,8,16,8,
};
// clang-format on
void SM83::reset(void) {
    EIDIFlag = false;
    IME = false;
    IMEhold = false;
    ishalt = false;

    PC = 0X0000;
    SP = 0x0000;
    A = B = C = D = E = H = L = F = 0x00;
}

void SM83::step(void) {
    IME = EIDIFlag ? IME : IMEhold;
    EIDIFlag = false;

    uint8_t iflags = mem->readByte(IFaddress);
    uint8_t ieflags = mem->readByte(IEaddress);
    uint8_t activeInterrupts = iflags & ieflags;

    if (activeInterrupts != 0) {
        ishalt = false;

        if (IME) {
            IMEhold = false;
            IME = false;

            if (activeInterrupts & VBlankInterrupt) {
                mem->writeByte(IFaddress, iflags & ~VBlankInterrupt);
                rst(0x40);
            } else if (activeInterrupts & LCDCInterrupt) {
                mem->writeByte(IFaddress, iflags & ~LCDCInterrupt);
                rst(0x48);
            } else if (activeInterrupts & timerOverflow) {
                mem->writeByte(IFaddress, iflags & ~timerOverflow);
                rst(0x50);
            } else if (activeInterrupts & buttonOverflow) {
                mem->writeByte(IFaddress, iflags & ~buttonOverflow);
                rst(0x60);
            }
        }
    }

    if (!ishalt) {
        executeInstruction(mem->readByte(PC));
    } else {
        lastCycleCount = 4;
    }
}

bool SM83::getDoubleSpeed() {
    return DoubleSpeed;
}

int SM83::getlastCycleCount() {
    return lastCycleCount;
}

/* For more information about the SM83 Instruction read this
   https://gbdev.io/gb-opcodes/optables/
*/
void SM83::executeInstruction(uint8_t opcode) {
    PC++;
    lastCycleCount = instructionCount[opcode];

    switch (opcode) {
    case 0x00:
        nop();
        break;
    case 0x01:
        setBC(mem->readWord(PC));
        PC += 2;
        break;
    case 0x02:
        mem->writeByte(BC(), A);
        break;
    case 0x03:
        setBC(BC() + 1);
        break;
    case 0x04:
        B = alu8bitINC(B);
        break;
    case 0x05:
        B = alu8bitDEC(B);
        break;
    case 0x06:
        B = mem->readByte(PC);
        PC++;
        break;
    case 0x07: {
        /*
         *A = RLC(A);
         */
        bool carry = (A & 0x80) != 0;
        A = (A << 1) | (uint8_t)carry;
        setFlag(Flag_Z, false);
        setFlag(Flag_N, false);
        setFlag(Flag_H, false);
        setFlag(Flag_C, carry);
        break;
    }
    case 0x08:
        mem->writeWord(mem->readWord(PC), SP);
        PC += 2;
        break;
    case 0x09:
        alu16bitADD(BC());
        break;
    case 0x0A:
        A = mem->readByte(BC());
        break;
    case 0x0B:
        setBC(BC() - 1);
        break;
    case 0x0C:
        C = alu8bitINC(C);
        break;
    case 0x0D:
        C = alu8bitDEC(C);
        break;
    case 0x0E:
        C = mem->readByte(PC);
        PC++;
        break;
    case 0x0F: {
        /*
         *A = RRC(A);
         */
        bool carry = (A & 0x1) != 0;
        A = (A >> 1) | (carry << 7);
        setFlag(Flag_Z, false);
        setFlag(Flag_N, false);
        setFlag(Flag_H, false);
        setFlag(Flag_C, carry);
        break;
    }
    case 0x10:
        stop();
        PC++;
        break;
    case 0x11:
        setDE(mem->readWord(PC));
        PC += 2;
        break;
    case 0x12:
        mem->writeByte(DE(), A);
        break;
    case 0x13:
        setDE(DE() + 1);
        break;
    case 0x14:
        D = alu8bitINC(D);
        break;
    case 0x15:
        D = alu8bitDEC(D);
        break;
    case 0x16:
        D = mem->readByte(PC);
        PC++;
        break;
    case 0x17: {
        /*
         *A = RL(A);
         */
        bool carryFlagBit = getFlagC();
        bool carry = (A & 0x80) != 0;
        A = (A << 1) | (uint8_t)carryFlagBit;
        setFlag(Flag_Z, false);
        setFlag(Flag_N, false);
        setFlag(Flag_H, false);
        setFlag(Flag_C, carry);
        break;
    }
    case 0x18:
        jr(true);
        break;
    case 0x19:
        alu16bitADD(DE());
        break;
    case 0x1A:
        A = mem->readByte(DE());
        break;
    case 0x1B:
        setDE(DE() - 1);
        break;
    case 0x1C:
        E = alu8bitINC(E);
        break;
    case 0x1D:
        E = alu8bitDEC(E);
        break;
    case 0x1E:
        E = mem->readByte(PC);
        PC++;
        break;
    case 0x1F: {
        /*
         * A = RR(A);
         */
        bool carryFlagBit = getFlagC();
        bool carry = (A & 0x1) != 0;
        A = (A >> 1) | (carryFlagBit << 7);
        setFlag(Flag_Z, false);
        setFlag(Flag_N, false);
        setFlag(Flag_H, false);
        setFlag(Flag_C, carry);
        break;
    }
    case 0x20:
        jr(!getFlagZ());
        break;
    case 0x21:
        setHL(mem->readWord(PC));
        PC += 2;
        break;
    case 0x22:
        mem->writeByte(HL(), A);
        setHL(HL() + 1);
        break;
    case 0x23:
        setHL(HL() + 1);
        break;
    case 0x24:
        H = alu8bitINC(H);
        break;
    case 0x25:
        H = alu8bitDEC(H);
        break;
    case 0x26:
        H = mem->readByte(PC);
        PC++;
        break;
    case 0x27:
        logic8bitDAA();
        break;
    case 0x28:
        jr(getFlagZ());
        break;
    case 0x29:
        alu16bitADD(HL());
        break;
    case 0x2A:
        A = mem->readByte(HL());
        setHL(HL() + 1);
        break;
    case 0x2B:
        setHL(HL() - 1);
        break;
    case 0x2C:
        L = alu8bitINC(L);
        break;
    case 0x2D:
        L = alu8bitDEC(L);
        break;
    case 0x2E:
        L = mem->readByte(PC);
        PC++;
        break;
    case 0x2F:
        logic8bitCPL();
        break;
    case 0x30:
        jr(!getFlagC());
        break;
    case 0x31:
        SP = mem->readWord(PC);
        PC += 2;
        break;
    case 0x32:
        mem->writeByte(HL(), A);
        setHL(HL() - 1);
        break;
    case 0x33:
        SP++;
        break;
    case 0x34:
        mem->writeByte(HL(), alu8bitINC(mem->readByte(HL())));
        break;
    case 0x35:
        mem->writeByte(HL(), alu8bitDEC(mem->readByte(HL())));
        break;
    case 0x36:
        mem->writeWord(HL(), mem->readByte(PC));
        PC++;
        break;
    case 0x37:
        scf();
        break;
    case 0x38:
        jr(getFlagC());
        break;
    case 0x39:
        alu16bitADD(SP);
        break;
    case 0x3A:
        A = mem->readByte(HL());
        setHL(HL() - 1);
        break;
    case 0x3B:
        SP--;
        break;
    case 0x3C:
        A = alu8bitINC(A);
        break;
    case 0x3D:
        A = alu8bitDEC(A);
        break;
    case 0x3E:
        A = mem->readByte(PC);
        PC++;
        break;
    case 0x3F:
        ccf();
        break;
    case 0x40:
        B = B;
        break;
    case 0x41:
        B = C;
        break;
    case 0x42:
        B = D;
        break;
    case 0x43:
        B = E;
        break;
    case 0x44:
        B = H;
        break;
    case 0x45:
        B = L;
        break;
    case 0x46:
        B = mem->readByte(HL());
        break;
    case 0x47:
        B = A;
        break;
    case 0x48:
        C = B;
        break;
    case 0x49:
        C = C;
        break;
    case 0x4A:
        C = D;
        break;
    case 0x4B:
        C = E;
        break;
    case 0x4C:
        C = H;
        break;
    case 0x4D:
        C = L;
        break;
    case 0x4E:
        C = mem->readByte(HL());
        break;
    case 0x4F:
        C = A;
        break;
    case 0x50:
        D = B;
        break;
    case 0x51:
        D = C;
        break;
    case 0x52:
        D = D;
        break;
    case 0x53:
        D = E;
        break;
    case 0x54:
        D = H;
        break;
    case 0x55:
        D = L;
        break;
    case 0x56:
        D = mem->readByte(HL());
        break;
    case 0x57:
        D = A;
        break;
    case 0x58:
        E = B;
        break;
    case 0x59:
        E = C;
        break;
    case 0x5A:
        E = D;
        break;
    case 0x5B:
        E = E;
        break;
    case 0x5C:
        E = H;
        break;
    case 0x5D:
        E = L;
        break;
    case 0x5E:
        E = mem->readByte(HL());
        break;
    case 0x5F:
        E = A;
        break;
    case 0x60:
        H = B;
        break;
    case 0x61:
        H = C;
        break;
    case 0x62:
        H = D;
        break;
    case 0x63:
        H = E;
        break;
    case 0x64:
        H = H;
        break;
    case 0x65:
        H = L;
        break;
    case 0x66:
        H = mem->readByte(HL());
        break;
    case 0x67:
        H = A;
        break;
    case 0x68:
        L = B;
        break;
    case 0x69:
        L = C;
        break;
    case 0x6A:
        L = D;
        break;
    case 0x6B:
        L = E;
        break;
    case 0x6C:
        L = H;
        break;
    case 0x6D:
        L = L;
        break;
    case 0x6E:
        L = mem->readByte(HL());
        break;
    case 0x6F:
        L = A;
        break;
    case 0x70:
        mem->writeByte(HL(), B);
        break;
    case 0x71:
        mem->writeByte(HL(), C);
        break;
    case 0x72:
        mem->writeByte(HL(), D);
        break;
    case 0x73:
        mem->writeByte(HL(), E);
        break;
    case 0x74:
        mem->writeByte(HL(), H);
        break;
    case 0x75:
        mem->writeByte(HL(), L);
        break;
    case 0x76:
        halt();
        break;
    case 0x77:
        mem->writeByte(HL(), A);
        break;
    case 0x78:
        A = B;
        break;
    case 0x79:
        A = C;
        break;
    case 0x7A:
        A = D;
        break;
    case 0x7B:
        A = E;
        break;
    case 0x7C:
        A = H;
        break;
    case 0x7D:
        A = L;
        break;
    case 0x7E:
        A = mem->readByte(HL());
        break;
    case 0x7F:
        A = A;
        break;
    case 0x80:
        alu8bitADD(B);
        break;
    case 0x81:
        alu8bitADD(C);
        break;
    case 0x82:
        alu8bitADD(D);
        break;
    case 0x83:
        alu8bitADD(E);
        break;
    case 0x84:
        alu8bitADD(H);
        break;
    case 0x85:
        alu8bitADD(L);
        break;
    case 0x86:
        alu8bitADD(mem->readByte(HL()));
        break;
    case 0x87:
        alu8bitADD(A);
        break;
    case 0x88:
        alu8bitADC(B, getFlagC());
        break;
    case 0x89:
        alu8bitADC(C, getFlagC());
        break;
    case 0x8A:
        alu8bitADC(D, getFlagC());
        break;
    case 0x8B:
        alu8bitADC(E, getFlagC());
        break;
    case 0x8C:
        alu8bitADC(H, getFlagC());
        break;
    case 0x8D:
        alu8bitADC(L, getFlagC());
        break;
    case 0x8E:
        alu8bitADC(mem->readByte(HL()), getFlagC());
        break;
    case 0x8F:
        alu8bitADC(A, getFlagC());
        break;
    case 0x90:
        alu8bitSUB(B);
        break;
    case 0x91:
        alu8bitSUB(C);
        break;
    case 0x92:
        alu8bitSUB(D);
        break;
    case 0x93:
        alu8bitSUB(E);
        break;
    case 0x94:
        alu8bitSUB(H);
        break;
    case 0x95:
        alu8bitSUB(L);
        break;
    case 0x96:
        alu8bitSUB(mem->readByte(HL()));
        break;
    case 0x97:
        alu8bitSUB(A);
        break;
    case 0x98:
        alu8bitSBC(B, getFlagC());
        break;
    case 0x99:
        alu8bitSBC(C, getFlagC());
        break;
    case 0x9A:
        alu8bitSBC(D, getFlagC());
        break;
    case 0x9B:
        alu8bitSBC(E, getFlagC());
        break;
    case 0x9C:
        alu8bitSBC(H, getFlagC());
        break;
    case 0x9D:
        alu8bitSBC(L, getFlagC());
        break;
    case 0x9E:
        alu8bitSBC(mem->readByte(HL()), getFlagC());
        break;
    case 0x9F:
        alu8bitSBC(A, getFlagC());
        break;
    case 0xA0:
        logic8bitAND(B);
        break;
    case 0xA1:
        logic8bitAND(C);
        break;
    case 0xA2:
        logic8bitAND(D);
        break;
    case 0xA3:
        logic8bitAND(E);
        break;
    case 0xA4:
        logic8bitAND(H);
        break;
    case 0xA5:
        logic8bitAND(L);
        break;
    case 0xA6:
        logic8bitAND(mem->readByte(HL()));
        break;
    case 0xA7:
        logic8bitAND(A);
        break;
    case 0xA8:
        logic8bitXOR(B);
        break;
    case 0xA9:
        logic8bitXOR(C);
        break;
    case 0xAA:
        logic8bitXOR(D);
        break;
    case 0xAB:
        logic8bitXOR(E);
        break;
    case 0xAC:
        logic8bitXOR(H);
        break;
    case 0xAD:
        logic8bitXOR(L);
        break;
    case 0xAE:
        logic8bitXOR(mem->readByte(HL()));
        break;
    case 0xAF:
        logic8bitXOR(A);
        break;
    case 0xB0:
        logic8bitOR(B);
        break;
    case 0xB1:
        logic8bitOR(C);
        break;
    case 0xB2:
        logic8bitOR(D);
        break;
    case 0xB3:
        logic8bitOR(E);
        break;
    case 0xB4:
        logic8bitOR(H);
        break;
    case 0xB5:
        logic8bitOR(L);
        break;
    case 0xB6:
        logic8bitOR(mem->readByte(HL()));
        break;
    case 0xB7:
        logic8bitOR(A);
        break;
    case 0xB8:
        logic8bitCP(B);
        break;
    case 0xB9:
        logic8bitCP(C);
        break;
    case 0xBA:
        logic8bitCP(D);
        break;
    case 0xBB:
        logic8bitCP(E);
        break;
    case 0xBC:
        logic8bitCP(H);
        break;
    case 0xBD:
        logic8bitCP(L);
        break;
    case 0xBE:
        logic8bitCP(mem->readByte(HL()));
        break;
    case 0xBF:
        logic8bitCP(A);
        break;
    case 0xC0:
        ret(!getFlagZ());
        break;
    case 0xC1:
        setBC(mem->readWord(SP));
        SP += 2;
        break;
    case 0xC2:
        jp(!getFlagZ());
        break;
    case 0xC3:
        jp(true);
        break;
    case 0xC4:
        call(!getFlagZ());
        break;
    case 0xC5:
        SP -= 2;
        mem->writeWord(SP, BC());
        break;
    case 0xC6:
        alu8bitADD(mem->readByte(PC));
        PC++;
        break;
    case 0xC7:
        rst(0x00);
        break;
    case 0xC8:
        ret(getFlagZ());
        break;
    case 0xC9:
        ret(true);
        break;
    case 0xCA:
        jp(getFlagZ());
        break;
    case 0xCB:
        executePrefixedInstruction(mem->readByte(PC));
        break;
    case 0xCC:
        call(getFlagZ());
        break;
    case 0xCD:
        call(true);
        break;
    case 0xCE:
        alu8bitADC(mem->readByte(PC), getFlagC());
        PC++;
        break;
    case 0xCF:
        rst(0x08);
        break;
    case 0xD0:
        ret(!getFlagC());
        break;
    case 0xD1:
        setDE(mem->readWord(SP));
        SP += 2;
        break;
    case 0xD2:
        jp(!getFlagC());
        break;
    case 0xD3:
        /* Empty Instruction */
        std::cout << "Empty Instruction" << std::endl;
        break;
    case 0xD4:
        call(!getFlagC());
        break;
    case 0xD5:
        SP -= 2;
        mem->writeWord(SP, DE());
        break;
    case 0xD6:
        alu8bitSUB(mem->readByte(PC));
        PC++;
        break;
    case 0xD7:
        rst(0x10);
        break;
    case 0xD8:
        ret(getFlagC());
        break;
    case 0xD9:
        reti();
        break;
    case 0xDA:
        jp(getFlagC());
        break;
    case 0xDB:
        /* Empty Instruction */
        std::cout << "Empty Instruction" << std::endl;
        break;
    case 0xDC:
        call(getFlagC());
        break;
    case 0xDD:
        /* Empty Instruction */
        std::cout << "Empty Instruction" << std::endl;
        break;
    case 0xDE:
        alu8bitSBC(mem->readByte(PC), getFlagC());
        PC++;
        break;
    case 0xDF:
        rst(0x18);
        break;
    case 0xE0:
        mem->writeByte(0xFF00 + mem->readByte(PC), A);
        PC++;
        break;
    case 0xE1:
        setHL(mem->readWord(SP));
        SP += 2;
        break;
    case 0xE2:
        mem->writeByte(0xFF00 + C, A);
        break;
    case 0xE3:
        /* Empty Instruction */
        std::cout << "Empty Instruction" << std::endl;
        break;
    case 0xE4:
        /* Empty Instruction */
        std::cout << "Empty Instruction" << std::endl;
        break;
    case 0xE5:
        SP -= 2;
        mem->writeWord(SP, HL());
        break;
    case 0xE6:
        logic8bitAND(mem->readByte(PC));
        PC++;
        break;
    case 0xE7:
        rst(0x20);
        break;
    case 0xE8:
        alu16bitSPADD();
        break;
    case 0xE9:
        PC = HL();
        PC += 2;
        break;
    case 0xEA:
        mem->writeByte(mem->readWord(PC), A);
        PC += 2;
        break;
    case 0xEB:
        /* Empty Instruction */
        std::cout << "Empty Instruction" << std::endl;
        break;
    case 0xEC:
        /* Empty Instruction */
        std::cout << "Empty Instruction" << std::endl;
        break;
    case 0xED:
        /* Empty Instruction */
        std::cout << "Empty Instruction" << std::endl;
        break;
    case 0xEE:
        logic8bitXOR(mem->readByte(PC));
        PC++;
        break;
    case 0xEF:
        rst(0x28);
        break;
    case 0xF0:
        A = mem->readByte(0xFF00 + mem->readByte(PC));
        PC++;
        break;
    case 0xF1:
        setAF(mem->readWord(SP));
        SP += 2;
        break;
    case 0xF2:
        A = mem->readByte(0xFF00 + C);
        break;
    case 0xF3:
        di();
        break;
    case 0xF4:
        /* Empty Instruction */
        std::cout << "Empty Instruction" << std::endl;
    case 0xF5:
        SP -= 2;
        mem->writeWord(SP, AF());
        break;
    case 0xF6:
        logic8bitOR(mem->readByte(PC));
        PC++;
        break;
    case 0xF7:
        rst(0x30);
        break;
    case 0xF8: {
        int8_t data = mem->readByte(PC);
        uint16_t result = SP + data;
        setFlag(Flag_Z, false);
        setFlag(Flag_N, false);
        setFlag(Flag_H, ((SP & 0x0F) + (data & 0x0F)) > 0x0F);
        setFlag(Flag_C, result > 0xFFFF);
        setHL(result);
        PC++;
        break;
    }
    case 0xF9:
        SP = HL();
        break;
    case 0xFA:
        A = mem->readByte(mem->readWord(PC));
        PC += 2;
        break;
    case 0xFB:
        ei();
        break;
    case 0xFC:
        /* Empty Instruction */
        std::printf("Empty Instruction");
        break;
    case 0xFD:
        /* Empty Instruction */
        std::printf("Empty Instruction");
        break;
    case 0xFE:
        logic8bitCP(mem->readByte(PC));
        PC++;
        break;
    case 0xFF:
        rst(0x38);
        break;
    }
}

void SM83::executePrefixedInstruction(uint8_t opcode) {
    PC++;
    lastCycleCount = prefixedinstructionCount[opcode];
    switch (opcode) {
    case 0x0:
        B = RLC(B);
        break;
    case 0x1:
        C = RLC(C);
        break;
    case 0x2:
        D = RLC(D);
        break;
    case 0x3:
        E = RLC(E);
        break;
    case 0x4:
        H = RLC(H);
        break;
    case 0x5:
        L = RLC(L);
        break;
    case 0x6:
        mem->writeByte(HL(), RLC(mem->readByte(HL())));
        break;
    case 0x7:
        A = RLC(A);
        break;
    case 0x8:
        B = RRC(B);
        break;
    case 0x9:
        C = RRC(C);
        break;
    case 0xA:
        D = RRC(D);
        break;
    case 0xB:
        E = RRC(E);
        break;
    case 0xC:
        H = RRC(H);
        break;
    case 0xD:
        L = RRC(L);
        break;
    case 0xE:
        mem->writeByte(HL(), RRC(mem->readByte(HL())));
        break;
    case 0xF:
        A = RRC(A);
        break;
    case 0x10:
        B = RL(B);
        break;
    case 0x11:
        C = RL(C);
        break;
    case 0x12:
        D = RL(D);
        break;
    case 0x13:
        E = RL(E);
        break;
    case 0x14:
        H = RL(H);
        break;
    case 0x15:
        L = RL(L);
        break;
    case 0x16:
        mem->writeByte(HL(), RL(mem->readByte(HL())));
        break;
    case 0x17:
        A = RL(A);
        break;
    case 0x18:
        B = RR(B);
        break;
    case 0x19:
        C = RR(C);
        break;
    case 0x1A:
        D = RR(D);
        break;
    case 0x1B:
        E = RR(E);
        break;
    case 0x1C:
        H = RR(H);
        break;
    case 0x1D:
        L = RR(L);
        break;
    case 0x1E:
        mem->writeByte(HL(), RR(mem->readByte(HL())));
        break;
    case 0x1F:
        A = RR(A);
        break;
    case 0x20:
        B = SLA(B);
        break;
    case 0x21:
        C = SLA(C);
        break;
    case 0x22:
        D = SLA(D);
        break;
    case 0x23:
        E = SLA(E);
        break;
    case 0x24:
        H = SLA(H);
        break;
    case 0x25:
        L = SLA(L);
        break;
    case 0x26:
        mem->writeByte(HL(), SLA(mem->readByte(HL())));
        break;
    case 0x27:
        A = SLA(A);
        break;
    case 0x28:
        B = SRA(B);
        break;
    case 0x29:
        C = SRA(C);
        break;
    case 0x2A:
        D = SRA(D);
        break;
    case 0x2B:
        E = SRA(E);
        break;
    case 0x2C:
        H = SRA(H);
        break;
    case 0x2D:
        L = SRA(L);
        break;
    case 0x2E:
        mem->writeByte(HL(), SRA(mem->readByte(HL())));
        break;
    case 0x2F:
        A = SRA(A);
        break;
    case 0x30:
        B = SWAP(B);
        break;
    case 0x31:
        C = SWAP(C);
        break;
    case 0x32:
        D = SWAP(D);
        break;
    case 0x33:
        E = SWAP(E);
        break;
    case 0x34:
        H = SWAP(H);
        break;
    case 0x35:
        L = SWAP(L);
        break;
    case 0x36:
        mem->writeByte(HL(), SWAP(mem->readByte(HL())));
        break;
    case 0x37:
        A = SWAP(A);
        break;
    case 0x38:
        B = SRL(B);
        break;
    case 0x39:
        C = SRL(C);
        break;
    case 0x3A:
        D = SRL(D);
        break;
    case 0x3B:
        E = SRL(E);
        break;
    case 0x3C:
        H = SRL(H);
        break;
    case 0x3D:
        L = SRL(L);
        break;
    case 0x3E:
        mem->writeByte(HL(), mem->readByte(HL()));
        break;
    case 0x3F:
        A = SRL(A);
        break;
    case 0x40:
        BIT(0, B);
        break;
    case 0x41:
        BIT(0, C);
        break;
    case 0x42:
        BIT(0, D);
        break;
    case 0x43:
        BIT(0, E);
        break;
    case 0x44:
        BIT(0, H);
        break;
    case 0x45:
        BIT(0, L);
        break;
    case 0x46:
        BIT(0, mem->readByte(HL()));
        break;
    case 0x47:
        BIT(0, A);
        break;
    case 0x48:
        BIT(1, B);
        break;
    case 0x49:
        BIT(1, C);
        break;
    case 0x4A:
        BIT(1, D);
        break;
    case 0x4B:
        BIT(1, E);
        break;
    case 0x4C:
        BIT(1, H);
        break;
    case 0x4D:
        BIT(1, L);
        break;
    case 0x4E:
        BIT(1, mem->readByte(HL()));
        break;
    case 0x4F:
        BIT(1, A);
        break;
    case 0x50:
        BIT(2, B);
        break;
    case 0x51:
        BIT(2, C);
        break;
    case 0x52:
        BIT(2, D);
        break;
    case 0x53:
        BIT(2, E);
        break;
    case 0x54:
        BIT(2, H);
        break;
    case 0x55:
        BIT(2, L);
        break;
    case 0x56:
        BIT(2, mem->readByte(HL()));
        break;
    case 0x57:
        BIT(2, A);
        break;
    case 0x58:
        BIT(3, B);
        break;
    case 0x59:
        BIT(3, C);
        break;
    case 0x5A:
        BIT(3, D);
        break;
    case 0x5B:
        BIT(3, E);
        break;
    case 0x5C:
        BIT(3, H);
        break;
    case 0x5D:
        BIT(3, L);
        break;
    case 0x5E:
        BIT(3, mem->readByte(HL()));
        break;
    case 0x5F:
        BIT(3, A);
        break;
    case 0x60:
        BIT(4, B);
        break;
    case 0x61:
        BIT(4, C);
        break;
    case 0x62:
        BIT(4, D);
        break;
    case 0x63:
        BIT(4, E);
        break;
    case 0x64:
        BIT(4, H);
        break;
    case 0x65:
        BIT(4, L);
        break;
    case 0x66:
        BIT(4, mem->readByte(HL()));
        break;
    case 0x67:
        BIT(4, A);
        break;
    case 0x68:
        BIT(5, B);
        break;
    case 0x69:
        BIT(5, C);
        break;
    case 0x6A:
        BIT(5, D);
        break;
    case 0x6B:
        BIT(5, E);
        break;
    case 0x6C:
        BIT(5, H);
        break;
    case 0x6D:
        BIT(5, L);
        break;
    case 0x6E:
        BIT(5, mem->readByte(HL()));
        break;
    case 0x6F:
        BIT(5, A);
        break;
    case 0x70:
        BIT(6, B);
        break;
    case 0x71:
        BIT(6, C);
        break;
    case 0x72:
        BIT(6, D);
        break;
    case 0x73:
        BIT(6, E);
        break;
    case 0x74:
        BIT(6, H);
        break;
    case 0x75:
        BIT(6, L);
        break;
    case 0x76:
        BIT(6, mem->readByte(HL()));
        break;
    case 0x77:
        BIT(6, A);
        break;
    case 0x78:
        BIT(7, B);
        break;
    case 0x79:
        BIT(7, C);
        break;
    case 0x7A:
        BIT(7, D);
        break;
    case 0x7B:
        BIT(7, E);
        break;
    case 0x7C:
        BIT(7, H);
        break;
    case 0x7D:
        BIT(7, L);
        break;
    case 0x7E:
        BIT(7, mem->readByte(HL()));
        break;
    case 0x7F:
        BIT(7, A);
        break;
    case 0x80:
        B = RES(0, B);
        break;
    case 0x81:
        C = RES(0, C);
        break;
    case 0x82:
        D = RES(0, D);
        break;
    case 0x83:
        E = RES(0, E);
        break;
    case 0x84:
        H = RES(0, H);
        break;
    case 0x85:
        L = RES(0, L);
        break;
    case 0x86:
        mem->writeByte(HL(), RES(mem->readByte(HL()), 0));
        break;
    case 0x87:
        A = RES(0, A);
        break;
    case 0x88:
        B = RES(1, B);
        break;
    case 0x89:
        C = RES(1, C);
        break;
    case 0x8A:
        D = RES(1, D);
        break;
    case 0x8B:
        E = RES(1, E);
        break;
    case 0x8C:
        H = RES(1, H);
        break;
    case 0x8D:
        L = RES(1, L);
        break;
    case 0x8E:
        mem->writeByte(HL(), RES(mem->readByte(HL()), 1));
        break;
    case 0x8F:
        A = RES(1, A);
        break;
    case 0x90:
        B = RES(2, B);
        break;
    case 0x91:
        C = RES(2, C);
        break;
    case 0x92:
        D = RES(2, D);
        break;
    case 0x93:
        E = RES(2, E);
        break;
    case 0x94:
        H = RES(2, H);
        break;
    case 0x95:
        L = RES(2, L);
        break;
    case 0x96:
        mem->writeByte(HL(), RES(mem->readByte(HL()), 2));
        break;
    case 0x97:
        A = RES(2, A);
        break;
    case 0x98:
        B = RES(3, B);
        break;
    case 0x99:
        C = RES(3, C);
        break;
    case 0x9A:
        D = RES(3, D);
        break;
    case 0x9B:
        E = RES(3, E);
        break;
    case 0x9C:
        H = RES(3, H);
        break;
    case 0x9D:
        L = RES(3, L);
        break;
    case 0x9E:
        mem->writeByte(HL(), RES(mem->readByte(HL()), 3));
        break;
    case 0x9F:
        A = RES(3, A);
        break;
    case 0xA0:
        B = RES(4, B);
        break;
    case 0xA1:
        C = RES(4, C);
        break;
    case 0xA2:
        D = RES(4, D);
        break;
    case 0xA3:
        E = RES(4, E);
        break;
    case 0xA4:
        H = RES(4, H);
        break;
    case 0xA5:
        L = RES(4, L);
        break;
    case 0xA6:
        mem->writeByte(HL(), RES(mem->readByte(HL()), 4));
        break;
    case 0xA7:
        A = RES(4, A);
        break;
    case 0xA8:
        B = RES(5, B);
        break;
    case 0xA9:
        C = RES(5, C);
        break;
    case 0xAA:
        D = RES(5, D);
        break;
    case 0xAB:
        E = RES(5, E);
        break;
    case 0xAC:
        H = RES(5, H);
        break;
    case 0xAD:
        L = RES(5, L);
        break;
    case 0xAE:
        mem->writeByte(HL(), RES(mem->readByte(HL()), 5));
        break;
    case 0xAF:
        A = RES(5, A);
        break;
    case 0xB0:
        B = RES(6, B);
        break;
    case 0xB1:
        C = RES(6, C);
        break;
    case 0xB2:
        D = RES(6, D);
        break;
    case 0xB3:
        E = RES(6, E);
        break;
    case 0xB4:
        H = RES(6, H);
        break;
    case 0xB5:
        L = RES(6, L);
        break;
    case 0xB6:
        mem->writeByte(HL(), RES(mem->readByte(HL()), 6));
        break;
    case 0xB7:
        A = RES(6, A);
        break;
    case 0xB8:
        B = RES(7, B);
        break;
    case 0xB9:
        C = RES(7, C);
        break;
    case 0xBA:
        D = RES(7, D);
        break;
    case 0xBB:
        E = RES(7, E);
        break;
    case 0xBC:
        H = RES(7, H);
        break;
    case 0xBD:
        L = RES(7, L);
        break;
    case 0xBE:
        mem->writeByte(HL(), RES(mem->readByte(HL()), 7));
        break;
    case 0xBF:
        A = RES(7, A);
        break;
    case 0xC0:
        B = SET(0, B);
        break;
    case 0xC1:
        C = SET(0, C);
        break;
    case 0xC2:
        D = SET(0, D);
        break;
    case 0xC3:
        E = SET(0, E);
        break;
    case 0xC4:
        H = SET(0, H);
        break;
    case 0xC5:
        L = SET(0, L);
        break;
    case 0xC6:
        mem->writeByte(HL(), SET(mem->readByte(HL()), 0));
        break;
    case 0xC7:
        A = SET(0, A);
        break;
    case 0xC8:
        B = SET(1, B);
        break;
    case 0xC9:
        C = SET(1, C);
        break;
    case 0xCA:
        D = SET(1, D);
        break;
    case 0xCB:
        E = SET(1, E);
        break;
    case 0xCC:
        H = SET(1, H);
        break;
    case 0xCD:
        L = SET(1, L);
        break;
    case 0xCE:
        mem->writeByte(HL(), SET(mem->readByte(HL()), 1));
        break;
    case 0xCF:
        A = SET(1, A);
        break;
    case 0xD0:
        B = SET(2, B);
        break;
    case 0xD1:
        C = SET(2, C);
        break;
    case 0xD2:
        D = SET(2, D);
        break;
    case 0xD3:
        E = SET(2, E);
        break;
    case 0xD4:
        H = SET(2, H);
        break;
    case 0xD5:
        L = SET(2, L);
        break;
    case 0xD6:
        mem->writeByte(HL(), SET(mem->readByte(HL()), 2));
        break;
    case 0xD7:
        A = SET(2, A);
        break;
    case 0xD8:
        B = SET(3, B);
        break;
    case 0xD9:
        C = SET(3, C);
        break;
    case 0xDA:
        D = SET(3, D);
        break;
    case 0xDB:
        E = SET(3, E);
        break;
    case 0xDC:
        H = SET(3, H);
        break;
    case 0xDD:
        L = SET(3, L);
        break;
    case 0xDE:
        mem->writeByte(HL(), SET(mem->readByte(HL()), 3));
        break;
    case 0xDF:
        A = SET(3, A);
        break;
    case 0xE0:
        B = SET(4, B);
        break;
    case 0xE1:
        C = SET(4, C);
        break;
    case 0xE2:
        D = SET(4, D);
        break;
    case 0xE3:
        E = SET(4, E);
        break;
    case 0xE4:
        H = SET(4, H);
        break;
    case 0xE5:
        L = SET(4, L);
        break;
    case 0xE6:
        mem->writeByte(HL(), SET(mem->readByte(HL()), 4));
        break;
    case 0xE7:
        A = SET(4, A);
        break;
    case 0xE8:
        B = SET(5, B);
        break;
    case 0xE9:
        C = SET(5, C);
        break;
    case 0xEA:
        D = SET(5, D);
        break;
    case 0xEB:
        E = SET(5, E);
        break;
    case 0xEC:
        H = SET(5, H);
        break;
    case 0xED:
        L = SET(5, L);
        break;
    case 0xEE:
        mem->writeByte(HL(), SET(mem->readByte(HL()), 5));
        break;
    case 0xEF:
        A = SET(5, A);
        break;
    case 0xF0:
        B = SET(6, B);
        break;
    case 0xF1:
        C = SET(6, C);
        break;
    case 0xF2:
        D = SET(6, D);
        break;
    case 0xF3:
        E = SET(6, E);
        break;
    case 0xF4:
        H = SET(6, H);
        break;
    case 0xF5:
        L = SET(6, L);
        break;
    case 0xF6:
        mem->writeByte(HL(), SET(mem->readByte(HL()), 6));
        break;
    case 0xF7:
        A = SET(6, A);
        break;
    case 0xF8:
        B = SET(7, B);
        break;
    case 0xF9:
        C = SET(7, C);
        break;
    case 0xFA:
        D = SET(7, D);
        break;
    case 0xFB:
        E = SET(7, E);
        break;
    case 0xFC:
        H = SET(7, H);
        break;
    case 0xFD:
        L = SET(7, L);
        break;
    case 0xFE:
        mem->writeByte(HL(), SET(mem->readByte(HL()), 7));
        break;
    case 0xFF:
        A = SET(7, A);
        break;
    default:
        break;
    }
}

uint16_t SM83::AF() {
    return (A << 8) | (F & 0xF0);
}

uint16_t SM83::BC() {
    return (B << 8) | C;
}

uint16_t SM83::DE() {
    return (D << 8) | E;
}

uint16_t SM83::HL() {
    return (H << 8) | L;
}

void SM83::setAF(uint16_t data) {
    A = (data >> 8) & 0xFF;
    F = data & 0xF0;
}

void SM83::setBC(uint16_t data) {
    B = (data >> 8) & 0xFF;
    C = data & 0xFF;
}

void SM83::setDE(uint16_t data) {
    D = (data >> 8) & 0xFF;
    E = data & 0xFF;
}

void SM83::setHL(uint16_t data) {
    H = (data >> 8) & 0xFF;
    L = data & 0xFF;
}

void SM83::setFlag(uint8_t flag, bool state) {
    if (state) {
        F |= (1 << flag);
    } else {
        F &= ~(1 << flag);
    }
}

bool SM83::getFlag(uint8_t flag) {
    return (F & (1 << flag)) != 0;
}

bool SM83::getFlagZ() {
    return (F & Flag_Z) != 0;
}

bool SM83::getFlagC() {
    return (F & Flag_C) != 0;
}

void SM83::alu16bitADD(uint16_t data) {
    uint16_t hl = HL();
    uint32_t result = hl + data;
    setHL(result & 0xFFFF);
    setFlag(Flag_N, false);
    setFlag(Flag_H, ((hl & 0x0FFF) + (data & 0x0FFF)) > 0x0FFF);
    setFlag(Flag_C, result > 0xFFFF);
}

void SM83::alu16bitSPADD() {
    int8_t data = (int8_t)mem->readByte(PC++);
    uint16_t tmp = SP;
    uint16_t result = SP + data;
    setFlag(Flag_Z, false);
    setFlag(Flag_N, false);
    setFlag(Flag_H, ((tmp & 0x0F) + (data & 0x0F)) > 0x0F);
    setFlag(Flag_C, ((tmp & 0xFF) + (data & 0xFF)) > 0xFF);
    SP = result;
}

void SM83::alu8bitADD(uint8_t data) {
    uint8_t tmp = A;
    A += data;
    setFlag(Flag_Z, (A == 0));
    setFlag(Flag_N, false);
    setFlag(Flag_H, ((tmp & 0x0F) + (data & 0x0F)) > 0x0F);
    setFlag(Flag_C, (tmp + data > 0xFF));
}

void SM83::alu8bitADC(uint8_t data, bool carry) {
    uint8_t tmp = A;
    uint8_t carryValue = carry ? 1 : 0;
    A += data + carryValue;
    setFlag(Flag_Z, (A == 0));
    setFlag(Flag_N, false);
    setFlag(Flag_H, ((tmp & 0x0F) + (data & 0x0F) + carryValue) > 0x0F);
    setFlag(Flag_C, (tmp + data + carryValue) > 0xFF);
}

void SM83::alu8bitSUB(uint8_t data) {
    uint8_t tmp = A;
    A -= data;
    setFlag(Flag_Z, (A == 0));
    setFlag(Flag_N, true);
    setFlag(Flag_H, (tmp & 0x0F) < (data & 0x0F));
    setFlag(Flag_C, (A > tmp));
}

void SM83::alu8bitSBC(uint8_t data, bool carry) {
    uint8_t tmp = A;
    uint8_t carryValue = carry ? 1 : 0;
    A -= (data + carryValue);
    setFlag(Flag_Z, (A == 0));
    setFlag(Flag_N, true);
    setFlag(Flag_H, ((tmp & 0x0F) < ((data & 0x0F) + carryValue)));
    setFlag(Flag_C, (tmp < (data + carryValue)));
}

uint8_t SM83::alu8bitINC(uint8_t data) {
    uint8_t tmp = data;
    data++;
    setFlag(Flag_Z, (data == 0));
    setFlag(Flag_N, false);
    setFlag(Flag_H, (tmp & 0x0F) == 0x0F);
    return data;
}

uint8_t SM83::alu8bitDEC(uint8_t data) {
    uint8_t tmp = data;
    data--;
    setFlag(Flag_Z, (data == 0));
    setFlag(Flag_N, true);
    setFlag(Flag_H, (tmp & 0x0F) == 0);
    return data;
}

void SM83::logic8bitAND(uint8_t data) {
    A &= data;
    setFlag(Flag_Z, (A == 0));
    setFlag(Flag_N, false);
    setFlag(Flag_H, true);
    setFlag(Flag_C, false);
}

void SM83::logic8bitOR(uint8_t data) {
    A |= data;
    setFlag(Flag_Z, (A == 0));
    setFlag(Flag_N, false);
    setFlag(Flag_H, false);
    setFlag(Flag_C, false);
}

void SM83::logic8bitXOR(uint8_t data) {
    A ^= data;
    setFlag(Flag_Z, (A == 0));
    setFlag(Flag_N, false);
    setFlag(Flag_H, true);
    setFlag(Flag_C, false);
}

void SM83::logic8bitCP(uint8_t data) {
    uint8_t result = A - data;
    setFlag(Flag_Z, (result == 0));
    setFlag(Flag_N, true);
    setFlag(Flag_H, ((A & 0x0F) < (data & 0x0F)));
    setFlag(Flag_C, (A < data));
}

void SM83::logic8bitDAA() {
    uint8_t correction = 0;
    bool carry = false;
    if (getFlag(Flag_H) || (!getFlag(Flag_N) && (A & 0x0F) > 9)) {
        correction |= 0x06;
    }
    if (getFlag(Flag_C) || (!getFlag(Flag_N) && A > 0x99)) {
        correction |= 0x60;
        carry = true;
    }
    if (getFlag(Flag_N)) {
        A -= correction;
    } else {
        A += correction;
    }
    setFlag(Flag_Z, (A == 0));
    setFlag(Flag_H, false);
    setFlag(Flag_C, carry);
}

void SM83::logic8bitCPL() {
    A = ~A;
    setFlag(Flag_N, true);
    setFlag(Flag_H, true);
}

uint8_t SM83::RLC(uint8_t data) {
    uint8_t result = data;
    bool carry = (result & 0x80) != 0;
    result = (result << 1) | (uint8_t)carry;
    setFlag(Flag_Z, (result == 0));
    setFlag(Flag_N, false);
    setFlag(Flag_H, false);
    setFlag(Flag_C, carry);
    return result;
}

uint8_t SM83::RL(uint8_t data) {
    uint8_t result = data;
    bool carryFlagBit = getFlagC();
    bool carry = (result & 0x80) != 0;
    result = (result << 1) | (carryFlagBit);
    setFlag(Flag_Z, (result == 0));
    setFlag(Flag_N, false);
    setFlag(Flag_H, false);
    setFlag(Flag_C, carry);
    return result;
}

uint8_t SM83::RRC(uint8_t data) {
    uint8_t result = data;
    bool carry = (result & 0x1) != 0;
    result = (result >> 1) | (carry << 7);
    setFlag(Flag_Z, (result == 0));
    setFlag(Flag_N, false);
    setFlag(Flag_H, false);
    setFlag(Flag_C, carry);
    return result;
}

uint8_t SM83::RR(uint8_t data) {
    uint8_t result = data;
    bool carryFlagBit = getFlagC();
    bool carry = (result & 0x01) != 0;
    result = (result >> 1) | (carryFlagBit << 7);
    setFlag(Flag_Z, (result == 0));
    setFlag(Flag_N, false);
    setFlag(Flag_H, false);
    setFlag(Flag_C, carry);
    return result;
}

uint8_t SM83::SLA(uint8_t data) {
    uint8_t result = data;
    bool carry = (result & 0x80) != 0;
    result = result << 1;
    setFlag(Flag_Z, (result == 0));
    setFlag(Flag_N, false);
    setFlag(Flag_H, false);
    setFlag(Flag_C, carry);
    return result;
}

uint8_t SM83::SRA(uint8_t data) {
    int8_t result = (int8_t)data;
    bool carry = (result & 0x01) != 0;
    result = result >> 1;
    setFlag(Flag_Z, (result == 0));
    setFlag(Flag_N, false);
    setFlag(Flag_H, false);
    setFlag(Flag_C, carry);
    return (uint8_t)result;
}

uint8_t SM83::SRL(uint8_t data) {
    uint8_t result = data;
    bool carry = (result & 0x01) != 0;
    result = result >> 1;
    setFlag(Flag_Z, (result == 0));
    setFlag(Flag_N, false);
    setFlag(Flag_H, false);
    setFlag(Flag_C, carry);
    return (uint8_t)result;
}

uint8_t SM83::SWAP(uint8_t data) {
    uint8_t result = (data & 0xF0) >> 4;
    result |= data << 4;
    setFlag(Flag_Z, (result == 0));
    setFlag(Flag_N, false);
    setFlag(Flag_H, false);
    setFlag(Flag_C, false);
    return result;
}

void SM83::BIT(int bit, uint8_t data) {
    bool result = (data & (1 << bit)) == 0;
    setFlag(Flag_Z, result);
    setFlag(Flag_N, false);
    setFlag(Flag_H, true);
}

uint8_t SM83::SET(int bit, uint8_t data) {
    uint8_t setBit = 1 << bit;
    uint8_t result = data | setBit;
    return result;
}

uint8_t SM83::RES(int bit, uint8_t data) {
    uint8_t resetBit = ~(1 << bit);
    uint8_t result = data & resetBit;
    return result;
}

void SM83::jp(bool state) {
    if (state) {
        lastCycleCount = 16;
        PC = mem->readWord(PC);
    } else {
        PC += 2;
    }
}

void SM83::jr(bool state) {
    if (state) {
        lastCycleCount = 12;
        int8_t offset = (int8_t)mem->readByte(PC++);
        PC += offset;
    } else {
        PC++;
    }
}

void SM83::call(bool state) {
    if (state) {
        uint16_t returnAddress = PC + 2;
        SP -= 2;
        mem->writeWord(SP, returnAddress);
        PC = mem->readWord(PC);
        lastCycleCount = 24;
    } else {
        PC += 2;
    }
}

void SM83::ret(bool state) {
    if (state) {
        lastCycleCount = 20;
        uint16_t address = mem->readWord(SP);
        SP += 2;
        PC = address;
    }
}

void SM83::reti(void) {
    ret(true);
    IME = true;
    IMEhold = true;
    lastCycleCount = 16;
}

void SM83::rst(uint8_t data) {
    SP -= 2;
    mem->writeWord(SP, PC);
    PC = data;
}

void SM83::ccf(void) {
    setFlag(Flag_C, !getFlagC());
    setFlag(Flag_N, false);
    setFlag(Flag_H, false);
}

void SM83::scf(void) {
    setFlag(Flag_N, false);
    setFlag(Flag_H, false);
    setFlag(Flag_C, true);
}

void SM83::nop(void) {
}

void SM83::halt(void) {
    ishalt = true;
    if (!IME && ((mem->readByte(IFaddress) & mem->readByte(IEaddress)) != 0)) {
        ishalt = false;
        PC--;
    }
}

void SM83::stop(void) {
    uint8_t key1 = mem->readByte(0xFF4D);

    if ((key1 & 0x1) == 0x1) {
        DoubleSpeed = !DoubleSpeed;
    }

    mem->writeByte(0xFF4D, (key1 & 0x7E) | ((DoubleSpeed ? 1 : 0) << 7));
}

void SM83::di(void) {
    IMEhold = false;
    EIDIFlag = true;
}

void SM83::ei(void) {
    IMEhold = true;
    EIDIFlag = true;
}