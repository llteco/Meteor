/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : export header definitions
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#ifndef IXR_ENGINE_EXPORT_EXPORT_H_
#define IXR_ENGINE_EXPORT_EXPORT_H_
#ifdef _MSC_VER
// We are using a Microsoft compiler:
#ifdef IXR_ENGINE_SHARED_LIBS
#ifdef ixr_engine_EXPORTS
#define IXR_ENGINE_API __declspec(dllexport)
#else
#define IXR_ENGINE_API __declspec(dllimport)
#endif
#else
#define IXR_ENGINE_API
#endif

#else
// Not Microsoft compiler so set empty definition:
#define IXR_ENGINE_API
#endif
#endif  // IXR_ENGINE_EXPORT_EXPORT_H_
