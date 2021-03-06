#include <Rcpp.h>
using namespace Rcpp;

//' Simulate trading data
//'
//' Simulates a matrix consisting of synthetic data for daily buys and sells
//'
//' If names are not set for \code{param} or one or more of the vector names do not match the valid choices, they are internally set to
//' \code{'alpha'}, \code{'delta'}, \code{'epsilon_b'}, \code{'epsilon_s'}, \code{'mu'} (in this order).
//'
//' @return \emph{numeric}: Matrix with \code{ndays} rows and two columns which are named \code{'Buys'} and \code{'Sells'}.
//'
//' @inheritParams pin_ll
//' @param ndays \emph{integer}: Number of trading days for which aggregated buys and sells are simulated, defaults to 60
//'
//' @export simulateBS
// [[Rcpp::export]]

NumericMatrix simulateBS(NumericVector param, int ndays) {
  NumericMatrix res(ndays,2);
  IntegerVector states_ind = IntegerVector::create(0,1,2);
  NumericVector state_probs(3);
  state_probs[0] = 1.0 - param[0];
  state_probs[1] = param[0] * (1.0 - param[1]);
  state_probs[2] = param[0] * param[1];

  NumericVector buys(ndays);
  NumericVector sells(ndays);

  IntegerVector states = sample(states_ind, ndays, true, state_probs);

// indices for no-, good- and bad-news days
  LogicalVector ind_no = states == 0;
  LogicalVector ind_good = states == 1;
  LogicalVector ind_bad = states == 2;

  int len_no = sum(ind_no);
  int len_good = sum(ind_good);
  int len_bad = sum(ind_bad);

// drawing Poisson distributed random numbers for daily buys and sells
// according to the actual buy and sell intensities depending on the
// condition of the trading day
  buys[ind_no] = rpois(len_no, param[2]);
  sells[ind_no] = rpois(len_no, param[3]);

  buys[ind_good] = rpois(len_good, param[2] + param[4]);
  sells[ind_good] = rpois(len_good, param[3]);

  buys[ind_bad] = rpois(len_bad, param[2]);
  sells[ind_bad] = rpois(len_bad, param[3] + param[4]);

  res(_,0) = buys;
  res(_,1) = sells;

  colnames(res) = CharacterVector::create("Buys", "Sells");

  return(res);
}
