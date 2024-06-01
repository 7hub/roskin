#include "ik.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char **argv) {
    
    IkReal coeff[3]={2,4,8};
    IkReal roots[2];
    int *numroots=0;

    polyroots2(coeff,roots,numroots);
    
    
    return 0;
}