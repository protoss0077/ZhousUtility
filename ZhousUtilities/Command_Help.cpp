#include "Command_Help.h"
//
#include "ZUCmdAssistants.h"
#include "ZUCmdLineErr.h"
#include "ZUCmdOptionPreset.h"
//
#include <iostream>

void ZUCommandHelp::Command_HelpRegister() {
  using namespace ZhousUtilities;
  ZUMCmdOpt optList =
      ZUMCmdOpt("list", "l", "�г�������ע�������", "�г�������ע�������",
                ZUMCmdOpt::FlagNecessary, DefValidationOper);
  //
  ZUMCmdOpt optBrief =
      ZUMCmdOpt("brief", "b", "��ʾ����ļ�Ҫ��Ϣ",
                "��ʾ����ļ�Ҫ��Ϣ,���ͬʱ����detail,����Դ���",
                ZUMCmdOpt::FlagNoneSet, DefValidationOper);
  //
  ZUMCmdOpt optDetail =
      ZUMCmdOpt("detail", "d", "��ʾ�������ϸ��Ϣ",
                "��ʾ�������ϸ��Ϣ,���ͬʱ����brief,��brief����Ч",
                ZUMCmdOpt::FlagNoneSet, DefValidationOper);
  //
  ZUMCmdOpt optName = ZUMCmdOpt(
      "name", "n", "��ȡָ���������Ϣ",
      "��ȡָ���������ϸ��Ϣ,����ʹ��ͨ��� *: �����ַ�����;?: �����ַ�",
      ZUMCmdOpt::FlagNeedArg, DefValidationOper);
  //
  ZUMainCmd helpCmd("help", "��ʾ������Ϣ,�г����õ�Command",
                    "��ʾ������Ϣ,�г�����Command�İ�����Ϣ;\ne.g. help "
                    "[--list] [--name] <command> [--brief|--detail]",
                    {optList, optBrief, optDetail, optName}, HelpCmdExeOper);
  //
  ZUMCmdManager::Register(helpCmd);
}
//
bool ZUCommandHelp::HelpCmdExeOper(
    const ZhousUtilities::ZUMCParseResult &parseRes) {
  using namespace ZhousUtilities;
  using ZUMCLimitLiterals::InvalidPos;
  try {
    size_t detailPos = DetailOptFilter(parseRes);
    size_t namePos = NameOptFilter(parseRes);
    if (namePos != InvalidPos) {
      auto &tarSingleRes = parseRes.GetResultColl()[namePos];
      auto &pattern = tarSingleRes.second[0];
      std::vector<ZUMainCmd> matchedCmdColl;
      for (const auto &mc : ZUMCmdManager::GetManData()) {
        if (ZUMCAssistants::WildcardMatch(mc.GetFullName(), pattern)) {
          matchedCmdColl.push_back(mc);
        }
      }
      //
      if (matchedCmdColl.empty()) {
        std::cout << "No Command Matched\n";
      } else {
        for (const auto &mc : matchedCmdColl) {
          if (detailPos != InvalidPos) {
            std::cout << mc.ToString() << std::endl;
          } else {
            std::cout << mc.ToStringBrief() << std::endl;
          }
        }
      }
      //
      return true;
    }
    //
    if (detailPos != InvalidPos) {
      for (const auto &mc : ZUMCmdManager::GetManData()) {
        std::cout << mc.ToString() << std::endl;
      }
    } else {
      for (const auto &mc : ZUMCmdManager::GetManData()) {
        std::cout << mc.ToStringBrief() << std::endl;
      }
    }
    //
  } catch (ZUCL_ExceptionBase &e) {
    std::cerr << e.what() << std::endl;
    return false;
  }
  return true;
}
//
size_t ZUCommandHelp::DetailOptFilter(
    const ZhousUtilities::ZUMCParseResult &parseRes) {
  using namespace ZhousUtilities;
  using ZUMCLimitLiterals::InvalidPos;
  size_t res = 0;
  for (const auto &s : parseRes.GetResultColl()) {
    if (s.first.GetOptFullName() == "detail") {
      return res;
    }
    res++;
  }
  return InvalidPos;
}
//
size_t
ZUCommandHelp::NameOptFilter(const ZhousUtilities::ZUMCParseResult &parseRes) {
  using namespace ZhousUtilities;
  using ZUMCLimitLiterals::InvalidPos;
  size_t res = 0;
  for (const auto &s : parseRes.GetResultColl()) {
    if (s.first.GetOptFullName() == "name") {
      return res;
    }
    res++;
  }
  return InvalidPos;
}
