#define IKFAST_HAS_LIBRARY
#define IKFAST_NAMESPACE
#include <stdio.h>
#include <string.h>

extern const char *GetIkFastVersion();
extern int GetIkRealSize();
extern void ComputeFk(int argc, char **argv);

int main(int argc, char *argv[]) {

  const char *version = GetIkFastVersion();
  int real = GetIkRealSize();

  printf("IkFaster version: %s,%d\n", version, real);

  printf("argc:%d,argv:%s,%s\n", argc, argv[0], argv[1]);
  // if (argc > 2 && strcmp(argv[1], "fk")) {
  // if (argc > 2) {
  //   printf("argv:%s,%s\n", argv[0], argv[1]);
  //   ComputeFk(argc, argv);
  // }

  return 0;
}
