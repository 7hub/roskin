#define IKFAST_HAS_LIBRARY
#define IKFAST_API extern 
#define IKFAST_NAMESPACE
#include <stdio.h>
#include <string.h>


/// \brief returns the number of free parameters users has to set apriori
IKFAST_API int GetNumFreeParameters();

/// \brief the indices of the free parameters indexed by the chain joints
IKFAST_API int* GetFreeParameters();

/// \brief the total number of indices of the chain
IKFAST_API int GetNumJoints();

/// \brief the size in bytes of the configured number type
IKFAST_API int GetIkRealSize();

/// \brief the ikfast version used to generate this file
IKFAST_API const char* GetIkFastVersion();

/// \brief the ik type ID
IKFAST_API int GetIkType();

/// \brief a hash of all the chain values used for double checking that the correct IK is used.
IKFAST_API const char* GetKinematicsHash();

IKFAST_API void ComputeFkC(int argc, char** argv);

IKFAST_API int ComputeIkC(int argc, char** argv);

int main(int argc, char *argv[]) {

  const char *version = GetIkFastVersion();
  int real = GetIkRealSize();

  printf("IkFaster version: %s,%d\n", version, real);

  printf("argc:%d,argv:%s,%s\n", argc, argv[0], argv[1]);
  // if (argc > 2 && strcmp(argv[1], "fk")) {
  if (argc > 2 && !strcmp(argv[1],"fk")) {
    printf("%d\n",GetIkType());
    ComputeFkC(argc, argv);
    printf("argv:%s,%s\n", argv[0], argv[1]);
  }

  if (argc>12 ) {
    printf("argv:%s,%s\n", argv[0], argv[1]);
    int ret= ComputeIkC(argc,argv);
    return ret;
  }


  return 0;
}
