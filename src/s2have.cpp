#include <iostream>
#include <vector>
#include <unordered_map>
#include <iterator>
#include <algorithm>
#include <string>
#include <Rcpp.h>
using namespace Rcpp;

// [[Rcpp::plugins("cpp11")]]

std::string rcpp_type(RObject x){
  if(is<NumericVector>(x)){
    if(Rf_isMatrix(x)){
      return "NumericMatrix";
    }else{
      return "NumericVector";
    }
  }
  else if(is<IntegerVector>(x)){
    if(Rf_isFactor(x)){
      return "factor";
    }else{
      return "IntegerVector";
    }
  }
  else if(is<CharacterVector>(x)){
    return "CharacterVector";
  }
  else if(is<LogicalVector>(x)){
    return "LogicalVector";
  }
  else if(is<DataFrame>(x)){
    return "DataFrame";
  }
  else if(is<List>(x)){
    return "List";
  }
  else if(x.isS4()){
    return "S4";
  }
  else if(x.isNULL()){
    return "NULL";
  }
  else{
    return "unknown";
  }
}

void print_dataframe(const DataFrame& input){
  int row = input.nrows();
  int col = input.length();
  Rcout << "print_dataframe ==================" << "\n";
  
  for (int i = 0; i  < row; i++){
    for(int j = 0; j < col; j++){
      std::string col_type = rcpp_type(input[j]);
      
      if(col_type == "NumericVector"){
        NumericVector temp = input[j];
        Rcout << temp[i] << "\t";
      }else if(col_type == "CharacterVector"){
        CharacterVector temp = input[j];
        Rcout << temp[i] << "\t";
      }else{
        Rcout << "err" << "\t";
      }
      if(j == col-1){
        Rcout << "\n";
      }
    }
  }
  Rcout << "\n" << "\n";
}

void print_vector(const std::vector<int>& input){
  Rcout <<"print_vector ==================" << "\n";
  for(auto itr = input.begin(); itr != input.end(); itr++){
      Rcout << *itr << "\t";
    }
    Rcout << "\n" << "\n";
}

void print_vector2d(const std::vector<std::vector<int>>& input){
  Rcout << "print_vector2d ==================" << "\n";
  for(auto itr = input.begin(); itr != input.end(); itr++){
    for(auto itr2 = itr->begin(); itr2 != itr->end(); itr2++){
      Rcout << *itr2 << "\t";
    }
    Rcout << "\n" << "\n";
  }
}

void print_map(const std::unordered_map<int,NumericVector>& map){
  Rcout << "print_map ==================" << "\n";
  for(auto itr=map.begin(); itr!=map.end(); itr++) {
    Rcout << "key: " << itr->first << ":\t";      // キーを表示
    Rcout << itr->second << "\n" << "\n";
  }
  Rcout << "\n" << "\n";
}

void print_map(const std::unordered_map<std::string,NumericVector>& map){
  Rcout << "print_map ==================" << "\n";
  for(auto itr=map.begin(); itr!=map.end(); itr++) {
    Rcout << "key: " << itr->first << ":\t";      // キーを表示
    Rcout << itr->second << "\n" << "\n";
  }
  Rcout << "\n" << "\n";
}

void print_map(const std::unordered_map<std::string,std::vector<int>>& map){
  Rcout << "print_map ==================" << "\n";
  for(auto itr=map.begin(); itr!=map.end(); itr++) {
    Rcout << "key: " << itr->first << ":\t";      // キーを表示
    
    for(int i = 0; i < itr->second.size();i++){
      Rcout << itr->second[i] << "\t";
    }
    Rcout << "\n";
  }
  Rcout << "\n" << "\n";
}


std::vector<int> summarise_vector(std::vector<std::vector<int>> vector2d){

  const std::size_t sz = vector2d[0].size();
  std::vector<int> output(sz);
  
  // Rcout << "sz" << sz << "\n";
  for(auto itr=vector2d.begin(); itr != vector2d.end(); itr++){
    std::vector<int> temp = *itr;
    for(int i = 0; i < sz; i++){
      output[i] += temp[i];
    }
  }
    // print_vector(output);
    return output;
  }




std::vector<int> cpp_s2loop(const int feder,const int s2,const DataFrame& input_fed,
                            std::unordered_map<std::string, std::vector<int>>& memo,int counter){
  
  if(counter > 10000){
    Rcout << "Error log: Feder = " << feder << "\n";
    Rcout << "         : " << "\n";
    //print_dataframe(input_fed);
    //Rcout << "\n\n";
    return std::vector<int> {0,0,0};
    stop("Error: Infinite loop has occurred.");
    }
  
  // s1_col　| s2_col | data_col...
  // ______________________________
  //  A　  　|  s2    | data_origin
  //  s2     |  B     | data[1]
  //  B      |  0     | data[2]
  //  B      |  C     | data[3]
  //  C      |  0     | data[4]
  
  const NumericVector s1_col = input_fed[0];
  const NumericVector s2_col = input_fed[1];
  
  // data_origin in s2
  const LogicalVector bool_s2 = (s2_col == s2);
  // s2 in s1_col
  const LogicalVector bool_s1 = (s1_col == s2);
  // zero in s2_col
  const LogicalVector bool_0 = (s2_col == 0);
  // next_s2 == zero
  const LogicalVector bool_s2_0 = bool_s1 & bool_0;
  // data_origin or s2 == 0
  const LogicalVector bool_s2end = bool_s2|bool_s2_0;
  // next_s2 not zero
  const LogicalVector bool_s2next = bool_s1 & !(bool_0);


  // summarize data_origin or s2==0
  std::vector<int> output;
  
  const int input_colnum = input_fed.length() -2;
  
  for(int i = 0; i < input_colnum ; i++){
    
    NumericVector col_tmp = input_fed[i+2];
    col_tmp = col_tmp[bool_s2end];
    int s = sum(col_tmp);
    output.push_back(s);
    
  }

  // get next_s2 in s2_col
  const NumericVector next_s2 = s2_col[bool_s2next];
  const NumericVector s2_unique = unique(next_s2);
  const std::string id = std::to_string(feder) + "_" + std::to_string(s2);
  
  if(s2_unique.size() == 0){
    
    // const std::vector<int> output (input_column,0);
    memo[id] = output;
    
    return output;
    
  }else{
  
  
  // Analyze s2 in order
  std::vector<std::vector<int>> store; 
  store.push_back(output);
  
  for(int i = 0; i < s2_unique.size(); i++){
    const int s = s2_unique[i];
    const std::string id_next = std::to_string(feder) + "_" + std::to_string(s); 
    
    // If s2 exists in the memo, store the value of the memo in s2
    if(memo.count(id_next) > 0){
    
      store.push_back(memo[id_next]);
      
    }else{
      counter ++;
      std::vector<int> output_temp = cpp_s2loop(feder,s,input_fed,memo,counter);
      store.push_back(output_temp);
    }
  }
    // print_vector2d(store);
    return summarise_vector(store);
  }
}



DataFrame map_to_dataframe(std::unordered_map<std::string,std::vector<int>>& map){
    
    const int row_count = map.size();
    auto itr_tmp = map.begin();
    const int col_count = itr_tmp->second.size();
    
    // Rcout << "mapsize:" << row_count << "\t" << col_count << "\n";
    std::vector<std::vector<int>> col_num(row_count,std::vector<int>(col_count));
    
    CharacterVector col_id;
    DataFrame output;
    
    int i = 0;
    
    for(auto itr = map.begin(); itr != map.end(); itr++) {

      col_id.push_back(itr->first);

      for(int j = 0; j < itr->second.size(); j++){
        // Rcout << itr->second[j] << "\t";
        col_num.at(i).at(j)=(itr->second[j]);
      }
      // Rcout << "\n";
      i++;
    }
    
    // print_vector2d(col_num);
    
    output = DataFrame::create(Named("id") = col_id);
    // print_dataframe(output);
    

    
    for(int i = 0; i < col_count; i++){
      NumericVector temp;
      
      for(int j = 0; j < row_count; j++){
        // Rcout << "a " << col_num[j][i] << "\n";
        temp.push_back(col_num[j][i]);
      }
      output.push_back(temp);

      // Rcout << "\n";
    }
    
    // print_dataframe(output);
    
    return output;
}



// [[Rcpp::export]]
DataFrame cpp_s2has(const DataFrame input)
{
    // print_dataframe(input);
    const NumericVector fed_1 = input[0]; // fed_1
    const NumericVector fed_2 = input[1]; // fed_2
    const NumericVector s1 = input[2];      // s1
    const NumericVector s2 = input[3];      // s2

    NumericVector fed_list_temp = unique(fed_1); // unique fed_1 list
    fed_list_temp.sort();
    const NumericVector fed_list = fed_list_temp[fed_list_temp > 0]; // remove 0 or -1

    std::unordered_map<std::string, std::vector<int>> memo;
    
    //Processing per fed
    for(int i = 0; i < fed_list.size();i++){
      //Rcout << "processing: " << i << " / " << fed_list.size() << "\n";  
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

      // add columns
      for(int j = 4; j < input_col; j++){
        NumericVector temp = input[j];
        temp = temp[bool3];
        map_r[j] = clone(temp);
        input_sub.push_back(map_r[j]);
        }

      // unique s2_sub and remove 0 or -1
      NumericVector s2_tmp = unique(s2_sub).sort();
      const NumericVector s2_unique = s2_tmp[s2_tmp > 0];

      for(int k = 0; k < s2_unique.size(); k++){
        const int feder_tmp = fed_list[i];
        const int s2_tmp = s2_unique[k];
        
        std::string id = std::to_string(feder_tmp) + "_" + std::to_string(s2_tmp);
        if(memo.count(id) > 0){
          
          // if memo already has data, skip
          continue;
          
        }else{
          
          std::vector<int> output = cpp_s2loop(fed_list[i],s2_unique[k],input_sub,memo,0);
          memo[id] = output;
        }
    }
    }
    // Rcout << "memo" << "__________________________________" << "\n";
    // print_map(memo);
    // Convert memo to data frame
    DataFrame data = map_to_dataframe(memo);
    // Rcout << "data" << "__________________________________" << "\n";
    // print_dataframe(data);
    
    return data;
}

/*** R
set.seed(100)
library(tictoc)
library(tidyverse)
# mat <- data.frame(1:3,2:4,3:5,4:6,5:7,6:8,101:103)

# loop
# mat <- data.frame(rep(1,3),rep(1,3),1:3,c(2,3,4),5:7,6:8,101:103)

mat <- read_csv("../dummy2.csv")
for(i in 1:10){
mat2 <- mat %>% mutate(fed1 = fed1+19,fed2=fed2+19)
mat <- rbind(mat,mat2)

}

# d <- 4000000
# mat <- data.frame(vapply(runif(d, min = 1, max = 9000),round,1),
#                   vapply(runif(d, min = 1, max = 9000),round,1),
#                   vapply(runif(d, min = 0, max = 1500000),round,1),
#                   vapply(runif(d, min = 0, max = 1500000),round,1),
#                   vapply(runif(d, min = 0, max = 1),round,1),
#                   vapply(runif(d, min = 0, max = 1),round,1),
#                   vapply(runif(d, min = 0, max = 1),round,1))
colnames(mat) <- c("fed1","fed2","s1","s2","data1","data2","data3","data4")
# 
# filter_df <- data.frame(fed1 = c(4482,5867,6357,7318,7670))
# mat <- mat %>% distinct(s2,.keep_all = TRUE) %>% anti_join(filter_df,by="fed1")

output <- cpp_s2has(mat)
# microbenchmark::microbenchmark(
# k <- cpp_s2has(mat),times=5
# )
*/
