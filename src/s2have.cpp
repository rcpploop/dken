#include <iostream>
#include <vector>
#include <unordered_map>
#include <iterator>
#include <algorithm>
#include <string>
#include <Rcpp.h>
using namespace Rcpp;

// [[Rcpp::plugins("cpp11")]]

std::vector<int> cpp_s2loop(const int s2,const DataFrame& input_fed,
                            std::unordered_map<int, std::vector<int>>& memo){
  std::vector<int> x = {1,2,3};
  return x;
  
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

    std::unordered_map<int, std::vector<int>> memo;

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
      NumericVector s2_fedlist_temp = unique(s2_fed).sort();
      const NumericVector s2_fedlist = s2_fedlist_temp[s2_fedlist_temp > 0];
      
      Rcout << "aa " << s2_fedlist << "\n";

      for(int k = 0; k < s2_fedlist.size(); k++){
        if(memo.count(s2_fedlist[k]) > 0){
          Rcout << "k" << k << " " << memo.count(s2_fedlist[k]);
          continue;
        }else{
          Rcout << "k " << k << " " << memo.count(s2_fedlist[k]);
          Rcout << "fed" << s2_fedlist[k] << "\n";
          std::vector<int> output = cpp_s2loop(s2_fedlist[k],input_sub,memo);
          memo[s2_fedlist[k]] = output;
        }
    }
      
      NumericVector ra;
      Rcout << "memo[1].size:" << memo[4].size() << "\n";
      
      for(auto itr=memo.begin(); itr!=memo.end(); itr++) {
        ra.push_back(itr->first);      // キーを表示
        for(int i=0;i < itr->second.size();i++)
        ra.push_back(itr->second[i]);
      }
    // memo[1] = x;
    // memo[2] = y;
    // memo[3] = z;

   // DataFrame temp = {};
   // for(auto itr = memo.begin(); itr != memo.end(); ++itr) {
   //    t = clone(itr->second);
   //   t.push_front(itr->first);
   //   temp.push_back(t);
   //  }
    }
    List zz = List::create("ra");

    return zz;

  
}

/*** R
mat <- data.frame(1:3,2:4,3:5,4:6)
colnames(mat) <- c("fed1","fed2","s1","s2")
k <- cpp_s2have(mat)
k
*/

