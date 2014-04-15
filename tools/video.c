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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "video.h"

#define CMD_SIZE                512
#define PIXELBUF_SIZE           20

static int get_video_resolution(const char *file, int *x, int *y)
{
        FILE *pipe;
        char cmd[CMD_SIZE];
        char line[PIXELBUF_SIZE];
        int cmdsize;

        /* ffprobe is part of the ffmpeg package. It prints the metadata of
         * the video. egrep filters the width and height and cut leaves only
         * the numbers divided by endline. */
        cmdsize = snprintf(cmd, sizeof(cmd), "ffprobe -loglevel error "
                        "-show_streams %s | egrep 'width|height' | "
                        "cut -d= -f2", file);

        if (cmdsize == -1 || cmdsize > ((int) sizeof(cmd))) {
                fprintf(stderr, "The video path is too long!\n");
                return 0;
        }

        if (!(pipe = (FILE *) popen(cmd, "r"))) {
                perror("Cannot open pipe");
                return 0;
        }

        if (!fgets(line, sizeof(line), pipe) ||
                        (*x = (int) strtol(line, NULL, 10)) == 0) {
                perror("Cannot determine the width");
                return 0;
        }

        if (!fgets(line, sizeof(line), pipe) ||
                        (*y = (int) strtol(line, NULL, 10)) == 0) {
                perror("Cannot determine the height");
                return 0;
        }

        pclose(pipe);

        return 1;
}

int video_input_pipe(const char *input, const char *output, int x, int y,
                int *finalx, int *finaly, pipe_processor pproc,
                frame_callback callback)
{
        int inputfd[2];
        pid_t inputchild;
        int origx, origy;

        if (!get_video_resolution(input, &origx, &origy))
                return EXIT_FAILURE;

        *finalx = (x == NOSCALE) ? (y == NOSCALE ? origx
                        : roundf((origx *
                                        (float) y)/origy))
                : x;
        *finaly = (y == NOSCALE) ? (x == NOSCALE ? origy
                        : roundf((origy *
                                        (float) x)/origx))
                : y;

        if (pipe(inputfd) == -1) {
                perror("Input pipe cannot be created");
                return EXIT_FAILURE;
        }

        if ((inputchild = fork()) == -1) {
                perror("Input fork cannot be established");
                return EXIT_FAILURE;
        }

        if (inputchild == 0) {
                int bufn;
                char scale[3*PIXELBUF_SIZE];
                /* 3 = 2 sizes with plus overhead */

                close(inputfd[0]);      /* pipe input closed */

                bufn = snprintf(scale, sizeof(scale), "scale=%d:%d", *finalx,
                                *finaly);

                if (bufn == -1 || bufn > ((int) sizeof(scale))) {
                        fprintf(stderr, "Scale buffer is too small!\n");
                        _exit(EXIT_FAILURE);
                }

                /* close stdout and duplicate pipe output into it */
                if (dup2(inputfd[1], STDOUT_FILENO) == -1) {
                        perror("Dup at the input side failed");
                        _exit(EXIT_FAILURE);
                }

                execlp("ffmpeg", "ffmpeg",
                                "-i", input,
                                "-f", "image2pipe",
                                "-pix_fmt", "gray", /* grayscale conversion */
                                "-vcodec", "rawvideo",
                                "-vf", scale,
                                "-", /* it will write to pipe */
                                (const char *) NULL);

                perror("Cannot run ffmpeg for input generation");
                _exit(EXIT_FAILURE);
        } else {
                pipe_processor_arg arg;

                close(inputfd[1]);      /* pipe output closed */

                arg.infd = inputfd[0];
                arg.output = output;
                arg.x = *finalx;
                arg.y = *finaly;
                arg.callback = callback;

                if (!(*pproc)(arg)) {
                        wait(NULL);     /* wait for the child */
                        return EXIT_FAILURE;
                }

                close(inputfd[0]);
                wait(NULL);     /* wait for the child */
        }

        return EXIT_SUCCESS;
}

int video_output_pipe(pipe_processor_arg arg)
{
        int outputfd[2];
        pid_t outputchild;

        if (pipe(outputfd) == -1) {
                perror("Output pipe cannot be created");
                return EXIT_FAILURE;
        }

        if ((outputchild = fork()) == -1) {
                perror("Output fork cannot be established");
                return EXIT_FAILURE;
        }

        if (outputchild == 0) {
                int bufn;
                char pix[3*PIXELBUF_SIZE]; /* 3 = 2 sizes with plus overhead */

                close(outputfd[1]);      /* pipe output closed */

                /* the output will contain 4 frames */
                bufn = snprintf(pix, sizeof(pix), "%dx%d", arg.x << 1,
                                arg.y << 1);

                if (bufn == -1 || bufn > ((int) sizeof(pix))) {
                        fprintf(stderr, "Pixel buffer is too small!\n");
                        _exit(EXIT_FAILURE);
                }

                /* close stdin and duplicate pipe input into it */
                if (dup2(outputfd[0], STDIN_FILENO) == -1) {
                        perror("Dup at the output side failed");
                        _exit(EXIT_FAILURE);
                }

                execlp("ffmpeg", "ffmpeg",
                                "-y", /* overwrite output */
                                "-f", "rawvideo",
                                "-vcodec", "rawvideo",
                                "-s", pix,
                                "-pix_fmt", "gray",
                                "-i", "-",
                                "-an", /* disable audio */
                                "-vcodec", "rawvideo",
                                arg.output,
                                (const char *) NULL);

                perror("Cannot run ffmpeg for output generation");
                _exit(EXIT_FAILURE);
        } else {
                FILE *instream;
                frm_t *frmin, *frmjoined;
                const int frmsize = arg.x * arg.y;
                const int frmjoinsize = frmsize << 2; /* 4 frames */

                close(outputfd[0]);      /* pipe input closed */

                frmin = allocate_frm(frmsize);
                frmjoined = allocate_frm(frmjoinsize);

                if (!frmin || !frmjoined) {
                        fprintf(stderr, "Cannot allocate frames!\n");
                        exit(EXIT_FAILURE);
                }

                instream = fdopen(arg.infd, "r");

                if (!instream) {
                        perror("Cannot read frame from the pipe");
                } else {
                        while (read_frm(frmin, frmsize, instream)) {

                                (*arg.callback)(frmin, frmsize, arg.x, arg.y,
                                                frmjoined);

                                if (write_frm(outputfd[1], frmjoined,
                                                frmjoinsize) != frmjoinsize) {
                                        fprintf(stderr, "Cannot write frame "
                                                        "to output pipe!\n");
                                        break;
                                }
                        }

                        fclose(instream);
                }

                deallocate_frm(frmin);
                deallocate_frm(frmjoined);

                close(outputfd[1]);
                wait(NULL);     /* wait for the child */
        }

        return EXIT_SUCCESS;
}

int raw_video_output(pipe_processor_arg arg)
{
        int outputfd[2];
        pid_t outputchild;

        if (pipe(outputfd) == -1) {
                perror("Output pipe cannot be created");
                return EXIT_FAILURE;
        }

        if ((outputchild = fork()) == -1) {
                perror("Output fork cannot be established");
                return EXIT_FAILURE;
        }

        if (outputchild == 0) {
                int bufn;
                char pix[3*PIXELBUF_SIZE]; /* 3 = 2 sizes with plus overhead */
                char vid[CMD_SIZE];

                close(outputfd[1]);      /* pipe output closed */

                bufn = snprintf(pix, sizeof(pix), "%dx%d", arg.x, arg.y);

                if (bufn == -1 || bufn > ((int) sizeof(pix))) {
                        fprintf(stderr, "Pixel buffer is too small!\n");
                        _exit(EXIT_FAILURE);
                }

                bufn = snprintf(vid, sizeof(vid), "%s.y4m", arg.output);

                if (bufn == -1 || bufn > ((int) sizeof(vid))) {
                        fprintf(stderr, "Path buffer is too small!\n");
                        _exit(EXIT_FAILURE);
                }

                /* close stdin and duplicate pipe input into it */
                if (dup2(outputfd[0], STDIN_FILENO) == -1) {
                        perror("Dup at the output side failed");
                        _exit(EXIT_FAILURE);
                }

                execlp("ffmpeg", "ffmpeg",
                                "-y", /* overwrite output */
                                "-f", "rawvideo",
                                "-vcodec", "rawvideo",
                                "-s", pix,
                                "-pix_fmt", "gray",
                                "-i", "-",
                                "-an", /* disable audio */
                                "-vcodec", "rawvideo",
                                vid,
                                (const char *) NULL);

                perror("Cannot run ffmpeg for output generation");
                _exit(EXIT_FAILURE);
        } else {
                FILE *instream, *outfile;
                frm_t *frmin, *frmout;
                const int frmsize = arg.x * arg.y;

                close(outputfd[0]);      /* pipe input closed */

                frmin = allocate_frm(frmsize);
                frmout = allocate_frm(frmsize);

                if (!frmin || !frmout) {
                        fprintf(stderr, "Cannot allocate frames!\n");
                        exit(EXIT_FAILURE);
                }

                instream = fdopen(arg.infd, "r");

                if (!instream) {
                        perror("Cannot read frame from the pipe");
                } else if (!(outfile = fopen(arg.output, "wb"))) {
                        perror("Cannot open output file");
                } else {
                        while (read_frm(frmin, frmsize, instream)) {

                                (*arg.callback)(frmin, frmsize, arg.x, arg.y,
                                                frmout);

                                if (write_frm(outputfd[1], frmout,
                                                frmsize) != frmsize) {
                                        fprintf(stderr, "Cannot write frame "
                                                        "to output pipe!\n");
                                        break;
                                }

                                if (fwrite(frmout, sizeof(frm_t), frmsize,
                                                outfile) !=
                                                ((size_t) frmsize)) {
                                        fprintf(stderr, "Cannot write frame "
                                                        "to output file!\n");
                                        break;
                                }
                        }

                        fclose(instream);
                        fclose(outfile);
                }

                deallocate_frm(frmin);
                deallocate_frm(frmout);

                close(outputfd[1]);
                wait(NULL);     /* wait for the child */
        }

        return EXIT_SUCCESS;
}
