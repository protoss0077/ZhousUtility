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
ResultPtr_Ty NOOP(const ArgPtrColl_Ty &argPtrColl, VarTableCPtr_Ty varTable);
/* PINone
 * Բ���� ��׺���޲�
 */
ResultPtr_Ty PINone(const ArgPtrColl_Ty &argPtrColl, VarTableCPtr_Ty varTable);
/* ENone
 * ��Ȼ����e ��׺���޲�
 */
ResultPtr_Ty ENone(const ArgPtrColl_Ty &argPtrColl, VarTableCPtr_Ty varTable);
/* PositiveSingle
 * �� ����(ǰ׺,����)
 */
ResultPtr_Ty PositiveSingle(const ArgPtrColl_Ty &argPtrColl,
                            VarTableCPtr_Ty varTable);
/* AddDouble
 * + �Ӻ�(��׺,˫��)
 */
ResultPtr_Ty AddDouble(const ArgPtrColl_Ty &argPtrColl,
                       VarTableCPtr_Ty varTable);
/* AddMultiple
 * add (ǰ/��׺�����)
 */
ResultPtr_Ty AddMultiple(const ArgPtrColl_Ty &argPtrColl,
                         VarTableCPtr_Ty varTable);
/* NegativeSingle
 * - ����(ǰ׺������)
 */
ResultPtr_Ty NegativeSingle(const ArgPtrColl_Ty &argPtrColl,
                            VarTableCPtr_Ty varTable);
/* SubDouble
 * - ����(��׺��˫��)
 */
ResultPtr_Ty SubDouble(const ArgPtrColl_Ty &argPtrColl,
                       VarTableCPtr_Ty varTable);
/* SubMultiple
 * sub ����(ǰ/��׺�����)
 */
ResultPtr_Ty SubMultiple(const ArgPtrColl_Ty &argPtrColl,
                         VarTableCPtr_Ty varTable);
/* AvgMultiple
 * avg ��ƽ��ֵ(ǰ/��׺�����)
 */
ResultPtr_Ty AvgMultiple(const ArgPtrColl_Ty &argPtrColl,
                         VarTableCPtr_Ty varTable);
/* FactSingle
 * ! fact ��׳�(��׺������)
 * �������������֣�������100��
 */
ResultPtr_Ty FactSingle(const ArgPtrColl_Ty &argPtrColl,
                        VarTableCPtr_Ty varTable);
/* MulDouble
 * * ��׺�˷�
 */
ResultPtr_Ty MulDouble(const ArgPtrColl_Ty &argPtrColl,
                       VarTableCPtr_Ty varTable);
/* DivDouble
 * / ��׺����
 */
ResultPtr_Ty DivDouble(const ArgPtrColl_Ty &argPtrColl,
                       VarTableCPtr_Ty varTable);
/* PowDouble
 * ^ pow �˷�
 */
ResultPtr_Ty PowDouble(const ArgPtrColl_Ty &argPtrColl,
                       VarTableCPtr_Ty varTable);
/***************************
 * preset arguments verifiers *
 ***************************
 */
/* NoNeedVerifier
 * �޲���
 */
bool NoNeedVerifier(const ArgPtrColl_Ty &argPtrColl, VarTableCPtr_Ty varTable);
/* SingleArgVerifier
 * ��������
 */
bool SingleArgVerifier(const ArgPtrColl_Ty &argPtrColl,
                       VarTableCPtr_Ty varTable);
/* DoubleArgsVerifier
 * ˫����
 */
bool DoubleArgsVerifier(const ArgPtrColl_Ty &argPtrColl,
                        VarTableCPtr_Ty varTable);
/* MultipleArgsVerifier
 * ���,������Ҫ1�������������б�Ϊ�ջ���������
 */
bool MultipleArgsVerifier(const ArgPtrColl_Ty &argPtrColl,
                          VarTableCPtr_Ty varTable, size_t nargc);
/* ArgumentsCountVerifier
 * ͨ����֤
 */
bool ArgumentsCountVerifier(const ArgPtrColl_Ty &argPtrColl,
                            VarTableCPtr_Ty varTab, size_t nargc);
/* DivIDArgsVerifier
 * ��׺����������֤��������Ϊ0
 */
bool DivIDArgsVerifier(const ArgPtrColl_Ty &argPtrColl, VarTableCPtr_Ty varTab,
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