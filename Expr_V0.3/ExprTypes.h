#pragma once
#include "ExprPreset.h"

namespace Expr {
/***************************
 * Forward declaration *
 ***************************
 */
class ExprDataBase;
class ExprNum;
class ExprVar;
class ExprOper;
class ExprOperWithArgs;
class ExprTeleprompter;
class ExprSynaxPaser;
//
using ArgPtr_Ty = std::shared_ptr<ExprDataBase>;
using ResultPtr_Ty = ArgPtr_Ty;
using ArgPtrColl_Ty = std::vector<ArgPtr_Ty>;
using OperCPtr_Ty = const ExprOper *;
//
using ArgValidation_Ty =
    std::function<bool(const ArgPtrColl_Ty &, VarTablePtr_Ty, size_t)>;
using OperExecution_Ty =
    std::function<ResultPtr_Ty(const ArgPtrColl_Ty &, VarTablePtr_Ty)>;
using OperHelper_Ty = std::function<std::string(bool)>;
//
using SymbolInfo_Ty = std::pair<const std::string_view, DataType>;
using SymbolInfoColl_Ty = std::vector<SymbolInfo_Ty>;
/* enum class DataType
 * 数据类型
 */
enum class DataType : unsigned char {
  Oper = 0,
  OperWithArgs = 1,
  Value = 2,
  Variable = 3,
};
/* GetDataTypeString
 */
std::string GetDataTypeString(DataType dt);
/* enum class OperPriority
 * 运算符优先级
 * 分为表达式内(读取位置)优先级
 * 栈内优先级
 * 栈顶>表达式内 栈顶求值
 * 栈顶<=表达式内 入栈
 */
enum class OperPriority : unsigned char {
  Lowest = 0,
  L1 = 1,
  L2 = 2,
  L3 = 3,
  L4 = 4,
  L5 = 5,
  L6 = 6,
  L7 = 7,
  L8 = 8,
  L9 = 9,
  Highest = 10
};
/* GetOperPriorityString
 */
std::string GetOperPriorityString(OperPriority opp);
/* enum class OperCategory
 * 运算符类型
 * 左/右括号，分隔符单独区分
 */
enum class OperCategory : unsigned char {
  Prefix = 0,     // 前缀操作符
  Infix = 1,      // 中缀操作符
  Suffix = 2,     // 后缀操作符
  PrefOrSuff = 3, // 前缀/后缀操作符
  LBracket = 4,   // 左括号
  RBracket = 5,   // 右括号
  Delimiter = 6,  // 分隔符
};
/* GetOperCategoryString
 */
std::string GetOperCategoryString(OperCategory cate);
/* class ExprDataBase
 * 将Num、Variable、OperWithArgs视作同一类型
 */
class ExprDataBase {
public:
  // 数据类型
  virtual DataType GetType() const = 0;
  // 根据变量表计算值，ExprNum的变量表可以为nullptr
  virtual [[nodiscard]] ResultPtr_Ty
  Calculate(VarTablePtr_Ty varTable) const = 0;
};
/* class ExprNum
 */
class ExprNum final : public ExprDataBase {
private:
  Number_Ty Data;

public:
  // 构造
  ExprNum();
  //
  ExprNum(Number_Ty num);
  //
  void operator=(const ExprNum &other);
  //
  std::string ToString() const;
  //
  [[nodiscard]] ResultPtr_Ty Calculate(VarTablePtr_Ty varTable) const override;
  //
  DataType GetType() const override { return DataType::Value; }
  //
  Number_Ty GetData() const { return Data; }
  //
  static Number_Ty GetValue(ArgPtr_Ty numPtr);
};
/* class ExprVar
 */
class ExprVar final : public ExprDataBase {
private:
  std::string Data; // 变量名
public:
  //
  ExprVar();
  ExprVar(const std::string &vna);
  ExprVar(std::string &&vna);
  ExprVar(const ExprVar &other);
  //
  void operator=(const ExprVar &other);
  //
  std::string ToString() const;
  //
  [[nodiscard]] ResultPtr_Ty Calculate(VarTablePtr_Ty varTable) const override;
  //
  DataType GetType() const override { return DataType::Variable; }
  //
  std::string GetData() const { return std::string(Data); }
  //
  static Number_Ty GetVarValue(ArgPtr_Ty argPtr, VarTablePtr_Ty varTablePtr);
};
/* class ExprOper
 *
 */
class ExprOper {
private:
  std::string MatchSymbol;  // 用于匹配的符号（可能有多个）
  std::string InternalName; // 内部名称（唯一）
  OperCategory Category;    // 符号类型
  size_t NeededArgsCount;   // 大于2时为多参数
  //
  OperPriority NormalPriority; // 表达式优先级
  OperPriority StackPriority;  // 栈内优先级
  //
  ArgValidation_Ty ArgVerifier; // 参数验证，某些函数有特殊规则
  OperExecution_Ty Executer;
  OperHelper_Ty Helper;

public:
  //
  ExprOper() = delete;
  ExprOper(const std::string &msymbol, const std::string &iname,
           OperCategory pos, size_t nargc, OperPriority npri, OperPriority spri,
           ArgValidation_Ty argVerifier, OperExecution_Ty exer,
           OperHelper_Ty helper);
  //
  std::string ToString() const;
  //
  std::string GetMatchSymbol() const { return MatchSymbol; }
  //
  std::string GetInternalName() const { return InternalName; }
  //
  OperCategory GetOpCategory() const { return Category; }
  //
  size_t GetNeededArgsCount() const { return NeededArgsCount; }
  //
  OperPriority GetNormalPriority() const { return NormalPriority; }
  //
  OperPriority GetStackPriority() const { return StackPriority; }
  //
  std::string GetBrief() const { return Helper(false); }
  std::string GetDetail() const { return Helper(true); }
  //
  bool VerifyArgs(const ArgPtrColl_Ty &argColl, VarTablePtr_Ty varTab) const {
    return ArgVerifier(argColl, varTab, NeededArgsCount);
  }
  //
  ResultPtr_Ty Execute(const ArgPtrColl_Ty &argColl,
                       VarTablePtr_Ty varTab) const {
    return Executer(argColl, varTab);
  }
};
/*
 */
class ExprOperWithArgs final : public ExprDataBase {
private:
  OperCPtr_Ty OperCPtr;
  ArgPtrColl_Ty Args;

public:
  ExprOperWithArgs() = delete;
  ExprOperWithArgs(OperCPtr_Ty opCPtr, const ArgPtrColl_Ty &args);
  ExprOperWithArgs(OperCPtr_Ty opCPtr, ArgPtrColl_Ty &&args);
  ExprOperWithArgs(const ExprOperWithArgs &other);
  //
  std::string ToString() const;
  //
  [[nodiscard]] ResultPtr_Ty Calculate(VarTablePtr_Ty varTable) const override;
  //
  DataType GetType() const override { return DataType::Variable; }
  //
  bool IsValid(VarTablePtr_Ty varTable) const;
  /* TrySimplify
   * 尝试化简
   * 在所有参数均为num时可化简
   * 你需要自行构建一个std::shared_ptr<ExprNum>的指针然后转型为ResultPtr
   */
  bool TrySimplify(ResultPtr_Ty &resNum) const;
  //
  OperCPtr_Ty GetOperPtr() const { return OperCPtr; }
  //
  ArgPtrColl_Ty &GetArgsColl() { return Args; }
};
/*
 */
class ExprOperMan {
private:
  static std::vector<ExprOper> RegisteredOperColl;
  static bool ValidTag;

public:
  static void Init();
  //
  static bool IsInitilized() { return ValidTag; }
  //
  static const std::vector<ExprOper> &GetColl() { return RegisteredOperColl; }
  //
  static size_t SearchSymbol(const char *tarStr, size_t tarStrLen);
  static size_t SearchSymbol(const std::string &tarStr);
  static size_t SearchSymbol(const std::string_view &tarStr);
  //
  static bool SymbolSynaxMatch(SymbolInfoColl_Ty siColl, size_t tarPos,
                               size_t &resFPos);
  //
  static size_t SearchInName(const char *tarStr, size_t tarStrLen);
  static size_t SearchInName(const std::string &tarStr);
  static size_t SearchInName(const std::string_view &tarStr);
  //
  static size_t Register(ExprOper &&tarOper);
  //
  static OperCPtr_Ty GetOperById(size_t id);
  //
  static size_t Find1stSymbol(const char *tar, size_t tarLen, size_t posSt,
                              size_t &posEdRes);
  static size_t Find1stSymbol(const std::string &tarStr, size_t posSt,
                              size_t &posEdRes);
  static size_t Find1stSymbol(const std::string_view &tarStr, size_t posSt,
                              size_t &posEdRes);
  //
  static bool StartWithSymbol(const char *tar, size_t tarLen, size_t &posEdRes);
  static bool StartWithSymbol(const std::string &tarStr, size_t &posEdRes);
  static bool StartWithSymbol(const std::string_view &tarStr, size_t &posEdRes);
  //
  static std::string GetRegisteredOperInfo();
  //
  static OperCPtr_Ty GetLeftBrA();
  //
  static OperCPtr_Ty GetRightBrA();
  //
  static bool IsMatchedBracket(OperCPtr_Ty left, OperCPtr_Ty right);
  //
  static OperCPtr_Ty GetDelimiterA();
  //
};
/*
 */
class ExprTeleprompter {
private:
  std::string Origin;
  std::string Trimed;
  size_t CurrentPos;
  SymbolInfoColl_Ty SymbolInfoColl;

public:
  //
  ExprTeleprompter() = delete;
  ExprTeleprompter(const char *origin, size_t originLen);
  ExprTeleprompter(const std::string &origin);
  ExprTeleprompter(const std::string_view &origin);
  //
  void ResetCurrent();
  //
  bool ToNextSymbol(SymbolInfo_Ty &resSI);
  //
  bool ChkCurrentSymbol(SymbolInfo_Ty &resSi) const;
  //
  bool ChkNextSymbol(SymbolInfo_Ty &resSi) const;
  //
  bool ChkPreviousSymbol(SymbolInfo_Ty &resSi) const;
  //
  bool GetSymbolByRelative(int offset, SymbolInfo_Ty &resSi) const;
  //
  std::string ToString() const;
  //
  static size_t BracketCategory(const char *tarStr, size_t tarStrLen,
                                bool LOrRTag);
  static size_t BracketCategory(const std::string &tarStr, bool LOrRTag);
  static size_t BracketCategory(const std::string_view &tarStr, bool LOrRTag);
  //
  static size_t IsLorRBracket(const char *tarStr, size_t tarStrLen);
  static size_t IsLorRBracket(const std::string &tarStr);
  static size_t IsLorRBracket(const std::string_view &tarStr);
  //
  static size_t StartWithBracket(const char *tarStr, size_t tarStrLen,
                                 bool LOrRTag);
  static size_t StartWithBracket(const std::string &tarStr, bool LOrRTag);
  static size_t StartWithBracket(const std::string_view &tarStr, bool LOrRTag);
  //
  static size_t StartWithLOrRBracket(const char *tarStr, size_t tarStrLen);
  static size_t StartWithLOrRBracket(const std::string &tarStr);
  static size_t StartWithLOrRBracket(const std::string_view &tarStr);
  //
  static size_t Find1stBracket(const char *tarStr, size_t tarStrLen,
                               bool LOrRTag, size_t &resBrId);
  static size_t Find1stBracket(const std::string &tarStr, bool LOrRTag,
                               size_t &resBrId);
  static size_t Find1stBracket(const std::string_view &tarStr, bool LOrRTag,
                               size_t &resBrId);
  //
  static size_t Find1stNumber(const char *tarStr, size_t tarStrLen,
                              size_t &numPosEd);
  static size_t Find1stNumber(const std::string &tarStr, size_t &numPosEd);
  static size_t Find1stNumber(const std::string_view &tarStr, size_t &numPosEd);
  //
  static bool IsIgnoredCh(char tarCh);
  //
  static bool IsDelimiter(char tarCh);
  //
  static bool IsValidArgCount(unsigned int count);
  //
  static bool IsChInColl(char tarCh, const std::string &tarStr, size_t &fpos);
  //
  static bool IsDigit(char ch, bool dotTag);
  //
  static bool IsValidMatchSymbol(const char *maNa, size_t maNaLen);
  static bool IsValidMatchSymbol(const std::string &maNa);
  static bool IsValidMatchSymbol(const std::string_view &maNa);
  //
  static bool IsValidInName(const char *inNa, size_t inNaLen);
  static bool IsValidInName(const std::string &inNa);
  static bool IsValidInName(const std::string_view &inNa);
  //
  static bool IsValidVarName(const char *varNa, size_t varNaLen);
  static bool IsValidVarName(const std::string &varNa);
  static bool IsValidVarName(const std::string_view &varNa);
  //
  static void ToUpperStringOnSite(char *tar, size_t tarLen);
  static void ToUpperStringOnSite(std::string &tarStr);
  static void ToUpperStringOnSite(std::string_view &tarStr);
  //
  static std::string ToUpperStringCpy(const char *tarStr, size_t tarStrLen);
  static std::string ToUpperStringCpy(const std::string &tarStr);
  static std::string ToUpperStringCpy(const std::string_view &tarStr);
  static bool IsValidCString(const char *tarStr, size_t tarStrLen);

private:
  //
  static std::string TrimAndFix(const std::string_view &tar);

  //
  static bool IsOriginValid(const std::string_view &tar);
  //
  static bool TryParseString(const std::string &trimed,
                             SymbolInfoColl_Ty &siColl);
  //
  static bool TryGetSufPosNum(const std::string &tarStr, size_t curPos,
                              size_t &sufPos);
  //
  static bool TryGetSufPosOper(const std::string &tarStr, size_t curPos,
                               size_t &sufPos);
  //
  static bool TryGetSufPosVariable(const std::string &tarStr, size_t curPos,
                                   size_t &sufPos);
  //
};

} // namespace Expr