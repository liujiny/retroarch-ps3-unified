/* Standalone real-hardware diagnostic. No RetroArch, core, stdio or graphics. */
#include <stdint.h>
#include <sys/process.h>
#include <sys/timer.h>
#include <cell/sysmodule.h>
#include <cell/fs/cell_fs_file_api.h>

SYS_PROCESS_PARAM(1001, 0x100000)

static int record(const char *path, const char *text)
{
   int fd, result;
   uint64_t size = 0, written = 0;
   while (text[size]) ++size;
   result = cellFsOpen(path, CELL_FS_O_WRONLY | CELL_FS_O_CREAT |
         CELL_FS_O_APPEND, &fd, 0, 0);
   if (result != 0) return result;
   result = cellFsWrite(fd, text, size, &written);
   cellFsFsync(fd);
   cellFsClose(fd);
   if (result != 0) return result;
   return written == size ? 0 : -1;
}

static void record_both(const char *text)
{
   record("/dev_hdd0/game/SSNE10001/USRDIR/ps3-native-probe.log", text);
   record("/dev_hdd0/ps3-native-probe.log", text);
}

static void record_result(unsigned value)
{
   char text[] = "CELL_SYSMODULE_FS return=0x00000000\n";
   static const char hex[] = "0123456789abcdef";
   unsigned i;
   for (i = 0; i < 8; ++i)
      text[26 + i] = hex[(value >> (28 - 4 * i)) & 15];
   record_both(text);
}

int main(void)
{
   int result;
   record_both("=== native probe v1: main entered, before FS module load ===\n");
   result = cellSysmoduleLoadModule(CELL_SYSMODULE_FS);
   record_both("native probe: FS module load returned; main is executing\n");
   record_result((unsigned)result);
   record_both("native probe: waiting 8 seconds; no core or graphics loaded\n");
   sys_timer_sleep(8);
   record_both("native probe: completed, returning to XMB intentionally\n");
   return 0;
}
