#include <assert.h>
#include <stdio.h>
#include <array/rhmap.h>

struct entry { unsigned value; };
struct config { struct entry **entries_map; };

int main(void)
{
   struct config conf = {0};
   struct entry entries[128];
   char key[48];
   unsigned i;
   entries[0].value = 42;
   /* The launcher's failing first insertion, using a struct-member map. */
   RHMAP_SET_STR(conf.entries_map, "libretro_path", &entries[0]);
   assert(RHMAP_GET_STR(conf.entries_map, "libretro_path") == &entries[0]);
   for (i = 0; i < 128; ++i)
   {
      entries[i].value = i;
      sprintf(key, "option_%u", i);
      RHMAP_SET_STR(conf.entries_map, key, &entries[i]);
   }
   for (i = 0; i < 128; ++i)
   {
      sprintf(key, "option_%u", i);
      assert(RHMAP_GET_STR(conf.entries_map, key) == &entries[i]);
   }
   assert(RHMAP_LEN(conf.entries_map) == 129);
   assert(!RHMAP_HAS_STR(conf.entries_map, "missing"));
   RHMAP_FREE(conf.entries_map);
   assert(conf.entries_map == NULL);
   puts("PASS: first config entry, repeated growth, lookup, missing key, free");
   return 0;
}
