#pragma once

/* �����ȳ���ʹ��STL
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
// �������
#define nameof(x) #x
// ��ֵ����
using Number_Ty = double;
// ��ֵ���͵�����
static constexpr const Number_Ty NumErrorPrecision = 0.000000000001;
// ��ֵ����ȫ�ױȽ�
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
// ת��
#define NumTrans(n) static_cast<Number_Ty>((n))
// ����ֵ����
using VarValueQueue_Ty = std::queue<Number_Ty>;
// ������
using VarTable_Ty = std::unordered_map<std::string, VarValueQueue_Ty>;
using VarTablePtr_Ty = VarTable_Ty *;
using VarTableCPtr_Ty = const VarTable_Ty *;
// ���Եķָ���������ı��ʽȫת��Ϊ��д��ȥ���ָ���
static const std::string IgnoredDelimiter{" _"};
// ���õ�����
static std::vector<std::pair<const std::string, const std::string>>
    ValidBracketColl{{"(", ")"}, {"[", "]"}, {"{", "}"}};
// ���õķָ��������ָ�
static const std::string ValidDelimiter{",;"};
// �ڲ������ķǷ�λ��
static const size_t InvalidPos = size_t(-1);
// �ɱ�������������ܵ�����������
static constexpr const unsigned int ArgCountLimit{16};
// ��������ַ���
static constexpr const unsigned int MaxVarNameLength{12};
// ����������ַ���
static constexpr const unsigned int MinVarNameLength{1};
// ������ʾ���
static constexpr const unsigned int MaxOperNameLength{32};
// ������ʾ�����
static constexpr const unsigned int MinOperNameLength{1};
// ����ƥ�����,ƥ����Ϊȫ��д,������,
static constexpr const unsigned int MaxOperSymbolLength{12};
// ����ƥ�������
static constexpr const unsigned int MinOperSymbolLength{1};
// �ڲ������,�ڲ���Ϊȫ��д,������,���һ���ַ���ʾ��������
static constexpr const unsigned int MaxOperInNameLength{12};
// �ڲ��������
static constexpr const unsigned int MinOperInNameLength{4};
// �������Ƿ��ַ�
static const std::string IllegalVarNameChColl{".>,<:;+=-_*&/?%~`!@#$^|\\'\""};
// ���ʽ��󳤶�
static constexpr const unsigned int MaxExpressionLength{1024};
// ���ʽ��С����
static constexpr const unsigned int MinExpressionLength{1};

/***************************
 * static messages *
 ***************************
 */
//
static const std::string InvalidDataType{"Node's data type error. Need type: "};
// ��Ч������
static const std::string InvalidVarNameMsg{"Invalid Variable Name: "};
// �ڱ�������δ�ҵ�
static const std::string NoSuchVarNameMsg{
    "There's no such target in variable table. Variable Name: "};
// ������Ϊnullptr
static const std::string NullVarTableMsg{"Variable Table is nullptr!!!"};
// �������ݶ���Ϊ��
static const std::string NoVarDataMsg{
    "Variable data is empty. Append some data to it's queue. Variable Name: "};
//
static const std::string InvalidVarValueMsg{"Variable value is NAN!"};
// ��Ч�ķ���ƥ���ַ���
static const std::string InvalidOperMaNameMsg{"Invalid Operator Match Name: "};
// ��Ч�ķ����ڲ�����
static const std::string InvalidOperInNameMsg{
    "Invalid Operator Internal Name: "};
//
// static const std::string NullOperMsg{"Operator's nullptr!"};
// ��Чoper���߲���
static const std::string InvalidOperOrArgMsg{
    "Operator or Arguments is Invalid!"};
// ��Ҫ�Ĳ�����������
static const std::string InvalidArgCountMsg{
    "Operator's arguments count out of limit range. Count: "};
// ִ��ʱ������Ч����
static const std::string InvalidArgWhenExecuteMsg{
    "An Error occured when Operator Execute!"};
// ���ʽ����
static const std::string ExpressionOverLengthMsg{
    "Target Expression's too long.Length: "};
// ��Ч oper id
static const std::string OperIdOutofRangeMsg{
    "Operator id is out of registered manager's range.Target Id: "};
// Operδע��
static const std::string OperNotRegisteredMsg{
    "No such Operator in Registered Operator Manager.Target Operator "};
//
// ���ʽ����
static const std::string ExpressionLengthOverLimitMsg{
    "Expression's too Long.Expression: "};
// ���ʽ��������
static const std::string ExpressionParseErrorMsg{"Expression Parse Error : "};

} // namespace Expr
