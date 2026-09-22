#ifndef PS3_CORE_INFO_DIAG_H
#define PS3_CORE_INFO_DIAG_H

/* Temporary, startup/menu-only diagnostics. Independent of log_verbosity. */
#if defined(__CELLOS_LV2__)
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
static void ps3_core_info_diag(const char *fmt, ...)
{
   int saved_errno = errno;
   FILE *file = fopen("/dev_hdd0/game/SSNE10001/USRDIR/ps3-core-info-diag.log", "a");
   va_list args;
   if (!file)
      file = fopen("/dev_hdd0/ps3-core-info-diag.log", "a");
   if (file)
   {
      va_start(args, fmt);
      vfprintf(file, fmt, args);
      va_end(args);
      fputc('\n', file);
      fclose(file);
   }
   errno = saved_errno;
}
#else
#define ps3_core_info_diag(...) ((void)0)
#endif
#endif
