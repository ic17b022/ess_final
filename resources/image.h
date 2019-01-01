/*
 * image.h
 *
 *  Created on: Jan 1, 2019
 *      Author: marcaurel
 */

#ifndef RESOURCES_IMAGE_H_
#define RESOURCES_IMAGE_H_

typedef const struct image {
    unsigned char       width;
    unsigned char       height;
    unsigned char       bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
    char          *comment;
    unsigned char *pixel_data;
} image;
#endif /* RESOURCES_IMAGE_H_ */

// Image of Joe Cool, very cool! (Could be the shut down image?)
extern const image cool_image;
// Starting logo, just refine it
extern const image logo_image ;
