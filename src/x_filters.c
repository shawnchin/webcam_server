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
    int min=255, max=0;

    for (i = 0; i < size; i++) {
        if (buf[i] < min) min = buf[i];
        if (buf[i] > max) max = buf[i];
    }

    scale = 256.0 / (max - min + 1);
    for (i = 0; i < size; i++) {
        buf[i] = (buf[i] - min) * scale;
    }
}

/*
void adjust_gamma(struct image* img, int corr)
{
	int i=img->bufsize;
	char *p = img->buf;
	while(--i)
	{
		if(corr > 0)
		{
			if(p[0] + corr <= 255)
				p[0] += corr;
			else
				p[0] = 255;
		}
		else
		{
			if(p[0] + corr >= 0)
				p[0] += corr;
			else
				p[0] = 0;
		}
		p++;
	}
}
*/
