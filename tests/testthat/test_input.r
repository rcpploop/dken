#context("Input file test")
library(dken)

test_that("basic",{
  expect_equal(10,10)
})

test_that("basic2",{
  expect_equal(test2(),1)
})