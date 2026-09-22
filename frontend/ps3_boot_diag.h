#ifndef PS3_BOOT_DIAG_H
#define PS3_BOOT_DIAG_H

#if defined(IS_SALAMANDER) && defined(__CELLOS_LV2__)
#include <stdio.h>
#include <stdarg.h>

/* Independent of RetroArch's optional logger. Close every record before
 * exit-spawn, which may never return. Never touch user configuration. */
static void ps3_boot_diag(const char *format, ...)
{
   FILE *file = fopen("/dev_hdd0/game/SSNE10001/USRDIR/ps3-boot-diag.log", "ab");
   va_list args;
   if (!file)
      file = fopen("/dev_hdd0/ps3-boot-diag.log", "ab");
   if (!file)
      return;
   va_start(args, format);
   vfprintf(file, format, args);
   va_end(args);
   fputc('\n', file);
   fflush(file);
   fclose(file);
}
#else
#define ps3_boot_diag(...) ((void)0)
#endif
#endif
