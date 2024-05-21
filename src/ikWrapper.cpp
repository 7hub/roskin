#include "ikWrapper.h"
#include "../include/ikfast.h"

extern const char* GetIkVersion();
extern int GetIkRealSize();

//extern "C" {
//
//
//}


extern "C" {


const char *IKSolver_GetIkVersion() { return GetIkVersion(); }

int IKSolver_GetIkRealSize() { return GetIkRealSize(); }
}
