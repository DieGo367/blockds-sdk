// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024
// SPDX-FileContributor: Adrian "asie" Siekierka, 2024

#include <nds.h>

// This is autogenerated from font.png and font.grit.
#include "font.h"

// Each 1bpp font tile is 8 bytes in size.
#define FONT_TILE_COUNT (fontTilesLen / 8)

// Draw the font tiles in a 32-wide rectangle formation.
#define FONT_WIDTH 32
#define FONT_HEIGHT (FONT_TILE_COUNT / FONT_WIDTH)

// The BIOS bit unpack routine can be effectively used to decompress
// one bit-per-pixel data, such as fonts, to any given color or two
// consecutive colors.
void bit_unpack_1bpp(void)
{
    int bg = bgInit(0, BgType_Text4bpp, BgSize_T_256x256, 0,1);
    u16 *gfx_data = bgGetGfxPtr(bg);
    u16 *map_data = bgGetMapPtr(bg);

    BG_PALETTE[0] = RGB15( 0,  0,  0); // Color 0 - Black
    BG_PALETTE[1] = RGB15(15, 15, 15); // Color 1 - Grey
    BG_PALETTE[2] = RGB15(31, 31, 31); // Color 2 - White

    // Clear the background map (32x32 tiles in size).
    memset(map_data, 0, sizeof(u16) * 32 * 32);

    // Place three copies of the font tiles on the map.
    int i = 0;
    for (int y = 0; y < FONT_HEIGHT * 3; y++)
    {
        for (int x = 0; x < FONT_WIDTH; x++)
        {
            map_data[y * 32 + x] = i++;
        }
    }

    // The first font is going to be unpacked so that the first color is
    // mapped to color 0, while the second color is mapped to color 1.
    TUnpackStruct unpackSettings = (TUnpackStruct) {
        fontTilesLen, // Source size, in bytes
        1, // Source bit depth
        4, // Destination bit depth
        0 | BIOS_UNPACK_OFFSET_ADD_NON_ZERO // Add zero to all non-zero converted values.
    };
    swiUnpackBits(fontTiles, gfx_data, &unpackSettings);

    // The second font is going to be unpacked so that the first color is
    // mapped to color 0, while the second color is mapped to color 2.
    unpackSettings = (TUnpackStruct) {
        fontTilesLen, // Source size, in bytes
        1, // Source bit depth
        4, // Destination bit depth
        1 | BIOS_UNPACK_OFFSET_ADD_NON_ZERO // Add one to all non-zero converted values.
    };
    swiUnpackBits(fontTiles, gfx_data + (FONT_TILE_COUNT * 32 / 2), &unpackSettings);

    // The third font is going to be unpacked so that the first color is
    // mapped to color 1, while the second color is mapped to color 2.
    unpackSettings = (TUnpackStruct) {
        fontTilesLen, // Source size, in bytes
        1, // Source bit depth
        4, // Destination bit depth
        1 | BIOS_UNPACK_OFFSET_ADD_ALWAYS // Add one to all converted values.
    };
    swiUnpackBits(fontTiles, gfx_data + 2 * (FONT_TILE_COUNT * 32 / 2), &unpackSettings);
}

// This is autogenerated from ball_*.png and ball_*.grit.
#include "ball_red.h"
#include "ball_green.h"

// Each 4bpp tile is 32 bytes in size.
#define BALL_RED_TILE_COUNT   (ball_redTilesLen / 32)
#define BALL_GREEN_TILE_COUNT (ball_greenTilesLen / 32)
#define BALL_COLOR_COUNT 16
#define BALL_WIDTH 4
#define BALL_HEIGHT 4

// The BIOS bit unpack routine can also be used to decompress
// 4 bit-per-pixel tile data on an 8-bit-per-pixel background.
void bit_unpack_4bpp(void)
{
    TUnpackStruct unpackSettings;
    int bg = bgInit(0, BgType_Text8bpp, BgSize_T_256x256, 0,1);
    u16 *gfx_data = bgGetGfxPtr(bg);
    u16 *map_data = bgGetMapPtr(bg);

    // Clear the first tile.
    BG_PALETTE[0] = RGB15( 0,  0,  0); // Color 0 - Black
    memset(gfx_data, 0, 64);

    // Clear the background map (32x32 tiles in size).
    memset(map_data, 0, sizeof(u16) * 32 * 32);

    // Calculate X/Y positions and data offsets for both 4bpp images.
    int ball_red_x = 4;
    int ball_red_y = 1;
    int ball_red_gfx_offset = 1;
    int ball_red_pal_offset = 1;
    int ball_green_x = 22;
    int ball_green_y = 9;
    int ball_green_gfx_offset = ball_red_gfx_offset + BALL_RED_TILE_COUNT;
    int ball_green_pal_offset = ball_red_pal_offset + BALL_COLOR_COUNT;

    // Place the red ball (palette, map, tile data).
    dmaCopy(ball_redPal, BG_PALETTE + ball_red_pal_offset, ball_redPalLen);

    for (int y = 0; y < BALL_HEIGHT; y++)
        for (int x = 0; x < BALL_WIDTH; x++)
            map_data[(y + ball_red_y) * 32 + (x + ball_red_x)] = ball_redMap[y * BALL_WIDTH + x] + ball_red_gfx_offset;

    unpackSettings = (TUnpackStruct) {
        ball_redTilesLen, // Source size, in bytes
        4, // Source bit depth
        8, // Destination bit depth
        ball_red_pal_offset | BIOS_UNPACK_OFFSET_ADD_ALWAYS // Add ball_red_pal_offset to all converted values.
    };
    swiUnpackBits(ball_redTiles, gfx_data + (ball_red_gfx_offset * 64 / 2), &unpackSettings);

    // Place the green ball (palette, map, tile data).
    dmaCopy(ball_greenPal, BG_PALETTE + ball_green_pal_offset, ball_greenPalLen);

    for (int y = 0; y < BALL_HEIGHT; y++)
        for (int x = 0; x < BALL_WIDTH; x++)
            map_data[(y + ball_green_y) * 32 + (x + ball_green_x)] = ball_greenMap[y * BALL_WIDTH + x] + ball_green_gfx_offset;

    unpackSettings = (TUnpackStruct) {
        ball_greenTilesLen, // Source size, in bytes
        4, // Source bit depth
        8, // Destination bit depth
        ball_green_pal_offset | BIOS_UNPACK_OFFSET_ADD_ALWAYS // Add ball_green_pal_offset to all converted values.
    };
    swiUnpackBits(ball_greenTiles, gfx_data + (ball_green_gfx_offset * 64 / 2), &unpackSettings);
}

void wait_any_key(void)
{
    scanKeys();

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();
        if (keysDown())
            break;
    }
}

int main(int argc, char *argv[])
{
    videoSetMode(MODE_0_2D);

    vramSetPrimaryBanks(VRAM_A_MAIN_BG, VRAM_B_LCD, VRAM_C_LCD, VRAM_D_LCD);

    consoleDemoInit();

    printf("1 -> 4 bits per pixel unpack\nAny button: Continue\n\n");
    bit_unpack_1bpp();
    wait_any_key();

    printf("4 -> 8 bits per pixel unpack\n");
    bit_unpack_4bpp();
    printf("START: Exit to loader\n");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();
        if (keysHeld() & KEY_START)
            break;
    }

    return 0;
}
