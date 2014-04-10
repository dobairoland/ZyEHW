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

#ifndef VIDEO_H
#define VIDEO_H

#include "frm.h"

#define NOSCALE         (-1)

/* Callback used to process the frames.
 * In vidprocsim this makes the joined frame for comparing the results.
 * In testvid this applies only the noise */
typedef void (*frame_callback)(const frm_t *frmin, int insize, int x, int y,
                frm_t *frmout);

typedef struct {
        int infd;
        const char *output;
        int x;
        int y;
        frame_callback callback;
} pipe_processor_arg;

/* Pointer to function used to determine how the input video will be further
 * processed */
typedef int (*pipe_processor)(pipe_processor_arg);

/* Reads the input video then process it further by pproc. pproc will use the
 * callback for frame processing. */
extern int video_input_pipe(const char *input, const char *output,
                int x, int y,
                int *finalx, int *finaly,
                pipe_processor pproc,
                frame_callback callback);

/* Two alternative solutions:
 * video_output_pipe - will generate video for comparing the results
 * raw_video_output - will just insert the noise and make raw output */
extern int video_output_pipe(pipe_processor_arg arg);
extern int raw_video_output(pipe_processor_arg arg);

#endif
