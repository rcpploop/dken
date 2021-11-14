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
List cpp_s2have(const DataFrame input)
{
    int i_row = input.nrows();

    NumericVector fed_1 = input[0]; // fed_1
    NumericVector fed_2 = input[1]; // fed_2
    NumericVector s1 = input[2];      // s1
    NumericVector s2 = input[3];      // s2

    std::unordered_map<int, NumericVector> memo;

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

