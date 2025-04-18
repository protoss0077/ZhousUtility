/******************************
 * ZUMCmdManager's Init definition *
 ******************************
 */
#include "ZUCmdLine.h"
#include "ZUCmdLineErr.h"
//
#include "CommandsDefinition.h"
//
size_t ZhousUtilities::ZUMCmdManager::Init() {
  try {
    //
    ZUCommandHelp::Command_HelpRegister();
    //

  } catch (...) {
    throw;
  }
  return ZUMCmdManager::GetRegisteredCount();
}