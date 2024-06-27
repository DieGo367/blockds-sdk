// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <nds.h>

// This is autogenerated from ball.png and ball.grit
#include "ball.h"
// This is autogenerated from city.png and city.grit
#include "city.h"

int main(int argc, char *argv[])
{
    videoSetMode(MODE_0_2D);

    vramSetPrimaryBanks(VRAM_A_MAIN_BG, VRAM_B_MAIN_SPRITE, VRAM_C_LCD, VRAM_D_LCD);

    int bg = bgInit(0, BgType_Text8bpp, BgSize_T_256x256, 0,1);

    dmaCopy(cityTiles, bgGetGfxPtr(bg), cityTilesLen);
    dmaCopy(cityMap, bgGetMapPtr(bg), cityMapLen);
    dmaCopy(cityPal, BG_PALETTE, cityPalLen);

    oamInit(&oamMain, SpriteMapping_1D_32, false);

    // Allocate space for the tiles and copy them there
    u16 *gfxMain = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
    dmaCopy(ballTiles, gfxMain, ballTilesLen);

    // No need to copy the palette, this sprite won't be displayed normally
    //dmaCopy(ballPal, SPRITE_PALETTE, ballPalLen);

    oamSet(&oamMain, 0,
           100, 50, // X, Y
           0, // Priority
           0, // Palette index (unused for window sprites)
           SpriteSize_32x32, SpriteColorFormat_256Color, // Size, format
           gfxMain,  // Graphics offset
           -1, // Affine index
           false, // Double size
           false, // Hide
           false, false, // H flip, V flip
           false); // Mosaic

    // Mark this sprite to be used as window mask
    oamSetBlendMode(&oamMain, 0, SpriteMode_Windowed);

    // Disable display of the background everywhere but in the parts highlighted
    // by the sprite.
    bgWindowEnable(bg, WINDOW_OBJ);
    bgWindowDisable(bg, WINDOW_OUT);
    windowEnable(WINDOW_OBJ);

    consoleDemoInit();

    printf("PAD:   Move sprite\n");
    printf("START: Exit to loader\n");

    int x = 50, y = 70;

    while (1)
    {
        swiWaitForVBlank();

        oamSetXY(&oamMain, 0, x, y);

        oamUpdate(&oamMain);

        scanKeys();

        u16 keys_held = keysHeld();

        if (keys_held & KEY_UP)
            y--;
        else if (keys_held & KEY_DOWN)
            y++;

        if (keys_held & KEY_LEFT)
            x--;
        else if (keys_held & KEY_RIGHT)
            x++;

        if (keys_held & KEY_START)
            break;
    }

    return 0;
}
