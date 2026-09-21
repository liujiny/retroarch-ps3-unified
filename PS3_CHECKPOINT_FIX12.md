# PS3 Cell SDK checkpoint — fix12 (not final)

Date: 2026-09-21

This is an intermediate recovery point, not a stable/final release.

## User validation

- kov2 and mslug5 entered gameplay with correct graphics on RPCS3.
- After the audio FIFO synchronization fix, kov2 also entered with PPU LLVM.
- Bilinear filtering ON/OFF visibly changes the image with fix12.
- Audible output has NOT been tested (no speakers available).
- This checkpoint has NOT been validated on PS3 hardware.

## Components and artifact

- Frontend: RetroArch 1.6.7 with Cell SDK 4.75 compatibility fixes.
- Core source: liujiny/fbneo-libretro-ps3 at
  `9d327db72d62606c75f80129d80f4fb5a2149a25`.
- SELF: `fbneo_libretro_ps3_startup_fix12_20260921.self`
- Size: 86,345,844 bytes.
- SHA256: `fc444d957d5dd01727dc889bd362b9cd8a0266b47ed159108f805a1c1142a53c`
- Deployment tested: `/dev_hdd0/game/SSNE10001/USRDIR/cores/`.
- Do not replace the frontend/Salamander EBOOT with the core SELF.
- SDK files, core archives, ELF/SELF files and build objects are not source commits.

## Important changes

- Griffin uses GCC `-mall-base-toc` and fatal assembler warnings to avoid
  overflowing 16-bit TOC addressing; several drivers are separate objects.
- Stock PSGL Cg shaders are compiled offline and loaded as CG_BINARY.
- POSIX worker threads on Cell use 256 KiB stacks.
- Audio FIFO checks, updates and condition waits share the same mutex;
  large writes are chunked and condition waits check errors.
- GL filtering callback updates all history textures. PSGL filtering changes
  no longer request a full video reinitialization.
- `[PS3 filter]` diagnostics report submitted parameters, not GPU readback.

## Known limitations

- Other paths that reinitialize PSGL can still report Raw SPU initialization
  failures; avoiding reinit for filtering is not a general lifecycle fix.
- New JSON history playlists are incompatible with this frontend's old
  six-line playlist reader. The user requested leaving history unchanged.
- Existing stb_vorbis duplicate-symbol linker warnings remain.

Next development line: crystalct/RetroArch_PSX_CE, isolated from this checkpoint.
