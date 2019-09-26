/****************************************
 * Copyright (c) 2019 Wenyi Tang
 * Author: Wenyi Tang
 * E-mail: wenyitang@outlook.com
 * Description: Open Windows dialog window
 ****************************************/
#ifndef METEOR_UI_WINDOWS_DIALOG_H_
#define METEOR_UI_WINDOWS_DIALOG_H_
#include <shobjidl.h>
#include <string>
#include <vector>

namespace mt {
namespace ui {

std::string CallOpenFileDialog(std::string old = "");

std::vector<std::string> CallOpenMultipleFileDialog();

std::string CallSaveFileDialog(std::string auto_name);

}  // namespace ui
}  // namespace mt
#endif  // METEOR_UI_WINDOWS_DIALOG_H_
