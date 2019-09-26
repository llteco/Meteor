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

using Microsoft::WRL::ComPtr;

namespace mt {
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
  HRESULT hr = MFCreateAttributes(&attr_, 2);
  hr = attr_->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
                      MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
  assert(SUCCEEDED(hr));
  if (dxgiman_) {
    hr = attr_->SetUnknown(MF_SOURCE_READER_D3D_MANAGER, dxgiman_.Get());
    assert(SUCCEEDED(hr));
  }
  IMFActivate** devices = nullptr;
  UINT count = 0;
  hr = MFEnumDeviceSources(attr_.Get(), &devices, &count);
  assert(SUCCEEDED(hr));
  for (UINT i = 0; i < count; i++) {
    WCHAR* name;
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

MfHelper::TypeLists MfHelper::EnumCaptureTypes(const GUID type, int w, int h,
                                               float fps) {
  TypeLists lists;
  UINT type_offset = 0;
  ComPtr<IMFMediaType> mf_type;
  std::vector<ComPtr<IMFMediaType>> candidates;
  GUID majortype, subtype;
  if (reader_) {
    while (S_OK == reader_->GetNativeMediaType(0, type_offset, &mf_type)) {
      HRESULT hr = mf_type->GetMajorType(&majortype);
      assert(MFMediaType_Video == majortype);
      hr = mf_type->GetGUID(MF_MT_SUBTYPE, &subtype);
      assert(SUCCEEDED(hr));
      lists.insert(subtype);
      if (subtype == type) {
        candidates.push_back(mf_type);
      }
      type_offset++;
    }
  }
  int e_area = std::numeric_limits<int>::max();
  float e_fps = std::numeric_limits<float>::max();
  // get the expected format with closest W/H and max FPS
  for (auto& _t : candidates) {
    PROPVARIANT var;
    _t->GetItem(MF_MT_FRAME_SIZE, &var);
    int _dif = abs((int)var.hVal.LowPart * var.hVal.HighPart - w * h);
    if (_dif <= e_area) {
      e_area = _dif;
      _t->GetItem(MF_MT_FRAME_RATE, &var);
      float _fps = (float)var.hVal.HighPart / (float)var.hVal.LowPart;
      if (fps - _fps <= e_fps) {
        e_fps = fps - _fps;
        mf_type = _t;
      }
    }
    PropVariantClear(&var);
  }
  if (mf_type) {
    type_ = mf_type;
    reader_->SetCurrentMediaType(0, nullptr, mf_type.Get());
  }
  return lists;
}

HRESULT MfHelper::QueryFrameSize(UINT* width, UINT* height, FLOAT* fps) {
  if (!type_ || !width || !height) return E_POINTER;
  PROPVARIANT var;
  HRESULT hr = type_->GetItem(MF_MT_FRAME_SIZE, &var);
  if (width) *width = static_cast<UINT>(var.uhVal.HighPart);
  if (height) *height = static_cast<UINT>(var.uhVal.LowPart);
  if (fps) {
    hr = type_->GetItem(MF_MT_FRAME_RATE, &var);
    *fps = (float)var.hVal.HighPart / (float)var.hVal.LowPart;
  }
  PropVariantClear(&var);
  return hr;
}

HRESULT MfHelper::LockFrame(BYTE** ptr, UINT* length) {
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
      hr = sample_->GetBufferByIndex(0, &buffer_);
      hr = buffer_->Lock(ptr, &max_size, &current_size);
      if (SUCCEEDED(hr) && length) *length = static_cast<UINT>(current_size);
    }
  }
  return hr;
}

HRESULT MfHelper::UnlockFrame() { return buffer_->Unlock(); }

Microsoft::WRL::ComPtr<ID3D11Device> MfHelper::InitializeD3DManager(
    ID3D11Device* dev) {
  HRESULT hr = S_OK;
  static const D3D_FEATURE_LEVEL levels[] = {
      D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1,
      D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_9_3,  D3D_FEATURE_LEVEL_9_2,
      D3D_FEATURE_LEVEL_9_1};
  ComPtr<ID3D11DeviceContext> ctx;
  if (!dev) {
    hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                           D3D11_CREATE_DEVICE_VIDEO_SUPPORT, levels,
                           ARRAYSIZE(levels), D3D11_SDK_VERSION, &device_,
                           nullptr, &ctx);
  } else {
    device_.Attach(dev);
  }
  if (SUCCEEDED(hr)) {
    ComPtr<ID3D10Multithread> multithread;
    hr = device_.As(&multithread);
    if (SUCCEEDED(hr)) {
      multithread->SetMultithreadProtected(TRUE);
    }
    UINT reset_token = 0;
    hr = MFCreateDXGIDeviceManager(&reset_token, &dxgiman_);
    if (SUCCEEDED(hr)) {
      hr = dxgiman_->ResetDevice(device_.Get(), reset_token);
    }
    if (SUCCEEDED(hr)) {
      return device_;
    }
  }
  return nullptr;
}

ComPtr<ID3D11Texture2D> MfHelper::GetSampleTex() {
  DWORD stream_index, flag;
  HRESULT hr = S_OK;
  hr = reader_->ReadSample(MF_SOURCE_READER_ANY_STREAM, 0, &stream_index, &flag,
                           nullptr, &sample_);
  if (FAILED(hr)) {
    return nullptr;
  }
  if (flag) {
    // check flags
    if (flag & MF_SOURCE_READERF_STREAMTICK) {
      // may not ready
      return nullptr;
    }
    if (!sample_) {
      return nullptr;
    }
  }
  ComPtr<IMFMediaBuffer> buffer0;
  ComPtr<IMFDXGIBuffer> buffer1;
  ComPtr<ID3D11Texture2D> tex;
  hr = sample_->GetBufferByIndex(0, &buffer0);
  if (FAILED(hr)) {
    return nullptr;
  }
  hr = buffer0.As(&buffer1);
  if (FAILED(hr)) {
    return nullptr;
  }
  hr = buffer1->GetResource(IID_PPV_ARGS(&tex));
  if (FAILED(hr)) {
    return nullptr;
  }
  return tex;
}

}  // namespace mt
