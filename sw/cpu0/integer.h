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

/*-------------------------------------------*/
/* Integer type definitions for FatFs module */
/*-------------------------------------------*/

#ifndef _FF_INTEGER
#define _FF_INTEGER

#ifdef _WIN32   /* FatFs development platform */

#include <windows.h>
#include <tchar.h>

#else           /* Embedded platform */

/* This type MUST be 8 bit */
typedef unsigned char   BYTE;

/* These types MUST be 16 bit */
typedef short           SHORT;
typedef unsigned short  WORD;
typedef unsigned short  WCHAR;

/* These types MUST be 16 bit or 32 bit */
typedef int             INT;
typedef unsigned int    UINT;

/* These types MUST be 32 bit */
typedef long            LONG;
typedef unsigned long   DWORD;

#endif

#endif
