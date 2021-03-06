library(dken)

test_that("check data.frame",{
  expect_error(s2have(1),"`input file is not a data.frame`")
})

test_that("check numeric vector",{
  string_matrix <- data.frame("a","b","c")
  string_and_numeric <- data.frame(1:3,c(1.2,120302,23),c("a","1","c"))
  
  expect_error(s2have(string_matrix),"`input file is not a numeric data.frame`")
  expect_error(s2have(string_and_numeric),"`input file is not a numeric data.frame`")
})

test_that("aa",{
  mat <- data.frame(1:3,2:4,3:5,4:6)
  colnames(mat) <- c("fed1","fed2","s1","s2")
  k <- s2have(mat)
  k
  
})