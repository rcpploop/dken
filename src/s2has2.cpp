#include <iostream>
#include <vector>
#include <unordered_map>
#include <iterator>
#include <algorithm>
#include <string>
#include <Rcpp.h>

using namespace Rcpp;

// [[Rcpp::plugins("cpp11")]]
std::vector<int> summarise_vector2(std::vector<std::vector<int>> vector2d,const int sz){
  
  std::vector<int> output(sz);
  std::vector<int> temp;
  
  for(auto itr=vector2d.begin(); itr != vector2d.end(); itr++){
    temp = *itr;
    for(int i = 0; i < sz; i++){
      output[i] += temp[i];
    }
  }
  // print_vector(output);
  return output;
}

List map_to_dataframe2(std::unordered_map<std::string,std::vector<int>>& map){
  
  // List output2_1;
  
  const int map_size = map.size();
  auto itr_tmp = map.begin();
  const int col_count = itr_tmp->second.size();
  
  std::vector<std::string> v(map_size);
  std::vector<std::vector<int>> vv(map_size,std::vector<int>(col_count));
  int i = 0;
  
  for(auto itr = map.begin(); itr != map.end(); itr++) {
    // Rcout << "itr" << "\t" << itr->first << "\t" << i << "/" << map_size-1 << "\n";
    v[i] = itr->first;
    vv[i] = itr->second;
    i++;
  }
  CharacterVector id = wrap(v);
  DataFrame df = wrap(vv);
  List output = List::create(Named("id")=id,Named("df")=df);
  
  return output;
}


std::vector<int> cpp_s2loop2(const int feder,const int s2,const DataFrame& input_fed,
                            std::unordered_map<std::string, std::vector<int>>& memo,int counter){
  
  if(counter > 10000){
    Rcout << "Error log: Feder = " << feder << "\n";
    Rcout << "         : " << "\n";
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
  NumericVector col_tmp;
  
  for(int i = 0; i < input_colnum ; i++){
    
    col_tmp = input_fed[i+2];
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
    std::string id_next;
    
    for(int i = 0; i < s2_unique.size(); i++){
      const int s = s2_unique[i];
      id_next = std::to_string(feder) + "_" + std::to_string(s); 
      // If s2 exists in the memo, store the value of the memo in s2
      if(memo.count(id_next) > 0){
        
        store.push_back(memo[id_next]);
        
      }else{
        counter ++;
        std::vector<int> output_temp = cpp_s2loop2(feder,s,input_fed,memo,counter);
        store.push_back(output_temp);
      }
    }
    // print_vector2d(store);
    return summarise_vector2(store,input_colnum);
  }
}

// [[Rcpp::export]]
List cpp_s2has2(const DataFrame INPUT,const int FEDER)
{
  // print_dataframe(input);
  const NumericVector s1 = INPUT[0];      // s1
  const NumericVector s2 = INPUT[1];      // s2
  
  std::unordered_map<std::string, std::vector<int>> memo;
  
    // unique s2_sub and remove 0 or -1
    const NumericVector s2_unique = s2[s2 > 0];
    
    for(int k = 0; k < s2_unique.size(); k++){
      const int s2_tmp = s2_unique[k];
      std::string id = std::to_string(FEDER) + "_" + std::to_string(s2_tmp);
      if(memo.count(id) > 0){
        
        // if memo already has data, skip
        continue;
        
      }else{
        std::vector<int> output = cpp_s2loop2(FEDER,s2_tmp,INPUT,memo,0);
        memo[id] = output;
      }
    }
  
  // Rcout << "memo" << "__________________________________" << "\n";
  // print_map(memo);
  // Convert memo to data frame
  List data = map_to_dataframe2(memo);
  // Rcout << "data" << "__________________________________" << "\n";
  // print_dataframe(data);
  
  return data;
}

# /*** R
# library(tidyverse)
# library(parallel)
# 
# mat <- read.csv("../teiden.csv")
# 
# # f <- function(feder){
# #   d <- mat %>% filter(fed1 == feder | fed2 == feder)
# #   d <- d %>% select(-1,-2)
# #   cpp_s2has2(d,feder)
# # }
# 
# fed_list <- unique(mat$fed1)
# fed_list <- fed_list[fed_list > 0]
# 
# # cl <- makeCluster(2)
# # 
# # clusterEvalQ(cl, {
# #   library(tidyverse)
# #   mat <- read_csv("../teiden.csv")
# # })
# # 
# # clusterExport(cl, c('cpp_s2has2','f'))
# # 
# # microbenchmark::microbenchmark(
# #   output <- parLapply(cl,fed_list,f),times=1
# # )
# # 
# # stopCluster(cl)
# ans <- NULL
# microbenchmark::microbenchmark(
# for(i in 1:length(fed_list)){
#   print(i)
#   df <- mat %>% filter(fed1 == fed_list[i] | fed2 == fed_list[i])
#   df <- df %>% select(-1,-2)
#   output <- cpp_s2has2(df,fed_list[i])
#   o1 <- output[[1]] %>% as.data.frame()
#   o2 <- reduce(output[[2]],rbind) %>% as.data.frame()
#   o3 <- cbind(o1,o2)
#   ans <- bind_rows(ans,o3)
# },times = 1
# )
# origin2 <- ans
# origin2 <- origin2 %>% as_tibble()
# colnames(origin2) <- c(1,2,3,4,5)
# # write.csv(origin,"origin.csv",row.names = FALSE)
# identical(origin,origin2)
# */
