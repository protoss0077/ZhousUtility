/******************************
 * 仿 System.Commandline 的 C++ 实现 *
 * 做了一些简化 *
 * 大概思路是全局维护一个ZUCmdManager对象 *
 * ZUCmdManager 内部维护注册的ZUMainCmd对象 *
 * 通过命令行参数调用内部注册的ZUMainCmd 执行不同的操作 *
 * 以help为例
 * 程序启动时首先调用ZUCmdManager::Init
 * 随后调用ZUCmdManager::Register注册 help ZUMainCmd对象 *
 * 该命令支持 List/Name/brief/detail 选项 *
 * 其中 List/brief/detail 选项不需要参数 *
 * Name 选项支持一个用于查询注册的命令名的字符串参数 *
 * 在命令行传递 help -Name help -brief
 * help 作为主命令 -Name 作为第1个选项 后面的 help 作为选项参数 *
 * -brief 作为第2个选项*
 * 这个命令行解析后调用已经注册的 help (ZUMainCmd) 主命令 *
 * 并将选项和参数一起传递给主命令的Execute以执行 *
 * 最后这个命令行在控制台打印 help 命令的简介 *
 ******************************
 */
#pragma once
#include <functional>
#include <string>
#include <vector>

//
namespace ZhousUtilities {
// 前向声明
class ZUMCmdOpt;
class ZUMainCmd;
class ZUMCParseResult;
class ZUMCmdManager;
/******************************
 * class ZUMCmdOpt's declaration *
 * 命令选项 *
 * 在命令行中选项以 '/' '-' "--" 开始 后跟参数字符串 *
 * 附加规则 -- 之后必须为fullname ,使用 -- 可以更快的定位 *
 * 成员包含: *
 * OptFullName(选项全名) *
 * OptShortName(选项简写) *
 * OptBrief(选项简介)
 * OptDetail(选项详细信息及用例，这部分之后会放进文件内通过解析获取) *
 * 选项是否必须存在 *
 * 选项是否需要参数 *
 * 选项是否支持多个参数 *
 * 选项参数的验证函数指针 *
 ******************************
 */
class ZUMCmdOpt {
public:
  using ZUMCOpt_ArgValidationOper_Ty =
      std::function<bool(std::vector<std::string>::const_iterator,
                         std::vector<std::string>::const_iterator)>;

private:
  std::string m_sFullName;
  std::string m_sShortName;
  std::string m_sBrief;
  std::string m_sDetail;
  /* 最低位-是否必须
   * b1-是否需要参数
   * b2-是否支持多个参数
   */
  unsigned char m_flags;
  ZUMCOpt_ArgValidationOper_Ty m_ArgValidOper;
  //
public:
  // 构造
  ZUMCmdOpt(const std::string &fna, const std::string &sna,
            const std::string &brief, const std::string &detail,
            unsigned char flags, ZUMCOpt_ArgValidationOper_Ty argValiFunc);
  //
  ZUMCmdOpt();
  //
  ZUMCmdOpt(const ZUMCmdOpt &opt);
  /* m_flags的位掩码
   * 使用b0~b3
   */
private:
  static const unsigned char FlagIllegal = 16; // 大于等于该值的flag为非法
public:
  static const unsigned char FlagNoneSet =
      1; // NoneSet优先,设置该位则忽略其他位
  static const unsigned char FlagNecessary = 2;
  static const unsigned char FlagNeedArg = 4;
  static const unsigned char FlagMultiArg = 8;
  // 内部成员访问器
  const std::string &GetOptFullName() const { return m_sFullName; }
  void SetOptFullName(const std::string &na);
  //
  const std::string &GetOptShortName() const { return m_sShortName; }
  void SetOptShortName(const std::string &na);
  //
  const std::string &GetOptBrief() const { return m_sBrief; }
  void SetOptBrief(const std::string &br) { m_sBrief = br; }
  //
  const std::string &GetOptDetail() const { return m_sDetail; }
  void SetOptDetail(const std::string &de) { m_sDetail = de; }
  // flag测试
  bool IsNecessary() const {
    return (m_flags & FlagNecessary) == FlagNecessary;
  }
  //
  bool IsArgAccept() const { return (m_flags & FlagNeedArg) == FlagNeedArg; }
  //
  bool IsMultiArgAccept() const {
    return (m_flags & FlagMultiArg) == FlagMultiArg;
  }
  void SetFlags(unsigned char flag);
  //
  void SetArgValidation(ZUMCOpt_ArgValidationOper_Ty ArgValidOper) {
    m_ArgValidOper = ArgValidOper;
  }
  //
  bool OptNameMatch(const std::string &OptName) const;
  //
  bool TestArgs(std::vector<std::string>::const_iterator iterBegin,
                std::vector<std::string>::const_iterator iterEnd) const {
    return m_ArgValidOper(iterBegin, iterEnd);
  }
  //
  bool operator==(const ZUMCmdOpt &opt) const;
  bool operator!=(const ZUMCmdOpt &opt) const;
  //
  void Reset(const ZUMCmdOpt &opt);
  void operator=(const ZUMCmdOpt &opt);
  //
  std::string ToStringBrief() const;
  std::string ToString() const;

protected:
  static bool OptNameValidationF(const std::string &na);
  static bool OptNameValidationS(const std::string &na);
  static bool OptNameValidationLength(const std::string &na, int minLimt,
                                      int maxLimt);
  static bool OptNameValidationSpChar(const std::string &na);
  //
  std::string GetFlagStr() const;
  /* FlagCheck
   * 检测flag是否合法
   * 不能大于等于FlagIllegal
   * 设置MultiArg时必须设置NeedArg
   */
  bool FlagCheck(short flag) const;
}; // class ZUMCmdOpt
/******************************
 * class ZUMainCmd's declaration *
 * 主命令继承自ZUMCmdOpt 内部的CmdOpt表示其默认的选项 *
 * 这意味着构建一个cmd必须要有一个opt *
 * 构造时该默认的选项会添加进支持的选项表里 *
 * 比如help的默认选项是 -list
 * 包含: *
 * fullName 全称(执行不区分大小写的比较) string *
 * brief 简介;
 * detail 详细信息及用例 *
 * 支持的ZUMCmdOpt(选项) vector<ZUMCmdOpt> 为空时表示该命令不需要选项及参数 *
 * 用于执行Execute接口的函数指针 *
 ******************************
 */
class ZUMainCmd : protected ZUMCmdOpt {
public:
  using ZUMCExeOper_Ty = std::function<bool(const ZUMCParseResult &)>;
  using ZUMCmdOptColl_Ty = std::vector<ZUMCmdOpt>;
  // static const size_t CmdOption_NPOS = size_t(-1);

private:
  std::string m_sFullName;
  std::string m_sBrief;
  std::string m_sDetail;
  ZUMCmdOptColl_Ty m_vCmdOpts;
  ZUMCExeOper_Ty m_ExeOper;
  //
public:
  ZUMainCmd(const std::string &FullName, const std::string &Brief,
            const std::string &Detail, const std::vector<ZUMCmdOpt> &CmdOpts,
            ZUMCExeOper_Ty ExeOper);
  ZUMainCmd(const ZUMainCmd &Cmd);
  //
  const std::string &GetFullName() const { return m_sFullName; }
  void SetFullName(const std::string &FullName);
  //
  const std::string &GetBrief() const { return m_sBrief; }
  void SetBrief(const std::string &Brief) { m_sBrief = Brief; }
  //
  const std::string &GetDetail() const { return m_sDetail; }
  void SetDetail(const std::string &Detail) { m_sDetail = Detail; }
  //
  size_t GetOptCount() const { return m_vCmdOpts.size(); }
  ZUMCmdOptColl_Ty &GetSupportedOpts() { return m_vCmdOpts; }
  const ZUMCmdOptColl_Ty &GetSupportedOpts() const { return m_vCmdOpts; }
  size_t SearchOptByName(const std::string &OptName) const;
  size_t AddOpt(const ZUMCmdOpt &Opt);
  size_t RemoveOpt(const std::string &OptName);
  void ResetOpt(size_t Id, const ZUMCmdOpt &Opt);
  //
  const ZUMCmdOpt &GetOptById(size_t id) const;
  ZUMCmdOpt &GetOptById(size_t id);
  const ZUMCmdOpt &operator[](size_t id) const;
  ZUMCmdOpt &operator[](size_t id);
  //
  ZUMCExeOper_Ty GetExeOper() const { return m_ExeOper; }
  void SetExeOper(ZUMCExeOper_Ty ExeOper) { m_ExeOper = ExeOper; }
  //
  bool Execute(const ZUMCParseResult &ParseResult) const {
    return m_ExeOper(ParseResult);
  }
  //
  bool operator==(const ZUMainCmd &Cmd) const {
    return m_sFullName == Cmd.m_sFullName;
  }
  bool operator!=(const ZUMainCmd &Cmd) const {
    return m_sFullName != Cmd.m_sFullName;
  }
  //
  std::string ToString() const;
  std::string ToStringBrief() const;
}; // class ZUMainCmd

/******************************
 * class ZUMCParseResult's declaration *
 * 解析结果 *
 * ZUMCParser解析后的结果 *
 * 包含: *
 * 原始命令行字符串 string *
 * 主命令名 string *
 * 传入的选项名及参数 pair<opt,vector<string>> *
 ******************************
 */
class ZUMCParseResult {
public:
  using ZUMCParseResultSingle_Ty =
      std::pair<ZUMCmdOpt, std::vector<std::string>>;
  using ZUMCParseResultColl_Ty = std::vector<ZUMCParseResultSingle_Ty>;
  //
private:
  std::string m_sOrigin;
  std::string m_sMainCmd;
  ZUMCParseResultColl_Ty m_vResultColl;
  //
public:
  // 构造
  ZUMCParseResult() : m_sOrigin(""), m_sMainCmd(""), m_vResultColl() {}
  //
  ZUMCParseResult(const std::string &Origin, const std::string &MainCmd)
      : m_sOrigin(Origin), m_sMainCmd(MainCmd), m_vResultColl() {}
  //
  ZUMCParseResult(const std::string &Origin, const std::string &MainCmd,
                  const ZUMCParseResultColl_Ty &ResultColl)
      : m_sOrigin(Origin), m_sMainCmd(MainCmd), m_vResultColl(ResultColl) {}
  //
  ZUMCParseResult(const std::string &Origin, const std::string &MainCmd,
                  ZUMCParseResultColl_Ty &&ResultColl)
      : m_sOrigin(Origin), m_sMainCmd(MainCmd),
        m_vResultColl(std::move(ResultColl)) {}
  //
  const std::string &GetOrigin() const { return m_sOrigin; }
  std::string &GetOrigin() { return m_sOrigin; }
  //
  const std::string &GetMainCmd() const { return m_sMainCmd; }
  std::string &GetMainCmd() { return m_sMainCmd; }
  //
  const ZUMCParseResultColl_Ty &GetResultColl() const { return m_vResultColl; }
  ZUMCParseResultColl_Ty &GetResultColl() { return m_vResultColl; }
  //
  bool IsValid() const {
    return !(m_sOrigin.empty() || m_sMainCmd.empty() || m_vResultColl.empty());
  }
  //
  std::string ToString() const;
}; // class ZUMCParseResult
/******************************
 * class ZUMCParser's declaration *
 * 对输入的命令行运行TryParse进行解析 *
 ******************************
 */
class ZUMCParser {
public:
  /* TryParse
   * 尝试解析命令行
   * originCmdLine 原始命令行字符串
   * ParseResult 解析结果
   * parseStatus 解析状态
   * 返回false时parseStatus包含具体的错误信息
   */
  static bool TryParse(const std::string &originCmdLine,
                       ZUMCParseResult &parseRes, std::string &parseStatus);
  /*
  static bool TryParseWithTargetMan(const std::string &CmdLine,
    ZUMCParseResult &ParseResult,
    ZUMCmdManager &CmdManager);
    */
private:
  //
  ZUMCParser();
  /* hTag指示第1个参数是命令名(true)/选项名(false)
   */
  static ZUMCParseResult::ZUMCParseResultSingle_Ty
  ParseOpt(const ZUMainCmd &mainCmd, const std::vector<std::string> &optArgs,
           bool hTag);
  /* ParseOptFlagCheck
   * 根据flag检查参数数量,!!!并调用参数验证函数
   */
  static bool ParseOptFlagCheck(const ZUMCmdOpt &opt,
                                const std::vector<std::string> &optArgs);
  //
  static void
  OptArgsCombine(ZUMCParseResult::ZUMCParseResultColl_Ty &resColl,
                 ZUMCParseResult::ZUMCParseResultSingle_Ty &&resSingle);
  //
  static bool OptNecessaryCheck(
      const ZUMainCmd &mainCmd,
      const ZUMCParseResult::ZUMCParseResultColl_Ty &parseResColl);

}; // class ZUMCParser
/******************************
 * class ZUMCmdManager's declaration *
 * 管理注册的ZUMainCmd对象 *
 * 内部维护一个vector<ZUMainCmd> *
 * 注册的命令是预设的 *
 * 调用 ZUMCmdManager::Init 将所有命令注册到内部容器 *
 * 为了便于扩展Init的实现放在ZUMCmdManagerImpl.cpp里 *
 ******************************
 */
class ZUMCmdManager {
private:
  static std::vector<ZUMainCmd> m_sMainCmds;

public:
  //
  static size_t Init();
  //
  static size_t SearchByName(const std::string &na);
  //
  static ZUMainCmd &SerachById(size_t id) { return m_sMainCmds[id]; }
  //
  static size_t Register(const ZUMainCmd &mainCmd);
  //
  static size_t DeleteById(size_t id);
  //
  static size_t DeleteByName(const std::string &na);
  //
  static bool ResetById(size_t id, const ZUMainCmd &mainCmd);
  //
  static bool CommandExecute(const ZUMCParseResult &parseRes);
  //
  static size_t GetRegisteredCount() { return m_sMainCmds.size(); }
  //
  static const std::vector<ZUMainCmd> &GetManData() { return m_sMainCmds; }
  //
  static std::string GetManInfo();
}; // class ZUMCmdManager

} // namespace ZhousUtilities
