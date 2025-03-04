//
//  main.c
//  420ToNv12
//
//  Created by Hank Lee on 2025/03/05.
//  Copyright (c) 2025 Hank Lee. All rights reserved.
//

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>

#include "420ToNv12.h"

#define BENCHMARK  1
#if (BENCHMARK)
struct timeval tv1, tv2, res;
#endif

#define MAX_WIDTH   7680
#define MAX_HEIGHT  4320


static uint8_t img[MAX_WIDTH * MAX_HEIGHT * 3 / 2];
static uint8_t u_et_v[MAX_WIDTH * MAX_HEIGHT / 2];

int main(int argc, const char * argv[]) {
    int ifd;
    int ofd;

    uint8_t *y;
    uint8_t *u;
    uint8_t *v;
    ssize_t rd_sz;

    uint32_t width;
    uint32_t height;
    uint32_t wxh;

    char *cp;
    char output[256] = { 0 };

    uint32_t frame_cnt;
    struct timeval tv_start = { 0 };
    struct timeval tv_end = { 0 };
    struct timeval tv_interval = { 0 };

    if (argc < 4)
    {
        fprintf(stderr, "usage: %s [input file] [width] [height]\n", argv[0]);
        
        return -1;
    }


    rd_sz       = 0;
    width       = 0;
    height      = 0;
    wxh         = 0;
    cp          = NULL;
    frame_cnt   = 0;


    // get input file name from comand line
    ifd = open(argv[1], O_RDONLY);
    if (ifd < 0)
    {
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    // specify output file name
    cp = strrchr(argv[1], '.');
    strncpy(output, argv[1], cp - argv[1]);
    strcat(output, "_nv12");
    strcat(output, cp);

    ofd = open
            (
             output,
             O_WRONLY | O_CREAT | O_TRUNC,
             S_IRUSR
            );

    width   = atoi(argv[2]);
    height  = atoi(argv[3]);

    wxh = width * height;

    y = img;
    u = y + wxh;
    v = u + wxh / 4;

    fprintf(stderr, "Processing: ");

    gettimeofday(&tv_start, NULL);
    while (1)
    {
        rd_sz = read(ifd, img, wxh * 3 / 2);
        if (rd_sz == wxh * 3 / 2)
        {
            #if (BENCHMARK)
            gettimeofday(&tv1, NULL);
            #endif

            planar_to_interleave
            (
                wxh,
                u_et_v,
                u,
                v
            );

            #if (BENCHMARK)
            gettimeofday(&tv2, NULL);
            timersub(&tv2, &tv1, &res);

            fprintf(stderr, "Total time = %ld seconds %4d microsecs\n",
                    res.tv_sec, res.tv_usec);
            #endif

            write(ofd, y, wxh);
            write(ofd, u_et_v, wxh / 2);

            frame_cnt++;
        }
        else
        {
            break;
        }
        fputc('.', stderr);
        fflush(stderr);
    }
    gettimeofday(&tv_end, NULL);
    timersub(&tv_end, &tv_start, &tv_interval);

    close(ifd);
    close(ofd);

    fprintf(stderr, "Done\n");
    fprintf(stderr, "Output file: %s\n", output);

    double interval_in_double = ((double) tv_interval.tv_sec * 1000000 + tv_interval.tv_usec) / 1000000;
    fprintf(stderr, "Performance: %f frames per second\n", frame_cnt / interval_in_double);

    return 0;
}
