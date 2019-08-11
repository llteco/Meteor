/********************************************************************
Copyright 2017 Intel Corp. All Rights Reserved.
Description : list DirectShow devices
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : December 1st, 2017
changelog
********************************************************************/
#ifndef IXR_APP_IXR_PLAYER_INPUT_READER_MF_HELPER_H_
#define IXR_APP_IXR_PLAYER_INPUT_READER_MF_HELPER_H_
#include <Windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <wrl/client.h>
#include <set>
#include <string>
#include <vector>

#pragma comment(lib, "dxva2.lib")
#pragma comment(lib, "evr.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfplay.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

namespace ixr {
namespace app {
namespace helper {

struct GUID_CMP : public GUID {
  uint64_t HighPart(const GUID &g) const {
    return (static_cast<uint64_t>(g.Data1) << 32) | (g.Data2 << 16) | g.Data3;
  }

  uint64_t LowPart(const GUID &g) const {
    uint64_t low = 0;
    for (int i = 0; i < 8; i++) {
      low |= (static_cast<uint64_t>(g.Data4[i]) << (8 * i));
    }
    return low;
  }

  bool operator()(const GUID lhs, const GUID rhs) const {
    uint64_t hlhs = HighPart(lhs), llhs = LowPart(lhs);
    uint64_t hrhs = HighPart(rhs), lrhs = LowPart(rhs);
    return (hlhs < hrhs) || (hlhs == hrhs && llhs < lrhs);
  }
};

/**
 * \brief Use M$ media foundation api to capture video source.
 * Usage example:
 * 1. Call EnumVideoCaptureDevices without devname to query device lists;
 * 2. Call EnumVideoCaptureDevices again with specified devname;
 * 3. Call EnumCaptureTypes without type to query supported types;
 * 4. Call EnumCaptureTypes again with specified type;
 * 5. Call QueryFrameSize to get captured width and height;
 * 6. Call Lock/UnlockFrame to query memory buffer pointer of frames.
 */
class MfHelper {
 public:
  using DeviceLists = std::vector<std::string>;
  using TypeLists = std::set<GUID, GUID_CMP>;

  MfHelper();

  ~MfHelper();

  DeviceLists EnumVideoCaptureDevices(const std::string devname = "");

  TypeLists EnumCaptureTypes(const GUID type = GUID());

  HRESULT QueryFrameSize(UINT *width, UINT *height);

  HRESULT LockFrame(BYTE **ptr, UINT *length);

  HRESULT UnlockFrame();

 private:
  Microsoft::WRL::ComPtr<IMFAttributes> attr_;
  Microsoft::WRL::ComPtr<IMFMediaSource> source_;
  Microsoft::WRL::ComPtr<IMFSourceReader> reader_;
  Microsoft::WRL::ComPtr<IMFMediaType> type_;
  Microsoft::WRL::ComPtr<IMFSample> sample_;
  Microsoft::WRL::ComPtr<IMFMediaBuffer> buffer_;
};

}  // namespace helper
}  // namespace app
}  // namespace ixr
#endif  // IXR_APP_IXR_PLAYER_INPUT_READER_MF_HELPER_H_
