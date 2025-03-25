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
#include "Interrupts.h"
#include "Motherboard.h"
#include "SM83.h"
#include "Timers.h"

std::vector<uint32_t> framebuffer;

void drawEmoji() {
    for (int y = 30; y < 110; y++) {
        for (int x = 40; x < 120; x++) {
            int index = y * 160 + x;

            // Face color (yellow)
            framebuffer[index] = 0xFFFF00FF;

            // Eyes (black)
            if ((x > 60 && x < 70 && y > 50 && y < 60) || (x > 90 && x < 100 && y > 50 && y < 60)) {
                framebuffer[index] = 0x000000FF;
            }

            // Smile (black curve)
            if (y > 80 && y < 90 && (x > 55 && x < 105)) {
                framebuffer[index] = 0x000000FF;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    Bios bios;
    Interrupts interrupts;
    Timers timers(interrupts);
    Motherboard board(bios, timers, interrupts, false);
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
    for (int i = 0; i < SDL_GetNumRenderDrivers(); i++) {
        SDL_Log("%d. %s", i + 1, SDL_GetRenderDriver(i));
    }

    renderer = SDL_CreateRenderer(windows, "gpu");

    bool done = false;
    bool emulating = false;

    framebuffer.resize(160 * 144, 0x000000FF);

    bios.loadBios("./resource/bios.bin");
    timers.reset();
    cpu.reset();

    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                emulating = true;
            }
        }

        drawEmoji();

        SDL_Surface *surface = SDL_CreateSurfaceFrom(
            160, 144,
            SDL_PIXELFORMAT_RGBA32,
            framebuffer.data(),
            160 * sizeof(uint32_t));

        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);

        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_DestroyTexture(texture);
    }

    SDL_DestroyWindow(windows);
    SDL_Quit();

    return 0;
}