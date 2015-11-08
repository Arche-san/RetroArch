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

#define BOXART_MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

#define LEVENSHTEIN_DISTANCE_MAX 5

#include "boxart.h"

static void boxart_find_path(char *out_path, const char *menu_title_name, menu_entry_t *entry, size_t size)
{
    char boxarts_directory[PATH_MAX_LENGTH] = {0};
    char game_name[PATH_MAX_LENGTH] = {0};
    settings_t *settings = config_get_ptr();
    struct string_list *images_file_list = NULL;
    char best_image_path[PATH_MAX_LENGTH] = {0};
    
    boxart_rom_type_t rom_type = boxart_get_rom_type(entry->label);
    boxart_cover_type_t cover_type = BOXART_COVER_TYPE_BOXFRONT;
    
    strlcpy(game_name, entry->path, sizeof(game_name));
    boxart_format_name(game_name);
    
    strlcpy(boxarts_directory, settings->boxarts_directory, sizeof(boxarts_directory));
    boxart_fill_rom_directory_path(boxarts_directory, rom_type, sizeof(boxarts_directory));
    boxart_fill_cover_directory_path(boxarts_directory, cover_type, rom_type, sizeof(boxarts_directory));
    
    images_file_list = dir_list_new(boxarts_directory, "png", false, false);
    if(NULL != images_file_list)
    {
        int best_distance = LEVENSHTEIN_DISTANCE_MAX;
        char image_name[PATH_MAX_LENGTH] = {0};
        
        for(int i=0; i<images_file_list->size; ++i)
        {
            fill_pathname_base(image_name, images_file_list->elems[i].data, sizeof(image_name));
            boxart_format_name(image_name);
            int distance = boxart_levenshtein(image_name, game_name);
            if(distance < best_distance)
            {
                strlcpy(best_image_path, images_file_list->elems[i].data, sizeof(best_image_path));
                best_distance = distance;
            }
        }
        
        dir_list_free(images_file_list);
        
        #if 0
        char msg[PATH_MAX_LENGTH];
        snprintf(msg, sizeof(msg), "BestDistance = %d", best_distance);
        rarch_main_msg_queue_push(msg, 0, 30, false);
        #endif
    }
    
    if(best_image_path) {
        strlcpy(out_path, best_image_path, size);
    }
}

static boxart_rom_type_t boxart_get_rom_type(const char *file_path) {
    
    const char *file_ext = path_get_extension(file_path);
    
    //ISO
    if (strstr("isp", file_ext) != NULL) {
        return BOXART_ROM_TYPE_PS1;
    }
    //32X
    if (strstr("32x", file_ext) != NULL)
        return BOXART_ROM_TYPE_32X;
    //Genesis/Megadrive
    if (strstr("smd,zsg", file_ext) != NULL) {
        return BOXART_ROM_TYPE_GENESIS;
    }
    //NeoGeo
    if (strstr("ngp", file_ext) != NULL)
        return BOXART_ROM_TYPE_NEOGEO;
    //SMS
    if (strstr("sms", file_ext) != NULL)
        return BOXART_ROM_TYPE_SMS;
    //C64
    if (strstr("c64", file_ext) != NULL)
        return BOXART_ROM_TYPE_C64;
    //NES
    if (strstr("nes", file_ext) != NULL)
        return BOXART_ROM_TYPE_NES;
    //SNES
    if (strstr("smc,sfc", file_ext) != NULL)
        return BOXART_ROM_TYPE_SNES;
    //N64
    if (strstr("n64,v64", file_ext) != NULL)
        return BOXART_ROM_TYPE_N64;
    //NDS
    if (strstr("nds", file_ext) != NULL)
        return BOXART_ROM_TYPE_NDS;
    //Amiga
    if (strstr("pp,adf,dms,hdf,izx,whd,dhf,hdz", file_ext) != NULL)
        return BOXART_ROM_TYPE_N64;
    
    
    return BOXART_ROM_TYPE_NONE;
}


static void boxart_fill_rom_directory_path(char *out_path, boxart_rom_type_t rom_type, size_t size)
{
    switch(rom_type)
    {
        case BOXART_ROM_TYPE_32X:
            strlcat(out_path, "/32X", size);
            break;
            
        case BOXART_ROM_TYPE_ATOMISWAVE:
            strlcat(out_path, "/atomiswave", size);
            break;
            
        case BOXART_ROM_TYPE_GENESIS:
            strlcat(out_path, "/genesis", size);
            break;
            
        case BOXART_ROM_TYPE_NEOGEO:
            strlcat(out_path, "/neogeo", size);
            break;
            
        case BOXART_ROM_TYPE_SMS:
            strlcat(out_path, "/sms", size);
            break;
            
        case BOXART_ROM_TYPE_MSX:
            strlcat(out_path, "/MSX", size);
            break;
            
        case BOXART_ROM_TYPE_C64:
            strlcat(out_path, "/c64", size);
            break;
            
        case BOXART_ROM_TYPE_MEGADRIVE:
            strlcat(out_path, "/megadrive", size);
            break;
            
        case BOXART_ROM_TYPE_NES:
            strlcat(out_path, "/nes", size);
            break;
            
        case BOXART_ROM_TYPE_SNES:
            strlcat(out_path, "/snes", size);
            break;
            
        case BOXART_ROM_TYPE_DREAMCAST:
            strlcat(out_path, "/dreamcast", size);
            break;
            
        case BOXART_ROM_TYPE_N64:
            strlcat(out_path, "/n64", size);
            break;
            
        case BOXART_ROM_TYPE_PS1:
            strlcat(out_path, "/ps1", size);
            break;
            
        case BOXART_ROM_TYPE_ST:
            strlcat(out_path, "/st", size);
            break;
            
        case BOXART_ROM_TYPE_AMIGA:
            strlcat(out_path, "/amiga", size);
            break;
            
        case BOXART_ROM_TYPE_GAMECUBE:
            strlcat(out_path, "/gamecube", size);
            break;
            
        case BOXART_ROM_TYPE_NAOMI:
            strlcat(out_path, "/naomi", size);
            break;
            
        case BOXART_ROM_TYPE_SEGACD:
            strlcat(out_path, "/sagacd", size);
            break;
            
        case BOXART_ROM_TYPE_TURBODUO:
            strlcat(out_path, "/turboduo", size);
            break;
            
        case BOXART_ROM_TYPE_NDS:
            strlcat(out_path, "/nds", size);
            break;
            
        default:
            //Do nothing here
            break;
    }
}

static void boxart_fill_cover_directory_path(char *out_path, boxart_cover_type_t cover_type, boxart_rom_type_t rom_type, size_t size) {
    
    switch(cover_type)
    {
        case BOXART_COVER_TYPE_BOXFRONT:
            strlcat(out_path, "/boxfront", size);
            break;
            
        case BOXART_COVER_TYPE_BOXBACK:
            strlcat(out_path, "/boxback", size);
            break;
            
        case BOXART_COVER_TYPE_CONTENT:
            strlcat(out_path, boxart_get_cover_content_dir_for_rom_type(rom_type), size);
            break;
            
        default:
            //Do nothing here
            break;
    }
}

static char* boxart_get_cover_content_dir_for_rom_type(boxart_rom_type_t rom_type)
{
    switch(rom_type)
    {
        case BOXART_ROM_TYPE_32X:
        case BOXART_ROM_TYPE_ATOMISWAVE:
        case BOXART_ROM_TYPE_GENESIS:
        case BOXART_ROM_TYPE_NEOGEO:
        case BOXART_ROM_TYPE_SMS:
        case BOXART_ROM_TYPE_MEGADRIVE:
        case BOXART_ROM_TYPE_NES:
        case BOXART_ROM_TYPE_SNES:
        case BOXART_ROM_TYPE_N64:
        case BOXART_ROM_TYPE_GAMECUBE:
        case BOXART_ROM_TYPE_NAOMI:
            return "/cartridge";
            
        case BOXART_ROM_TYPE_AMIGA:
        case BOXART_ROM_TYPE_C64:
        case BOXART_ROM_TYPE_DREAMCAST:
        case BOXART_ROM_TYPE_MSX:
        case BOXART_ROM_TYPE_PS1:
        case BOXART_ROM_TYPE_SEGACD:
        case BOXART_ROM_TYPE_ST:
        case BOXART_ROM_TYPE_TURBODUO:
            return "/disk";
          
        default:
            return "";
    }
}

static int boxart_levenshtein(char *s1, char *s2) {
    unsigned int x, y, s1len, s2len;
    s1len = strlen(s1);
    s2len = strlen(s2);
    unsigned int matrix[s2len+1][s1len+1];
    matrix[0][0] = 0;
    for (x = 1; x <= s2len; x++)
        matrix[x][0] = matrix[x-1][0] + 1;
    for (y = 1; y <= s1len; y++)
        matrix[0][y] = matrix[0][y-1] + 1;
    for (x = 1; x <= s2len; x++)
        for (y = 1; y <= s1len; y++)
            matrix[x][y] = BOXART_MIN3(matrix[x-1][y] + 1, matrix[x][y-1] + 1, matrix[x-1][y-1] + (s1[y-1] == s2[x-1] ? 0 : 1));
    
    return(matrix[s2len][s1len]);
}

static void boxart_format_name(char *str) {
    //Trim white spaces
    boxart_trimwhitespace(str);
    //Lowercase
    for (int i = 0; i < strlen(str); ++i)
        str[i] = tolower(str[i]);
    //Remove file extension
    path_remove_extension(str);
    //Remove bracket contents (ie: "Mario (REV3) Test Edition" => "Mario Test Edition"
    boxart_remove_brackets_content(str);
}

static char *boxart_trimwhitespace(char *str)
{
    char *end;
    
    // Trim leading space
    while(isspace(*str)) str++;
    
    if(*str == 0)  // All spaces?
        return str;
    
    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace(*end)) end--;
    
    // Write new null terminator
    *(end+1) = 0;
    
    return str;
}

static void boxart_remove_brackets_content(char *str)
{
    int size = strlen(str);
    
    int stack[size];
    int stack_cursor = 0;
    int str_cursor = 0;
    
    int i;
    for(i=0; (str[i] != '\0'); ++i)
    {
        if(str[i] == '(')
        {
            stack[stack_cursor] = str_cursor;
            ++stack_cursor;
            continue;
        }
        
        if(str[i] == ')')
        {
            if(stack_cursor > 0)
            {
                --stack_cursor;
                str_cursor = stack[stack_cursor];
                continue;
            }
        }
        
        str[str_cursor] = str[i];
        ++str_cursor;
    }
    
    str[str_cursor] = '\0';
}