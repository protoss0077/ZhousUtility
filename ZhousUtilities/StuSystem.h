/* StuSystem.h
 * 提供一个可交互的学生个人信息管理系统
 * 命令行参数
 * --new 以创建临时数据源的方式进入交互模式(默认选项)
 * --help 显示命令帮助信息
 * --check SourceFileColl
 * 检查指定数据源(可指定多个)的数据并输出结果(仅测试数据源)
 * --source SourceFileColl
 * 检查指定数据源(可指定多个),输出简要结果;创建一个临时数据源，合并指定源的有效数据,进入交互模式
 * --combine NewSourceFile
 * 合并数据源，将合并后的数据保存到指定文件,不会进入交互模式与--source同时使用
 * --compare LeftSourceFile RightSourceFile 输出两个数据源的差异
 * --checkout CheckResultFile 将检查阶段的结果输出到文件,此时控制台输出简要信息
 * 交互模式下可使用的命令
 * new 丢弃当前数据，创建一个新的数据源(如果当前编辑的数据不为空会闻讯是否保存)
 * open SourceFile 如果指定的数据源有效,丢弃当前数据导入指定数据源
 * append SourceFile 如果指定的数据源有效,将指定数据源的数据追加到当前数据源
 * search 在当前数据源中搜索指定的支持姓名和学号查找(通配符仅支持 * ?)
 * add 创建一个新的学生信息,如果信息有效则添加到当前数据源
 * delete 删除当前数据源中指定的学生信息接受以姓名或学号(通配符仅支持 * ?)
 * save [SourceFile] 保存当前数据源到指定文件,如果不指定文件则保存到当前数据源
 * quit 退出交互,如果当前数据源存在数据提示是否保存
 * help 显示交互模式下的帮助
 */
#pragma once
//
#include "ZUCmdLine.h"
#include "ZUCmdLineErr.h"
//
#include <regex>
//
namespace StuSystem {
/* Literal & Limit
 * 一些常量和限值
 */
static const std::string StuSystemPromptOut = "\nStuSystem |> ";
static const std::string StuSystemPromptIn = "\nStuSystem <| ";
// 姓名非法字符集
static const std::string InvalidNameCharColl =
    "~!@#$%^&*()+{}|:\"<>?`=[]\\;',.0123456789";
// 姓名长度限制
constexpr static const size_t MinStuNameLength = 2;
constexpr static const size_t MaxStuNameLength = 32;
// 最小出生日期的数字,登记年龄时小于此年份会被视为错误
constexpr static const size_t MinBirthDate = 195000;
// 用于录入时生成学号最后5位的计数器,如果采用导入数据源的方式
// 会根据当前日期(前6位)与导入数据源学号匹配并更新计数器
// 该值不会大于StuSystemIdCounterMax
static size_t StuSystemIdCounter = 0;
constexpr static const size_t StuSystemIdCounterMax = 15000;
//
/* Major
 * 专业
 */
enum class Major : unsigned char {
  Unknown = 0,
  Science = 1,
  Engineering = 2,
  Economics = 3,
  Management = 4,
  Agriculture = 5,
  Medicine = 6,
  Literature = 7,
  History = 8,
  Philosophy = 9,
  Law = 10,
  Art = 11,
  Education = 12
};
// MajorStringColl 与Major对应的字符串
static const std::string MajorStringColl[13]{
    "未知(Unknown)",     "科学(Science)",      "工学(Engineering)",
    "经济学(Economics)", "管理学(Management)", "农学(Agriculture)",
    "医学(Medicine)",    "文学(Literature)",   "历史(History)",
    "哲学(Philosophy)",  "法学(Law)",          "艺术(Art)",
    "教育(Education)"};
//
class StudentRecord {
  /* 文件序列化时一行为一条记录以字符串形式存储
   * 姓名,出生年月,专业,入学年月,学号
   * ",; "都被视为分隔符
   * 内部m_BirthDate和m_EnrollmentDate以yyyy.mm格式存储
   */
private:
  std::string m_Name;           // 姓名
  std::string m_BirthDate;      // 出生年月 yyyy.mm
  Major m_Major;                // 专业分类
  std::string m_EnrollmentDate; // 入学年月 yyyy.mm
  // 学号 格式为 yyyy(入学年)mm(入学月)##(2位专业编号)#####(5位序号)
  std::string m_Id;
  //
public:
  StudentRecord(const std::string &na, const std::string &bdate, Major m);
  //
  StudentRecord(const std::string &str);
  //
  std::string ToString() const;
  //
  static StudentRecord FromString(const std::string &str);
  // 日期格式匹配的正则
  // yyyy年mm月 | yyyy.mm | yyyy-mm
  static std::regex s_DateRegex;

  // 初始化入学日期字符串为当前日期
  static std::string EnrollDateString;
  // 设置录入时的入学日期 可识别 yyyy年mm月 yyyy.mm
  // yyyy-mm,返回设置后的EnrollDateString
  static const std::string &SetEnrollDateString(const std::string dstr = "");
  //
private:
  //
  StudentRecord();
  //
  /* NameValidation
   * 验证姓名是否有效
   */
  static bool NameValidation(const std::string &na);
  /* DateValidation
   * 验证日期字符串是否满足DateRegexA/B/C的格式
   * 如果为true则视为出生日期添加MinBirthYear限制
   * 为false则视为入学日期添加当前年份限制
   */
  static bool DateValidation(const std::string &bdate, bool tag = true);
  /*
   */
  /* GenerateID1stHalf
   * 生成学号,格式为 yyyy(入学年)mm(入学月)##(2位专业编号)#####(5位序号)
   */
  static std::string GenerateID(Major m);
  /* IsIdMatched
   * 学号id是否与日期、专业匹配
   */
  static bool IsIdMatched(const StudentRecord &stuRd);
  /* DateFmtConvert
   * 将日期字符串统一转换为yyyy.mm格式
   */
  static std::string DateFmtConvert(const std::string &dstr);

}; // class StudentRecord

//
class ZUCommandStuSystem {
public:
  // 注册命令
  static void Command_StuSystemRegister();

private:
  /* StuSystemCmdExeOper
   * 执行函数
   */
  static bool
  StuSystemCmdExeOper(const ZhousUtilities::ZUMCParseResult &parseRes);
  /**/
}; // class ZUCommandStySystem
//
class StuSysErrIllegalParameter final
    : public ZhousUtilities::ZUCL_ExceptionBase {
public:
  StuSysErrIllegalParameter(const char *msgAdd, const char *fna, const int ln)
      : ZUCL_ExceptionBase((std::string("Illegal Parameter When Create "
                                        "StudentRecord Instance\nMessage:") +
                            msgAdd)
                               .c_str(),
                           fna, ln) {}
}; // class StuSysErrIllegalParameter

} // namespace StuSystem