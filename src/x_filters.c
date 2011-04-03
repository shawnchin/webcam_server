/*********************************************************************
* Additional filters for webcam_server                              *
*                                                                   *
* (c) 2011 Shawn Chin (http://shawnchin.github.com)                 *
*                                                                   *
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <linux/videodev.h>

#include "webcam_server.h"
#include "x_filters.h"

#define RED(buf, elem)   buf[elem*3]
#define GREEN(buf, elem) buf[elem*3 + 1]
#define BLUE(buf, elem)  buf[elem*3 + 2]

/* autoscaling - set lightest value as white, and darkest as black 
 *    range, R = max - min + 1
 *    scale, S = 256 / R
 *    with original value, U
 *    new value, V = (U - min) * S
 */
void x_autoscale(struct image* img) {
    int i;
    float scale;
    int size = img->bufsize;
    unsigned char *buf = img->buf;
    unsigned int min=255, max=0;

    for (i = 0; i < size; i++) {
        if (buf[i] < min) min = buf[i];
        if (buf[i] > max) max = buf[i];
    }

    scale = 256.0 / (max - min + 1);
    for (i = 0; i < size; i++) {
        buf[i] = (buf[i] - min) * scale;
    }
}

/* convert to greyscale image 
 * R - 30%, G - 59%, B - 11%
 */
void x_greyscale(struct image* img) {
    int i, R, G, B;
    unsigned int v;
    int elems = img->bufsize / 3;
    unsigned char *buf = img->buf;
    for (i = 0; i < elems; i++) {
        v = (unsigned int)(RED(buf, i) * 0.3 + GREEN(buf, i) * 0.59 + BLUE(buf, i) * 0.11);
        RED(buf, i) = v;
        GREEN(buf, i) = v;
        BLUE(buf, i) = v;
    }
}

