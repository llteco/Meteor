/****************************************
 * Copyright (c) 2019 Wenyi Tang
 * Author: Wenyi Tang
 * E-mail: wenyitang@outlook.com
 * Description: PNG decoder
 ****************************************/
#include <png.h>
#include <zlib.h>
#include <string>
#include "meteor/builtin/builtin.h"

namespace mt {

static std::string gFileName;
namespace png {
int Open(const char *file) {
  gFileName = file;
  return 0;
}

int Close() { return 0; }

int Run(const char *bytes, char *image) {
  png_image img{};
  img.version = PNG_IMAGE_VERSION;
  png_image_begin_read_from_file(&img, gFileName.c_str());
  png_color bg{};
  img.format |= PNG_FORMAT_FLAG_ALPHA;
  png_image_finish_read(&img, &bg, image, 0, 0);
  return 0;
}

int Info(int *w, int *h, void *info) {
  png_image img{};
  img.version = PNG_IMAGE_VERSION;
  png_image_begin_read_from_file(&img, gFileName.c_str());
  if (w) *w = img.width;
  if (h) *h = img.height;
  return 0;
}
}  // namespace png

}  // namespace mt