/* Assistants.h
 * �ṩһЩ��ݵ�С����
 * ���嶨����Assistants_Impɸѡ����
 */
#pragma once
//
#include <initializer_list>
#include <string>
#include <vector>
//
#include <chrono>
#include <ctime>
//
#include <random>
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
 * String processing extension *
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
 * ���ؼ����ڵ�1��ƥ���const_iterator
 */
std::initializer_list<const std::string>::const_iterator
IsPrefixMatchInColl(const std::string &tarStr,
                    const std::initializer_list<const std::string> &strColl);
/* IsPrefixMatchInColl
 * �жϸ����ļ������Ƿ����ַ�����tarStrǰ׺ƥ��
 * caseTag=trueʱ���ִ�Сд
 * ���ؼ����ڵ�1��ƥ���id
 * δ�ҵ�ʱ���� ZUMCLimitLiterals::InvalidPos
 */
size_t IsPrefixMatchInCollWithCase(
    const std::string &tarStr, bool caseTag,
    const std::initializer_list<const std::string> &strColl);
/* Find1stInCollWithCase
 * ��tarStr�в��Ҹ���strColl��ĳ���1�γ��ֵ�λ��
 * ����pair(posFoundInTar,posFoundInStrColl)
 */
std::pair<size_t, size_t>
Find1stInCollWithCase(const std::string &tarStr, bool caseTag,
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
/* StringCompareWithCase
 * �ַ����Ƚ�
 * caseTagΪtrueʱ���ִ�Сд(Ĭ��)
 */
bool StringCompareWithCase(const std::string &str1, const std::string &str2,
                           bool caseTag = true);
/* WildcardMatch
 * ͨ���ƥ��
 * ֧��
 * * : �����ַ�����
 * ? : �����ַ�����
 */
bool WildcardMatch(const std::string &tarStr, const std::string &wildcardStr);
/******************************
 * Date And Time processing extension *
 ******************************
 */
/* GetCurrentTM
 * �ڲ�����std::localtime_s��ȡ��ǰʱ��
 * ��ȡ��ǰʱ��tm
 */
tm GetCurrentTM();
/* TimeRecorder
 * �򵥼�ʱ��,����
 */
class TimeRecorder {
public:
  using Clock_Ty = std::chrono::high_resolution_clock; // �߾���ʱ��
  using TimePoint_Ty = std::chrono::time_point<Clock_Ty>;
  using Duration_Ty = std::chrono::nanoseconds;

private:
  TimePoint_Ty m_StartTime; // ��ʼʱ��

public:
  // ���캯��
  TimeRecorder() : m_StartTime(Clock_Ty::time_point::min()) {}
  // Start()��ʼ��ʱ
  void Start() { m_StartTime = Clock_Ty::now(); }
  // Reset()���ü�ʱ
  void Reset() { m_StartTime = Clock_Ty::time_point::min(); }
  // Restart()���¿�ʼ��ʱ
  void Restart() { m_StartTime = Clock_Ty::now(); }
  // Elapsed()���ؾ�����ʱ��(��nanosecΪ��λ���ַ���)
  std::string Elapsed() const;
}; // class TimeRecorder
/******************************
 * Customer IO Manipulator *
 ******************************
 */
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
  static const size_t LoopCntLimit = 100;
  static const size_t TotalLenLimit = 10000;
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
    std::string ToString() const;
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
  unsigned char InitFlagTest(unsigned char flag);
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
  std::string ToString() const;
  //
  template <typename CharT, typename ChTraits>
  friend std::basic_ostream<CharT, ChTraits> &
  operator<<(std::basic_ostream<CharT, ChTraits> &os, const IOMTextStyle &ts) {
    os << ts.ToString();
    return os;
  }
  //
}; // class IOMTextStyle
/******************************
 * Small Components For Testing *
 ******************************
 */
/* IntNumericalRange
 * ���ڱ�ʾһ����������,����
 */
class IntNumericalRange {
private:
  int m_Upper; // ��������
  int m_Lower; // ��������
public:
  // ���캯��
  IntNumericalRange(int lower, int upper) : m_Lower(lower), m_Upper(upper) {
    if (m_Lower > m_Upper) {
      std::swap(m_Lower, m_Upper);
    }
  }
  // ��ȡ��������
  int UpperLimit() const { return m_Upper; }
  // ��ȡ��������
  int LowerLimit() const { return m_Lower; }
  // �ж��Ƿ���������
  bool IsInRange(int value) const {
    return (value >= m_Lower && value <= m_Upper);
  }
}; // class IntNumericalRange
/* IntRandomGenerator
 * ���������������
 * �ڲ�ʹ��mt19937_64
 * ���ȷֲ�uniform_int_distribution
 */
class IntRandomGenerator {
private:
  std::mt19937_64 m_Engine;                          // ���������
  std::uniform_int_distribution<int> m_Distribution; // ���ȷֲ�
  unsigned long long m_Seed;                         // ���������
  IntNumericalRange m_DefRange;                      // ����������ֵ��Ĭ�Ϸ�Χ
public:
  // ���캯��
  IntRandomGenerator(int lower = 0, int upper = 100)
      : m_DefRange(lower, upper) {
    m_Seed = std::chrono::steady_clock::now().time_since_epoch().count();
    m_Engine.seed(m_Seed);
    m_Distribution = std::uniform_int_distribution<int>(
        m_DefRange.LowerLimit(), m_DefRange.UpperLimit());
  }
  // ����
  ~IntRandomGenerator() {}
  // ���������
  int Generate() { return m_Distribution(m_Engine); }
  int Generate(int lower, int upper) {
    if (lower > upper) {
      std::swap(lower, upper);
    }
    auto tmpDr = std::uniform_int_distribution<int>(lower, upper);
    return tmpDr(m_Engine);
  }
  int Generate(const IntNumericalRange &range) {
    auto tmpDr = std::uniform_int_distribution<int>(range.LowerLimit(),
                                                    range.UpperLimit());
    return tmpDr(m_Engine);
  }
  // ����ָ�����ȵ�����ַ���(��д)
  std::string GenerateString(int length) {
    std::string str;
    auto tmpDr = std::uniform_int_distribution<int>('A', 'Z');
    for (int i = 0; i < length; ++i) {
      str.push_back(static_cast<char>(tmpDr(m_Engine)));
    }
    return str;
  }

  // �������������
  void Reset() {
    m_Seed = std::chrono::system_clock::now().time_since_epoch().count();
    m_Engine.seed(m_Seed);
  }
  // ����ֲ�
  void ResetRange(int lower = 0, int upper = 100) {
    if (lower > upper) {
      std::swap(lower, upper);
    }
    m_Distribution = std::uniform_int_distribution<int>(lower, upper);
  }
  void ResetRange(const IntNumericalRange &range) {
    m_Distribution = std::uniform_int_distribution<int>(range.LowerLimit(),
                                                        range.UpperLimit());
  }
}; // class IntRandomGenerator

} // namespace ZUMCAssistants
} // namespace ZhousUtilities