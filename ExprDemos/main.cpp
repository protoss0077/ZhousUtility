#include "..\Expr_V0.3\ExprTypes.h"
//
#include <iostream>
//
int Test1() {
  using namespace Expr;
  std::string testExprString{
      "12_3.45_6 + .567 - (12.3_4 -       a) +      add(1.23, sub(2.34 * b, "
      "4.56*c), (4.67 - 2.12) !, 3.14) + 12_34 + y"};
  try {

    ExprTeleprompter et(testExprString);
    std::cout << "\nOrigin Expression:\n"
              << et.GetOrigin() << "\nTrimed Expression:\n"
              << et.GetTrimed() << "\nTeleprompter Parse Result:\n";
    const auto &etInColl = et.GetInternalColl();
    size_t curPos = 0;
    for (; curPos < etInColl.size(); ++curPos) {
      std::cout << "\nSymbol [" << curPos << "]: " << etInColl[curPos].first
                << " ; Symbol Type: "
                << GetDataTypeString(etInColl[curPos].second);
    }
    std::cout << "\n--- SPLIT LINE ---\n";
    //
    auto res = ExprSyntaxPaser::TrySyntaxPaser(et);
    std::cout << res->ToString();
  } catch (...) {
    throw;
  }
  return 0;
}

/************************************************/
int main() {
  try {
    //
    std::cout << "\n Starting Test -= 1 =-\n"
              << ((Test1() == 0) ? "Successed" : "Failed");

  } catch (const std::exception &e) {
    std::cout << "\nAn Error Occured.Error Message:\n" << e.what();
  }

  std::cout << "\n All Test Done. Press Any Key To Exit..." << std::endl;
  std::cin.get();
  return 0;
}