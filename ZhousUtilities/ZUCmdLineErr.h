/******************************
 * ͨ�� ZUCL_ExceptionBase �������쳣�� *
 * Ŀǰ��Ԥ�����쳣: *
 * ���ڻ�: *
 * ע������������������� *
 * ��ͻ�������� *
 * ѡ�������������� *
 * ѡ����ע�� *
 * ����: *
 * ���������Ϸ� *
 * ����δע�� *
 * ѡ�������Ϸ� *
 * ���֧�ֵ�ѡ�� *
 * ѡ��������Ϸ� *
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
 * ZUCmdLineʹ�õ��쳣���� *
 * Ŀ�ģ��ṩ��ǿ�Ķ�λ��Ϣ�Լ�����Ĵ�����Ϣ *
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
 * ���ڷǷ���Command/Option��(����ʱ) *
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
 * ���ڷǷ���ѡ���־�쳣(����ʱ) *
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
 * ����������1��ѡ��(ע��ʱ) *
 */
class ZUCLERR_CmdMustHaveOption final : public ZUCL_ExceptionBase {
public:
  ZUCLERR_CmdMustHaveOption(const char *fna, const int ln)
      : ZUCL_ExceptionBase("\nCommand Must Have Option:", fna, ln) {}
}; // class ZUCLERR_CmdMustHaveOption

/* ZUCLERR_NameConflict *
 * ����ע�����������ͻ(ע��ʱ) *
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
 * δ�ҵ�������(����ʱ) *
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
 * δע������� *
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
 * ĳ�����֧��ĳ��ѡ�� *
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
 * ĳ��ѡ��������Ϸ�,ѡ�������֤ʱ *
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
 * ��������δ�ҵ���Ҫ��option�������
 */
class ZUCLERR_NecessaryOptionNotFound : public ZUCL_ExceptionBase {
public:
  ZUCLERR_NecessaryOptionNotFound(const char *fna, const int ln)
      : ZUCL_ExceptionBase("Necessary Option not found...", fna, ln) {}
}; // class ZUCLERR_NecessaryOptionNotFound


/**����δע�� **ѡ�������Ϸ� **���֧�ֵ�ѡ�� **ѡ��������Ϸ� */

} // namespace ZhousUtilities
