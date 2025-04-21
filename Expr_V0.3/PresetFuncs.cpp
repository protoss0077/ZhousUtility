#include "PresetFuncs.h"

namespace Expr {
// ��ֵ����ȫ�ױȽ�
// ==
bool NumEQ(const Number_Ty &left, const Number_Ty &right) {
  return left < (right + NumErrorPrecision) &&
         left > (right - NumErrorPrecision);
}
// !=
bool NumNE(const Number_Ty &left, const Number_Ty &right) {
  return !NumEQ(left, right);
}
// <
bool NumLT(const Number_Ty &left, const Number_Ty &right) {
  return left < (right - NumErrorPrecision);
}
// <=
bool NumLE(const Number_Ty &left, const Number_Ty &right) {
  return left < right + NumErrorPrecision;
}
// >
bool NumGT(const Number_Ty &left, const Number_Ty &right) {
  return !NumLE(left, right);
}
// >=
bool NumGE(const Number_Ty &left, const Number_Ty &right) {
  return !NumLT(left, right);
}
//
namespace PresetFunctions {

/***************************
 * preset Executers *
 * ���е�ִ�к��������������ͨ��verifier��֤ *
 * �ɽڵ��Calculate�ӿڱ�֤ *
 ***************************
 */
ResultPtr_Ty NOOP(const ArgPtrColl_Ty &argPtrColl, VarTableCPtr_Ty varTable) {
  throw std::runtime_error("ִ�����޲�����ռλ����");
  return std::static_pointer_cast<ExprDataBase>(std::make_shared<ExprNum>(NAN));
}
//
ResultPtr_Ty PINone(const ArgPtrColl_Ty &argPtrColl, VarTableCPtr_Ty varTable) {
  return std::static_pointer_cast<ExprDataBase>(std::make_shared<ExprNum>(
      static_cast<Number_Ty>(3.14159265358979323846)));
}

ResultPtr_Ty ENone(const ArgPtrColl_Ty &argPtrColl, VarTableCPtr_Ty varTable) {
  return std::static_pointer_cast<ExprDataBase>(std::make_shared<ExprNum>(
      static_cast<Number_Ty>(2.71828182845904523536)));
}
//
ResultPtr_Ty PositiveSingle(const ArgPtrColl_Ty &argPtrColl,
                            VarTableCPtr_Ty varTable) {
  auto resPtr = argPtrColl[0]->Calculate(varTable);
  if (resPtr->GetType() != DataType::Value)
    throw std::runtime_error(InvalidDataType + nameof(DataType::Value));
  return resPtr;
}
//
ResultPtr_Ty AddDouble(const ArgPtrColl_Ty &argPtrColl,
                       VarTableCPtr_Ty varTable) {
  ResultPtr_Ty lresPtr{argPtrColl[0]->Calculate(varTable)};
  ResultPtr_Ty rresPtr{argPtrColl[1]->Calculate(varTable)};
  //
  if (lresPtr->GetType() != DataType::Value ||
      rresPtr->GetType() != DataType::Value)
    throw std::runtime_error(InvalidDataType + nameof(DataType::Value));
  auto resPtr = std::make_shared<ExprNum>(ExprNum::GetValue(lresPtr) +
                                          ExprNum::GetValue(rresPtr));
  return std::static_pointer_cast<ExprDataBase>(resPtr);
}
//
ResultPtr_Ty AddMultiple(const ArgPtrColl_Ty &argPtrColl,
                         VarTableCPtr_Ty varTable) {
  Number_Ty resNum{0};
  for (const auto &r : argPtrColl) {
    auto tmp{r->Calculate(varTable)};
    if (tmp->GetType() != DataType::Value)
      throw std::runtime_error(InvalidDataType + nameof(DataType::Value));
    resNum += ExprNum::GetValue(tmp);
  }
  return std::static_pointer_cast<ExprDataBase>(
      std::make_shared<ExprNum>(resNum));
}
//
ResultPtr_Ty NegativeSingle(const ArgPtrColl_Ty &argPtrColl,
                            VarTableCPtr_Ty varTable) {
  auto argResPtr = argPtrColl[0]->Calculate(varTable);
  if (argResPtr->GetType() != DataType::Value)
    throw std::runtime_error(InvalidDataType + nameof(DataType::Value));
  Number_Ty resNum = ExprNum::GetValue(argResPtr);
  return std::static_pointer_cast<ExprDataBase>(
      std::make_shared<ExprNum>(resNum));
}
//
ResultPtr_Ty SubDouble(const ArgPtrColl_Ty &argPtrColl,
                       VarTableCPtr_Ty varTable) {
  ResultPtr_Ty lresPtr{argPtrColl[0]->Calculate(varTable)};
  ResultPtr_Ty rresPtr{argPtrColl[1]->Calculate(varTable)};
  //
  if (lresPtr->GetType() != DataType::Value ||
      rresPtr->GetType() != DataType::Value)
    throw std::runtime_error(InvalidDataType + nameof(DataType::Value));
  auto resPtr = std::make_shared<ExprNum>(ExprNum::GetValue(lresPtr) -
                                          ExprNum::GetValue(rresPtr));
  return std::static_pointer_cast<ExprDataBase>(resPtr);
}
//
ResultPtr_Ty SubMultiple(const ArgPtrColl_Ty &argPtrColl,
                         VarTableCPtr_Ty varTable) {
  auto arg1st{argPtrColl[0]->Calculate(varTable)};
  if (arg1st->GetType() != DataType::Value)
    throw std::runtime_error(InvalidDataType + nameof(DataType::Value));
  Number_Ty resNum{ExprNum::GetValue(arg1st)};
  for (size_t cnt = 1; cnt < argPtrColl.size(); ++cnt) {
    auto tmp{argPtrColl[cnt]->Calculate(varTable)};
    if (tmp->GetType() != DataType::Value)
      throw std::runtime_error(InvalidDataType + nameof(DataType::Value));
    resNum -= ExprNum::GetValue(tmp);
  }
  return std::static_pointer_cast<ExprDataBase>(
      std::make_shared<ExprNum>(resNum));
}
//
ResultPtr_Ty AvgMultiple(const ArgPtrColl_Ty &argPtrColl,
                         VarTableCPtr_Ty varTable) {
  Number_Ty resNum{0};
  for (const auto &r : argPtrColl) {
    auto tmp{r->Calculate(varTable)};
    if (tmp->GetType() != DataType::Value)
      throw std::runtime_error(InvalidDataType + nameof(DataType::Value));
    resNum += ExprNum::GetValue(tmp);
  }
  return std::static_pointer_cast<ExprDataBase>(
      std::make_shared<ExprNum>(resNum / argPtrColl.size()));
}
//
ResultPtr_Ty FactSingle(const ArgPtrColl_Ty &argPtrColl,
                        VarTableCPtr_Ty varTable) {
  static const std::string OutOfFactorialHandlerMsg{
      "Factorial can only handle parts smaller than 100��"};
  //
  auto arg1st{argPtrColl[0]->Calculate(varTable)};
  if (arg1st->GetType() != DataType::Value)
    throw std::runtime_error(InvalidDataType + nameof(DataType::Value));
  Number_Ty tarNum = ExprNum::GetValue(arg1st);
  if (tarNum > 100)
    throw std::runtime_error(OutOfFactorialHandlerMsg);
  bool negativeTag{tarNum < 0}; // true:Ŀ��Ϊ����
  long long tarNumFix = negativeTag ? -(static_cast<long long>(tarNum))
                                    : static_cast<long long>(tarNum);
  Number_Ty resNum{1};
  for (long long cnt = 2; cnt <= tarNumFix; ++cnt) {
    resNum *= cnt;
  }
  return std::static_pointer_cast<ExprDataBase>(
      std::make_shared<ExprNum>(negativeTag ? -resNum : resNum));
}
//
ResultPtr_Ty MulDouble(const ArgPtrColl_Ty &argPtrColl,
                       VarTableCPtr_Ty varTable) {
  ResultPtr_Ty lresPtr{argPtrColl[0]->Calculate(varTable)};
  ResultPtr_Ty rresPtr{argPtrColl[1]->Calculate(varTable)};
  //
  if (lresPtr->GetType() != DataType::Value ||
      rresPtr->GetType() != DataType::Value)
    throw std::runtime_error(InvalidDataType + nameof(DataType::Value));
  auto resPtr = std::make_shared<ExprNum>(ExprNum::GetValue(lresPtr) *
                                          ExprNum::GetValue(rresPtr));
  return std::static_pointer_cast<ExprDataBase>(resPtr);
}
//
ResultPtr_Ty DivDouble(const ArgPtrColl_Ty &argPtrColl,
                       VarTableCPtr_Ty varTable) {
  ResultPtr_Ty lresPtr{argPtrColl[0]->Calculate(varTable)};
  ResultPtr_Ty rresPtr{argPtrColl[1]->Calculate(varTable)};
  //
  if (lresPtr->GetType() != DataType::Value ||
      rresPtr->GetType() != DataType::Value)
    throw std::runtime_error(InvalidDataType + nameof(DataType::Value));
  if (NumEQ(ExprNum::GetValue(rresPtr), 0.0))
    throw std::runtime_error("��׺���������У�����Ϊ0.0��");
  auto resPtr = std::make_shared<ExprNum>(ExprNum::GetValue(lresPtr) /
                                          ExprNum::GetValue(rresPtr));
  return std::static_pointer_cast<ExprDataBase>(resPtr);
}
//
ResultPtr_Ty PowDouble(const ArgPtrColl_Ty &argPtrColl,
                       VarTableCPtr_Ty varTable) {
  ResultPtr_Ty lresPtr{argPtrColl[0]->Calculate(varTable)};
  ResultPtr_Ty rresPtr{argPtrColl[1]->Calculate(varTable)};
  //
  if (lresPtr->GetType() != DataType::Value ||
      rresPtr->GetType() != DataType::Value)
    throw std::runtime_error(InvalidDataType + nameof(DataType::Value));
  auto resPtr = std::make_shared<ExprNum>(
      pow(ExprNum::GetValue(lresPtr), ExprNum::GetValue(rresPtr)));
  return std::static_pointer_cast<ExprDataBase>(resPtr);
}
/***************************
 * preset arguments verifiers *
 ***************************
 */
bool NoNeedVerifier(const ArgPtrColl_Ty &argPtrColl, VarTableCPtr_Ty varTable) {
  return argPtrColl.empty();
}
//
bool SingleArgVerifier(const ArgPtrColl_Ty &argPtrColl,
                       VarTableCPtr_Ty varTable) {
  if (argPtrColl.size() != 1)
    return false;
  ArgPtr_Ty data1 = argPtrColl[0];
  if (data1->GetType() == Expr::DataType::Variable) {
    auto varPtr1 = std::static_pointer_cast<Expr::ExprVar>(data1);
    auto fIter = varTable->find(varPtr1->GetData());
    if (fIter == varTable->end())
      return false;
  }
  return true;
}
//
bool DoubleArgsVerifier(const ArgPtrColl_Ty &argPtrColl,
                        VarTableCPtr_Ty varTable) {
  if (argPtrColl.size() != 2)
    return false;
  if (argPtrColl[0]->GetType() == Expr::DataType::Variable) {
    auto varPtr = std::static_pointer_cast<Expr::ExprVar>(argPtrColl[0]);
    auto fIter = varTable->find(varPtr->GetData());
    if (fIter == varTable->end())
      return false;
  }
  if (argPtrColl[0]->GetType() == Expr::DataType::Variable) {
    auto varPtr = std::static_pointer_cast<Expr::ExprVar>(argPtrColl[1]);
    auto fIter = varTable->find(varPtr->GetData());
    if (fIter == varTable->end())
      return false;
  }
  return true;
}
//
bool MultipleArgsVerifier(const ArgPtrColl_Ty &argPtrColl,
                          VarTableCPtr_Ty varTable, size_t nargc) {
  if (argPtrColl.empty() || argPtrColl.size() == nargc)
    return false;
  for (size_t cnt = 0; cnt < argPtrColl.size(); ++cnt) {
    if (argPtrColl[cnt]->GetType() == Expr::DataType::Variable) {
      auto varPtr = std::static_pointer_cast<Expr::ExprVar>(argPtrColl[cnt]);
      auto fIter = varTable->find(varPtr->GetData());
      if (fIter == varTable->end())
        return false;
    }
  }
  return true;
}
//
bool ArgumentsCountVerifier(const ArgPtrColl_Ty &argPtrColl,
                            VarTableCPtr_Ty varTab, size_t nargc) {
  switch (nargc) {
  case 0:
    return NoNeedVerifier(argPtrColl, varTab);
  case 1:
    return SingleArgVerifier(argPtrColl, varTab);
  case 2:
    return DoubleArgsVerifier(argPtrColl, varTab);
  default:
    return MultipleArgsVerifier(argPtrColl, varTab, nargc);
  }
  //
}
//
bool DivIDArgsVerifier(const ArgPtrColl_Ty &argPtrColl, VarTableCPtr_Ty varTab,
                       size_t nargc) {
  if (argPtrColl.size() != 2)
    return false;
  if (argPtrColl[0]->GetType() == Expr::DataType::Variable) {
    auto varPtr = std::static_pointer_cast<Expr::ExprVar>(argPtrColl[0]);
    auto fIter = varTab->find(varPtr->GetData());
    if (fIter == varTab->end())
      return false;
  }
  if (argPtrColl[0]->GetType() == Expr::DataType::Variable) {
    auto varPtr = std::static_pointer_cast<Expr::ExprVar>(argPtrColl[1]);
    auto fIter = varTab->find(varPtr->GetData());
    if (fIter == varTab->end())
      return false;
    // ����Ҳ�л��ᴦ�������Ƴٵ�ִ�к�����
    if (NumEQ(fIter->second.front(), 0.0f))
      return false;
  }
  return true;
}

/***************************
 * preset helpers *
 ***************************
 */
std::string PISNHelper(bool detailTag) {
  static const std::string brief{"pi/pi() Բ����"};
  static const std::string detail{
      "pi/pi() "
      "Բ���ʣ��޲β�����\n�ò���������Ϊֵʹ��\nʾ����\n...12."
      "34+pi*56.78...\n...12.34+pi()*56.78..."};
  return detailTag ? detail : brief;
}
//
std::string ESNHelper(bool detailTag) {
  static const std::string brief{"e/e() ��Ȼ����"};
  static const std::string detail{
      "e/e() "
      "��Ȼ�������޲β�����\n�ò���������Ϊֵʹ��\nʾ����\n..."
      "12."
      "34+e*56.78...\n...12.34+e()*56.78..."};
  return detailTag ? detail : brief;
}
//
std::string PositivePSHelper(bool detailTag) {
  static const std::string brief{"+[Number|Variable|Expression]����ʾ����."};
  static const std::string detail{
      "+[Number]����ʾ����."
      "\nֻ�ܳ�����������֮�󣬱��ʽ��ͷ����һ��������\n����λ�û���Ϊ�������"
      "׺ + ,ͨ����û��Ҫʹ�ô˲�����\nʾ����\n +123.456 + "
      "X...\n...(+123.456)..."};
  return detailTag ? detail : brief;
}
std::string AddIDHelper(bool detailTag) {
  static const std::string brief{"[Number|Variable|Expression] + "
                                 "[Number|Variable|Expression],��׺�ӷ�����."};
  static const std::string detail{
      "[Number|Variable|Expression] + "
      "[Number|Variable|Expression],��׺�ӷ�����."
      "\nֻ�ܳ������������ʽ֮��\nʾ����\n ...123.456 + X..."
      "\n...(12.34*56.78) + X...\n...X + Y..."};
  return detailTag ? detail : brief;
}
//
std::string AddPMHelper(bool detailTag) {
  static const std::string brief{"add([number|variable|expression];[number|"
                                 "variable|expression]...)��ǰ׺�ۼ������."};
  static const std::string detail{
      "add([number|variable|expression],[number|variable|expression]...)"
      "��ǰ׺�ۼ������.ֻ�ܳ����ڲ�����֮���������һ�����ţ������Էָ���','"
      " | ';'�ָ�\nʾ����\n "
      "...add(1,2,3...)\n...add(1,x,y)...\n...add(1,(1+2),y...)..."};
  return detailTag ? detail : brief;
}
//
std::string NegativePSHelper(bool detailTag) {
  static const std::string brief{"-[Number|Variable|Expression]����ʾ����."};
  static const std::string detail{
      "-[Number]����ʾ����."
      "\nֻ�ܳ�����������֮�󣬱��ʽ��ͷ����һ��������\n����λ�û���Ϊ�������"
      "׺ - \nʾ����\n -123.456 - "
      "X...\n...(-123.456)..."};
  return detailTag ? detail : brief;
}
//
std::string SubIDHelper(bool detailTag) {
  static const std::string brief{"[Number|Variable|Expression] - "
                                 "[Number|Variable|Expression],��׺��������."};
  static const std::string detail{
      "[Number|Variable|Expression] - "
      "[Number|Variable|Expression],��׺��������."
      "\nֻ�ܳ������������ʽ֮��\nʾ����\n ...123.456 - X..."
      "\n...(12.34*56.78) - X...\n...X - Y..."};
  return detailTag ? detail : brief;
}
//
std::string SubHelper(bool detailTag) {
  static const std::string brief{
      "sub([number|variable|expression];[number|"
      "variable|expression]...)sub��ǰ/��׺���������."};
  static const std::string detail{
      "sub([number|variable|expression],[number|variable|expression]...)sub"
      "��ǰ/"
      "��׺���������.ֻ�ܳ����ڲ�����֮���������һ�����ţ������Էָ���','"
      " | ';'�ָ�\nʾ����\n "
      "...sub(1,2,3...)\n...sub(1,x,y)...\n...sub(1,(1+2),y...)..."};
  return detailTag ? detail : brief;
}
//
std::string AvgHelper(bool detailTag) {
  static const std::string brief{
      "avg([number|variable|expression];[number|"
      "variable|expression]...)avg��ǰ/��׺�ۼ���ƽ��ֵ�����."};
  static const std::string detail{
      "avg([number|variable|expression],[number|variable|expression]...)avg"
      "��ǰ׺�ۼ���ƽ��ֵ�����."
      "ֻ�ܳ����ڲ�����֮���������һ�����ţ������Էָ���','"
      " | ';'�ָ�\nʾ����\n "
      "...avg(1,2,3...)\n...avg(1,x,y)...\n...avg(1,(1+2),y...)..."};
  return detailTag ? detail : brief;
}
//
std::string FactSSHelper(bool detailTag) {
  static const std::string brief{
      "[Number|Variable|Expression]!������׳ˣ���������������������."};
  static const std::string detail{
      "[Number|Variable|Expression]!������׳ˣ���������������������."
      "\nֻ�ܳ�����ֵ��������������֮��\nֻ�ܼ���100���ڵĽ׳�,"
      "����100�ᱻ��Ϊ����\n����λ�û���Ϊ����\nʾ����\n "
      "...123.456!\n...(123.456)!..."};
  return detailTag ? detail : brief;
}
//
std::string MulIDHelper(bool detailTag) {
  static const std::string brief{"[Number|Variable|Expression] * "
                                 "[Number|Variable|Expression],��׺�˷�����."};
  static const std::string detail{
      "[Number|Variable|Expression] * "
      "[Number|Variable|Expression],��׺�˷�����."
      "\nֻ�ܳ������������ʽ֮��\nʾ����\n ...123.456 * X..."
      "\n...(12.34*56.78) * X...\n...X * Y..."};
  return detailTag ? detail : brief;
}
//
std::string DivIDHelper(bool detailTag) {
  static const std::string brief{
      "[Number|Variable|Expression] / "
      "[Number|Variable|Expression],��׺��������.������right������Ϊ0."};
  static const std::string detail{
      "[Number|Variable|Expression] / "
      "[Number|Variable|Expression],��׺��������.������right������Ϊ0."
      "\nֻ�ܳ������������ʽ֮��\nʾ����\n ...123.456 / X..."
      "\n...(12.34/56.78) / X...\n...X / Y..."};
  return detailTag ? detail : brief;
}
//
std::string PowIDHelper(bool detailTag) {
  static const std::string brief{"[Number|Variable|Expression] ^ "
                                 "[Number|Variable|Expression],��׺�˷�����."};
  static const std::string detail{
      "[Number|Variable|Expression] ^ "
      "[Number|Variable|Expression],��׺�˷�����."
      "\nֻ�ܳ������������ʽ֮��\nʾ����\n ...123.456 ^ X..."
      "\n...(12.34^56.78) ^ X...\n...X ^ Y..."};
  return detailTag ? detail : brief;
}
//
std::string PowHelper(bool detailTag) {
  static const std::string brief{
      "pow([Number|Variable|Expression], "
      "[Number|Variable|Expression]),ǰ/��׺�˷�����."};
  static const std::string detail{
      "pow([Number|Variable|Expression], "
      "[Number|Variable|Expression]),ǰ/��׺�˷�����."
      "\n��ǰ/��׺��ʽ����\nʾ����\n ...pow(123.456 , X)..."
      "\n...(pow(12.34,56.78) , X)pow...\n...pow(X , Y)..."};
  return detailTag ? detail : brief;
}
//
std::string LeftBracketHelper(bool detailTag) {
  static const std::string brief{
      "�����ţ����õ�������: ( [ { ,��������Ӧ��������ƥ��"};
  static const std::string detail{
      "�����ţ����õ�������: ( [ "
      "{ ,��������Ӧ��������ƥ��\n�������ŵ���ֵ����Ϊ��ȡ����ʱ�ض�������ջ("
      "������ֵ)\n����������ʱ��ֵ��������\n�������ű���ƥ�䣬���� ( ƥ�� ) "
      "����ᱻ��Ϊ����\n"};
  return detailTag ? detail : brief;
}
//
std::string RightBracketHelper(bool detailTag) {
  static const std::string brief{
      "�����ţ����õ�������: ) ] } ,��������Ӧ��������ƥ��"};
  static const std::string detail{
      "�����ţ����õ�������: ( [ "
      "{,"
      "��������Ӧ��������ƥ��\n�������ŵ���ֵ����Ϊ���������ջ\n��ջ�ڷ�������"
      "��ֵ��ֱ�����������Ų���֮ƥ��\n�������ű���ƥ�䣬���� ( ƥ�� ) "
      "����ᱻ��Ϊ����\n"};
  return detailTag ? detail : brief;
}
//
std::string ArgDelimiterHelper(bool detailTag) {
  static const std::string brief{"��ʹ�ö������ʱ����� , �� ; �ָ�����"};
  static const std::string detail{
      "��ʹ�ö������ʱ����� , �� ; "
      "�ָ�����\nֻ������Ҫ���������ǰ/"
      "��׺���ʽ�ڲ�ʹ�ã������ط����ֻᱻ��Ϊ����"};
  return detailTag ? detail : brief;
}
} // namespace PresetFunctions
} // namespace Expr