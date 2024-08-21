// SPDX-License-Identifier: Zlib
//
// Copyright (c) 2024 Antonio Niño Díaz

#include <stdlib.h>

#include <nds.h>

int oamLoadGfxGrf(OamState *oam, const char *path, int palIndex, void **gfxOut,
                  SpriteSize *sizeOut, SpriteColorFormat *formatOut)
{
    if ((oam != &oamMain) && (oam != &oamSub))
        return -1;

    int ret = 0;

    void *gfxGrf = NULL;
    void *palGrf = NULL;
    size_t gfxSize, palSize;
    GRFHeader header = { 0 };
    GRFError err = grfLoadPath(path, &header, &gfxGrf, &gfxSize, NULL, NULL,
                               &palGrf, &palSize);
    if (err != GRF_NO_ERROR)
    {
        printf("grferr: %d\n", err);
        ret = -2;
        goto exit;
    }

    if (gfxGrf == NULL)
    {
        ret = -3;
        goto exit;
    }

    SpriteSize size = oamDimensionsToSize(header.gfxWidth, header.gfxHeight);
    if (size == SpriteSize_Invalid)
    {
        ret = -4;
        goto exit;
    }

    SpriteColorFormat format;

    switch (header.gfxAttr)
    {
        case 4: // 16 color
        {
            format = SpriteColorFormat_16Color;

            // If we haven't been requested to load the palette, ignore this
            if (palIndex < 0)
                break;

            // If the GRF file had no palette, fail
            if (palGrf == NULL)
            {
                ret = -5;
                goto exit;
            }

            // If the palette index is too high, fail
            if (palIndex >= 16)
            {
                ret = -6;
                goto exit;
            }

            uint16_t *palAlloc;

            if (oam == &oamMain)
                palAlloc = SPRITE_PALETTE + palIndex * 16;
            else
                palAlloc = SPRITE_PALETTE_SUB + palIndex * 16;

            DC_FlushRange(palGrf, palSize);
            dmaCopy(palGrf, palAlloc, palSize);
            break;
        }
        case 8: // 256 color
        {
            format = SpriteColorFormat_256Color;

            // If we haven't been requested to load the palette, ignore this
            if (palIndex < 0)
                break;

            // If the GRF file had no palette, fail
            if (palGrf == NULL)
            {
                ret = -7;
                goto exit;
            }

            if (oam == &oamMain)
            {
                if (REG_DISPCNT & DISPLAY_SPR_EXT_PALETTE) // Extended palette
                {
                    // If the palette index is too high, fail
                    if (palIndex >= 16)
                    {
                        ret = -8;
                        goto exit;
                    }

                    uint8_t old_vram_f = VRAM_F_CR;
                    uint8_t old_vram_g = VRAM_G_CR;

                    uint16_t *palAlloc;

                    if (VRAM_F_CR == (VRAM_ENABLE | VRAM_F_SPRITE_EXT_PALETTE))
                    {
                        vramSetBankF(VRAM_F_LCD);
                        palAlloc = VRAM_F_EXT_SPR_PALETTE[palIndex];
                    }
                    else if (VRAM_G_CR == (VRAM_ENABLE | VRAM_G_SPRITE_EXT_PALETTE))
                    {
                        vramSetBankG(VRAM_G_LCD);
                        palAlloc = VRAM_G_EXT_SPR_PALETTE[palIndex];
                    }
                    else
                    {
                        ret = -9;
                        goto exit;
                    }

                    DC_FlushRange(palGrf, palSize);
                    dmaCopy(palGrf, palAlloc, palSize);

                    VRAM_F_CR = old_vram_f;
                    VRAM_G_CR = old_vram_g;
                }
                else // Regular palette
                {
                    if (palIndex != 0)
                    {
                        ret = -10;
                        goto exit;
                    }

                    DC_FlushRange(palGrf, palSize);
                    dmaCopy(palGrf, SPRITE_PALETTE, palSize);
                }

            }
            else // if (oam == &oamSub)
            {
                if (REG_DISPCNT_SUB & DISPLAY_SPR_EXT_PALETTE) // Extended palette
                {
                    // If the palette index is too high, fail
                    if (palIndex >= 16)
                    {
                        ret = -8;
                        goto exit;
                    }

                    uint8_t old_vram_i = VRAM_I_CR;

                    uint16_t *palAlloc;

                    if (VRAM_I_CR == (VRAM_ENABLE | VRAM_I_SUB_SPRITE_EXT_PALETTE))
                    {
                        vramSetBankI(VRAM_I_LCD);
                        palAlloc = VRAM_I_EXT_SPR_PALETTE[palIndex];
                    }
                    else
                    {
                        ret = -11;
                        goto exit;
                    }

                    DC_FlushRange(palGrf, palSize);
                    dmaCopy(palGrf, palAlloc, palSize);

                    VRAM_I_CR = old_vram_i;
                }
                else // Regular palette
                {
                    if (palIndex != 0)
                    {
                        ret = -12;
                        goto exit;
                    }

                    DC_FlushRange(palGrf, palSize);
                    dmaCopy(palGrf, SPRITE_PALETTE_SUB, palSize);
                }
            }

            break;
        }
        case 16: // 16 bit per pixel bitmap
        {
            format = SpriteColorFormat_Bmp;

            if (palIndex > 0)
            {
                ret = -13;
                goto exit;
            }

            break;
        }
        default: // Invalid format
        {
            ret = -14;
            goto exit;
        }
    }

    // Allocate space for the tiles and copy them there.
    void *gfxAlloc = oamAllocateGfx(oam, size, format);
    if (gfxAlloc == NULL)
    {
        ret = -15;
        goto exit;
    }

    // Flush cache so that DMA can be used to copy the data to VRAM
    DC_FlushRange(gfxGrf, gfxSize);
    dmaCopy(gfxGrf, gfxAlloc, gfxSize);

    // Return information of the loaded graphics
    *gfxOut = gfxAlloc;
    if (sizeOut)
        *sizeOut = size;
    if (formatOut)
        *formatOut = format;

    ret = 0;
exit:
    free(gfxGrf);
    free(palGrf);

    return ret;
}
