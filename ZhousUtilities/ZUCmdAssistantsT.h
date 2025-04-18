/* Assistants.h
 * 模板代码,以头文件形式提供
 */
#pragma once
//
#include <iterator>
#include <string>
#include <type_traits>
//
namespace ZhousUtilities {
/* GetLegacyIteratorTypeString
 * 获取迭代器类型的字符串
 * 按种类：
 * 是/否 常量迭代器
 * 是/否 连续迭代器
 * 是/否 输入/输出迭代器
 * 是/否 前向/双向/随机迭代器
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
  // 是否 常量迭代器
  if (std::is_const_v<typename std::remove_reference_t<decltype(*tarIter)>>) {
    sres = sprintf_s(inBuf + curSize, 128 - curSize, "Const");
    if (sres > 0)
      curSize += sres;
  }
  // 是否 连续迭代器
  if constexpr (std::contiguous_iterator<Iter_Ty>) {
    sres = sprintf_s(inBuf + curSize, 128 - curSize, "%sMemory_Contiguous",
                     (curSize > 0) ? "_" : "");
    curSize += (sres > 0) ? sres : 0;
  }
  // 是否 输入/输出迭代器
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
  // 是否 可读/可写迭代器
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
  // 是否 前向/双向/随机迭代器
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