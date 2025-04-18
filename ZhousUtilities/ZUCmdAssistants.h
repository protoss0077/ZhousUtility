/* Assistants.h
 * 提供一些便捷的小工具
 * 具体定义在Assistants_Imp筛选器中
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
 * String processing extension *
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
 * 返回集合内第1个匹配的const_iterator
 */
std::initializer_list<const std::string>::const_iterator
IsPrefixMatchInColl(const std::string &tarStr,
                    const std::initializer_list<const std::string> &strColl);
/* IsPrefixMatchInColl
 * 判断给定的集合内是否有字符串与tarStr前缀匹配
 * caseTag=true时区分大小写
 * 返回集合内第1个匹配的id
 * 未找到时返回 ZUMCLimitLiterals::InvalidPos
 */
size_t IsPrefixMatchInCollWithCase(
    const std::string &tarStr, bool caseTag,
    const std::initializer_list<const std::string> &strColl);
/* Find1stInCollWithCase
 * 在tarStr中查找给定strColl中某项第1次出现的位置
 * 返回pair(posFoundInTar,posFoundInStrColl)
 */
std::pair<size_t, size_t>
Find1stInCollWithCase(const std::string &tarStr, bool caseTag,
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
/* StringCompareWithCase
 * 字符串比较
 * caseTag为true时区分大小写(默认)
 */
bool StringCompareWithCase(const std::string &str1, const std::string &str2,
                           bool caseTag = true);
/* WildcardMatch
 * 通配符匹配
 * 支持
 * * : 任意字符序列
 * ? : 单个字符序列
 */
bool WildcardMatch(const std::string &tarStr, const std::string &wildcardStr);
/******************************
 * Date And Time processing extension *
 ******************************
 */
/* GetCurrentTM
 * 内部调用std::localtime_s获取当前时间
 * 获取当前时间tm
 */
tm GetCurrentTM();
/* TimeRecorder
 * 简单计时器,自用
 */
class TimeRecorder {
public:
  using Clock_Ty = std::chrono::high_resolution_clock; // 高精度时钟
  using TimePoint_Ty = std::chrono::time_point<Clock_Ty>;
  using Duration_Ty = std::chrono::nanoseconds;

private:
  TimePoint_Ty m_StartTime; // 开始时间

public:
  // 构造函数
  TimeRecorder() : m_StartTime(Clock_Ty::time_point::min()) {}
  // Start()开始计时
  void Start() { m_StartTime = Clock_Ty::now(); }
  // Reset()重置计时
  void Reset() { m_StartTime = Clock_Ty::time_point::min(); }
  // Restart()重新开始计时
  void Restart() { m_StartTime = Clock_Ty::now(); }
  // Elapsed()返回经过的时间(以nanosec为单位的字符串)
  std::string Elapsed() const;
}; // class TimeRecorder
/******************************
 * Customer IO Manipulator *
 ******************************
 */
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
   * 测试并返回正确的flag
   */
  unsigned char InitFlagTest(unsigned char flag);
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
 * 用于表示一个整数区间,自用
 */
class IntNumericalRange {
private:
  int m_Upper; // 区间上限
  int m_Lower; // 区间下限
public:
  // 构造函数
  IntNumericalRange(int lower, int upper) : m_Lower(lower), m_Upper(upper) {
    if (m_Lower > m_Upper) {
      std::swap(m_Lower, m_Upper);
    }
  }
  // 获取区间上限
  int UpperLimit() const { return m_Upper; }
  // 获取区间下限
  int LowerLimit() const { return m_Lower; }
  // 判断是否在区间内
  bool IsInRange(int value) const {
    return (value >= m_Lower && value <= m_Upper);
  }
}; // class IntNumericalRange
/* IntRandomGenerator
 * 整数随机数生成器
 * 内部使用mt19937_64
 * 均匀分布uniform_int_distribution
 */
class IntRandomGenerator {
private:
  std::mt19937_64 m_Engine;                          // 随机数引擎
  std::uniform_int_distribution<int> m_Distribution; // 均匀分布
  unsigned long long m_Seed;                         // 随机数种子
  IntNumericalRange m_DefRange;                      // 控制生成数值的默认范围
public:
  // 构造函数
  IntRandomGenerator(int lower = 0, int upper = 100)
      : m_DefRange(lower, upper) {
    m_Seed = std::chrono::steady_clock::now().time_since_epoch().count();
    m_Engine.seed(m_Seed);
    m_Distribution = std::uniform_int_distribution<int>(
        m_DefRange.LowerLimit(), m_DefRange.UpperLimit());
  }
  // 析构
  ~IntRandomGenerator() {}
  // 生成随机数
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
  // 生成指定长度的随机字符串(大写)
  std::string GenerateString(int length) {
    std::string str;
    auto tmpDr = std::uniform_int_distribution<int>('A', 'Z');
    for (int i = 0; i < length; ++i) {
      str.push_back(static_cast<char>(tmpDr(m_Engine)));
    }
    return str;
  }

  // 重设随机数种子
  void Reset() {
    m_Seed = std::chrono::system_clock::now().time_since_epoch().count();
    m_Engine.seed(m_Seed);
  }
  // 重设分布
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