/* RetroArch - A frontend for libretro.
 *  Copyright (C) 2014-2015 - Ali Bouhlel
 *
 * RetroArch is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Found-
 * ation, either version 3 of the License, or (at your option) any later version.
 *
 * RetroArch is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with RetroArch.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <boolean.h>

#include <3ds.h>

#include <file/file_path.h>
#ifndef IS_SALAMANDER
#include <file/file_list.h>
#endif

#include "../../general.h"
#include "retroarch.h"
#include "audio/audio_driver.h"

#include "ctr/ctr_debug.h"

#ifdef IS_SALAMANDER
#include "../../file_ext.h"
#else
#include "../../menu/menu.h"
#endif

const char* elf_path_cst = "sdmc:/retroarch/test.3dsx";

static void frontend_ctr_get_environment_settings(int *argc, char *argv[],
      void *args, void *params_data)
{
   (void)args;

#ifndef IS_SALAMANDER
#if defined(HAVE_LOGGER)
   logger_init();
#elif defined(HAVE_FILE_LOGGER)
   global_t *global  = global_get_ptr();
   global->log_file = fopen("sdmc:/retroarch/retroarch-log.txt", "w");
#endif
#endif

   fill_pathname_basedir(g_defaults.dir.port, elf_path_cst, sizeof(g_defaults.dir.port));
   RARCH_LOG("port dir: [%s]\n", g_defaults.dir.port);

   fill_pathname_join(g_defaults.dir.core_assets, g_defaults.dir.port,
         "downloads", sizeof(g_defaults.dir.core_assets));
   fill_pathname_join(g_defaults.dir.assets, g_defaults.dir.port,
         "media", sizeof(g_defaults.dir.assets));
   fill_pathname_join(g_defaults.dir.core, g_defaults.dir.port,
         "cores", sizeof(g_defaults.dir.core));
   fill_pathname_join(g_defaults.dir.core_info, g_defaults.dir.port,
         "cores", sizeof(g_defaults.dir.core_info));
   fill_pathname_join(g_defaults.dir.savestate, g_defaults.dir.core,
         "savestates", sizeof(g_defaults.dir.savestate));
   fill_pathname_join(g_defaults.dir.sram, g_defaults.dir.core,
         "savefiles", sizeof(g_defaults.dir.sram));
   fill_pathname_join(g_defaults.dir.system, g_defaults.dir.core,
         "system", sizeof(g_defaults.dir.system));
   fill_pathname_join(g_defaults.dir.playlist, g_defaults.dir.core,
         "playlists", sizeof(g_defaults.dir.playlist));
   fill_pathname_join(g_defaults.dir.remap, g_defaults.dir.port,
         "remaps", sizeof(g_defaults.dir.remap));
   fill_pathname_join(g_defaults.path.config, g_defaults.dir.port,
         "retroarch.cfg", sizeof(g_defaults.path.config));
   
#if 0
   int i;
   DEBUG_VAR(*argc);
   for (i=0; i < *argc; i++)
      DEBUG_STR(argv[i]);
   DEBUG_HOLD();
#endif

   *argc = 0;

#ifndef IS_SALAMANDER
#if 0
   if (argv[1] && (argv[1][0] != '\0'))
   {
      static char path[PATH_MAX_LENGTH];
      struct rarch_main_wrap *args = NULL;

      *path = '\0';
      args = (struct rarch_main_wrap*)params_data;

      if (args)
      {
         strlcpy(path, argv[1], sizeof(path));

         args->touched        = true;
         args->no_content     = false;
         args->verbose        = false;
         args->config_path    = NULL;
         args->sram_path      = NULL;
         args->state_path     = NULL;
         args->content_path   = path;
         args->libretro_path  = NULL;

         RARCH_LOG("argv[0]: %s\n", argv[0]);
         RARCH_LOG("argv[1]: %s\n", argv[1]);
         RARCH_LOG("argv[2]: %s\n", argv[2]);

         RARCH_LOG("Auto-start game %s.\n", argv[1]);
      }
   }
#endif
#endif
}

static void frontend_ctr_deinit(void *data)
{
   extern PrintConsole* currentConsole;
   Handle lcd_handle;
   u8 not_2DS;
   (void)data;
#ifndef IS_SALAMANDER
   global_t *global   = global_get_ptr();
   global->verbosity = true;

#ifdef HAVE_FILE_LOGGER
   if (global->log_file)
      fclose(global->log_file);
   global->log_file = NULL;
#endif

   if(gfxBottomFramebuffers[0] == (u8*)currentConsole->frameBuffer)
      wait_for_input();

   CFGU_GetModelNintendo2DS(&not_2DS);
   if(not_2DS && srvGetServiceHandle(&lcd_handle, "gsp::Lcd") >= 0)
   {
      u32 *cmdbuf = getThreadCommandBuffer();
      cmdbuf[0] = 0x00110040;
      cmdbuf[1] = 2;
      svcSendSyncRequest(lcd_handle);
      svcCloseHandle(lcd_handle);
   }

   exitCfgu();
   ndspExit();
   csndExit();   
   gfxExit();
#endif
}

static void frontend_ctr_shutdown(bool unused)
{
   (void)unused;
}

static void ctr_check_dspfirm(void)
{
   FILE* dsp_fp = fopen("sdmc:/3ds/dspfirm.cdc", "rb");

   if(dsp_fp)
      fclose(dsp_fp);
   else
   {
      uint32_t* code_buffer;
      uint32_t* ptr;
      FILE* code_fp;
      size_t code_size;
      const uint32_t dsp1_magic = 0x31505344; /* "DSP1" */

      code_fp =fopen("sdmc:/3ds/code.bin", "rb");
      if(code_fp)
      {
         fseek(code_fp, 0, SEEK_END);
         code_size = ftell(code_fp);
         fseek(code_fp, 0, SEEK_SET);

         code_buffer = (uint32_t*) malloc(code_size);
         if(code_buffer)
         {
            fread(code_buffer, 1, code_size, code_fp);

            for (ptr = code_buffer + 0x40; ptr < (code_buffer + (code_size >> 2)); ptr++)
            {
               if (*ptr == dsp1_magic)
               {
                  size_t dspfirm_size = ptr[1];
                  ptr -= 0x40;
                  if ((ptr + (dspfirm_size >> 2)) > (code_buffer + (code_size >> 2)))
                     break;

                  dsp_fp = fopen("sdmc:/3ds/dspfirm.cdc", "wb");
                  if(!dsp_fp)
                     break;
                  fwrite(ptr, 1, dspfirm_size, dsp_fp);
                  fclose(dsp_fp);
                  break;
               }
            }
            free(code_buffer);
         }
         fclose(code_fp);
      }
   }
}

void svchax_init(void);

static void frontend_ctr_init(void *data)
{
#ifndef IS_SALAMANDER
   (void)data;
   global_t *global   = global_get_ptr();
   global->verbosity = true;

#if 0
   APT_SetAppCpuTimeLimit(NULL, 80);
#endif
   gfxInit(GSP_BGR8_OES,GSP_RGB565_OES,false);   
   gfxSet3D(false);
   consoleInit(GFX_BOTTOM, NULL);

   /* enable access to all service calls when possible. */
   osSetSpeedupEnable(false);
   svchax_init();
   osSetSpeedupEnable(true);

   audio_driver_t* dsp_audio_driver = &audio_ctr_dsp;
   if(csndInit() != 0)
   {
      dsp_audio_driver = &audio_ctr_csnd;
      audio_ctr_csnd = audio_ctr_dsp;
      audio_ctr_dsp  = audio_null;
   }
   ctr_check_dspfirm();
   if(ndspInit() != 0)
      *dsp_audio_driver = audio_null;
   initCfgu();
#endif
}


static int frontend_ctr_get_rating(void)
{
   return 3;
}

enum frontend_architecture frontend_ctr_get_architecture(void)
{
   return FRONTEND_ARCH_ARM;
}

static int frontend_ctr_parse_drive_list(void *data)
{
   file_list_t *list = (file_list_t*)data;

#ifndef IS_SALAMANDER
   if (!list)
      return -1;

   menu_entries_push(list,
         "sdmc:/", "", MENU_FILE_DIRECTORY, 0, 0);
#endif

   return 0;
}

frontend_ctx_driver_t frontend_ctx_ctr = {
   frontend_ctr_get_environment_settings,
   frontend_ctr_init,
   frontend_ctr_deinit,
   NULL,                         /* exitspawn */
   NULL,                         /* process_args */
   NULL,                         /* exec */
   NULL,                         /* set_fork */
   frontend_ctr_shutdown,
   NULL,                         /* get_name */
   NULL,                         /* get_os */
   frontend_ctr_get_rating,
   NULL,                         /* load_content */
   frontend_ctr_get_architecture,
   NULL,                         /* get_powerstate */
   frontend_ctr_parse_drive_list,
   "ctr",
};
