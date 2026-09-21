#!/bin/sh
set -eu
# Incremental builds only. SDK/core binaries remain outside version control.
task_root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
task_sdk=${PS3_CELL_SDK_HOST:-/home/humor/src/fbneo-libretro-ps3/.cellsdk-4.75-full/4.75/cell}
task_wrappers=${PS3_CELL_WRAPPERS:-/home/humor/src/fbneo-libretro-ps3-cell/.cell-wrappers}
task_makefile=${1:-Makefile.ps3}
task_target=${2:-retroarch_ps3.elf}
docker run --rm -v "$task_root:/work" -v "$task_sdk:/usr/local/cell:ro" \
 -v "$task_wrappers:/wrappers:ro" -w /work -e CELL_SDK=/usr/local/cell \
 cellsdk-4.75-wine:bookworm make -f "$task_makefile" -j1 \
 PPU_CC=/wrappers/ppu/bin/ppu-lv2-gcc \
 PPU_CXX=/wrappers/ppu/bin/ppu-lv2-g++ \
 PPU_CXXLD=/wrappers/ppu/bin/ppu-lv2-g++ \
 PPU_CCLD=/wrappers/ppu/bin/ppu-lv2-gcc "$task_target"
