/* Copyright  (C) 2010-2017 The RetroArch team
*
* ---------------------------------------------------------------------------------------
* The following license statement only applies to this file (file_stream_transforms.c).
* ---------------------------------------------------------------------------------------
*
* Permission is hereby granted, free of charge,
* to any person obtaining a copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation the rights to
* use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
* and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <streams/file_stream.h>
#include <file/file_path.h>
#include <retro_dirent.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

RFILE* rfopen(const char *path, char *mode)
{
   unsigned int retro_mode = RFILE_MODE_READ_TEXT;
   if (strstr(mode, "r"))
      if (strstr(mode, "b"))
         retro_mode = RFILE_MODE_READ;

   if (strstr(mode, "w"))
      retro_mode = RFILE_MODE_WRITE;
   if (strstr(mode, "+"))
      retro_mode = RFILE_MODE_READ_WRITE;

   return filestream_open(path, retro_mode, -1);
}

int rfclose(RFILE* stream)
{
   return filestream_close(stream);
}

long rftell(RFILE* stream)
{
   return filestream_tell(stream);
}

int rfseek(RFILE* stream, long offset, int origin)
{
   return filestream_seek(stream, offset, origin);
}

size_t rfread(void* buffer,
   size_t elementSize, size_t elementCount, RFILE* stream)
{
   if (!stream || !elementSize || !elementCount)
      return 0;
   return filestream_read(stream, buffer, elementSize * elementCount) /
      elementSize;
}

char *rfgets(char *buffer, int maxCount, RFILE* stream)
{
   return filestream_gets(stream, buffer, maxCount);
}

size_t rfwrite(void const* buffer,
   size_t elementSize, size_t elementCount, RFILE* stream)
{
   if (!stream || !elementSize || !elementCount)
      return 0;
   return filestream_write(stream, buffer, elementSize * elementCount) /
      elementSize;
}

int rfgetc(RFILE* stream)
{
   return stream ? filestream_getc(stream) : EOF;
}

int rfputc(int character, RFILE* stream)
{
   return stream ? filestream_putc(stream, character) : EOF;
}

long long int rfflush(RFILE* stream)
{
   return stream ? filestream_flush(stream) : EOF;
}

int rfprintf(RFILE* stream, const char* format, ...)
{
   int length;
   int result;
   char* buffer;
   va_list args;
   va_list args_copy;

   if (!stream)
      return -1;

   va_start(args, format);
   va_copy(args_copy, args);
   length = vsnprintf(NULL, 0, format, args_copy);
   va_end(args_copy);
   if (length < 0)
   {
      va_end(args);
      return length;
   }

   buffer = (char*)malloc((size_t)length + 1);
   if (!buffer)
   {
      va_end(args);
      return -1;
   }

   vsnprintf(buffer, (size_t)length + 1, format, args);
   va_end(args);
   result = (int)filestream_write(stream, buffer, (size_t)length);
   free(buffer);
   return result;
}

int rferror(RFILE* stream)
{
   (void)stream;
   return 0;
}

int rfeof(RFILE* stream)
{
   return stream ? filestream_eof(stream) : 1;
}

/* Newer static libretro cores initialise the optional VFS interface. The
 * 1.6.7 frontend uses native Cell FS directly, so these are intentionally
 * no-ops while preserving the current libretro ABI. */
struct retro_vfs_interface_info;

void filestream_vfs_init(const struct retro_vfs_interface_info* vfs_info)
{
   (void)vfs_info;
}

void path_vfs_init(const struct retro_vfs_interface_info* vfs_info)
{
   (void)vfs_info;
}

void dirent_vfs_init(const struct retro_vfs_interface_info* vfs_info)
{
   (void)vfs_info;
}

bool filestream_exists(const char* path)
{
   return path_file_exists(path);
}

struct RDIR* retro_opendir_include_hidden(const char* name,
      bool include_hidden)
{
   (void)include_hidden;
   return retro_opendir(name);
}
