//
#pragma once
//
#include "ZUCmdLine.h"

class ZUCommandHelp {
public:
  static void Command_HelpRegister();

private:
  /* HelpCmdExeOper
   * 执行函数
   */
  static bool HelpCmdExeOper(const ZhousUtilities::ZUMCParseResult &parseRes);
  /* DetailOptFilter
   * 测试detail选项出现的位置
   * 没出现时返回 ZhousUtilities::ZUMCLimitLiterals::InvalidPos
   */
  static size_t
  DetailOptFilter(const ZhousUtilities::ZUMCParseResult &parseRes);
  /* NameOptFilter
   * 测试name选项出现的位置
   * 没出现时返回 ZhousUtilities::ZUMCLimitLiterals::InvalidPos
   */
  static size_t NameOptFilter(const ZhousUtilities::ZUMCParseResult &parseRes);
}; // class ZUCommandHelp
