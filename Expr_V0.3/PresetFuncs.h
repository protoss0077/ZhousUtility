#pragma once
#include "ExprTypes.h"

namespace Expr {
namespace PresetFunctions {
/***************************
 * preset Executers *
 * 所有的执行函数假设其参数已通过verifier验证 *
 * 注意verifier对于varTable为nullptr的情况 *
 * 由节点的Calculate接口保证 *
 ***************************
 */
/* NOOP
 * 不做操作的占位符
 */
ResultPtr_Ty NOOP(ArgPtrColl_Ty &argPtrColl, VarTablePtr_Ty varTable);
/* PINone
 * 圆周率 后缀，无参
 */
ResultPtr_Ty PINone(ArgPtrColl_Ty &argPtrColl, VarTablePtr_Ty varTable);
/* ENone
 * 自然对数e 后缀，无参
 */
ResultPtr_Ty ENone(ArgPtrColl_Ty &argPtrColl, VarTablePtr_Ty varTable);
/* PositiveSingle
 * ＋ 正号(前缀,单参)
 */
ResultPtr_Ty PositiveSingle(ArgPtrColl_Ty &argPtrColl, VarTablePtr_Ty varTable);
/* AddDouble
 * + 加号(中缀,双参)
 */
ResultPtr_Ty AddDouble(ArgPtrColl_Ty &argPtrColl, VarTablePtr_Ty varTable);
/* AddMultiple
 * add (前/后缀，多参)
 */
ResultPtr_Ty AddMultiple(ArgPtrColl_Ty &argPtrColl, VarTablePtr_Ty varTable);
/* NegativeSingle
 * - 负号(前缀，单参)
 */
ResultPtr_Ty NegativeSingle(ArgPtrColl_Ty &argPtrColl, VarTablePtr_Ty varTable);
/* SubDouble
 * - 减号(中缀，双参)
 */
ResultPtr_Ty SubDouble(ArgPtrColl_Ty &argPtrColl, VarTablePtr_Ty varTable);
/* SubMultiple
 * sub 减号(前/后缀，多参)
 */
ResultPtr_Ty SubMultiple(ArgPtrColl_Ty &argPtrColl, VarTablePtr_Ty varTable);
/* AvgMultiple
 * avg 求平均值(前/后缀，多参)
 */
ResultPtr_Ty AvgMultiple(ArgPtrColl_Ty &argPtrColl, VarTablePtr_Ty varTable);
/* FactSingle
 * ! fact 求阶乘(后缀，单参)
 * 仅处理整数部分，不大于100！
 */
ResultPtr_Ty FactSingle(ArgPtrColl_Ty &argPtrColl, VarTablePtr_Ty varTable);
/* MulDouble
 * * 中缀乘法
 */
ResultPtr_Ty MulDouble(ArgPtrColl_Ty &argPtrColl, VarTablePtr_Ty varTable);
/* DivDouble
 * / 中缀除法
 */
ResultPtr_Ty DivDouble(ArgPtrColl_Ty &argPtrColl, VarTablePtr_Ty varTable);
/* PowDouble
 * ^ pow 乘方
 */
ResultPtr_Ty PowDouble(ArgPtrColl_Ty &argPtrColl, VarTablePtr_Ty varTable);
/***************************
 * preset arguments verifiers *
 ***************************
 */
/* NoNeedVerifier
 * 无参数
 */
bool NoNeedVerifier(const Expr::ArgPtrColl_Ty &argPtrColl,
                    Expr::VarTablePtr_Ty varTable);
/* SingleArgVerifier
 * 单个参数
 */
bool SingleArgVerifier(const Expr::ArgPtrColl_Ty &argPtrColl,
                       Expr::VarTablePtr_Ty varTable);
/* DoubleArgsVerifier
 * 双参数
 */
bool DoubleArgsVerifier(const Expr::ArgPtrColl_Ty &argPtrColl,
                        Expr::VarTablePtr_Ty varTable);
/* MultipleArgsVerifier
 * 多参,至少需要1个参数，参数列表为空会视作错误
 */
bool MultipleArgsVerifier(const Expr::ArgPtrColl_Ty &argPtrColl,
                          Expr::VarTablePtr_Ty varTable, size_t nargc);
/* ArgumentsCountVerifier
 * 通用验证
 */
bool ArgumentsCountVerifier(const ArgPtrColl_Ty &argPtrColl,
                            VarTablePtr_Ty varTab, size_t nargc);
/* DivIDArgsVerifier
 * 中缀除法参数验证，除数不为0
 */
bool DivIDArgsVerifier(const ArgPtrColl_Ty &argPtrColl, VarTablePtr_Ty varTab,
                       size_t nargc);
/***************************
 * preset helpers *
 ***************************
 */
/* pi
 */
std::string PISNHelper(bool detailTag = true);
/* e
 */
std::string ESNHelper(bool detailTag = true);
/* + 正号
 */
std::string PositivePSHelper(bool detailTag = true);
/* + 中缀
 */
std::string AddIDHelper(bool detailTag = true);
/* add 前缀
 */
std::string AddPMHelper(bool detailTag = true);
/* - 负号
 */
std::string NegativePSHelper(bool detailTag = true);
/* - 中缀
 */
std::string SubIDHelper(bool detailTag = true);
/* sub 前缀
 */
std::string SubHelper(bool detailTag = true);
/* avg 前缀
 * 求平均值
 */
std::string AvgHelper(bool detailTag = true);
/* ! 后缀
 * 求阶乘
 */
std::string FactSSHelper(bool detailTag = true);
/* * 中缀
 * left*right
 */
std::string MulIDHelper(bool detailTag = true);
/* / 中缀
 * left/right
 */
std::string DivIDHelper(bool detailTag = true);
/* ^ 中缀
 * left的right次方
 */
std::string PowIDHelper(bool detailTag = true);
/* pow 前/后缀
 * left的right次方
 */
std::string PowHelper(bool detailTag = true);
//
/* ([{ 左括号
 */
std::string LeftBracketHelper(bool detailTag = true);
/* )]} 右括号
 */
std::string RightBracketHelper(bool detailTag = true);
/* ,; 参数分隔符
 */
std::string ArgDelimiterHelper(bool detailTag = true);

//

} // namespace PresetFunctions

} // namespace Expr