// #define IKFAST_HAS_LIBRARY
// #include "../include/ikfast.h"
#include <stdio.h>

#include "ikfast.h"

const char *GetIkFastVersion();
int GetIkType();

int main(int argc, char *argv[]) {
  const char *version = GetIkFastVersion();

  printf("IkFaster version: %s\n", version);

  // ikfast::IkFastFunctions<IkFastReal>::GetIkFastVersion();

  printf("sotest verstion: %s\n", GetIkFastVersion());

  return 0;
}
