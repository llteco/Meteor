#ifndef IXR_UTILITY_MISC_WINDOWS_DIALOG_H_
#define IXR_UTILITY_MISC_WINDOWS_DIALOG_H_
#include <shobjidl.h>
#include <string>
#include <vector>

namespace ixr {
namespace utils {

inline std::string CallOpenFileDialog(std::string old = "") {
  IFileOpenDialog *dialog;
  std::string ret = old;
  HRESULT init = CoInitialize(NULL);
  // Create the FileOpenDialog object.
  HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
                                IID_PPV_ARGS(&dialog));
  if (SUCCEEDED(hr)) {
    // Show the Open dialog box.
    hr = dialog->Show(NULL);
    // Get the file name from the dialog box.
    if (SUCCEEDED(hr)) {
      IShellItem *item;
      hr = dialog->GetResult(&item);
      if (SUCCEEDED(hr)) {
        PWSTR file_path;
        hr = item->GetDisplayName(SIGDN_FILESYSPATH, &file_path);
        // Display the file name to the user.
        if (SUCCEEDED(hr)) {
          std::wstring result(file_path);
          ret = std::string(result.begin(), result.end());
        }
        CoTaskMemFree(file_path);
        item->Release();
      }
    }
    dialog->Release();
  }
  if (SUCCEEDED(init)) {
    CoUninitialize();
  }
  return ret;
}

inline std::vector<std::string> CallOpenMultipleFileDialog() {
  IFileOpenDialog *dialog;
  std::vector<std::string> rets;
  HRESULT init = CoInitialize(NULL);
  // Create the FileOpenDialog object.
  HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
                                IID_PPV_ARGS(&dialog));
  if (SUCCEEDED(hr)) {
    FILEOPENDIALOGOPTIONS opt;
    hr = dialog->GetOptions(&opt);
    hr = dialog->SetOptions(opt | FOS_ALLOWMULTISELECT);
    // Show the Open dialog box.
    hr = dialog->Show(NULL);
    // Get the file name from the dialog box.
    if (SUCCEEDED(hr)) {
      // multiple select
      IShellItemArray *items;
      hr = dialog->GetResults(&items);
      if (SUCCEEDED(hr)) {
        DWORD total;
        hr = items->GetCount(&total);
        for (DWORD i = 0; i < total; i++) {
          IShellItem *item;
          hr = items->GetItemAt(i, &item);
          if (SUCCEEDED(hr)) {
            PWSTR file_path;
            hr = item->GetDisplayName(SIGDN_FILESYSPATH, &file_path);
            // Display the file name to the user.
            if (SUCCEEDED(hr)) {
              std::wstring result(file_path);
              rets.push_back(std::string(result.begin(), result.end()));
            }
            CoTaskMemFree(file_path);
            item->Release();
          }
        }
      }
    }
    dialog->Release();
  }
  if (SUCCEEDED(init)) {
    CoUninitialize();
  }
  return rets;
}

inline std::string CallSaveFileDialog(std::string auto_name) {
  IFileSaveDialog *dialog;
  std::string ret("iloveliuxing");  // magic code :)
  HRESULT init = CoInitialize(NULL);
  // Create the FileOpenDialog object.
  HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
                                IID_PPV_ARGS(&dialog));
  const COMDLG_FILTERSPEC kSaveTypes[]{
      {L"PNG Image (*.png)", L"*.png"},
  };
  std::wstring wname(auto_name.begin(), auto_name.end());
  if (SUCCEEDED(hr)) {
    hr = dialog->SetFileTypes(ARRAYSIZE(kSaveTypes), kSaveTypes);
    hr = dialog->SetFileName(wname.c_str());
    hr = dialog->Show(NULL);
    if (SUCCEEDED(hr)) {
      IShellItem *item;
      hr = dialog->GetResult(&item);
      if (SUCCEEDED(hr)) {
        PWSTR file_path;
        hr = item->GetDisplayName(SIGDN_FILESYSPATH, &file_path);
        // Display the file name to the user.
        if (SUCCEEDED(hr)) {
          std::wstring result(file_path);
          ret = std::string(result.begin(), result.end());
        }
        CoTaskMemFree(file_path);
        item->Release();
      }
    }
    dialog->Release();
  }
  if (SUCCEEDED(init)) {
    CoUninitialize();
  }
  return ret;
}

}  // namespace utils
}  // namespace ixr
#endif  // IXR_UTILITY_MISC_WINDOWS_DIALOG_H_
