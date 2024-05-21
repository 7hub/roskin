#ifndef IKWRAPPER_H
#define IKWRAPPER_H
#include "ikWrapper.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// typedef struct IKSolver IKSolver;
// typedef double IkReal;
// typedef struct ikfast::IkSolutionListBase<IkReal> IkSolutionListBase;

// IKSolver *CreateIKSolver();
// bool IKSolver_ComputeIk(IKSolver v, const IkReal *eetrans, const IkReal *eerot,
//                         const IkReal *pfree, IkSolutionListBase &solutions);

const char *IKSolver_GetIkVersion();

int IKSolver_GetIkRealSize();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
