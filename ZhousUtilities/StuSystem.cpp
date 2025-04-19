#include "StuSystem.h"
//
#include "ZUCmdAssistants.h"
#include "ZUCmdLineErr.h"
#include "ZUCmdOptionPreset.h"
//
namespace StuSystem {
void ZUCommandStuSystem::Command_StuSystemRegister() {
  using namespace ZhousUtilities;
  // --new
  ZUMCmdOpt optNew = ZUMCmdOpt("new", "n", "创建一个空的数据源并进入交互模式",
                               "以创建临时数据源的方式进入交互模式",
                               ZUMCmdOpt::FlagNoneSet, DefValidationOper);
  // --help
  ZUMCmdOpt optHelp = ZUMCmdOpt("help", "h", "列出stusystem所有可用的选项",
                                "列出stusystem所有可用的选项,可以使用 "
                                "brief显示简要介绍|detail显示详细介绍",
                                ZUMCmdOpt::FlagNoneSet, DefValidationOper);
  // --check
  ZUMCmdOpt optCheck = ZUMCmdOpt(
      "check", "c", "检查指定数据源的数据", "检查指定数据源的数据",
      ZUMCmdOpt::FlagNeedArg | ZUMCmdOpt::FlagMultiArg, FileCollValidationOper);
  // --source
  ZUMCmdOpt optSource = ZUMCmdOpt(
      "source", "s",
      "指定数据源并导入;随后进入交互模式;和--combine一起使用时输出到文件",
      "导入指定的数据源，可指定多个数据源；合并导入结果;如果和--"
      "combine一起使用时输出到文件;没有其他关联选项时，进入交互模式;",
      ZUMCmdOpt::FlagNeedArg | ZUMCmdOpt::FlagMultiArg, FileCollValidationOper);
  // --combine
  ZUMCmdOpt optCombine = ZUMCmdOpt(
      "combine", "", "合并数据源，与--source配合使用",
      "指定--source时合并数据源将结果保存到指定文件,此时不会进入交互模式",
      ZUMCmdOpt::FlagNeedArg, FileValidationOper);
  // --compare
  ZUMCmdOpt optCompare = ZUMCmdOpt(
      "compare", "", "比较两个数据源的差异", "比较两个数据源的差异",
      ZUMCmdOpt::FlagNeedArg | ZUMCmdOpt::FlagMultiArg, FileCollValidationOper);
  // --checkout
  ZUMCmdOpt optCheckout = ZUMCmdOpt(
      "checkout", "", "将检查阶段的结果输出到文件",
      "将检查阶段的结果输出到文件", ZUMCmdOpt::FlagNeedArg, FileValidationOper);
  // command stusystem
  ZUMainCmd stusysCmd("stusystem", "学生信息管理系统", "学生信息管理系统",
                      {optNew, optHelp, optCheck, optSource, optCombine,
                       optCompare, optCheckout},
                      StuSystemCmdExeOper);
  //
  ZUMCmdManager::Register(stusysCmd);
}
//
bool ZUCommandStuSystem::StuSystemCmdExeOper(
    const ZhousUtilities::ZUMCParseResult &parseRes) {
  return true;
}
//
/******************************
 * StudentRecord's definition *
 ******************************
 */
//
StudentRecord::StudentRecord(const std::string &na, const std::string &bdate,
                             Major m)
    : m_Name(""), m_BirthDate(""), m_Major(Major::Unknown),
      m_EnrollmentDate(""), m_Id("") {
  if (!NameValidation(na)) {
    throw StuSysErrIllegalParameter(
        (std::string("NameValidation Failed:") + na).c_str(), __FILE__,
        __LINE__);
  }
  if (!DateValidation(bdate)) {
    throw StuSysErrIllegalParameter(
        (std::string("DateValidation Failed:") + bdate).c_str(), __FILE__,
        __LINE__);
  }
  m_Name = na;
  m_BirthDate = DateFmtConvert(bdate);
  m_Major = m;
  if (EnrollDateString.empty()) {
    SetEnrollDateString();
  }
  m_EnrollmentDate = EnrollDateString;
  m_Id = GenerateID(m);
}
//
StudentRecord::StudentRecord(const std::string &str) {
  *this = std::move(FromString(str));
}
//
std::string StudentRecord::ToString() const {
  std::ostringstream oss;
  oss << "\n---***--- Student Record ---***---\n";
  oss << "Name: " << m_Name;
  oss << "\nBirthDate: " << m_BirthDate;
  oss << "\nMajor: " << (int)m_Major;
  oss << "\nEnrollmentDate: " << m_EnrollmentDate;
  oss << "\nID: " << m_Id << std::endl;
  return oss.str();
}
//
StudentRecord::StudentRecord()
    : m_Name(""), m_BirthDate(""), m_Major(Major::Unknown),
      m_EnrollmentDate(""), m_Id("") {}
//
StudentRecord StudentRecord::FromString(const std::string &str) {
  using namespace ZhousUtilities;
  using namespace ZUMCAssistants;
  auto tarStr = StringTrim(str, " ,;\n");
  auto strColl = StringSplitByChar(tarStr, " ,;");
  if (strColl.size() != 5) {
    throw StuSysErrIllegalParameter(
        (std::string("StudentRecord Constructor Failed:") + str).c_str(),
        __FILE__, __LINE__);
  }
  //
  if (!NameValidation(strColl[0])) {
    throw StuSysErrIllegalParameter(
        (std::string("NameValidation Failed:") + strColl[0]).c_str(), __FILE__,
        __LINE__);
  }
  //
  if (!DateValidation(strColl[1]) || !DateValidation(strColl[3], false)) {
    throw StuSysErrIllegalParameter(
        (std::string("DateValidation Failed:") + strColl[1]).c_str(), __FILE__,
        __LINE__);
  }
  //
  try {
    Major m = (Major)std::stoi(strColl[2]);
  } catch (std::invalid_argument &e) {
    throw StuSysErrIllegalParameter(
        (std::string("Major Convert Failed:") + strColl[2]).c_str(), __FILE__,
        __LINE__);
  }
  //
  StudentRecord res;
  res.m_Name = strColl[0];
  res.m_BirthDate = DateFmtConvert(strColl[1]);
  res.m_Major = (Major)std::stoi(strColl[2]);
  res.m_EnrollmentDate = DateFmtConvert(strColl[3]);
  res.m_Id = strColl[4];
  if (!IsIdMatched(res)) {
    throw StuSysErrIllegalParameter(
        (std::string("ID Match Failed:") + str).c_str(), __FILE__, __LINE__);
  }
  return res;
}
//
const std::string &StudentRecord::SetEnrollDateString(const std::string dstr) {
  if (dstr.empty() || !DateValidation(dstr, false)) {
    auto now = ZhousUtilities::ZUMCAssistants::GetCurrentTM();
    char buf[8] = "\0";
    sprintf_s(buf, 8, "%04d%02d", now.tm_year + 1900, now.tm_mon + 1);
    EnrollDateString = std::string(buf);
  } else {
    EnrollDateString = DateFmtConvert(dstr);
  }
  return EnrollDateString;
}
//
bool StudentRecord::NameValidation(const std::string &na) {
  size_t naLen = na.size();
  if (naLen < MinStuNameLength || naLen > MaxStuNameLength) {
    return false;
  }
  for (size_t i = 0; i < naLen; ++i) {
    if (InvalidNameCharColl.find(na[i]) != std::string::npos) {
      return false;
    }
  }
  return true;
}
//
bool StudentRecord::DateValidation(const std::string &bdate, bool tag) {
  if (!std::regex_match(bdate, s_DateRegex)) {
    return false;
  }
  size_t dateNum = 0;
  for (auto c : bdate) {
    if (std::isdigit(c)) {
      dateNum *= 10;
      dateNum += c - '0';
    }
  }
  if (tag) {
    if (dateNum < MinBirthDate) {
      return false;
    }
  } else {
    if (EnrollDateString.empty()) {
      SetEnrollDateString();
    }
    size_t enrollNum = 0;
    for (auto c : EnrollDateString) {
      if (std::isdigit(c)) {
        enrollNum *= 10;
        enrollNum += c - '0';
      }
    }
    if (dateNum > enrollNum) {
      return false;
    }
  }
  return true;
}
//
std::string StudentRecord::GenerateID(Major m) {
  if (EnrollDateString.empty()) {
    SetEnrollDateString();
  }
  size_t enrollNum = 0;
  for (auto c : EnrollDateString) {
    if (std::isdigit(c)) {
      enrollNum *= 10;
      enrollNum += c - '0';
    }
  }
  size_t majorNum = (size_t)m;
  size_t idNum = ++StuSystemIdCounter;
  char buf[14] = "\0";
  sprintf_s(buf, 14, "%04zd%02zd%02zd%05zd", enrollNum / 100, enrollNum % 100,
            majorNum, idNum);
  return std::string(buf);
}
//
bool StudentRecord::IsIdMatched(const StudentRecord &stuRd) {
  size_t enrollNum = 0;
  for (auto c : stuRd.m_EnrollmentDate) {
    if (std::isdigit(c)) {
      enrollNum *= 10;
      enrollNum += c - '0';
    }
  }
  size_t majorNum = (size_t)stuRd.m_Major;
  char buf[9] = "\0";
  sprintf_s(buf, 9, "%04zd%02zd%02zd", enrollNum / 100, enrollNum % 100, majorNum);
  for (size_t i = 0; i < 8; ++i) {
    if (buf[i] != stuRd.m_Id[i]) {
      return false;
    }
  }
  return true;
}
//
std::string StudentRecord::DateFmtConvert(const std::string &bdate) {
  size_t dateNum = 0;
  for (auto c : bdate) {
    if (std::isdigit(c)) {
      dateNum *= 10;
      dateNum += c - '0';
    }
  }
  char buf[8] = "\0";
  sprintf_s(buf, 8, "%04zd.%02zd", dateNum / 100, dateNum % 100);
  return std::string(buf);
}
// static members definition
std::regex StudentRecord::s_DateRegex =
    std::regex(R"(\d{4}(年\d{2}月|[.-]\d{2}))");
//
std::string StudentRecord::EnrollDateString = "";
//

} // namespace StuSystem