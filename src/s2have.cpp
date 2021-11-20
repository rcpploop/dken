#include <iostream>
#include <vector>
#include <unordered_map>
#include <iterator>
#include <algorithm>
#include <string>
#include <Rcpp.h>
using namespace Rcpp;

// [[Rcpp::plugins("cpp11")]]

void print_dataframe(const DataFrame& input){
  int row = input.nrows();
  int col = input.length();
  
  for (int i = 0; i  < row; i++){
    for(int j = 0; j < col; j++){
      NumericVector temp = input[j];
      Rcout << temp[i] << "\t";
      
      if(j == col-1){
        Rcout << "\n";
      }
    }
  }
  Rcout << "\n";
}

void print_map(const std::unordered_map<int,NumericVector>& map){
  for(auto itr=map.begin(); itr!=map.end(); itr++) {
    Rcout << "key: " << itr->first << ":\t";      // キーを表示
    Rcout << itr->second << "\n";
  }
}


std::vector<int> cpp_s2loop(const int s2,const DataFrame& input_fed,
                            std::unordered_map<int, std::vector<int>>& memo){
  
  // s1_col　| s2_col | data_col...
  // ______________________________
  //  A　  　|  s2    | data_origin
  //  s2     |  B     | data[1]
  //  B      |  0     | data[2]
  //  B      |  C     | data[3]
  //  C      |  0     | data[4]
  
  const NumericVector s1_col = input_fed[0];
  const NumericVector s2_col = input_fed[1];
  const int input_col = input_fed.length();
  
  // summarize data_origin
  const LogicalVector bool_s2 = (s2_col == s2);
  std::vector<int> output;
  
  for(int i = 0; i < input_col-2 ; i++){
    
    NumericVector input_col = input_fed[i+2];
    input_col = input_col[bool_s2];
    int s = sum(input_col);
    output.push_back(s);
    
  }
  
  // find s2 in s1_col
  const LogicalVector bool_s1 = (s1_col == s2);
  
  NumericVector s2_tmp = s2_col[bools];
  const NumericVector s2_unique = unique(s2_tmp);
  
  if(s2_unique.size() == 0){
    const int input_row = input_fed.length() -2;
    
    const std::vector<int> output (input_row,0);
    memo[s2] = output;
    
    return output;
    
  }else{
  
  std::vector<std::vector<int>> store; 
    
  for(int i = 0; i < s2_unique.size(); i++){
    
    if(memo.count(s2_unique[i]) > 0){
      
      int s = s2_unique[i];
      store.push_back(memo[s]);
      
    }else{
      // std::vector<int> output_temp = cpp_s2loop();
    }
  }
  }
  std::vector<int> x = {1,2,3};
  return x;
  
}



// [[Rcpp::export]]
DataFrame cpp_s2have(const DataFrame input)
{
  // print_dataframe(input);
    const NumericVector fed_1 = input[0]; // fed_1
    const NumericVector fed_2 = input[1]; // fed_2
    const NumericVector s1 = input[2];      // s1
    const NumericVector s2 = input[3];      // s2

    NumericVector fed_list_temp = unique(fed_1); // unique fed_1 list
    fed_list_temp.sort();
    const NumericVector fed_list = fed_list_temp[fed_list_temp > 0]; // remove 0 or -1

    std::unordered_map<int, std::vector<int>> memo;
    
    //Processing per fed
    for(int i = 0; i < fed_list.size();i++){
      // search fed_list[i] index
      const LogicalVector bool1 = (fed_1 == fed_list[i]);
      const LogicalVector bool2 = (fed_2 == fed_list[i]);
      const LogicalVector bool3 = bool1 | bool2;

      // s1 and s2 vector of fed_list[i]
      const NumericVector s1_sub = s1[bool3];
      const NumericVector s2_sub = s2[bool3];

      // subset input DataFrame
      DataFrame input_sub = DataFrame::create(Named("s1") = s1_sub,
                                              Named("s2") = s2_sub);
      
      std::unordered_map<int, NumericVector> map_r;
      const int input_col = input.length();
      // Rcout << "icol" << input_col << "\n";
      
      // add columns
      for(int j = 4; j < input_col; j++){
        NumericVector temp = input[j];
        temp = temp[bool3];
        map_r[j] = clone(temp);
        input_sub.push_back(map_r[j]);
        }
      
      // print_map(map_r);
      // Rcout << "Feder: " << fed_list[i] << "\n"; 
      // print_dataframe(input_sub);

      // unique s2_sub and remove 0 or -1
      NumericVector s2_tmp = unique(s2_sub).sort();
      const NumericVector s2_unique = s2_tmp[s2_tmp > 0];

      for(int k = 0; k < s2_unique.size(); k++){
        if(memo.count(s2_unique[k]) > 0){
          
          continue;
          
        }else{
          
          std::vector<int> output = cpp_s2loop(s2_unique[k],input_sub,memo);
          memo[s2_unique[k]] = output;
          
        }
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
set.seed(100)
library(tictoc)
#mat <- data.frame(1:4,2:5,3:6,4:7,5:8,105:108)
d <- 100
mat <- data.frame(vapply(runif(d, min = 0, max = 20),round,1),
                  vapply(runif(d, min = 0, max = 20),round,1),
                  vapply(runif(d, min = 0, max = 200),round,1),
                  vapply(runif(d, min = 0, max = 200),round,1),
                  vapply(runif(d, min = 0, max = 200),round,1),
                  vapply(runif(d, min = 0, max = 200),round,1),
                  vapply(runif(d, min = 0, max = 200),round,1))
colnames(mat) <- c("fed1","fed2","s1","s2","data1","data2","data3")
tic()
k <- cpp_s2have(mat)
toc()
#k
*/

