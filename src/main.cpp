// #define IKFAST_HAS_LIBRARY
// #include "../include/ikfast.h"
// #include "ikfast.h"
#include <stdio.h>

const char* GetIkFastVersion();
int GetIkType();

int main(int argc, char *argv[]) {

const char* version = GetIkFastVersion();


printf("IkFaster version: %s\n",version);
  // ikfast::IkFastFunctions::GetIkFastVersion();

// printf("sotest verstion: %s\n",getVersion());

  return 0;
}
