#include "ExprTypes.h"
//
#include <assert.h>

namespace Expr {
//
std::string GetDataTypeString(DataType ndty) {
  switch (ndty) {
  case DataType::Oper:
    return nameof(DataType::Oper);
  case DataType::OperWithArgs:
    return nameof(DataType::OperWithArgs);
  case DataType::Value:
    return nameof(DataType::Value);
  case DataType::Variable:
    return nameof(DataType::Variable);
  default:
    return "Unknown";
  }
}
//
std::string GetOperPriorityString(OperPriority opp) {
  switch (opp) {
  case OperPriority::Lowest:
    return nameof(OperPriority::Lowest);
  case OperPriority::L1:
    return nameof(OperPriority::L1);
  case OperPriority::L2:
    return nameof(OperPriority::L2);
  case OperPriority::L3:
    return nameof(OperPriority::L3);
  case OperPriority::L4:
    return nameof(OperPriority::L4);
  case OperPriority::L5:
    return nameof(OperPriority::L5);
  case OperPriority::L6:
    return nameof(OperPriority::L6);
  case OperPriority::L7:
    return nameof(OperPriority::L7);
  case OperPriority::L8:
    return nameof(OperPriority::L8);
  case OperPriority::L9:
    return nameof(OperPriority::L9);
  case OperPriority::Highest:
    return nameof(OperPriority::Highest);
  default:
    return "Illegal Priority";
  }
}
//
std::string GetOperCategoryString(OperCategory pos) {
  switch (pos) {
  case OperCategory::Prefix:
    return nameof(OperCategory::Prefix);
  case OperCategory::Infix:
    return nameof(OperCategory::Infix);
  case OperCategory::Suffix:
    return nameof(OperCategory::Suffix);
  case OperCategory::PrefOrSuff:
    return nameof(OperCategory::PrefOrSuff);
  case OperCategory::LBracket:
    return nameof(OperCategory::LBracket);
  case OperCategory::RBracket:
    return nameof(OperCategory::RBracket);
  case OperCategory::Delimiter:
    return nameof(OperCategory::Delimiter);
  default:
    return "Invalid Operator Position";
  }
}
/***************************
 * class ExprNum definition *
 ***************************
 */
ExprNum::ExprNum() : Data{NAN} {}
//
ExprNum::ExprNum(Number_Ty value) : Data{value} {}
//
void ExprNum::operator=(const ExprNum &other) { Data = other.Data; }
//
std::string ExprNum::ToString() const {
  static const char *fmtStr = "Value: %.6lf";
  char buf[32]{"\0"};
  sprintf_s(buf, 32, fmtStr, Data);
  return std::string(buf);
}
//
[[nodiscard]] ResultPtr_Ty ExprNum::Calculate(VarTablePtr_Ty varTable) {
  assert(GetType() == DataType::Value);
  auto res = std::make_shared<ExprNum>(Data);
  return std::static_pointer_cast<ExprDataBase>(res);
}
//
Number_Ty ExprNum::GetValue(ArgPtr_Ty argPtr) {
  if (argPtr->GetType() != DataType::Value)
    throw std::runtime_error(InvalidDataType + nameof(DataType::Value));
  auto numPtr = std::static_pointer_cast<ExprNum>(argPtr);
  return numPtr->GetData();
}
/***************************
 * class ExprVar definition *
 ***************************
 */
ExprVar::ExprVar() : Data{"\0"} {}
//
ExprVar::ExprVar(const std::string &varNa) : Data{"\0"} {

  if (!ExprTeleprompter::IsValidVarName(varNa))
    throw std::runtime_error(InvalidVarNameMsg + varNa);
  Data = varNa;
}
//
ExprVar::ExprVar(std::string &&varNa) : Data{"\0"} {
  if (!ExprTeleprompter::IsValidVarName(varNa))
    throw std::runtime_error(InvalidVarNameMsg + varNa);
  Data = std::move(varNa);
}
//
ExprVar::ExprVar(const ExprVar &other) : Data{other.Data} {}
//
void ExprVar::operator=(const ExprVar &other) { Data = other.Data; }
//
std::string ExprVar::ToString() const {
  static const char *fmtStr = "Variable Name: %s";
  char buf[32]{"\0"};
  sprintf_s(buf, 32, fmtStr, Data.c_str());
  return std::string(buf);
}
//
[[nodiscard]] ResultPtr_Ty ExprVar::Calculate(VarTablePtr_Ty varTable) {
  assert(GetType() == DataType::Variable);
  if (varTable == nullptr)
    throw std::runtime_error(NullVarTableMsg + Data);
  auto tarIter = varTable->find(Data);
  if (tarIter == varTable->end())
    throw std::runtime_error(NoSuchVarNameMsg + Data);
  auto &tarQue = tarIter->second;
  if (tarQue.empty())
    throw std::runtime_error(NoVarDataMsg + Data);
  auto res = tarQue.front();
  // tarQue.pop(); // 表达式最终计算时从队列弹出
  auto resPtr = std::make_shared<ExprNum>(ExprNum(res));
  return std::static_pointer_cast<ExprDataBase>(resPtr);
}
//
Number_Ty ExprVar::GetVarValue(ArgPtr_Ty argPtr, VarTablePtr_Ty varTablePtr) {
  if (argPtr->GetType() != DataType::Variable)
    throw std::runtime_error(InvalidDataType + nameof(DataType::Variable));
  if (varTablePtr == nullptr)
    throw std::runtime_error(NullVarTableMsg);
  auto varPtr = std::static_pointer_cast<ExprVar>(argPtr);
  auto fIter = varTablePtr->find(varPtr->GetData());
  if (fIter == varTablePtr->end())
    throw std::runtime_error(NoSuchVarNameMsg + varPtr->GetData());
  VarValueQueue_Ty varQue = fIter->second;
  if (varQue.empty())
    throw std::runtime_error(NoVarDataMsg + varPtr->GetData());
  Number_Ty res = varQue.front();
  // varQue.pop();
  return res;
}
/***************************
 * class ExprOper definition *
 ***************************
 */
ExprOper::ExprOper(const std::string &msymbol, const std::string &iname,
                   OperCategory pos, size_t nargc, OperPriority npri,
                   OperPriority spri, ArgValidation_Ty argVerifier,
                   OperExecution_Ty exer, OperHelper_Ty helper)
    : MatchSymbol{""}, InternalName{""}, Category{pos}, NeededArgsCount{nargc},
      NormalPriority{npri}, StackPriority{spri}, ArgVerifier{argVerifier},
      Executer{exer}, Helper{helper} {
  if (!ExprTeleprompter::IsValidMatchSymbol(msymbol))
    throw std::runtime_error(InvalidOperMaNameMsg + msymbol);
  if (!ExprTeleprompter::IsValidInName(iname))
    throw std::runtime_error(InvalidOperInNameMsg + iname);
  if (nargc > ArgCountLimit)
    throw std::runtime_error(
        "创建的运算符可接受参数数量超过限制.MatchSymbol: " + msymbol +
        "; internal name: " + iname);
  MatchSymbol = ExprTeleprompter::ToUpperStringCpy(msymbol);
  InternalName = ExprTeleprompter::ToUpperStringCpy(iname);
}
//
std::string ExprOper::ToString() const {
  static const char *fmtStr{
      "Operator Match Symbol: %s\nOperator Internal Name: %s\nPosition of "
      "appearance: %s\nNeeded Arguments Count: %u\nNormal Priority: %u\nStack "
      "Priority: %u\nHelper "
      "brief:\n%s"};
  char buf[512]{"\0"};
  sprintf_s(buf, 512, fmtStr, MatchSymbol.c_str(), InternalName.c_str(),
            GetOperCategoryString(Category).c_str(), NeededArgsCount,
            (unsigned int)NormalPriority, (unsigned int)StackPriority,
            Helper(false).c_str());
  return std::string(buf);
}
/***************************
 * class ExprOperWithArgs definition *
 ***************************
 */
ExprOperWithArgs::ExprOperWithArgs(OperCPtr_Ty opCPtr, ArgPtrColl_Ty &args)
    : OperCPtr{opCPtr}, Args{args} {}
ExprOperWithArgs::ExprOperWithArgs(OperCPtr_Ty opCPtr, ArgPtrColl_Ty &&args)
    : OperCPtr{opCPtr}, Args{std::move(args)} {}
//
ExprOperWithArgs::ExprOperWithArgs(const ExprOperWithArgs &other)
    : OperCPtr{other.OperCPtr}, Args{other.Args} {}
//
std::string ExprOperWithArgs::ToString() const {
  std::string resMsg{OperCPtr->ToString()};
  resMsg = resMsg + "\nArguments Information:\nTotal Count:" +
           std::to_string(Args.size());
  for (size_t cnt = 0; cnt < Args.size(); ++cnt) {
    resMsg =
        resMsg + "\nArg[" + std::to_string(cnt) + "]: " + Args[cnt]->ToString();
  }
  return resMsg;
}
//
bool ExprOperWithArgs::IsValid(VarTablePtr_Ty varTable) const {
  return OperCPtr->VerifyArgs(Args, varTable);
}
//
[[nodiscard]] ResultPtr_Ty
ExprOperWithArgs::Calculate(VarTablePtr_Ty varTablePtr) {
  assert(GetType() == DataType::OperWithArgs);
  if (!IsValid(varTablePtr))
    throw std::runtime_error(InvalidOperOrArgMsg);
  return OperCPtr->Execute(Args, varTablePtr);
}
//
bool ExprOperWithArgs::TrySimplify(ResultPtr_Ty &res) {
  if (GetType() != DataType::OperWithArgs)
    return false;
  // 只有Args都为值类型时才化简
  for (const auto &a : Args) {
    if (a->GetType() != DataType::Value) {
      return false;
    }
  }
  if (res != nullptr)
    res.reset();
  res = OperCPtr->Execute(Args, nullptr);
  return true;
}
/***************************
 * class ExprOperMan's definition *
 ***************************
 */
/************************************
 * static member's initilize *
 ************************************
 */
std::vector<ExprOper> ExprOperMan::RegisteredOperColl;
bool ExprOperMan::ValidTag = false;
//
size_t ExprOperMan::SearchSymbol(const char *tarStr, size_t tarStrLen) {
  if (tarStr == nullptr)
    return InvalidPos;
  return SearchSymbol(std::string_view(tarStr, tarStrLen));
}
//
size_t ExprOperMan::SearchSymbol(const std::string &symbol) {
  return SearchSymbol(std::string_view(symbol));
}
//
size_t ExprOperMan::SearchSymbol(const std::string_view &symbol) {
  if (!IsInitilized())
    Init();
  std::string tmp = ExprTeleprompter::ToUpperStringCpy(symbol);
  if (!ExprTeleprompter::IsValidMatchSymbol(tmp))
    return InvalidPos;
  for (size_t cnt = 0; cnt < RegisteredOperColl.size(); ++cnt) {
    if (RegisteredOperColl[cnt].GetMatchSymbol() == tmp) {
      return cnt;
    }
  }
  return InvalidPos;
}
//
bool ExprOperMan::SymbolOperSyntaxMatch(const SymbolInfoColl_Ty &siColl,
                                        size_t tarPos, size_t &resFPos) {
  if (tarPos >= siColl.size())
    throw std::runtime_error("查询位置位于符号表之外！");
  const auto &operColl = ExprOperMan::GetColl();
  for (size_t curPos = 0; curPos < operColl.size(); ++curPos) {
    if (operColl[curPos].GetMatchSymbol() == siColl[tarPos].first) {
      if (ExprSyntaxPaser::SymbolSyntaxVarifier(operColl[curPos], siColl,
                                                tarPos)) {
        resFPos = curPos;
        return true;
      }
    }
  }
  resFPos = InvalidPos;
  return false;
}
//
bool ExprOperMan::SymbolVarSyntaxMatch(const SymbolInfoColl_Ty &siColl,
                                       size_t tarPos) {
  if (siColl[tarPos].second != DataType::Variable)
    return false;
  // 可以出现在表达式开头，如果有前缀，前缀必然是符号
  if (tarPos > 1) {
    if (siColl[tarPos - 1].second != DataType::Oper)
      return false;
  }
  // 可以出现在表达式末尾，如果有后缀，后缀必然是符号
  if (tarPos + 1 < siColl.size()) {
    if (siColl[tarPos + 1].second != DataType::Oper)
      return false;
  }
  return true;
}
//
bool ExprOperMan::SymbolNumSyntaxMatch(const SymbolInfoColl_Ty &siColl,
                                       size_t tarPos) {
  if (siColl[tarPos].second != DataType::Value)
    return false;
  // 可以出现在表达式开头，如果有前缀，前缀必然是符号
  if (tarPos > 1) {
    if (siColl[tarPos - 1].second != DataType::Oper)
      return false;
  }
  // 可以出现在表达式末尾，如果有后缀，后缀必然是符号
  if (tarPos + 1 < siColl.size()) {
    if (siColl[tarPos + 1].second != DataType::Oper)
      return false;
  }
  return true;
}

//
size_t ExprOperMan::SearchInName(const char *tarStr, size_t tarStrLen) {
  if (tarStr == nullptr)
    return InvalidPos;
  return SearchInName(std::string_view(tarStr, tarStrLen));
}
//
size_t ExprOperMan::SearchInName(const std::string &inNa) {
  return SearchInName(std::string_view(inNa));
}
//
size_t ExprOperMan::SearchInName(const std::string_view &inNa) {
  if (!IsInitilized())
    Init();
  std::string tmp = ExprTeleprompter::ToUpperStringCpy(inNa);
  if (!ExprTeleprompter::IsValidInName(tmp))
    return InvalidPos;
  //
  for (size_t cnt = 0; cnt < RegisteredOperColl.size(); ++cnt) {
    if (RegisteredOperColl[cnt].GetInternalName() == tmp) {
      return cnt;
    }
  }
  return InvalidPos;
}
//
size_t ExprOperMan::Register(ExprOper &&tarOper) {
  for (size_t cnt = 0; cnt < RegisteredOperColl.size(); ++cnt) {
    // Internal唯一，符号可重复
    if (RegisteredOperColl[cnt].GetInternalName() ==
        tarOper.GetInternalName()) {
      return RegisteredOperColl.size();
    }
  }
  //
  RegisteredOperColl.push_back(tarOper);
  return RegisteredOperColl.size();
}
//
OperCPtr_Ty ExprOperMan::GetOperById(size_t id) {
  if (!IsInitilized())
    Init();
  if (id > RegisteredOperColl.size()) {
    throw std::runtime_error(OperIdOutofRangeMsg + std::to_string(id));
  }
  return &RegisteredOperColl[id];
}
//
size_t ExprOperMan::Find1stSymbol(const char *tar, size_t tarLen, size_t posSt,
                                  size_t &posEdRes) {
  if (tar == nullptr)
    return InvalidPos;
  return Find1stSymbol(std::string_view(tar, tarLen), posSt, posEdRes);
}
//
size_t ExprOperMan::Find1stSymbol(const std::string &tar, size_t posSt,
                                  size_t &posEdRes) {
  return Find1stSymbol(std::string_view(tar), posSt, posEdRes);
}
//
size_t ExprOperMan::Find1stSymbol(const std::string_view &tar, size_t posSt,
                                  size_t &posEdRes) {
  if (!IsInitilized())
    Init();
  if (tar.length() < posSt) {
    posEdRes = InvalidPos;
    return InvalidPos;
  }
  for (size_t curPos = posSt; curPos < tar.length(); ++curPos) {
    for (size_t cnt = 0; cnt < RegisteredOperColl.size(); ++cnt) {
      const std::string tmp(RegisteredOperColl[cnt].GetMatchSymbol());
      if (tar.length() - curPos < tmp.length()) {
        posEdRes = InvalidPos;
        return InvalidPos;
      }
      const std::string_view subView(&tar[curPos], tmp.length());
      if (subView == tmp) {
        posEdRes = curPos + tmp.length();
        return curPos;
      }
    }
  }
  posEdRes = InvalidPos;
  return InvalidPos;
}
//
bool ExprOperMan::StartWithSymbol(const char *tar, size_t tarLen,
                                  size_t &posEdRes) {
  return StartWithSymbol(std::string_view(tar, tarLen), posEdRes);
}
//
bool ExprOperMan::StartWithSymbol(const std::string &tar, size_t &posEdRes) {
  return StartWithSymbol(std::string_view(tar), posEdRes);
}
//
bool ExprOperMan::StartWithSymbol(const std::string_view &tar,
                                  size_t &posEdRes) {
  if (!IsInitilized())
    Init();
  for (size_t cnt = 0; cnt < RegisteredOperColl.size(); ++cnt) {
    const std::string tmp(RegisteredOperColl[cnt].GetMatchSymbol());
    if (tar.length() < tmp.length()) {
      posEdRes = InvalidPos;
      return false;
    }
    const std::string_view tmpView(tar.data(), tmp.length());
    if (tmpView == tmp) {
      posEdRes = tmp.length();
      return true;
    }
  }
  posEdRes = InvalidPos;
  return false;
}
//
std::string ExprOperMan::GetRegisteredOperInfo() {
  if (!IsInitilized())
    Init();
  if (RegisteredOperColl.empty()) {
    return "No Expression Operator Registered...";
  }
  std::string res{"Total ["};
  res.append(std::to_string(RegisteredOperColl.size()));
  res.append("] Registed Expression Operator:\n");
  for (size_t cnt = 0; cnt < RegisteredOperColl.size(); ++cnt) {
    res.append("\n=== Operator id [");
    res.append(std::to_string(cnt));
    res.append("] ===\n");
    //
    res.append(RegisteredOperColl[cnt].ToString());
    //
    res.append("\n=== Operator id [");
    res.append(std::to_string(cnt));
    res.append("] ===");
  }
  return res;
}
//
OperCPtr_Ty ExprOperMan::GetLeftBrA() {
  if (!IsInitilized())
    Init();
  size_t fpos = SearchInName(std::string_view("LBRA_PN"));
  if (fpos == InvalidPos)
    throw std::runtime_error(
        "cannot find Left Bracket A: '(',check Preset file!");
  return &RegisteredOperColl[fpos];
}
//
OperCPtr_Ty ExprOperMan::GetRightBrA() {
  if (!IsInitilized())
    Init();
  size_t fpos = SearchInName(std::string_view("RBRA_SN"));
  if (fpos == InvalidPos)
    throw std::runtime_error(
        "cannot find Right Bracket A: ')',check Preset file!");
  return &RegisteredOperColl[fpos];
}
//
OperCPtr_Ty ExprOperMan::GetDelimiterA() {
  if (!IsInitilized())
    Init();
  size_t fpos = SearchInName(std::string_view("DELIMITER_IN"));
  if (fpos == InvalidPos)
    throw std::runtime_error("cannot find Delimiter A: ',',check Preset file!");
  return &RegisteredOperColl[fpos];
}
//
bool ExprOperMan::IsMatchedBracket(OperCPtr_Ty left, OperCPtr_Ty right) {
  if (left == nullptr || right == nullptr)
    throw std::runtime_error("Need An Operator's pointer but it's nullptr!");
  if (left->GetOpCategory() != OperCategory::LBracket ||
      right->GetOpCategory() != OperCategory::RBracket)
    return false;
  //
  for (const auto &p : Expr::ValidBracketColl) {
    if (p.first == left->GetMatchSymbol() &&
        p.second == right->GetMatchSymbol())
      return true;
  }
  return false;
}
/***************************
 * class ExprTeleprompter definition *
 ***************************
 */
ExprTeleprompter::ExprTeleprompter(const char *origin, size_t originLen)
    : ExprTeleprompter(std::string_view(origin, originLen)) {}
//
ExprTeleprompter::ExprTeleprompter(const std::string &origin)
    : ExprTeleprompter(std::string_view(origin)) {}
//
ExprTeleprompter::ExprTeleprompter(const std::string_view &origin)
    : Origin{origin}, Trimed{TrimAndFix(origin)}, CurrentPos{InvalidPos},
      SymbolInfoColl{} {
  if (!IsOriginValid(Origin))
    throw std::runtime_error(ExpressionLengthOverLimitMsg +
                             std::string(origin));
  if (!TryParseString(Trimed, SymbolInfoColl))
    throw std::runtime_error(ExpressionParseErrorMsg +
                             "Cannot Trans to Symbols");
  if (SymbolInfoColl.empty())
    throw std::runtime_error(ExpressionParseErrorMsg +
                             "Cannot Trans to Symbols");
  CurrentPos = 0;
}
//
void ExprTeleprompter::ResetCurrent() {
  CurrentPos = SymbolInfoColl.empty() ? InvalidPos : 0;
}
//
bool ExprTeleprompter::ToNextSymbol(SymbolInfo_Ty &resView) {
  if (CurrentPos < SymbolInfoColl.size()) {
    resView = SymbolInfoColl[CurrentPos++];
    return true;
  }
  CurrentPos = InvalidPos;
  return false;
}
//
bool ExprTeleprompter::ChkCurrentSymbol(SymbolInfo_Ty &resView) const {
  if (CurrentPos >= SymbolInfoColl.size())
    return false;
  resView = SymbolInfoColl[CurrentPos];
  return true;
}
//
bool ExprTeleprompter::ChkNextSymbol(SymbolInfo_Ty &resView) const {
  if (CurrentPos + 1 >= SymbolInfoColl.size())
    return false;
  resView = SymbolInfoColl[CurrentPos + 1];
  return true;
}
//
bool ExprTeleprompter::ChkPreviousSymbol(SymbolInfo_Ty &resView) const {
  if (CurrentPos < 1 || CurrentPos - 1 >= SymbolInfoColl.size())
    return false;
  resView = SymbolInfoColl[CurrentPos - 1];
  return true;
}
//
bool ExprTeleprompter::GetSymbolByRelative(int Offset,
                                           SymbolInfo_Ty &resView) const {
  size_t tarPos = InvalidPos;
  if (Offset > 0) {
    tarPos = CurrentPos + Offset;
  } else {
    size_t tmp = static_cast<size_t>(-Offset);
    tarPos = (tmp > CurrentPos) ? InvalidPos : tarPos = CurrentPos - tmp;
  }
  //
  if (tarPos >= SymbolInfoColl.size())
    return false;
  resView = SymbolInfoColl[tarPos];
  return true;
}
//
std::string ExprTeleprompter::ToString() const {
  std::string res{"Expression Teleprompter's Information:\nOrigin String:\n" +
                  Origin + "\nTrimed and fixed:\n" + Trimed +
                  "\nTotal Symbol(Number/Variable/Operator) Count: [" +
                  std::to_string(SymbolInfoColl.size()) +
                  "]\n Symbols Information:\n"};
  for (size_t cnt{0}; cnt < SymbolInfoColl.size(); ++cnt) {
    res.append("\n[Symbol " + std::to_string(cnt) +
               "] Type: " + GetDataTypeString(SymbolInfoColl[cnt].second) +
               "; Symbol: " + std::string(SymbolInfoColl[cnt].first));
  }
  return res;
}
/***************************
 * private member's definition *
 ***************************
 */
bool ExprTeleprompter::TryParseString(const std::string &trimedStr,
                                      SymbolInfoColl_Ty &resColl) {
  size_t curPos{0}, sufPos{0}; // trimedStr的绝对位置（相对位置0）
  resColl.clear();
  for (; curPos < trimedStr.length(); curPos = sufPos) {
    // 数值
    if (TryGetSufPosNum(trimedStr, curPos, sufPos)) {
      resColl.emplace_back(
          std::make_pair(std::string_view(&trimedStr[curPos], sufPos - curPos),
                         DataType::Value));
      continue;
    }
    if (TryGetSufPosOper(trimedStr, curPos, sufPos)) {
      resColl.emplace_back(
          std::make_pair(std::string_view(&trimedStr[curPos], sufPos - curPos),
                         DataType::Oper));
      continue;
    }
    // 变量,不验证变量名!
    if (TryGetSufPosVariable(trimedStr, curPos, sufPos)) {
      resColl.emplace_back(
          std::make_pair(std::string_view(&trimedStr[curPos], sufPos - curPos),
                         DataType::Variable));
    } else {
      throw std::runtime_error(
          std::string(nameof(ExprTeleprompter::TryParseString)) +
          " Parse Error ; Current Position: " + std::to_string(curPos) +
          "Suffix Position: " + std::to_string(sufPos));
    }
  }
  return !resColl.empty();
}
//
std::string ExprTeleprompter::TrimAndFix(const std::string_view &tar) {
  char buf[MaxExpressionLength]{"\0"};
  size_t tarPos = 0, resPos = 0;
  for (; tarPos < tar.length(); ++tarPos) {
    char ch = std::toupper(tar[tarPos]);
    if (!IsIgnoredCh(ch))
      buf[resPos++] = ch;
  }
  buf[resPos] = '\0';
  return std::string(buf);
}
//
bool ExprTeleprompter::IsOriginValid(const std::string_view &tar) {
  if (tar.length() < MinExpressionLength || tar.length() > MaxExpressionLength)
    return false;
  return true;
}
//
bool ExprTeleprompter::TryGetSufPosNum(const std::string &tarStr, size_t curPos,
                                       size_t &sufPos) {
  if (!IsDigit(tarStr[curPos], true)) {
    sufPos = InvalidPos;
    return false;
  }
  bool dotTag{tarStr[curPos] == '.'};
  size_t tmpSuf = curPos + 1;
  for (; tmpSuf < tarStr.length(); ++tmpSuf) {
    if (!IsDigit(tarStr[tmpSuf], !dotTag))
      break;
  }
  sufPos = tmpSuf;
  return true;
}
//
bool ExprTeleprompter::TryGetSufPosOper(const std::string &tarStr,
                                        size_t curPos, size_t &sufPos) {
  // 分隔符
  if (IsDelimiter(tarStr[curPos])) {
    sufPos = curPos + 1;
    return true;
  }
  // 左括号
  size_t fPos{
      StartWithBracket(&tarStr[curPos], tarStr.length() - curPos, true)};
  if (fPos != InvalidPos) {
    sufPos = curPos + ValidBracketColl[fPos].first.length();
    return true;
  }
  // 右括号
  fPos = StartWithBracket(&tarStr[curPos], tarStr.length() - curPos, false);
  if (fPos != InvalidPos) {
    sufPos = curPos + ValidBracketColl[fPos].second.length();
    return true;
  }
  // 其他
  if (ExprOperMan::StartWithSymbol(&tarStr[curPos], tarStr.length() - curPos,
                                   fPos)) {
    sufPos = curPos + fPos;
    return true;
  }
  return false;
}
//
bool ExprTeleprompter::TryGetSufPosVariable(const std::string &tarStr,
                                            size_t curPos, size_t &sufPos) {
  size_t tmpSufPos = curPos + 1;
  if (IsDelimiter(tarStr[tmpSufPos])) {
    sufPos = tmpSufPos;
    return true;
  }
  size_t fPos1{InvalidPos}, fPos2{InvalidPos}, fPos3{InvalidPos},
      fPos4{InvalidPos}, ignored;
  fPos1 =
      Find1stNumber(&tarStr[tmpSufPos], tarStr.length() - tmpSufPos, ignored);
  fPos2 = Find1stBracket(&tarStr[tmpSufPos], tarStr.length() - tmpSufPos, true,
                         ignored);
  fPos3 = Find1stBracket(&tarStr[tmpSufPos], tarStr.length() - tmpSufPos, false,
                         ignored);
  fPos4 = ExprOperMan::Find1stSymbol(&tarStr[tmpSufPos],
                                     tarStr.length() - tmpSufPos, 0, ignored);
  size_t fPos = std::min({fPos1, fPos2, fPos3, fPos4});
  if (fPos == InvalidPos) {
    sufPos = tmpSufPos;
    return false;
  }
  sufPos = tmpSufPos + fPos;
  return true;
}
//
/* public func definition
 */

size_t ExprTeleprompter::BracketCategory(const char *tarStr, size_t tarStrLen,
                                         bool LOrRTag) {
  if (tarStr == nullptr)
    return InvalidPos;
  //
  return BracketCategory(std::string_view(tarStr, tarStrLen), LOrRTag);
}
//
size_t ExprTeleprompter::BracketCategory(const std::string &tarStr,
                                         bool LOrRTag) {
  return BracketCategory(std::string_view(tarStr), LOrRTag);
}
//
size_t ExprTeleprompter::BracketCategory(const std::string_view &tarStr,
                                         bool LOrRTag) {
  for (size_t cnt = 0; cnt < ValidBracketColl.size(); ++cnt) {
    const std::string_view tmpView(
        (LOrRTag ? ValidBracketColl[cnt].first : ValidBracketColl[cnt].second));
    if (tarStr == tmpView)
      return cnt;
  }
  return InvalidPos;
}
//
size_t ExprTeleprompter::IsLorRBracket(const char *tarStr, size_t tarStrLen) {
  if (tarStr == nullptr)
    return InvalidPos;
  //
  return IsLorRBracket(std::string_view(tarStr, tarStrLen));
}
//
size_t ExprTeleprompter::IsLorRBracket(const std::string &tarStr) {
  return IsLorRBracket(std::string_view(tarStr));
}
//
size_t ExprTeleprompter::IsLorRBracket(const std::string_view &tarStr) {
  for (size_t cnt = 0; cnt < ValidBracketColl.size(); ++cnt) {
    const std::string_view tmpViewL(ValidBracketColl[cnt].first);
    const std::string_view tmpViewR(ValidBracketColl[cnt].second);
    if (tarStr == tmpViewL || tarStr == tmpViewR)
      return cnt;
  }
  return InvalidPos;
}
//
size_t ExprTeleprompter::StartWithBracket(const char *tarStr, size_t tarStrLen,
                                          bool LOrRTag) {
  if (tarStr == nullptr)
    return InvalidPos;
  return StartWithBracket(std::string_view(tarStr, tarStrLen), LOrRTag);
}
//
size_t ExprTeleprompter::StartWithBracket(const std::string &tarStr,
                                          bool LOrRTag) {
  return StartWithBracket(std::string_view(tarStr), LOrRTag);
}
//
size_t ExprTeleprompter::StartWithBracket(const std::string_view &tarStr,
                                          bool LOrRTag) {
  for (size_t cnt = 0; cnt < ValidBracketColl.size(); ++cnt) {
    const std::string_view tmpView(
        (LOrRTag ? ValidBracketColl[cnt].first : ValidBracketColl[cnt].second));
    if (tarStr.length() < tmpView.length())
      return InvalidPos;
    const std::string_view tarView(tarStr.data(), tmpView.length());
    if (tarView == tmpView)
      return cnt;
  }
  return InvalidPos;
}
//
size_t ExprTeleprompter::StartWithLOrRBracket(const char *tarStr,
                                              size_t tarStrLen) {
  if (tarStr == nullptr)
    return InvalidPos;
  return StartWithLOrRBracket(std::string_view(tarStr, tarStrLen));
}
//
size_t ExprTeleprompter::StartWithLOrRBracket(const std::string &tarStr) {
  return StartWithLOrRBracket(std::string_view(tarStr));
}
//
size_t ExprTeleprompter::StartWithLOrRBracket(const std::string_view &tarStr) {
  for (size_t cnt = 0; cnt < ValidBracketColl.size(); ++cnt) {
    const std::string_view tmpViewL(ValidBracketColl[cnt].first);
    const std::string_view tmpViewR(ValidBracketColl[cnt].second);
    if (tarStr.length() >= tmpViewL.length()) {
      const std::string_view tarViewL(tarStr.data(), tmpViewL.length());
      if (tarViewL == tmpViewL)
        return cnt;
    }
    if (tarStr.length() >= tmpViewR.length()) {
      const std::string_view tarViewR(tarStr.data(), tmpViewR.length());
      if (tarViewR == tmpViewR)
        return cnt;
    }
  }
  return InvalidPos;
}
//
size_t ExprTeleprompter::Find1stBracket(const char *tarStr, size_t tarStrLen,
                                        bool LOrRTag, size_t &resBrId) {
  if (tarStr == nullptr) {
    resBrId = InvalidPos;
    return InvalidPos;
  }
  //
  return Find1stBracket(std::string_view(tarStr, tarStrLen), LOrRTag, resBrId);
}
//
size_t ExprTeleprompter::Find1stBracket(const std::string &tarStr, bool LOrRTag,
                                        size_t &resBrId) {
  return Find1stBracket(std::string_view(tarStr), LOrRTag, resBrId);
}
//
size_t ExprTeleprompter::Find1stBracket(const std::string_view &tarStr,
                                        bool LOrRTag, size_t &resBrId) {
  for (size_t cnt = 0; cnt < ValidBracketColl.size(); ++cnt) {
    const std::string_view view(LOrRTag ? ValidBracketColl[cnt].first
                                        : ValidBracketColl[cnt].second);
    for (size_t fpos = 0; fpos < tarStr.length(); ++fpos) {
      if (tarStr.length() - fpos >= view.length()) {
        const std::string_view tarView{&tarStr[fpos], view.length()};
        if (tarView == view) {

          resBrId = cnt;
          return fpos;
        }
      }
    }
  }

  resBrId = InvalidPos;
  return InvalidPos;
}
//
size_t ExprTeleprompter::Find1stNumber(const char *tarStr, size_t tarStrLen,
                                       size_t &numPosEd) {
  if (tarStr == nullptr) {
    numPosEd = InvalidPos;
    return InvalidPos;
  }
  //
  return Find1stNumber(std::string_view(tarStr, tarStrLen), numPosEd);
}
//
size_t ExprTeleprompter::Find1stNumber(const std::string &tarStr,
                                       size_t &numPosEd) {
  return Find1stNumber(std::string_view(tarStr), numPosEd);
}
//
size_t ExprTeleprompter::Find1stNumber(const std::string_view &tarStr,
                                       size_t &numPosEd) {
  size_t fpos = 0;
  bool dotTag = true;
  for (; fpos < tarStr.length(); ++fpos) {
    if (IsDigit(tarStr[fpos], dotTag))
      break;
    dotTag = tarStr[fpos] == '.';
  }
  //
  if (fpos == tarStr.length()) {
    numPosEd = InvalidPos;
  } else {
    for (numPosEd = fpos + 1; numPosEd < tarStr.length(); ++numPosEd) {
      if (!IsDigit(tarStr[numPosEd], dotTag))
        break;
    }
  }
  return fpos;
}

// 是否为忽略的字符
bool ExprTeleprompter::IsIgnoredCh(char tarCh) {
  size_t tmp;
  return IsChInColl(tarCh, IgnoredDelimiter, tmp);
}
// 是否为分隔符
bool ExprTeleprompter::IsDelimiter(char tarCh) {
  size_t tmp;
  return IsChInColl(tarCh, ValidDelimiter, tmp);
}
//
bool ExprTeleprompter::IsValidArgCount(unsigned int count) {
  return count <= ArgCountLimit;
}
//
bool ExprTeleprompter::IsChInColl(char tarCh, const std::string &tarStr,
                                  size_t &fpos) {
  size_t foundPos = InvalidPos;
  for (size_t cnt = 0; cnt < tarStr.length(); ++cnt) {
    if (tarCh == tarStr[cnt]) {
      foundPos = cnt;
      break;
    }
  }
  //
  fpos = foundPos;
  return foundPos != InvalidPos;
}
//
bool ExprTeleprompter::IsDigit(char ch, bool dotTag) {
  if (ch >= '0' && ch <= '9') {
    return true;
  }
  return dotTag ? ch == '.' : false;
}
//
bool ExprTeleprompter::IsValidMatchSymbol(const char *maNa, size_t maNaLen) {
  if (maNa == nullptr)
    return false;
  // 长度
  if (maNaLen > MaxOperSymbolLength || maNaLen < MinOperSymbolLength)
    return false;
  // 不含数字/分隔符
  size_t ignored;
  for (size_t cnt = 0; cnt < maNaLen; ++cnt) {
    if (IsDigit(maNa[cnt], true))
      return false;
    if (IsChInColl(maNa[cnt], IgnoredDelimiter, ignored))
      return false;
    /*
    if (IsChInColl(maNa[cnt], ValidDelimiter, ignored))
      return false;
      */
  }
  /*
  // 不含左/右括号
  for (size_t cnt = 0; cnt < ValidBracketColl.size(); ++cnt) {
    const auto &p = ValidBracketColl[cnt];
    if (strstr(maNa, p.first.c_str()) != nullptr ||
        strstr(maNa, p.second.c_str()) != nullptr)
      return false;
  }
  */
  return true;
}
//
bool ExprTeleprompter::IsValidMatchSymbol(const std::string &maNa) {
  return IsValidMatchSymbol(maNa.c_str(), maNa.length());
}
//
bool ExprTeleprompter::IsValidMatchSymbol(const std::string_view &maNa) {
  return IsValidMatchSymbol(maNa.data(), maNa.length());
}
//
bool ExprTeleprompter::IsValidInName(const char *inNa, size_t inNaLen) {
  size_t ignored;
  if (inNa == nullptr)
    return false;
  // 长度
  if (inNaLen > MaxOperInNameLength || inNaLen < MinOperInNameLength)
    return false;
  if (!IsValidMatchSymbol(inNa, inNaLen - 3))
    return false;
  // *_[P|I|S|B][N|S|D|M]
  // [P(Prefix)|I(Infix)|S(Suffix)|B(PrefixOrSuffix)]
  // [N(None)S(Single)D(Double)M(Multiple)]
  if (inNa[inNaLen - 3] != '_')
    return false;
  if (!IsChInColl(inNa[inNaLen - 2], "PpIiSsBb", ignored))
    return false;
  return IsChInColl(inNa[inNaLen - 1], "NnSsDdMm", ignored);
}
//
bool ExprTeleprompter::IsValidInName(const std::string &inNa) {
  return IsValidInName(inNa.c_str(), inNa.length());
}
//
bool ExprTeleprompter::IsValidInName(const std::string_view &inNa) {
  return IsValidInName(inNa.data(), inNa.length());
}
//
// 长度&非法字符检测
bool ExprTeleprompter::IsValidVarName(const char *varNa, size_t varNaLen) {
  if (varNa == nullptr)
    return false;
  if (varNaLen > MaxVarNameLength || varNaLen < MinVarNameLength)
    return false;
  for (size_t cnt = 0; cnt < varNaLen; ++cnt) {
    size_t igTmp;
    if (IsChInColl(varNa[cnt], IllegalVarNameChColl, igTmp))
      return false;
  }
  return true;
}
//
bool ExprTeleprompter::IsValidVarName(const std::string &varNa) {
  return IsValidVarName(varNa.c_str(), varNa.length());
}
//
bool ExprTeleprompter::IsValidVarName(const std::string_view &varNa) {
  return IsValidVarName(varNa.data(), varNa.length());
}
//
void ExprTeleprompter::ToUpperStringOnSite(char *tar, size_t tarLen) {
  for (size_t cnt = 0; cnt < tarLen && tar[cnt] != '\0'; ++cnt) {
    tar[cnt] = std::toupper(tar[cnt]);
  }
}
//
void ExprTeleprompter::ToUpperStringOnSite(std::string &tar) {
  for (size_t cnt = 0; cnt < tar.size(); ++cnt)
    tar[cnt] = std::toupper(tar[cnt]);
}
//
std::string ExprTeleprompter::ToUpperStringCpy(const char *tarStr,
                                               size_t tarStrLen) {
  std::string res(tarStr, tarStrLen);
  ToUpperStringOnSite(res);
  return res;
}
//
std::string ExprTeleprompter::ToUpperStringCpy(const std::string &tar) {
  std::string res(tar);
  ToUpperStringOnSite(res);
  return res;
}
//
std::string ExprTeleprompter::ToUpperStringCpy(const std::string_view &tar) {
  std::string res(tar);
  ToUpperStringOnSite(res);
  return res;
}
//
bool ExprTeleprompter::IsValidCString(const char *tarStr, size_t tarStrLen) {
  if (tarStr == nullptr)
    return false;
  size_t cnt = 0;
  for (; cnt < tarStrLen && tarStr[cnt] != '\0'; ++cnt)
    ;
  return tarStrLen >= cnt;
}
/***************************
 * class ExprSyntaxPaser's definition *
 ***************************
 */
/* static member's definition
 */
std::stack<ArgPtr_Ty> ExprSyntaxPaser::ArgStk;           // 值栈
std::stack<OperCPtr_Ty> ExprSyntaxPaser::OpStk;          // 符号栈
std::vector<std::string> ExprSyntaxPaser::VariableTable; // 参数表
// 多参时的计数器,执行前/后缀后清零
size_t ExprSyntaxPaser::CurrentArgCounter = 0;
/* public member's definition
 */
ResultPtr_Ty ExprSyntaxPaser::TrySyntaxPaser(const char *tarStr,
                                             size_t tarStrLen) {
  if (tarStr == nullptr)
    throw std::runtime_error("参数字符串不能为nullptr！");
  return TrySyntaxPaser(std::string_view(tarStr, tarStrLen));
}
ResultPtr_Ty ExprSyntaxPaser::TrySyntaxPaser(const std::string &tarStr) {
  ExprTeleprompter et(tarStr);
  return TrySyntaxPaser(et);
}
ResultPtr_Ty ExprSyntaxPaser::TrySyntaxPaser(const std::string_view &tarStr) {
  ExprTeleprompter et(tarStr);
  return TrySyntaxPaser(et);
}
// !!!
ResultPtr_Ty ExprSyntaxPaser::TrySyntaxPaser(ExprTeleprompter &et) {
  Reset();
  OpStk.push(ExprOperMan::GetLeftBrA());
  //
  SymbolInfo_Ty tmpSymbolInfo;
  for (; et.ToNextSymbol(tmpSymbolInfo);) {
    // 数值
    if (tmpSymbolInfo.second == DataType::Value) {
      //
      if (!ExprOperMan::SymbolNumSyntaxMatch(et.GetInternalColl(),
                                             et.GetCurrentPos() - 1)) {
        throw std::runtime_error(
            std::string("A Syntax Error ; Symbol: ") +
            std::string(tmpSymbolInfo.first) +
            " ; Symbol pos: " + std::to_string(et.GetCurrentPos() - 1) +
            " ; Symbol Type: " + GetDataTypeString(tmpSymbolInfo.second));
      }
      //
      Number_Ty num = std::atof(tmpSymbolInfo.first.data());
      auto numPtr = std::make_shared<ExprNum>(num);
      ArgStk.push(std::static_pointer_cast<ExprDataBase>(numPtr));
      continue;
    }
    // 变量
    if (tmpSymbolInfo.second == DataType::Variable) {
      //
      if (!ExprOperMan::SymbolVarSyntaxMatch(et.GetInternalColl(),
                                             et.GetCurrentPos() - 1)) {
        throw std::runtime_error(
            std::string("A Syntax Error ; Symbol: ") +
            std::string(tmpSymbolInfo.first) +
            " ; Symbol pos: " + std::to_string(et.GetCurrentPos() - 1) +
            " ; Symbol Type: " + GetDataTypeString(tmpSymbolInfo.second));
      }
      //
      auto fIter = std::find(VariableTable.begin(), VariableTable.end(),
                             std::string(tmpSymbolInfo.first));
      if (fIter != VariableTable.end())
        continue;
      VariableTable.emplace_back(std::string(tmpSymbolInfo.first));
      auto varPtr = std::make_shared<ExprVar>(tmpSymbolInfo.first);
      ArgStk.push(std::static_pointer_cast<ExprDataBase>(varPtr));
      continue;
    }
    // 符号
    if (tmpSymbolInfo.second == DataType::Oper) {
      size_t regId = InvalidPos;
      if (!ExprOperMan::SymbolOperSyntaxMatch(et.GetInternalColl(),
                                              et.GetCurrentPos() - 1, regId)) {
        throw std::runtime_error(
            std::string("A Syntax Error ; Symbol: ") +
            std::string(tmpSymbolInfo.first) +
            " ; Symbol pos: " + std::to_string(et.GetCurrentPos() - 1) +
            " ; Symbol Type: " + GetDataTypeString(tmpSymbolInfo.second));
      }
      //
      OperCPtr_Ty opPtr = ExprOperMan::GetOperById(regId);
      // 右括号
      if (opPtr->GetOpCategory() == OperCategory::RBracket) {
        if (OpStk.empty())
          throw std::runtime_error("符号栈意外为空！");
        HandleToLBR();
        // 检查括号匹配!
        OperCPtr_Ty lbrPtr = OpStk.top();
        if (!ExprOperMan::IsMatchedBracket(lbrPtr, opPtr))
          throw std::runtime_error("不匹配的左右括号！");
        OpStk.pop();
        continue;
      }
      // 其他符号
      if (OpStk.empty())
        throw std::runtime_error("符号栈意外为空！");
      for (; OpStk.top()->GetStackPriority() > opPtr->GetNormalPriority() &&
             !OpStk.empty();) {
        HandleTopOper();
      }
      OpStk.push(opPtr);
      continue;
    }
    // 意外的情况
    throw std::runtime_error(
        std::string("意外的符号: ") + std::string(tmpSymbolInfo.first) +
        " ; 类型: " + GetDataTypeString(tmpSymbolInfo.second));
  }
  // 后续处理
  HandleToLBR();
  OpStk.pop();
  if (ArgStk.size() != 1 || !OpStk.empty())
    throw std::runtime_error("解析失败！");
  return ArgStk.top();
}
//
void ExprSyntaxPaser::Reset() {
  for (; !ArgStk.empty();)
    ArgStk.pop();
  //
  for (; !OpStk.empty();)
    OpStk.pop();
  VariableTable.clear();
  CurrentArgCounter = 0;
}
//
bool ExprSyntaxPaser::SymbolSyntaxVarifier(const ExprOper &opRef,
                                           const SymbolInfoColl_Ty &siColl,
                                           size_t tarPos) {
  if (opRef.GetOpCategory() == OperCategory::LBracket) {
    return LeftBracketSyntax(siColl, tarPos);
  }
  if (opRef.GetOpCategory() == OperCategory::RBracket) {
    return RightBracketSyntax(siColl, tarPos);
  }
  if (opRef.GetOpCategory() == OperCategory::Delimiter) {
    return DelimiterSyntax(siColl, tarPos);
  }
  if (opRef.GetOpCategory() == OperCategory::Prefix) {
    if (opRef.GetNeededArgsCount() > 1) {
      return PrefixNeedArgs(siColl, tarPos);
    } else {
      return PrefixSingle(siColl, tarPos);
    }
  }
  if (opRef.GetOpCategory() == OperCategory::Infix) {
    return InfixDoubleArgs(siColl, tarPos);
  }
  if (opRef.GetOpCategory() == OperCategory::Suffix) {
    size_t nac = opRef.GetNeededArgsCount();
    switch (nac) {
    case 0:
      return SuffixNone(siColl, tarPos);
    case 1:
      return SuffixSingle(siColl, tarPos);
    default:
      return SuffixNeedArgs(siColl, tarPos);
    }
  }
  if (opRef.GetOpCategory() == OperCategory::PrefOrSuff) {
    size_t nac = opRef.GetNeededArgsCount();
    switch (nac) {
    case 0:
      return SuffixNone(siColl, tarPos);
    case 1:
      return PrefixNeedArgs(siColl, tarPos) || SuffixSingle(siColl, tarPos);
    default:
      return PrefixNeedArgs(siColl, tarPos) || SuffixNeedArgs(siColl, tarPos);
    }
  }
  return false;
}
/* private member's definition
 */
bool ExprSyntaxPaser::PrefixSingle(const SymbolInfoColl_Ty &siColl,
                                   size_t tarPos) {
  if (siColl[tarPos].second != DataType::Oper)
    return false;
  // 不会在结尾出现
  if (tarPos + 1 >= siColl.size())
    return false;
  if (siColl[tarPos + 1].second == DataType::Value ||
      siColl[tarPos + 1].second == DataType::Variable ||
      ExprTeleprompter::BracketCategory(siColl[tarPos + 1].first, true) !=
          InvalidPos) {
    return tarPos < 1 ? true
                      : ExprTeleprompter::BracketCategory(
                            siColl[tarPos - 1].first, true) != InvalidPos;
  }
  return false;
}
//
bool ExprSyntaxPaser::PrefixNeedArgs(const SymbolInfoColl_Ty &siColl,
                                     size_t tarPos) {
  if (siColl[tarPos].second != DataType::Oper)
    return false;
  // 不会在结尾出现
  if (tarPos + 1 >= siColl.size())
    return false;
  // oper之后必须为括号，前一个必须为oper但可以在表达式开头
  if (ExprTeleprompter::BracketCategory(siColl[tarPos + 1].first, true) !=
      InvalidPos)
    return tarPos < 1 ? true : siColl[tarPos - 1].second == DataType::Oper;
  return false;
}
//
bool ExprSyntaxPaser::InfixDoubleArgs(const SymbolInfoColl_Ty &siColl,
                                      size_t tarPos) {
  if (siColl[tarPos].second != DataType::Oper)
    return false;
  // 不会在表达式开头、结尾出现
  if (tarPos < 1)
    return false;
  if (siColl.size() <= tarPos + 1)
    return false;
  // 前一个不能是左括号或分隔符
  if (siColl[tarPos - 1].second == DataType::Oper) {
    if (ExprTeleprompter::BracketCategory(siColl[tarPos - 1].first, true) !=
        InvalidPos)
      return false;
    if (ExprTeleprompter::IsDelimiter(siColl[tarPos - 1].first[0]))
      return false;
  }
  // 后一个不能为右括号或分隔符
  if (siColl[tarPos + 1].second == DataType::Oper) {
    if (ExprTeleprompter::BracketCategory(siColl[tarPos + 1].first, false) !=
        InvalidPos)
      return false;
    if (ExprTeleprompter::IsDelimiter(siColl[tarPos + 1].first[0]))
      return false;
  }
  return true;
}
//
bool ExprSyntaxPaser::SuffixNone(const SymbolInfoColl_Ty &siColl,
                                 size_t tarPos) {
  if (siColl[tarPos].second != DataType::Oper)
    return false;
  // 可以在表达式开头或结尾
  if (tarPos == 0 || tarPos + 1 == siColl.size())
    return true;
  //
  if (tarPos > 1) {
    // 前一个不能是值、变量、右括号
    if (siColl[tarPos - 1].second == DataType::Value ||
        siColl[tarPos - 1].second == DataType::Variable ||
        ExprTeleprompter::BracketCategory(siColl[tarPos - 1].first, false) !=
            InvalidPos)
      return false;
  }
  //
  if (tarPos + 1 < siColl.size()) {
    // 后一个不能是值、变量、左括号
    if (siColl[tarPos + 1].second == DataType::Value ||
        siColl[tarPos + 1].second == DataType::Variable ||
        ExprTeleprompter::BracketCategory(siColl[tarPos - 1].first, true) !=
            InvalidPos)
      return false;
  }
  return true;
}
//
bool ExprSyntaxPaser::SuffixSingle(const SymbolInfoColl_Ty &siColl,
                                   size_t tarPos) {
  if (siColl[tarPos].second != DataType::Oper)
    return false;
  // 不会在表达式开头
  if (tarPos < 1)
    return false;
  // 前一个不能是分隔符
  // 允许 num! numfact 的形式 虽然第2种比较怪
  if (siColl[tarPos - 1].second == DataType::Oper) {
    if (ExprTeleprompter::IsDelimiter(siColl[tarPos - 1].first[0]))
      return false;
  }
  // 可以在结尾，如果右后续，后一个必然是符号但不能是分隔符
  if (tarPos + 1 < siColl.size()) {
    if (siColl[tarPos + 1].second != DataType::Oper &&
        ExprTeleprompter::IsDelimiter(siColl[tarPos + 1].first[0]))
      return false;
  }
  return true;
}
//
bool ExprSyntaxPaser::SuffixNeedArgs(const SymbolInfoColl_Ty &siColl,
                                     size_t tarPos) {
  if (siColl[tarPos].second != DataType::Oper)
    return false;
  // (num)op 出现位置至少是3
  if (tarPos < 4)
    return false;
  // 前一个符号必然是右括号
  if (ExprTeleprompter::BracketCategory(siColl[tarPos - 1].first, false) ==
      InvalidPos)
    return false;
  // 可以在结尾，如果有后续,后续必然是符号
  if (tarPos + 1 < siColl.size()) {
    if (siColl[tarPos + 1].second != DataType::Oper)
      return false;
  }
  return true;
}
//
bool ExprSyntaxPaser::LeftBracketSyntax(const SymbolInfoColl_Ty &siColl,
                                        size_t tarPos) {
  if (siColl[tarPos].second != DataType::Oper)
    return false;
  if (ExprTeleprompter::BracketCategory(siColl[tarPos].first, true) ==
      InvalidPos)
    return false;
  // 可以在开头，不能在结尾
  if (tarPos == 0)
    return true;
  if (tarPos + 1 >= siColl.size())
    return false;
  // 前一个必然是符号 必须显式写出 *
  if (siColl[tarPos - 1].second != DataType::Oper)
    return false;
  return true;
}
//
bool ExprSyntaxPaser::RightBracketSyntax(const SymbolInfoColl_Ty &siColl,
                                         size_t tarPos) {
  if (siColl[tarPos].second != DataType::Oper)
    return false;
  if (ExprTeleprompter::BracketCategory(siColl[tarPos].first, false) ==
      InvalidPos)
    return false;
  // 允许()...开头 ,出现位置至少是1
  if (tarPos < 2)
    return false;
  // 前一个符号不能是分隔符
  if (siColl[tarPos - 1].second == DataType::Oper) {
    if (ExprTeleprompter::IsDelimiter(siColl[tarPos - 1].first[0]))
      return false;
  }
  // 如果有后续，后续不能是值、变量、左括号
  if (tarPos + 1 < siColl.size()) {
    if (siColl[tarPos + 1].second == DataType::Value ||
        siColl[tarPos + 1].second == DataType::Variable ||
        ExprTeleprompter::BracketCategory(siColl[tarPos + 1].first, true) !=
            InvalidPos)
      return false;
  }
  return true;
}
//
bool ExprSyntaxPaser::DelimiterSyntax(const SymbolInfoColl_Ty &siColl,
                                      size_t tarPos) {
  // 只能出现在括号内 这里做了简化，仅判断前后
  if (siColl[tarPos].second != DataType::Oper)
    return false;
  //(num, 出现位置至少是2
  if (tarPos < 3)
    return false;
  // 前一个不能是左括号或分隔符
  if (ExprTeleprompter::BracketCategory(siColl[tarPos - 1].first, true) !=
          InvalidPos ||
      ExprTeleprompter::IsDelimiter(siColl[tarPos - 1].first[0]))
    return false;
  // 不能作为结尾,后一个不能是右括号或分隔符
  if (tarPos + 1 >= siColl.size())
    return false;
  if (ExprTeleprompter::BracketCategory(siColl[tarPos + 1].first, false) !=
          InvalidPos ||
      ExprTeleprompter::IsDelimiter(siColl[tarPos - 1].first[0]))
    return false;
  return true;
}
//
void ExprSyntaxPaser::HandleToLBR() {
  for (;;) {
    OperCPtr_Ty topOper = OpStk.top();
    // 左括号中止，不弹出
    if (topOper->GetOpCategory() == OperCategory::LBracket)
      break;
    // 分隔符
    if (topOper->GetOpCategory() == OperCategory::Delimiter) {
      CurrentArgCounter++;
      OpStk.pop();
      continue;
    }
    //
    size_t nac = topOper->GetNeededArgsCount();
    ArgPtrColl_Ty tmpArgPtrColl;
    if (nac > 2) {
      nac = CurrentArgCounter + 1; // 多参时获取真正的参数数量
      CurrentArgCounter = 0;       // 重置
    }
    //
    for (size_t cnt = 0; cnt < nac; ++cnt) {
      tmpArgPtrColl.push_back(ArgStk.top());
      ArgStk.pop();
    }
    std::reverse(tmpArgPtrColl.begin(), tmpArgPtrColl.end()); // 保证参数次序
    std::shared_ptr<ExprOperWithArgs> eow =
        std::make_shared<ExprOperWithArgs>(topOper, tmpArgPtrColl);
    ResultPtr_Ty resPtr =
        std::static_pointer_cast<ExprDataBase>(std::make_shared<ExprNum>(NAN));
    if (eow->TrySimplify(resPtr))
      ArgStk.push(resPtr);
    else
      ArgStk.push(std::static_pointer_cast<ExprDataBase>(eow));
    //
    OpStk.pop();
  }
}
/* HandleTopOper
 * 执行栈顶操作符并弹出!
 */
void ExprSyntaxPaser::HandleTopOper() {
  OperCPtr_Ty topOper = OpStk.top();
  OpStk.pop();
  //
  size_t nac = topOper->GetNeededArgsCount();
  ArgPtrColl_Ty tmpArgPtrColl;
  if (nac > 2) {
    nac = CurrentArgCounter + 1;
    CurrentArgCounter = 0;
  }
  for (size_t cnt = 0; cnt < nac; ++cnt) {
    if (ArgStk.empty())
      throw std::runtime_error("参数栈意外为空！");
    tmpArgPtrColl.push_back(ArgStk.top());
    ArgStk.pop();
  }
  std::reverse(tmpArgPtrColl.begin(), tmpArgPtrColl.end()); // 保证参数次序
  std::shared_ptr<ExprOperWithArgs> eow =
      std::make_shared<ExprOperWithArgs>(topOper, tmpArgPtrColl);
  ResultPtr_Ty resPtr =
      std::static_pointer_cast<ExprDataBase>(std::make_shared<ExprNum>(NAN));
  if (eow->TrySimplify(resPtr))
    ArgStk.push(resPtr);
  else
    ArgStk.push(std::static_pointer_cast<ExprDataBase>(eow));
}
/************************************
 * class ExprAbstractSyntaxTree's definition *
 ************************************
 */
ExprAbstractSyntaxTree::ExprAbstractSyntaxTree(const char *tarStr,
                                               size_t tarStrLen)
    : ExprAbstractSyntaxTree(std::string(tarStr, tarStrLen)) {}
//
ExprAbstractSyntaxTree::ExprAbstractSyntaxTree(const std::string &tarStr)
    : OriginData(tarStr), ASTRoot(nullptr), VariableColl() {
  ASTRoot = ExprSyntaxPaser::TrySyntaxPaser(OriginData);
  //
  size_t varCount = ExprSyntaxPaser::VariableTable.size();
  for (size_t cnt = 0; cnt < varCount; ++cnt) {
    VariableColl.insert(std::make_pair(ExprSyntaxPaser::VariableTable[cnt],
                                       std::queue<Number_Ty>()));
  }
}
//
ExprAbstractSyntaxTree::ExprAbstractSyntaxTree(const std::string_view &tarStr)
    : ExprAbstractSyntaxTree(std::string(tarStr.data(), tarStr.length())) {}
//
std::string ExprAbstractSyntaxTree::ToString() const {
  std::string res{"Trimed Expression :\n" + OriginData.GetTrimed() + "\n"};
  res.append(ToTreeViewString());
  res.append("\nTotal [" + std::to_string(VariableColl.size()) +
             "] Variable Needed.");
  res.append("\nVariable's Information:");
  for (const auto &cIter : VariableColl) {
    res.append("\n[Variable Name] : " + cIter.first);
    const auto &tmpQue = cIter.second;
    if (tmpQue.size() == 0)
      res.append("\nNo value in Variable Table.");
    else {
      res.append("\n[s] ");
      auto vals = tmpQue._Get_container();
      for (size_t cnt = 0; cnt < 24 && cnt < vals.size(); ++cnt) {
        if (cnt % 12 == 11)
          res.append("\n");
        res.append(std::to_string(vals[cnt]) + ";");
      }
      if (vals.size() >= 24)
        res.append("\n...[e]");
      res.append("\n[e]");
    }
    //
  }
  //
  return res;
}
//


//
std::string ExprAbstractSyntaxTree::ToTreeViewString() const {

}
//
} // namespace Expr