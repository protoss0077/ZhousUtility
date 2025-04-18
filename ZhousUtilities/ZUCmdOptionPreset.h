/* ZUCmdOptionPreset.h
 * 提供一些常用的参数验证函数
 */
#pragma once
//
#include <filesystem>
#include <stdexcept>

namespace ZhousUtilities {
// 参数验证预设
static bool DefValidationOper(std::vector<std::string>::const_iterator,
                              std::vector<std::string>::const_iterator) {
  return true;
}
//
static bool
IntValidationOper(std::vector<std::string>::const_iterator iterBegin,
                  std::vector<std::string>::const_iterator iterEnd) {
  try {
    (void)std::stoi(*iterBegin);
    return (iterBegin + 1) == iterEnd;
  } catch (std::invalid_argument &e) {
    return false;
  }
  return true;
}
//
static bool
IntCollValidationOper(std::vector<std::string>::const_iterator iterBegin,
                      std::vector<std::string>::const_iterator iterEnd) {
  for (auto iter = iterBegin; iter != iterEnd; iter++) {
    try {
      (void)std::stoi(*iter);
    } catch (std::invalid_argument &e) {
      return false;
    }
  }
  return true;
}
//
static bool
FloatValidationOper(std::vector<std::string>::const_iterator iterBegin,
                    std::vector<std::string>::const_iterator iterEnd) {
  try {
    (void)std::stof(*iterBegin);
    return (iterBegin + 1) == iterEnd;
  } catch (std::invalid_argument &e) {
    return false;
  }
  return true;
}
//
static bool
FloatCollValidationOper(std::vector<std::string>::const_iterator iterBegin,
                        std::vector<std::string>::const_iterator iterEnd) {
  for (auto iter = iterBegin; iter != iterEnd; iter++) {
    try {
      (void)std::stof(*iter);
    } catch (std::invalid_argument &e) {
      return false;
    }
  }
  return true;
}
//
static bool
DirValidationOper(std::vector<std::string>::const_iterator iterBegin,
                  std::vector<std::string>::const_iterator iterEnd) {
  try {
    (void)std::filesystem::is_directory(*iterBegin);
    return (iterBegin + 1) == iterEnd;
  } catch (std::filesystem::filesystem_error &e) {
    return false;
  }
  return true;
}
//
static bool
DirCollValidationOper(std::vector<std::string>::const_iterator iterBegin,
                      std::vector<std::string>::const_iterator iterEnd) {
  for (auto iter = iterBegin; iter != iterEnd; iter++) {
    try {
      (void)std::filesystem::is_directory(*iter);
    } catch (std::filesystem::filesystem_error &e) {
      return false;
    }
  }
  return true;
}
//
static bool
FileValidationOper(std::vector<std::string>::const_iterator iterBegin,
                   std::vector<std::string>::const_iterator iterEnd) {
  try {
    (void)std::filesystem::is_regular_file(*iterBegin);
    return (iterBegin + 1) == iterEnd;
  } catch (std::filesystem::filesystem_error &e) {
    return false;
  }
  return true;
}
//
static bool
FileCollValidationOper(std::vector<std::string>::const_iterator iterBegin,
                       std::vector<std::string>::const_iterator iterEnd) {
  for (auto iter = iterBegin; iter != iterEnd; iter++) {
    try {
      (void)std::filesystem::is_regular_file(*iter);
    } catch (std::filesystem::filesystem_error &e) {
      return false;
    }
  }
  return true;
}
//
} // namespace ZhousUtilities