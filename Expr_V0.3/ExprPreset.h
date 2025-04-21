#pragma once

/* 数据先尝试使用STL
 */
#include <queue>
#include <string>
#include <unordered_map>
//
#include <list>
#include <vector>
//
#include <functional>
//
#include <memory>
//
#include <stdexcept>
//
#include <stack>
//
namespace Expr {
// 无需多言
#define nameof(x) #x
// 数值类型
using Number_Ty = double;
// 数值类型的误差精度
static constexpr const Number_Ty NumErrorPrecision = 0.000000000001;
// 数值类型全套比较
// ==
bool NumEQ(const Number_Ty &left, const Number_Ty &right);
// !=
bool NumNE(const Number_Ty &left, const Number_Ty &right);
// <
bool NumLT(const Number_Ty &left, const Number_Ty &right);
// <=
bool NumLE(const Number_Ty &left, const Number_Ty &right);
// >
bool NumGT(const Number_Ty &left, const Number_Ty &right);
// >=
bool NumGE(const Number_Ty &left, const Number_Ty &right);
// 转换
#define NumTrans(n) static_cast<Number_Ty>((n))
// 变量值队列
using VarValueQueue_Ty = std::queue<Number_Ty>;
// 变量表
using VarTable_Ty = std::unordered_map<std::string, VarValueQueue_Ty>;
using VarTablePtr_Ty = VarTable_Ty *;
using VarTableCPtr_Ty = const VarTable_Ty *;
// 忽略的分隔符，传入的表达式全转换为大写并去掉分隔符
static const std::string IgnoredDelimiter{" _"};
// 可用的括号
static std::vector<std::pair<const std::string, const std::string>>
    ValidBracketColl{{"(", ")"}, {"[", "]"}, {"{", "}"}};
// 可用的分隔符参数分隔
static const std::string ValidDelimiter{",;"};
// 内部容器的非法位置
static const size_t InvalidPos = size_t(-1);
// 可变参数操作符接受的最大参数数量
static constexpr const unsigned int ArgCountLimit{16};
// 变量名最长字符串
static constexpr const unsigned int MaxVarNameLength{12};
// 变量名最短字符串
static constexpr const unsigned int MinVarNameLength{1};
// 符号显示名最长
static constexpr const unsigned int MaxOperNameLength{32};
// 符号显示名最短
static constexpr const unsigned int MinOperNameLength{1};
// 符号匹配名最长,匹配名为全大写,无数字,
static constexpr const unsigned int MaxOperSymbolLength{12};
// 符号匹配名最短
static constexpr const unsigned int MinOperSymbolLength{1};
// 内部名称最长,内部名为全大写,无数字,最后一个字符表示参数数量
static constexpr const unsigned int MaxOperInNameLength{12};
// 内部名称最短
static constexpr const unsigned int MinOperInNameLength{4};
// 变量名非法字符
static const std::string IllegalVarNameChColl{".>,<:;+=-_*&/?%~`!@#$^|\\'\""};
// 表达式最大长度
static constexpr const unsigned int MaxExpressionLength{1024};
// 表达式最小长度
static constexpr const unsigned int MinExpressionLength{1};

/***************************
 * static messages *
 ***************************
 */
//
static const std::string InvalidDataType{"Node's data type error. Need type: "};
// 无效变量名
static const std::string InvalidVarNameMsg{"Invalid Variable Name: "};
// 在变量表中未找到
static const std::string NoSuchVarNameMsg{
    "There's no such target in variable table. Variable Name: "};
// 变量表为nullptr
static const std::string NullVarTableMsg{"Variable Table is nullptr!!!"};
// 变量数据队列为空
static const std::string NoVarDataMsg{
    "Variable data is empty. Append some data to it's queue. Variable Name: "};
//
static const std::string InvalidVarValueMsg{"Variable value is NAN!"};
// 无效的符号匹配字符串
static const std::string InvalidOperMaNameMsg{"Invalid Operator Match Name: "};
// 无效的符号内部名称
static const std::string InvalidOperInNameMsg{
    "Invalid Operator Internal Name: "};
//
// static const std::string NullOperMsg{"Operator's nullptr!"};
// 无效oper或者参数
static const std::string InvalidOperOrArgMsg{
    "Operator or Arguments is Invalid!"};
// 需要的参数数量有误
static const std::string InvalidArgCountMsg{
    "Operator's arguments count out of limit range. Count: "};
// 执行时出现无效参数
static const std::string InvalidArgWhenExecuteMsg{
    "An Error occured when Operator Execute!"};
// 表达式过长
static const std::string ExpressionOverLengthMsg{
    "Target Expression's too long.Length: "};
// 无效 oper id
static const std::string OperIdOutofRangeMsg{
    "Operator id is out of registered manager's range.Target Id: "};
// Oper未注册
static const std::string OperNotRegisteredMsg{
    "No such Operator in Registered Operator Manager.Target Operator "};
//
// 表达式过长
static const std::string ExpressionLengthOverLimitMsg{
    "Expression's too Long.Expression: "};
// 表达式解析错误
static const std::string ExpressionParseErrorMsg{"Expression Parse Error : "};

} // namespace Expr
