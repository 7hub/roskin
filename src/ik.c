#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

jypedef double IkReal;
typedef struct IkSingleDOFSolutionBase {
  int freeind;
  double foffset;
  double fmul;
  unsigned char maxsolutions;
  int indices[2];
} IkSingleDOFSolutionBase;

typedef struct IkSolutionBase {
  // Define the structure for IkSolutionBase
  // This is a placeholder, you need to define the actual structure
} IkSolutionBase;

void IkSolutionBase_GetSolution(const IkSolutionBase *solution, double *result, const double *freevalues) {
  // Implement the actual solution retrieval
  // This is a placeholder, you need to implement the actual function
}

void IkSolutionBase_GetSolution_Vector(const IkSolutionBase *solution, double *result, size_t result_size, const double *freevalues, size_t freevalues_size) {
  // Implement the actual solution retrieval for vector
  // This is a placeholder, you need to implement the actual function
}

const int *IkSolutionBase_GetFree(const IkSolutionBase *solution, size_t *size) {
  // Implement the actual retrieval of free parameters
  // This is a placeholder, you need to implement the actual function
  return NULL;
}

int IkSolutionBase_GetDOF(const IkSolutionBase *solution) {
  // Implement the actual retrieval of DOF
  // This is a placeholder, you need to implement the actual function
  return 0;
}

typedef struct IkSolutionListBase {
  IkSolutionBase **solutions;
  size_t num_solutions;
} IkSolutionListBase;

void IkSolutionListBase_Init(IkSolutionListBase *list) {
  list->solutions = NULL;
  list->num_solutions = 0;
}

void IkSolutionListBase_Destroy(IkSolutionListBase *list) {
  for (size_t i = 0; i < list->num_solutions; ++i) {
    free(list->solutions[i]);
  }
  free(list->solutions);
}

typedef struct IkSolution {
  IkSingleDOFSolutionBase *_vbasesol;
  int *_vfree;
  size_t _vbasesol_size;
  size_t _vfree_size;
} IkSolution;

void IkSolution_Init(IkSolution *solution, IkSingleDOFSolutionBase *vinfos, size_t vinfos_size, int *vfree, size_t vfree_size) {
  solution->_vbasesol = (IkSingleDOFSolutionBase *)malloc(vinfos_size * sizeof(IkSingleDOFSolutionBase));
  memcpy(solution->_vbasesol, vinfos, vinfos_size * sizeof(IkSingleDOFSolutionBase));
  solution->_vbasesol_size = vinfos_size;

  solution->_vfree = (int *)malloc(vfree_size * sizeof(int));
  memcpy(solution->_vfree, vfree, vfree_size * sizeof(int));
  solution->_vfree_size = vfree_size;
}

void IkSolution_GetSolution(const IkSolution *solution, double *sol, const double *freevalues) {
  for (size_t i = 0; i < solution->_vbasesol_size; ++i) {
    if (solution->_vbasesol[i].freeind < 0) {
      sol[i] = solution->_vbasesol[i].foffset;
    } else {
      sol[i] = freevalues[solution->_vbasesol[i].freeind] * solution->_vbasesol[i].fmul + solution->_vbasesol[i].foffset;
      if (sol[i] > 3.14159265358979) {
        sol[i] -= 6.28318530717959;
      } else if (sol[i] < -3.14159265358979) {
        sol[i] += 6.28318530717959;
      }
    }
  }
}

void IkSolution_GetSolution_Vector(const IkSolution *solution, double *sol, size_t sol_size, const double *freevalues, size_t freevalues_size) {
  if (sol_size < solution->_vbasesol_size) {
    sol = (double *)realloc(sol, solution->_vbasesol_size * sizeof(double));
  }
  IkSolution_GetSolution(solution, sol, freevalues_size > 0 ? freevalues : NULL);
}

const int *IkSolution_GetFree(const IkSolution *solution) { return solution->_vfree; }

int IkSolution_GetDOF(const IkSolution *solution) { return (int)solution->_vbasesol_size; }

void IkSolution_Validate(const IkSolution *solution) {
  for (size_t i = 0; i < solution->_vbasesol_size; ++i) {
    if (solution->_vbasesol[i].maxsolutions == (unsigned char)-1) {
      fprintf(stderr, "max solutions for joint not initialized\n");
      exit(EXIT_FAILURE);
    }
    if (solution->_vbasesol[i].maxsolutions > 0) {
      if (solution->_vbasesol[i].indices[0] >= solution->_vbasesol[i].maxsolutions) {
        fprintf(stderr, "index >= max solutions for joint\n");
        exit(EXIT_FAILURE);
      }
      if (solution->_vbasesol[i].indices[1] != (unsigned char)-1 && solution->_vbasesol[i].indices[1] >= solution->_vbasesol[i].maxsolutions) {
        fprintf(stderr, "2nd index >= max solutions for joint\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

void IkSolution_GetSolutionIndices(const IkSolution *solution, unsigned int *v, size_t *v_size) {
  *v_size = 1;
  v[0] = 0;
  for (int i = (int)solution->_vbasesol_size - 1; i >= 0; --i) {
    if (solution->_vbasesol[i].maxsolutions != (unsigned char)-1 && solution->_vbasesol[i].maxsolutions > 1) {
      for (size_t j = 0; j < *v_size; ++j) {
        v[j] *= solution->_vbasesol[i].maxsolutions;
      }
      size_t orgsize = *v_size;
      if (solution->_vbasesol[i].indices[1] != (unsigned char)-1) {
        for (size_t j = 0; j < orgsize; ++j) {
          v[*v_size] = v[j] + solution->_vbasesol[i].indices[1];
          (*v_size)++;
        }
      }
      if (solution->_vbasesol[i].indices[0] != (unsigned char)-1) {
        for (size_t j = 0; j < orgsize; ++j) {
          v[j] += solution->_vbasesol[i].indices[0];
        }
      }
    }
  }
}

void IkSolution_Free(IkSolution *solution) {
  free(solution->_vbasesol);
  free(solution->_vfree);
}

// size_t IkSolutionListBase_AddSolution(IkSolutionListBase* list, const IkSingleDOFSolutionBase* vinfos, size_t vinfos_size, const int* vfree, size_t vfree_size) {
//     list->solutions = (IkSolutionBase**)realloc(list->solutions, (list->num_solutions + 1) * sizeof(IkSolutionBase*));
//     list->solutions[list->num_solutions] = (IkSolutionBase*)malloc(sizeof(IkSolutionBase));
//     // Initialize the new solution with vinfos and vfree
//     // This is a placeholder, you need to implement the actual initialization
//     list->num_solutions++;
//     return list->num_solutions - 1;
// }
size_t IkSolutionListBase_AddSolution(IkSolutionListBase *list, const IkSingleDOFSolutionBase *vinfos, size_t vinfos_size, const int *vfree, size_t vfree_size) {
  list->solutions = (IkSolutionBase **)realloc(list->solutions, (list->num_solutions + 1) * sizeof(IkSolutionBase *));
  list->solutions[list->num_solutions] = (IkSolutionBase *)malloc(sizeof(IkSolutionBase));
  // Initialize the new solution with vinfos and vfree
  // This is a placeholder, you need to implement the actual initialization
  list->num_solutions++;
  return list->num_solutions - 1;
}

// const IkSolutionBase* IkSolutionListBase_GetSolution(const IkSolutionListBase* list, size_t index) {
//     if (index < list->num_solutions) {
//         return list->solutions[index];
//     }
//     return NULL;
// }
const IkSolutionBase *IkSolutionListBase_GetSolution(const IkSolutionListBase *list, size_t index) {
  if (index < list->num_solutions) {
    return list->solutions[index];
  }
  return NULL;
}

size_t IkSolutionListBase_GetNumSolutions(const IkSolutionListBase *list) { return list->num_solutions; }

void IkSolutionListBase_Clear(IkSolutionListBase *list) {
  for (size_t i = 0; i < list->num_solutions; ++i) {
    free(list->solutions[i]);
  }
  free(list->solutions);
  list->solutions = NULL;
  list->num_solutions = 0;
}

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// typedef struct {
//     // Define the structure for IkSolution here
//     IkSingleDOFSolutionBase* vinfos;
//     int* vfree;
//     size_t vinfos_size;
//     size_t vfree_size;
// } IkSolution;

typedef struct IkSolutionList {
  IkSolution *solutions;
  size_t size;
  size_t capacity;
} IkSolutionList;

void IkSolutionList_Init(IkSolutionList *list) {
  list->solutions = NULL;
  list->size = 0;
  list->capacity = 0;
}

size_t IkSolutionList_AddSolution(IkSolutionList *list, IkSingleDOFSolutionBase *vinfos, size_t vinfos_size, int *vfree, size_t vfree_size) {
  if (list->size == list->capacity) {
    size_t new_capacity = list->capacity == 0 ? 1 : list->capacity * 2;
    IkSolution *new_solutions = (IkSolution *)realloc(list->solutions, new_capacity * sizeof(IkSolution));
    if (!new_solutions) {
      perror("Failed to allocate memory");
      exit(EXIT_FAILURE);
    }
    list->solutions = new_solutions;
    list->capacity = new_capacity;
  }
  IkSolution *solution = &list->solutions[list->size];
  solution->_vbasesol = (IkSingleDOFSolutionBase *)malloc(vinfos_size * sizeof(IkSingleDOFSolutionBase));
  if (!solution->_vbasesol) {
    perror("Failed to allocate memory");
    exit(EXIT_FAILURE);
  }
  memcpy(solution->_vbasesol, vinfos, vinfos_size * sizeof(IkSingleDOFSolutionBase));
  solution->_vbasesol_size = vinfos_size;

  solution->_vfree = (int *)malloc(vfree_size * sizeof(int));
  if (!solution->_vfree) {
    perror("Failed to allocate memory");
    exit(EXIT_FAILURE);
  }
  memcpy(solution->_vfree, vfree, vfree_size * sizeof(int));
  solution->_vfree_size = vfree_size;

  return list->size++;
}

const IkSolution *IkSolutionList_GetSolution(const IkSolutionList *list, size_t index) {
  if (index >= list->size) {
    fprintf(stderr, "GetSolution index is invalid\n");
    exit(EXIT_FAILURE);
  }
  return &list->solutions[index];
}

size_t IkSolutionList_GetNumSolutions(const IkSolutionList *list) { return list->size; }

void IkSolutionList_Clear(IkSolutionList *list) {
  for (size_t i = 0; i < list->size; ++i) {
    free(list->solutions[i]._vbasesol);
    free(list->solutions[i]._vfree);
  }
  free(list->solutions);
  list->solutions = NULL;
  list->size = 0;
  list->capacity = 0;
}

typedef struct IKSolver {
  IkReal j0, cj0, sj0, htj0, j0mul, j1, cj1, sj1, htj1, j1mul, j2, cj2, sj2, htj2, j2mul, j3, cj3, sj3, htj3, j3mul, j4, cj4, sj4, htj4, j4mul, j5, cj5, sj5, htj5, j5mul, new_r00,
    r00, rxp0_0, new_r01, r01, rxp0_1, new_r02, r02, rxp0_2, new_r10, r10, rxp1_0, new_r11, r11, rxp1_1, new_r12, r12, rxp1_2, new_r20, r20, rxp2_0, new_r21, r21, rxp2_1, new_r22,
    r22, rxp2_2, new_px, px, npx, new_py, py, npy, new_pz, pz, npz, pp;
  unsigned char _ij0[2], _nj0, _ij1[2], _nj1, _ij2[2], _nj2, _ij3[2], _nj3, _ij4[2], _nj4, _ij5[2], _nj5;

  IkReal j100, cj100, sj100;
  unsigned char _ij100[2], _nj100;
} IKSolver;

//TODO:: `ComputeIk` is not implemented yet.
bool ComputeIk(const IkReal *eetrans, const IkReal *eerot, const IkReal *pfree, IkSolutionListBase *solutions);
//TODO:: `rotationfunction0` is not implemented yet.
void rotationfunction0(IkSolutionListBase *solutions);
static void polyroots3(IkReal rawcoeffs[4], IkReal rawroots[3], int *numroots);
static void polyroots4(IkReal rawcoeffs[5], IkReal rawroots[4], int *numroots);
static void polyroots2(double rawcoeffs[3], double rawroots[2], int *numroots);

#include <complex.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define IKFAST_ASSERT(x)                                                                                                                                                           \
  if (!(x)) {                                                                                                                                                                      \
    printf("Assertion failed: %s\n", #x);                                                                                                                                          \
    exit(1);                                                                                                                                                                       \
  }
#define IKabs(x) fabs(x)

static void polyroots3(double rawcoeffs[4], double rawroots[3], int *numroots) {
  if (rawcoeffs[0] == 0) {
    // solve with one reduced degree
    polyroots2(&rawcoeffs[1], &rawroots[0], numroots);
    return;
  }
  IKFAST_ASSERT(rawcoeffs[0] != 0);
  const double tol = 128.0 * DBL_EPSILON;
  const double tolsqrt = sqrt(DBL_EPSILON);
  double complex coeffs[3];
  const int maxsteps = 110;
  for (int i = 0; i < 3; ++i)
    coeffs[i] = rawcoeffs[i + 1] / rawcoeffs[0];
  double complex roots[3];
  double err[3];
  roots[0] = 1.0 + 0.0 * I;
  roots[1] = 0.4 + 0.9 * I; // any complex number not a root of unity works
  err[0] = 1.0;
  err[1] = 1.0;
  for (int i = 2; i < 3; ++i) {
    roots[i] = roots[i - 1] * roots[1];
    err[i] = 1.0;
  }
  for (int step = 0; step < maxsteps; ++step) {
    int changed = 0;
    for (int i = 0; i < 3; ++i) {
      if (err[i] >= tol) {
        changed = 1;
        // evaluate
        double complex x = roots[i] + coeffs[0];
        for (int j = 1; j < 3; ++j)
          x = roots[i] * x + coeffs[j];
        for (int j = 0; j < 3; ++j) {
          if (i != j) {
            if (roots[i] != roots[j])
              x /= (roots[i] - roots[j]);
          }
        }
        roots[i] -= x;
        err[i] = cabs(x);
      }
    }
    if (!changed)
      break;
  }

  *numroots = 0;
  int visited[3] = {0};
  for (int i = 0; i < 3; ++i) {
    if (!visited[i]) {
      // might be a multiple root, in which case it will have more error than
      // the other roots find any neighboring roots, and take the average
      double complex newroot = roots[i];
      int n = 1;
      for (int j = i + 1; j < 3; ++j) {
        // care about error in real much more than imaginary
        if (fabs(creal(roots[i]) - creal(roots[j])) < tolsqrt && fabs(cimag(roots[i]) - cimag(roots[j])) < 0.002) {
          newroot += roots[j];
          n += 1;
          visited[j] = 1;
        }
      }
      if (n > 1)
        newroot /= n;
      // there are still cases where even the mean is not accurate enough,
      // until a better multi-root algorithm is used, need to use the sqrt
      if (IKabs(cimag(newroot)) < tolsqrt)
        rawroots[(*numroots)++] = creal(newroot);
    }
  }
}

// #include <math.h>
// #include <stdbool.h>
// #include <stdio.h>

// typedef double IkReal;

// // typedef struct {
// //     IkReal real;
// //     IkReal imag;
// // } complex;

static void polyroots2(IkReal rawcoeffs[2 + 1], IkReal rawroots[2], int *numroots) {
  IkReal det = rawcoeffs[1] * rawcoeffs[1] - 4 * rawcoeffs[0] * rawcoeffs[2];
  if (det < 0) {
    *numroots = 0;
  } else if (det == 0) {
    rawroots[0] = -0.5 * rawcoeffs[1] / rawcoeffs[0];
    *numroots = 1;
  } else {
    det = sqrt(det);
    rawroots[0] = (-rawcoeffs[1] + det) / (2 * rawcoeffs[0]);
    rawroots[1] = (-rawcoeffs[1] - det) / (2 * rawcoeffs[0]);
    *numroots = 2;
  }
}

static void polyroots4(IkReal rawcoeffs[4 + 1], IkReal rawroots[4], int *numroots) {
  if (rawcoeffs[0] == 0) {
    // solve with one reduced degree
    polyroots3(&rawcoeffs[1], &rawroots[0], numroots);
    return;
  }

  const IkReal tol = 128.0 * DBL_EPSILON;
  const IkReal tolsqrt = sqrt(DBL_EPSILON);
  complex coeffs[4];
  const int maxsteps = 110;
  for (int i = 0; i < 4; ++i)
    coeffs[i] = rawcoeffs[i + 1] / rawcoeffs[0];

  complex roots[4];
  IkReal err[4];
  roots[0] = 1.0 + 0.0 * I;
  roots[1] = 0.4 + 0.9 * I; // any complex number not a root of unity works
  err[0] = 1.0;
  err[1] = 1.0;

  for (int i = 2; i < 4; ++i) {
    // roots[i] = (complex){roots[i - 1].real * roots[1].real - roots[i - 1].imag * roots[1].imag, roots[i - 1].real * roots[1].imag + roots[i - 1].imag * roots[1].real};
    roots[i] = roots[i - 1] * roots[1];
    err[i] = 1.0;
  }

  for (int step = 0; step < maxsteps; ++step) {
    bool changed = false;
    for (int i = 0; i < 4; ++i) {
      if (err[i] >= tol) {
        changed = true;
        // complex x = (complex){roots[i].real + coeffs[0].real, roots[i].imag + coeffs[0].imag};
        complex x = roots[i] + coeffs[0];
        for (int j = 1; j < 4; ++j) {
          // x = (complex){x.real * roots[i].real - x.imag * roots[i].imag + coeffs[j].real, x.real * roots[i].imag + x.imag * roots[i].real + coeffs[j].imag};
          x = roots[i] * x + coeffs[j];
        }
        for (int j = 0; j < 4; ++j) {
          if (i != j) {
            // if (roots[i].real != roots[j].real || roots[i].imag != roots[j].imag) {
            //     x.real /= (roots[i].real - roots[j].real);
            //     x.imag /= (roots[i].real - roots[j].real);
            // }
            if (roots[i] != roots[j]) {
              x /= (roots[i] - roots[j]);
            }
          }
        }
        //   roots[i] = (complex){roots[i].real - x.real, roots[i].imag - x.imag};
        roots[i] -= x;
        //   err[i] = sqrt(x.real * x.real + x.imag * x.imag);
        err[i] = fabs(x);
      }
    }
    if (!changed)
      break;
  }

  *numroots = 0;
  bool visited[4] = {false};
  for (int i = 0; i < 4; ++i) {
    if (!visited[i]) {
      complex newroot = roots[i];
      int n = 1;
      for (int j = i + 1; j < 4; ++j) {
        // if (fabs(roots[i].real - roots[j].real) < tolsqrt && fabs(roots[i].imag - roots[j].imag) < 0.002) {
          if (fabs(creal(roots[i]) - creal(roots[j])) < tolsqrt && fabs(cimag(roots[i]) - cimag(roots[j])) < 0.002) {
        //   newroot.real += roots[j].real;
        //   newroot.imag += roots[j].imag;
        newroot +=roots[j];
          n += 1;
          visited[j] = true;
        }
      }
      if (n > 1) {
        // newroot.real /= n;
        // newroot.imag /= n;
        newroot/=n;
      }
      if (fabs(cimag(newroot)) < tolsqrt)
        rawroots[(*numroots)++] = creal(newroot);
    }
  }
}
