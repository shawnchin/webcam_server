/*********************************************************************
 * Additional filters for webcam_server                              *
 *                                                                   *
 * (c) 2011 Shawn Chin (http://shawnchin.github.com)                 *
 *                                                                   *
 *********************************************************************/

#ifndef _X_FILTERS_H_INCLUDED_
#define _X_FILTERS_H_INCLUDED_

enum LUMA_MODEL { HSV, YUV, AVG };

void x_autoscale(struct image* img);
void x_histequal(struct image* img, enum LUMA_MODEL mode);
void x_greyscale(struct image* img);
void x_sepia(struct image* img);

#endif
