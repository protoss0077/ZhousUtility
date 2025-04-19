#include "ZUCmdAssistants.h"

namespace ZhousUtilities {
namespace ZUMCAssistants {
/* static member's initialization */
std::string IOMTextStyle::DefaultSet = "\x1b[0m";
//
std::string IOMTextStyle::RGBColor::ToString() const {
  if (R == 0 && G == 0 && B == 0) {
    return std::string("");
  }
  char buf[20] = "\0";
  sprintf_s(buf, 20, ";2;%d;%d;%dm", R, G, B);
  return std::string(buf);
}
//
unsigned char IOMTextStyle::InitFlagTest(unsigned char flag) {
  if (flag >= StyleIllegal) {
    return StyleNone;
  }
  if ((flag & StyleNone) == StyleNone) {
    return StyleNone;
  }
  return flag;
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
std::string IOMTextStyle::ToString() const {
  std::string res(std::move(GetStyleStr()));
  if (!m_fgColor.empty())
    res = res + "\x1b[38" + m_fgColor;
  if (!m_bgColor.empty())
    res = res + "\x1b[48" + m_bgColor;
  return res;
}

} // namespace ZUMCAssistants
} // namespace ZhousUtilities