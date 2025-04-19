#pragma once
#include "ExprTypes.h"

namespace Expr {
namespace PresetFunctions {
/***************************
 * preset Executers *
 * ���е�ִ�к��������������ͨ��verifier��֤ *
 * ע��verifier����varTableΪnullptr����� *
 * �ɽڵ��Calculate�ӿڱ�֤ *
 ***************************
 */
/* NOOP
 * ����������ռλ��
 */
ResultPtr_Ty NOOP(ArgPtrColl_Ty &argPtrColl, VarTablePtr_Ty varTable);
/* PINone
 * Բ���� ��׺���޲�
 */
ResultPtr_Ty PINone(ArgPtrColl_Ty &argPtrColl, VarTablePtr_Ty varTable);
/* ENone
 * ��Ȼ����e ��׺���޲�
 */
ResultPtr_Ty ENone(ArgPtrColl_Ty &argPtrColl, VarTablePtr_Ty varTable);
/* PositiveSingle
 * �� ����(ǰ׺,����)
 */
ResultPtr_Ty PositiveSingle(ArgPtrColl_Ty &argPtrColl, VarTablePtr_Ty varTable);
/* AddDouble
 * + �Ӻ�(��׺,˫��)
 */
ResultPtr_Ty AddDouble(ArgPtrColl_Ty &argPtrColl, VarTablePtr_Ty varTable);
/* AddMultiple
 * add (ǰ/��׺�����)
 */
ResultPtr_Ty AddMultiple(ArgPtrColl_Ty &argPtrColl, VarTablePtr_Ty varTable);
/* NegativeSingle
 * - ����(ǰ׺������)
 */
ResultPtr_Ty NegativeSingle(ArgPtrColl_Ty &argPtrColl, VarTablePtr_Ty varTable);
/* SubDouble
 * - ����(��׺��˫��)
 */
ResultPtr_Ty SubDouble(ArgPtrColl_Ty &argPtrColl, VarTablePtr_Ty varTable);
/* SubMultiple
 * sub ����(ǰ/��׺�����)
 */
ResultPtr_Ty SubMultiple(ArgPtrColl_Ty &argPtrColl, VarTablePtr_Ty varTable);
/* AvgMultiple
 * avg ��ƽ��ֵ(ǰ/��׺�����)
 */
ResultPtr_Ty AvgMultiple(ArgPtrColl_Ty &argPtrColl, VarTablePtr_Ty varTable);
/* FactSingle
 * ! fact ��׳�(��׺������)
 * �������������֣�������100��
 */
ResultPtr_Ty FactSingle(ArgPtrColl_Ty &argPtrColl, VarTablePtr_Ty varTable);
/* MulDouble
 * * ��׺�˷�
 */
ResultPtr_Ty MulDouble(ArgPtrColl_Ty &argPtrColl, VarTablePtr_Ty varTable);
/* DivDouble
 * / ��׺����
 */
ResultPtr_Ty DivDouble(ArgPtrColl_Ty &argPtrColl, VarTablePtr_Ty varTable);
/* PowDouble
 * ^ pow �˷�
 */
ResultPtr_Ty PowDouble(ArgPtrColl_Ty &argPtrColl, VarTablePtr_Ty varTable);
/***************************
 * preset arguments verifiers *
 ***************************
 */
/* NoNeedVerifier
 * �޲���
 */
bool NoNeedVerifier(const Expr::ArgPtrColl_Ty &argPtrColl,
                    Expr::VarTablePtr_Ty varTable);
/* SingleArgVerifier
 * ��������
 */
bool SingleArgVerifier(const Expr::ArgPtrColl_Ty &argPtrColl,
                       Expr::VarTablePtr_Ty varTable);
/* DoubleArgsVerifier
 * ˫����
 */
bool DoubleArgsVerifier(const Expr::ArgPtrColl_Ty &argPtrColl,
                        Expr::VarTablePtr_Ty varTable);
/* MultipleArgsVerifier
 * ���,������Ҫ1�������������б�Ϊ�ջ���������
 */
bool MultipleArgsVerifier(const Expr::ArgPtrColl_Ty &argPtrColl,
                          Expr::VarTablePtr_Ty varTable, size_t nargc);
/* ArgumentsCountVerifier
 * ͨ����֤
 */
bool ArgumentsCountVerifier(const ArgPtrColl_Ty &argPtrColl,
                            VarTablePtr_Ty varTab, size_t nargc);
/* DivIDArgsVerifier
 * ��׺����������֤��������Ϊ0
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
/* + ����
 */
std::string PositivePSHelper(bool detailTag = true);
/* + ��׺
 */
std::string AddIDHelper(bool detailTag = true);
/* add ǰ׺
 */
std::string AddPMHelper(bool detailTag = true);
/* - ����
 */
std::string NegativePSHelper(bool detailTag = true);
/* - ��׺
 */
std::string SubIDHelper(bool detailTag = true);
/* sub ǰ׺
 */
std::string SubHelper(bool detailTag = true);
/* avg ǰ׺
 * ��ƽ��ֵ
 */
std::string AvgHelper(bool detailTag = true);
/* ! ��׺
 * ��׳�
 */
std::string FactSSHelper(bool detailTag = true);
/* * ��׺
 * left*right
 */
std::string MulIDHelper(bool detailTag = true);
/* / ��׺
 * left/right
 */
std::string DivIDHelper(bool detailTag = true);
/* ^ ��׺
 * left��right�η�
 */
std::string PowIDHelper(bool detailTag = true);
/* pow ǰ/��׺
 * left��right�η�
 */
std::string PowHelper(bool detailTag = true);
//
/* ([{ ������
 */
std::string LeftBracketHelper(bool detailTag = true);
/* )]} ������
 */
std::string RightBracketHelper(bool detailTag = true);
/* ,; �����ָ���
 */
std::string ArgDelimiterHelper(bool detailTag = true);

//

} // namespace PresetFunctions

} // namespace Expr