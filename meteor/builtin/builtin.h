/****************************************
 * Copyright (c) 2019 Wenyi Tang
 * Author: Wenyi Tang
 * E-mail: wenyitang@outlook.com
 * Description: Built-in decoders
 ****************************************/
#ifndef METEOR_BUILTIN_BUILTIN_H_
#define METEOR_BUILTIN_BUILTIN_H_
namespace mt {
namespace flo {
int Open(const char* file);
int Close();
int Run(const char* bytes, char* image);
int Info(int* w, int* h, void* info);
}  // namespace flo

namespace png {
int Open(const char* file);
int Close();
int Run(const char* bytes, char* image);
int Info(int* w, int* h, void* info);
}  // namespace png

}  // namespace mt
#endif  // METEOR_BUILTIN_BUILTIN_H_
