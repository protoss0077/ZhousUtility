#include "..\Expr_V0.3\ExprTypes.h"
//
#include <iostream>
//
int Test1() {
  using namespace Expr;
  std::vector<std::string> testStrColl{
      "1+2",                                     //
      "add(1,2)",                                //
      "1+2*3/4!-add(5,2,sub(4,2)!,add(2,3)!,6)", //
      "1+2*a/4!-add(5,b,sub(4,2)!,add(2,c)!,6)", //
      "1+2*a/4!-add(5,b,sub(4,2)!,(2,c)add!,6)"  //
  };
  for (const auto &s : testStrColl) {
    ExprAbstractSyntaxTree EAST(s);
    std::cout << EAST.ToString();
    if (EAST.GetVariableCount() > 1) {
      EAST.VariableSet("a", {1, 2, 3});
      EAST.VariableSet("b", {2, 4, 6});
      EAST.VariableSet("c", {3, 6, 9});
      std::cout << "\nCheck Variable [A]:\n" << EAST.ChkVariable("a");
      std::cout << "\nCheck Variable [B]:\n" << EAST.ChkVariable("b");
      std::cout << "\nCheck Variable [C]:\n" << EAST.ChkVariable("c");
      std::cout << "\nCheck Variable [D]:\n" << EAST.ChkVariable("d");
      ResultNumPtr_Ty tmpRes = std::make_shared<ExprNum>();
      if (EAST.TryCalculate(tmpRes))
        std::cout << "\nCalled TryCalculate Successed,Result:"
                  << tmpRes->ToString();
      else
        std::cout << "\nCalled TryCalculate Failed!";
      EAST.VariableConsume();
      //
      std::cout << "\nCheck Variable [A]:\n" << EAST.ChkVariable("a");
      std::cout << "\nCheck Variable [B]:\n" << EAST.ChkVariable("b");
      std::cout << "\nCheck Variable [C]:\n" << EAST.ChkVariable("c");
      std::cout << "\nCheck Variable [D]:\n" << EAST.ChkVariable("d");
      if (EAST.TryCalculate(tmpRes))
        std::cout << "\nCalled TryCalculate Successed,Result:"
                  << tmpRes->ToString();
      else
        std::cout << "\nCalled TryCalculate Failed!";
      EAST.VariableConsume();
      //
      std::cout << "\nCheck Variable [A]:\n" << EAST.ChkVariable("a");
      std::cout << "\nCheck Variable [B]:\n" << EAST.ChkVariable("b");
      std::cout << "\nCheck Variable [C]:\n" << EAST.ChkVariable("c");
      std::cout << "\nCheck Variable [D]:\n" << EAST.ChkVariable("d");
      if (EAST.TryCalculate(tmpRes))
        std::cout << "\nCalled TryCalculate Successed,Result:"
                  << tmpRes->ToString();
      else
        std::cout << "\nCalled TryCalculate Failed!";
      std::cout << "\n--- next subtest ---\n";
    }
  }
  return 0;
}

/************************************************/
int main() {
  try {
    //
    std::cout << "\n Starting Test -= 1 =-\n"
              << ((Test1() == 0) ? "\nSuccessed" : "\nFailed");

  } catch (const std::runtime_error &re) {
    std::cout << "\n运行时错误!\n错误消息: " << re.what();
  } catch (...) {
    std::cout << "\n未知错误类型";
  }

  std::cout << "\n All Test Done. Press Any Key To Exit..." << std::endl;
  std::cin.get();
  return 0;
}