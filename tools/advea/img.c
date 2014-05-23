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

#include <stdlib.h>
#include "img.h"

img_t *allocate_img(int size)
{
        return ((img_t *) malloc(size * sizeof(img_t)));
}

void deallocate_img(img_t *img)
{
        free(img);
}

void update_img(img_t *img, int i, img_t val)
{
        if (img != NULL)
                img[i] = val;
}

img_t get_img(const img_t *img, int i)
{
        return img[i];
}
