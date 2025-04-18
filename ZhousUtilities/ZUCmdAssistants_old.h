#pragma once
//
#include <initializer_list>
#include <string>
#include <vector>
//
#include <ctime>
//

namespace ZhousUtilities {
namespace ZUMCLimitLiterals {
static const int OptFullNameLimit_Max = 32;
static const int OptFullNameLimit_Min = 4;
static const int OptShortNameLimit_Max = 8;
// 可以不设置ShortName,如果提供则最小长度为1
static const int OptShortNameLimit_Min = 1;
// 选项名不得包含以下字符
static const std::string InvalidSpecial = "~!@#$%^&*()+{}|:\"<>?`=[]\\;',.";

// 解析时用于分隔的字符
static const std::string ValidSplitCharsL1 = "/-";
static const std::string ValidSplitCharsL2 = " ";
static const std::string IgnoreSplitChars = "'\"";
// 内部容器的非法位置
static const size_t InvalidPos = size_t(-1);

} // namespace ZUMCLimitLiterals
namespace ZUMCAssistants {
/******************************
 * 助手函数 *
 * 暂时放在这里 *
 ******************************
 */
/* StringSplitByChar
 * 指定一个分隔符集合分割字符串
 * 优先考虑此接口,避免复杂的字符串分隔逻辑
 */
std::vector<std::string> StringSplitByChar(const std::string &tarStr,
                                           const std::string &spChColl);
/* StringSplitByCharWithSplit
 * 增加一个版本,可以指定返回的结果中含分隔符
 * dupTag为true时,tarStr中如果出现连续的相同分隔符,则该分隔符字符串会完整保留
 * 为false时,连续的相同分隔符仅记录一个
 */
std::vector<std::string> StringSplitByCharWithSplit(const std::string &tarStr,
                                                    const std::string &spChColl,
                                                    bool dupTag = false);
/* StringSplitByChar
 * 增加一个版本,可以指定一个成对出现的忽略字符集
 * 规则为成对出现的忽略字符集内部括起来的部分不会被分割
 * 如果不是成对出现则将ignoreChColl内的字符视为普通字符
 * 比如 指定分隔符为空格 忽略字符集为 "'\""
 * 像'a b c' "a b c"这样的字符串不会被分割
 */
std::vector<std::string>
StringSplitByCharWithIgnore(const std::string &tarStr,
                            const std::string &spChColl,
                            const std::string &ignoreChColl);
/* IsPrefixMatchInColl
 * 判断集合内是否有字符串与tarStr前缀匹配
 */

std::initializer_list<const std::string>::const_iterator
IsPrefixMatchInColl(const std::string &tarStr,
                    const std::initializer_list<const std::string> &strColl);
/* StringSplitByDelim
 * 指定一个分隔符列表，分割目标字符串
 * 返回分割后的字符串集合
 */
std::vector<std::string>
StringSplitByDelim(const std::string &tarStr,
                   const std::initializer_list<const std::string> &delimColl);
/* HasCommonChar
 * 判断两个字符串是否有相同的字符
 */
bool HasCommonChar(const std::string &str1, const std::string &str2);
/* StringTrimLeft
 * 给定一个忽略字符集去除字符串左端的忽略字符
 */
std::string StringTrimLeft(const std::string &tarStr,
                           const std::string &ignoreChColl);
/* StringTrimRight
 * 给定一个忽略字符集去除字符串右端的忽略字符
 */
std::string StringTrimRight(const std::string &tarStr,
                            const std::string &ignoreChColl);
/* StringTrim
 * 给定一个忽略字符集去除字符串两端的忽略字符
 */
std::string StringTrim(const std::string &tarStr,
                       const std::string &ignoreChColl);
/* WildcardMatch
 * 通配符匹配
 * 支持
 * * : 任意字符序列
 * ? : 单个字符序列
 */
bool WildcardMatch(const std::string &tarStr, const std::string &wildcardStr);
/* GetCurrentTM
 * 内部调用std::localtime_s获取当前时间
 * 获取当前时间tm
 */
tm GetCurrentTM();

/* IOMRepeatStr
 * 用于输出重复字符串的操控器
 */
class IOMRepeatStr {
private:
  std::string m_RepeatStr;
  size_t m_LoopCnt;
  //
  const std::string &GetRepeatStr() const { return m_RepeatStr; }
  //
  size_t GetLoopCnt() const { return m_LoopCnt; }
  //
  const size_t LoopCntLimit = 100;
  const size_t TotalLenLimit = 10000;
  //
public:
  IOMRepeatStr(const std::string &repeatStr, size_t loopCnt)
      : m_RepeatStr(""), m_LoopCnt(0) {
    if (loopCnt > LoopCntLimit)
      return;
    if (repeatStr.size() * loopCnt > TotalLenLimit)
      return;
    m_RepeatStr = repeatStr;
    m_LoopCnt = loopCnt;
  }
  //
  template <typename CharT, typename ChTraits>
  friend std::basic_ostream<CharT, ChTraits> &
  operator<<(std::basic_ostream<CharT, ChTraits> &os, const IOMRepeatStr &rs) {
    // string没有string(n,str)的构造
    auto &repStr = rs.GetRepeatStr();
    size_t loopCnt = rs.GetLoopCnt();
    if (repStr.empty() || loopCnt < 1)
      return os;
    std::string tmpStr((repStr.size() * loopCnt), '\0');
    for (size_t i = 0; i < loopCnt; i++) {
      memcpy_s(&tmpStr[i * repStr.size()], repStr.size(), repStr.c_str(),
               repStr.size());
    }
    os << tmpStr;
    return os;
  }
  //
}; // class IOMRepeatStr
/* IOMTextStyle
 * 使用虚拟终端序列控制文字样式
 */
class IOMTextStyle {
public:
  /* StyleFlog
   * 使用b0-b3位
   * 如果设置了StyleNone则忽略其他位
   */
  const static unsigned char StyleNone = 1;
  const static unsigned char StyleUnderline = 2;
  const static unsigned char StyleItalic = 4;
  const static unsigned char StyleDelete = 8;
  // 大于等于该值的StyleFlag为非法,此时会设置StyleNone
  const static unsigned char StyleIllegal = 16;
  static std::string DefaultSet;
  /* RGBColor
   * 用于指定RGB颜色
   * 如果支持可以指定RGB颜色
   */
  struct RGBColor {
    unsigned char R;
    unsigned char G;
    unsigned char B;
    //
    RGBColor(unsigned char r, unsigned char g, unsigned char b)
        : R(r), G(g), B(b) {}
    //
    std::string ToString() const {
      if (R == 0 && G == 0 && B == 0) {
        return std::string("");
      }
      char buf[20] = "\0";
      sprintf_s(buf, 20, ";2;%d;%d;%dm", R, G, B);
      return std::string(buf);
    }
  }; // struct RGBColor
  //
private:
  unsigned char m_StyleFlag;
  std::string m_fgColor;
  std::string m_bgColor;
  //
private:
  /* InitFlagTest
   * 测试并返回正确的flag
   */
  unsigned char InitFlagTest(unsigned char flag) {
    if (flag >= StyleIllegal) {
      return StyleNone;
    }
    if ((flag & StyleNone) == StyleNone) {
      return StyleNone;
    }
    return flag;
  }
  /* GetStyleStr
   * 根据StyleFlag返回格式设置字符串
   */
  std::string GetStyleStr() const;

public:
  explicit IOMTextStyle(unsigned char flag = 1, unsigned char fgnum = 0,
                        unsigned char bgnum = 0)
      : m_StyleFlag(InitFlagTest(flag)), m_fgColor(""), m_bgColor("") {
    char buf[10] = "\0";
    if (fgnum != 0) {
      sprintf_s(buf, 10, ";5;%dm", fgnum);
      m_fgColor = std::string(buf);
      buf[0] = '\0';
    }
    if (bgnum != 0) {
      sprintf_s(buf, 10, ";5;%dm", bgnum);
      m_bgColor = std::string(buf);
      buf[0] = '\0';
    }
  }
  //
  IOMTextStyle(unsigned char flag, const RGBColor &fgColor,
               const RGBColor &bgColor)
      : m_StyleFlag(InitFlagTest(flag)),
        m_fgColor(std::move(fgColor.ToString())),
        m_bgColor(std::move(bgColor.ToString())) {}
  //
  std::string ToString() const {
    std::string res(std::move(GetStyleStr()));
    if (!m_fgColor.empty())
      res = res + "\x1b[38" + m_fgColor;
    if (!m_bgColor.empty())
      res = res + "\x1b[48" + m_bgColor;
    return res;
  }
  //
  template <typename CharT, typename ChTraits>
  friend std::basic_ostream<CharT, ChTraits> &
  operator<<(std::basic_ostream<CharT, ChTraits> &os, const IOMTextStyle &ts) {
    os << ts.ToString();
    return os;
  }

}; // class IOMTextStyle

} // namespace ZUMCAssistants
} // namespace ZhousUtilities