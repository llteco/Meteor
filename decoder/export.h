/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : export header definitions
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#ifndef DECODER_EXPORT_H_
#define DECODER_EXPORT_H_
#ifdef _MSC_VER
// We are using a Microsoft compiler:
#define METEOR_EXPORT __declspec(dllexport)

#else
// Not Microsoft compiler so set empty definition:
#define IXR_ENGINE_API
#endif

extern "C" {
METEOR_EXPORT int Open(const char *file);
METEOR_EXPORT int Close();
METEOR_EXPORT int Run(const char *bytes, char *image);
METEOR_EXPORT int Info(int *w, int *h, void *info);
}
#endif  // DECODER_EXPORT_H_
