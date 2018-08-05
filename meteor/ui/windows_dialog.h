#ifndef IXR_UTILITY_MISC_WINDOWS_DIALOG_H_
#define IXR_UTILITY_MISC_WINDOWS_DIALOG_H_
#include <shobjidl.h>
#include <string>

namespace ixr {
namespace utils {

inline std::string CallOpenFileDialog(std::string old="") {
  IFileOpenDialog *dialog;
  std::string ret = old;
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
  return ret;
}

}  // namespace utils
}  // namespace ixr
#endif  // IXR_UTILITY_MISC_WINDOWS_DIALOG_H_
