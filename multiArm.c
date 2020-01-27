#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <map>
#include <fstream>
#include <iostream>

#include <sml_shift_register.h>

// the game and reward values

double arm_0 [30] = {9.0/28.0, 5.0/28.0, 8.0/28.0, 8.0/28.0, 6.0/28.0,
                     5.0/28.0, 4.0/28.0, 6.0/28.0, 7.0/28.0, 6.0/28.0,
		     8.0/28.0, 4.0/28.0, 5.0/28.0, 7.0/28.0, 5.0/28.0,
		     8.0/28.0, 7.0/28.0, 5.0/28.0, 4.0/28.0, 3.0/28.0,
		     5.0/28.0, 7.0/28.0, 8.0/28.0 ,5.0/28.0, 3.0/28.0,
		     6.0/28.0, 7.0/28.0, 5.0/28.0, 7.0/28.0, 5.0/28.0};

double arm_1 [30] = { 2.0/28.0, 3.0/28.0, 20.0/28.0, 2.0/28.0, 0.0/28.0,
                     23.0/28.0, 3.0/28.0, 14.0/28.0, 4.0/28.0, 3.0/28.0,
		     15.0/28.0, 26.0/28.0, 3.0/28.0, 2.0/28.0, 3.0/28.0,
		     28.0/28.0, 11.0/28.0, 5.0/28.0, 9.0/28.0, 2.0/28.0,
		     3.0/28.0, 21.0/28.0, 0.0/28.0, 5.0/28.0, 3.0/28.0,
		     1.0/28.0, 12.0/28.0, 11.0/28.0, 6.0/28.0, 3.0/28.0};

double arm_2 [30] = {0.0/28.0, 1.0/28.0, 3.0/28.0, 2.0/28.0, 1.0/28.0,
                     4.0/28.0, 2.0/28.0, 1.0/28.0, 2.0/28.0, 0.0/28.0,
		     3.0/28.0, 2.0/28.0, 3.0/28.0, 4.0/28.0, 1.0/28.0,
		     2.0/28.0, 3.0/28.0, 4.0/28.0, 3.0/28.0, 0.0/28.0,
		     4.0/28.0, 3.0/28.0, 2.0/28.0, 3.0/28.0, 1.0/28.0,
		     3.0/28.0, 4.0/28.0, 0.0/28.0, 4.0/28.0, 1.0/28.0};
// -------------------------------------------------------------------
template<class random_type>
double substitute(const unsigned int index,random_type& ran,
                  double& L0,double& L1,double& L2,
		  double& P0,double& P1,double& P2)
{
    double score;
    const double eta = std::sqrt(2*std::log(3.0)/(90.0));
    // loss
    double ltd_0;
    double ltd_1;
    double ltd_2;

    double e = ran()*1.0/ran.max();
//    double e = rand()*1.0/RAND_MAX;

    if (e < P0)
    {
     //  printf("arm 0 was chosen\n");
       ltd_0 = (1.0 - arm_0[index])/P0;
       ltd_1 = 0;
       ltd_2 = 0;
       score = arm_0[index];
    }
    else if  (e < P0 + P1)
    {
       // printf("arm 1 was chosen\n");
        ltd_0 = 0;
        ltd_1 = (1.0 - arm_1[index])/P1;
        ltd_2 = 0;
        score = arm_1[index];
    }
    else
    {
      //  printf("arm 2 was chosen\n");
        ltd_0 = 0;
        ltd_1 = 0;
        ltd_2 = (1.0 - arm_2[index])/P2;
        score = arm_2[index];
    }
    // update cumulative loss
    L0 = L0 + ltd_0;
    L1 = L1 + ltd_1;
    L2 = L2 + ltd_2;

    // update probabilities
    P0 = exp(-eta*L0) / (exp(-eta*L0)+ exp(-eta*L1)+ exp(-eta*L2));
    P1 = exp(-eta*L1) / (exp(-eta*L0)+ exp(-eta*L1)+ exp(-eta*L2));
    P2 = 1 - P0 - P1;

    return score;
}
// -------------------------------------------------
template<class random_type>
double exp3algo(random_type& ran) {

  // cumulative loss
  double L0=0;
  double L1=0;
  double L2=0;

  double P0 = 1.0/3.0;
  double P1 = 1.0/3.0;
  double P2 = 1.0 - P0 - P1;

  double total = 0.0;

  for(unsigned int index=0;index<30;index++)
  {
    total += substitute(index,ran,L0,L1,L2,P0,P1,P2);
  }
  return total;
}
// -------------------------------------------------
template<class histogram_type>
void accumulate(histogram_type& histogram,double true_score) {

  unsigned int int_score = static_cast<unsigned int>(true_score);

  if(histogram.find(int_score) == histogram.end()) {
  // no such entry, so we make one new one
    histogram.insert(std::make_pair(int_score,1));
  }
  else {
    histogram[int_score]++;
  }
}
// -------------------------------------------------

int main(){

  sml::shift_register_default  ran(177);

  std::map<unsigned int, unsigned int>  histogram;

  double score;
  double total=0;
  double true_score=0;
  for(unsigned int k=0;k<10;++k) {
  for(unsigned int repeat=0; repeat<50000; repeat++){
      true_score = 28*exp3algo(ran)+1e-10;
      accumulate(histogram,true_score);
  } // for repeat
  } // for k

  // print the histogram into a file
  std::ofstream hist_file("histogram.dat");
  std::for_each(histogram.begin(),histogram.end(),
                [&hist_file](std::pair<unsigned int,unsigned int> v) {
		  hist_file<<v.first<<" "<<v.second<<std::endl;
		});
  hist_file.close();

  return 0;
}
