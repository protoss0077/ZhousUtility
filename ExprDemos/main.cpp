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
      auto varNaColl = EAST.GetVariableNameColl();
      for (const auto &vn : varNaColl) {
        std::vector<Number_Ty> tmpValColl;
        if (ExprAbstractSyntaxTree::RangeSampled(tmpValColl, 1, 3, .5)) {
          std::cout << "\n������Χ [1 ~ 3] ; ������:.5\n����:[s]";
          for (size_t cnt = 0; cnt < tmpValColl.size() && cnt < 30; ++cnt) {
            if (cnt % 12 == 11)
              std::cout << "\n";
            std::cout << tmpValColl[cnt] << " ; ";
          }
        } else {
          std::cout << "\n����ʧ�ܣ�";
          break;
        }
        //
        std::cout << "\n[e]\n���ò�����" << vn;
        EAST.VariableSet(vn, tmpValColl);
        std::cout<<EAST.GetVariableInfo(vn);
      }
      //
      std::vector<ResultNumPtr_Ty> tmpResNumColl;
      for (;;) {
        ResultNumPtr_Ty tmpResNum = std::make_shared<ExprNum>();
        if (!EAST.TryCalculate(tmpResNum))
          break;
        tmpResNumColl.emplace_back(tmpResNum);
        EAST.VariableConsume();
        std::cout << "\nTryCalculate Successed.��鵱ǰ��������:";
        for (const auto &vn : varNaColl)
          std::cout << EAST.GetVariableInfo(vn);
      }
      //
      std::cout << "\n�����[s]\n";
      for (const auto &np : tmpResNumColl)
        std::cout << np->GetData() << " ; ";
      std::cout << "\n[e]";
      //
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
    std::cout << "\n����ʱ����!\n������Ϣ: " << re.what();
  } catch (...) {
    std::cout << "\nδ֪��������";
  }

  std::cout << "\n All Test Done. Press Any Key To Exit..." << std::endl;
  std::cin.get();
  return 0;
}