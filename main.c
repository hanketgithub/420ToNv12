//
//  main.c
//  420ToNv12
//
//  Created by Hank Lee on 5/31/15.
//  Copyright (c) 2015 Hank Lee. All rights reserved.
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

#define TIMING  1
#if (TIMING)
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
    
    if (argc < 4)
    {
        fprintf(stderr, "useage: %s [input_file] [width] [height]\n", argv[0]);
        
        return -1;
    }
    
    
    rd_sz       = 0;
    width       = 0;
    height      = 0;
    wxh         = 0;
    cp          = NULL;


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
    
    while (1)
    {
        rd_sz = read(ifd, img, wxh * 3 / 2);
        
        if (rd_sz == wxh * 3 / 2)
        {
            #if (TIMING)
            gettimeofday(&tv1, NULL);
            #endif
            
            planar_to_interleave
            (
                wxh,
                u_et_v,
                u,
                v
            );
            
            #if (TIMING)
            gettimeofday(&tv2, NULL);
            timersub(&tv2, &tv1, &res);
            
            fprintf(stderr, "Total time = %d seconds %d microsecs\n",
                    res.tv_sec, res.tv_usec);
            #endif
            
            write(ofd, y, wxh);
            write(ofd, u_et_v, wxh / 2);
        }
        else
        {
            break;
        }
        fputc('.', stderr);
        fflush(stderr);
    }

    close(ifd);
    close(ofd);
    
    fprintf(stderr, "Done\n");
    fprintf(stderr, "Output file: %s\n", output);
    
    return 0;
}
