/*! \file image.h
 *  \brief header for image declare image struct
 *  \date Jan 1, 2019
 *  \author Valentin Platzgummer
*/

/*!
 * \addtogroup group_oled_res
 *! @{
*/
#ifndef RESOURCES_IMAGE_H_
#define RESOURCES_IMAGE_H_

//! \brief image struct for use with oled 96x96
typedef const struct image {
    unsigned char       width;      ///< width in pixels of image
    unsigned char       height;     ///< height in pixels of image
    unsigned char       bytes_per_pixel; ///< Bytes per pixels 2:RGB16, 3:RGB, 4:RGBA */
    char          *comment;         ///< name of image
    unsigned char *pixel_data;      ///< image data ascii format octal, coded bitmap
} image;
#endif /* RESOURCES_IMAGE_H_ */

//! \brief Image of Joe Cool, very cool! (Could be the shut down image?)
extern const image cool_image;
//!s \brief Starting logo, just refine it
extern const image logo_image ;

// Close the Doxygen group.
//! @}
