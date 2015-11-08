/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2010-2014 - Hans-Kristian Arntzen
 *  Copyright (C) 2011-2015 - Daniel De Matteis
 * 
 *  RetroArch is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  RetroArch is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with RetroArch.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

typedef enum
{
    BOXART_COVER_TYPE_NONE = 0,
    BOXART_COVER_TYPE_BOXFRONT,
    BOXART_COVER_TYPE_BOXBACK,
    BOXART_COVER_TYPE_CONTENT,
} boxart_cover_type_t;

typedef enum
{
    BOXART_ROM_TYPE_NONE = 0,
    BOXART_ROM_TYPE_32X,
    BOXART_ROM_TYPE_ATOMISWAVE,
    BOXART_ROM_TYPE_GENESIS,
    BOXART_ROM_TYPE_NEOGEO,
    BOXART_ROM_TYPE_SMS,
    BOXART_ROM_TYPE_MSX,
    BOXART_ROM_TYPE_C64,
    BOXART_ROM_TYPE_MEGADRIVE,
    BOXART_ROM_TYPE_NES,
    BOXART_ROM_TYPE_SNES,
    BOXART_ROM_TYPE_DREAMCAST,
    BOXART_ROM_TYPE_N64,
    BOXART_ROM_TYPE_PS1,
    BOXART_ROM_TYPE_ST,
    BOXART_ROM_TYPE_AMIGA,
    BOXART_ROM_TYPE_GAMECUBE,
    BOXART_ROM_TYPE_NAOMI,
    BOXART_ROM_TYPE_SEGACD,
    BOXART_ROM_TYPE_TURBODUO,
    BOXART_ROM_TYPE_NDS,
} boxart_rom_type_t;

static void boxart_find_path(char *out_path, const char *menu_title_name, menu_entry_t *entry, size_t size);

static boxart_rom_type_t boxart_get_rom_type(const char *file_ext);

static void boxart_fill_rom_directory_path(char *out_path, boxart_rom_type_t rom_type, size_t size);

static void boxart_fill_cover_directory_path(char *out_path, boxart_cover_type_t cover_type, boxart_rom_type_t rom_type, size_t size);

static char* boxart_get_cover_content_dir_for_rom_type(boxart_rom_type_t boxart_rom_type);

static int boxart_levenshtein(char *s1, char *s2);

static void boxart_format_name(char *str);

static void boxart_remove_brackets_content(char *str);

static char *boxart_trimwhitespace(char *str);