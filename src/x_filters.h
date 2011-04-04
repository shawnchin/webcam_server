/*********************************************************************
 * Additional filters for webcam_server                              *
 *                                                                   *
 * (c) 2011 Shawn Chin (http://shawnchin.github.com)                 *
 *                                                                   *
 *********************************************************************/

#ifndef _X_FILTERS_H_INCLUDED_
#define _X_FILTERS_H_INCLUDED_

void x_autoscale(struct image* img);
void x_histequal(struct image* img);
void x_greyscale(struct image* img);
void x_sepia(struct image* img);

#endif
