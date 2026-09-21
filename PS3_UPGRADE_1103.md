# RetroArch 1.10.3 / Cell SDK experiment (NOT FINAL)

## Provenance

- Official RetroArch tag `v1.10.3`, commit
  `9b282aa742b6c3d2f2925ae5a12e2cd7c6b6ad38` (not upstream master).
- PSX Community Edition wrapper commit
  `41fdf7f0d82b428aa90fe7b79afe2d69f6da580b`: Cell SDK Makefile and
  `diff.patch` directory layout, with local adaptations.
- Fix12 reference: `liujiny/retroarch-ps3-unified` commit
  `a0324116ad675f5b0d4406732e4c1e25b7e8de17`.
- Reuses the existing FBNeo `libretro_ps3.a`; no core source upgrade.

Do NOT run the CE wrapper's `make init`: it resets the submodule and pulls
unversioned master. Its custom HTTP updater endpoint, index-name rewrite,
unconditional restart changes and unused config-copy helper are not imported.
This build does not claim compatibility with that historical update server.

## Ported fixes

- Full-width GCC TOC addressing (`-mall-base-toc`) and fatal assembler warnings.
- Offline-compiled RSX stock shaders loaded as ordinary Cg program objects.
- 256 KiB POSIX thread stacks on Cell SDK.
- Audio FIFO and condition-variable synchronization use the same mutex;
  writes are chunked to available capacity.
- Live texture filtering callback without PSGL reinitialization.
- Guard launch arguments before reading them; SSNE10001 fallback directory.

## Lightweight EBOOT

`Makefile.ps3.salamander` builds the upstream Salamander bootstrap separately.
It has no FBNeo archive, GL/PSGL, audio driver, or RetroArch menu linked in.
It reads `USRDIR/retroarch-salamander.cfg` (`libretro_path`), otherwise finds
a core SELF in `USRDIR/cores`. It does not show a core-selection menu itself.
Core selection remains in the frontend bundled with each core SELF.

The PS3 exit-spawn API replaces the calling process. The system reclaims the
old process's address space; this is not in-process PRX/dlopen loading and does
not require manual memory wiping. Each core SELF still contains its frontend.
This removes a resident launcher, but does not reduce the active core's own
frontend footprint. Never attempt to zero arbitrary memory before exit-spawn.

## Build

Run `sh tools/build_ps3_cell.sh` for the frontend/core ELF.
Run `sh tools/build_ps3_cell.sh Makefile.ps3.salamander retroarch-salamander_ps3.elf`
for the launcher. Both use Docker and incremental builds, separate object
directories, and the Cell SDK GCC wrappers. Host SDK/wrapper paths can be
overridden with `PS3_CELL_SDK_HOST` / `PS3_CELL_WRAPPERS`.
Package ELFs with SDK `make_fself_npdrm.exe`.

## Deployment and remaining tests

Back up the existing EBOOT, core, configs and assets before testing.
Install the new launcher at `SSNE10001/USRDIR/EBOOT.BIN` and the new core SELF
in `SSNE10001/USRDIR/cores/`; do not swap those two files.
Set `libretro_path` to the exact new core's guest path. A sample config should
be copied deliberately, not blindly overwrite a user's existing config.
Never mix ELF/SELF files between builds; record SHA256.

The new 1.10.3 frontend may require matching menu assets. Existing playlists
and history have not been converted or overwritten. Audio audibility and
real-PS3 behavior of fix12 were not validated; this upgrade additionally needs
RPCS3 LLVM boot, kov2/mslug5 gameplay, filtering, EBOOT/core switching and
repeated launch/exit testing. A successful build is not runtime validation.

The host harness `tools/test_ps3_audio_fifo.js` extracts the actual write
function and tests it with mock FIFO/locking/wait primitives. Chunked blocking
writes, nonblocking full/partial writes, wait-error returns and shutdown passed.
This is a logic test, not a real concurrency or audio-device test.

## Test2: configuration hash map aliasing fix

RPCS3 test1 failed before exit-spawn while reading `libretro_path` from
`retroarch-salamander.cfg`. PC 0x1a8c8 mapped to `rhmap__idx`, called from
`config_file_from_string_internal` at 0x1c758. The compiler loaded the old map
pointer before storing the result of `rhmap__grow`, then used old NULL minus
20 as the header; reading header maxlen accessed 0xfffffff0.

`RHMAP__GROW` now assigns through the actual pointer type on GCC instead of
type-punning via `void **`. Config lookups use an index lookup without
allocating through a const config pointer. Both launcher and frontend need
this fix. Host optimized hash-map growth tests and upstream config parsing
tests pass. The user confirmed test2 runs normally in RPCS3 on 2026-09-21.
Real-PS3 and audible audio validation remain outstanding; this is not a final release.

## Test2 checkpoint (2026-09-21)

- FBNeo source revision: `9d327db72d62606c75f80129d80f4fb5a2149a25`.
- EBOOT.BIN SHA256:
  `af6bd12583a347be9db297b274fde131f6b388fa0444568b2b67dee3f94c9821`.
- `cores/fbneo_ra1103_test1.self` SHA256:
  `4f93c80159d9e2c43f8b1ceeb53474e5ca6da2e30b16e766e98b7bd61f7e63b7`.
- Core filename and internal test1/build=110301 marker remain unchanged to
  preserve the existing launcher configuration. Identify test2 by SHA256.
- User-provided memory log covers mslug5 and two kov sessions (not kov2).
  Reported user-memory total is 213 MiB; free memory before ROM load is
  132.31 MiB. First-frame free memory: mslug5 23.04 MiB, kov 67.66/66.66 MiB.
  Tracked allocation failures are zero and tracked live allocations return
  to zero on unload. This is not proof that all untracked allocations are
  leak-free, nor a measurement of host RPCS3 RAM usage.
- SDK, static core archive, ELF/SELF binaries and user logs are not committed.
