library(purrr)
library(magrittr)
library(dplyr)

s2have <- function(input){
  if(!is.data.frame(input)){
    stop("`input file is not a data.frame`", .call = FALSE)
  }
  
  not_num <- input %>% map(is.numeric) %>% has_element(FALSE)
  if(not_num){
    stop("`input file is not a numeric data.frame`",.call = FALSE)
  }
  
  cl <- colnames(input)
  check_col <- cl[1:4] != c("fed1","fed2","s1","s2")
  if(check_col){
    stop("`input col must be fed1,fed2,s1,s2...`")
  }
  
  input <- input %>% arrange(s1)
  
  cpp_s2has(input)
}
