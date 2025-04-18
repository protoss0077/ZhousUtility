/*
 */

#include "ZUCmdAssistants.h"
#include "ZUCmdLine.h"
#include "ZUCmdLineErr.h"
#include "ZUCmdOptionPreset.h"
//
#include <iostream>
// 测试函数声明
void TestCmdCreate();
bool HelpCmdOper(const ZhousUtilities::ZUMCParseResult &parseRes);
//
void TestCmdError();
//
void TestCustomerIOM();
//
void TestHelpCommand();

int main(int argc, char *argv[]) {
  try {
    ZhousUtilities::ZUMCmdManager::Init();
    //
    // std::stringstream oss;
    // for (int i = 1; i < argc; i++) {
    //  oss << argv[i] << " ";
    //}
    // std::string cmdLineStr = oss.str();
    // std::cout << cmdLineStr << std::endl;
    // std::string parseStatus;
    // ZhousUtilities::ZUMCParseResult parseRes;
    ////
    // if (ZhousUtilities::ZUMCParser::TryParse(cmdLineStr, parseRes,
    //                                          parseStatus)) {
    //   std::cout << parseStatus;
    //   std::cout << parseRes.ToString();
    //   //
    //   if (ZhousUtilities::ZUMCmdManager::CommandExecute(parseRes)) {
    //     std::cout << "Command Execute Success\n";
    //   } else {
    //     std::cout << "Command Execute Failed\n";
    //   }
    // } else {
    //   std::cout << parseStatus;
    // }
    //
    TestHelpCommand();
    //
  } catch (ZhousUtilities::ZUCL_ExceptionBase &ze) {
    std::cout << ze.what() << std::endl;
  }
  //
  return 0;
}

void TestCmdCreate() {
  using namespace ZhousUtilities;
  ZUMCmdOpt optList("list", "l", "Display a list of information",
                    "Display a list of information if the command supported",
                    ZUMCmdOpt::FlagNecessary, DefValidationOper);
  std::cout << optList.ToString() << std::endl;
  ZUMCmdOpt optBrief("brief", "b", "Display a brief of information",
                     "Display a brief of information if the command supported",
                     ZUMCmdOpt::FlagNoneSet, DefValidationOper);
  std::cout << optBrief.ToString() << std::endl;
  ZUMCmdOpt optDetail(
      "detail", "d", "Display a detail of information",
      "Display a detail of information if the command supported",
      ZUMCmdOpt::FlagNoneSet, DefValidationOper);
  std::cout << optDetail.ToString() << std::endl;
  ZUMCmdOpt optName("name", "n", "Display a name of information",
                    "Display a name of information if the command supported",
                    ZUMCmdOpt::FlagNeedArg, DefValidationOper);
  std::cout << optName.ToString() << std::endl;
  ZUMainCmd helpCmd("help", "Display help information",
                    "Display help information if the command supported",
                    {optList, optBrief, optDetail, optName}, HelpCmdOper);
  std::cout << helpCmd.ToString() << std::endl;
  ZUMCmdManager::Register(helpCmd);
  std::cout << ZUMCmdManager::GetManInfo();
  //
  std::string testCmdLine = "help -brief -name st*";
  std::string parseStatus;
  ZUMCParseResult parseRes;
  ZUMCParser::TryParse(testCmdLine, parseRes, parseStatus);
  std::cout << parseStatus;

  std::cout << parseRes.ToString();
}

void TestCmdError() {
  using namespace ZhousUtilities;
  try {
    throw ZUCLERR_CmdNotRegister("Command not register", __FILE__, __LINE__);
  } catch (const ZUCLERR_CmdNotRegister &e) {
    std::cout << e.what() << std::endl;
  }
}
//
bool HelpCmdOper(const ZhousUtilities::ZUMCParseResult &parseRes) {
  std::cout << "\nHelp Command Operate:";
  std::cout << "\nParse Result:";
  std::cout << parseRes.ToString();
  return true;
}
//
void TestCustomerIOM() {
  using namespace ZhousUtilities::ZUMCAssistants;
  std::string testStr = "\nThis is a test String...";
  std::cout << IOMTextStyle(IOMTextStyle::StyleDelete |
                                IOMTextStyle::StyleItalic |
                                IOMTextStyle::StyleUnderline,
                            90, 190);
  std::cout << testStr;
  //
  std::cout << IOMTextStyle(
      IOMTextStyle::StyleNone | IOMTextStyle::StyleDelete |
          IOMTextStyle::StyleItalic | IOMTextStyle::StyleUnderline,
      90, 190);
  std::cout << testStr;
  //
  std::cout << IOMTextStyle(IOMTextStyle::StyleUnderline, 190, 90);
  std::cout << testStr;
  //
  std::cout << IOMTextStyle(IOMTextStyle::StyleUnderline, {12, 36, 122},
                            {122, 36, 12});
  std::cout << testStr;
  std::cout << IOMTextStyle();
  std::cout << testStr;
}
//
void TestHelpCommand() {
  std::vector<std::string> testColl = {
      "help",
      "help -l",
      "help --list",
      "help -n help",
      "help -n h* ",
      "help -n *lp",
      "help --name",
      "help --brief",
      "help --name h* --detail",
      "help --list --name *lp --brief --detail"};
  //
  for (const auto &s : testColl) {
    std::string parseStatus;
    ZhousUtilities::ZUMCParseResult parseRes;
    if (ZhousUtilities::ZUMCParser::TryParse(s, parseRes, parseStatus)) {
      std::cout << parseStatus;
      std::cout << parseRes.ToString();
      //
      if (ZhousUtilities::ZUMCmdManager::CommandExecute(parseRes)) {
        std::cout << "Command Execute Success\n";
      } else {
        std::cout << "Command Execute Failed\n";
      }
    } else {
      std::cout << parseStatus;
    }
  }
}