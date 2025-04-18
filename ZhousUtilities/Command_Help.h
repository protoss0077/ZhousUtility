//
#pragma once
//
#include "ZUCmdLine.h"

class ZUCommandHelp {
public:
  static void Command_HelpRegister();

private:
  /* HelpCmdExeOper
   * ִ�к���
   */
  static bool HelpCmdExeOper(const ZhousUtilities::ZUMCParseResult &parseRes);
  /* DetailOptFilter
   * ����detailѡ����ֵ�λ��
   * û����ʱ���� ZhousUtilities::ZUMCLimitLiterals::InvalidPos
   */
  static size_t
  DetailOptFilter(const ZhousUtilities::ZUMCParseResult &parseRes);
  /* NameOptFilter
   * ����nameѡ����ֵ�λ��
   * û����ʱ���� ZhousUtilities::ZUMCLimitLiterals::InvalidPos
   */
  static size_t NameOptFilter(const ZhousUtilities::ZUMCParseResult &parseRes);
}; // class ZUCommandHelp
