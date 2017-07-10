#ifndef JO_INCLUDE_JPEG_H
#define JO_INCLUDE_JPEG_H

// To get a header file for this, either cut and paste the header,
// or create jo_jpeg.h, #define JO_JPEG_HEADER_FILE_ONLY, and
// then include jo_jpeg.c from it.

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
  
// Returns false on failure
bool jo_write_jpg(const char *filename, const void *data, int width, int height, int comp, int quality);


#ifdef __cplusplus
}
#endif

#endif // JO_INCLUDE_JPEG_H
