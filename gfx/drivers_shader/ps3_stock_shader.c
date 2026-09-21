#include <stddef.h>
#include <Cg/cg.h>

extern unsigned char _binary_gfx_drivers_gl_shaders_stock_ps3_vpo_start[];
extern unsigned char _binary_gfx_drivers_gl_shaders_stock_ps3_fpo_start[];

int ps3_create_stock_cg_programs(CGcontext context,
      CGprofile vertex_profile, CGprofile fragment_profile,
      CGprogram *vertex, CGprogram *fragment)
{
   *vertex = cgCreateProgram(context, CG_BINARY,
         (const char*)_binary_gfx_drivers_gl_shaders_stock_ps3_vpo_start,
         vertex_profile, NULL, NULL);
   *fragment = cgCreateProgram(context, CG_BINARY,
         (const char*)_binary_gfx_drivers_gl_shaders_stock_ps3_fpo_start,
         fragment_profile, NULL, NULL);

   if (!*vertex || !*fragment)
   {
      if (*vertex)
         cgDestroyProgram(*vertex);
      if (*fragment)
         cgDestroyProgram(*fragment);
      *vertex   = NULL;
      *fragment = NULL;
      return 0;
   }

   return 1;
}
