#!/bin/sh
# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2024

BLOCKSDS=/opt/blocksds/core/
WONDERFUL_TOOLCHAIN=/opt/wonderful

GRIT=$BLOCKSDS/tools/grit/grit

HUFFMAN=$WONDERFUL_TOOLCHAIN/bin/wf-nnpack-huffman
LZSS=$WONDERFUL_TOOLCHAIN/bin/wf-nnpack-lzss
RLE=$WONDERFUL_TOOLCHAIN/bin/wf-nnpack-rle

rm -rf data
mkdir data

$GRIT assets/city.png -ftb -fh! -W3 -odata/city

mv data/city.img.bin data/city_uncompressed.bin

$HUFFMAN -e8 data/city_uncompressed.bin data/city_huffman.bin
$LZSS -ewo data/city_uncompressed.bin data/city_lzss.bin
$RLE -e data/city_uncompressed.bin data/city_rle.bin
