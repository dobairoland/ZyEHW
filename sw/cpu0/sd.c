/*
 * Copyright (C) 2014 Roland Dobai
 *
 * This file is part of ZyEHW.
 *
 * ZyEHW is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * ZyEHW is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along
 * with ZyEHW. If not, see <http://www.gnu.org/licenses/>.
 */

#include "sd.h"
#include "ff.h"
#include "xil_printf.h"

#define LOGIC_DRIVE ""
#define MOUNT_IMMEDIATELY 1

static FIL file;
static FATFS fs;

int open_sd_file(const char *name)
{
        FRESULT res;

        if ((res = f_mount(&fs, LOGIC_DRIVE, MOUNT_IMMEDIATELY)) != FR_OK) {
                xil_printf("Cannot mount SD! (error: %d)\n\r", res);
                return 0;
        }

        if ((res = f_open(&file, name, FA_READ)) != FR_OK) {
                xil_printf("Cannot open SD file! (error: %d)\n\r", res);
                return 0;
        }

        return 1;
}

u32 read_sd_file(u8 *dst, u32 len)
{
        FRESULT res;
        UINT num;

        if ((res = f_read(&file, (void *) dst, len, &num)) != FR_OK) {
                xil_printf("Cannot read SD file! (error: %d)\n\r", res);
                return 0;
        }

        return num;
}

void close_sd_file()
{
        f_close(&file);
        f_mount(NULL, LOGIC_DRIVE, MOUNT_IMMEDIATELY); /* NULL is unmount */
}
