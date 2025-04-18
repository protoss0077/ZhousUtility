/* Assistants.h
 * ģ�����,��ͷ�ļ���ʽ�ṩ
 */
#pragma once
//
#include <iterator>
#include <string>
#include <type_traits>
//
namespace ZhousUtilities {
/* GetLegacyIteratorTypeString
 * ��ȡ���������͵��ַ���
 * �����ࣺ
 * ��/�� ����������
 * ��/�� ����������
 * ��/�� ����/���������
 * ��/�� ǰ��/˫��/���������
 */
template <typename Iter_Ty>
std::string GetLegacyIteratorTypeString(Iter_Ty tarIter) {
  using IterCategoryTrait =
      typename std::iterator_traits<Iter_Ty>::iterator_category;
  using IterValueTrait = typename std::iterator_traits<Iter_Ty>::value_type;
  static char inBuf[128] = "\0";
  inBuf[0] = '\0';
  int curSize = 0;
  int sres = 0;
  // �Ƿ� ����������
  if (std::is_const_v<typename std::remove_reference_t<decltype(*tarIter)>>) {
    sres = sprintf_s(inBuf + curSize, 128 - curSize, "Const");
    if (sres > 0)
      curSize += sres;
  }
  // �Ƿ� ����������
  if constexpr (std::contiguous_iterator<Iter_Ty>) {
    sres = sprintf_s(inBuf + curSize, 128 - curSize, "%sMemory_Contiguous",
                     (curSize > 0) ? "_" : "");
    curSize += (sres > 0) ? sres : 0;
  }
  // �Ƿ� ����/���������
  if constexpr (std::is_same_v<IterCategoryTrait, std::input_iterator_tag>) {
    sres = sprintf_s(inBuf + curSize, 128 - curSize, "%sInput",
                     (curSize > 0) ? "_" : "");
    curSize += (sres > 0) ? sres : 0;
  }
  if constexpr (std::is_same_v<IterCategoryTrait, std::output_iterator_tag>) {
    sres = sprintf_s(inBuf + curSize, 128 - curSize, "%sOutput",
                     (curSize > 0) ? "_" : "");
    curSize += (sres > 0) ? sres : 0;
  }
  // �Ƿ� �ɶ�/��д������
  if constexpr (std::input_iterator<Iter_Ty>) {
    sres = sprintf_s(inBuf + curSize, 128 - curSize, "%sReadable",
                     (curSize > 0) ? "_" : "");
    curSize += (sres > 0) ? sres : 0;
  }
  if constexpr (std::output_iterator<Iter_Ty, IterValueTrait>) {
    sres = sprintf_s(inBuf + curSize, 128 - curSize, "%sWritable",
                     (curSize > 0) ? "_" : "");
    curSize += (sres > 0) ? sres : 0;
  }
  // �Ƿ� ǰ��/˫��/���������
  if constexpr (std::is_same_v<IterCategoryTrait,
                               std::random_access_iterator_tag>) {
    sres = sprintf_s(inBuf + curSize, 128 - curSize, "%sRandom",
                     (curSize > 0) ? "_" : "");
    curSize += (sres > 0) ? sres : 0;
  }
  if constexpr (std::is_same_v<IterCategoryTrait,
                               std::bidirectional_iterator_tag>) {
    sres = sprintf_s(inBuf + curSize, 128 - curSize, "%sBidirectional",
                     (curSize > 0) ? "_" : "");
    curSize += (sres > 0) ? sres : 0;
  }
  if constexpr (std::is_same_v<IterCategoryTrait, std::forward_iterator_tag>) {
    sres = sprintf_s(inBuf + curSize, 128 - curSize, "%sForward",
                     (curSize > 0) ? "_" : "");
    curSize += (sres > 0) ? sres : 0;
  }
  sprintf_s(inBuf + curSize, 128 - curSize, " Iterator");
  return std::string(inBuf);
}
} // namespace ZhousUtilities