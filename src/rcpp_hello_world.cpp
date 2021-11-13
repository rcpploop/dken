#include <iostream>
#include <vector>
#include <unordered_map>
#include <iterator>
#include <algorithm>
#include <string>

#include <Rcpp.h>
using namespace Rcpp;

// [[Rcpp::plugins("cpp11")]]

// [[Rcpp::export]]
List s2have(const DataFrame input)
{
    int input_row = input.nrows();

    IntegerVector feder_1 = input[0]; // feder_1
    IntegerVector feder_2 = input[1]; // feder_2
    IntegerVector s1 = input[2];      // s1
    IntegerVector s2 = input[3];      // s2

    std::unordered_map<long int, std::vector<int>> memo;

    CharacterVector x = CharacterVector::create("foo", "bar");
    NumericVector y = NumericVector::create(0.0, 1.0);
    List z = List::create(x, y);

    return z;
}