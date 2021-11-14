#include <iostream>
#include <vector>
#include <unordered_map>
#include <iterator>
#include <algorithm>
#include <string>
#include <Rcpp.h>
using namespace Rcpp;

// [[Rcpp::plugins("cpp11")]]

std::vector<int> cpp_s2loop(const DataFrame& input_fed){


}

// [[Rcpp::export]]
DataFrame cpp_s2have(const DataFrame input)
{
    const NumericVector fed_1 = input[0]; // fed_1
    const NumericVector fed_2 = input[1]; // fed_2
    const NumericVector s1 = input[2];      // s1
    const NumericVector s2 = input[3];      // s2

    NumericVector fed_list_temp = unique(fed_1); // unique fed_1 list
    fed_list_temp.sort();
    const NumericVector fed_list = fed_list_temp[fed_list_temp > 0]; // remove 0 or -1

    std::unordered_map<int, NumericVector> memo;

    for(int i = 0; i < fed_list.size();i++){

      // search fed_list[i] index
      const LogicalVector bool1 = (fed_1 == fed_list[i]);
      const LogicalVector bool2 = (fed_2 == fed_list[i]);
      const LogicalVector bool3 = bool1 | bool2;

      // s1 and s2 vector of fed_list[i]
      const NumericVector s1_fed = s1[bool3];
      const NumericVector s2_fed = s2[bool3];

// subset input DataFrame
const int input_row = input.nrow();
Rcout << "s1_fed" << s1_fed << "\n";
Rcout << "s2_fed" << s2_fed << "\n";

NumericVector s1_fedtmp = clone(s1_fed);
NumericVector s2_fedtmp = clone(s2_fed);
DataFrame input_sub = DataFrame::create(Named("s1") = s1_fedtmp,
                                        Named("s2") = s2_fedtmp);

for(int j = 4; j < input_row; j++){
  NumericVector input_temp = input[j];
  input_temp = input_temp[bool3];
  input_sub.push_back(input_temp);
}

      // unique s2_fed and remove 0 or -1
      NumericVector s2_fed_list_temp = unique(s2_fed).sort();
      const NumericVector s2_fed_list = s2_fed_list_temp[s2_fed_list_temp > 0];

      for(int k = 0; k < s2_fed_list.size(); k++){


      }
    }

  NumericVector te {0,0,2,0,321,100,200,300,400};
  NumericVector te2 {0,1,2,3,4,5,6,7,0};

  LogicalVector ra1;
  LogicalVector ra2;
  LogicalVector ra3;

  ra1 = (te == 0);
  ra2 = (te2 == 0);
  ra3 = (ra1 | ra2);

  Rcout << "ra1:" << ra1 << "\n";
  Rcout << "ra2:" << ra2 << "\n";
  Rcout << "ra3:" << ra3 << "\n";

  LogicalVector v1 = {1,1,0,0};
  LogicalVector v2 = {1,0,1,0};

  LogicalVector res1 = v1 & v2;
  LogicalVector res2 = v1 | v2;
  LogicalVector res3 = !(v1 | v2);

  Rcout << res1 << "\n"; // 1 0 0 0
  Rcout << res2 << "\n"; // 1 1 1 0
  Rcout << res3 << "\n"; // 0 0 0 1

    NumericVector x {100,200,300,400};
    NumericVector y {2000,2000,3000,4000};
    NumericVector z {30,20,50,40};

    memo[1] = x;
    memo[2] = y;
    memo[3] = z;

   DataFrame temp = {};
   for(auto itr = memo.begin(); itr != memo.end(); ++itr) {
     NumericVector t = clone(itr->second);
     t.push_front(itr->first);
     temp.push_back(t);
    }
    List zz = List::create(temp);

    return zz;

}

/*** R
mat <- data.frame(1:3,2:4,3:5,4:6)
colnames(mat) <- c("fed1","fed2","s1","s2")
k <- cpp_s2have(mat)
k
*/

