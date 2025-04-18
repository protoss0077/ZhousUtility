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
      ZUMCmdOpt("list", "l", "列出所有已注册的命令", "列出所有已注册的命令",
                ZUMCmdOpt::FlagNecessary, DefValidationOper);
  //
  ZUMCmdOpt optBrief =
      ZUMCmdOpt("brief", "b", "显示命令的简要信息",
                "显示命令的简要信息,如果同时设置detail,则忽略此项",
                ZUMCmdOpt::FlagNoneSet, DefValidationOper);
  //
  ZUMCmdOpt optDetail =
      ZUMCmdOpt("detail", "d", "显示命令的详细信息",
                "显示命令的详细信息,如果同时设置brief,则brief不生效",
                ZUMCmdOpt::FlagNoneSet, DefValidationOper);
  //
  ZUMCmdOpt optName = ZUMCmdOpt(
      "name", "n", "获取指定命令的信息",
      "获取指定命令的详细信息,可以使用通配符 *: 任意字符序列;?: 单个字符",
      ZUMCmdOpt::FlagNeedArg, DefValidationOper);
  //
  ZUMainCmd helpCmd("help", "显示帮助信息,列出可用的Command",
                    "显示帮助信息,列出可用Command的帮助信息;\ne.g. help "
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
