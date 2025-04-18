/* StuSystem.h
 * �ṩһ���ɽ�����ѧ��������Ϣ����ϵͳ
 * �����в���
 * --new �Դ�����ʱ����Դ�ķ�ʽ���뽻��ģʽ(Ĭ��ѡ��)
 * --help ��ʾ���������Ϣ
 * --check SourceFileColl
 * ���ָ������Դ(��ָ�����)�����ݲ�������(����������Դ)
 * --source SourceFileColl
 * ���ָ������Դ(��ָ�����),�����Ҫ���;����һ����ʱ����Դ���ϲ�ָ��Դ����Ч����,���뽻��ģʽ
 * --combine NewSourceFile
 * �ϲ�����Դ�����ϲ�������ݱ��浽ָ���ļ�,������뽻��ģʽ��--sourceͬʱʹ��
 * --compare LeftSourceFile RightSourceFile �����������Դ�Ĳ���
 * --checkout CheckResultFile �����׶εĽ��������ļ�,��ʱ����̨�����Ҫ��Ϣ
 * ����ģʽ�¿�ʹ�õ�����
 * new ������ǰ���ݣ�����һ���µ�����Դ(�����ǰ�༭�����ݲ�Ϊ�ջ���Ѷ�Ƿ񱣴�)
 * open SourceFile ���ָ��������Դ��Ч,������ǰ���ݵ���ָ������Դ
 * append SourceFile ���ָ��������Դ��Ч,��ָ������Դ������׷�ӵ���ǰ����Դ
 * search �ڵ�ǰ����Դ������ָ����֧��������ѧ�Ų���(ͨ�����֧�� * ?)
 * add ����һ���µ�ѧ����Ϣ,�����Ϣ��Ч����ӵ���ǰ����Դ
 * delete ɾ����ǰ����Դ��ָ����ѧ����Ϣ������������ѧ��(ͨ�����֧�� * ?)
 * save [SourceFile] ���浱ǰ����Դ��ָ���ļ�,�����ָ���ļ��򱣴浽��ǰ����Դ
 * quit �˳�����,�����ǰ����Դ����������ʾ�Ƿ񱣴�
 * help ��ʾ����ģʽ�µİ���
 */
#pragma once
//
#include "ZUCmdLine.h"
#include "ZUCmdLineErr.h"
//
#include <regex>
//
namespace StuSystem {
/* Literal & Limit
 * һЩ��������ֵ
 */
static const std::string StuSystemPromptOut = "\nStuSystem |> ";
static const std::string StuSystemPromptIn = "\nStuSystem <| ";
// �����Ƿ��ַ���
static const std::string InvalidNameCharColl =
    "~!@#$%^&*()+{}|:\"<>?`=[]\\;',.0123456789";
// ������������
constexpr static const size_t MinStuNameLength = 2;
constexpr static const size_t MaxStuNameLength = 32;
// ��С�������ڵ�����,�Ǽ�����ʱС�ڴ���ݻᱻ��Ϊ����
constexpr static const size_t MinBirthDate = 195000;
// ����¼��ʱ����ѧ�����5λ�ļ�����,������õ�������Դ�ķ�ʽ
// ����ݵ�ǰ����(ǰ6λ)�뵼������Դѧ��ƥ�䲢���¼�����
// ��ֵ�������StuSystemIdCounterMax
static size_t StuSystemIdCounter = 0;
constexpr static const size_t StuSystemIdCounterMax = 15000;
//
/* Major
 * רҵ
 */
enum class Major : unsigned char {
  Unknown = 0,
  Science = 1,
  Engineering = 2,
  Economics = 3,
  Management = 4,
  Agriculture = 5,
  Medicine = 6,
  Literature = 7,
  History = 8,
  Philosophy = 9,
  Law = 10,
  Art = 11,
  Education = 12
};
// MajorStringColl ��Major��Ӧ���ַ���
static const std::string MajorStringColl[13]{
    "δ֪(Unknown)",     "��ѧ(Science)",      "��ѧ(Engineering)",
    "����ѧ(Economics)", "����ѧ(Management)", "ũѧ(Agriculture)",
    "ҽѧ(Medicine)",    "��ѧ(Literature)",   "��ʷ(History)",
    "��ѧ(Philosophy)",  "��ѧ(Law)",          "����(Art)",
    "����(Education)"};
//
class StudentRecord {
  /* �ļ����л�ʱһ��Ϊһ����¼���ַ�����ʽ�洢
   * ����,��������,רҵ,��ѧ����,ѧ��
   * ",; "������Ϊ�ָ���
   * �ڲ�m_BirthDate��m_EnrollmentDate��yyyy.mm��ʽ�洢
   */
private:
  std::string m_Name;           // ����
  std::string m_BirthDate;      // �������� yyyy.mm
  Major m_Major;                // רҵ����
  std::string m_EnrollmentDate; // ��ѧ���� yyyy.mm
  // ѧ�� ��ʽΪ yyyy(��ѧ��)mm(��ѧ��)##(2λרҵ���)#####(5λ���)
  std::string m_Id;
  //
public:
  StudentRecord(const std::string &na, const std::string &bdate, Major m);
  //
  StudentRecord(const std::string &str);
  //
  std::string ToString() const;
  //
  static StudentRecord FromString(const std::string &str);
  // ���ڸ�ʽƥ�������
  // yyyy��mm�� | yyyy.mm | yyyy-mm
  static std::regex s_DateRegex;

  // ��ʼ����ѧ�����ַ���Ϊ��ǰ����
  static std::string EnrollDateString;
  // ����¼��ʱ����ѧ���� ��ʶ�� yyyy��mm�� yyyy.mm
  // yyyy-mm,�������ú��EnrollDateString
  static const std::string &SetEnrollDateString(const std::string dstr = "");
  //
private:
  //
  StudentRecord();
  //
  /* NameValidation
   * ��֤�����Ƿ���Ч
   */
  static bool NameValidation(const std::string &na);
  /* DateValidation
   * ��֤�����ַ����Ƿ�����DateRegexA/B/C�ĸ�ʽ
   * ���Ϊtrue����Ϊ�����������MinBirthYear����
   * Ϊfalse����Ϊ��ѧ������ӵ�ǰ�������
   */
  static bool DateValidation(const std::string &bdate, bool tag = true);
  /*
   */
  /* GenerateID1stHalf
   * ����ѧ��,��ʽΪ yyyy(��ѧ��)mm(��ѧ��)##(2λרҵ���)#####(5λ���)
   */
  static std::string GenerateID(Major m);
  /* IsIdMatched
   * ѧ��id�Ƿ������ڡ�רҵƥ��
   */
  static bool IsIdMatched(const StudentRecord &stuRd);
  /* DateFmtConvert
   * �������ַ���ͳһת��Ϊyyyy.mm��ʽ
   */
  static std::string DateFmtConvert(const std::string &dstr);

}; // class StudentRecord

//
class ZUCommandStuSystem {
public:
  // ע������
  static void Command_StuSystemRegister();

private:
  /* StuSystemCmdExeOper
   * ִ�к���
   */
  static bool
  StuSystemCmdExeOper(const ZhousUtilities::ZUMCParseResult &parseRes);
  /**/
}; // class ZUCommandStySystem
//
class StuSysErrIllegalParameter final
    : public ZhousUtilities::ZUCL_ExceptionBase {
public:
  StuSysErrIllegalParameter(const char *msgAdd, const char *fna, const int ln)
      : ZUCL_ExceptionBase((std::string("Illegal Parameter When Create "
                                        "StudentRecord Instance\nMessage:") +
                            msgAdd)
                               .c_str(),
                           fna, ln) {}
}; // class StuSysErrIllegalParameter

} // namespace StuSystem