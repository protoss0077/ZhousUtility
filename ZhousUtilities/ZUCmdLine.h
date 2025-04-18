/******************************
 * �� System.Commandline �� C++ ʵ�� *
 * ����һЩ�� *
 * ���˼·��ȫ��ά��һ��ZUCmdManager���� *
 * ZUCmdManager �ڲ�ά��ע���ZUMainCmd���� *
 * ͨ�������в��������ڲ�ע���ZUMainCmd ִ�в�ͬ�Ĳ��� *
 * ��helpΪ��
 * ��������ʱ���ȵ���ZUCmdManager::Init
 * ������ZUCmdManager::Registerע�� help ZUMainCmd���� *
 * ������֧�� List/Name/brief/detail ѡ�� *
 * ���� List/brief/detail ѡ���Ҫ���� *
 * Name ѡ��֧��һ�����ڲ�ѯע������������ַ������� *
 * �������д��� help -Name help -brief
 * help ��Ϊ������ -Name ��Ϊ��1��ѡ�� ����� help ��Ϊѡ����� *
 * -brief ��Ϊ��2��ѡ��*
 * ��������н���������Ѿ�ע��� help (ZUMainCmd) ������ *
 * ����ѡ��Ͳ���һ�𴫵ݸ��������Execute��ִ�� *
 * �������������ڿ���̨��ӡ help ����ļ�� *
 ******************************
 */
#pragma once
#include <functional>
#include <string>
#include <vector>

//
namespace ZhousUtilities {
// ǰ������
class ZUMCmdOpt;
class ZUMainCmd;
class ZUMCParseResult;
class ZUMCmdManager;
/******************************
 * class ZUMCmdOpt's declaration *
 * ����ѡ�� *
 * ����������ѡ���� '/' '-' "--" ��ʼ ��������ַ��� *
 * ���ӹ��� -- ֮�����Ϊfullname ,ʹ�� -- ���Ը���Ķ�λ *
 * ��Ա����: *
 * OptFullName(ѡ��ȫ��) *
 * OptShortName(ѡ���д) *
 * OptBrief(ѡ����)
 * OptDetail(ѡ����ϸ��Ϣ���������ⲿ��֮���Ž��ļ���ͨ��������ȡ) *
 * ѡ���Ƿ������� *
 * ѡ���Ƿ���Ҫ���� *
 * ѡ���Ƿ�֧�ֶ������ *
 * ѡ���������֤����ָ�� *
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
  /* ���λ-�Ƿ����
   * b1-�Ƿ���Ҫ����
   * b2-�Ƿ�֧�ֶ������
   */
  unsigned char m_flags;
  ZUMCOpt_ArgValidationOper_Ty m_ArgValidOper;
  //
public:
  // ����
  ZUMCmdOpt(const std::string &fna, const std::string &sna,
            const std::string &brief, const std::string &detail,
            unsigned char flags, ZUMCOpt_ArgValidationOper_Ty argValiFunc);
  //
  ZUMCmdOpt();
  //
  ZUMCmdOpt(const ZUMCmdOpt &opt);
  /* m_flags��λ����
   * ʹ��b0~b3
   */
private:
  static const unsigned char FlagIllegal = 16; // ���ڵ��ڸ�ֵ��flagΪ�Ƿ�
public:
  static const unsigned char FlagNoneSet =
      1; // NoneSet����,���ø�λ���������λ
  static const unsigned char FlagNecessary = 2;
  static const unsigned char FlagNeedArg = 4;
  static const unsigned char FlagMultiArg = 8;
  // �ڲ���Ա������
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
  // flag����
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
   * ���flag�Ƿ�Ϸ�
   * ���ܴ��ڵ���FlagIllegal
   * ����MultiArgʱ��������NeedArg
   */
  bool FlagCheck(short flag) const;
}; // class ZUMCmdOpt
/******************************
 * class ZUMainCmd's declaration *
 * ������̳���ZUMCmdOpt �ڲ���CmdOpt��ʾ��Ĭ�ϵ�ѡ�� *
 * ����ζ�Ź���һ��cmd����Ҫ��һ��opt *
 * ����ʱ��Ĭ�ϵ�ѡ�����ӽ�֧�ֵ�ѡ����� *
 * ����help��Ĭ��ѡ���� -list
 * ����: *
 * fullName ȫ��(ִ�в����ִ�Сд�ıȽ�) string *
 * brief ���;
 * detail ��ϸ��Ϣ������ *
 * ֧�ֵ�ZUMCmdOpt(ѡ��) vector<ZUMCmdOpt> Ϊ��ʱ��ʾ�������Ҫѡ����� *
 * ����ִ��Execute�ӿڵĺ���ָ�� *
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
 * ������� *
 * ZUMCParser������Ľ�� *
 * ����: *
 * ԭʼ�������ַ��� string *
 * �������� string *
 * �����ѡ���������� pair<opt,vector<string>> *
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
  // ����
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
 * �����������������TryParse���н��� *
 ******************************
 */
class ZUMCParser {
public:
  /* TryParse
   * ���Խ���������
   * originCmdLine ԭʼ�������ַ���
   * ParseResult �������
   * parseStatus ����״̬
   * ����falseʱparseStatus��������Ĵ�����Ϣ
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
  /* hTagָʾ��1��������������(true)/ѡ����(false)
   */
  static ZUMCParseResult::ZUMCParseResultSingle_Ty
  ParseOpt(const ZUMainCmd &mainCmd, const std::vector<std::string> &optArgs,
           bool hTag);
  /* ParseOptFlagCheck
   * ����flag����������,!!!�����ò�����֤����
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
 * ����ע���ZUMainCmd���� *
 * �ڲ�ά��һ��vector<ZUMainCmd> *
 * ע���������Ԥ��� *
 * ���� ZUMCmdManager::Init ����������ע�ᵽ�ڲ����� *
 * Ϊ�˱�����չInit��ʵ�ַ���ZUMCmdManagerImpl.cpp�� *
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
