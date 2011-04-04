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


unsigned char clamp(float v) {
    if (v > 255) return 255;
    else if (v < 0) return 0;
    else return (unsigned char)v;
}

static void RGBtoYUV(struct image* img) {
    int i, offset, elems = img->bufsize/3;
    unsigned char* buf = img->buf;
    unsigned char R,G,B;
    
    for (i = 0; i < elems; i++) {
        offset = i * 3;
        R = buf[offset]; G = buf[offset+1]; B = buf[offset+2];
        buf[offset] = clamp(0.299*R + 0.587*G + 0.114*B);
        buf[offset+1] = clamp(-0.14713*R - 0.28886*G + 0.436*B);
        buf[offset+2] = clamp(0.615*R - 0.51499*G - 0.10001*B);
    }
}

static void YUVtoRGB(struct image* img) {
    int i, offset, elems = img->bufsize/3;
    unsigned char* buf = img->buf;
    unsigned char Y,U,V;
    
    for (i = 0; i < elems; i++) {
        offset = i * 3;
        Y = buf[offset]; U = buf[offset+1]; V = buf[offset+2];
        buf[offset] = clamp(Y + 1.13983*V);
        buf[offset+1] = clamp(Y - 0.39465*U - 0.58060*V);
        buf[offset+2] = clamp(Y + 2.03211*U);
    }
}

/* returns max of R,G,B */
static unsigned char get_brightness(unsigned char pixel[3]){
    unsigned char v;
    v = (pixel[0] > pixel[1]) ? pixel[0] : pixel[1];
    return (v > pixel[2]) ? v : pixel[2];
}

/*
 * To set a new brightness value, we'll need to first work out the HSV
 * representation, update V, then convert it back to RGB.
 */
static void update_brightness(unsigned char pixel[3], int new_v) {
    unsigned char r = pixel[0];
    unsigned char g = pixel[1];
    unsigned char b = pixel[2];
    unsigned char p, q, t, v, min;
    float f, h, s, delta;
    int sector;
    
    /* brightness, V */
    v = get_brightness(pixel);

    /* get min value */
    min = (r < g) ? r : g;
    min = (min < b) ? min : b;
    
    if (v == min) { /* acromatic : r = g = b */
        pixel[0] = new_v;
        pixel[1] = new_v;
        pixel[2] = new_v;
        return;
    }
    
    /* saturation, S */
    delta = (float)(v - min);
    s = delta / v;
    
    /* hue, H but factor out 60 to simplify sector computation */
    if (v == r) {
        h = (g - b)/delta;
        h = (h < 0) ? h + 6 : h; /* wrap round */
    } else if (v == g) {
        h = 2 + (b - r)/delta;
    } else {
        h = 4 + (r - g)/delta;
    }

    /* now convert back to RGB using new_v */
    sector = (int)h;
    f = h - sector;
    v = new_v;
    p = (unsigned char)(v * (1 - s));
    q = (unsigned char)(v * (1 - s*f));
    t = (unsigned char)(v * (1 - s*(1 - f)));
    
    switch (sector) {
        case 0:
            pixel[0] = v; pixel[1] = t; pixel[2] = p;
            break;
        case 1:
            pixel[0] = q; pixel[1] = v; pixel[2] = p;
            break;
        case 2:
            pixel[0] = p; pixel[1] = v; pixel[2] = t;
            break;
        case 3:
            pixel[0] = p; pixel[1] = q; pixel[2] = v;
            break;
        case 4:
            pixel[0] = t; pixel[1] = p; pixel[2] = v;
            break;
        default:
            pixel[0] = v; pixel[1] = p; pixel[2] = q;
            break;
    }
}

/* histogram equalisation 
 *                 cdf(v) - cdf_min 
 * h(v) = round ( ------------------  * 255 )
 *                  size - cdf_min  
 */

void x_histequal(struct image* img) {
    int i, elems = img->bufsize/3;
    int cdf_min;
    unsigned int offset;
    float scale;
    unsigned char v;
    unsigned char *buf = img->buf;
    int hist[256];

    /* build histogram using Y component */
    for (i = 0; i < 256; i++) hist[i] = 0;
    for (i = 0; i < elems; i++) {
        offset = i * 3;
        v = get_brightness(buf + offset);
        hist[(int)v]++; 
    }

    /* convert to cumulative distribution function (cdf) */
    for (i = 1; i < 256; i++) { hist[i] = hist[i] + hist[i-1]; }
    
    /* determine cdf min */
    for (i = 0; i < 256; i++) { 
        if (hist[i] != 0) {
            cdf_min = hist[i];
            break;
        }
    }

    /* convert cdf to lookup table */
    for (i = 0; i < 256; i++) { 
        hist[i] = ((double)(hist[i] - cdf_min) / (elems - cdf_min)) * 255;
    }

    /* map to new values of Y */
    for (i = 0; i < elems; i++) {
        offset = i*3;
        v = get_brightness(buf + offset);
        update_brightness(buf + offset, hist[(int)v]);
    }
    
}

/* autoscaling 
 * - set lightest value as white, and darkest as black,
 *   use linear scale for everything else in between.
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

/* convert to greyscale image */
void x_greyscale(struct image* img) {
    int i, offset, elems = img->bufsize/3;
    unsigned char *buf = img->buf;
    for (i = 0; i < elems; i++) {
        offset = i*3;
        buf[offset] = clamp( /* use Y component of YUV conversion */
                        0.299*buf[offset] 
                        + 0.587*buf[offset+1] 
                        + 0.114*buf[offset+2]);
        buf[offset+1] = buf[offset]; 
        buf[offset+2] = buf[offset]; 
    }
}

/* Sepia filter */
void x_sepia(struct image* img) {
    int i;
    unsigned int r, g, b, v;
    int elems = img->bufsize / 3;
    unsigned char *buf = img->buf;
    for (i = 0; i < elems; i++) {
        r = RED(buf, i); g = GREEN(buf, i); b = BLUE(buf, i);
        v = (unsigned int)(r*0.393 + g*0.769 + b*0.189);
        RED(buf, i) = (v > 255) ? 255: v;
        v = (unsigned int)(r*0.349 + g*0.686 + b*0.168);
        GREEN(buf, i) = (v > 255) ? 255: v;
        BLUE(buf, i) = (unsigned int)(r*0.272 + g*0.534 + b*0.131);
    }
}

