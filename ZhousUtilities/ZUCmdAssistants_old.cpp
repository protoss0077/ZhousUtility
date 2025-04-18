#include "ZUCmdAssistants.h"
//
#include <unordered_set>

namespace ZhousUtilities {
namespace ZUMCAssistants {
/******************************
 * ZUMCAssistants's definition *
 ******************************
 */
//
std::vector<std::string> StringSplitByChar(const std::string &tarStr,
                                           const std::string &spChColl) {
  std::vector<std::string> res;
  size_t prefPos = 0, curPos = 0;
  bool validFlag = false; // 有效标志防止连续分隔符
  for (; curPos < tarStr.size(); ++curPos) {
    if (spChColl.find(tarStr[curPos]) != std::string::npos) {
      if (validFlag) {
        if (prefPos < curPos) {
          res.push_back(tarStr.substr(prefPos, curPos - prefPos));
        }
        validFlag = false;
      }
      prefPos = curPos + 1;
    } else {
      validFlag = true;
    }
  }
  //
  if (prefPos < tarStr.size()) {
    res.push_back(tarStr.substr(prefPos));
  }
  return res;
}
//
std::vector<std::string> StringSplitByCharWithSplit(const std::string &tarStr,
                                                    const std::string &spChColl,
                                                    bool dupTag) {
  std::vector<std::string> res;
  size_t prefPos = 0, curPos = 0;
  for (; curPos < tarStr.size(); ++curPos) {
    size_t foundSpChPos = spChColl.find(tarStr[curPos]);
    if (foundSpChPos != std::string::npos) {
      if ((tarStr[prefPos] != spChColl[foundSpChPos]) && (prefPos < curPos)) {
        res.push_back(std::move(tarStr.substr(prefPos, curPos - prefPos)));
        prefPos = curPos;
      }
      //
      for (; curPos < tarStr.size(); ++curPos) {
        if (spChColl[foundSpChPos] != tarStr[curPos]) {
          break;
        }
      }
      //
      if (dupTag) {
        res.push_back(std::move(tarStr.substr(prefPos, curPos - prefPos)));
      } else {
        res.push_back(std::move(tarStr.substr(prefPos, 1)));
      }
      prefPos = curPos;
      continue;
    }
    //
  }
  if (prefPos < tarStr.size()) {
    res.push_back(tarStr.substr(prefPos));
  }
  return res;
}
//
std::vector<std::string>
StringSplitByCharWithIgnore(const std::string &tarStr,
                            const std::string &spChColl,
                            const std::string &ignoreChColl) {
  std::vector<std::string> res;
  size_t prefPos = 0, curPos = 0;
  bool validFlag = false; // 有效标志防止连续分隔符
  for (; curPos < tarStr.size(); ++curPos) {
    // 处理成对出现的忽略字符
    size_t posI = ignoreChColl.find(tarStr[curPos]);
    if (posI != std::string::npos) {
      size_t off = 1;
      for (; off < tarStr.size() - curPos; off++) {
        if (ignoreChColl[posI] == tarStr[curPos + off])
          break;
      }
      if (off == tarStr.size() - curPos) {
        continue;
      }
      if (validFlag) {
        curPos += off + 1;
      } else {
        res.push_back(tarStr.substr(curPos, off + 1));
        curPos += off + 1;
        prefPos = curPos;
      }
      continue;
    }
    //
    if (spChColl.find(tarStr[curPos]) != std::string::npos) {
      if (validFlag) {
        if (prefPos < curPos) {
          res.push_back(tarStr.substr(prefPos, curPos - prefPos));
        }
        validFlag = false;
      }
      prefPos = curPos + 1;
    } else {
      validFlag = true;
    }
  }
  //
  if (prefPos < tarStr.size()) {
    res.push_back(tarStr.substr(prefPos));
  }
  return res;
}
//
std::initializer_list<const std::string>::const_iterator
IsPrefixMatchInColl(const std::string &tarStr,
                    const std::initializer_list<const std::string> &strColl) {
  if (tarStr.size() < 1 || strColl.size() < 1)
    return strColl.end();
  size_t tarLen = tarStr.length();
  auto cIter = strColl.begin();
  for (; cIter != strColl.end(); cIter++) {
    if (cIter->length() > tarLen)
      continue;
    size_t i = 0;
    for (; i < cIter->length(); i++) {
      if (tarStr[i] != (*cIter)[i]) {
        break;
      }
    }
    if (i == (cIter->length())) {
      return cIter;
    }
  }
  return strColl.end();
}
//
std::vector<std::string>
StringSplitByDelim(const std::string &tarStr,
                   const std::initializer_list<const std::string> &delimColl) {
  std::vector<std::string> res;
  size_t prefPos = 0, curPos = 0;

  bool validFlag = false;
  for (; curPos < tarStr.length(); curPos++) {
    auto foundIter = IsPrefixMatchInColl(tarStr.substr(curPos), delimColl);
    if (foundIter != delimColl.end()) {
      if (validFlag) {
        if (prefPos < curPos) {
          res.push_back(tarStr.substr(prefPos, curPos - prefPos));
        }
        validFlag = false;
      }
      prefPos = curPos + foundIter->length();
      curPos = prefPos;
    } else {
      validFlag = true;
    }
  }
  // 处理剩余部分
  if (prefPos < tarStr.length()) {
    res.push_back(tarStr.substr(prefPos));
  }
  return res;
}
//
bool HasCommonChar(const std::string &str1, const std::string &str2) {
  std::unordered_set<char> charSet(str1.begin(), str1.end());
  for (auto &c : str2) {
    if (charSet.find(c) != charSet.end()) {
      return true;
    }
  }
  return false;
}
//
std::string StringTrimLeft(const std::string &tarStr,
                           const std::string &ignoreChColl) {
  size_t pos = 0;
  for (; pos < tarStr.size(); pos++) {
    if (ignoreChColl.find(tarStr[pos]) == std::string::npos) {
      break;
    }
  }
  return tarStr.substr(pos);
}
//
std::string StringTrimRight(const std::string &tarStr,
                            const std::string &ignoreChColl) {
  size_t pos = tarStr.size();
  for (; pos > 0; pos--) {
    if (ignoreChColl.find(tarStr[pos - 1]) == std::string::npos) {
      break;
    }
  }
  return tarStr.substr(0, pos);
}
//
std::string StringTrim(const std::string &tarStr,
                       const std::string &ignoreChColl) {
  size_t posL = 0, posR = tarStr.size();
  for (; posL < tarStr.size(); posL++) {
    if (ignoreChColl.find(tarStr[posL]) == std::string::npos) {
      break;
    }
  }
  //
  for (; posR > 0; posR--) {
    if (ignoreChColl.find(tarStr[posR - 1]) == std::string::npos) {
      break;
    }
  }
  return tarStr.substr(posL, posR - posL);
}
//
bool WildcardMatch(const std::string &tarStr, const std::string &wildcardStr) {
  auto splited = StringSplitByCharWithSplit(wildcardStr, "*?", true);
  //
  size_t tarCurPos = 0;
  size_t spResPos = 0;
  bool asteriskTag = false;
  for (; spResPos < splited.size(); ++spResPos) {
    // * 连续的*只算一个
    if (splited[spResPos][0] == '*') {
      if (spResPos == splited.size() - 1) {
        return true;
      }
      asteriskTag = true;
      continue;
    }
    // ?
    if (splited[spResPos][0] == '?') {
      size_t matchLen = splited[spResPos].size();
      if (tarCurPos + matchLen > tarStr.size()) {
        return false;
      }
      tarCurPos += matchLen;
      continue;
    }
    // normal
    if (asteriskTag) {
      auto foundPos = tarStr.substr(tarCurPos).find(splited[spResPos]);
      if (foundPos == std::string::npos) {
        return false;
      }
      tarCurPos += (foundPos + splited[spResPos].size() + 1);
      asteriskTag = false;
      continue;
    }
    size_t matchLen = splited[spResPos].size();
    if (tarCurPos + matchLen > tarStr.size()) {
      return false;
    }
    if (tarStr.substr(tarCurPos, matchLen) != splited[spResPos]) {
      return false;
    }
    tarCurPos += (matchLen + 1);
  }
  return tarCurPos >= tarStr.size();
}
//
tm GetCurrentTM() {
  time_t t = time(0);
  tm now;
  localtime_s(&now, &t);
  return now;
}

//
std::string IOMTextStyle::GetStyleStr() const {
  std::string res(DefaultSet);
  if ((m_StyleFlag & StyleNone) == StyleNone)
    return res;
  if ((m_StyleFlag & StyleUnderline) == StyleUnderline)
    res += "\x1b[4m";
  if ((m_StyleFlag & StyleItalic) == StyleItalic)
    res += "\x1b[3m";
  if ((m_StyleFlag & StyleDelete) == StyleDelete)
    res += "\x1b[9m";
  return res;
}
//
/* some static members init */
//
std::string IOMTextStyle::DefaultSet = "\x1b[0m";
} // namespace ZUMCAssistants
} // namespace ZhousUtilities