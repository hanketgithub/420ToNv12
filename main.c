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
struct timeval tv1, tv2;
#endif

#define MAX_WIDTH   3840
#define MAX_HEIGHT  2160


typedef struct
{
    char name[256];
} string_t;


static uint8_t img[MAX_WIDTH * MAX_HEIGHT * 3 / 2];
static uint8_t u_et_v[MAX_WIDTH * MAX_HEIGHT / 2];

static string_t null;

int main(int argc, const char * argv[]) {
    int fd_rd;
    int fd_wr;
    
    uint8_t *y;
    uint8_t *u;
    uint8_t *v;
    ssize_t rd_sz;
    
    uint32_t width;
    uint32_t height;
    uint32_t wxh;
    
    char *cp;
    string_t output;
    
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
    output      = null;


    // get input file name from comand line
    fd_rd = open(argv[1], O_RDONLY);
    if (fd_rd < 0)
    {
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }
    
    // specify output file name
    cp = strchr(argv[1], '.');
    strncpy(output.name, argv[1], cp - argv[1]);
    strcat(output.name, "_nv12");
    strcat(output.name, cp);
    
    fd_wr = open
            (
             output.name,
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
        rd_sz = read(fd_rd, img, wxh * 3 / 2);
        
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
            fprintf(stderr, "Total time = %f seconds\n",
            (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
            (double) (tv2.tv_sec - tv1.tv_sec));
            #endif
            
            write(fd_wr, y, wxh);
            write(fd_wr, u_et_v, wxh / 2);
        }
        else
        {
            break;
        }
        fputc('.', stderr);
        fflush(stderr);
    }

    close(fd_rd);
    close(fd_wr);
    
    fprintf(stderr, "Done\n");
    fprintf(stderr, "Output file: %s\n", output.name);
    
    return 0;
}
