library(dken)

test_that("check data.frame",{
  expect_error(s2have(1),"`input file is not a data.frame`")
})

test_that("check numeric vector",{
  string_matrix <- data.frame("a","b","c")
  string_and_numeric <- data.frame(1:3,c(1.2,120302,23),c("a","1","c"))
  expect_error(s2have(string_matrix),"Error:input file is not data.frame")
})