#include "ZUCmdLine.h"
#include "ZUCmdAssistants.h"
#include "ZUCmdLineErr.h"
//
#include <assert.h>

namespace ZhousUtilities {

/******************************
 * class ZUMCmdOpt's definition *
 ******************************
 */
//
inline ZUMCmdOpt::ZUMCmdOpt(const std::string &fna, const std::string &sna,
                            const std::string &brief, const std::string &detail,
                            unsigned char flags,
                            ZUMCOpt_ArgValidationOper_Ty argValiFunc)
    : m_sFullName(""), m_sShortName(""), m_sBrief(brief), m_sDetail(detail),
      m_flags(0), m_ArgValidOper(nullptr) {
  if (!OptNameValidationF(fna))
    throw ZUCLERR_IllegalName(fna.c_str(), "Illegal full name", __FILE__,
                              __LINE__);
  if (!OptNameValidationS(sna))
    throw ZUCLERR_IllegalName(sna.c_str(), "Illegal short name", __FILE__,
                              __LINE__);
  if (fna == sna)
    throw ZUCLERR_IllegalName(sna.c_str(),
                              "Full name and short name are the same", __FILE__,
                              __LINE__);
  try {
    SetFlags(flags);
  } catch (...) {
    throw;
  }
  m_sFullName = fna;
  m_sShortName = sna;
  m_ArgValidOper = argValiFunc;
}
//
inline ZUMCmdOpt::ZUMCmdOpt()
    : m_sFullName(""), m_sShortName(""), m_sBrief(""), m_sDetail(""),
      m_flags(0), m_ArgValidOper(nullptr) {}
//
inline ZUMCmdOpt::ZUMCmdOpt(const ZUMCmdOpt &opt)
    : ZUMCmdOpt(opt.m_sFullName, opt.m_sShortName, opt.m_sBrief, opt.m_sDetail,
                opt.m_flags, opt.m_ArgValidOper) {}
//
inline void ZUMCmdOpt::SetOptFullName(const std::string &na) {
  if (OptNameValidationF(na)) {
    m_sFullName = na;
  }
}
//
inline void ZUMCmdOpt::SetOptShortName(const std::string &na) {
  if (OptNameValidationS(na)) {
    m_sShortName = na;
  }
}
//
inline void ZUMCmdOpt::SetFlags(unsigned char flag) {
  if ((flag & FlagNoneSet) == FlagNoneSet) {
    m_flags = FlagNoneSet;
    return;
  }
  if (!FlagCheck(flag)) {
    throw ZUCLERR_IllegalOptFlag(
        "Flag setting error,Only use B0~B3. if B3 setted B2 must setted...",
        flag, __FILE__, __LINE__);
  }
  m_flags = flag;
}
//
inline bool ZUMCmdOpt::OptNameMatch(const std::string &na) const {
  assert(!na.empty());
  return m_sFullName == na || m_sShortName == na;
}
//
inline bool ZUMCmdOpt::operator==(const ZUMCmdOpt &opt) const {
  return m_sFullName == opt.m_sFullName || m_sShortName == opt.m_sShortName;
}
//
inline bool ZUMCmdOpt::operator!=(const ZUMCmdOpt &opt) const {
  return !this->operator==(opt);
}
//
inline std::string ZUMCmdOpt::ToStringBrief() const {
  std::stringstream oss;
  oss << "\n****** MainCommandOption Information ******\n";
  oss << "\nFull Name: " << m_sFullName;
  oss << "\nShort Name: " << m_sShortName;
  oss << "\nBrief: " << m_sBrief << "\n";
  return oss.str();
}
//
std::string ZUMCmdOpt::ToString() const {
  std::stringstream oss;
  oss << ToStringBrief();
  oss << GetFlagStr();
  oss << "\nDetail: " << m_sDetail;
  return oss.str();
}
//
bool ZUMCmdOpt::OptNameValidationF(const std::string &na) {
  return OptNameValidationLength(na, ZUMCLimitLiterals::OptFullNameLimit_Min,
                                 ZUMCLimitLiterals::OptFullNameLimit_Max) &&
         OptNameValidationSpChar(na);
}
//
bool ZUMCmdOpt::OptNameValidationS(const std::string &na) {
  if (na.empty())
    return true;
  return OptNameValidationLength(na, ZUMCLimitLiterals::OptShortNameLimit_Min,
                                 ZUMCLimitLiterals::OptShortNameLimit_Max) &&
         OptNameValidationSpChar(na);
}
//
bool ZUMCmdOpt::OptNameValidationLength(const std::string &na, int minLimit,
                                        int maxLimit) {
  auto naLen = na.length();
  return !(naLen < minLimit || naLen > maxLimit);
}
//
bool ZUMCmdOpt::OptNameValidationSpChar(const std::string &na) {
  // 首字符只能为字母,不含InvalidSpecial中的字符
  if (!std::isalpha(na[0])) {
    return false;
  }
  for (auto &c : na) {
    if (ZUMCLimitLiterals::InvalidSpecial.find(c) != std::string::npos) {
      return false;
    }
  }
  return true;
}
//
std::string ZUMCmdOpt::GetFlagStr() const {
  std::stringstream oss;
  int settedCount = 0;
  oss << "\nFlags(Setted): ";
  if ((m_flags & FlagNoneSet) == FlagNoneSet) {
    oss << "NoneSet";
    return oss.str();
  }
  if (IsNecessary()) {
    oss << "Necessary";
    settedCount++;
  }
  if (IsArgAccept()) {
    if (settedCount > 0) {
      oss << " | ";
    }
    oss << "NeedArg";
    settedCount++;
  }
  if (IsMultiArgAccept()) {
    if (settedCount > 0) {
      oss << " | ";
    }
    oss << "MultiArg";
    settedCount++;
  }
  return oss.str();
}
//
inline bool ZUMCmdOpt::FlagCheck(short flag) const {
  if (flag >= FlagIllegal) {
    return false;
  }
  if ((!((flag & FlagNeedArg) == FlagNeedArg)) &&
      ((flag & FlagMultiArg) == FlagMultiArg)) {
    return false;
  }
  return true;
}

//
inline void ZUMCmdOpt::Reset(const ZUMCmdOpt &opt) {
  m_sFullName = opt.m_sFullName;
  m_sShortName = opt.m_sShortName;
  m_sBrief = opt.m_sBrief;
  m_sDetail = opt.m_sDetail;
  m_flags = opt.m_flags;
  m_ArgValidOper = opt.m_ArgValidOper;
}
//
inline void ZUMCmdOpt::operator=(const ZUMCmdOpt &opt) { Reset(opt); }
//
/******************************
 * class ZUMainCmd's definition *
 ******************************
 */
//
ZUMainCmd::ZUMainCmd(const std::string &FullName, const std::string &Brief,
                     const std::string &Detail,
                     const std::vector<ZUMCmdOpt> &CmdOpts,
                     ZUMCExeOper_Ty ExeOper)
    : ZUMCmdOpt(), m_sFullName(""), m_sBrief(Brief), m_sDetail(Detail),
      m_vCmdOpts(CmdOpts), m_ExeOper(ExeOper) {
  if (CmdOpts.empty()) {
    throw ZUCLERR_CmdMustHaveOption(__FILE__, __LINE__);
  }
  if (!ZUMCmdOpt::OptNameValidationF(FullName)) {
    throw ZUCLERR_IllegalName(FullName.c_str(), "Illegal Command name",
                              __FILE__, __LINE__);
  }
  //
  ZUMCmdOpt::Reset(CmdOpts[0]);
  m_sFullName = FullName;
}
//
inline ZUMainCmd::ZUMainCmd(const ZUMainCmd &Cmd)
    : ZUMainCmd(Cmd.m_sFullName, Cmd.m_sBrief, Cmd.m_sDetail, Cmd.m_vCmdOpts,
                Cmd.m_ExeOper) {}
//
inline void ZUMainCmd::SetFullName(const std::string &FullName) {
  if (ZUMCmdOpt::OptNameValidationF(FullName)) {
    m_sFullName = FullName;
  }
}
//
inline size_t ZUMainCmd::SearchOptByName(const std::string &OptName) const {
  for (size_t i = 0; i < m_vCmdOpts.size(); i++) {
    if (m_vCmdOpts[i].OptNameMatch(OptName)) {
      return i;
    }
  }
  return ZUMCLimitLiterals::InvalidPos;
}
//
inline size_t ZUMainCmd::AddOpt(const ZUMCmdOpt &Opt) {
  auto fpos = SearchOptByName(Opt.GetOptFullName());
  if (fpos != ZUMCLimitLiterals::InvalidPos) {
    return m_vCmdOpts.size();
  }
  m_vCmdOpts.push_back(Opt);
  return m_vCmdOpts.size();
}
//
inline size_t ZUMainCmd::RemoveOpt(const std::string &OptName) {
  auto fpos = SearchOptByName(OptName);
  if (fpos == ZUMCLimitLiterals::InvalidPos) {
    return m_vCmdOpts.size();
  }
  m_vCmdOpts.erase(m_vCmdOpts.begin() + fpos);
  return m_vCmdOpts.size();
}
//
inline void ZUMainCmd::ResetOpt(size_t Id, const ZUMCmdOpt &Opt) {
  if (Id >= m_vCmdOpts.size()) {
    throw ZUCLERR_OptNotSupport("No such option", m_sFullName.c_str(), __FILE__,
                                __LINE__);
  }
  m_vCmdOpts[Id] = Opt;
}
//
inline const ZUMCmdOpt &ZUMainCmd::GetOptById(size_t id) const {
  if (id >= m_vCmdOpts.size()) {
    throw ZUCLERR_OptNotSupport("No such option", m_sFullName.c_str(), __FILE__,
                                __LINE__);
  }
  return m_vCmdOpts[id];
}
//
inline ZUMCmdOpt &ZUMainCmd::GetOptById(size_t id) {
  if (id >= m_vCmdOpts.size()) {
    throw ZUCLERR_OptNotSupport("No such option", m_sFullName.c_str(), __FILE__,
                                __LINE__);
  }
  return m_vCmdOpts[id];
}
//
inline const ZUMCmdOpt &ZUMainCmd::operator[](size_t id) const {
  return GetOptById(id);
}
//
inline ZUMCmdOpt &ZUMainCmd::operator[](size_t id) { return GetOptById(id); }
//
std::string ZUMainCmd::ToStringBrief() const {
  std::stringstream oss;
  oss << "\n****** MainCommand Information ******\n";
  oss << "\nFull Name: " << m_sFullName;
  oss << "\nBrief: " << m_sBrief;
  oss << "\nAvailable options:";
  for (auto &opt : m_vCmdOpts) {
    // int width = 15;
    oss << "\n  --" << opt.GetOptFullName();
    // width -= opt.GetOptFullName().length();
    if (!opt.GetOptShortName().empty()) {
      oss << " [-" << opt.GetOptShortName() << "]";
    }
    oss << "  " << opt.GetOptBrief();
  }
  return oss.str();
}
//
std::string ZUMainCmd::ToString() const {
  std::stringstream oss;
  oss << "\n****** MainCommand Information ******\n";
  oss << "\nFull Name: " << m_sFullName;
  oss << "\nBrief: " << m_sBrief;
  oss << "\nDetail: " << m_sDetail;
  oss << "\nAvailable options:";
  for (auto &opt : m_vCmdOpts) {
    oss << "\n  --" << opt.GetOptFullName();
    if (!opt.GetOptShortName().empty()) {
      oss << " [-" << opt.GetOptShortName() << "]";
    }
    oss << "    " << opt.GetOptBrief();
  }
  return oss.str();
}
//
/******************************
 * ZUMCParseResult's definition *
 ******************************
 */
//
std::string ZUMCParseResult::ToString() const {
  std::stringstream oss;
  oss << "\n****** Parse Result ******\n";
  oss << "\nOrigin Command Line: " << m_sOrigin;
  oss << "\nMain Command: " << m_sMainCmd;
  oss << "\nOptions:";
  for (const auto &res : m_vResultColl) {
    oss << "\n  --" << res.first.GetOptFullName();
    if (!res.first.GetOptShortName().empty()) {
      oss << "[-" << res.first.GetOptShortName() << "]";
    }
    oss << "\n  Option Args:";
    for (const auto &a : res.second) {
      oss << a << " ";
    }
  }
  return oss.str();
}
/******************************
 * ZUMCParser's definition *
 ******************************
 */
//
bool ZUMCParser::TryParse(const std::string &originCmdLine,
                          ZUMCParseResult &parseRes, std::string &parseStatus) {
  //
  using namespace ZhousUtilities::ZUMCLimitLiterals;
  using namespace ZhousUtilities::ZUMCAssistants;
  //
  auto strResCollL1 = StringSplitByChar(originCmdLine, ValidSplitCharsL1);
  size_t posCmdFound = InvalidPos;
  auto strResCollL2 = StringSplitByCharWithIgnore(
      strResCollL1[0], ValidSplitCharsL2, IgnoreSplitChars);
  try {
    posCmdFound = ZUMCmdManager::SearchByName(strResCollL2[0]);
    if (posCmdFound == InvalidPos) {
      throw ZUCLERR_MainCmdNotFound(originCmdLine.c_str(), __FILE__, __LINE__);
    }
    ZUMainCmd &mainCmd = ZUMCmdManager::SerachById(posCmdFound);
    ZUMCParseResult tmpRes(originCmdLine, mainCmd.GetFullName());
    auto &parseResColl = tmpRes.GetResultColl();
    ZUMCParseResult::ZUMCParseResultSingle_Ty tmpParseSingle;
    //
    tmpParseSingle = std::move(ParseOpt(mainCmd, strResCollL2, true));
    OptArgsCombine(parseResColl, std::move(tmpParseSingle));
    for (size_t i = 1; i < strResCollL1.size(); i++) {
      strResCollL2 = StringSplitByCharWithIgnore(
          strResCollL1[i], ValidSplitCharsL2, IgnoreSplitChars);
      tmpParseSingle = ParseOpt(mainCmd, strResCollL2, false);
      OptArgsCombine(parseResColl, std::move(tmpParseSingle));
    }
    if (!OptNecessaryCheck(mainCmd, parseResColl)) {
      throw ZUCLERR_NecessaryOptionNotFound(__FILE__, __LINE__);
    }
    parseRes = std::move(tmpRes);
  } catch (ZUCL_ExceptionBase &e) {
    parseStatus =
        std::string("Origin Command line:\n") + originCmdLine + e.what();
    return false;
  }
  parseStatus = "Parse Successed...";
  return true;
}
//
ZUMCParseResult::ZUMCParseResultSingle_Ty
ZUMCParser::ParseOpt(const ZUMainCmd &mainCmd,
                     const std::vector<std::string> &optArgs, bool hTag) {
  ZUMCParseResult::ZUMCParseResultSingle_Ty resSingle;
  if (hTag) {
    resSingle.first = mainCmd.GetSupportedOpts()[0];
  } else {
    size_t posOptFound = mainCmd.SearchOptByName(optArgs[0]);
    //
    if (posOptFound == ZUMCLimitLiterals::InvalidPos) {
      throw ZUCLERR_OptNotSupport(mainCmd.GetFullName().c_str(),
                                  optArgs[0].c_str(), __FILE__, __LINE__);
    }
    resSingle.first = mainCmd.GetOptById(posOptFound);
  }
  //
  if (!ParseOptFlagCheck(resSingle.first, optArgs)) {
    throw ZUCLERR_OptParamIllegal(resSingle.first.GetOptFullName().c_str(),
                                  __FILE__, __LINE__);
  }
  //
  if (resSingle.first.IsArgAccept()) {
    if (resSingle.first.IsMultiArgAccept()) {
      for (size_t i = 1; i < optArgs.size(); i++) {
        resSingle.second.push_back(optArgs[i]);
      }
    } else {
      resSingle.second.push_back(optArgs[1]);
    }
  }
  //
  return resSingle;
}
//
bool ZUMCParser::ParseOptFlagCheck(const ZUMCmdOpt &opt,
                                   const std::vector<std::string> &optArgs) {
  assert(!optArgs.empty());
  size_t argCount = optArgs.size() - 1;
  //
  if (opt.IsArgAccept() && argCount < 1)
    return false;
  if (argCount > 1 && !opt.IsMultiArgAccept())
    return false;
  //
  auto iterBegin = optArgs.begin() + 1;
  return opt.TestArgs(iterBegin, optArgs.end());
}
//
void ZUMCParser::OptArgsCombine(
    ZUMCParseResult::ZUMCParseResultColl_Ty &resColl,
    ZUMCParseResult::ZUMCParseResultSingle_Ty &&resSingle) {
  size_t posOptFound = ZUMCLimitLiterals::InvalidPos;
  for (size_t i = 0; i < resColl.size(); i++) {
    if (resColl[i].first == resSingle.first) {
      posOptFound = i;
      break;
    }
  }
  //
  if (posOptFound == ZUMCLimitLiterals::InvalidPos) {
    resColl.push_back(resSingle);
  } else {
    if (resSingle.first.IsMultiArgAccept() || posOptFound == 0) {
      for (auto &a : resSingle.second) {
        resColl[posOptFound].second.push_back(a);
      }
    } else {
      throw ZUCLERR_OptParamIllegal(resSingle.first.GetOptFullName().c_str(),
                                    __FILE__, __LINE__);
    }
  }
}
//
bool ZUMCParser::OptNecessaryCheck(
    const ZUMainCmd &mainCmd,
    const ZUMCParseResult::ZUMCParseResultColl_Ty &parseResColl) {
  for (const auto &opt : mainCmd.GetSupportedOpts()) {
    if (opt.IsNecessary()) {
      size_t posOptFound = ZUMCLimitLiterals::InvalidPos;
      for (size_t i = 0; i < parseResColl.size(); i++) {
        if (parseResColl[i].first == opt) {
          posOptFound = i;
          break;
        }
      }
      if (posOptFound == ZUMCLimitLiterals::InvalidPos) {
        return false;
      }
    }
  }
  return true;
}
//
/******************************
 * ZUMCmdManager's definition *
 ******************************
 */
//
size_t ZUMCmdManager::SearchByName(const std::string &na) {
  for (size_t i = 0; i < m_sMainCmds.size(); i++) {
    if (m_sMainCmds[i].GetFullName() == na) {
      return i;
    }
  }
  return ZUMCLimitLiterals::InvalidPos;
}

//
size_t ZUMCmdManager::Register(const ZUMainCmd &mainCmd) {
  auto posFound = SearchByName(mainCmd.GetFullName());
  if (posFound != ZUMCLimitLiterals::InvalidPos) {
    throw ZUCLERR_NameConflict(mainCmd.GetFullName().c_str(), __FILE__,
                               __LINE__);
  }
  m_sMainCmds.push_back(mainCmd);
  return m_sMainCmds.size();
}
//
size_t ZUMCmdManager::DeleteById(size_t id) {
  if (id > 0 && id < m_sMainCmds.size()) {
    m_sMainCmds.erase(m_sMainCmds.begin() + id);
  } else {
    throw ZUCLERR_CmdNotRegister(std::to_string(id).c_str(), __FILE__,
                                 __LINE__);
  }
  return m_sMainCmds.size();
}
//
size_t ZUMCmdManager::DeleteByName(const std::string &na) {
  auto posFound = SearchByName(na);
  if (posFound == ZUMCLimitLiterals::InvalidPos) {
    return m_sMainCmds.size();
  }
  m_sMainCmds.erase(m_sMainCmds.begin() + posFound);
  return m_sMainCmds.size();
}
//
bool ZUMCmdManager::ResetById(size_t id, const ZUMainCmd &mainCmd) {
  if (id >= m_sMainCmds.size()) {
    throw ZUCLERR_CmdNotRegister(std::to_string(id).c_str(), __FILE__,
                                 __LINE__);
  }
  m_sMainCmds[id] = mainCmd;
  return true;
}
//
bool ZUMCmdManager::CommandExecute(const ZUMCParseResult &parseRes) {
  using namespace ZhousUtilities::ZUMCLimitLiterals;
  using namespace ZhousUtilities::ZUMCAssistants;
  auto posCmdFound = SearchByName(parseRes.GetMainCmd());
  assert(posCmdFound != InvalidPos);
  auto &mainCmd = SerachById(posCmdFound);
  return mainCmd.Execute(parseRes);
}
//
std::string ZUMCmdManager::GetManInfo() {
  std::stringstream oss;
  oss << "\n****** Command Manager Information ******\n";
  oss << "\nTotal [" << m_sMainCmds.size() << "] Main Commands Registered";
  oss << "\nRegistered Main Commands:";
  for (auto &mc : m_sMainCmds) {
    oss << "\n" << mc.ToString();
  }
  return oss.str();
}

//
/* some static members init */
//
std::vector<ZUMainCmd> ZUMCmdManager::m_sMainCmds;

} // namespace ZhousUtilities
