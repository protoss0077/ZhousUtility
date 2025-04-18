/******************************
 * 通过 ZUCL_ExceptionBase 派生的异常类 *
 * 目前可预见的异常: *
 * 初期化: *
 * 注册的命令命名规则不满足 *
 * 冲突的命令名 *
 * 选项命名规则不满足 *
 * 选项已注册 *
 * 解析: *
 * 命令名不合法 *
 * 命令未注册 *
 * 选项名不合法 *
 * 命令不支持的选项 *
 * 选项参数不合法 *
 ******************************
 */
#pragma once

#include "ZUCmdLine.h"
//
#include <exception>
#include <sstream>
#include <string>

namespace ZhousUtilities {
/******************************
 * class ZUCL_ExceptionBase *
 * ZUCmdLine使用的异常基类 *
 * 目的：提供更强的定位信息以及更多的错误信息 *
 ******************************
 */
class ZUCL_ExceptionBase : public std::exception {
protected:
  std::string m_sErrMessage;
  //
  virtual void SetErrMessage(const char *msg, const char *fn, const int ln) {
    std::ostringstream oss;
    oss << "\nAn error Ocurred at file:" << fn << "\nLine: [" << ln
        << "]\nError Message:\n"
        << m_sErrMessage;
    m_sErrMessage = oss.str();
  }

public:
  ZUCL_ExceptionBase(const char *msg, const char *fna, const int ln)
      : exception(msg), m_sErrMessage() {
    SetErrMessage(msg, fna, ln);
  }
  //
  const char *what() const noexcept override { return m_sErrMessage.c_str(); }
}; // class ZUCL_ExceptionBase

/*
 * ZUCLERR_IllegalName *
 * 用于非法的Command/Option名(构建时) *
 */
class ZUCLERR_IllegalName final : public ZUCL_ExceptionBase {
private:
  std::string m_sErrName;

public:
  ZUCLERR_IllegalName(const char *msgAdd, const char *ena, const char *fna,
                      const int ln)
      : ZUCL_ExceptionBase((std::string("\nIllegalName Error: ") + ena +
                            "\nAdditional:\n" + msgAdd)
                               .c_str(),
                           fna, ln),
        m_sErrName(ena) {}
}; // class ZUCLERR_IllegalName
/* ZUCLERR_IllegalOptFlag *
 * 用于非法的选项标志异常(构建时) *
 */
class ZUCLERR_IllegalOptFlag final : public ZUCL_ExceptionBase {
private:
  short m_flag;
  //
public:
  ZUCLERR_IllegalOptFlag(const char *msgAdd, short flag, const char *fna,
                         const int ln)
      : ZUCL_ExceptionBase((std::string("\nIllegal Option's Flag:") +
                            std::to_string(flag) + "\nAdditional:\n" + msgAdd)
                               .c_str(),
                           fna, ln),
        m_flag(flag) {}
}; // class ZUCLERR_IllegalOptFlag
/* ZUCLERR_CmdMustHaveOption *
 * 命令至少有1个选项(注册时) *
 */
class ZUCLERR_CmdMustHaveOption final : public ZUCL_ExceptionBase {
public:
  ZUCLERR_CmdMustHaveOption(const char *fna, const int ln)
      : ZUCL_ExceptionBase("\nCommand Must Have Option:", fna, ln) {}
}; // class ZUCLERR_CmdMustHaveOption

/* ZUCLERR_NameConflict *
 * 与已注册的命令名冲突(注册时) *
 */
class ZUCLERR_NameConflict final : public ZUCL_ExceptionBase {
public:
  ZUCLERR_NameConflict(const char *tna, const char *fna, const int ln)
      : ZUCL_ExceptionBase(
            (std::string("\nMain Command or Option Name Conflict:") + tna)
                .c_str(),
            fna, ln) {}

}; // class ZUCLERR_NameConflict
/* ZUCLERR_MainCmdNotFound *
 * 未找到主命令(解析时) *
 */
class ZUCLERR_MainCmdNotFound final : public ZUCL_ExceptionBase {
public:
  ZUCLERR_MainCmdNotFound(const char *origin, const char *fna, const int ln)
      : ZUCL_ExceptionBase((std::string("\nMain Command Not Found:") +
                            "\nCheck origin command line:\n" + origin)
                               .c_str(),
                           fna, ln) {}

private:
  std::string m_originCmdLine;
}; // class ZUCLERR_MainCmdNotFound

/* ZUCLERR_CmdNotRegister *
 * 未注册的命令 *
 */
class ZUCLERR_CmdNotRegister final : public ZUCL_ExceptionBase {
public:
  ZUCLERR_CmdNotRegister(const char *tna, const char *fna,
                         const int ln)
      : ZUCL_ExceptionBase((std::string("\nCommand Not Register:") + tna)
                               .c_str(),
                           fna, ln){}
}; // class ZUCLERR_CmdNotRegister
/* ZUCLERR_OptNotSupport *
 * 某个命令不支持某个选项 *
 */
class ZUCLERR_OptNotSupport final : public ZUCL_ExceptionBase {
public:
  ZUCLERR_OptNotSupport(const char *msgAdd, const char *tna, const char *fna,
                        const int ln)
      : ZUCL_ExceptionBase((std::string("\nOption Not Support:") + tna +
                            "\nAdditional:\n" + msgAdd)
                               .c_str(),
                           fna, ln),
        m_sTargetName(tna) {}
  //
private:
  std::string m_sTargetName;
}; // class ZUCLERR_OptNotSupport
/* ZUCLERR_OptParamIllegal *
 * 某个选项参数不合法,选项参数验证时 *
 */
class ZUCLERR_OptParamIllegal final : public ZUCL_ExceptionBase {
public:
  ZUCLERR_OptParamIllegal(const char *ona, const char *fna, const int ln)
      : ZUCL_ExceptionBase(
            (std::string("The parameters provided to the options are "
                         "incorrect...\nEntered Option:") +
             ona)
                .c_str(),
            fna, ln),
        m_OriginOptName(ona) {}
  //

private:
  std::string m_OriginOptName;

}; // class ZUCLERR_OptParamIllegal
/* ZUCLERR_NecessaryOptionNotFound
 * 命令行中未找到必要的option及其参数
 */
class ZUCLERR_NecessaryOptionNotFound : public ZUCL_ExceptionBase {
public:
  ZUCLERR_NecessaryOptionNotFound(const char *fna, const int ln)
      : ZUCL_ExceptionBase("Necessary Option not found...", fna, ln) {}
}; // class ZUCLERR_NecessaryOptionNotFound


/**命令未注册 **选项名不合法 **命令不支持的选项 **选项参数不合法 */

} // namespace ZhousUtilities
