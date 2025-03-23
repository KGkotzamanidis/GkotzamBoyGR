/*
 *An Gameboy and GameboyColor emulation with project name GkotzamBoyGR
 *Copyright (C) <Tue Mar 18 2025>  <KGkotzamanidis>
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
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "Bios.h"
#include "Motherboard.h"
#include "Interrupts.h"
#include "SM83.h"
#include "Timers.h"
int main(int argc, char *argv[])
{
    Bios bios;
    bios.loadBios("./resource/bios.bin");

    Interrupts interrupts;
    Timers timers(interrupts);
    Motherboard board(bios, timers, interrupts);
    SM83 cpu(board);

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *windows = nullptr;
    SDL_Surface *icon = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Event event;
    windows = SDL_CreateWindow("GkotzamBoyGR", 300, 250, SDL_WINDOW_OPENGL);
    icon = IMG_Load("./resource/GkotzamBoy.png");
    SDL_SetWindowIcon(windows, icon);

    SDL_Log("Debug:Available renderer drivers:");
    for (int i = 0; i < SDL_GetNumRenderDrivers(); i++)
    {
        SDL_Log("%d. %s", i + 1, SDL_GetRenderDriver(i));
    }

    renderer = SDL_CreateRenderer(windows, "gpu");

    bool done = false;
    bool emulating = false;

    cpu.reset();
    
    while (!done)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                done = true;
            }
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                emulating = true;
            }
        }
    }

    SDL_DestroyWindow(windows);
    SDL_Quit();

    return 0;
}