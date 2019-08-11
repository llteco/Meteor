/********************************************************************
Copyright 2017 Intel Corp. All Rights Reserved.
Description : list DirectShow devices
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : December 1st, 2017
changelog
********************************************************************/
#include "meteor/core/cam_loader.h"
#include <cassert>

namespace ixr {
namespace app {
namespace helper {
using Microsoft::WRL::ComPtr;

MfHelper::MfHelper() { MFStartup(MF_VERSION); }

MfHelper::~MfHelper() {
  if (source_) source_->Shutdown();
  // release all resources before MFShutdown.
  buffer_.Reset();
  sample_.Reset();
  reader_.Reset();
  MFShutdown();
}

MfHelper::DeviceLists MfHelper::EnumVideoCaptureDevices(
    const std::string devname) {
  CoInitialize(nullptr);
  DeviceLists lists;
  HRESULT hr = MFCreateAttributes(&attr_, 1);
  hr = attr_->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
                      MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
  assert(SUCCEEDED(hr));
  IMFActivate **devices = nullptr;
  UINT count = 0;
  hr = MFEnumDeviceSources(attr_.Get(), &devices, &count);
  assert(SUCCEEDED(hr));
  for (UINT i = 0; i < count; i++) {
    WCHAR *name;
    UINT name_len = 0;
    hr = devices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,
                                        &name, &name_len);
    assert(SUCCEEDED(hr));
    std::wstring wstr(name);
    lists.emplace_back(wstr.begin(), wstr.end());
    if (lists.back() == devname) {
      hr = devices[i]->ActivateObject(IID_PPV_ARGS(&source_));
      assert(SUCCEEDED(hr));
      hr = MFCreateSourceReaderFromMediaSource(source_.Get(), attr_.Get(),
                                               &reader_);
      assert(SUCCEEDED(hr));
    } else {
      devices[i]->Release();
    }
    CoTaskMemFree(name);
  }
  CoTaskMemFree(devices);
  CoUninitialize();
  lists.emplace_back("null");
  return lists;
}

MfHelper::TypeLists MfHelper::EnumCaptureTypes(const GUID type) {
  TypeLists lists;
  UINT type_offset = 0;
  ComPtr<IMFMediaType> mf_type;
  GUID majortype, subtype;
  if (reader_) {
    while (S_OK == reader_->GetNativeMediaType(0, type_offset, &mf_type)) {
      HRESULT hr = mf_type->GetMajorType(&majortype);
      assert(MFMediaType_Video == majortype);
      hr = mf_type->GetGUID(MF_MT_SUBTYPE, &subtype);
      assert(SUCCEEDED(hr));
      lists.insert(subtype);
      // set max fps
      PROPVARIANT var;
      mf_type->GetItem(MF_MT_FRAME_RATE_RANGE_MAX, &var);
      mf_type->SetItem(MF_MT_FRAME_RATE, var);
      PropVariantClear(&var);
      if (subtype == type) {
        type_ = mf_type;
        hr = reader_->SetCurrentMediaType(0, nullptr, mf_type.Get());
        assert(SUCCEEDED(hr));
        break;
      }
      type_offset++;
    }
  }
  return lists;
}

HRESULT MfHelper::QueryFrameSize(UINT *width, UINT *height) {
  if (!type_ || !width || !height) return E_POINTER;
  PROPVARIANT var;
  HRESULT hr = type_->GetItem(MF_MT_FRAME_SIZE, &var);
  *width = static_cast<UINT>(var.uhVal.HighPart);
  *height = static_cast<UINT>(var.uhVal.LowPart);
  PropVariantClear(&var);
  return hr;
}

HRESULT MfHelper::LockFrame(BYTE **ptr, UINT *length) {
  DWORD stream_index, flag;
  HRESULT hr = S_OK;
  hr = reader_->ReadSample(MF_SOURCE_READER_ANY_STREAM, 0, &stream_index, &flag,
                           nullptr, &sample_);
  if (SUCCEEDED(hr)) {
    if (flag) {
      // check flags
      if (flag & MF_SOURCE_READERF_STREAMTICK) {
        // may not ready
        return E_ABORT;
      }
    }
    if (sample_) {
      DWORD current_size = 0, max_size = 0;
      hr = sample_->ConvertToContiguousBuffer(&buffer_);
      hr = buffer_->Lock(ptr, &max_size, &current_size);
      if (SUCCEEDED(hr) && length) *length = static_cast<UINT>(current_size);
    }
  }
  return hr;
}

HRESULT MfHelper::UnlockFrame() { return buffer_->Unlock(); }

}  // namespace helper
}  // namespace app
}  // namespace ixr
