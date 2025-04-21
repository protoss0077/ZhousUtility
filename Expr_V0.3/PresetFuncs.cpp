#include "PresetFuncs.h"

namespace Expr {
// 数值类型全套比较
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
 * 所有的执行函数假设其参数已通过verifier验证 *
 * 由节点的Calculate接口保证 *
 ***************************
 */
ResultPtr_Ty NOOP(const ArgPtrColl_Ty &argPtrColl, VarTableCPtr_Ty varTable) {
  throw std::runtime_error("执行了无操作的占位符！");
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
      "Factorial can only handle parts smaller than 100！"};
  //
  auto arg1st{argPtrColl[0]->Calculate(varTable)};
  if (arg1st->GetType() != DataType::Value)
    throw std::runtime_error(InvalidDataType + nameof(DataType::Value));
  Number_Ty tarNum = ExprNum::GetValue(arg1st);
  if (tarNum > 100)
    throw std::runtime_error(OutOfFactorialHandlerMsg);
  bool negativeTag{tarNum < 0}; // true:目标为负数
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
    throw std::runtime_error("中缀除法运算中，除数为0.0！");
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
    // 这里也有机会处理，或者推迟到执行函数内
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
  static const std::string brief{"pi/pi() 圆周率"};
  static const std::string detail{
      "pi/pi() "
      "圆周率，无参操作符\n该操作符可作为值使用\n示例：\n...12."
      "34+pi*56.78...\n...12.34+pi()*56.78..."};
  return detailTag ? detail : brief;
}
//
std::string ESNHelper(bool detailTag) {
  static const std::string brief{"e/e() 自然对数"};
  static const std::string detail{
      "e/e() "
      "自然对数，无参操作符\n该操作符可作为值使用\n示例：\n..."
      "12."
      "34+e*56.78...\n...12.34+e()*56.78..."};
  return detailTag ? detail : brief;
}
//
std::string PositivePSHelper(bool detailTag) {
  static const std::string brief{"+[Number|Variable|Expression]，表示正数."};
  static const std::string detail{
      "+[Number]，表示正数."
      "\n只能出现在左括号之后，表达式开头隐含一个左括号\n其他位置会视为错误或中"
      "缀 + ,通常你没必要使用此操作符\n示例：\n +123.456 + "
      "X...\n...(+123.456)..."};
  return detailTag ? detail : brief;
}
std::string AddIDHelper(bool detailTag) {
  static const std::string brief{"[Number|Variable|Expression] + "
                                 "[Number|Variable|Expression],中缀加法运算."};
  static const std::string detail{
      "[Number|Variable|Expression] + "
      "[Number|Variable|Expression],中缀加法运算."
      "\n只能出现在两个表达式之间\n示例：\n ...123.456 + X..."
      "\n...(12.34*56.78) + X...\n...X + Y..."};
  return detailTag ? detail : brief;
}
//
std::string AddPMHelper(bool detailTag) {
  static const std::string brief{"add([number|variable|expression];[number|"
                                 "variable|expression]...)；前缀累加运算符."};
  static const std::string detail{
      "add([number|variable|expression],[number|variable|expression]...)"
      "；前缀累加运算符.只能出现在操作符之后，其必须有一对括号，参数以分隔符','"
      " | ';'分隔\n示例：\n "
      "...add(1,2,3...)\n...add(1,x,y)...\n...add(1,(1+2),y...)..."};
  return detailTag ? detail : brief;
}
//
std::string NegativePSHelper(bool detailTag) {
  static const std::string brief{"-[Number|Variable|Expression]，表示负数."};
  static const std::string detail{
      "-[Number]，表示负数."
      "\n只能出现在左括号之后，表达式开头隐含一个左括号\n其他位置会视为错误或中"
      "缀 - \n示例：\n -123.456 - "
      "X...\n...(-123.456)..."};
  return detailTag ? detail : brief;
}
//
std::string SubIDHelper(bool detailTag) {
  static const std::string brief{"[Number|Variable|Expression] - "
                                 "[Number|Variable|Expression],中缀减法运算."};
  static const std::string detail{
      "[Number|Variable|Expression] - "
      "[Number|Variable|Expression],中缀减法运算."
      "\n只能出现在两个表达式之间\n示例：\n ...123.456 - X..."
      "\n...(12.34*56.78) - X...\n...X - Y..."};
  return detailTag ? detail : brief;
}
//
std::string SubHelper(bool detailTag) {
  static const std::string brief{
      "sub([number|variable|expression];[number|"
      "variable|expression]...)sub；前/后缀减法运算符."};
  static const std::string detail{
      "sub([number|variable|expression],[number|variable|expression]...)sub"
      "；前/"
      "后缀减法运算符.只能出现在操作符之后，其必须有一对括号，参数以分隔符','"
      " | ';'分隔\n示例：\n "
      "...sub(1,2,3...)\n...sub(1,x,y)...\n...sub(1,(1+2),y...)..."};
  return detailTag ? detail : brief;
}
//
std::string AvgHelper(bool detailTag) {
  static const std::string brief{
      "avg([number|variable|expression];[number|"
      "variable|expression]...)avg；前/后缀累加求平均值运算符."};
  static const std::string detail{
      "avg([number|variable|expression],[number|variable|expression]...)avg"
      "；前缀累加求平均值运算符."
      "只能出现在操作符之后，其必须有一对括号，参数以分隔符','"
      " | ';'分隔\n示例：\n "
      "...avg(1,2,3...)\n...avg(1,x,y)...\n...avg(1,(1+2),y...)..."};
  return detailTag ? detail : brief;
}
//
std::string FactSSHelper(bool detailTag) {
  static const std::string brief{
      "[Number|Variable|Expression]!，计算阶乘，浮点数仅计算整数部分."};
  static const std::string detail{
      "[Number|Variable|Expression]!，计算阶乘，浮点数仅计算整数部分."
      "\n只能出现在值、变量或右括号之后\n只能计算100以内的阶乘,"
      "超过100会被视为错误\n其他位置会视为错误\n示例：\n "
      "...123.456!\n...(123.456)!..."};
  return detailTag ? detail : brief;
}
//
std::string MulIDHelper(bool detailTag) {
  static const std::string brief{"[Number|Variable|Expression] * "
                                 "[Number|Variable|Expression],中缀乘法运算."};
  static const std::string detail{
      "[Number|Variable|Expression] * "
      "[Number|Variable|Expression],中缀乘法运算."
      "\n只能出现在两个表达式之间\n示例：\n ...123.456 * X..."
      "\n...(12.34*56.78) * X...\n...X * Y..."};
  return detailTag ? detail : brief;
}
//
std::string DivIDHelper(bool detailTag) {
  static const std::string brief{
      "[Number|Variable|Expression] / "
      "[Number|Variable|Expression],中缀除法运算.除数（right）不能为0."};
  static const std::string detail{
      "[Number|Variable|Expression] / "
      "[Number|Variable|Expression],中缀除法运算.除数（right）不能为0."
      "\n只能出现在两个表达式之间\n示例：\n ...123.456 / X..."
      "\n...(12.34/56.78) / X...\n...X / Y..."};
  return detailTag ? detail : brief;
}
//
std::string PowIDHelper(bool detailTag) {
  static const std::string brief{"[Number|Variable|Expression] ^ "
                                 "[Number|Variable|Expression],中缀乘方运算."};
  static const std::string detail{
      "[Number|Variable|Expression] ^ "
      "[Number|Variable|Expression],中缀乘方运算."
      "\n只能出现在两个表达式之间\n示例：\n ...123.456 ^ X..."
      "\n...(12.34^56.78) ^ X...\n...X ^ Y..."};
  return detailTag ? detail : brief;
}
//
std::string PowHelper(bool detailTag) {
  static const std::string brief{
      "pow([Number|Variable|Expression], "
      "[Number|Variable|Expression]),前/后缀乘方运算."};
  static const std::string detail{
      "pow([Number|Variable|Expression], "
      "[Number|Variable|Expression]),前/后缀乘方运算."
      "\n以前/后缀形式出现\n示例：\n ...pow(123.456 , X)..."
      "\n...(pow(12.34,56.78) , X)pow...\n...pow(X , Y)..."};
  return detailTag ? detail : brief;
}
//
std::string LeftBracketHelper(bool detailTag) {
  static const std::string brief{
      "左括号，可用的左括号: ( [ { ,必须与相应的右括号匹配"};
  static const std::string detail{
      "左括号，可用的左括号: ( [ "
      "{ ,必须与相应的右括号匹配\n对左括号的求值规则为读取符号时必定进符号栈("
      "优先求值)\n遇到右括号时求值到左括号\n左右括号必须匹配，比如 ( 匹配 ) "
      "否则会被视为错误\n"};
  return detailTag ? detail : brief;
}
//
std::string RightBracketHelper(bool detailTag) {
  static const std::string brief{
      "左括号，可用的右括号: ) ] } ,必须与相应的左括号匹配"};
  static const std::string detail{
      "左括号，可用的左括号: ( [ "
      "{,"
      "必须与相应的左括号匹配\n对右括号的求值规则为不进入符号栈\n对栈内符号依次"
      "求值，直到遇到左括号并与之匹配\n左右括号必须匹配，比如 ( 匹配 ) "
      "否则会被视为错误\n"};
  return detailTag ? detail : brief;
}
//
std::string ArgDelimiterHelper(bool detailTag) {
  static const std::string brief{"在使用多个参数时候可用 , 或 ; 分隔参数"};
  static const std::string detail{
      "在使用多个参数时候可用 , 或 ; "
      "分隔参数\n只会在需要多个参数的前/"
      "后缀表达式内部使用，其他地方出现会被视为错误"};
  return detailTag ? detail : brief;
}
} // namespace PresetFunctions
} // namespace Expr