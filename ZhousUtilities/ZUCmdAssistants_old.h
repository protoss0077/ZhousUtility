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
// ���Բ�����ShortName,����ṩ����С����Ϊ1
static const int OptShortNameLimit_Min = 1;
// ѡ�������ð��������ַ�
static const std::string InvalidSpecial = "~!@#$%^&*()+{}|:\"<>?`=[]\\;',.";

// ����ʱ���ڷָ����ַ�
static const std::string ValidSplitCharsL1 = "/-";
static const std::string ValidSplitCharsL2 = " ";
static const std::string IgnoreSplitChars = "'\"";
// �ڲ������ķǷ�λ��
static const size_t InvalidPos = size_t(-1);

} // namespace ZUMCLimitLiterals
namespace ZUMCAssistants {
/******************************
 * ���ֺ��� *
 * ��ʱ�������� *
 ******************************
 */
/* StringSplitByChar
 * ָ��һ���ָ������Ϸָ��ַ���
 * ���ȿ��Ǵ˽ӿ�,���⸴�ӵ��ַ����ָ��߼�
 */
std::vector<std::string> StringSplitByChar(const std::string &tarStr,
                                           const std::string &spChColl);
/* StringSplitByCharWithSplit
 * ����һ���汾,����ָ�����صĽ���к��ָ���
 * dupTagΪtrueʱ,tarStr�����������������ͬ�ָ���,��÷ָ����ַ�������������
 * Ϊfalseʱ,��������ͬ�ָ�������¼һ��
 */
std::vector<std::string> StringSplitByCharWithSplit(const std::string &tarStr,
                                                    const std::string &spChColl,
                                                    bool dupTag = false);
/* StringSplitByChar
 * ����һ���汾,����ָ��һ���ɶԳ��ֵĺ����ַ���
 * ����Ϊ�ɶԳ��ֵĺ����ַ����ڲ��������Ĳ��ֲ��ᱻ�ָ�
 * ������ǳɶԳ�����ignoreChColl�ڵ��ַ���Ϊ��ͨ�ַ�
 * ���� ָ���ָ���Ϊ�ո� �����ַ���Ϊ "'\""
 * ��'a b c' "a b c"�������ַ������ᱻ�ָ�
 */
std::vector<std::string>
StringSplitByCharWithIgnore(const std::string &tarStr,
                            const std::string &spChColl,
                            const std::string &ignoreChColl);
/* IsPrefixMatchInColl
 * �жϼ������Ƿ����ַ�����tarStrǰ׺ƥ��
 */

std::initializer_list<const std::string>::const_iterator
IsPrefixMatchInColl(const std::string &tarStr,
                    const std::initializer_list<const std::string> &strColl);
/* StringSplitByDelim
 * ָ��һ���ָ����б��ָ�Ŀ���ַ���
 * ���طָ����ַ�������
 */
std::vector<std::string>
StringSplitByDelim(const std::string &tarStr,
                   const std::initializer_list<const std::string> &delimColl);
/* HasCommonChar
 * �ж������ַ����Ƿ�����ͬ���ַ�
 */
bool HasCommonChar(const std::string &str1, const std::string &str2);
/* StringTrimLeft
 * ����һ�������ַ���ȥ���ַ�����˵ĺ����ַ�
 */
std::string StringTrimLeft(const std::string &tarStr,
                           const std::string &ignoreChColl);
/* StringTrimRight
 * ����һ�������ַ���ȥ���ַ����Ҷ˵ĺ����ַ�
 */
std::string StringTrimRight(const std::string &tarStr,
                            const std::string &ignoreChColl);
/* StringTrim
 * ����һ�������ַ���ȥ���ַ������˵ĺ����ַ�
 */
std::string StringTrim(const std::string &tarStr,
                       const std::string &ignoreChColl);
/* WildcardMatch
 * ͨ���ƥ��
 * ֧��
 * * : �����ַ�����
 * ? : �����ַ�����
 */
bool WildcardMatch(const std::string &tarStr, const std::string &wildcardStr);
/* GetCurrentTM
 * �ڲ�����std::localtime_s��ȡ��ǰʱ��
 * ��ȡ��ǰʱ��tm
 */
tm GetCurrentTM();

/* IOMRepeatStr
 * ��������ظ��ַ����Ĳٿ���
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
    // stringû��string(n,str)�Ĺ���
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
 * ʹ�������ն����п���������ʽ
 */
class IOMTextStyle {
public:
  /* StyleFlog
   * ʹ��b0-b3λ
   * ���������StyleNone���������λ
   */
  const static unsigned char StyleNone = 1;
  const static unsigned char StyleUnderline = 2;
  const static unsigned char StyleItalic = 4;
  const static unsigned char StyleDelete = 8;
  // ���ڵ��ڸ�ֵ��StyleFlagΪ�Ƿ�,��ʱ������StyleNone
  const static unsigned char StyleIllegal = 16;
  static std::string DefaultSet;
  /* RGBColor
   * ����ָ��RGB��ɫ
   * ���֧�ֿ���ָ��RGB��ɫ
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
   * ���Բ�������ȷ��flag
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
   * ����StyleFlag���ظ�ʽ�����ַ���
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