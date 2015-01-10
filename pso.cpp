#include <random>

#include <stdio.h>

main(){
   std::default_random_engine gen;
   std::uniform_real_distribution<double> distribution(0.0,1.0);
   for(;;){
      printf("%f\r\n",distribution(gen));
   }

}

