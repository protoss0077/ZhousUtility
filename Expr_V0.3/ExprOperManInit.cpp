#include "PresetFuncs.h"

void Expr::ExprOperMan::Init() {
  using namespace Expr::PresetFunctions;
  size_t rc = 0;
  // pi
  Register(ExprOper("pi", "pi_sn", OperCategory::Suffix, 0,
                    OperPriority::Highest, OperPriority::Highest,
                    ArgumentsCountVerifier, PINone, PISNHelper));
  // e
  Register(ExprOper("e", "e_sn", OperCategory::Suffix, 0, OperPriority::Highest,
                    OperPriority::Highest, ArgumentsCountVerifier, ENone,
                    ESNHelper));
  // + 正号
  Register(ExprOper("+", "positive_ps", OperCategory::Prefix, 1,
                    OperPriority::Highest, OperPriority::Highest,
                    ArgumentsCountVerifier, PositiveSingle, PositivePSHelper));
  // + 中缀
  Register(ExprOper("+", "add_id", OperCategory::Infix, 2, OperPriority::L1,
                    OperPriority::L2, ArgumentsCountVerifier, AddDouble,
                    AddIDHelper));
  // add 前/后缀
  Register(ExprOper("add", "add_bm", OperCategory::PrefOrSuff, 3,
                    OperPriority::Highest, OperPriority::Highest,
                    ArgumentsCountVerifier, AddMultiple, AddPMHelper));
  // - 负号
  Register(ExprOper("-", "negative_ps", OperCategory::Prefix, 1,
                    OperPriority::Highest, OperPriority::Highest,
                    ArgumentsCountVerifier, NegativeSingle, NegativePSHelper));
  // - 中缀
  Register(ExprOper("-", "sub_id", OperCategory::Infix, 2, OperPriority::L1,
                    OperPriority::L2, ArgumentsCountVerifier, SubDouble,
                    SubIDHelper));
  // sub 前/后缀
  Register(ExprOper("sub", "sub_bm", OperCategory::PrefOrSuff, 3,
                    OperPriority::Highest, OperPriority::Highest,
                    ArgumentsCountVerifier, SubMultiple, SubHelper));
  // avg
  Register(ExprOper("avg", "avg_bm", OperCategory::PrefOrSuff, 3,
                    OperPriority::Highest, OperPriority::Highest,
                    ArgumentsCountVerifier, AvgMultiple, AvgHelper));
  // ! 阶乘 后缀
  Register(ExprOper("!", "fact_ss", OperCategory::Suffix, 1,
                    OperPriority::Highest, OperPriority::Highest,
                    ArgumentsCountVerifier, FactSingle, FactSSHelper));
  // fact 前/后缀
  Register(ExprOper("fact", "fact_bs", OperCategory::PrefOrSuff, 1,
                    OperPriority::Highest, OperPriority::Highest,
                    ArgumentsCountVerifier, FactSingle, FactSSHelper));
  // * 中缀乘法
  Register(ExprOper("*", "mul_id", OperCategory::Infix, 2, OperPriority::L2,
                    OperPriority::L3, ArgumentsCountVerifier, MulDouble,
                    MulIDHelper));
  // / 中缀除法
  Register(ExprOper("/", "div_id", OperCategory::Infix, 2, OperPriority::L2,
                    OperPriority::L3, DivIDArgsVerifier, DivDouble,
                    DivIDHelper));
  // ^ 中缀乘方
  Register(ExprOper("^", "pow_id", OperCategory::Infix, 2, OperPriority::L3,
                    OperPriority::L4, ArgumentsCountVerifier, PowDouble,
                    PowIDHelper));
  // pow 前/后缀乘方
  Register(ExprOper("pow", "pow_bd", OperCategory::PrefOrSuff, 2,
                    OperPriority::Highest, OperPriority::Highest,
                    ArgumentsCountVerifier, PowDouble, PowHelper));
  //
  // ( 左括号
  Register(ExprOper("(", "lbrA_pn", OperCategory::LBracket, 0,
                    OperPriority::Highest, OperPriority::Lowest,
                    ArgumentsCountVerifier, NOOP, LeftBracketHelper));
  // ) 右括号
  Register(ExprOper(")", "rbrA_pn", OperCategory::RBracket, 0,
                    OperPriority::Highest, OperPriority::Highest,
                    ArgumentsCountVerifier, NOOP, RightBracketHelper));
  // , 分隔符
  Register(ExprOper(",", "DelimA_id", OperCategory::Delimiter, 0,
                    OperPriority::L9, OperPriority::Lowest,
                    ArgumentsCountVerifier, NOOP, ArgDelimiterHelper));
  // ; 分隔符
  Register(ExprOper(";", "DelimB_id", OperCategory::Delimiter, 0,
                    OperPriority::L9, OperPriority::Lowest,
                    ArgumentsCountVerifier, NOOP, ArgDelimiterHelper));
  //
  ValidTag = true;
}