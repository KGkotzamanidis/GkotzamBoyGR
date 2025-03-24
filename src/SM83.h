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
#include <cstdint>

#include "EmulationUtils.h"
#include "Motherboard.h"

class SM83
{
public:
    SM83(Motherboard &mem);

    void reset(void);
    void step(void);

    bool getDoubleSpeed();
    int getlastCycleCount();

private: /* CPU Functions */
    Motherboard *mem= nullptr;

    int lastCycleCount;

    static const int instructionCount[];
    static const int prefixedinstructionCount[];

    void executeInstruction(uint8_t opcode);
    void executePrefixedInstruction(uint8_t opcode);

private: /* CPU Register functions */
    uint8_t A, B, C, D, E, F, H, L;
    uint16_t PC, SP;
    uint16_t AF();
    uint16_t BC();
    uint16_t DE();
    uint16_t HL();

    bool IME, IMEhold, ishalt, EIDIFlag, DoubleSpeed;

    void setAF(uint16_t data);
    void setBC(uint16_t data);
    void setDE(uint16_t data);
    void setHL(uint16_t data);

private: /* Flags */
    void setFlag(uint8_t flag, bool state);
    bool getFlag(uint8_t flag);

    bool getFlagZ();
    bool getFlagC();

private: /* Arithemtic and Logical Unit (ALU) */
    void alu16bitADD(uint16_t data);
    void alu16bitSPADD();

    void alu8bitADD(uint8_t data);
    void alu8bitADC(uint8_t data, bool carry);
    void alu8bitSUB(uint8_t data);
    void alu8bitSBC(uint8_t data, bool carry);

    uint8_t alu8bitINC(uint8_t data);
    uint8_t alu8bitDEC(uint8_t data);

    void logic8bitAND(uint8_t data);
    void logic8bitOR(uint8_t data);
    void logic8bitXOR(uint8_t data);
    void logic8bitCP(uint8_t data);
    void logic8bitDAA();
    void logic8bitCPL();

private: /* Rotate and Shift */
    uint8_t RLC(uint8_t data);
    uint8_t RL(uint8_t data);
    uint8_t RRC(uint8_t data);
    uint8_t RR(uint8_t data);
    uint8_t SLA(uint8_t data);
    uint8_t SRA(uint8_t data);
    uint8_t SRL(uint8_t data);
    uint8_t SWAP(uint8_t data);

    void BIT(int bit, uint8_t data);
    uint8_t SET(int bit, uint8_t data);
    uint8_t RES(int bit, uint8_t data);

private: /* CPU Control Instruction */
    void ccf(void);
    void scf(void);
    void nop(void);
    void halt(void);
    void stop(void);
    void di(void);
    void ei(void);

private: /* Jump Instruction */
    void jp(bool state);
    void jr(bool state);
    void call(bool state);
    void ret(bool state);
    void reti(void);
    void rst(uint8_t data);
};