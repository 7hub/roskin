#include "ik.h"

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

// static void polyroots2(IkReal rawcoeffs[2 + 1], IkReal rawroots[2], int *numroots) {
//   IkReal det = rawcoeffs[1] * rawcoeffs[1] - 4 * rawcoeffs[0] * rawcoeffs[2];
//   if (det < 0) {
//     *numroots = 0;
//   } else if (det == 0) {
//     rawroots[0] = -0.5 * rawcoeffs[1] / rawcoeffs[0];
//     *numroots = 1;
//   } else {
//     det = sqrt(det);
//     rawroots[0] = (-rawcoeffs[1] + det) / (2 * rawcoeffs[0]);
//     rawroots[1] = (-rawcoeffs[1] - det) / (2 * rawcoeffs[0]);
//     *numroots = 2;
//   }
// }

static void polyroots2(IkReal rawcoeffs[3], IkReal rawroots[2], int *numroots) {
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
          newroot += roots[j];
          n += 1;
          visited[j] = true;
        }
      }
      if (n > 1) {
        // newroot.real /= n;
        // newroot.imag /= n;
        newroot /= n;
      }
      if (fabs(cimag(newroot)) < tolsqrt)
        rawroots[(*numroots)++] = creal(newroot);
    }
  }
}

#ifdef __CCONVERTED__
// #define NaN (IkReal)nan("");
#define IKRREAL_NAN NAN;

//  IkReal j100, cj100, sj100;
//   unsigned char _ij100[2], _nj100;
int ComputeIk(IKSolver *solver, const IkReal *eetrans, const IkReal *eerot, const IkReal *pfree, IkSolutionListBase *solutions) {

  solver->j0 = IKRREAL_NAN;

  solver->_ij0[0] = -1;
  solver->_ij0[1] = -1;
  solver->_nj0 = -1;
  solver->j1 = NAN;
  solver->_ij1[0] = -1;
  solver->_ij1[1] = -1;
  solver->_nj1 = -1;
  solver->j2 = NaN;
  solver->_ij2[0] = -1;
  solver->_ij2[1] = -1;
  solver->_nj2 = -1;
  solver->j3 = NaN;
  solver->_ij3[0] = -1;
  solver->_ij3[1] = -1;
  solver->_nj3 = -1;
  solver->j4 = NaN;
  solver->_ij4[0] = -1;
  solver->_ij4[1] = -1;
  solver->_nj4 = -1;
  solver->j5 = NaN;
  solver->_ij5[0] = -1;
  solver->_ij5[1] = -1;
  solver->_nj5 = -1;
  for (int dummyiter = 0; dummyiter < 1; ++dummyiter) {
    // solutions.Clear();
    IkSolutionListBase_Clear(solutions);
    solver->r00 = eerot[0 * 3 + 0];
    solver->r01 = eerot[0 * 3 + 1];
    solver->r02 = eerot[0 * 3 + 2];
    solver->r10 = eerot[1 * 3 + 0];
    solver->r11 = eerot[1 * 3 + 1];
    solver->r12 = eerot[1 * 3 + 2];
    solver->r20 = eerot[2 * 3 + 0];
    solver->r21 = eerot[2 * 3 + 1];
    solver->r22 = eerot[2 * 3 + 2];
    solver->px = eetrans[0];
    solver->py = eetrans[1];
    solver->pz = eetrans[2];

    solver->new_r00 = solver->r00;
    solver->new_r01 = solver->r01;
    solver->new_r02 = solver->r02;
    solver->new_px = (((-0.07) * solver->r02) + solver->px);
    solver->new_r10 = solver->r10;
    solver->new_r11 = solver->r11;
    solver->new_r12 = solver->r12;
    solver->new_py = (((-0.07) * solver->r12) + solver->py);
    solver->new_r20 = solver->r20;
    solver->new_r21 = solver->r21;
    solver->new_r22 = solver->r22;
    solver->new_pz = ((-0.375) + solver->pz + ((-0.07) * solver->r22));
    solver->r00 = solver->new_r00;
    solver->r01 = solver->new_r01;
    solver->r02 = solver->new_r02;
    solver->r10 = solver->new_r10;
    solver->r11 = solver->new_r11;
    solver->r12 = solver->new_r12;
    solver->r20 = solver->new_r20;
    solver->r21 = solver->new_r21;
    solver->r22 = solver->new_r22;
    solver->new_px;
    solver->new_py;
    solver->new_pz;
    IkReal x48 = ((1.0) * px);
    IkReal x49 = ((1.0) * pz);
    IkReal x50 = ((1.0) * py);
    pp = ((px * px) + (py * py) + (pz * pz));
    npx = ((px * r00) + (py * r10) + (pz * r20));
    npy = ((px * r01) + (py * r11) + (pz * r21));
    npz = ((px * r02) + (py * r12) + (pz * r22));
    rxp0_0 = (((-1.0) * r20 * x50) + (pz * r10));
    rxp0_1 = ((px * r20) + ((-1.0) * r00 * x49));
    rxp0_2 = (((-1.0) * r10 * x48) + (py * r00));
    rxp1_0 = (((-1.0) * r21 * x50) + (pz * r11));
    rxp1_1 = ((px * r21) + ((-1.0) * r01 * x49));
    rxp1_2 = (((-1.0) * r11 * x48) + (py * r01));
    rxp2_0 = ((pz * r12) + ((-1.0) * r22 * x50));
    rxp2_1 = (((-1.0) * r02 * x49) + (px * r22));
    rxp2_2 = (((-1.0) * r12 * x48) + (py * r02));
    {
      IkReal j2array[2], cj2array[2], sj2array[2];
      bool j2valid[2] = {false};
      _nj2 = 2;
      cj2array[0] = ((-1.00444938820912) + ((5.56173526140156) * pp));
      if (cj2array[0] >= -1 - IKFAST_SINCOS_THRESH && cj2array[0] <= 1 + IKFAST_SINCOS_THRESH) {
        j2valid[0] = j2valid[1] = true;
        j2array[0] = IKacos(cj2array[0]);
        sj2array[0] = IKsin(j2array[0]);
        cj2array[1] = cj2array[0];
        j2array[1] = -j2array[0];
        sj2array[1] = -sj2array[0];
      } else if (isnan(cj2array[0])) {
        // probably any value will work
        j2valid[0] = true;
        cj2array[0] = 1;
        sj2array[0] = 0;
        j2array[0] = 0;
      }
      for (int ij2 = 0; ij2 < 2; ++ij2) {
        if (!j2valid[ij2])
          continue;
        _ij2[0] = ij2;
        _ij2[1] = -1;
        for (int iij2 = ij2 + 1; iij2 < 2; ++iij2) {
          if (j2valid[iij2] && IKabs(cj2array[ij2] - cj2array[iij2]) < IKFAST_SOLUTION_THRESH && IKabs(sj2array[ij2] - sj2array[iij2]) < IKFAST_SOLUTION_THRESH) {
            j2valid[iij2] = false;
            _ij2[1] = iij2;
            break;
          }
        }
        j2 = j2array[ij2];
        cj2 = cj2array[ij2];
        sj2 = sj2array[ij2];

        {
          IkReal j0eval[2];
          j0eval[0] = ((px * px) + (py * py));
          j0eval[1] = ((IKabs(px)) + (IKabs(py)));
          if (IKabs(j0eval[0]) < 0.0000010000000000 || IKabs(j0eval[1]) < 0.0000010000000000) {
            {
              IkReal j1eval[2];
              j1eval[0] = ((1.0) + ((1.14268727705113) * (sj2 * sj2)) + ((1.14268727705113) * (cj2 * cj2)) + ((2.13793103448276) * cj2));
              j1eval[1] = ((IKabs(sj2)) + ((3.2258064516129) * (IKabs((-0.29) + ((-0.31) * cj2)))));
              if (IKabs(j1eval[0]) < 0.0000010000000000 || IKabs(j1eval[1]) < 0.0000010000000000) {
                continue; // no branches [j0, j1]

              } else {
                {
                  IkReal j1array[2], cj1array[2], sj1array[2];
                  bool j1valid[2] = {false};
                  _nj1 = 2;
                  IkReal x51 = ((-0.29) + ((-0.31) * cj2));
                  CheckValue<IkReal> x54 = IKatan2WithCheck(IkReal(x51), IkReal((0.31) * sj2), IKFAST_ATAN2_MAGTHRESH);
                  if (!x54.valid)
                    continue;
                  IkReal x52 = ((1.0) * (x54.value));
                  if ((((0.0961) * (sj2 * sj2)) + (x51 * x51)) < -0.00001)
                    continue;
                  CheckValue<IkReal> x55 = IKPowWithIntegerCheck(IKabs(IKsqrt(((0.0961) * (sj2 * sj2)) + (x51 * x51))), -1);
                  if (!x55.valid)
                    continue;
                  if ((pz * (x55.value)) < -1 - IKFAST_SINCOS_THRESH || (pz * (x55.value)) > 1 + IKFAST_SINCOS_THRESH) {
                    continue;
                  }
                  IkReal x53 = IKasin(pz * (x55.value));
                  j1array[0] = (((-1.0) * x52) + ((-1.0) * x53));
                  sj1array[0] = IKsin(j1array[0]);
                  cj1array[0] = IKcos(j1array[0]);
                  j1array[1] = ((3.14159265358979) + x53 + ((-1.0) * x52));
                  sj1array[1] = IKsin(j1array[1]);
                  cj1array[1] = IKcos(j1array[1]);
                  if (j1array[0] > IKPI)
                    j1array[0] -= IK2PI;
                  else if (j1array[0] < -IKPI)
                    j1array[0] += IK2PI;
                  j1valid[0] = true;
                  if (j1array[1] > IKPI)
                    j1array[1] -= IK2PI;
                  else if (j1array[1] < -IKPI)
                    j1array[1] += IK2PI;
                  j1valid[1] = true;
                  for (int ij1 = 0; ij1 < 2; ++ij1) {
                    if (!j1valid[ij1])
                      continue;
                    _ij1[0] = ij1;
                    _ij1[1] = -1;
                    for (int iij1 = ij1 + 1; iij1 < 2; ++iij1) {
                      if (j1valid[iij1] && IKabs(cj1array[ij1] - cj1array[iij1]) < IKFAST_SOLUTION_THRESH && IKabs(sj1array[ij1] - sj1array[iij1]) < IKFAST_SOLUTION_THRESH) {
                        j1valid[iij1] = false;
                        _ij1[1] = iij1;
                        break;
                      }
                    }
                    j1 = j1array[ij1];
                    cj1 = cj1array[ij1];
                    sj1 = sj1array[ij1];

                    {
                      IkReal j0eval[3];
                      IkReal x56 = pz * pz;
                      IkReal x57 = (cj1 * pp);
                      IkReal x58 = ((1.0) * cj1);
                      IkReal x59 = ((15.5) * sj2);
                      IkReal x60 = ((50.0) * pz * sj1);
                      j0eval[0] = (((-1.0) * x56 * x58) + x57);
                      j0eval[1] = ((IKabs((px * x59) + (px * x60) + (cj1 * py))) + (IKabs(((-1.0) * px * x58) + (py * x60) + (py * x59))));
                      j0eval[2] = IKsign(((-50.0) * cj1 * x56) + ((50.0) * x57));
                      if (IKabs(j0eval[0]) < 0.0000010000000000 || IKabs(j0eval[1]) < 0.0000010000000000 || IKabs(j0eval[2]) < 0.0000010000000000) {
                        {
                          IkReal j0eval[3];
                          IkReal x61 = pz * pz;
                          IkReal x62 = (py * sj1);
                          IkReal x63 = ((15.5) * cj2);
                          IkReal x64 = (px * sj1);
                          IkReal x65 = ((15.5) * cj1 * sj2);
                          j0eval[0] = (pp + ((-1.0) * x61));
                          j0eval[1] = ((IKabs((px * x65) + ((14.5) * x64) + py + (x63 * x64))) + (IKabs((x62 * x63) + ((14.5) * x62) + ((-1.0) * px) + (py * x65))));
                          j0eval[2] = IKsign(((-50.0) * x61) + ((50.0) * pp));
                          if (IKabs(j0eval[0]) < 0.0000010000000000 || IKabs(j0eval[1]) < 0.0000010000000000 || IKabs(j0eval[2]) < 0.0000010000000000) {
                            {
                              IkReal j0eval[3];
                              IkReal x66 = pz * pz;
                              IkReal x67 = ((15.5) * cj2);
                              IkReal x68 = ((1.0) * sj1);
                              IkReal x69 = (pp * sj1);
                              IkReal x70 = ((50.0) * cj1 * pz);
                              j0eval[0] = (((-1.0) * x66 * x68) + x69);
                              j0eval[1] = IKsign(((-50.0) * sj1 * x66) + ((50.0) * x69));
                              j0eval[2] = ((IKabs(((-1.0) * px * x68) + ((14.5) * py) + ((-1.0) * py * x70) + (py * x67))) +
                                           (IKabs(((-1.0) * px * x70) + (px * x67) + ((14.5) * px) + (py * sj1))));
                              if (IKabs(j0eval[0]) < 0.0000010000000000 || IKabs(j0eval[1]) < 0.0000010000000000 || IKabs(j0eval[2]) < 0.0000010000000000) {
                                {
                                  IkReal evalcond[1];
                                  bool bgotonextstatement = true;
                                  do {
                                    evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs(j1))), 6.28318530717959)));
                                    if (IKabs(evalcond[0]) < 0.0000050000000000) {
                                      bgotonextstatement = false;
                                      {
                                        IkReal j0eval[3];
                                        sj1 = 0;
                                        cj1 = 1.0;
                                        j1 = 0;
                                        IkReal x71 = pz * pz;
                                        IkReal x72 = ((31.0) * sj2);
                                        j0eval[0] = (pp + ((-1.0) * x71));
                                        j0eval[1] = IKsign(((100.0) * pp) + ((-100.0) * x71));
                                        j0eval[2] = ((IKabs((py * x72) + ((-2.0) * px))) + (IKabs(((2.0) * py) + (px * x72))));
                                        if (IKabs(j0eval[0]) < 0.0000010000000000 || IKabs(j0eval[1]) < 0.0000010000000000 || IKabs(j0eval[2]) < 0.0000010000000000) {
                                          continue; // no branches [j0]

                                        } else {
                                          {
                                            IkReal j0array[1], cj0array[1], sj0array[1];
                                            bool j0valid[1] = {false};
                                            _nj0 = 1;
                                            IkReal x73 = ((31.0) * sj2);
                                            CheckValue<IkReal> x74 = IKPowWithIntegerCheck(IKsign(((-100.0) * (pz * pz)) + ((100.0) * pp)), -1);
                                            if (!x74.valid)
                                              continue;
                                            CheckValue<IkReal> x75 =
                                              IKatan2WithCheck(IkReal((py * x73) + ((-2.0) * px)), IkReal(((2.0) * py) + (px * x73)), IKFAST_ATAN2_MAGTHRESH);
                                            if (!x75.valid)
                                              continue;
                                            j0array[0] = ((-1.5707963267949) + ((1.5707963267949) * (x74.value)) + (x75.value));
                                            sj0array[0] = IKsin(j0array[0]);
                                            cj0array[0] = IKcos(j0array[0]);
                                            if (j0array[0] > IKPI)
                                              j0array[0] -= IK2PI;
                                            else if (j0array[0] < -IKPI)
                                              j0array[0] += IK2PI;
                                            j0valid[0] = true;
                                            for (int ij0 = 0; ij0 < 1; ++ij0) {
                                              if (!j0valid[ij0])
                                                continue;
                                              _ij0[0] = ij0;
                                              _ij0[1] = -1;
                                              for (int iij0 = ij0 + 1; iij0 < 1; ++iij0) {
                                                if (j0valid[iij0] && IKabs(cj0array[ij0] - cj0array[iij0]) < IKFAST_SOLUTION_THRESH &&
                                                    IKabs(sj0array[ij0] - sj0array[iij0]) < IKFAST_SOLUTION_THRESH) {
                                                  j0valid[iij0] = false;
                                                  _ij0[1] = iij0;
                                                  break;
                                                }
                                              }
                                              j0 = j0array[ij0];
                                              cj0 = cj0array[ij0];
                                              sj0 = sj0array[ij0];
                                              {
                                                IkReal evalcond[2];
                                                IkReal x76 = IKcos(j0);
                                                IkReal x77 = IKsin(j0);
                                                IkReal x78 = ((1.0) * x76);
                                                evalcond[0] = ((0.02) + ((-1.0) * py * x78) + (px * x77));
                                                evalcond[1] = (((0.31) * sj2) + ((-1.0) * px * x78) + ((-1.0) * py * x77));
                                                if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH) {
                                                  continue;
                                                }
                                              }

                                              rotationfunction0(solutions);
                                            }
                                          }
                                        }
                                      }
                                    }
                                  } while (0);
                                  if (bgotonextstatement) {
                                    bool bgotonextstatement = true;
                                    do {
                                      evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs((-3.14159265358979) + j1))), 6.28318530717959)));
                                      if (IKabs(evalcond[0]) < 0.0000050000000000) {
                                        bgotonextstatement = false;
                                        {
                                          IkReal j0eval[3];
                                          sj1 = 0;
                                          cj1 = -1.0;
                                          j1 = 3.14159265358979;
                                          IkReal x619 = pz * pz;
                                          IkReal x620 = ((31.0) * sj2);
                                          j0eval[0] = (((-1.0) * pp) + x619);
                                          j0eval[1] = ((IKabs((py * x620) + ((2.0) * px))) + (IKabs(((-2.0) * py) + (px * x620))));
                                          j0eval[2] = IKsign(((-100.0) * pp) + ((100.0) * x619));
                                          if (IKabs(j0eval[0]) < 0.0000010000000000 || IKabs(j0eval[1]) < 0.0000010000000000 || IKabs(j0eval[2]) < 0.0000010000000000) {
                                            continue; // no branches [j0]

                                          } else {
                                            {
                                              IkReal j0array[1], cj0array[1], sj0array[1];
                                              bool j0valid[1] = {false};
                                              _nj0 = 1;
                                              IkReal x621 = ((31.0) * sj2);
                                              CheckValue<IkReal> x622 =
                                                IKatan2WithCheck(IkReal((py * x621) + ((2.0) * px)), IkReal(((-2.0) * py) + (px * x621)), IKFAST_ATAN2_MAGTHRESH);
                                              if (!x622.valid)
                                                continue;
                                              CheckValue<IkReal> x623 = IKPowWithIntegerCheck(IKsign(((-100.0) * pp) + ((100.0) * (pz * pz))), -1);
                                              if (!x623.valid)
                                                continue;
                                              j0array[0] = ((-1.5707963267949) + (x622.value) + ((1.5707963267949) * (x623.value)));
                                              sj0array[0] = IKsin(j0array[0]);
                                              cj0array[0] = IKcos(j0array[0]);
                                              if (j0array[0] > IKPI)
                                                j0array[0] -= IK2PI;
                                              else if (j0array[0] < -IKPI)
                                                j0array[0] += IK2PI;
                                              j0valid[0] = true;
                                              for (int ij0 = 0; ij0 < 1; ++ij0) {
                                                if (!j0valid[ij0])
                                                  continue;
                                                _ij0[0] = ij0;
                                                _ij0[1] = -1;
                                                for (int iij0 = ij0 + 1; iij0 < 1; ++iij0) {
                                                  if (j0valid[iij0] && IKabs(cj0array[ij0] - cj0array[iij0]) < IKFAST_SOLUTION_THRESH &&
                                                      IKabs(sj0array[ij0] - sj0array[iij0]) < IKFAST_SOLUTION_THRESH) {
                                                    j0valid[iij0] = false;
                                                    _ij0[1] = iij0;
                                                    break;
                                                  }
                                                }
                                                j0 = j0array[ij0];
                                                cj0 = cj0array[ij0];
                                                sj0 = sj0array[ij0];
                                                {
                                                  IkReal evalcond[2];
                                                  IkReal x624 = IKsin(j0);
                                                  IkReal x625 = IKcos(j0);
                                                  evalcond[0] = ((0.02) + (px * x624) + ((-1.0) * py * x625));
                                                  evalcond[1] = ((py * x624) + ((0.31) * sj2) + (px * x625));
                                                  if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH) {
                                                    continue;
                                                  }
                                                }

                                                rotationfunction0(solutions);
                                              }
                                            }
                                          }
                                        }
                                      }
                                    } while (0);
                                    if (bgotonextstatement) {
                                      bool bgotonextstatement = true;
                                      do {
                                        evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs((-1.5707963267949) + j1))), 6.28318530717959)));
                                        if (IKabs(evalcond[0]) < 0.0000050000000000) {
                                          bgotonextstatement = false;
                                          {
                                            IkReal j0eval[3];
                                            sj1 = 1.0;
                                            cj1 = 0;
                                            j1 = 1.5707963267949;
                                            IkReal x626 = pz * pz;
                                            IkReal x627 = ((86.2068965517241) * pp);
                                            j0eval[0] = (((-1.0) * pp) + x626);
                                            j0eval[1] = IKsign(((50.0) * x626) + ((-50.0) * pp));
                                            j0eval[2] = ((IKabs(((-1.0) * py) + ((1.06896551724138) * px) + ((-1.0) * px * x627))) +
                                                         (IKabs(px + ((1.06896551724138) * py) + ((-1.0) * py * x627))));
                                            if (IKabs(j0eval[0]) < 0.0000010000000000 || IKabs(j0eval[1]) < 0.0000010000000000 || IKabs(j0eval[2]) < 0.0000010000000000) {
                                              {
                                                IkReal j0eval[3];
                                                sj1 = 1.0;
                                                cj1 = 0;
                                                j1 = 1.5707963267949;
                                                IkReal x628 = pz * pz;
                                                IkReal x629 = ((2500.0) * pp);
                                                j0eval[0] = (((-1.0) * pp) + x628);
                                                j0eval[1] =
                                                  ((IKabs(((31.0) * px) + ((-29.0) * py) + ((-1.0) * px * x629))) + (IKabs(((31.0) * py) + ((29.0) * px) + ((-1.0) * py * x629))));
                                                j0eval[2] = IKsign(((1450.0) * x628) + ((-1450.0) * pp));
                                                if (IKabs(j0eval[0]) < 0.0000010000000000 || IKabs(j0eval[1]) < 0.0000010000000000 || IKabs(j0eval[2]) < 0.0000010000000000) {
                                                  continue; // no branches
                                                            // [j0]

                                                } else {
                                                  {
                                                    IkReal j0array[1], cj0array[1], sj0array[1];
                                                    bool j0valid[1] = {false};
                                                    _nj0 = 1;
                                                    IkReal x630 = ((2500.0) * pp);
                                                    CheckValue<IkReal> x631 = IKPowWithIntegerCheck(IKsign(((1450.0) * (pz * pz)) + ((-1450.0) * pp)), -1);
                                                    if (!x631.valid)
                                                      continue;
                                                    CheckValue<IkReal> x632 =
                                                      IKatan2WithCheck(IkReal(((31.0) * py) + ((29.0) * px) + ((-1.0) * py * x630)),
                                                                       IkReal(((31.0) * px) + ((-29.0) * py) + ((-1.0) * px * x630)), IKFAST_ATAN2_MAGTHRESH);
                                                    if (!x632.valid)
                                                      continue;
                                                    j0array[0] = ((-1.5707963267949) + ((1.5707963267949) * (x631.value)) + (x632.value));
                                                    sj0array[0] = IKsin(j0array[0]);
                                                    cj0array[0] = IKcos(j0array[0]);
                                                    if (j0array[0] > IKPI) {
                                                      j0array[0] -= IK2PI;
                                                    } else if (j0array[0] < -IKPI) {
                                                      j0array[0] += IK2PI;
                                                    }
                                                    j0valid[0] = true;
                                                    for (int ij0 = 0; ij0 < 1; ++ij0) {
                                                      if (!j0valid[ij0])
                                                        continue;
                                                      _ij0[0] = ij0;
                                                      _ij0[1] = -1;
                                                      for (int iij0 = ij0 + 1; iij0 < 1; ++iij0) {
                                                        if (j0valid[iij0] && IKabs(cj0array[ij0] - cj0array[iij0]) < IKFAST_SOLUTION_THRESH &&
                                                            IKabs(sj0array[ij0] - sj0array[iij0]) < IKFAST_SOLUTION_THRESH) {
                                                          j0valid[iij0] = false;
                                                          _ij0[1] = iij0;
                                                          break;
                                                        }
                                                      }
                                                      j0 = j0array[ij0];
                                                      cj0 = cj0array[ij0];
                                                      sj0 = sj0array[ij0];
                                                      {
                                                        IkReal evalcond[3];
                                                        IkReal x633 = IKsin(j0);
                                                        IkReal x634 = IKcos(j0);
                                                        IkReal x635 = (py * x633);
                                                        IkReal x636 = (px * x634);
                                                        evalcond[0] = ((0.02) + ((-1.0) * py * x634) + (px * x633));
                                                        evalcond[1] = ((0.0213793103448276) + x636 + x635 + ((-1.72413793103448) * pp));
                                                        evalcond[2] = ((0.0124) + ((0.58) * x635) + ((0.58) * x636) + ((-1.0) * pp));
                                                        if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                            IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH) {
                                                          continue;
                                                        }
                                                      }

                                                      rotationfunction0(solutions);
                                                    }
                                                  }
                                                }
                                              }

                                            } else {
                                              {
                                                IkReal j0array[1], cj0array[1], sj0array[1];
                                                bool j0valid[1] = {false};
                                                _nj0 = 1;
                                                IkReal x637 = ((86.2068965517241) * pp);
                                                CheckValue<IkReal> x638 =
                                                  IKatan2WithCheck(IkReal(px + ((1.06896551724138) * py) + ((-1.0) * py * x637)),
                                                                   IkReal(((-1.0) * py) + ((1.06896551724138) * px) + ((-1.0) * px * x637)), IKFAST_ATAN2_MAGTHRESH);
                                                if (!x638.valid)
                                                  continue;
                                                CheckValue<IkReal> x639 = IKPowWithIntegerCheck(IKsign(((-50.0) * pp) + ((50.0) * (pz * pz))), -1);
                                                if (!x639.valid)
                                                  continue;
                                                j0array[0] = ((-1.5707963267949) + (x638.value) + ((1.5707963267949) * (x639.value)));
                                                sj0array[0] = IKsin(j0array[0]);
                                                cj0array[0] = IKcos(j0array[0]);
                                                if (j0array[0] > IKPI)
                                                  j0array[0] -= IK2PI;
                                                else if (j0array[0] < -IKPI)
                                                  j0array[0] += IK2PI;
                                                j0valid[0] = true;
                                                for (int ij0 = 0; ij0 < 1; ++ij0) {
                                                  if (!j0valid[ij0])
                                                    continue;
                                                  _ij0[0] = ij0;
                                                  _ij0[1] = -1;
                                                  for (int iij0 = ij0 + 1; iij0 < 1; ++iij0) {
                                                    if (j0valid[iij0] && IKabs(cj0array[ij0] - cj0array[iij0]) < IKFAST_SOLUTION_THRESH &&
                                                        IKabs(sj0array[ij0] - sj0array[iij0]) < IKFAST_SOLUTION_THRESH) {
                                                      j0valid[iij0] = false;
                                                      _ij0[1] = iij0;
                                                      break;
                                                    }
                                                  }
                                                  j0 = j0array[ij0];
                                                  cj0 = cj0array[ij0];
                                                  sj0 = sj0array[ij0];
                                                  {
                                                    IkReal evalcond[3];
                                                    IkReal x640 = IKsin(j0);
                                                    IkReal x641 = IKcos(j0);
                                                    IkReal x642 = (py * x640);
                                                    IkReal x643 = (px * x641);
                                                    evalcond[0] = ((0.02) + ((-1.0) * py * x641) + (px * x640));
                                                    evalcond[1] = ((0.0213793103448276) + x643 + x642 + ((-1.72413793103448) * pp));
                                                    evalcond[2] = ((0.0124) + ((0.58) * x642) + ((0.58) * x643) + ((-1.0) * pp));
                                                    if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                        IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH) {
                                                      continue;
                                                    }
                                                  }

                                                  rotationfunction0(solutions);
                                                }
                                              }
                                            }
                                          }
                                        }
                                      } while (0);
                                      if (bgotonextstatement) {
                                        bool bgotonextstatement = true;
                                        do {
                                          evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs((1.5707963267949) + j1))), 6.28318530717959)));
                                          if (IKabs(evalcond[0]) < 0.0000050000000000) {
                                            bgotonextstatement = false;
                                            {
                                              IkReal j0eval[3];
                                              sj1 = -1.0;
                                              cj1 = 0;
                                              j1 = -1.5707963267949;
                                              IkReal x644 = pz * pz;
                                              IkReal x645 = ((86.2068965517241) * pp);
                                              j0eval[0] = (((-1.0) * x644) + pp);
                                              j0eval[1] = ((IKabs(((-1.0) * px * x645) + py + ((1.06896551724138) * px))) +
                                                           (IKabs(((-1.0) * px) + ((1.06896551724138) * py) + ((-1.0) * py * x645))));
                                              j0eval[2] = IKsign(((-50.0) * x644) + ((50.0) * pp));
                                              if (IKabs(j0eval[0]) < 0.0000010000000000 || IKabs(j0eval[1]) < 0.0000010000000000 || IKabs(j0eval[2]) < 0.0000010000000000) {
                                                {
                                                  IkReal j0eval[3];
                                                  sj1 = -1.0;
                                                  cj1 = 0;
                                                  j1 = -1.5707963267949;
                                                  IkReal x646 = pz * pz;
                                                  IkReal x647 = ((2500.0) * pp);
                                                  j0eval[0] = (((-1.0) * x646) + pp);
                                                  j0eval[1] = IKsign(((1450.0) * pp) + ((-1450.0) * x646));
                                                  j0eval[2] = ((IKabs(((31.0) * py) + ((-29.0) * px) + ((-1.0) * py * x647))) +
                                                               (IKabs(((31.0) * px) + ((29.0) * py) + ((-1.0) * px * x647))));
                                                  if (IKabs(j0eval[0]) < 0.0000010000000000 || IKabs(j0eval[1]) < 0.0000010000000000 || IKabs(j0eval[2]) < 0.0000010000000000) {
                                                    continue; // no branches
                                                              // [j0]

                                                  } else {
                                                    {
                                                      IkReal j0array[1], cj0array[1], sj0array[1];
                                                      bool j0valid[1] = {false};
                                                      _nj0 = 1;
                                                      IkReal x648 = ((2500.0) * pp);
                                                      CheckValue<IkReal> x649 = IKPowWithIntegerCheck(IKsign(((1450.0) * pp) + ((-1450.0) * (pz * pz))), -1);
                                                      if (!x649.valid)
                                                        continue;
                                                      CheckValue<IkReal> x650 =
                                                        IKatan2WithCheck(IkReal(((31.0) * py) + ((-29.0) * px) + ((-1.0) * py * x648)),
                                                                         IkReal(((31.0) * px) + ((29.0) * py) + ((-1.0) * px * x648)), IKFAST_ATAN2_MAGTHRESH);
                                                      if (!x650.valid)
                                                        continue;
                                                      j0array[0] = ((-1.5707963267949) + ((1.5707963267949) * (x649.value)) + (x650.value));
                                                      sj0array[0] = IKsin(j0array[0]);
                                                      cj0array[0] = IKcos(j0array[0]);
                                                      if (j0array[0] > IKPI) {
                                                        j0array[0] -= IK2PI;
                                                      } else if (j0array[0] < -IKPI) {
                                                        j0array[0] += IK2PI;
                                                      }
                                                      j0valid[0] = true;
                                                      for (int ij0 = 0; ij0 < 1; ++ij0) {
                                                        if (!j0valid[ij0])
                                                          continue;
                                                        _ij0[0] = ij0;
                                                        _ij0[1] = -1;
                                                        for (int iij0 = ij0 + 1; iij0 < 1; ++iij0) {
                                                          if (j0valid[iij0] && IKabs(cj0array[ij0] - cj0array[iij0]) < IKFAST_SOLUTION_THRESH &&
                                                              IKabs(sj0array[ij0] - sj0array[iij0]) < IKFAST_SOLUTION_THRESH) {
                                                            j0valid[iij0] = false;
                                                            _ij0[1] = iij0;
                                                            break;
                                                          }
                                                        }
                                                        j0 = j0array[ij0];
                                                        cj0 = cj0array[ij0];
                                                        sj0 = sj0array[ij0];
                                                        {
                                                          IkReal evalcond[3];
                                                          IkReal x651 = IKcos(j0);
                                                          IkReal x652 = IKsin(j0);
                                                          IkReal x653 = ((1.0) * x651);
                                                          IkReal x654 = (py * x652);
                                                          evalcond[0] = ((0.02) + (px * x652) + ((-1.0) * py * x653));
                                                          evalcond[1] = ((0.0213793103448276) + ((-1.0) * px * x653) + ((-1.0) * x654) + ((-1.72413793103448) * pp));
                                                          evalcond[2] = ((0.0124) + ((-0.58) * x654) + ((-0.58) * px * x651) + ((-1.0) * pp));
                                                          if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                              IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH) {
                                                            continue;
                                                          }
                                                        }

                                                        rotationfunction0(solutions);
                                                      }
                                                    }
                                                  }
                                                }

                                              } else {
                                                {
                                                  IkReal j0array[1], cj0array[1], sj0array[1];
                                                  bool j0valid[1] = {false};
                                                  _nj0 = 1;
                                                  IkReal x655 = ((86.2068965517241) * pp);
                                                  CheckValue<IkReal> x656 = IKatan2WithCheck(IkReal(((-1.0) * px) + ((1.06896551724138) * py) + ((-1.0) * py * x655)),
                                                                                             IkReal(((-1.0) * px * x655) + py + ((1.06896551724138) * px)), IKFAST_ATAN2_MAGTHRESH);
                                                  if (!x656.valid)
                                                    continue;
                                                  CheckValue<IkReal> x657 = IKPowWithIntegerCheck(IKsign(((-50.0) * (pz * pz)) + ((50.0) * pp)), -1);
                                                  if (!x657.valid)
                                                    continue;
                                                  j0array[0] = ((-1.5707963267949) + (x656.value) + ((1.5707963267949) * (x657.value)));
                                                  sj0array[0] = IKsin(j0array[0]);
                                                  cj0array[0] = IKcos(j0array[0]);
                                                  if (j0array[0] > IKPI)
                                                    j0array[0] -= IK2PI;
                                                  else if (j0array[0] < -IKPI)
                                                    j0array[0] += IK2PI;
                                                  j0valid[0] = true;
                                                  for (int ij0 = 0; ij0 < 1; ++ij0) {
                                                    if (!j0valid[ij0])
                                                      continue;
                                                    _ij0[0] = ij0;
                                                    _ij0[1] = -1;
                                                    for (int iij0 = ij0 + 1; iij0 < 1; ++iij0) {
                                                      if (j0valid[iij0] && IKabs(cj0array[ij0] - cj0array[iij0]) < IKFAST_SOLUTION_THRESH &&
                                                          IKabs(sj0array[ij0] - sj0array[iij0]) < IKFAST_SOLUTION_THRESH) {
                                                        j0valid[iij0] = false;
                                                        _ij0[1] = iij0;
                                                        break;
                                                      }
                                                    }
                                                    j0 = j0array[ij0];
                                                    cj0 = cj0array[ij0];
                                                    sj0 = sj0array[ij0];
                                                    {
                                                      IkReal evalcond[3];
                                                      IkReal x658 = IKcos(j0);
                                                      IkReal x659 = IKsin(j0);
                                                      IkReal x660 = ((1.0) * x658);
                                                      IkReal x661 = (py * x659);
                                                      evalcond[0] = ((0.02) + ((-1.0) * py * x660) + (px * x659));
                                                      evalcond[1] = ((0.0213793103448276) + ((-1.0) * x661) + ((-1.0) * px * x660) + ((-1.72413793103448) * pp));
                                                      evalcond[2] = ((0.0124) + ((-0.58) * px * x658) + ((-1.0) * pp) + ((-0.58) * x661));
                                                      if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                          IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH) {
                                                        continue;
                                                      }
                                                    }

                                                    rotationfunction0(solutions);
                                                  }
                                                }
                                              }
                                            }
                                          }
                                        } while (0);
                                        if (bgotonextstatement) {
                                          bool bgotonextstatement = true;
                                          do {
                                            if (1) {
                                              bgotonextstatement = false;
                                              continue; // branch miss [j0]
                                            }
                                          } while (0);
                                          if (bgotonextstatement) {
                                          }
                                        }
                                      }
                                    }
                                  }
                                }

                              } else {
                                {
                                  IkReal j0array[1], cj0array[1], sj0array[1];
                                  bool j0valid[1] = {false};
                                  _nj0 = 1;
                                  IkReal x662 = ((15.5) * cj2);
                                  IkReal x663 = ((50.0) * sj1);
                                  IkReal x664 = ((50.0) * cj1 * pz);
                                  CheckValue<IkReal> x665 = IKPowWithIntegerCheck(IKsign((pp * x663) + ((-1.0) * x663 * (pz * pz))), -1);
                                  if (!x665.valid)
                                    continue;
                                  CheckValue<IkReal> x666 = IKatan2WithCheck(IkReal(((14.5) * py) + (py * x662) + ((-1.0) * py * x664) + ((-1.0) * px * sj1)),
                                                                             IkReal((px * x662) + ((14.5) * px) + ((-1.0) * px * x664) + (py * sj1)), IKFAST_ATAN2_MAGTHRESH);
                                  if (!x666.valid)
                                    continue;
                                  j0array[0] = ((-1.5707963267949) + ((1.5707963267949) * (x665.value)) + (x666.value));
                                  sj0array[0] = IKsin(j0array[0]);
                                  cj0array[0] = IKcos(j0array[0]);
                                  if (j0array[0] > IKPI)
                                    j0array[0] -= IK2PI;
                                  else if (j0array[0] < -IKPI)
                                    j0array[0] += IK2PI;
                                  j0valid[0] = true;
                                  for (int ij0 = 0; ij0 < 1; ++ij0) {
                                    if (!j0valid[ij0])
                                      continue;
                                    _ij0[0] = ij0;
                                    _ij0[1] = -1;
                                    for (int iij0 = ij0 + 1; iij0 < 1; ++iij0) {
                                      if (j0valid[iij0] && IKabs(cj0array[ij0] - cj0array[iij0]) < IKFAST_SOLUTION_THRESH &&
                                          IKabs(sj0array[ij0] - sj0array[iij0]) < IKFAST_SOLUTION_THRESH) {
                                        j0valid[iij0] = false;
                                        _ij0[1] = iij0;
                                        break;
                                      }
                                    }
                                    j0 = j0array[ij0];
                                    cj0 = cj0array[ij0];
                                    sj0 = sj0array[ij0];
                                    {
                                      IkReal evalcond[5];
                                      IkReal x667 = IKcos(j0);
                                      IkReal x668 = IKsin(j0);
                                      IkReal x669 = (cj1 * pz);
                                      IkReal x670 = ((0.31) * cj2);
                                      IkReal x671 = ((1.0) * py);
                                      IkReal x672 = ((0.31) * sj2);
                                      IkReal x673 = (px * x667);
                                      IkReal x674 = (py * sj1 * x668);
                                      evalcond[0] = ((0.02) + (px * x668) + ((-1.0) * x667 * x671));
                                      evalcond[1] = ((-0.29) + x669 + x674 + (sj1 * x673) + ((-1.0) * x670));
                                      evalcond[2] = (x672 + (pz * sj1) + ((-1.0) * cj1 * x668 * x671) + ((-1.0) * cj1 * x673));
                                      evalcond[3] = ((0.0124) + ((0.58) * x669) + ((0.58) * sj1 * x673) + ((-1.0) * pp) + ((0.58) * x674));
                                      evalcond[4] = (((0.29) * sj1) + ((-1.0) * x668 * x671) + ((-1.0) * x673) + (sj1 * x670) + (cj1 * x672));
                                      if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                          IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                          IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH) {
                                        continue;
                                      }
                                    }

                                    rotationfunction0(solutions);
                                  }
                                }
                              }
                            }

                          } else {
                            {
                              IkReal j0array[1], cj0array[1], sj0array[1];
                              bool j0valid[1] = {false};
                              _nj0 = 1;
                              IkReal x675 = (py * sj1);
                              IkReal x676 = ((15.5) * cj2);
                              IkReal x677 = (px * sj1);
                              IkReal x678 = ((15.5) * cj1 * sj2);
                              CheckValue<IkReal> x679 = IKPowWithIntegerCheck(IKsign(((-50.0) * (pz * pz)) + ((50.0) * pp)), -1);
                              if (!x679.valid)
                                continue;
                              CheckValue<IkReal> x680 = IKatan2WithCheck(IkReal((py * x678) + ((14.5) * x675) + ((-1.0) * px) + (x675 * x676)),
                                                                         IkReal(((14.5) * x677) + (x676 * x677) + py + (px * x678)), IKFAST_ATAN2_MAGTHRESH);
                              if (!x680.valid)
                                continue;
                              j0array[0] = ((-1.5707963267949) + ((1.5707963267949) * (x679.value)) + (x680.value));
                              sj0array[0] = IKsin(j0array[0]);
                              cj0array[0] = IKcos(j0array[0]);
                              if (j0array[0] > IKPI)
                                j0array[0] -= IK2PI;
                              else if (j0array[0] < -IKPI)
                                j0array[0] += IK2PI;
                              j0valid[0] = true;
                              for (int ij0 = 0; ij0 < 1; ++ij0) {
                                if (!j0valid[ij0])
                                  continue;
                                _ij0[0] = ij0;
                                _ij0[1] = -1;
                                for (int iij0 = ij0 + 1; iij0 < 1; ++iij0) {
                                  if (j0valid[iij0] && IKabs(cj0array[ij0] - cj0array[iij0]) < IKFAST_SOLUTION_THRESH &&
                                      IKabs(sj0array[ij0] - sj0array[iij0]) < IKFAST_SOLUTION_THRESH) {
                                    j0valid[iij0] = false;
                                    _ij0[1] = iij0;
                                    break;
                                  }
                                }
                                j0 = j0array[ij0];
                                cj0 = cj0array[ij0];
                                sj0 = sj0array[ij0];
                                {
                                  IkReal evalcond[5];
                                  IkReal x681 = IKcos(j0);
                                  IkReal x682 = IKsin(j0);
                                  IkReal x683 = (cj1 * pz);
                                  IkReal x684 = ((0.31) * cj2);
                                  IkReal x685 = ((1.0) * py);
                                  IkReal x686 = ((0.31) * sj2);
                                  IkReal x687 = (px * x681);
                                  IkReal x688 = (py * sj1 * x682);
                                  evalcond[0] = ((0.02) + (px * x682) + ((-1.0) * x681 * x685));
                                  evalcond[1] = ((-0.29) + ((-1.0) * x684) + x688 + x683 + (sj1 * x687));
                                  evalcond[2] = (((-1.0) * cj1 * x682 * x685) + x686 + (pz * sj1) + ((-1.0) * cj1 * x687));
                                  evalcond[3] = ((0.0124) + ((-1.0) * pp) + ((0.58) * x683) + ((0.58) * x688) + ((0.58) * sj1 * x687));
                                  evalcond[4] = ((cj1 * x686) + ((0.29) * sj1) + ((-1.0) * x682 * x685) + (sj1 * x684) + ((-1.0) * x687));
                                  if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH ||
                                      IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH) {
                                    continue;
                                  }
                                }

                                rotationfunction0(solutions);
                              }
                            }
                          }
                        }

                      } else {
                        {
                          IkReal j0array[1], cj0array[1], sj0array[1];
                          bool j0valid[1] = {false};
                          _nj0 = 1;
                          IkReal x689 = ((15.5) * sj2);
                          IkReal x690 = ((50.0) * cj1);
                          IkReal x691 = ((50.0) * pz * sj1);
                          CheckValue<IkReal> x692 =
                            IKatan2WithCheck(IkReal((py * x691) + ((-1.0) * cj1 * px) + (py * x689)), IkReal((px * x689) + (cj1 * py) + (px * x691)), IKFAST_ATAN2_MAGTHRESH);
                          if (!x692.valid)
                            continue;
                          CheckValue<IkReal> x693 = IKPowWithIntegerCheck(IKsign((pp * x690) + ((-1.0) * x690 * (pz * pz))), -1);
                          if (!x693.valid)
                            continue;
                          j0array[0] = ((-1.5707963267949) + (x692.value) + ((1.5707963267949) * (x693.value)));
                          sj0array[0] = IKsin(j0array[0]);
                          cj0array[0] = IKcos(j0array[0]);
                          if (j0array[0] > IKPI)
                            j0array[0] -= IK2PI;
                          else if (j0array[0] < -IKPI)
                            j0array[0] += IK2PI;
                          j0valid[0] = true;
                          for (int ij0 = 0; ij0 < 1; ++ij0) {
                            if (!j0valid[ij0])
                              continue;
                            _ij0[0] = ij0;
                            _ij0[1] = -1;
                            for (int iij0 = ij0 + 1; iij0 < 1; ++iij0) {
                              if (j0valid[iij0] && IKabs(cj0array[ij0] - cj0array[iij0]) < IKFAST_SOLUTION_THRESH &&
                                  IKabs(sj0array[ij0] - sj0array[iij0]) < IKFAST_SOLUTION_THRESH) {
                                j0valid[iij0] = false;
                                _ij0[1] = iij0;
                                break;
                              }
                            }
                            j0 = j0array[ij0];
                            cj0 = cj0array[ij0];
                            sj0 = sj0array[ij0];
                            {
                              IkReal evalcond[5];
                              IkReal x694 = IKcos(j0);
                              IkReal x695 = IKsin(j0);
                              IkReal x696 = (cj1 * pz);
                              IkReal x697 = ((0.31) * cj2);
                              IkReal x698 = ((1.0) * py);
                              IkReal x699 = ((0.31) * sj2);
                              IkReal x700 = (px * x694);
                              IkReal x701 = (py * sj1 * x695);
                              evalcond[0] = ((0.02) + (px * x695) + ((-1.0) * x694 * x698));
                              evalcond[1] = ((-0.29) + x696 + ((-1.0) * x697) + (sj1 * x700) + x701);
                              evalcond[2] = (((-1.0) * cj1 * x700) + x699 + (pz * sj1) + ((-1.0) * cj1 * x695 * x698));
                              evalcond[3] = ((0.0124) + ((0.58) * x696) + ((0.58) * sj1 * x700) + ((0.58) * x701) + ((-1.0) * pp));
                              evalcond[4] = ((cj1 * x699) + (sj1 * x697) + ((0.29) * sj1) + ((-1.0) * x700) + ((-1.0) * x695 * x698));
                              if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH ||
                                  IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH) {
                                continue;
                              }
                            }

                            rotationfunction0(solutions);
                          }
                        }
                      }
                    }
                  }
                }
              }
            }

          } else {
            {
              IkReal j0array[2], cj0array[2], sj0array[2];
              bool j0valid[2] = {false};
              _nj0 = 2;
              CheckValue<IkReal> x704 = IKatan2WithCheck(IkReal((-1.0) * py), IkReal(px), IKFAST_ATAN2_MAGTHRESH);
              if (!x704.valid)
                continue;
              IkReal x702 = ((1.0) * (x704.value));
              if (((px * px) + (py * py)) < -0.00001)
                continue;
              CheckValue<IkReal> x705 = IKPowWithIntegerCheck(IKabs(IKsqrt((px * px) + (py * py))), -1);
              if (!x705.valid)
                continue;
              if (((0.02) * (x705.value)) < -1 - IKFAST_SINCOS_THRESH || ((0.02) * (x705.value)) > 1 + IKFAST_SINCOS_THRESH) {
                continue;
              }
              IkReal x703 = IKasin((0.02) * (x705.value));
              j0array[0] = (((-1.0) * x703) + ((-1.0) * x702));
              sj0array[0] = IKsin(j0array[0]);
              cj0array[0] = IKcos(j0array[0]);
              j0array[1] = ((3.14159265358979) + ((-1.0) * x702) + x703);
              sj0array[1] = IKsin(j0array[1]);
              cj0array[1] = IKcos(j0array[1]);
              if (j0array[0] > IKPI)
                j0array[0] -= IK2PI;
              else if (j0array[0] < -IKPI)
                j0array[0] += IK2PI;
              j0valid[0] = true;
              if (j0array[1] > IKPI)
                j0array[1] -= IK2PI;
              else if (j0array[1] < -IKPI)
                j0array[1] += IK2PI;
              j0valid[1] = true;
              for (int ij0 = 0; ij0 < 2; ++ij0) {
                if (!j0valid[ij0])
                  continue;
                _ij0[0] = ij0;
                _ij0[1] = -1;
                for (int iij0 = ij0 + 1; iij0 < 2; ++iij0) {
                  if (j0valid[iij0] && IKabs(cj0array[ij0] - cj0array[iij0]) < IKFAST_SOLUTION_THRESH && IKabs(sj0array[ij0] - sj0array[iij0]) < IKFAST_SOLUTION_THRESH) {
                    j0valid[iij0] = false;
                    _ij0[1] = iij0;
                    break;
                  }
                }
                j0 = j0array[ij0];
                cj0 = cj0array[ij0];
                sj0 = sj0array[ij0];

                {
                  IkReal j1eval[3];
                  IkReal x706 = ((3100.0) * sj2);
                  IkReal x707 = (cj0 * px);
                  IkReal x708 = ((3100.0) * cj2);
                  IkReal x709 = (py * sj0);
                  j1eval[0] = ((-1.00222469410456) + ((-1.0) * cj2));
                  j1eval[1] = IKsign((-1802.0) + ((-1798.0) * cj2));
                  j1eval[2] = ((IKabs(((-1.0) * x707 * x708) + ((-2900.0) * x709) + ((-2900.0) * x707) + ((-1.0) * x708 * x709) + (pz * x706))) +
                               (IKabs(((-2900.0) * pz) + ((-1.0) * pz * x708) + ((-1.0) * x706 * x707) + ((-1.0) * x706 * x709))));
                  if (IKabs(j1eval[0]) < 0.0000010000000000 || IKabs(j1eval[1]) < 0.0000010000000000 || IKabs(j1eval[2]) < 0.0000010000000000) {
                    {
                      IkReal j1eval[3];
                      IkReal x710 = (cj0 * px);
                      IkReal x711 = ((100.0) * pz);
                      IkReal x712 = (pz * sj2);
                      IkReal x713 = (py * sj0);
                      IkReal x714 = (cj2 * x713);
                      j1eval[0] = (((-1.06896551724138) * cj2 * x710) + ((-1.0) * x713) + ((-1.0) * x710) + ((-1.06896551724138) * x714) + ((-1.06896551724138) * x712));
                      j1eval[1] = ((IKabs(((-1.0) * x711 * x713) + ((-1.0) * x710 * x711) + ((-9.61) * cj2 * sj2) + ((-8.99) * sj2))) +
                                   (IKabs((-8.41) + ((-17.98) * cj2) + ((-9.61) * (cj2 * cj2)) + (pz * x711))));
                      j1eval[2] = IKsign(((-31.0) * x712) + ((-31.0) * x714) + ((-31.0) * cj2 * x710) + ((-29.0) * x710) + ((-29.0) * x713));
                      if (IKabs(j1eval[0]) < 0.0000010000000000 || IKabs(j1eval[1]) < 0.0000010000000000 || IKabs(j1eval[2]) < 0.0000010000000000) {
                        {
                          IkReal j1eval[3];
                          IkReal x715 = (py * sj0);
                          IkReal x716 = ((2900.0) * pz);
                          IkReal x717 = (cj0 * px);
                          IkReal x718 = ((1.06896551724138) * cj2);
                          IkReal x719 = ((1550.0) * pp);
                          IkReal x720 = ((899.0) * cj2);
                          IkReal x721 = (pz * sj2);
                          j1eval[0] = (((-1.0) * x717) + ((-1.0) * x715) + ((-1.0) * x717 * x718) + ((-1.06896551724138) * x721) + ((-1.0) * x715 * x718));
                          j1eval[1] = IKsign(((-841.0) * x715) + ((-841.0) * x717) + ((-1.0) * x717 * x720) + ((-1.0) * x715 * x720) + ((-899.0) * x721));
                          j1eval[2] = ((IKabs((17.98) + ((-1.0) * cj2 * x719) + ((-1450.0) * pp) + ((19.22) * cj2) + (pz * x716))) +
                                       (IKabs(((-1.0) * x716 * x717) + ((19.22) * sj2) + ((-1.0) * sj2 * x719) + ((-1.0) * x715 * x716))));
                          if (IKabs(j1eval[0]) < 0.0000010000000000 || IKabs(j1eval[1]) < 0.0000010000000000 || IKabs(j1eval[2]) < 0.0000010000000000) {
                            continue; // no branches [j1]

                          } else {
                            {
                              IkReal j1array[1], cj1array[1], sj1array[1];
                              bool j1valid[1] = {false};
                              _nj1 = 1;
                              IkReal x722 = (py * sj0);
                              IkReal x723 = ((2900.0) * pz);
                              IkReal x724 = (cj0 * px);
                              IkReal x725 = ((1550.0) * pp);
                              IkReal x726 = ((899.0) * cj2);
                              CheckValue<IkReal> x727 =
                                IKPowWithIntegerCheck(IKsign(((-1.0) * x724 * x726) + ((-841.0) * x724) + ((-841.0) * x722) + ((-1.0) * x722 * x726) + ((-899.0) * pz * sj2)), -1);
                              if (!x727.valid)
                                continue;
                              CheckValue<IkReal> x728 =
                                IKatan2WithCheck(IkReal((17.98) + ((-1.0) * cj2 * x725) + ((-1450.0) * pp) + ((19.22) * cj2) + (pz * x723)),
                                                 IkReal(((-1.0) * x723 * x724) + ((-1.0) * x722 * x723) + ((-1.0) * sj2 * x725) + ((19.22) * sj2)), IKFAST_ATAN2_MAGTHRESH);
                              if (!x728.valid)
                                continue;
                              j1array[0] = ((-1.5707963267949) + ((1.5707963267949) * (x727.value)) + (x728.value));
                              sj1array[0] = IKsin(j1array[0]);
                              cj1array[0] = IKcos(j1array[0]);
                              if (j1array[0] > IKPI)
                                j1array[0] -= IK2PI;
                              else if (j1array[0] < -IKPI)
                                j1array[0] += IK2PI;
                              j1valid[0] = true;
                              for (int ij1 = 0; ij1 < 1; ++ij1) {
                                if (!j1valid[ij1])
                                  continue;
                                _ij1[0] = ij1;
                                _ij1[1] = -1;
                                for (int iij1 = ij1 + 1; iij1 < 1; ++iij1) {
                                  if (j1valid[iij1] && IKabs(cj1array[ij1] - cj1array[iij1]) < IKFAST_SOLUTION_THRESH &&
                                      IKabs(sj1array[ij1] - sj1array[iij1]) < IKFAST_SOLUTION_THRESH) {
                                    j1valid[iij1] = false;
                                    _ij1[1] = iij1;
                                    break;
                                  }
                                }
                                j1 = j1array[ij1];
                                cj1 = cj1array[ij1];
                                sj1 = sj1array[ij1];
                                {
                                  IkReal evalcond[5];
                                  IkReal x729 = IKcos(j1);
                                  IkReal x730 = IKsin(j1);
                                  IkReal x731 = (py * sj0);
                                  IkReal x732 = ((0.31) * cj2);
                                  IkReal x733 = (cj0 * px);
                                  IkReal x734 = ((0.31) * sj2);
                                  IkReal x735 = ((0.31) * x729);
                                  IkReal x736 = ((1.0) * x729);
                                  IkReal x737 = (pz * x729);
                                  IkReal x738 = ((0.58) * x730);
                                  evalcond[0] = (((-0.29) * x729) + pz + ((-1.0) * x729 * x732) + (x730 * x734));
                                  evalcond[1] = ((-0.29) + ((-1.0) * x732) + x737 + (x730 * x731) + (x730 * x733));
                                  evalcond[2] = (((-1.0) * x731 * x736) + ((-1.0) * x733 * x736) + (pz * x730) + x734);
                                  evalcond[3] = ((0.0124) + (x731 * x738) + ((0.58) * x737) + ((-1.0) * pp) + (x733 * x738));
                                  evalcond[4] = ((x729 * x734) + ((0.29) * x730) + ((-1.0) * x733) + ((-1.0) * x731) + (x730 * x732));
                                  if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH ||
                                      IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH) {
                                    continue;
                                  }
                                }

                                rotationfunction0(solutions);
                              }
                            }
                          }
                        }

                      } else {
                        {
                          IkReal j1array[1], cj1array[1], sj1array[1];
                          bool j1valid[1] = {false};
                          _nj1 = 1;
                          IkReal x739 = (cj0 * px);
                          IkReal x740 = ((100.0) * pz);
                          IkReal x741 = ((31.0) * cj2);
                          IkReal x742 = (py * sj0);
                          CheckValue<IkReal> x743 =
                            IKPowWithIntegerCheck(IKsign(((-31.0) * pz * sj2) + ((-29.0) * x742) + ((-1.0) * x741 * x742) + ((-29.0) * x739) + ((-1.0) * x739 * x741)), -1);
                          if (!x743.valid)
                            continue;
                          CheckValue<IkReal> x744 =
                            IKatan2WithCheck(IkReal((-8.41) + ((-17.98) * cj2) + ((-9.61) * (cj2 * cj2)) + (pz * x740)),
                                             IkReal(((-9.61) * cj2 * sj2) + ((-1.0) * x740 * x742) + ((-1.0) * x739 * x740) + ((-8.99) * sj2)), IKFAST_ATAN2_MAGTHRESH);
                          if (!x744.valid)
                            continue;
                          j1array[0] = ((-1.5707963267949) + ((1.5707963267949) * (x743.value)) + (x744.value));
                          sj1array[0] = IKsin(j1array[0]);
                          cj1array[0] = IKcos(j1array[0]);
                          if (j1array[0] > IKPI)
                            j1array[0] -= IK2PI;
                          else if (j1array[0] < -IKPI)
                            j1array[0] += IK2PI;
                          j1valid[0] = true;
                          for (int ij1 = 0; ij1 < 1; ++ij1) {
                            if (!j1valid[ij1])
                              continue;
                            _ij1[0] = ij1;
                            _ij1[1] = -1;
                            for (int iij1 = ij1 + 1; iij1 < 1; ++iij1) {
                              if (j1valid[iij1] && IKabs(cj1array[ij1] - cj1array[iij1]) < IKFAST_SOLUTION_THRESH &&
                                  IKabs(sj1array[ij1] - sj1array[iij1]) < IKFAST_SOLUTION_THRESH) {
                                j1valid[iij1] = false;
                                _ij1[1] = iij1;
                                break;
                              }
                            }
                            j1 = j1array[ij1];
                            cj1 = cj1array[ij1];
                            sj1 = sj1array[ij1];
                            {
                              IkReal evalcond[5];
                              IkReal x745 = IKcos(j1);
                              IkReal x746 = IKsin(j1);
                              IkReal x747 = (py * sj0);
                              IkReal x748 = ((0.31) * cj2);
                              IkReal x749 = (cj0 * px);
                              IkReal x750 = ((0.31) * sj2);
                              IkReal x751 = ((0.31) * x745);
                              IkReal x752 = ((1.0) * x745);
                              IkReal x753 = (pz * x745);
                              IkReal x754 = ((0.58) * x746);
                              evalcond[0] = (((-1.0) * x745 * x748) + (x746 * x750) + pz + ((-0.29) * x745));
                              evalcond[1] = ((-0.29) + (x746 * x747) + (x746 * x749) + x753 + ((-1.0) * x748));
                              evalcond[2] = (((-1.0) * x747 * x752) + x750 + ((-1.0) * x749 * x752) + (pz * x746));
                              evalcond[3] = ((0.0124) + (x747 * x754) + ((-1.0) * pp) + (x749 * x754) + ((0.58) * x753));
                              evalcond[4] = ((x745 * x750) + (x746 * x748) + ((0.29) * x746) + ((-1.0) * x747) + ((-1.0) * x749));
                              if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH ||
                                  IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH) {
                                continue;
                              }
                            }

                            rotationfunction0(solutions);
                          }
                        }
                      }
                    }

                  } else {
                    {
                      IkReal j1array[1], cj1array[1], sj1array[1];
                      bool j1valid[1] = {false};
                      _nj1 = 1;
                      IkReal x755 = ((3100.0) * cj2);
                      IkReal x756 = (py * sj0);
                      IkReal x757 = ((3100.0) * sj2);
                      IkReal x758 = (cj0 * px);
                      CheckValue<IkReal> x759 = IKPowWithIntegerCheck(IKsign((-1802.0) + ((-1798.0) * cj2)), -1);
                      if (!x759.valid)
                        continue;
                      CheckValue<IkReal> x760 =
                        IKatan2WithCheck(IkReal((pz * x757) + ((-1.0) * x755 * x758) + ((-1.0) * x755 * x756) + ((-2900.0) * x756) + ((-2900.0) * x758)),
                                         IkReal(((-2900.0) * pz) + ((-1.0) * x756 * x757) + ((-1.0) * pz * x755) + ((-1.0) * x757 * x758)), IKFAST_ATAN2_MAGTHRESH);
                      if (!x760.valid)
                        continue;
                      j1array[0] = ((-1.5707963267949) + ((1.5707963267949) * (x759.value)) + (x760.value));
                      sj1array[0] = IKsin(j1array[0]);
                      cj1array[0] = IKcos(j1array[0]);
                      if (j1array[0] > IKPI)
                        j1array[0] -= IK2PI;
                      else if (j1array[0] < -IKPI)
                        j1array[0] += IK2PI;
                      j1valid[0] = true;
                      for (int ij1 = 0; ij1 < 1; ++ij1) {
                        if (!j1valid[ij1])
                          continue;
                        _ij1[0] = ij1;
                        _ij1[1] = -1;
                        for (int iij1 = ij1 + 1; iij1 < 1; ++iij1) {
                          if (j1valid[iij1] && IKabs(cj1array[ij1] - cj1array[iij1]) < IKFAST_SOLUTION_THRESH && IKabs(sj1array[ij1] - sj1array[iij1]) < IKFAST_SOLUTION_THRESH) {
                            j1valid[iij1] = false;
                            _ij1[1] = iij1;
                            break;
                          }
                        }
                        j1 = j1array[ij1];
                        cj1 = cj1array[ij1];
                        sj1 = sj1array[ij1];
                        {
                          IkReal evalcond[5];
                          IkReal x761 = IKcos(j1);
                          IkReal x762 = IKsin(j1);
                          IkReal x763 = (py * sj0);
                          IkReal x764 = ((0.31) * cj2);
                          IkReal x765 = (cj0 * px);
                          IkReal x766 = ((0.31) * sj2);
                          IkReal x767 = ((0.31) * x761);
                          IkReal x768 = ((1.0) * x761);
                          IkReal x769 = (pz * x761);
                          IkReal x770 = ((0.58) * x762);
                          evalcond[0] = ((x762 * x766) + pz + ((-0.29) * x761) + ((-1.0) * x761 * x764));
                          evalcond[1] = ((-0.29) + ((-1.0) * x764) + (x762 * x763) + (x762 * x765) + x769);
                          evalcond[2] = (((-1.0) * x763 * x768) + ((-1.0) * x765 * x768) + (pz * x762) + x766);
                          evalcond[3] = ((0.0124) + ((-1.0) * pp) + (x765 * x770) + ((0.58) * x769) + (x763 * x770));
                          evalcond[4] = (((0.29) * x762) + (x761 * x766) + (x762 * x764) + ((-1.0) * x763) + ((-1.0) * x765));
                          if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH ||
                              IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH) {
                            continue;
                          }
                        }

                        rotationfunction0(solutions);
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return solutions.GetNumSolutions() > 0;
}
inline void rotationfunction0(IkSolutionListBase<IkReal> &solutions) {
  for (int rotationiter = 0; rotationiter < 1; ++rotationiter) {
    IkReal x79 = (cj0 * r00);
    IkReal x80 = (cj0 * r01);
    IkReal x81 = ((1.0) * sj1);
    IkReal x82 = ((1.0) * sj0);
    IkReal x83 = (r11 * sj0);
    IkReal x84 = (cj1 * sj2);
    IkReal x85 = (cj0 * r02);
    IkReal x86 = (((-1.0) * sj2 * x81) + (cj1 * cj2));
    IkReal x87 = ((cj2 * sj1) + x84);
    IkReal x88 = (cj0 * x86);
    IkReal x89 = (sj0 * x86);
    IkReal x90 = (sj0 * x87);
    IkReal x91 = (((-1.0) * x84) + ((-1.0) * cj2 * x81));
    new_r00 = ((x79 * x86) + (r10 * x89) + (r20 * x91));
    new_r01 = ((r21 * x91) + (x83 * x86) + (x80 * x86));
    new_r02 = ((r12 * x89) + (x85 * x86) + (r22 * x91));
    new_r10 = (((-1.0) * r00 * x82) + (cj0 * r10));
    new_r11 = ((cj0 * r11) + ((-1.0) * r01 * x82));
    new_r12 = (((-1.0) * r02 * x82) + (cj0 * r12));
    new_r20 = ((x79 * x87) + (r10 * x90) + (r20 * x86));
    new_r21 = ((r21 * x86) + (x83 * x87) + (x80 * x87));
    new_r22 = ((r12 * x90) + (x85 * x87) + (r22 * x86));
    {
      IkReal j4array[2], cj4array[2], sj4array[2];
      bool j4valid[2] = {false};
      _nj4 = 2;
      cj4array[0] = new_r22;
      if (cj4array[0] >= -1 - IKFAST_SINCOS_THRESH && cj4array[0] <= 1 + IKFAST_SINCOS_THRESH) {
        j4valid[0] = j4valid[1] = true;
        j4array[0] = IKacos(cj4array[0]);
        sj4array[0] = IKsin(j4array[0]);
        cj4array[1] = cj4array[0];
        j4array[1] = -j4array[0];
        sj4array[1] = -sj4array[0];
      } else if (isnan(cj4array[0])) {
        // probably any value will work
        j4valid[0] = true;
        cj4array[0] = 1;
        sj4array[0] = 0;
        j4array[0] = 0;
      }
      for (int ij4 = 0; ij4 < 2; ++ij4) {
        if (!j4valid[ij4])
          continue;
        _ij4[0] = ij4;
        _ij4[1] = -1;
        for (int iij4 = ij4 + 1; iij4 < 2; ++iij4) {
          if (j4valid[iij4] && IKabs(cj4array[ij4] - cj4array[iij4]) < IKFAST_SOLUTION_THRESH && IKabs(sj4array[ij4] - sj4array[iij4]) < IKFAST_SOLUTION_THRESH) {
            j4valid[iij4] = false;
            _ij4[1] = iij4;
            break;
          }
        }
        j4 = j4array[ij4];
        cj4 = cj4array[ij4];
        sj4 = sj4array[ij4];

        {
          IkReal j3eval[3];
          j3eval[0] = sj4;
          j3eval[1] = IKsign(sj4);
          j3eval[2] = ((IKabs(new_r12)) + (IKabs(new_r02)));
          if (IKabs(j3eval[0]) < 0.0000010000000000 || IKabs(j3eval[1]) < 0.0000010000000000 || IKabs(j3eval[2]) < 0.0000010000000000) {
            {
              IkReal j5eval[3];
              j5eval[0] = sj4;
              j5eval[1] = IKsign(sj4);
              j5eval[2] = ((IKabs(new_r20)) + (IKabs(new_r21)));
              if (IKabs(j5eval[0]) < 0.0000010000000000 || IKabs(j5eval[1]) < 0.0000010000000000 || IKabs(j5eval[2]) < 0.0000010000000000) {
                {
                  IkReal j3eval[2];
                  j3eval[0] = new_r12;
                  j3eval[1] = sj4;
                  if (IKabs(j3eval[0]) < 0.0000010000000000 || IKabs(j3eval[1]) < 0.0000010000000000) {
                    {
                      IkReal evalcond[5];
                      bool bgotonextstatement = true;
                      do {
                        evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs(j4))), 6.28318530717959)));
                        evalcond[1] = new_r20;
                        evalcond[2] = new_r02;
                        evalcond[3] = new_r12;
                        evalcond[4] = new_r21;
                        if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000 && IKabs(evalcond[2]) < 0.0000050000000000 &&
                            IKabs(evalcond[3]) < 0.0000050000000000 && IKabs(evalcond[4]) < 0.0000050000000000) {
                          bgotonextstatement = false;
                          IkReal j5mul = 1;
                          j5 = 0;
                          j3mul = -1.0;
                          if (IKabs((-1.0) * new_r01) < IKFAST_ATAN2_MAGTHRESH && IKabs(new_r00) < IKFAST_ATAN2_MAGTHRESH &&
                              IKabs(IKsqr((-1.0) * new_r01) + IKsqr(new_r00) - 1) <= IKFAST_SINCOS_THRESH) {
                            continue;
                          }
                          j3 = IKatan2(((-1.0) * new_r01), new_r00);
                          {
                            std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                            vinfos[0].jointtype = 1;
                            vinfos[0].foffset = j0;
                            vinfos[0].indices[0] = _ij0[0];
                            vinfos[0].indices[1] = _ij0[1];
                            vinfos[0].maxsolutions = _nj0;
                            vinfos[1].jointtype = 1;
                            vinfos[1].foffset = j1;
                            vinfos[1].indices[0] = _ij1[0];
                            vinfos[1].indices[1] = _ij1[1];
                            vinfos[1].maxsolutions = _nj1;
                            vinfos[2].jointtype = 1;
                            vinfos[2].foffset = j2;
                            vinfos[2].indices[0] = _ij2[0];
                            vinfos[2].indices[1] = _ij2[1];
                            vinfos[2].maxsolutions = _nj2;
                            vinfos[3].jointtype = 1;
                            vinfos[3].foffset = j3;
                            vinfos[3].fmul = j3mul;
                            vinfos[3].freeind = 0;
                            vinfos[3].maxsolutions = 0;
                            vinfos[4].jointtype = 1;
                            vinfos[4].foffset = j4;
                            vinfos[4].indices[0] = _ij4[0];
                            vinfos[4].indices[1] = _ij4[1];
                            vinfos[4].maxsolutions = _nj4;
                            vinfos[5].jointtype = 1;
                            vinfos[5].foffset = j5;
                            vinfos[5].fmul = j5mul;
                            vinfos[5].freeind = 0;
                            vinfos[5].maxsolutions = 0;
                            std::vector<int> vfree(1);
                            vfree[0] = 5;
                            solutions.AddSolution(vinfos, vfree);
                          }
                        }
                      } while (0);
                      if (bgotonextstatement) {
                        bool bgotonextstatement = true;
                        do {
                          evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs((-3.14159265358979) + j4))), 6.28318530717959)));
                          evalcond[1] = new_r20;
                          evalcond[2] = new_r02;
                          evalcond[3] = new_r12;
                          evalcond[4] = new_r21;
                          if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000 && IKabs(evalcond[2]) < 0.0000050000000000 &&
                              IKabs(evalcond[3]) < 0.0000050000000000 && IKabs(evalcond[4]) < 0.0000050000000000) {
                            bgotonextstatement = false;
                            IkReal j5mul = 1;
                            j5 = 0;
                            j3mul = 1.0;
                            if (IKabs((-1.0) * new_r01) < IKFAST_ATAN2_MAGTHRESH && IKabs((-1.0) * new_r00) < IKFAST_ATAN2_MAGTHRESH &&
                                IKabs(IKsqr((-1.0) * new_r01) + IKsqr((-1.0) * new_r00) - 1) <= IKFAST_SINCOS_THRESH) {
                              continue;
                            }
                            j3 = IKatan2(((-1.0) * new_r01), ((-1.0) * new_r00));
                            {
                              std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                              vinfos[0].jointtype = 1;
                              vinfos[0].foffset = j0;
                              vinfos[0].indices[0] = _ij0[0];
                              vinfos[0].indices[1] = _ij0[1];
                              vinfos[0].maxsolutions = _nj0;
                              vinfos[1].jointtype = 1;
                              vinfos[1].foffset = j1;
                              vinfos[1].indices[0] = _ij1[0];
                              vinfos[1].indices[1] = _ij1[1];
                              vinfos[1].maxsolutions = _nj1;
                              vinfos[2].jointtype = 1;
                              vinfos[2].foffset = j2;
                              vinfos[2].indices[0] = _ij2[0];
                              vinfos[2].indices[1] = _ij2[1];
                              vinfos[2].maxsolutions = _nj2;
                              vinfos[3].jointtype = 1;
                              vinfos[3].foffset = j3;
                              vinfos[3].fmul = j3mul;
                              vinfos[3].freeind = 0;
                              vinfos[3].maxsolutions = 0;
                              vinfos[4].jointtype = 1;
                              vinfos[4].foffset = j4;
                              vinfos[4].indices[0] = _ij4[0];
                              vinfos[4].indices[1] = _ij4[1];
                              vinfos[4].maxsolutions = _nj4;
                              vinfos[5].jointtype = 1;
                              vinfos[5].foffset = j5;
                              vinfos[5].fmul = j5mul;
                              vinfos[5].freeind = 0;
                              vinfos[5].maxsolutions = 0;
                              std::vector<int> vfree(1);
                              vfree[0] = 5;
                              solutions.AddSolution(vinfos, vfree);
                            }
                          }
                        } while (0);
                        if (bgotonextstatement) {
                          bool bgotonextstatement = true;
                          do {
                            evalcond[0] = ((IKabs(new_r12)) + (IKabs(new_r02)));
                            if (IKabs(evalcond[0]) < 0.0000050000000000) {
                              bgotonextstatement = false;
                              {
                                IkReal j3eval[1];
                                new_r02 = 0;
                                new_r12 = 0;
                                new_r20 = 0;
                                new_r21 = 0;
                                IkReal x92 = new_r22 * new_r22;
                                IkReal x93 = ((16.0) * new_r10);
                                IkReal x94 = ((16.0) * new_r01);
                                IkReal x95 = ((16.0) * new_r22);
                                IkReal x96 = ((8.0) * new_r11);
                                IkReal x97 = ((8.0) * new_r00);
                                IkReal x98 = (x92 * x93);
                                IkReal x99 = (x92 * x94);
                                j3eval[0] =
                                  ((IKabs(x94 + ((-1.0) * x99))) + (IKabs(((16.0) * new_r00) + (new_r11 * x95) + ((-32.0) * new_r00 * x92))) + (IKabs(x99 + ((-1.0) * x94))) +
                                   (IKabs(((32.0) * new_r11) + ((-16.0) * new_r11 * x92) + ((-1.0) * new_r00 * x95))) + (IKabs((new_r22 * x96) + ((-1.0) * x97))) +
                                   (IKabs(x98 + ((-1.0) * x93))) + (IKabs(x93 + ((-1.0) * x98))) + (IKabs((x92 * x96) + ((-1.0) * new_r22 * x97))));
                                if (IKabs(j3eval[0]) < 0.0000000100000000) {
                                  continue; // no branches [j3, j5]

                                } else {
                                  IkReal op[4 + 1], zeror[4];
                                  int numroots;
                                  IkReal j3evalpoly[1];
                                  IkReal x100 = new_r22 * new_r22;
                                  IkReal x101 = ((16.0) * new_r10);
                                  IkReal x102 = (new_r11 * new_r22);
                                  IkReal x103 = (x100 * x101);
                                  IkReal x104 = (((8.0) * x102) + ((-8.0) * new_r00));
                                  op[0] = x104;
                                  op[1] = (((-1.0) * x103) + x101);
                                  op[2] = (((16.0) * x102) + ((-32.0) * new_r00 * x100) + ((16.0) * new_r00));
                                  op[3] = (((-1.0) * x101) + x103);
                                  op[4] = x104;
                                  polyroots4(op, zeror, numroots);
                                  IkReal j3array[4], cj3array[4], sj3array[4], tempj3array[1];
                                  int numsolutions = 0;
                                  for (int ij3 = 0; ij3 < numroots; ++ij3) {
                                    IkReal htj3 = zeror[ij3];
                                    tempj3array[0] = ((2.0) * (atan(htj3)));
                                    for (int kj3 = 0; kj3 < 1; ++kj3) {
                                      j3array[numsolutions] = tempj3array[kj3];
                                      if (j3array[numsolutions] > IKPI)
                                        j3array[numsolutions] -= IK2PI;
                                      else if (j3array[numsolutions] < -IKPI)
                                        j3array[numsolutions] += IK2PI;
                                      sj3array[numsolutions] = IKsin(j3array[numsolutions]);
                                      cj3array[numsolutions] = IKcos(j3array[numsolutions]);
                                      numsolutions++;
                                    }
                                  }
                                  bool j3valid[4] = {true, true, true, true};
                                  _nj3 = 4;
                                  for (int ij3 = 0; ij3 < numsolutions; ++ij3) {
                                    if (!j3valid[ij3])
                                      continue;
                                    j3 = j3array[ij3];
                                    cj3 = cj3array[ij3];
                                    sj3 = sj3array[ij3];
                                    htj3 = IKtan(j3 / 2);

                                    IkReal x105 = new_r22 * new_r22;
                                    IkReal x106 = ((16.0) * new_r01);
                                    IkReal x107 = (new_r00 * new_r22);
                                    IkReal x108 = ((8.0) * x107);
                                    IkReal x109 = (new_r11 * x105);
                                    IkReal x110 = ((8.0) * x109);
                                    IkReal x111 = (x105 * x106);
                                    j3evalpoly[0] = (((-1.0) * x108) + x110 + ((htj3 * htj3 * htj3) * (((-1.0) * x106) + x111)) +
                                                     ((htj3 * htj3) * (((32.0) * new_r11) + ((-16.0) * x109) + ((-16.0) * x107))) + (htj3 * (((-1.0) * x111) + x106)) +
                                                     ((htj3 * htj3 * htj3 * htj3) * (((-1.0) * x108) + x110)));
                                    if (IKabs(j3evalpoly[0]) > 0.0000001000000000) {
                                      continue;
                                    }
                                    _ij3[0] = ij3;
                                    _ij3[1] = -1;
                                    for (int iij3 = ij3 + 1; iij3 < numsolutions; ++iij3) {
                                      if (j3valid[iij3] && IKabs(cj3array[ij3] - cj3array[iij3]) < IKFAST_SOLUTION_THRESH &&
                                          IKabs(sj3array[ij3] - sj3array[iij3]) < IKFAST_SOLUTION_THRESH) {
                                        j3valid[iij3] = false;
                                        _ij3[1] = iij3;
                                        break;
                                      }
                                    }
                                    {
                                      IkReal j5eval[3];
                                      new_r02 = 0;
                                      new_r12 = 0;
                                      new_r20 = 0;
                                      new_r21 = 0;
                                      IkReal x112 = cj3 * cj3;
                                      IkReal x113 = (cj3 * new_r22);
                                      IkReal x114 = ((-1.0) + x112 + ((-1.0) * x112 * (new_r22 * new_r22)));
                                      j5eval[0] = x114;
                                      j5eval[1] = ((IKabs((new_r01 * sj3) + ((-1.0) * new_r00 * x113))) + (IKabs((new_r00 * sj3) + (new_r01 * x113))));
                                      j5eval[2] = IKsign(x114);
                                      if (IKabs(j5eval[0]) < 0.0000010000000000 || IKabs(j5eval[1]) < 0.0000010000000000 || IKabs(j5eval[2]) < 0.0000010000000000) {
                                        {
                                          IkReal j5eval[1];
                                          new_r02 = 0;
                                          new_r12 = 0;
                                          new_r20 = 0;
                                          new_r21 = 0;
                                          j5eval[0] = new_r22;
                                          if (IKabs(j5eval[0]) < 0.0000010000000000) {
                                            {
                                              IkReal j5eval[2];
                                              new_r02 = 0;
                                              new_r12 = 0;
                                              new_r20 = 0;
                                              new_r21 = 0;
                                              IkReal x115 = new_r22 * new_r22;
                                              j5eval[0] = ((cj3 * x115) + ((-1.0) * cj3));
                                              j5eval[1] = (((-1.0) * sj3) + (sj3 * x115));
                                              if (IKabs(j5eval[0]) < 0.0000010000000000 || IKabs(j5eval[1]) < 0.0000010000000000) {
                                                {
                                                  IkReal evalcond[1];
                                                  bool bgotonextstatement = true;
                                                  do {
                                                    evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs((-1.5707963267949) + j3))), 6.28318530717959)));
                                                    if (IKabs(evalcond[0]) < 0.0000050000000000) {
                                                      bgotonextstatement = false;
                                                      {
                                                        IkReal j5array[1], cj5array[1], sj5array[1];
                                                        bool j5valid[1] = {false};
                                                        _nj5 = 1;
                                                        if (IKabs((-1.0) * new_r00) < IKFAST_ATAN2_MAGTHRESH && IKabs((-1.0) * new_r01) < IKFAST_ATAN2_MAGTHRESH &&
                                                            IKabs(IKsqr((-1.0) * new_r00) + IKsqr((-1.0) * new_r01) - 1) <= IKFAST_SINCOS_THRESH) {
                                                          continue;
                                                        }
                                                        j5array[0] = IKatan2(((-1.0) * new_r00), ((-1.0) * new_r01));
                                                        sj5array[0] = IKsin(j5array[0]);
                                                        cj5array[0] = IKcos(j5array[0]);
                                                        if (j5array[0] > IKPI) {
                                                          j5array[0] -= IK2PI;
                                                        } else if (j5array[0] < -IKPI) {
                                                          j5array[0] += IK2PI;
                                                        }
                                                        j5valid[0] = true;
                                                        for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                          if (!j5valid[ij5])
                                                            continue;
                                                          _ij5[0] = ij5;
                                                          _ij5[1] = -1;
                                                          for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                            if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                              j5valid[iij5] = false;
                                                              _ij5[1] = iij5;
                                                              break;
                                                            }
                                                          }
                                                          j5 = j5array[ij5];
                                                          cj5 = cj5array[ij5];
                                                          sj5 = sj5array[ij5];
                                                          {
                                                            IkReal evalcond[4];
                                                            IkReal x116 = IKsin(j5);
                                                            IkReal x117 = IKcos(j5);
                                                            evalcond[0] = x116;
                                                            evalcond[1] = ((-1.0) * x117);
                                                            evalcond[2] = (((-1.0) * new_r00) + ((-1.0) * x116));
                                                            evalcond[3] = (((-1.0) * new_r01) + ((-1.0) * x117));
                                                            if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH) {
                                                              continue;
                                                            }
                                                          }

                                                          {
                                                            std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                            vinfos[0].jointtype = 1;
                                                            vinfos[0].foffset = j0;
                                                            vinfos[0].indices[0] = _ij0[0];
                                                            vinfos[0].indices[1] = _ij0[1];
                                                            vinfos[0].maxsolutions = _nj0;
                                                            vinfos[1].jointtype = 1;
                                                            vinfos[1].foffset = j1;
                                                            vinfos[1].indices[0] = _ij1[0];
                                                            vinfos[1].indices[1] = _ij1[1];
                                                            vinfos[1].maxsolutions = _nj1;
                                                            vinfos[2].jointtype = 1;
                                                            vinfos[2].foffset = j2;
                                                            vinfos[2].indices[0] = _ij2[0];
                                                            vinfos[2].indices[1] = _ij2[1];
                                                            vinfos[2].maxsolutions = _nj2;
                                                            vinfos[3].jointtype = 1;
                                                            vinfos[3].foffset = j3;
                                                            vinfos[3].indices[0] = _ij3[0];
                                                            vinfos[3].indices[1] = _ij3[1];
                                                            vinfos[3].maxsolutions = _nj3;
                                                            vinfos[4].jointtype = 1;
                                                            vinfos[4].foffset = j4;
                                                            vinfos[4].indices[0] = _ij4[0];
                                                            vinfos[4].indices[1] = _ij4[1];
                                                            vinfos[4].maxsolutions = _nj4;
                                                            vinfos[5].jointtype = 1;
                                                            vinfos[5].foffset = j5;
                                                            vinfos[5].indices[0] = _ij5[0];
                                                            vinfos[5].indices[1] = _ij5[1];
                                                            vinfos[5].maxsolutions = _nj5;
                                                            std::vector<int> vfree(0);
                                                            solutions.AddSolution(vinfos, vfree);
                                                          }
                                                        }
                                                      }
                                                    }
                                                  } while (0);
                                                  if (bgotonextstatement) {
                                                    bool bgotonextstatement = true;
                                                    do {
                                                      evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs((1.5707963267949) + j3))), 6.28318530717959)));
                                                      if (IKabs(evalcond[0]) < 0.0000050000000000) {
                                                        bgotonextstatement = false;
                                                        {
                                                          IkReal j5array[1], cj5array[1], sj5array[1];
                                                          bool j5valid[1] = {false};
                                                          _nj5 = 1;
                                                          if (IKabs(new_r00) < IKFAST_ATAN2_MAGTHRESH && IKabs(new_r01) < IKFAST_ATAN2_MAGTHRESH &&
                                                              IKabs(IKsqr(new_r00) + IKsqr(new_r01) - 1) <= IKFAST_SINCOS_THRESH) {
                                                            continue;
                                                          }
                                                          j5array[0] = IKatan2(new_r00, new_r01);
                                                          sj5array[0] = IKsin(j5array[0]);
                                                          cj5array[0] = IKcos(j5array[0]);
                                                          if (j5array[0] > IKPI) {
                                                            j5array[0] -= IK2PI;
                                                          } else if (j5array[0] < -IKPI) {
                                                            j5array[0] += IK2PI;
                                                          }
                                                          j5valid[0] = true;
                                                          for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                            if (!j5valid[ij5])
                                                              continue;
                                                            _ij5[0] = ij5;
                                                            _ij5[1] = -1;
                                                            for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                              if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                  IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                j5valid[iij5] = false;
                                                                _ij5[1] = iij5;
                                                                break;
                                                              }
                                                            }
                                                            j5 = j5array[ij5];
                                                            cj5 = cj5array[ij5];
                                                            sj5 = sj5array[ij5];
                                                            {
                                                              IkReal evalcond[4];
                                                              IkReal x118 = IKsin(j5);
                                                              IkReal x119 = IKcos(j5);
                                                              evalcond[0] = x118;
                                                              evalcond[1] = ((-1.0) * x119);
                                                              evalcond[2] = (new_r00 + ((-1.0) * x118));
                                                              evalcond[3] = (new_r01 + ((-1.0) * x119));
                                                              if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                  IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH) {
                                                                continue;
                                                              }
                                                            }

                                                            {
                                                              std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                              vinfos[0].jointtype = 1;
                                                              vinfos[0].foffset = j0;
                                                              vinfos[0].indices[0] = _ij0[0];
                                                              vinfos[0].indices[1] = _ij0[1];
                                                              vinfos[0].maxsolutions = _nj0;
                                                              vinfos[1].jointtype = 1;
                                                              vinfos[1].foffset = j1;
                                                              vinfos[1].indices[0] = _ij1[0];
                                                              vinfos[1].indices[1] = _ij1[1];
                                                              vinfos[1].maxsolutions = _nj1;
                                                              vinfos[2].jointtype = 1;
                                                              vinfos[2].foffset = j2;
                                                              vinfos[2].indices[0] = _ij2[0];
                                                              vinfos[2].indices[1] = _ij2[1];
                                                              vinfos[2].maxsolutions = _nj2;
                                                              vinfos[3].jointtype = 1;
                                                              vinfos[3].foffset = j3;
                                                              vinfos[3].indices[0] = _ij3[0];
                                                              vinfos[3].indices[1] = _ij3[1];
                                                              vinfos[3].maxsolutions = _nj3;
                                                              vinfos[4].jointtype = 1;
                                                              vinfos[4].foffset = j4;
                                                              vinfos[4].indices[0] = _ij4[0];
                                                              vinfos[4].indices[1] = _ij4[1];
                                                              vinfos[4].maxsolutions = _nj4;
                                                              vinfos[5].jointtype = 1;
                                                              vinfos[5].foffset = j5;
                                                              vinfos[5].indices[0] = _ij5[0];
                                                              vinfos[5].indices[1] = _ij5[1];
                                                              vinfos[5].maxsolutions = _nj5;
                                                              std::vector<int> vfree(0);
                                                              solutions.AddSolution(vinfos, vfree);
                                                            }
                                                          }
                                                        }
                                                      }
                                                    } while (0);
                                                    if (bgotonextstatement) {
                                                      bool bgotonextstatement = true;
                                                      do {
                                                        evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs(j3))), 6.28318530717959)));
                                                        if (IKabs(evalcond[0]) < 0.0000050000000000) {
                                                          bgotonextstatement = false;
                                                          {
                                                            IkReal j5array[1], cj5array[1], sj5array[1];
                                                            bool j5valid[1] = {false};
                                                            _nj5 = 1;
                                                            if (IKabs(new_r10) < IKFAST_ATAN2_MAGTHRESH && IKabs(new_r11) < IKFAST_ATAN2_MAGTHRESH &&
                                                                IKabs(IKsqr(new_r10) + IKsqr(new_r11) - 1) <= IKFAST_SINCOS_THRESH) {
                                                              continue;
                                                            }
                                                            j5array[0] = IKatan2(new_r10, new_r11);
                                                            sj5array[0] = IKsin(j5array[0]);
                                                            cj5array[0] = IKcos(j5array[0]);
                                                            if (j5array[0] > IKPI) {
                                                              j5array[0] -= IK2PI;
                                                            } else if (j5array[0] < -IKPI) {
                                                              j5array[0] += IK2PI;
                                                            }
                                                            j5valid[0] = true;
                                                            for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                              if (!j5valid[ij5]) {
                                                                continue;
                                                              }
                                                              _ij5[0] = ij5;
                                                              _ij5[1] = -1;
                                                              for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                                if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                    IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                  j5valid[iij5] = false;
                                                                  _ij5[1] = iij5;
                                                                  break;
                                                                }
                                                              }
                                                              j5 = j5array[ij5];
                                                              cj5 = cj5array[ij5];
                                                              sj5 = sj5array[ij5];
                                                              {
                                                                IkReal evalcond[4];
                                                                IkReal x120 = IKsin(j5);
                                                                IkReal x121 = IKcos(j5);
                                                                evalcond[0] = x120;
                                                                evalcond[1] = ((-1.0) * x121);
                                                                evalcond[2] = (((-1.0) * x120) + new_r10);
                                                                evalcond[3] = (((-1.0) * x121) + new_r11);
                                                                if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                    IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH) {
                                                                  continue;
                                                                }
                                                              }

                                                              {
                                                                std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                                vinfos[0].jointtype = 1;
                                                                vinfos[0].foffset = j0;
                                                                vinfos[0].indices[0] = _ij0[0];
                                                                vinfos[0].indices[1] = _ij0[1];
                                                                vinfos[0].maxsolutions = _nj0;
                                                                vinfos[1].jointtype = 1;
                                                                vinfos[1].foffset = j1;
                                                                vinfos[1].indices[0] = _ij1[0];
                                                                vinfos[1].indices[1] = _ij1[1];
                                                                vinfos[1].maxsolutions = _nj1;
                                                                vinfos[2].jointtype = 1;
                                                                vinfos[2].foffset = j2;
                                                                vinfos[2].indices[0] = _ij2[0];
                                                                vinfos[2].indices[1] = _ij2[1];
                                                                vinfos[2].maxsolutions = _nj2;
                                                                vinfos[3].jointtype = 1;
                                                                vinfos[3].foffset = j3;
                                                                vinfos[3].indices[0] = _ij3[0];
                                                                vinfos[3].indices[1] = _ij3[1];
                                                                vinfos[3].maxsolutions = _nj3;
                                                                vinfos[4].jointtype = 1;
                                                                vinfos[4].foffset = j4;
                                                                vinfos[4].indices[0] = _ij4[0];
                                                                vinfos[4].indices[1] = _ij4[1];
                                                                vinfos[4].maxsolutions = _nj4;
                                                                vinfos[5].jointtype = 1;
                                                                vinfos[5].foffset = j5;
                                                                vinfos[5].indices[0] = _ij5[0];
                                                                vinfos[5].indices[1] = _ij5[1];
                                                                vinfos[5].maxsolutions = _nj5;
                                                                std::vector<int> vfree(0);
                                                                solutions.AddSolution(vinfos, vfree);
                                                              }
                                                            }
                                                          }
                                                        }
                                                      } while (0);
                                                      if (bgotonextstatement) {
                                                        bool bgotonextstatement = true;
                                                        do {
                                                          evalcond[0] =
                                                            ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs((-3.14159265358979) + j3))), 6.28318530717959)));
                                                          if (IKabs(evalcond[0]) < 0.0000050000000000) {
                                                            bgotonextstatement = false;
                                                            {
                                                              IkReal j5array[1], cj5array[1], sj5array[1];
                                                              bool j5valid[1] = {false};
                                                              _nj5 = 1;
                                                              if (IKabs((-1.0) * new_r10) < IKFAST_ATAN2_MAGTHRESH && IKabs((-1.0) * new_r11) < IKFAST_ATAN2_MAGTHRESH &&
                                                                  IKabs(IKsqr((-1.0) * new_r10) + IKsqr((-1.0) * new_r11) - 1) <= IKFAST_SINCOS_THRESH) {
                                                                continue;
                                                              }
                                                              j5array[0] = IKatan2(((-1.0) * new_r10), ((-1.0) * new_r11));
                                                              sj5array[0] = IKsin(j5array[0]);
                                                              cj5array[0] = IKcos(j5array[0]);
                                                              if (j5array[0] > IKPI) {
                                                                j5array[0] -= IK2PI;
                                                              } else if (j5array[0] < -IKPI) {
                                                                j5array[0] += IK2PI;
                                                              }
                                                              j5valid[0] = true;
                                                              for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                                if (!j5valid[ij5]) {
                                                                  continue;
                                                                }
                                                                _ij5[0] = ij5;
                                                                _ij5[1] = -1;
                                                                for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                                  if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                      IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                    j5valid[iij5] = false;
                                                                    _ij5[1] = iij5;
                                                                    break;
                                                                  }
                                                                }
                                                                j5 = j5array[ij5];
                                                                cj5 = cj5array[ij5];
                                                                sj5 = sj5array[ij5];
                                                                {
                                                                  IkReal evalcond[4];
                                                                  IkReal x122 = IKsin(j5);
                                                                  IkReal x123 = IKcos(j5);
                                                                  evalcond[0] = x122;
                                                                  evalcond[1] = ((-1.0) * x123);
                                                                  evalcond[2] = (((-1.0) * new_r10) + ((-1.0) * x122));
                                                                  evalcond[3] = (((-1.0) * new_r11) + ((-1.0) * x123));
                                                                  if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                      IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH) {
                                                                    continue;
                                                                  }
                                                                }

                                                                {
                                                                  std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                                  vinfos[0].jointtype = 1;
                                                                  vinfos[0].foffset = j0;
                                                                  vinfos[0].indices[0] = _ij0[0];
                                                                  vinfos[0].indices[1] = _ij0[1];
                                                                  vinfos[0].maxsolutions = _nj0;
                                                                  vinfos[1].jointtype = 1;
                                                                  vinfos[1].foffset = j1;
                                                                  vinfos[1].indices[0] = _ij1[0];
                                                                  vinfos[1].indices[1] = _ij1[1];
                                                                  vinfos[1].maxsolutions = _nj1;
                                                                  vinfos[2].jointtype = 1;
                                                                  vinfos[2].foffset = j2;
                                                                  vinfos[2].indices[0] = _ij2[0];
                                                                  vinfos[2].indices[1] = _ij2[1];
                                                                  vinfos[2].maxsolutions = _nj2;
                                                                  vinfos[3].jointtype = 1;
                                                                  vinfos[3].foffset = j3;
                                                                  vinfos[3].indices[0] = _ij3[0];
                                                                  vinfos[3].indices[1] = _ij3[1];
                                                                  vinfos[3].maxsolutions = _nj3;
                                                                  vinfos[4].jointtype = 1;
                                                                  vinfos[4].foffset = j4;
                                                                  vinfos[4].indices[0] = _ij4[0];
                                                                  vinfos[4].indices[1] = _ij4[1];
                                                                  vinfos[4].maxsolutions = _nj4;
                                                                  vinfos[5].jointtype = 1;
                                                                  vinfos[5].foffset = j5;
                                                                  vinfos[5].indices[0] = _ij5[0];
                                                                  vinfos[5].indices[1] = _ij5[1];
                                                                  vinfos[5].maxsolutions = _nj5;
                                                                  std::vector<int> vfree(0);
                                                                  solutions.AddSolution(vinfos, vfree);
                                                                }
                                                              }
                                                            }
                                                          }
                                                        } while (0);
                                                        if (bgotonextstatement) {
                                                          bool bgotonextstatement = true;
                                                          do {
                                                            CheckValue<IkReal> x124 = IKPowWithIntegerCheck(((1.0) + ((-1.0) * (new_r22 * new_r22))), -1);
                                                            if (!x124.valid)
                                                              continue;
                                                            if ((x124.value) < -0.00001) {
                                                              continue;
                                                            }
                                                            IkReal gconst0 = ((-1.0) * (IKsqrt(x124.value)));
                                                            evalcond[0] = ((-3.14159265358979) +
                                                                           (IKfmod(((3.14159265358979) + (IKabs((-1.0) + (IKsign(sj3)))) + (IKabs(cj3 + ((-1.0) * gconst0)))),
                                                                                   6.28318530717959)));
                                                            if (IKabs(evalcond[0]) < 0.0000050000000000) {
                                                              bgotonextstatement = false;
                                                              {
                                                                IkReal j5eval[1];
                                                                new_r02 = 0;
                                                                new_r12 = 0;
                                                                new_r20 = 0;
                                                                new_r21 = 0;
                                                                if (((1.0) + ((-1.0) * (gconst0 * gconst0))) < -0.00001) {
                                                                  continue;
                                                                }
                                                                sj3 = IKsqrt((1.0) + ((-1.0) * (gconst0 * gconst0)));
                                                                cj3 = gconst0;
                                                                if ((gconst0) < -1 - IKFAST_SINCOS_THRESH || (gconst0) > 1 + IKFAST_SINCOS_THRESH) {
                                                                  continue;
                                                                }
                                                                j3 = IKacos(gconst0);
                                                                CheckValue<IkReal> x125 = IKPowWithIntegerCheck(((1.0) + ((-1.0) * (new_r22 * new_r22))), -1);
                                                                if (!x125.valid) {
                                                                  continue;
                                                                }
                                                                if ((x125.value) < -0.00001) {
                                                                  continue;
                                                                }
                                                                IkReal gconst0 = ((-1.0) * (IKsqrt(x125.value)));
                                                                j5eval[0] = ((IKabs(new_r11)) + (IKabs(new_r10)));
                                                                if (IKabs(j5eval[0]) < 0.0000010000000000) {
                                                                  {
                                                                    IkReal j5array[1], cj5array[1], sj5array[1];
                                                                    bool j5valid[1] = {false};
                                                                    _nj5 = 1;
                                                                    if (((1.0) + ((-1.0) * (gconst0 * gconst0))) < -0.00001) {
                                                                      continue;
                                                                    }
                                                                    CheckValue<IkReal> x126 = IKPowWithIntegerCheck(gconst0, -1);
                                                                    if (!x126.valid) {
                                                                      continue;
                                                                    }
                                                                    if (IKabs(((-1.0) * new_r00 * (IKsqrt((1.0) + ((-1.0) * (gconst0 * gconst0))))) + (gconst0 * new_r10)) <
                                                                          IKFAST_ATAN2_MAGTHRESH &&
                                                                        IKabs(new_r11 * (x126.value)) < IKFAST_ATAN2_MAGTHRESH &&
                                                                        IKabs(IKsqr(((-1.0) * new_r00 * (IKsqrt((1.0) + ((-1.0) * (gconst0 * gconst0))))) + (gconst0 * new_r10)) +
                                                                              IKsqr(new_r11 * (x126.value)) - 1) <= IKFAST_SINCOS_THRESH) {
                                                                      continue;
                                                                    }
                                                                    j5array[0] =
                                                                      IKatan2((((-1.0) * new_r00 * (IKsqrt((1.0) + ((-1.0) * (gconst0 * gconst0))))) + (gconst0 * new_r10)),
                                                                              (new_r11 * (x126.value)));
                                                                    sj5array[0] = IKsin(j5array[0]);
                                                                    cj5array[0] = IKcos(j5array[0]);
                                                                    if (j5array[0] > IKPI) {
                                                                      j5array[0] -= IK2PI;
                                                                    } else if (j5array[0] < -IKPI) {
                                                                      j5array[0] += IK2PI;
                                                                    }
                                                                    j5valid[0] = true;
                                                                    for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                                      if (!j5valid[ij5]) {
                                                                        continue;
                                                                      }
                                                                      _ij5[0] = ij5;
                                                                      _ij5[1] = -1;
                                                                      for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                                        if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                            IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                          j5valid[iij5] = false;
                                                                          _ij5[1] = iij5;
                                                                          break;
                                                                        }
                                                                      }
                                                                      j5 = j5array[ij5];
                                                                      cj5 = cj5array[ij5];
                                                                      sj5 = sj5array[ij5];
                                                                      {
                                                                        IkReal evalcond[8];
                                                                        IkReal x127 = IKcos(j5);
                                                                        IkReal x128 = IKsin(j5);
                                                                        IkReal x129 = ((1.0) * x128);
                                                                        IkReal x130 = ((1.0) * x127);
                                                                        if (((1.0) + ((-1.0) * (gconst0 * gconst0))) < -0.00001) {
                                                                          continue;
                                                                        }
                                                                        IkReal x131 = IKsqrt((1.0) + ((-1.0) * (gconst0 * gconst0)));
                                                                        IkReal x132 = ((1.0) * x131);
                                                                        evalcond[0] = x128;
                                                                        evalcond[1] = ((-1.0) * x127);
                                                                        evalcond[2] = (((-1.0) * gconst0 * x130) + new_r11);
                                                                        evalcond[3] = (((-1.0) * gconst0 * x129) + new_r10);
                                                                        evalcond[4] = ((x127 * x131) + new_r01);
                                                                        evalcond[5] = ((x128 * x131) + new_r00);
                                                                        evalcond[6] = (((-1.0) * new_r00 * x132) + ((-1.0) * x129) + (gconst0 * new_r10));
                                                                        evalcond[7] = (((-1.0) * new_r01 * x132) + ((-1.0) * x130) + (gconst0 * new_r11));
                                                                        if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                            IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                                            IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                                            IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                                                          continue;
                                                                        }
                                                                      }

                                                                      {
                                                                        std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                                        vinfos[0].jointtype = 1;
                                                                        vinfos[0].foffset = j0;
                                                                        vinfos[0].indices[0] = _ij0[0];
                                                                        vinfos[0].indices[1] = _ij0[1];
                                                                        vinfos[0].maxsolutions = _nj0;
                                                                        vinfos[1].jointtype = 1;
                                                                        vinfos[1].foffset = j1;
                                                                        vinfos[1].indices[0] = _ij1[0];
                                                                        vinfos[1].indices[1] = _ij1[1];
                                                                        vinfos[1].maxsolutions = _nj1;
                                                                        vinfos[2].jointtype = 1;
                                                                        vinfos[2].foffset = j2;
                                                                        vinfos[2].indices[0] = _ij2[0];
                                                                        vinfos[2].indices[1] = _ij2[1];
                                                                        vinfos[2].maxsolutions = _nj2;
                                                                        vinfos[3].jointtype = 1;
                                                                        vinfos[3].foffset = j3;
                                                                        vinfos[3].indices[0] = _ij3[0];
                                                                        vinfos[3].indices[1] = _ij3[1];
                                                                        vinfos[3].maxsolutions = _nj3;
                                                                        vinfos[4].jointtype = 1;
                                                                        vinfos[4].foffset = j4;
                                                                        vinfos[4].indices[0] = _ij4[0];
                                                                        vinfos[4].indices[1] = _ij4[1];
                                                                        vinfos[4].maxsolutions = _nj4;
                                                                        vinfos[5].jointtype = 1;
                                                                        vinfos[5].foffset = j5;
                                                                        vinfos[5].indices[0] = _ij5[0];
                                                                        vinfos[5].indices[1] = _ij5[1];
                                                                        vinfos[5].maxsolutions = _nj5;
                                                                        std::vector<int> vfree(0);
                                                                        solutions.AddSolution(vinfos, vfree);
                                                                      }
                                                                    }
                                                                  }

                                                                } else {
                                                                  {
                                                                    IkReal j5array[1], cj5array[1], sj5array[1];
                                                                    bool j5valid[1] = {false};
                                                                    _nj5 = 1;
                                                                    CheckValue<IkReal> x133 = IKPowWithIntegerCheck(IKsign(gconst0), -1);
                                                                    if (!x133.valid) {
                                                                      continue;
                                                                    }
                                                                    CheckValue<IkReal> x134 = IKatan2WithCheck(IkReal(new_r10), IkReal(new_r11), IKFAST_ATAN2_MAGTHRESH);
                                                                    if (!x134.valid) {
                                                                      continue;
                                                                    }
                                                                    j5array[0] = ((-1.5707963267949) + ((1.5707963267949) * (x133.value)) + (x134.value));
                                                                    sj5array[0] = IKsin(j5array[0]);
                                                                    cj5array[0] = IKcos(j5array[0]);
                                                                    if (j5array[0] > IKPI) {
                                                                      j5array[0] -= IK2PI;
                                                                    } else if (j5array[0] < -IKPI) {
                                                                      j5array[0] += IK2PI;
                                                                    }
                                                                    j5valid[0] = true;
                                                                    for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                                      if (!j5valid[ij5]) {
                                                                        continue;
                                                                      }
                                                                      _ij5[0] = ij5;
                                                                      _ij5[1] = -1;
                                                                      for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                                        if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                            IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                          j5valid[iij5] = false;
                                                                          _ij5[1] = iij5;
                                                                          break;
                                                                        }
                                                                      }
                                                                      j5 = j5array[ij5];
                                                                      cj5 = cj5array[ij5];
                                                                      sj5 = sj5array[ij5];
                                                                      {
                                                                        IkReal evalcond[8];
                                                                        IkReal x135 = IKcos(j5);
                                                                        IkReal x136 = IKsin(j5);
                                                                        IkReal x137 = ((1.0) * x136);
                                                                        IkReal x138 = ((1.0) * x135);
                                                                        if (((1.0) + ((-1.0) * (gconst0 * gconst0))) < -0.00001) {
                                                                          continue;
                                                                        }
                                                                        IkReal x139 = IKsqrt((1.0) + ((-1.0) * (gconst0 * gconst0)));
                                                                        IkReal x140 = ((1.0) * x139);
                                                                        evalcond[0] = x136;
                                                                        evalcond[1] = ((-1.0) * x135);
                                                                        evalcond[2] = (((-1.0) * gconst0 * x138) + new_r11);
                                                                        evalcond[3] = (((-1.0) * gconst0 * x137) + new_r10);
                                                                        evalcond[4] = ((x135 * x139) + new_r01);
                                                                        evalcond[5] = ((x136 * x139) + new_r00);
                                                                        evalcond[6] = (((-1.0) * x137) + (gconst0 * new_r10) + ((-1.0) * new_r00 * x140));
                                                                        evalcond[7] = (((-1.0) * new_r01 * x140) + ((-1.0) * x138) + (gconst0 * new_r11));
                                                                        if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                            IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                                            IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                                            IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                                                          continue;
                                                                        }
                                                                      }

                                                                      {
                                                                        std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                                        vinfos[0].jointtype = 1;
                                                                        vinfos[0].foffset = j0;
                                                                        vinfos[0].indices[0] = _ij0[0];
                                                                        vinfos[0].indices[1] = _ij0[1];
                                                                        vinfos[0].maxsolutions = _nj0;
                                                                        vinfos[1].jointtype = 1;
                                                                        vinfos[1].foffset = j1;
                                                                        vinfos[1].indices[0] = _ij1[0];
                                                                        vinfos[1].indices[1] = _ij1[1];
                                                                        vinfos[1].maxsolutions = _nj1;
                                                                        vinfos[2].jointtype = 1;
                                                                        vinfos[2].foffset = j2;
                                                                        vinfos[2].indices[0] = _ij2[0];
                                                                        vinfos[2].indices[1] = _ij2[1];
                                                                        vinfos[2].maxsolutions = _nj2;
                                                                        vinfos[3].jointtype = 1;
                                                                        vinfos[3].foffset = j3;
                                                                        vinfos[3].indices[0] = _ij3[0];
                                                                        vinfos[3].indices[1] = _ij3[1];
                                                                        vinfos[3].maxsolutions = _nj3;
                                                                        vinfos[4].jointtype = 1;
                                                                        vinfos[4].foffset = j4;
                                                                        vinfos[4].indices[0] = _ij4[0];
                                                                        vinfos[4].indices[1] = _ij4[1];
                                                                        vinfos[4].maxsolutions = _nj4;
                                                                        vinfos[5].jointtype = 1;
                                                                        vinfos[5].foffset = j5;
                                                                        vinfos[5].indices[0] = _ij5[0];
                                                                        vinfos[5].indices[1] = _ij5[1];
                                                                        vinfos[5].maxsolutions = _nj5;
                                                                        std::vector<int> vfree(0);
                                                                        solutions.AddSolution(vinfos, vfree);
                                                                      }
                                                                    }
                                                                  }
                                                                }
                                                              }
                                                            }
                                                          } while (0);
                                                          if (bgotonextstatement) {
                                                            bool bgotonextstatement = true;
                                                            do {
                                                              CheckValue<IkReal> x141 = IKPowWithIntegerCheck(((1.0) + ((-1.0) * (new_r22 * new_r22))), -1);
                                                              if (!x141.valid)
                                                                continue;
                                                              if ((x141.value) < -0.00001) {
                                                                continue;
                                                              }
                                                              IkReal gconst0 = ((-1.0) * (IKsqrt(x141.value)));
                                                              evalcond[0] = ((-3.14159265358979) +
                                                                             (IKfmod(((3.14159265358979) + (IKabs(cj3 + ((-1.0) * gconst0))) + (IKabs((1.0) + (IKsign(sj3))))),
                                                                                     6.28318530717959)));
                                                              if (IKabs(evalcond[0]) < 0.0000050000000000) {
                                                                bgotonextstatement = false;
                                                                {
                                                                  IkReal j5eval[1];
                                                                  new_r02 = 0;
                                                                  new_r12 = 0;
                                                                  new_r20 = 0;
                                                                  new_r21 = 0;
                                                                  if (((1.0) + ((-1.0) * (gconst0 * gconst0))) < -0.00001) {
                                                                    continue;
                                                                  }
                                                                  sj3 = ((-1.0) * (IKsqrt((1.0) + ((-1.0) * (gconst0 * gconst0)))));
                                                                  cj3 = gconst0;
                                                                  if ((gconst0) < -1 - IKFAST_SINCOS_THRESH || (gconst0) > 1 + IKFAST_SINCOS_THRESH) {
                                                                    continue;
                                                                  }
                                                                  j3 = ((-1.0) * (IKacos(gconst0)));
                                                                  CheckValue<IkReal> x142 = IKPowWithIntegerCheck(((1.0) + ((-1.0) * (new_r22 * new_r22))), -1);
                                                                  if (!x142.valid) {
                                                                    continue;
                                                                  }
                                                                  if ((x142.value) < -0.00001) {
                                                                    continue;
                                                                  }
                                                                  IkReal gconst0 = ((-1.0) * (IKsqrt(x142.value)));
                                                                  j5eval[0] = ((IKabs(new_r11)) + (IKabs(new_r10)));
                                                                  if (IKabs(j5eval[0]) < 0.0000010000000000) {
                                                                    {
                                                                      IkReal j5array[1], cj5array[1], sj5array[1];
                                                                      bool j5valid[1] = {false};
                                                                      _nj5 = 1;
                                                                      if (((1.0) + ((-1.0) * (gconst0 * gconst0))) < -0.00001) {
                                                                        continue;
                                                                      }
                                                                      CheckValue<IkReal> x143 = IKPowWithIntegerCheck(gconst0, -1);
                                                                      if (!x143.valid) {
                                                                        continue;
                                                                      }
                                                                      if (IKabs((new_r00 * (IKsqrt((1.0) + ((-1.0) * (gconst0 * gconst0))))) + (gconst0 * new_r10)) <
                                                                            IKFAST_ATAN2_MAGTHRESH &&
                                                                          IKabs(new_r11 * (x143.value)) < IKFAST_ATAN2_MAGTHRESH &&
                                                                          IKabs(IKsqr((new_r00 * (IKsqrt((1.0) + ((-1.0) * (gconst0 * gconst0))))) + (gconst0 * new_r10)) +
                                                                                IKsqr(new_r11 * (x143.value)) - 1) <= IKFAST_SINCOS_THRESH) {
                                                                        continue;
                                                                      }
                                                                      j5array[0] = IKatan2(((new_r00 * (IKsqrt((1.0) + ((-1.0) * (gconst0 * gconst0))))) + (gconst0 * new_r10)),
                                                                                           (new_r11 * (x143.value)));
                                                                      sj5array[0] = IKsin(j5array[0]);
                                                                      cj5array[0] = IKcos(j5array[0]);
                                                                      if (j5array[0] > IKPI) {
                                                                        j5array[0] -= IK2PI;
                                                                      } else if (j5array[0] < -IKPI) {
                                                                        j5array[0] += IK2PI;
                                                                      }
                                                                      j5valid[0] = true;
                                                                      for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                                        if (!j5valid[ij5]) {
                                                                          continue;
                                                                        }
                                                                        _ij5[0] = ij5;
                                                                        _ij5[1] = -1;
                                                                        for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                                          if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                              IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                            j5valid[iij5] = false;
                                                                            _ij5[1] = iij5;
                                                                            break;
                                                                          }
                                                                        }
                                                                        j5 = j5array[ij5];
                                                                        cj5 = cj5array[ij5];
                                                                        sj5 = sj5array[ij5];
                                                                        {
                                                                          IkReal evalcond[8];
                                                                          IkReal x144 = IKcos(j5);
                                                                          IkReal x145 = IKsin(j5);
                                                                          IkReal x146 = ((1.0) * x145);
                                                                          IkReal x147 = ((1.0) * x144);
                                                                          if (((1.0) + ((-1.0) * (gconst0 * gconst0))) < -0.00001) {
                                                                            continue;
                                                                          }
                                                                          IkReal x148 = IKsqrt((1.0) + ((-1.0) * (gconst0 * gconst0)));
                                                                          evalcond[0] = x145;
                                                                          evalcond[1] = ((-1.0) * x144);
                                                                          evalcond[2] = (((-1.0) * gconst0 * x147) + new_r11);
                                                                          evalcond[3] = (((-1.0) * gconst0 * x146) + new_r10);
                                                                          evalcond[4] = (((-1.0) * x147 * x148) + new_r01);
                                                                          evalcond[5] = (((-1.0) * x146 * x148) + new_r00);
                                                                          evalcond[6] = (((-1.0) * x146) + (new_r00 * x148) + (gconst0 * new_r10));
                                                                          evalcond[7] = (((-1.0) * x147) + (new_r01 * x148) + (gconst0 * new_r11));
                                                                          if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                              IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                                              IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                                              IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                                                            continue;
                                                                          }
                                                                        }

                                                                        {
                                                                          std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                                          vinfos[0].jointtype = 1;
                                                                          vinfos[0].foffset = j0;
                                                                          vinfos[0].indices[0] = _ij0[0];
                                                                          vinfos[0].indices[1] = _ij0[1];
                                                                          vinfos[0].maxsolutions = _nj0;
                                                                          vinfos[1].jointtype = 1;
                                                                          vinfos[1].foffset = j1;
                                                                          vinfos[1].indices[0] = _ij1[0];
                                                                          vinfos[1].indices[1] = _ij1[1];
                                                                          vinfos[1].maxsolutions = _nj1;
                                                                          vinfos[2].jointtype = 1;
                                                                          vinfos[2].foffset = j2;
                                                                          vinfos[2].indices[0] = _ij2[0];
                                                                          vinfos[2].indices[1] = _ij2[1];
                                                                          vinfos[2].maxsolutions = _nj2;
                                                                          vinfos[3].jointtype = 1;
                                                                          vinfos[3].foffset = j3;
                                                                          vinfos[3].indices[0] = _ij3[0];
                                                                          vinfos[3].indices[1] = _ij3[1];
                                                                          vinfos[3].maxsolutions = _nj3;
                                                                          vinfos[4].jointtype = 1;
                                                                          vinfos[4].foffset = j4;
                                                                          vinfos[4].indices[0] = _ij4[0];
                                                                          vinfos[4].indices[1] = _ij4[1];
                                                                          vinfos[4].maxsolutions = _nj4;
                                                                          vinfos[5].jointtype = 1;
                                                                          vinfos[5].foffset = j5;
                                                                          vinfos[5].indices[0] = _ij5[0];
                                                                          vinfos[5].indices[1] = _ij5[1];
                                                                          vinfos[5].maxsolutions = _nj5;
                                                                          std::vector<int> vfree(0);
                                                                          solutions.AddSolution(vinfos, vfree);
                                                                        }
                                                                      }
                                                                    }

                                                                  } else {
                                                                    {
                                                                      IkReal j5array[1], cj5array[1], sj5array[1];
                                                                      bool j5valid[1] = {false};
                                                                      _nj5 = 1;
                                                                      CheckValue<IkReal> x149 = IKPowWithIntegerCheck(IKsign(gconst0), -1);
                                                                      if (!x149.valid) {
                                                                        continue;
                                                                      }
                                                                      CheckValue<IkReal> x150 = IKatan2WithCheck(IkReal(new_r10), IkReal(new_r11), IKFAST_ATAN2_MAGTHRESH);
                                                                      if (!x150.valid) {
                                                                        continue;
                                                                      }
                                                                      j5array[0] = ((-1.5707963267949) + ((1.5707963267949) * (x149.value)) + (x150.value));
                                                                      sj5array[0] = IKsin(j5array[0]);
                                                                      cj5array[0] = IKcos(j5array[0]);
                                                                      if (j5array[0] > IKPI) {
                                                                        j5array[0] -= IK2PI;
                                                                      } else if (j5array[0] < -IKPI) {
                                                                        j5array[0] += IK2PI;
                                                                      }
                                                                      j5valid[0] = true;
                                                                      for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                                        if (!j5valid[ij5]) {
                                                                          continue;
                                                                        }
                                                                        _ij5[0] = ij5;
                                                                        _ij5[1] = -1;
                                                                        for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                                          if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                              IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                            j5valid[iij5] = false;
                                                                            _ij5[1] = iij5;
                                                                            break;
                                                                          }
                                                                        }
                                                                        j5 = j5array[ij5];
                                                                        cj5 = cj5array[ij5];
                                                                        sj5 = sj5array[ij5];
                                                                        {
                                                                          IkReal evalcond[8];
                                                                          IkReal x151 = IKcos(j5);
                                                                          IkReal x152 = IKsin(j5);
                                                                          IkReal x153 = ((1.0) * x152);
                                                                          IkReal x154 = ((1.0) * x151);
                                                                          if (((1.0) + ((-1.0) * (gconst0 * gconst0))) < -0.00001) {
                                                                            continue;
                                                                          }
                                                                          IkReal x155 = IKsqrt((1.0) + ((-1.0) * (gconst0 * gconst0)));
                                                                          evalcond[0] = x152;
                                                                          evalcond[1] = ((-1.0) * x151);
                                                                          evalcond[2] = (((-1.0) * gconst0 * x154) + new_r11);
                                                                          evalcond[3] = (((-1.0) * gconst0 * x153) + new_r10);
                                                                          evalcond[4] = (((-1.0) * x154 * x155) + new_r01);
                                                                          evalcond[5] = (((-1.0) * x153 * x155) + new_r00);
                                                                          evalcond[6] = (((-1.0) * x153) + (new_r00 * x155) + (gconst0 * new_r10));
                                                                          evalcond[7] = (((-1.0) * x154) + (new_r01 * x155) + (gconst0 * new_r11));
                                                                          if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                              IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                                              IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                                              IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                                                            continue;
                                                                          }
                                                                        }

                                                                        {
                                                                          std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                                          vinfos[0].jointtype = 1;
                                                                          vinfos[0].foffset = j0;
                                                                          vinfos[0].indices[0] = _ij0[0];
                                                                          vinfos[0].indices[1] = _ij0[1];
                                                                          vinfos[0].maxsolutions = _nj0;
                                                                          vinfos[1].jointtype = 1;
                                                                          vinfos[1].foffset = j1;
                                                                          vinfos[1].indices[0] = _ij1[0];
                                                                          vinfos[1].indices[1] = _ij1[1];
                                                                          vinfos[1].maxsolutions = _nj1;
                                                                          vinfos[2].jointtype = 1;
                                                                          vinfos[2].foffset = j2;
                                                                          vinfos[2].indices[0] = _ij2[0];
                                                                          vinfos[2].indices[1] = _ij2[1];
                                                                          vinfos[2].maxsolutions = _nj2;
                                                                          vinfos[3].jointtype = 1;
                                                                          vinfos[3].foffset = j3;
                                                                          vinfos[3].indices[0] = _ij3[0];
                                                                          vinfos[3].indices[1] = _ij3[1];
                                                                          vinfos[3].maxsolutions = _nj3;
                                                                          vinfos[4].jointtype = 1;
                                                                          vinfos[4].foffset = j4;
                                                                          vinfos[4].indices[0] = _ij4[0];
                                                                          vinfos[4].indices[1] = _ij4[1];
                                                                          vinfos[4].maxsolutions = _nj4;
                                                                          vinfos[5].jointtype = 1;
                                                                          vinfos[5].foffset = j5;
                                                                          vinfos[5].indices[0] = _ij5[0];
                                                                          vinfos[5].indices[1] = _ij5[1];
                                                                          vinfos[5].maxsolutions = _nj5;
                                                                          std::vector<int> vfree(0);
                                                                          solutions.AddSolution(vinfos, vfree);
                                                                        }
                                                                      }
                                                                    }
                                                                  }
                                                                }
                                                              }
                                                            } while (0);
                                                            if (bgotonextstatement) {
                                                              bool bgotonextstatement = true;
                                                              do {
                                                                CheckValue<IkReal> x156 = IKPowWithIntegerCheck(((1.0) + ((-1.0) * (new_r22 * new_r22))), -1);
                                                                if (!x156.valid) {
                                                                  continue;
                                                                }
                                                                if ((x156.value) < -0.00001) {
                                                                  continue;
                                                                }
                                                                IkReal gconst1 = IKsqrt(x156.value);
                                                                evalcond[0] = ((-3.14159265358979) +
                                                                               (IKfmod(((3.14159265358979) + (IKabs(cj3 + ((-1.0) * gconst1))) + (IKabs((-1.0) + (IKsign(sj3))))),
                                                                                       6.28318530717959)));
                                                                if (IKabs(evalcond[0]) < 0.0000050000000000) {
                                                                  bgotonextstatement = false;
                                                                  {
                                                                    IkReal j5eval[1];
                                                                    new_r02 = 0;
                                                                    new_r12 = 0;
                                                                    new_r20 = 0;
                                                                    new_r21 = 0;
                                                                    if (((1.0) + ((-1.0) * (gconst1 * gconst1))) < -0.00001) {
                                                                      continue;
                                                                    }
                                                                    sj3 = IKsqrt((1.0) + ((-1.0) * (gconst1 * gconst1)));
                                                                    cj3 = gconst1;
                                                                    if ((gconst1) < -1 - IKFAST_SINCOS_THRESH || (gconst1) > 1 + IKFAST_SINCOS_THRESH) {
                                                                      continue;
                                                                    }
                                                                    j3 = IKacos(gconst1);
                                                                    CheckValue<IkReal> x157 = IKPowWithIntegerCheck(((1.0) + ((-1.0) * (new_r22 * new_r22))), -1);
                                                                    if (!x157.valid) {
                                                                      continue;
                                                                    }
                                                                    if ((x157.value) < -0.00001) {
                                                                      continue;
                                                                    }
                                                                    IkReal gconst1 = IKsqrt(x157.value);
                                                                    j5eval[0] = ((IKabs(new_r11)) + (IKabs(new_r10)));
                                                                    if (IKabs(j5eval[0]) < 0.0000010000000000) {
                                                                      {
                                                                        IkReal j5array[1], cj5array[1], sj5array[1];
                                                                        bool j5valid[1] = {false};
                                                                        _nj5 = 1;
                                                                        if (((1.0) + ((-1.0) * (gconst1 * gconst1))) < -0.00001) {
                                                                          continue;
                                                                        }
                                                                        CheckValue<IkReal> x158 = IKPowWithIntegerCheck(gconst1, -1);
                                                                        if (!x158.valid) {
                                                                          continue;
                                                                        }
                                                                        if (IKabs((gconst1 * new_r10) + ((-1.0) * new_r00 * (IKsqrt((1.0) + ((-1.0) * (gconst1 * gconst1)))))) <
                                                                              IKFAST_ATAN2_MAGTHRESH &&
                                                                            IKabs(new_r11 * (x158.value)) < IKFAST_ATAN2_MAGTHRESH &&
                                                                            IKabs(
                                                                              IKsqr((gconst1 * new_r10) + ((-1.0) * new_r00 * (IKsqrt((1.0) + ((-1.0) * (gconst1 * gconst1)))))) +
                                                                              IKsqr(new_r11 * (x158.value)) - 1) <= IKFAST_SINCOS_THRESH) {
                                                                          continue;
                                                                        }
                                                                        j5array[0] =
                                                                          IKatan2(((gconst1 * new_r10) + ((-1.0) * new_r00 * (IKsqrt((1.0) + ((-1.0) * (gconst1 * gconst1)))))),
                                                                                  (new_r11 * (x158.value)));
                                                                        sj5array[0] = IKsin(j5array[0]);
                                                                        cj5array[0] = IKcos(j5array[0]);
                                                                        if (j5array[0] > IKPI) {
                                                                          j5array[0] -= IK2PI;
                                                                        } else if (j5array[0] < -IKPI) {
                                                                          j5array[0] += IK2PI;
                                                                        }
                                                                        j5valid[0] = true;
                                                                        for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                                          if (!j5valid[ij5]) {
                                                                            continue;
                                                                          }
                                                                          _ij5[0] = ij5;
                                                                          _ij5[1] = -1;
                                                                          for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                                            if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                                IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                              j5valid[iij5] = false;
                                                                              _ij5[1] = iij5;
                                                                              break;
                                                                            }
                                                                          }
                                                                          j5 = j5array[ij5];
                                                                          cj5 = cj5array[ij5];
                                                                          sj5 = sj5array[ij5];
                                                                          {
                                                                            IkReal evalcond[8];
                                                                            IkReal x159 = IKcos(j5);
                                                                            IkReal x160 = IKsin(j5);
                                                                            IkReal x161 = ((1.0) * x160);
                                                                            IkReal x162 = ((1.0) * x159);
                                                                            if (((1.0) + ((-1.0) * (gconst1 * gconst1))) < -0.00001) {
                                                                              continue;
                                                                            }
                                                                            IkReal x163 = IKsqrt((1.0) + ((-1.0) * (gconst1 * gconst1)));
                                                                            IkReal x164 = ((1.0) * x163);
                                                                            evalcond[0] = x160;
                                                                            evalcond[1] = ((-1.0) * x159);
                                                                            evalcond[2] = (((-1.0) * gconst1 * x162) + new_r11);
                                                                            evalcond[3] = (((-1.0) * gconst1 * x161) + new_r10);
                                                                            evalcond[4] = (new_r01 + (x159 * x163));
                                                                            evalcond[5] = (new_r00 + (x160 * x163));
                                                                            evalcond[6] = (((-1.0) * new_r00 * x164) + ((-1.0) * x161) + (gconst1 * new_r10));
                                                                            evalcond[7] = (((-1.0) * x162) + (gconst1 * new_r11) + ((-1.0) * new_r01 * x164));
                                                                            if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                                IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                                                IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                                                IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                                                              continue;
                                                                            }
                                                                          }

                                                                          {
                                                                            std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                                            vinfos[0].jointtype = 1;
                                                                            vinfos[0].foffset = j0;
                                                                            vinfos[0].indices[0] = _ij0[0];
                                                                            vinfos[0].indices[1] = _ij0[1];
                                                                            vinfos[0].maxsolutions = _nj0;
                                                                            vinfos[1].jointtype = 1;
                                                                            vinfos[1].foffset = j1;
                                                                            vinfos[1].indices[0] = _ij1[0];
                                                                            vinfos[1].indices[1] = _ij1[1];
                                                                            vinfos[1].maxsolutions = _nj1;
                                                                            vinfos[2].jointtype = 1;
                                                                            vinfos[2].foffset = j2;
                                                                            vinfos[2].indices[0] = _ij2[0];
                                                                            vinfos[2].indices[1] = _ij2[1];
                                                                            vinfos[2].maxsolutions = _nj2;
                                                                            vinfos[3].jointtype = 1;
                                                                            vinfos[3].foffset = j3;
                                                                            vinfos[3].indices[0] = _ij3[0];
                                                                            vinfos[3].indices[1] = _ij3[1];
                                                                            vinfos[3].maxsolutions = _nj3;
                                                                            vinfos[4].jointtype = 1;
                                                                            vinfos[4].foffset = j4;
                                                                            vinfos[4].indices[0] = _ij4[0];
                                                                            vinfos[4].indices[1] = _ij4[1];
                                                                            vinfos[4].maxsolutions = _nj4;
                                                                            vinfos[5].jointtype = 1;
                                                                            vinfos[5].foffset = j5;
                                                                            vinfos[5].indices[0] = _ij5[0];
                                                                            vinfos[5].indices[1] = _ij5[1];
                                                                            vinfos[5].maxsolutions = _nj5;
                                                                            std::vector<int> vfree(0);
                                                                            solutions.AddSolution(vinfos, vfree);
                                                                          }
                                                                        }
                                                                      }

                                                                    } else {
                                                                      {
                                                                        IkReal j5array[1], cj5array[1], sj5array[1];
                                                                        bool j5valid[1] = {false};
                                                                        _nj5 = 1;
                                                                        CheckValue<IkReal> x165 = IKPowWithIntegerCheck(IKsign(gconst1), -1);
                                                                        if (!x165.valid) {
                                                                          continue;
                                                                        }
                                                                        CheckValue<IkReal> x166 = IKatan2WithCheck(IkReal(new_r10), IkReal(new_r11), IKFAST_ATAN2_MAGTHRESH);
                                                                        if (!x166.valid) {
                                                                          continue;
                                                                        }
                                                                        j5array[0] = ((-1.5707963267949) + ((1.5707963267949) * (x165.value)) + (x166.value));
                                                                        sj5array[0] = IKsin(j5array[0]);
                                                                        cj5array[0] = IKcos(j5array[0]);
                                                                        if (j5array[0] > IKPI) {
                                                                          j5array[0] -= IK2PI;
                                                                        } else if (j5array[0] < -IKPI) {
                                                                          j5array[0] += IK2PI;
                                                                        }
                                                                        j5valid[0] = true;
                                                                        for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                                          if (!j5valid[ij5]) {
                                                                            continue;
                                                                          }
                                                                          _ij5[0] = ij5;
                                                                          _ij5[1] = -1;
                                                                          for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                                            if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                                IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                              j5valid[iij5] = false;
                                                                              _ij5[1] = iij5;
                                                                              break;
                                                                            }
                                                                          }
                                                                          j5 = j5array[ij5];
                                                                          cj5 = cj5array[ij5];
                                                                          sj5 = sj5array[ij5];
                                                                          {
                                                                            IkReal evalcond[8];
                                                                            IkReal x167 = IKcos(j5);
                                                                            IkReal x168 = IKsin(j5);
                                                                            IkReal x169 = ((1.0) * x168);
                                                                            IkReal x170 = ((1.0) * x167);
                                                                            if (((1.0) + ((-1.0) * (gconst1 * gconst1))) < -0.00001) {
                                                                              continue;
                                                                            }
                                                                            IkReal x171 = IKsqrt((1.0) + ((-1.0) * (gconst1 * gconst1)));
                                                                            IkReal x172 = ((1.0) * x171);
                                                                            evalcond[0] = x168;
                                                                            evalcond[1] = ((-1.0) * x167);
                                                                            evalcond[2] = (((-1.0) * gconst1 * x170) + new_r11);
                                                                            evalcond[3] = (((-1.0) * gconst1 * x169) + new_r10);
                                                                            evalcond[4] = ((x167 * x171) + new_r01);
                                                                            evalcond[5] = ((x168 * x171) + new_r00);
                                                                            evalcond[6] = (((-1.0) * x169) + (gconst1 * new_r10) + ((-1.0) * new_r00 * x172));
                                                                            evalcond[7] = (((-1.0) * x170) + (gconst1 * new_r11) + ((-1.0) * new_r01 * x172));
                                                                            if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                                IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                                                IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                                                IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                                                              continue;
                                                                            }
                                                                          }

                                                                          {
                                                                            std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                                            vinfos[0].jointtype = 1;
                                                                            vinfos[0].foffset = j0;
                                                                            vinfos[0].indices[0] = _ij0[0];
                                                                            vinfos[0].indices[1] = _ij0[1];
                                                                            vinfos[0].maxsolutions = _nj0;
                                                                            vinfos[1].jointtype = 1;
                                                                            vinfos[1].foffset = j1;
                                                                            vinfos[1].indices[0] = _ij1[0];
                                                                            vinfos[1].indices[1] = _ij1[1];
                                                                            vinfos[1].maxsolutions = _nj1;
                                                                            vinfos[2].jointtype = 1;
                                                                            vinfos[2].foffset = j2;
                                                                            vinfos[2].indices[0] = _ij2[0];
                                                                            vinfos[2].indices[1] = _ij2[1];
                                                                            vinfos[2].maxsolutions = _nj2;
                                                                            vinfos[3].jointtype = 1;
                                                                            vinfos[3].foffset = j3;
                                                                            vinfos[3].indices[0] = _ij3[0];
                                                                            vinfos[3].indices[1] = _ij3[1];
                                                                            vinfos[3].maxsolutions = _nj3;
                                                                            vinfos[4].jointtype = 1;
                                                                            vinfos[4].foffset = j4;
                                                                            vinfos[4].indices[0] = _ij4[0];
                                                                            vinfos[4].indices[1] = _ij4[1];
                                                                            vinfos[4].maxsolutions = _nj4;
                                                                            vinfos[5].jointtype = 1;
                                                                            vinfos[5].foffset = j5;
                                                                            vinfos[5].indices[0] = _ij5[0];
                                                                            vinfos[5].indices[1] = _ij5[1];
                                                                            vinfos[5].maxsolutions = _nj5;
                                                                            std::vector<int> vfree(0);
                                                                            solutions.AddSolution(vinfos, vfree);
                                                                          }
                                                                        }
                                                                      }
                                                                    }
                                                                  }
                                                                }
                                                              } while (0);
                                                              if (bgotonextstatement) {
                                                                bool bgotonextstatement = true;
                                                                do {
                                                                  CheckValue<IkReal> x173 = IKPowWithIntegerCheck(((1.0) + ((-1.0) * (new_r22 * new_r22))), -1);
                                                                  if (!x173.valid) {
                                                                    continue;
                                                                  }
                                                                  if ((x173.value) < -0.00001) {
                                                                    continue;
                                                                  }
                                                                  IkReal gconst1 = IKsqrt(x173.value);
                                                                  evalcond[0] = ((-3.14159265358979) +
                                                                                 (IKfmod(((3.14159265358979) + (IKabs(cj3 + ((-1.0) * gconst1))) + (IKabs((1.0) + (IKsign(sj3))))),
                                                                                         6.28318530717959)));
                                                                  if (IKabs(evalcond[0]) < 0.0000050000000000) {
                                                                    bgotonextstatement = false;
                                                                    {
                                                                      IkReal j5eval[1];
                                                                      new_r02 = 0;
                                                                      new_r12 = 0;
                                                                      new_r20 = 0;
                                                                      new_r21 = 0;
                                                                      if (((1.0) + ((-1.0) * (gconst1 * gconst1))) < -0.00001) {
                                                                        continue;
                                                                      }
                                                                      sj3 = ((-1.0) * (IKsqrt((1.0) + ((-1.0) * (gconst1 * gconst1)))));
                                                                      cj3 = gconst1;
                                                                      if ((gconst1) < -1 - IKFAST_SINCOS_THRESH || (gconst1) > 1 + IKFAST_SINCOS_THRESH) {
                                                                        continue;
                                                                      }
                                                                      j3 = ((-1.0) * (IKacos(gconst1)));
                                                                      CheckValue<IkReal> x174 = IKPowWithIntegerCheck(((1.0) + ((-1.0) * (new_r22 * new_r22))), -1);
                                                                      if (!x174.valid) {
                                                                        continue;
                                                                      }
                                                                      if ((x174.value) < -0.00001) {
                                                                        continue;
                                                                      }
                                                                      IkReal gconst1 = IKsqrt(x174.value);
                                                                      j5eval[0] = ((IKabs(new_r11)) + (IKabs(new_r10)));
                                                                      if (IKabs(j5eval[0]) < 0.0000010000000000) {
                                                                        {
                                                                          IkReal j5array[1], cj5array[1], sj5array[1];
                                                                          bool j5valid[1] = {false};
                                                                          _nj5 = 1;
                                                                          if (((1.0) + ((-1.0) * (gconst1 * gconst1))) < -0.00001) {
                                                                            continue;
                                                                          }
                                                                          CheckValue<IkReal> x175 = IKPowWithIntegerCheck(gconst1, -1);
                                                                          if (!x175.valid) {
                                                                            continue;
                                                                          }
                                                                          if (IKabs((gconst1 * new_r10) + (new_r00 * (IKsqrt((1.0) + ((-1.0) * (gconst1 * gconst1)))))) <
                                                                                IKFAST_ATAN2_MAGTHRESH &&
                                                                              IKabs(new_r11 * (x175.value)) < IKFAST_ATAN2_MAGTHRESH &&
                                                                              IKabs(IKsqr((gconst1 * new_r10) + (new_r00 * (IKsqrt((1.0) + ((-1.0) * (gconst1 * gconst1)))))) +
                                                                                    IKsqr(new_r11 * (x175.value)) - 1) <= IKFAST_SINCOS_THRESH) {
                                                                            continue;
                                                                          }
                                                                          j5array[0] = IKatan2(((gconst1 * new_r10) + (new_r00 * (IKsqrt((1.0) + ((-1.0) * (gconst1 * gconst1)))))),
                                                                                               (new_r11 * (x175.value)));
                                                                          sj5array[0] = IKsin(j5array[0]);
                                                                          cj5array[0] = IKcos(j5array[0]);
                                                                          if (j5array[0] > IKPI) {
                                                                            j5array[0] -= IK2PI;
                                                                          } else if (j5array[0] < -IKPI) {
                                                                            j5array[0] += IK2PI;
                                                                          }
                                                                          j5valid[0] = true;
                                                                          for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                                            if (!j5valid[ij5]) {
                                                                              continue;
                                                                            }
                                                                            _ij5[0] = ij5;
                                                                            _ij5[1] = -1;
                                                                            for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                                              if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                                  IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                                j5valid[iij5] = false;
                                                                                _ij5[1] = iij5;
                                                                                break;
                                                                              }
                                                                            }
                                                                            j5 = j5array[ij5];
                                                                            cj5 = cj5array[ij5];
                                                                            sj5 = sj5array[ij5];
                                                                            {
                                                                              IkReal evalcond[8];
                                                                              IkReal x176 = IKcos(j5);
                                                                              IkReal x177 = IKsin(j5);
                                                                              IkReal x178 = ((1.0) * x177);
                                                                              IkReal x179 = ((1.0) * x176);
                                                                              if (((1.0) + ((-1.0) * (gconst1 * gconst1))) < -0.00001) {
                                                                                continue;
                                                                              }
                                                                              IkReal x180 = IKsqrt((1.0) + ((-1.0) * (gconst1 * gconst1)));
                                                                              evalcond[0] = x177;
                                                                              evalcond[1] = ((-1.0) * x176);
                                                                              evalcond[2] = (((-1.0) * gconst1 * x179) + new_r11);
                                                                              evalcond[3] = (((-1.0) * gconst1 * x178) + new_r10);
                                                                              evalcond[4] = (new_r01 + ((-1.0) * x179 * x180));
                                                                              evalcond[5] = (((-1.0) * x178 * x180) + new_r00);
                                                                              evalcond[6] = ((new_r00 * x180) + ((-1.0) * x178) + (gconst1 * new_r10));
                                                                              evalcond[7] = ((new_r01 * x180) + ((-1.0) * x179) + (gconst1 * new_r11));
                                                                              if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                                  IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                                                  IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                                                  IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                                                                continue;
                                                                              }
                                                                            }

                                                                            {
                                                                              std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                                              vinfos[0].jointtype = 1;
                                                                              vinfos[0].foffset = j0;
                                                                              vinfos[0].indices[0] = _ij0[0];
                                                                              vinfos[0].indices[1] = _ij0[1];
                                                                              vinfos[0].maxsolutions = _nj0;
                                                                              vinfos[1].jointtype = 1;
                                                                              vinfos[1].foffset = j1;
                                                                              vinfos[1].indices[0] = _ij1[0];
                                                                              vinfos[1].indices[1] = _ij1[1];
                                                                              vinfos[1].maxsolutions = _nj1;
                                                                              vinfos[2].jointtype = 1;
                                                                              vinfos[2].foffset = j2;
                                                                              vinfos[2].indices[0] = _ij2[0];
                                                                              vinfos[2].indices[1] = _ij2[1];
                                                                              vinfos[2].maxsolutions = _nj2;
                                                                              vinfos[3].jointtype = 1;
                                                                              vinfos[3].foffset = j3;
                                                                              vinfos[3].indices[0] = _ij3[0];
                                                                              vinfos[3].indices[1] = _ij3[1];
                                                                              vinfos[3].maxsolutions = _nj3;
                                                                              vinfos[4].jointtype = 1;
                                                                              vinfos[4].foffset = j4;
                                                                              vinfos[4].indices[0] = _ij4[0];
                                                                              vinfos[4].indices[1] = _ij4[1];
                                                                              vinfos[4].maxsolutions = _nj4;
                                                                              vinfos[5].jointtype = 1;
                                                                              vinfos[5].foffset = j5;
                                                                              vinfos[5].indices[0] = _ij5[0];
                                                                              vinfos[5].indices[1] = _ij5[1];
                                                                              vinfos[5].maxsolutions = _nj5;
                                                                              std::vector<int> vfree(0);
                                                                              solutions.AddSolution(vinfos, vfree);
                                                                            }
                                                                          }
                                                                        }

                                                                      } else {
                                                                        {
                                                                          IkReal j5array[1], cj5array[1], sj5array[1];
                                                                          bool j5valid[1] = {false};
                                                                          _nj5 = 1;
                                                                          CheckValue<IkReal> x181 = IKPowWithIntegerCheck(IKsign(gconst1), -1);
                                                                          if (!x181.valid) {
                                                                            continue;
                                                                          }
                                                                          CheckValue<IkReal> x182 = IKatan2WithCheck(IkReal(new_r10), IkReal(new_r11), IKFAST_ATAN2_MAGTHRESH);
                                                                          if (!x182.valid) {
                                                                            continue;
                                                                          }
                                                                          j5array[0] = ((-1.5707963267949) + ((1.5707963267949) * (x181.value)) + (x182.value));
                                                                          sj5array[0] = IKsin(j5array[0]);
                                                                          cj5array[0] = IKcos(j5array[0]);
                                                                          if (j5array[0] > IKPI) {
                                                                            j5array[0] -= IK2PI;
                                                                          } else if (j5array[0] < -IKPI) {
                                                                            j5array[0] += IK2PI;
                                                                          }
                                                                          j5valid[0] = true;
                                                                          for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                                            if (!j5valid[ij5]) {
                                                                              continue;
                                                                            }
                                                                            _ij5[0] = ij5;
                                                                            _ij5[1] = -1;
                                                                            for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                                              if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                                  IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                                j5valid[iij5] = false;
                                                                                _ij5[1] = iij5;
                                                                                break;
                                                                              }
                                                                            }
                                                                            j5 = j5array[ij5];
                                                                            cj5 = cj5array[ij5];
                                                                            sj5 = sj5array[ij5];
                                                                            {
                                                                              IkReal evalcond[8];
                                                                              IkReal x183 = IKcos(j5);
                                                                              IkReal x184 = IKsin(j5);
                                                                              IkReal x185 = ((1.0) * x184);
                                                                              IkReal x186 = ((1.0) * x183);
                                                                              if (((1.0) + ((-1.0) * (gconst1 * gconst1))) < -0.00001) {
                                                                                continue;
                                                                              }
                                                                              IkReal x187 = IKsqrt((1.0) + ((-1.0) * (gconst1 * gconst1)));
                                                                              evalcond[0] = x184;
                                                                              evalcond[1] = ((-1.0) * x183);
                                                                              evalcond[2] = (new_r11 + ((-1.0) * gconst1 * x186));
                                                                              evalcond[3] = (new_r10 + ((-1.0) * gconst1 * x185));
                                                                              evalcond[4] = (((-1.0) * x186 * x187) + new_r01);
                                                                              evalcond[5] = (((-1.0) * x185 * x187) + new_r00);
                                                                              evalcond[6] = ((new_r00 * x187) + (gconst1 * new_r10) + ((-1.0) * x185));
                                                                              evalcond[7] = ((new_r01 * x187) + (gconst1 * new_r11) + ((-1.0) * x186));
                                                                              if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                                  IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                                                  IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                                                  IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                                                                continue;
                                                                              }
                                                                            }

                                                                            {
                                                                              std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                                              vinfos[0].jointtype = 1;
                                                                              vinfos[0].foffset = j0;
                                                                              vinfos[0].indices[0] = _ij0[0];
                                                                              vinfos[0].indices[1] = _ij0[1];
                                                                              vinfos[0].maxsolutions = _nj0;
                                                                              vinfos[1].jointtype = 1;
                                                                              vinfos[1].foffset = j1;
                                                                              vinfos[1].indices[0] = _ij1[0];
                                                                              vinfos[1].indices[1] = _ij1[1];
                                                                              vinfos[1].maxsolutions = _nj1;
                                                                              vinfos[2].jointtype = 1;
                                                                              vinfos[2].foffset = j2;
                                                                              vinfos[2].indices[0] = _ij2[0];
                                                                              vinfos[2].indices[1] = _ij2[1];
                                                                              vinfos[2].maxsolutions = _nj2;
                                                                              vinfos[3].jointtype = 1;
                                                                              vinfos[3].foffset = j3;
                                                                              vinfos[3].indices[0] = _ij3[0];
                                                                              vinfos[3].indices[1] = _ij3[1];
                                                                              vinfos[3].maxsolutions = _nj3;
                                                                              vinfos[4].jointtype = 1;
                                                                              vinfos[4].foffset = j4;
                                                                              vinfos[4].indices[0] = _ij4[0];
                                                                              vinfos[4].indices[1] = _ij4[1];
                                                                              vinfos[4].maxsolutions = _nj4;
                                                                              vinfos[5].jointtype = 1;
                                                                              vinfos[5].foffset = j5;
                                                                              vinfos[5].indices[0] = _ij5[0];
                                                                              vinfos[5].indices[1] = _ij5[1];
                                                                              vinfos[5].maxsolutions = _nj5;
                                                                              std::vector<int> vfree(0);
                                                                              solutions.AddSolution(vinfos, vfree);
                                                                            }
                                                                          }
                                                                        }
                                                                      }
                                                                    }
                                                                  }
                                                                } while (0);
                                                                if (bgotonextstatement) {
                                                                  bool bgotonextstatement = true;
                                                                  do {
                                                                    if (1) {
                                                                      bgotonextstatement = false;
                                                                      continue; // branch miss [j5]
                                                                    }
                                                                  } while (0);
                                                                  if (bgotonextstatement) {
                                                                  }
                                                                }
                                                              }
                                                            }
                                                          }
                                                        }
                                                      }
                                                    }
                                                  }
                                                }

                                              } else {
                                                {
                                                  IkReal j5array[1], cj5array[1], sj5array[1];
                                                  bool j5valid[1] = {false};
                                                  _nj5 = 1;
                                                  IkReal x188 = new_r22 * new_r22;
                                                  CheckValue<IkReal> x189 = IKPowWithIntegerCheck(((cj3 * x188) + ((-1.0) * cj3)), -1);
                                                  if (!x189.valid)
                                                    continue;
                                                  CheckValue<IkReal> x190 = IKPowWithIntegerCheck((((-1.0) * sj3) + (sj3 * x188)), -1);
                                                  if (!x190.valid)
                                                    continue;
                                                  if (IKabs((x189.value) * (((-1.0) * new_r01 * new_r22) + ((-1.0) * new_r10))) < IKFAST_ATAN2_MAGTHRESH &&
                                                      IKabs((x190.value) * ((new_r10 * new_r22) + new_r01)) < IKFAST_ATAN2_MAGTHRESH &&
                                                      IKabs(IKsqr((x189.value) * (((-1.0) * new_r01 * new_r22) + ((-1.0) * new_r10))) +
                                                            IKsqr((x190.value) * ((new_r10 * new_r22) + new_r01)) - 1) <= IKFAST_SINCOS_THRESH) {
                                                    continue;
                                                  }
                                                  j5array[0] =
                                                    IKatan2(((x189.value) * (((-1.0) * new_r01 * new_r22) + ((-1.0) * new_r10))), ((x190.value) * ((new_r10 * new_r22) + new_r01)));
                                                  sj5array[0] = IKsin(j5array[0]);
                                                  cj5array[0] = IKcos(j5array[0]);
                                                  if (j5array[0] > IKPI)
                                                    j5array[0] -= IK2PI;
                                                  else if (j5array[0] < -IKPI)
                                                    j5array[0] += IK2PI;
                                                  j5valid[0] = true;
                                                  for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                    if (!j5valid[ij5])
                                                      continue;
                                                    _ij5[0] = ij5;
                                                    _ij5[1] = -1;
                                                    for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                      if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                          IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                        j5valid[iij5] = false;
                                                        _ij5[1] = iij5;
                                                        break;
                                                      }
                                                    }
                                                    j5 = j5array[ij5];
                                                    cj5 = cj5array[ij5];
                                                    sj5 = sj5array[ij5];
                                                    {
                                                      IkReal evalcond[10];
                                                      IkReal x191 = IKsin(j5);
                                                      IkReal x192 = IKcos(j5);
                                                      IkReal x193 = (cj3 * new_r22);
                                                      IkReal x194 = (new_r22 * sj3);
                                                      IkReal x195 = ((1.0) * sj3);
                                                      IkReal x196 = ((1.0) * x192);
                                                      IkReal x197 = (sj3 * x191);
                                                      IkReal x198 = ((1.0) * x191);
                                                      evalcond[0] = ((new_r11 * sj3) + (new_r22 * x191) + (cj3 * new_r01));
                                                      evalcond[1] = ((new_r11 * x194) + (new_r01 * x193) + x191);
                                                      evalcond[2] = (((-1.0) * x198) + (cj3 * new_r10) + ((-1.0) * new_r00 * x195));
                                                      evalcond[3] = (((-1.0) * x196) + ((-1.0) * new_r01 * x195) + (cj3 * new_r11));
                                                      evalcond[4] = ((sj3 * x192) + (x191 * x193) + new_r01);
                                                      evalcond[5] = ((new_r10 * sj3) + ((-1.0) * new_r22 * x196) + (cj3 * new_r00));
                                                      evalcond[6] = (x197 + new_r00 + ((-1.0) * x193 * x196));
                                                      evalcond[7] = ((x191 * x194) + new_r11 + ((-1.0) * cj3 * x196));
                                                      evalcond[8] = ((new_r10 * x194) + (new_r00 * x193) + ((-1.0) * x196));
                                                      evalcond[9] = (((-1.0) * x194 * x196) + new_r10 + ((-1.0) * cj3 * x198));
                                                      if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                          IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                          IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                          IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH ||
                                                          IKabs(evalcond[8]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[9]) > IKFAST_EVALCOND_THRESH) {
                                                        continue;
                                                      }
                                                    }

                                                    {
                                                      std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                      vinfos[0].jointtype = 1;
                                                      vinfos[0].foffset = j0;
                                                      vinfos[0].indices[0] = _ij0[0];
                                                      vinfos[0].indices[1] = _ij0[1];
                                                      vinfos[0].maxsolutions = _nj0;
                                                      vinfos[1].jointtype = 1;
                                                      vinfos[1].foffset = j1;
                                                      vinfos[1].indices[0] = _ij1[0];
                                                      vinfos[1].indices[1] = _ij1[1];
                                                      vinfos[1].maxsolutions = _nj1;
                                                      vinfos[2].jointtype = 1;
                                                      vinfos[2].foffset = j2;
                                                      vinfos[2].indices[0] = _ij2[0];
                                                      vinfos[2].indices[1] = _ij2[1];
                                                      vinfos[2].maxsolutions = _nj2;
                                                      vinfos[3].jointtype = 1;
                                                      vinfos[3].foffset = j3;
                                                      vinfos[3].indices[0] = _ij3[0];
                                                      vinfos[3].indices[1] = _ij3[1];
                                                      vinfos[3].maxsolutions = _nj3;
                                                      vinfos[4].jointtype = 1;
                                                      vinfos[4].foffset = j4;
                                                      vinfos[4].indices[0] = _ij4[0];
                                                      vinfos[4].indices[1] = _ij4[1];
                                                      vinfos[4].maxsolutions = _nj4;
                                                      vinfos[5].jointtype = 1;
                                                      vinfos[5].foffset = j5;
                                                      vinfos[5].indices[0] = _ij5[0];
                                                      vinfos[5].indices[1] = _ij5[1];
                                                      vinfos[5].maxsolutions = _nj5;
                                                      std::vector<int> vfree(0);
                                                      solutions.AddSolution(vinfos, vfree);
                                                    }
                                                  }
                                                }
                                              }
                                            }

                                          } else {
                                            {
                                              IkReal j5array[1], cj5array[1], sj5array[1];
                                              bool j5valid[1] = {false};
                                              _nj5 = 1;
                                              IkReal x199 = ((1.0) * new_r01);
                                              CheckValue<IkReal> x200 = IKPowWithIntegerCheck(new_r22, -1);
                                              if (!x200.valid)
                                                continue;
                                              if (IKabs((x200.value) * (((-1.0) * new_r11 * sj3) + ((-1.0) * cj3 * x199))) < IKFAST_ATAN2_MAGTHRESH &&
                                                  IKabs(((-1.0) * sj3 * x199) + (cj3 * new_r11)) < IKFAST_ATAN2_MAGTHRESH &&
                                                  IKabs(IKsqr((x200.value) * (((-1.0) * new_r11 * sj3) + ((-1.0) * cj3 * x199))) + IKsqr(((-1.0) * sj3 * x199) + (cj3 * new_r11)) -
                                                        1) <= IKFAST_SINCOS_THRESH) {
                                                continue;
                                              }
                                              j5array[0] = IKatan2(((x200.value) * (((-1.0) * new_r11 * sj3) + ((-1.0) * cj3 * x199))), (((-1.0) * sj3 * x199) + (cj3 * new_r11)));
                                              sj5array[0] = IKsin(j5array[0]);
                                              cj5array[0] = IKcos(j5array[0]);
                                              if (j5array[0] > IKPI)
                                                j5array[0] -= IK2PI;
                                              else if (j5array[0] < -IKPI)
                                                j5array[0] += IK2PI;
                                              j5valid[0] = true;
                                              for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                if (!j5valid[ij5])
                                                  continue;
                                                _ij5[0] = ij5;
                                                _ij5[1] = -1;
                                                for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                  if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                      IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                    j5valid[iij5] = false;
                                                    _ij5[1] = iij5;
                                                    break;
                                                  }
                                                }
                                                j5 = j5array[ij5];
                                                cj5 = cj5array[ij5];
                                                sj5 = sj5array[ij5];
                                                {
                                                  IkReal evalcond[10];
                                                  IkReal x201 = IKsin(j5);
                                                  IkReal x202 = IKcos(j5);
                                                  IkReal x203 = (cj3 * new_r22);
                                                  IkReal x204 = (new_r22 * sj3);
                                                  IkReal x205 = ((1.0) * sj3);
                                                  IkReal x206 = ((1.0) * x202);
                                                  IkReal x207 = (sj3 * x201);
                                                  IkReal x208 = ((1.0) * x201);
                                                  evalcond[0] = ((new_r11 * sj3) + (new_r22 * x201) + (cj3 * new_r01));
                                                  evalcond[1] = ((new_r11 * x204) + (new_r01 * x203) + x201);
                                                  evalcond[2] = (((-1.0) * new_r00 * x205) + ((-1.0) * x208) + (cj3 * new_r10));
                                                  evalcond[3] = (((-1.0) * x206) + (cj3 * new_r11) + ((-1.0) * new_r01 * x205));
                                                  evalcond[4] = ((sj3 * x202) + (x201 * x203) + new_r01);
                                                  evalcond[5] = (((-1.0) * new_r22 * x206) + (new_r10 * sj3) + (cj3 * new_r00));
                                                  evalcond[6] = (((-1.0) * x203 * x206) + x207 + new_r00);
                                                  evalcond[7] = ((x201 * x204) + new_r11 + ((-1.0) * cj3 * x206));
                                                  evalcond[8] = ((new_r10 * x204) + (new_r00 * x203) + ((-1.0) * x206));
                                                  evalcond[9] = (((-1.0) * x204 * x206) + new_r10 + ((-1.0) * cj3 * x208));
                                                  if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                      IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                      IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                      IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH ||
                                                      IKabs(evalcond[8]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[9]) > IKFAST_EVALCOND_THRESH) {
                                                    continue;
                                                  }
                                                }

                                                {
                                                  std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                  vinfos[0].jointtype = 1;
                                                  vinfos[0].foffset = j0;
                                                  vinfos[0].indices[0] = _ij0[0];
                                                  vinfos[0].indices[1] = _ij0[1];
                                                  vinfos[0].maxsolutions = _nj0;
                                                  vinfos[1].jointtype = 1;
                                                  vinfos[1].foffset = j1;
                                                  vinfos[1].indices[0] = _ij1[0];
                                                  vinfos[1].indices[1] = _ij1[1];
                                                  vinfos[1].maxsolutions = _nj1;
                                                  vinfos[2].jointtype = 1;
                                                  vinfos[2].foffset = j2;
                                                  vinfos[2].indices[0] = _ij2[0];
                                                  vinfos[2].indices[1] = _ij2[1];
                                                  vinfos[2].maxsolutions = _nj2;
                                                  vinfos[3].jointtype = 1;
                                                  vinfos[3].foffset = j3;
                                                  vinfos[3].indices[0] = _ij3[0];
                                                  vinfos[3].indices[1] = _ij3[1];
                                                  vinfos[3].maxsolutions = _nj3;
                                                  vinfos[4].jointtype = 1;
                                                  vinfos[4].foffset = j4;
                                                  vinfos[4].indices[0] = _ij4[0];
                                                  vinfos[4].indices[1] = _ij4[1];
                                                  vinfos[4].maxsolutions = _nj4;
                                                  vinfos[5].jointtype = 1;
                                                  vinfos[5].foffset = j5;
                                                  vinfos[5].indices[0] = _ij5[0];
                                                  vinfos[5].indices[1] = _ij5[1];
                                                  vinfos[5].maxsolutions = _nj5;
                                                  std::vector<int> vfree(0);
                                                  solutions.AddSolution(vinfos, vfree);
                                                }
                                              }
                                            }
                                          }
                                        }

                                      } else {
                                        {
                                          IkReal j5array[1], cj5array[1], sj5array[1];
                                          bool j5valid[1] = {false};
                                          _nj5 = 1;
                                          IkReal x209 = cj3 * cj3;
                                          IkReal x210 = (cj3 * new_r22);
                                          CheckValue<IkReal> x211 = IKatan2WithCheck(IkReal((new_r00 * sj3) + (new_r01 * x210)),
                                                                                     IkReal((new_r01 * sj3) + ((-1.0) * new_r00 * x210)), IKFAST_ATAN2_MAGTHRESH);
                                          if (!x211.valid)
                                            continue;
                                          CheckValue<IkReal> x212 = IKPowWithIntegerCheck(IKsign((-1.0) + ((-1.0) * x209 * (new_r22 * new_r22)) + x209), -1);
                                          if (!x212.valid)
                                            continue;
                                          j5array[0] = ((-1.5707963267949) + (x211.value) + ((1.5707963267949) * (x212.value)));
                                          sj5array[0] = IKsin(j5array[0]);
                                          cj5array[0] = IKcos(j5array[0]);
                                          if (j5array[0] > IKPI)
                                            j5array[0] -= IK2PI;
                                          else if (j5array[0] < -IKPI)
                                            j5array[0] += IK2PI;
                                          j5valid[0] = true;
                                          for (int ij5 = 0; ij5 < 1; ++ij5) {
                                            if (!j5valid[ij5])
                                              continue;
                                            _ij5[0] = ij5;
                                            _ij5[1] = -1;
                                            for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                              if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                  IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                j5valid[iij5] = false;
                                                _ij5[1] = iij5;
                                                break;
                                              }
                                            }
                                            j5 = j5array[ij5];
                                            cj5 = cj5array[ij5];
                                            sj5 = sj5array[ij5];
                                            {
                                              IkReal evalcond[10];
                                              IkReal x213 = IKsin(j5);
                                              IkReal x214 = IKcos(j5);
                                              IkReal x215 = (cj3 * new_r22);
                                              IkReal x216 = (new_r22 * sj3);
                                              IkReal x217 = ((1.0) * sj3);
                                              IkReal x218 = ((1.0) * x214);
                                              IkReal x219 = (sj3 * x213);
                                              IkReal x220 = ((1.0) * x213);
                                              evalcond[0] = ((new_r22 * x213) + (new_r11 * sj3) + (cj3 * new_r01));
                                              evalcond[1] = (x213 + (new_r01 * x215) + (new_r11 * x216));
                                              evalcond[2] = ((cj3 * new_r10) + ((-1.0) * new_r00 * x217) + ((-1.0) * x220));
                                              evalcond[3] = (((-1.0) * new_r01 * x217) + (cj3 * new_r11) + ((-1.0) * x218));
                                              evalcond[4] = ((x213 * x215) + (sj3 * x214) + new_r01);
                                              evalcond[5] = ((new_r10 * sj3) + ((-1.0) * new_r22 * x218) + (cj3 * new_r00));
                                              evalcond[6] = (x219 + new_r00 + ((-1.0) * x215 * x218));
                                              evalcond[7] = ((x213 * x216) + ((-1.0) * cj3 * x218) + new_r11);
                                              evalcond[8] = ((new_r00 * x215) + ((-1.0) * x218) + (new_r10 * x216));
                                              evalcond[9] = (((-1.0) * cj3 * x220) + new_r10 + ((-1.0) * x216 * x218));
                                              if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                  IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                  IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                  IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH ||
                                                  IKabs(evalcond[8]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[9]) > IKFAST_EVALCOND_THRESH) {
                                                continue;
                                              }
                                            }

                                            {
                                              std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                              vinfos[0].jointtype = 1;
                                              vinfos[0].foffset = j0;
                                              vinfos[0].indices[0] = _ij0[0];
                                              vinfos[0].indices[1] = _ij0[1];
                                              vinfos[0].maxsolutions = _nj0;
                                              vinfos[1].jointtype = 1;
                                              vinfos[1].foffset = j1;
                                              vinfos[1].indices[0] = _ij1[0];
                                              vinfos[1].indices[1] = _ij1[1];
                                              vinfos[1].maxsolutions = _nj1;
                                              vinfos[2].jointtype = 1;
                                              vinfos[2].foffset = j2;
                                              vinfos[2].indices[0] = _ij2[0];
                                              vinfos[2].indices[1] = _ij2[1];
                                              vinfos[2].maxsolutions = _nj2;
                                              vinfos[3].jointtype = 1;
                                              vinfos[3].foffset = j3;
                                              vinfos[3].indices[0] = _ij3[0];
                                              vinfos[3].indices[1] = _ij3[1];
                                              vinfos[3].maxsolutions = _nj3;
                                              vinfos[4].jointtype = 1;
                                              vinfos[4].foffset = j4;
                                              vinfos[4].indices[0] = _ij4[0];
                                              vinfos[4].indices[1] = _ij4[1];
                                              vinfos[4].maxsolutions = _nj4;
                                              vinfos[5].jointtype = 1;
                                              vinfos[5].foffset = j5;
                                              vinfos[5].indices[0] = _ij5[0];
                                              vinfos[5].indices[1] = _ij5[1];
                                              vinfos[5].maxsolutions = _nj5;
                                              std::vector<int> vfree(0);
                                              solutions.AddSolution(vinfos, vfree);
                                            }
                                          }
                                        }
                                      }
                                    }
                                  }
                                }
                              }
                            }
                          } while (0);
                          if (bgotonextstatement) {
                            bool bgotonextstatement = true;
                            do {
                              if (1) {
                                bgotonextstatement = false;
                                continue; // branch miss [j3, j5]
                              }
                            } while (0);
                            if (bgotonextstatement) {
                            }
                          }
                        }
                      }
                    }

                  } else {
                    {
                      IkReal j3array[1], cj3array[1], sj3array[1];
                      bool j3valid[1] = {false};
                      _nj3 = 1;
                      CheckValue<IkReal> x222 = IKPowWithIntegerCheck(sj4, -1);
                      if (!x222.valid)
                        continue;
                      IkReal x221 = x222.value;
                      CheckValue<IkReal> x223 = IKPowWithIntegerCheck(new_r12, -1);
                      if (!x223.valid)
                        continue;
                      if (IKabs(x221 * (x223.value) * ((1.0) + ((-1.0) * (cj4 * cj4)) + ((-1.0) * (new_r02 * new_r02)))) < IKFAST_ATAN2_MAGTHRESH &&
                          IKabs(new_r02 * x221) < IKFAST_ATAN2_MAGTHRESH &&
                          IKabs(IKsqr(x221 * (x223.value) * ((1.0) + ((-1.0) * (cj4 * cj4)) + ((-1.0) * (new_r02 * new_r02)))) + IKsqr(new_r02 * x221) - 1) <=
                            IKFAST_SINCOS_THRESH) {
                        continue;
                      }
                      j3array[0] = IKatan2((x221 * (x223.value) * ((1.0) + ((-1.0) * (cj4 * cj4)) + ((-1.0) * (new_r02 * new_r02)))), (new_r02 * x221));
                      sj3array[0] = IKsin(j3array[0]);
                      cj3array[0] = IKcos(j3array[0]);
                      if (j3array[0] > IKPI)
                        j3array[0] -= IK2PI;
                      else if (j3array[0] < -IKPI)
                        j3array[0] += IK2PI;
                      j3valid[0] = true;
                      for (int ij3 = 0; ij3 < 1; ++ij3) {
                        if (!j3valid[ij3])
                          continue;
                        _ij3[0] = ij3;
                        _ij3[1] = -1;
                        for (int iij3 = ij3 + 1; iij3 < 1; ++iij3) {
                          if (j3valid[iij3] && IKabs(cj3array[ij3] - cj3array[iij3]) < IKFAST_SOLUTION_THRESH && IKabs(sj3array[ij3] - sj3array[iij3]) < IKFAST_SOLUTION_THRESH) {
                            j3valid[iij3] = false;
                            _ij3[1] = iij3;
                            break;
                          }
                        }
                        j3 = j3array[ij3];
                        cj3 = cj3array[ij3];
                        sj3 = sj3array[ij3];
                        {
                          IkReal evalcond[8];
                          IkReal x224 = IKcos(j3);
                          IkReal x225 = IKsin(j3);
                          IkReal x226 = ((1.0) * sj4);
                          IkReal x227 = ((1.0) * cj4);
                          IkReal x228 = (new_r02 * x224);
                          IkReal x229 = (new_r12 * x225);
                          evalcond[0] = (((-1.0) * x224 * x226) + new_r02);
                          evalcond[1] = (((-1.0) * x225 * x226) + new_r12);
                          evalcond[2] = ((new_r12 * x224) + ((-1.0) * new_r02 * x225));
                          evalcond[3] = (x229 + x228 + ((-1.0) * x226));
                          evalcond[4] = (((-1.0) * new_r22 * x226) + (cj4 * x228) + (cj4 * x229));
                          evalcond[5] = (((-1.0) * new_r10 * x225 * x226) + ((-1.0) * new_r00 * x224 * x226) + ((-1.0) * new_r20 * x227));
                          evalcond[6] = (((-1.0) * new_r01 * x224 * x226) + ((-1.0) * new_r11 * x225 * x226) + ((-1.0) * new_r21 * x227));
                          evalcond[7] = ((1.0) + ((-1.0) * new_r22 * x227) + ((-1.0) * x226 * x228) + ((-1.0) * x226 * x229));
                          if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH ||
                              IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                              IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                            continue;
                          }
                        }

                        {
                          IkReal j5eval[3];
                          j5eval[0] = sj4;
                          j5eval[1] = IKsign(sj4);
                          j5eval[2] = ((IKabs(new_r20)) + (IKabs(new_r21)));
                          if (IKabs(j5eval[0]) < 0.0000010000000000 || IKabs(j5eval[1]) < 0.0000010000000000 || IKabs(j5eval[2]) < 0.0000010000000000) {
                            {
                              IkReal j5eval[2];
                              j5eval[0] = sj3;
                              j5eval[1] = sj4;
                              if (IKabs(j5eval[0]) < 0.0000010000000000 || IKabs(j5eval[1]) < 0.0000010000000000) {
                                {
                                  IkReal j5eval[3];
                                  j5eval[0] = cj3;
                                  j5eval[1] = cj4;
                                  j5eval[2] = sj4;
                                  if (IKabs(j5eval[0]) < 0.0000010000000000 || IKabs(j5eval[1]) < 0.0000010000000000 || IKabs(j5eval[2]) < 0.0000010000000000) {
                                    {
                                      IkReal evalcond[5];
                                      bool bgotonextstatement = true;
                                      do {
                                        evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs((-1.5707963267949) + j3))), 6.28318530717959)));
                                        evalcond[1] = new_r02;
                                        if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000) {
                                          bgotonextstatement = false;
                                          {
                                            IkReal j5eval[3];
                                            sj3 = 1.0;
                                            cj3 = 0;
                                            j3 = 1.5707963267949;
                                            j5eval[0] = sj4;
                                            j5eval[1] = IKsign(sj4);
                                            j5eval[2] = ((IKabs(new_r20)) + (IKabs(new_r21)));
                                            if (IKabs(j5eval[0]) < 0.0000010000000000 || IKabs(j5eval[1]) < 0.0000010000000000 || IKabs(j5eval[2]) < 0.0000010000000000) {
                                              {
                                                IkReal j5eval[3];
                                                sj3 = 1.0;
                                                cj3 = 0;
                                                j3 = 1.5707963267949;
                                                j5eval[0] = cj4;
                                                j5eval[1] = IKsign(cj4);
                                                j5eval[2] = ((IKabs(new_r11)) + (IKabs(new_r10)));
                                                if (IKabs(j5eval[0]) < 0.0000010000000000 || IKabs(j5eval[1]) < 0.0000010000000000 || IKabs(j5eval[2]) < 0.0000010000000000) {
                                                  {
                                                    IkReal j5eval[1];
                                                    sj3 = 1.0;
                                                    cj3 = 0;
                                                    j3 = 1.5707963267949;
                                                    j5eval[0] = sj4;
                                                    if (IKabs(j5eval[0]) < 0.0000010000000000) {
                                                      {
                                                        IkReal evalcond[4];
                                                        bool bgotonextstatement = true;
                                                        do {
                                                          evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs(j4))), 6.28318530717959)));
                                                          evalcond[1] = new_r20;
                                                          evalcond[2] = new_r12;
                                                          evalcond[3] = new_r21;
                                                          if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000 &&
                                                              IKabs(evalcond[2]) < 0.0000050000000000 && IKabs(evalcond[3]) < 0.0000050000000000) {
                                                            bgotonextstatement = false;
                                                            {
                                                              IkReal j5array[1], cj5array[1], sj5array[1];
                                                              bool j5valid[1] = {false};
                                                              _nj5 = 1;
                                                              if (IKabs((-1.0) * new_r11) < IKFAST_ATAN2_MAGTHRESH && IKabs(new_r10) < IKFAST_ATAN2_MAGTHRESH &&
                                                                  IKabs(IKsqr((-1.0) * new_r11) + IKsqr(new_r10) - 1) <= IKFAST_SINCOS_THRESH) {
                                                                continue;
                                                              }
                                                              j5array[0] = IKatan2(((-1.0) * new_r11), new_r10);
                                                              sj5array[0] = IKsin(j5array[0]);
                                                              cj5array[0] = IKcos(j5array[0]);
                                                              if (j5array[0] > IKPI) {
                                                                j5array[0] -= IK2PI;
                                                              } else if (j5array[0] < -IKPI) {
                                                                j5array[0] += IK2PI;
                                                              }
                                                              j5valid[0] = true;
                                                              for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                                if (!j5valid[ij5]) {
                                                                  continue;
                                                                }
                                                                _ij5[0] = ij5;
                                                                _ij5[1] = -1;
                                                                for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                                  if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                      IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                    j5valid[iij5] = false;
                                                                    _ij5[1] = iij5;
                                                                    break;
                                                                  }
                                                                }
                                                                j5 = j5array[ij5];
                                                                cj5 = cj5array[ij5];
                                                                sj5 = sj5array[ij5];
                                                                {
                                                                  IkReal evalcond[4];
                                                                  IkReal x230 = IKsin(j5);
                                                                  IkReal x231 = ((1.0) * (IKcos(j5)));
                                                                  evalcond[0] = (x230 + new_r11);
                                                                  evalcond[1] = (((-1.0) * x231) + new_r10);
                                                                  evalcond[2] = (((-1.0) * new_r00) + ((-1.0) * x230));
                                                                  evalcond[3] = (((-1.0) * x231) + ((-1.0) * new_r01));
                                                                  if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                      IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH) {
                                                                    continue;
                                                                  }
                                                                }

                                                                {
                                                                  std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                                  vinfos[0].jointtype = 1;
                                                                  vinfos[0].foffset = j0;
                                                                  vinfos[0].indices[0] = _ij0[0];
                                                                  vinfos[0].indices[1] = _ij0[1];
                                                                  vinfos[0].maxsolutions = _nj0;
                                                                  vinfos[1].jointtype = 1;
                                                                  vinfos[1].foffset = j1;
                                                                  vinfos[1].indices[0] = _ij1[0];
                                                                  vinfos[1].indices[1] = _ij1[1];
                                                                  vinfos[1].maxsolutions = _nj1;
                                                                  vinfos[2].jointtype = 1;
                                                                  vinfos[2].foffset = j2;
                                                                  vinfos[2].indices[0] = _ij2[0];
                                                                  vinfos[2].indices[1] = _ij2[1];
                                                                  vinfos[2].maxsolutions = _nj2;
                                                                  vinfos[3].jointtype = 1;
                                                                  vinfos[3].foffset = j3;
                                                                  vinfos[3].indices[0] = _ij3[0];
                                                                  vinfos[3].indices[1] = _ij3[1];
                                                                  vinfos[3].maxsolutions = _nj3;
                                                                  vinfos[4].jointtype = 1;
                                                                  vinfos[4].foffset = j4;
                                                                  vinfos[4].indices[0] = _ij4[0];
                                                                  vinfos[4].indices[1] = _ij4[1];
                                                                  vinfos[4].maxsolutions = _nj4;
                                                                  vinfos[5].jointtype = 1;
                                                                  vinfos[5].foffset = j5;
                                                                  vinfos[5].indices[0] = _ij5[0];
                                                                  vinfos[5].indices[1] = _ij5[1];
                                                                  vinfos[5].maxsolutions = _nj5;
                                                                  std::vector<int> vfree(0);
                                                                  solutions.AddSolution(vinfos, vfree);
                                                                }
                                                              }
                                                            }
                                                          }
                                                        } while (0);
                                                        if (bgotonextstatement) {
                                                          bool bgotonextstatement = true;
                                                          do {
                                                            evalcond[0] =
                                                              ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs((-3.14159265358979) + j4))), 6.28318530717959)));
                                                            evalcond[1] = new_r20;
                                                            evalcond[2] = new_r12;
                                                            evalcond[3] = new_r21;
                                                            if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000 &&
                                                                IKabs(evalcond[2]) < 0.0000050000000000 && IKabs(evalcond[3]) < 0.0000050000000000) {
                                                              bgotonextstatement = false;
                                                              {
                                                                IkReal j5array[1], cj5array[1], sj5array[1];
                                                                bool j5valid[1] = {false};
                                                                _nj5 = 1;
                                                                if (IKabs(new_r11) < IKFAST_ATAN2_MAGTHRESH && IKabs((-1.0) * new_r10) < IKFAST_ATAN2_MAGTHRESH &&
                                                                    IKabs(IKsqr(new_r11) + IKsqr((-1.0) * new_r10) - 1) <= IKFAST_SINCOS_THRESH) {
                                                                  continue;
                                                                }
                                                                j5array[0] = IKatan2(new_r11, ((-1.0) * new_r10));
                                                                sj5array[0] = IKsin(j5array[0]);
                                                                cj5array[0] = IKcos(j5array[0]);
                                                                if (j5array[0] > IKPI) {
                                                                  j5array[0] -= IK2PI;
                                                                } else if (j5array[0] < -IKPI) {
                                                                  j5array[0] += IK2PI;
                                                                }
                                                                j5valid[0] = true;
                                                                for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                                  if (!j5valid[ij5]) {
                                                                    continue;
                                                                  }
                                                                  _ij5[0] = ij5;
                                                                  _ij5[1] = -1;
                                                                  for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                                    if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                        IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                      j5valid[iij5] = false;
                                                                      _ij5[1] = iij5;
                                                                      break;
                                                                    }
                                                                  }
                                                                  j5 = j5array[ij5];
                                                                  cj5 = cj5array[ij5];
                                                                  sj5 = sj5array[ij5];
                                                                  {
                                                                    IkReal evalcond[4];
                                                                    IkReal x232 = IKcos(j5);
                                                                    IkReal x233 = ((1.0) * (IKsin(j5)));
                                                                    evalcond[0] = (x232 + new_r10);
                                                                    evalcond[1] = (((-1.0) * x233) + new_r11);
                                                                    evalcond[2] = (((-1.0) * x233) + ((-1.0) * new_r00));
                                                                    evalcond[3] = (((-1.0) * new_r01) + ((-1.0) * x232));
                                                                    if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                        IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH) {
                                                                      continue;
                                                                    }
                                                                  }

                                                                  {
                                                                    std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                                    vinfos[0].jointtype = 1;
                                                                    vinfos[0].foffset = j0;
                                                                    vinfos[0].indices[0] = _ij0[0];
                                                                    vinfos[0].indices[1] = _ij0[1];
                                                                    vinfos[0].maxsolutions = _nj0;
                                                                    vinfos[1].jointtype = 1;
                                                                    vinfos[1].foffset = j1;
                                                                    vinfos[1].indices[0] = _ij1[0];
                                                                    vinfos[1].indices[1] = _ij1[1];
                                                                    vinfos[1].maxsolutions = _nj1;
                                                                    vinfos[2].jointtype = 1;
                                                                    vinfos[2].foffset = j2;
                                                                    vinfos[2].indices[0] = _ij2[0];
                                                                    vinfos[2].indices[1] = _ij2[1];
                                                                    vinfos[2].maxsolutions = _nj2;
                                                                    vinfos[3].jointtype = 1;
                                                                    vinfos[3].foffset = j3;
                                                                    vinfos[3].indices[0] = _ij3[0];
                                                                    vinfos[3].indices[1] = _ij3[1];
                                                                    vinfos[3].maxsolutions = _nj3;
                                                                    vinfos[4].jointtype = 1;
                                                                    vinfos[4].foffset = j4;
                                                                    vinfos[4].indices[0] = _ij4[0];
                                                                    vinfos[4].indices[1] = _ij4[1];
                                                                    vinfos[4].maxsolutions = _nj4;
                                                                    vinfos[5].jointtype = 1;
                                                                    vinfos[5].foffset = j5;
                                                                    vinfos[5].indices[0] = _ij5[0];
                                                                    vinfos[5].indices[1] = _ij5[1];
                                                                    vinfos[5].maxsolutions = _nj5;
                                                                    std::vector<int> vfree(0);
                                                                    solutions.AddSolution(vinfos, vfree);
                                                                  }
                                                                }
                                                              }
                                                            }
                                                          } while (0);
                                                          if (bgotonextstatement) {
                                                            bool bgotonextstatement = true;
                                                            do {
                                                              evalcond[0] =
                                                                ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs((-1.5707963267949) + j4))), 6.28318530717959)));
                                                              evalcond[1] = new_r22;
                                                              evalcond[2] = new_r11;
                                                              evalcond[3] = new_r10;
                                                              if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000 &&
                                                                  IKabs(evalcond[2]) < 0.0000050000000000 && IKabs(evalcond[3]) < 0.0000050000000000) {
                                                                bgotonextstatement = false;
                                                                {
                                                                  IkReal j5array[1], cj5array[1], sj5array[1];
                                                                  bool j5valid[1] = {false};
                                                                  _nj5 = 1;
                                                                  if (IKabs(new_r21) < IKFAST_ATAN2_MAGTHRESH && IKabs((-1.0) * new_r20) < IKFAST_ATAN2_MAGTHRESH &&
                                                                      IKabs(IKsqr(new_r21) + IKsqr((-1.0) * new_r20) - 1) <= IKFAST_SINCOS_THRESH) {
                                                                    continue;
                                                                  }
                                                                  j5array[0] = IKatan2(new_r21, ((-1.0) * new_r20));
                                                                  sj5array[0] = IKsin(j5array[0]);
                                                                  cj5array[0] = IKcos(j5array[0]);
                                                                  if (j5array[0] > IKPI) {
                                                                    j5array[0] -= IK2PI;
                                                                  } else if (j5array[0] < -IKPI) {
                                                                    j5array[0] += IK2PI;
                                                                  }
                                                                  j5valid[0] = true;
                                                                  for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                                    if (!j5valid[ij5]) {
                                                                      continue;
                                                                    }
                                                                    _ij5[0] = ij5;
                                                                    _ij5[1] = -1;
                                                                    for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                                      if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                          IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                        j5valid[iij5] = false;
                                                                        _ij5[1] = iij5;
                                                                        break;
                                                                      }
                                                                    }
                                                                    j5 = j5array[ij5];
                                                                    cj5 = cj5array[ij5];
                                                                    sj5 = sj5array[ij5];
                                                                    {
                                                                      IkReal evalcond[4];
                                                                      IkReal x234 = IKcos(j5);
                                                                      IkReal x235 = ((1.0) * (IKsin(j5)));
                                                                      evalcond[0] = (x234 + new_r20);
                                                                      evalcond[1] = (((-1.0) * x235) + new_r21);
                                                                      evalcond[2] = (((-1.0) * x235) + ((-1.0) * new_r00));
                                                                      evalcond[3] = (((-1.0) * new_r01) + ((-1.0) * x234));
                                                                      if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                          IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH) {
                                                                        continue;
                                                                      }
                                                                    }

                                                                    {
                                                                      std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                                      vinfos[0].jointtype = 1;
                                                                      vinfos[0].foffset = j0;
                                                                      vinfos[0].indices[0] = _ij0[0];
                                                                      vinfos[0].indices[1] = _ij0[1];
                                                                      vinfos[0].maxsolutions = _nj0;
                                                                      vinfos[1].jointtype = 1;
                                                                      vinfos[1].foffset = j1;
                                                                      vinfos[1].indices[0] = _ij1[0];
                                                                      vinfos[1].indices[1] = _ij1[1];
                                                                      vinfos[1].maxsolutions = _nj1;
                                                                      vinfos[2].jointtype = 1;
                                                                      vinfos[2].foffset = j2;
                                                                      vinfos[2].indices[0] = _ij2[0];
                                                                      vinfos[2].indices[1] = _ij2[1];
                                                                      vinfos[2].maxsolutions = _nj2;
                                                                      vinfos[3].jointtype = 1;
                                                                      vinfos[3].foffset = j3;
                                                                      vinfos[3].indices[0] = _ij3[0];
                                                                      vinfos[3].indices[1] = _ij3[1];
                                                                      vinfos[3].maxsolutions = _nj3;
                                                                      vinfos[4].jointtype = 1;
                                                                      vinfos[4].foffset = j4;
                                                                      vinfos[4].indices[0] = _ij4[0];
                                                                      vinfos[4].indices[1] = _ij4[1];
                                                                      vinfos[4].maxsolutions = _nj4;
                                                                      vinfos[5].jointtype = 1;
                                                                      vinfos[5].foffset = j5;
                                                                      vinfos[5].indices[0] = _ij5[0];
                                                                      vinfos[5].indices[1] = _ij5[1];
                                                                      vinfos[5].maxsolutions = _nj5;
                                                                      std::vector<int> vfree(0);
                                                                      solutions.AddSolution(vinfos, vfree);
                                                                    }
                                                                  }
                                                                }
                                                              }
                                                            } while (0);
                                                            if (bgotonextstatement) {
                                                              bool bgotonextstatement = true;
                                                              do {
                                                                evalcond[0] =
                                                                  ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs((1.5707963267949) + j4))), 6.28318530717959)));
                                                                evalcond[1] = new_r22;
                                                                evalcond[2] = new_r11;
                                                                evalcond[3] = new_r10;
                                                                if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000 &&
                                                                    IKabs(evalcond[2]) < 0.0000050000000000 && IKabs(evalcond[3]) < 0.0000050000000000) {
                                                                  bgotonextstatement = false;
                                                                  {
                                                                    IkReal j5array[1], cj5array[1], sj5array[1];
                                                                    bool j5valid[1] = {false};
                                                                    _nj5 = 1;
                                                                    if (IKabs((-1.0) * new_r21) < IKFAST_ATAN2_MAGTHRESH && IKabs(new_r20) < IKFAST_ATAN2_MAGTHRESH &&
                                                                        IKabs(IKsqr((-1.0) * new_r21) + IKsqr(new_r20) - 1) <= IKFAST_SINCOS_THRESH) {
                                                                      continue;
                                                                    }
                                                                    j5array[0] = IKatan2(((-1.0) * new_r21), new_r20);
                                                                    sj5array[0] = IKsin(j5array[0]);
                                                                    cj5array[0] = IKcos(j5array[0]);
                                                                    if (j5array[0] > IKPI) {
                                                                      j5array[0] -= IK2PI;
                                                                    } else if (j5array[0] < -IKPI) {
                                                                      j5array[0] += IK2PI;
                                                                    }
                                                                    j5valid[0] = true;
                                                                    for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                                      if (!j5valid[ij5]) {
                                                                        continue;
                                                                      }
                                                                      _ij5[0] = ij5;
                                                                      _ij5[1] = -1;
                                                                      for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                                        if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                            IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                          j5valid[iij5] = false;
                                                                          _ij5[1] = iij5;
                                                                          break;
                                                                        }
                                                                      }
                                                                      j5 = j5array[ij5];
                                                                      cj5 = cj5array[ij5];
                                                                      sj5 = sj5array[ij5];
                                                                      {
                                                                        IkReal evalcond[4];
                                                                        IkReal x236 = IKsin(j5);
                                                                        IkReal x237 = ((1.0) * (IKcos(j5)));
                                                                        evalcond[0] = (x236 + new_r21);
                                                                        evalcond[1] = (((-1.0) * x237) + new_r20);
                                                                        evalcond[2] = (((-1.0) * new_r00) + ((-1.0) * x236));
                                                                        evalcond[3] = (((-1.0) * x237) + ((-1.0) * new_r01));
                                                                        if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                            IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH) {
                                                                          continue;
                                                                        }
                                                                      }

                                                                      {
                                                                        std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                                        vinfos[0].jointtype = 1;
                                                                        vinfos[0].foffset = j0;
                                                                        vinfos[0].indices[0] = _ij0[0];
                                                                        vinfos[0].indices[1] = _ij0[1];
                                                                        vinfos[0].maxsolutions = _nj0;
                                                                        vinfos[1].jointtype = 1;
                                                                        vinfos[1].foffset = j1;
                                                                        vinfos[1].indices[0] = _ij1[0];
                                                                        vinfos[1].indices[1] = _ij1[1];
                                                                        vinfos[1].maxsolutions = _nj1;
                                                                        vinfos[2].jointtype = 1;
                                                                        vinfos[2].foffset = j2;
                                                                        vinfos[2].indices[0] = _ij2[0];
                                                                        vinfos[2].indices[1] = _ij2[1];
                                                                        vinfos[2].maxsolutions = _nj2;
                                                                        vinfos[3].jointtype = 1;
                                                                        vinfos[3].foffset = j3;
                                                                        vinfos[3].indices[0] = _ij3[0];
                                                                        vinfos[3].indices[1] = _ij3[1];
                                                                        vinfos[3].maxsolutions = _nj3;
                                                                        vinfos[4].jointtype = 1;
                                                                        vinfos[4].foffset = j4;
                                                                        vinfos[4].indices[0] = _ij4[0];
                                                                        vinfos[4].indices[1] = _ij4[1];
                                                                        vinfos[4].maxsolutions = _nj4;
                                                                        vinfos[5].jointtype = 1;
                                                                        vinfos[5].foffset = j5;
                                                                        vinfos[5].indices[0] = _ij5[0];
                                                                        vinfos[5].indices[1] = _ij5[1];
                                                                        vinfos[5].maxsolutions = _nj5;
                                                                        std::vector<int> vfree(0);
                                                                        solutions.AddSolution(vinfos, vfree);
                                                                      }
                                                                    }
                                                                  }
                                                                }
                                                              } while (0);
                                                              if (bgotonextstatement) {
                                                                bool bgotonextstatement = true;
                                                                do {
                                                                  evalcond[0] = ((IKabs(new_r20)) + (IKabs(new_r21)));
                                                                  if (IKabs(evalcond[0]) < 0.0000050000000000) {
                                                                    bgotonextstatement = false;
                                                                    {
                                                                      IkReal j5array[1], cj5array[1], sj5array[1];
                                                                      bool j5valid[1] = {false};
                                                                      _nj5 = 1;
                                                                      if (IKabs((-1.0) * new_r00) < IKFAST_ATAN2_MAGTHRESH && IKabs((-1.0) * new_r01) < IKFAST_ATAN2_MAGTHRESH &&
                                                                          IKabs(IKsqr((-1.0) * new_r00) + IKsqr((-1.0) * new_r01) - 1) <= IKFAST_SINCOS_THRESH) {
                                                                        continue;
                                                                      }
                                                                      j5array[0] = IKatan2(((-1.0) * new_r00), ((-1.0) * new_r01));
                                                                      sj5array[0] = IKsin(j5array[0]);
                                                                      cj5array[0] = IKcos(j5array[0]);
                                                                      if (j5array[0] > IKPI) {
                                                                        j5array[0] -= IK2PI;
                                                                      } else if (j5array[0] < -IKPI) {
                                                                        j5array[0] += IK2PI;
                                                                      }
                                                                      j5valid[0] = true;
                                                                      for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                                        if (!j5valid[ij5]) {
                                                                          continue;
                                                                        }
                                                                        _ij5[0] = ij5;
                                                                        _ij5[1] = -1;
                                                                        for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                                          if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                              IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                            j5valid[iij5] = false;
                                                                            _ij5[1] = iij5;
                                                                            break;
                                                                          }
                                                                        }
                                                                        j5 = j5array[ij5];
                                                                        cj5 = cj5array[ij5];
                                                                        sj5 = sj5array[ij5];
                                                                        {
                                                                          IkReal evalcond[6];
                                                                          IkReal x238 = IKsin(j5);
                                                                          IkReal x239 = IKcos(j5);
                                                                          IkReal x240 = ((-1.0) * x239);
                                                                          evalcond[0] = x238;
                                                                          evalcond[1] = (new_r22 * x238);
                                                                          evalcond[2] = x240;
                                                                          evalcond[3] = (new_r22 * x240);
                                                                          evalcond[4] = (((-1.0) * new_r00) + ((-1.0) * x238));
                                                                          evalcond[5] = (((-1.0) * new_r01) + ((-1.0) * x239));
                                                                          if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                              IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                                              IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH) {
                                                                            continue;
                                                                          }
                                                                        }

                                                                        {
                                                                          std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                                          vinfos[0].jointtype = 1;
                                                                          vinfos[0].foffset = j0;
                                                                          vinfos[0].indices[0] = _ij0[0];
                                                                          vinfos[0].indices[1] = _ij0[1];
                                                                          vinfos[0].maxsolutions = _nj0;
                                                                          vinfos[1].jointtype = 1;
                                                                          vinfos[1].foffset = j1;
                                                                          vinfos[1].indices[0] = _ij1[0];
                                                                          vinfos[1].indices[1] = _ij1[1];
                                                                          vinfos[1].maxsolutions = _nj1;
                                                                          vinfos[2].jointtype = 1;
                                                                          vinfos[2].foffset = j2;
                                                                          vinfos[2].indices[0] = _ij2[0];
                                                                          vinfos[2].indices[1] = _ij2[1];
                                                                          vinfos[2].maxsolutions = _nj2;
                                                                          vinfos[3].jointtype = 1;
                                                                          vinfos[3].foffset = j3;
                                                                          vinfos[3].indices[0] = _ij3[0];
                                                                          vinfos[3].indices[1] = _ij3[1];
                                                                          vinfos[3].maxsolutions = _nj3;
                                                                          vinfos[4].jointtype = 1;
                                                                          vinfos[4].foffset = j4;
                                                                          vinfos[4].indices[0] = _ij4[0];
                                                                          vinfos[4].indices[1] = _ij4[1];
                                                                          vinfos[4].maxsolutions = _nj4;
                                                                          vinfos[5].jointtype = 1;
                                                                          vinfos[5].foffset = j5;
                                                                          vinfos[5].indices[0] = _ij5[0];
                                                                          vinfos[5].indices[1] = _ij5[1];
                                                                          vinfos[5].maxsolutions = _nj5;
                                                                          std::vector<int> vfree(0);
                                                                          solutions.AddSolution(vinfos, vfree);
                                                                        }
                                                                      }
                                                                    }
                                                                  }
                                                                } while (0);
                                                                if (bgotonextstatement) {
                                                                  bool bgotonextstatement = true;
                                                                  do {
                                                                    if (1) {
                                                                      bgotonextstatement = false;
                                                                      continue; // branch miss [j5]
                                                                    }
                                                                  } while (0);
                                                                  if (bgotonextstatement) {
                                                                  }
                                                                }
                                                              }
                                                            }
                                                          }
                                                        }
                                                      }

                                                    } else {
                                                      {
                                                        IkReal j5array[1], cj5array[1], sj5array[1];
                                                        bool j5valid[1] = {false};
                                                        _nj5 = 1;
                                                        CheckValue<IkReal> x241 = IKPowWithIntegerCheck(sj4, -1);
                                                        if (!x241.valid)
                                                          continue;
                                                        if (IKabs((-1.0) * new_r00) < IKFAST_ATAN2_MAGTHRESH && IKabs((-1.0) * new_r20 * (x241.value)) < IKFAST_ATAN2_MAGTHRESH &&
                                                            IKabs(IKsqr((-1.0) * new_r00) + IKsqr((-1.0) * new_r20 * (x241.value)) - 1) <= IKFAST_SINCOS_THRESH) {
                                                          continue;
                                                        }
                                                        j5array[0] = IKatan2(((-1.0) * new_r00), ((-1.0) * new_r20 * (x241.value)));
                                                        sj5array[0] = IKsin(j5array[0]);
                                                        cj5array[0] = IKcos(j5array[0]);
                                                        if (j5array[0] > IKPI) {
                                                          j5array[0] -= IK2PI;
                                                        } else if (j5array[0] < -IKPI) {
                                                          j5array[0] += IK2PI;
                                                        }
                                                        j5valid[0] = true;
                                                        for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                          if (!j5valid[ij5])
                                                            continue;
                                                          _ij5[0] = ij5;
                                                          _ij5[1] = -1;
                                                          for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                            if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                              j5valid[iij5] = false;
                                                              _ij5[1] = iij5;
                                                              break;
                                                            }
                                                          }
                                                          j5 = j5array[ij5];
                                                          cj5 = cj5array[ij5];
                                                          sj5 = sj5array[ij5];
                                                          {
                                                            IkReal evalcond[8];
                                                            IkReal x242 = IKsin(j5);
                                                            IkReal x243 = IKcos(j5);
                                                            IkReal x244 = ((1.0) * sj4);
                                                            IkReal x245 = ((1.0) * x243);
                                                            IkReal x246 = ((1.0) * x242);
                                                            evalcond[0] = (new_r20 + (sj4 * x243));
                                                            evalcond[1] = ((cj4 * x242) + new_r11);
                                                            evalcond[2] = (((-1.0) * x242 * x244) + new_r21);
                                                            evalcond[3] = (((-1.0) * cj4 * x245) + new_r10);
                                                            evalcond[4] = (((-1.0) * new_r00) + ((-1.0) * x246));
                                                            evalcond[5] = (((-1.0) * new_r01) + ((-1.0) * x245));
                                                            evalcond[6] = ((cj4 * new_r11) + ((-1.0) * new_r21 * x244) + x242);
                                                            evalcond[7] = ((cj4 * new_r10) + ((-1.0) * new_r20 * x244) + ((-1.0) * x245));
                                                            if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                                IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                                IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                                              continue;
                                                            }
                                                          }

                                                          {
                                                            std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                            vinfos[0].jointtype = 1;
                                                            vinfos[0].foffset = j0;
                                                            vinfos[0].indices[0] = _ij0[0];
                                                            vinfos[0].indices[1] = _ij0[1];
                                                            vinfos[0].maxsolutions = _nj0;
                                                            vinfos[1].jointtype = 1;
                                                            vinfos[1].foffset = j1;
                                                            vinfos[1].indices[0] = _ij1[0];
                                                            vinfos[1].indices[1] = _ij1[1];
                                                            vinfos[1].maxsolutions = _nj1;
                                                            vinfos[2].jointtype = 1;
                                                            vinfos[2].foffset = j2;
                                                            vinfos[2].indices[0] = _ij2[0];
                                                            vinfos[2].indices[1] = _ij2[1];
                                                            vinfos[2].maxsolutions = _nj2;
                                                            vinfos[3].jointtype = 1;
                                                            vinfos[3].foffset = j3;
                                                            vinfos[3].indices[0] = _ij3[0];
                                                            vinfos[3].indices[1] = _ij3[1];
                                                            vinfos[3].maxsolutions = _nj3;
                                                            vinfos[4].jointtype = 1;
                                                            vinfos[4].foffset = j4;
                                                            vinfos[4].indices[0] = _ij4[0];
                                                            vinfos[4].indices[1] = _ij4[1];
                                                            vinfos[4].maxsolutions = _nj4;
                                                            vinfos[5].jointtype = 1;
                                                            vinfos[5].foffset = j5;
                                                            vinfos[5].indices[0] = _ij5[0];
                                                            vinfos[5].indices[1] = _ij5[1];
                                                            vinfos[5].maxsolutions = _nj5;
                                                            std::vector<int> vfree(0);
                                                            solutions.AddSolution(vinfos, vfree);
                                                          }
                                                        }
                                                      }
                                                    }
                                                  }

                                                } else {
                                                  {
                                                    IkReal j5array[1], cj5array[1], sj5array[1];
                                                    bool j5valid[1] = {false};
                                                    _nj5 = 1;
                                                    CheckValue<IkReal> x247 = IKPowWithIntegerCheck(IKsign(cj4), -1);
                                                    if (!x247.valid)
                                                      continue;
                                                    CheckValue<IkReal> x248 = IKatan2WithCheck(IkReal((-1.0) * new_r11), IkReal(new_r10), IKFAST_ATAN2_MAGTHRESH);
                                                    if (!x248.valid)
                                                      continue;
                                                    j5array[0] = ((-1.5707963267949) + ((1.5707963267949) * (x247.value)) + (x248.value));
                                                    sj5array[0] = IKsin(j5array[0]);
                                                    cj5array[0] = IKcos(j5array[0]);
                                                    if (j5array[0] > IKPI) {
                                                      j5array[0] -= IK2PI;
                                                    } else if (j5array[0] < -IKPI) {
                                                      j5array[0] += IK2PI;
                                                    }
                                                    j5valid[0] = true;
                                                    for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                      if (!j5valid[ij5])
                                                        continue;
                                                      _ij5[0] = ij5;
                                                      _ij5[1] = -1;
                                                      for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                        if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                            IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                          j5valid[iij5] = false;
                                                          _ij5[1] = iij5;
                                                          break;
                                                        }
                                                      }
                                                      j5 = j5array[ij5];
                                                      cj5 = cj5array[ij5];
                                                      sj5 = sj5array[ij5];
                                                      {
                                                        IkReal evalcond[8];
                                                        IkReal x249 = IKsin(j5);
                                                        IkReal x250 = IKcos(j5);
                                                        IkReal x251 = ((1.0) * sj4);
                                                        IkReal x252 = ((1.0) * x250);
                                                        IkReal x253 = ((1.0) * x249);
                                                        evalcond[0] = ((sj4 * x250) + new_r20);
                                                        evalcond[1] = ((cj4 * x249) + new_r11);
                                                        evalcond[2] = (((-1.0) * x249 * x251) + new_r21);
                                                        evalcond[3] = (((-1.0) * cj4 * x252) + new_r10);
                                                        evalcond[4] = (((-1.0) * new_r00) + ((-1.0) * x253));
                                                        evalcond[5] = (((-1.0) * new_r01) + ((-1.0) * x252));
                                                        evalcond[6] = (((-1.0) * new_r21 * x251) + (cj4 * new_r11) + x249);
                                                        evalcond[7] = ((cj4 * new_r10) + ((-1.0) * new_r20 * x251) + ((-1.0) * x252));
                                                        if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                            IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                            IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                            IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                                          continue;
                                                        }
                                                      }

                                                      {
                                                        std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                        vinfos[0].jointtype = 1;
                                                        vinfos[0].foffset = j0;
                                                        vinfos[0].indices[0] = _ij0[0];
                                                        vinfos[0].indices[1] = _ij0[1];
                                                        vinfos[0].maxsolutions = _nj0;
                                                        vinfos[1].jointtype = 1;
                                                        vinfos[1].foffset = j1;
                                                        vinfos[1].indices[0] = _ij1[0];
                                                        vinfos[1].indices[1] = _ij1[1];
                                                        vinfos[1].maxsolutions = _nj1;
                                                        vinfos[2].jointtype = 1;
                                                        vinfos[2].foffset = j2;
                                                        vinfos[2].indices[0] = _ij2[0];
                                                        vinfos[2].indices[1] = _ij2[1];
                                                        vinfos[2].maxsolutions = _nj2;
                                                        vinfos[3].jointtype = 1;
                                                        vinfos[3].foffset = j3;
                                                        vinfos[3].indices[0] = _ij3[0];
                                                        vinfos[3].indices[1] = _ij3[1];
                                                        vinfos[3].maxsolutions = _nj3;
                                                        vinfos[4].jointtype = 1;
                                                        vinfos[4].foffset = j4;
                                                        vinfos[4].indices[0] = _ij4[0];
                                                        vinfos[4].indices[1] = _ij4[1];
                                                        vinfos[4].maxsolutions = _nj4;
                                                        vinfos[5].jointtype = 1;
                                                        vinfos[5].foffset = j5;
                                                        vinfos[5].indices[0] = _ij5[0];
                                                        vinfos[5].indices[1] = _ij5[1];
                                                        vinfos[5].maxsolutions = _nj5;
                                                        std::vector<int> vfree(0);
                                                        solutions.AddSolution(vinfos, vfree);
                                                      }
                                                    }
                                                  }
                                                }
                                              }

                                            } else {
                                              {
                                                IkReal j5array[1], cj5array[1], sj5array[1];
                                                bool j5valid[1] = {false};
                                                _nj5 = 1;
                                                CheckValue<IkReal> x254 = IKPowWithIntegerCheck(IKsign(sj4), -1);
                                                if (!x254.valid)
                                                  continue;
                                                CheckValue<IkReal> x255 = IKatan2WithCheck(IkReal(new_r21), IkReal((-1.0) * new_r20), IKFAST_ATAN2_MAGTHRESH);
                                                if (!x255.valid)
                                                  continue;
                                                j5array[0] = ((-1.5707963267949) + ((1.5707963267949) * (x254.value)) + (x255.value));
                                                sj5array[0] = IKsin(j5array[0]);
                                                cj5array[0] = IKcos(j5array[0]);
                                                if (j5array[0] > IKPI)
                                                  j5array[0] -= IK2PI;
                                                else if (j5array[0] < -IKPI)
                                                  j5array[0] += IK2PI;
                                                j5valid[0] = true;
                                                for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                  if (!j5valid[ij5])
                                                    continue;
                                                  _ij5[0] = ij5;
                                                  _ij5[1] = -1;
                                                  for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                    if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                        IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                      j5valid[iij5] = false;
                                                      _ij5[1] = iij5;
                                                      break;
                                                    }
                                                  }
                                                  j5 = j5array[ij5];
                                                  cj5 = cj5array[ij5];
                                                  sj5 = sj5array[ij5];
                                                  {
                                                    IkReal evalcond[8];
                                                    IkReal x256 = IKsin(j5);
                                                    IkReal x257 = IKcos(j5);
                                                    IkReal x258 = ((1.0) * sj4);
                                                    IkReal x259 = ((1.0) * x257);
                                                    IkReal x260 = ((1.0) * x256);
                                                    evalcond[0] = ((sj4 * x257) + new_r20);
                                                    evalcond[1] = (new_r11 + (cj4 * x256));
                                                    evalcond[2] = (((-1.0) * x256 * x258) + new_r21);
                                                    evalcond[3] = (((-1.0) * cj4 * x259) + new_r10);
                                                    evalcond[4] = (((-1.0) * new_r00) + ((-1.0) * x260));
                                                    evalcond[5] = (((-1.0) * new_r01) + ((-1.0) * x259));
                                                    evalcond[6] = (((-1.0) * new_r21 * x258) + (cj4 * new_r11) + x256);
                                                    evalcond[7] = ((cj4 * new_r10) + ((-1.0) * new_r20 * x258) + ((-1.0) * x259));
                                                    if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                        IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                        IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                        IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                                      continue;
                                                    }
                                                  }

                                                  {
                                                    std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                    vinfos[0].jointtype = 1;
                                                    vinfos[0].foffset = j0;
                                                    vinfos[0].indices[0] = _ij0[0];
                                                    vinfos[0].indices[1] = _ij0[1];
                                                    vinfos[0].maxsolutions = _nj0;
                                                    vinfos[1].jointtype = 1;
                                                    vinfos[1].foffset = j1;
                                                    vinfos[1].indices[0] = _ij1[0];
                                                    vinfos[1].indices[1] = _ij1[1];
                                                    vinfos[1].maxsolutions = _nj1;
                                                    vinfos[2].jointtype = 1;
                                                    vinfos[2].foffset = j2;
                                                    vinfos[2].indices[0] = _ij2[0];
                                                    vinfos[2].indices[1] = _ij2[1];
                                                    vinfos[2].maxsolutions = _nj2;
                                                    vinfos[3].jointtype = 1;
                                                    vinfos[3].foffset = j3;
                                                    vinfos[3].indices[0] = _ij3[0];
                                                    vinfos[3].indices[1] = _ij3[1];
                                                    vinfos[3].maxsolutions = _nj3;
                                                    vinfos[4].jointtype = 1;
                                                    vinfos[4].foffset = j4;
                                                    vinfos[4].indices[0] = _ij4[0];
                                                    vinfos[4].indices[1] = _ij4[1];
                                                    vinfos[4].maxsolutions = _nj4;
                                                    vinfos[5].jointtype = 1;
                                                    vinfos[5].foffset = j5;
                                                    vinfos[5].indices[0] = _ij5[0];
                                                    vinfos[5].indices[1] = _ij5[1];
                                                    vinfos[5].maxsolutions = _nj5;
                                                    std::vector<int> vfree(0);
                                                    solutions.AddSolution(vinfos, vfree);
                                                  }
                                                }
                                              }
                                            }
                                          }
                                        }
                                      } while (0);
                                      if (bgotonextstatement) {
                                        bool bgotonextstatement = true;
                                        do {
                                          evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs((1.5707963267949) + j3))), 6.28318530717959)));
                                          evalcond[1] = new_r02;
                                          if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000) {
                                            bgotonextstatement = false;
                                            {
                                              IkReal j5array[1], cj5array[1], sj5array[1];
                                              bool j5valid[1] = {false};
                                              _nj5 = 1;
                                              if (IKabs(new_r00) < IKFAST_ATAN2_MAGTHRESH && IKabs(new_r01) < IKFAST_ATAN2_MAGTHRESH &&
                                                  IKabs(IKsqr(new_r00) + IKsqr(new_r01) - 1) <= IKFAST_SINCOS_THRESH) {
                                                continue;
                                              }
                                              j5array[0] = IKatan2(new_r00, new_r01);
                                              sj5array[0] = IKsin(j5array[0]);
                                              cj5array[0] = IKcos(j5array[0]);
                                              if (j5array[0] > IKPI)
                                                j5array[0] -= IK2PI;
                                              else if (j5array[0] < -IKPI)
                                                j5array[0] += IK2PI;
                                              j5valid[0] = true;
                                              for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                if (!j5valid[ij5])
                                                  continue;
                                                _ij5[0] = ij5;
                                                _ij5[1] = -1;
                                                for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                  if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                      IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                    j5valid[iij5] = false;
                                                    _ij5[1] = iij5;
                                                    break;
                                                  }
                                                }
                                                j5 = j5array[ij5];
                                                cj5 = cj5array[ij5];
                                                sj5 = sj5array[ij5];
                                                {
                                                  IkReal evalcond[8];
                                                  IkReal x261 = IKcos(j5);
                                                  IkReal x262 = IKsin(j5);
                                                  IkReal x263 = ((1.0) * cj4);
                                                  IkReal x264 = ((1.0) * sj4);
                                                  IkReal x265 = ((1.0) * x261);
                                                  IkReal x266 = ((1.0) * x262);
                                                  evalcond[0] = (new_r20 + (sj4 * x261));
                                                  evalcond[1] = (new_r00 + ((-1.0) * x266));
                                                  evalcond[2] = (new_r01 + ((-1.0) * x265));
                                                  evalcond[3] = (((-1.0) * x262 * x264) + new_r21);
                                                  evalcond[4] = ((cj4 * x262) + ((-1.0) * new_r11));
                                                  evalcond[5] = (((-1.0) * x261 * x263) + ((-1.0) * new_r10));
                                                  evalcond[6] = (((-1.0) * new_r21 * x264) + x262 + ((-1.0) * new_r11 * x263));
                                                  evalcond[7] = (((-1.0) * new_r10 * x263) + ((-1.0) * new_r20 * x264) + ((-1.0) * x265));
                                                  if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                      IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                      IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                      IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                                    continue;
                                                  }
                                                }

                                                {
                                                  std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                  vinfos[0].jointtype = 1;
                                                  vinfos[0].foffset = j0;
                                                  vinfos[0].indices[0] = _ij0[0];
                                                  vinfos[0].indices[1] = _ij0[1];
                                                  vinfos[0].maxsolutions = _nj0;
                                                  vinfos[1].jointtype = 1;
                                                  vinfos[1].foffset = j1;
                                                  vinfos[1].indices[0] = _ij1[0];
                                                  vinfos[1].indices[1] = _ij1[1];
                                                  vinfos[1].maxsolutions = _nj1;
                                                  vinfos[2].jointtype = 1;
                                                  vinfos[2].foffset = j2;
                                                  vinfos[2].indices[0] = _ij2[0];
                                                  vinfos[2].indices[1] = _ij2[1];
                                                  vinfos[2].maxsolutions = _nj2;
                                                  vinfos[3].jointtype = 1;
                                                  vinfos[3].foffset = j3;
                                                  vinfos[3].indices[0] = _ij3[0];
                                                  vinfos[3].indices[1] = _ij3[1];
                                                  vinfos[3].maxsolutions = _nj3;
                                                  vinfos[4].jointtype = 1;
                                                  vinfos[4].foffset = j4;
                                                  vinfos[4].indices[0] = _ij4[0];
                                                  vinfos[4].indices[1] = _ij4[1];
                                                  vinfos[4].maxsolutions = _nj4;
                                                  vinfos[5].jointtype = 1;
                                                  vinfos[5].foffset = j5;
                                                  vinfos[5].indices[0] = _ij5[0];
                                                  vinfos[5].indices[1] = _ij5[1];
                                                  vinfos[5].maxsolutions = _nj5;
                                                  std::vector<int> vfree(0);
                                                  solutions.AddSolution(vinfos, vfree);
                                                }
                                              }
                                            }
                                          }
                                        } while (0);
                                        if (bgotonextstatement) {
                                          bool bgotonextstatement = true;
                                          do {
                                            evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs((-1.5707963267949) + j4))), 6.28318530717959)));
                                            evalcond[1] = new_r22;
                                            if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000) {
                                              bgotonextstatement = false;
                                              {
                                                IkReal j5array[1], cj5array[1], sj5array[1];
                                                bool j5valid[1] = {false};
                                                _nj5 = 1;
                                                if (IKabs(new_r21) < IKFAST_ATAN2_MAGTHRESH && IKabs((-1.0) * new_r20) < IKFAST_ATAN2_MAGTHRESH &&
                                                    IKabs(IKsqr(new_r21) + IKsqr((-1.0) * new_r20) - 1) <= IKFAST_SINCOS_THRESH) {
                                                  continue;
                                                }
                                                j5array[0] = IKatan2(new_r21, ((-1.0) * new_r20));
                                                sj5array[0] = IKsin(j5array[0]);
                                                cj5array[0] = IKcos(j5array[0]);
                                                if (j5array[0] > IKPI)
                                                  j5array[0] -= IK2PI;
                                                else if (j5array[0] < -IKPI)
                                                  j5array[0] += IK2PI;
                                                j5valid[0] = true;
                                                for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                  if (!j5valid[ij5])
                                                    continue;
                                                  _ij5[0] = ij5;
                                                  _ij5[1] = -1;
                                                  for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                    if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                        IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                      j5valid[iij5] = false;
                                                      _ij5[1] = iij5;
                                                      break;
                                                    }
                                                  }
                                                  j5 = j5array[ij5];
                                                  cj5 = cj5array[ij5];
                                                  sj5 = sj5array[ij5];
                                                  {
                                                    IkReal evalcond[8];
                                                    IkReal x267 = IKcos(j5);
                                                    IkReal x268 = IKsin(j5);
                                                    IkReal x269 = ((1.0) * sj3);
                                                    IkReal x270 = ((1.0) * x268);
                                                    IkReal x271 = ((1.0) * x267);
                                                    evalcond[0] = (x267 + new_r20);
                                                    evalcond[1] = (((-1.0) * x270) + new_r21);
                                                    evalcond[2] = ((sj3 * x267) + new_r01);
                                                    evalcond[3] = ((sj3 * x268) + new_r00);
                                                    evalcond[4] = (((-1.0) * cj3 * x271) + new_r11);
                                                    evalcond[5] = (((-1.0) * new_r02 * x270) + new_r10);
                                                    evalcond[6] = (((-1.0) * new_r00 * x269) + (cj3 * new_r10) + ((-1.0) * x270));
                                                    evalcond[7] = (((-1.0) * new_r01 * x269) + (cj3 * new_r11) + ((-1.0) * x271));
                                                    if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                        IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                        IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                        IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                                      continue;
                                                    }
                                                  }

                                                  {
                                                    std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                    vinfos[0].jointtype = 1;
                                                    vinfos[0].foffset = j0;
                                                    vinfos[0].indices[0] = _ij0[0];
                                                    vinfos[0].indices[1] = _ij0[1];
                                                    vinfos[0].maxsolutions = _nj0;
                                                    vinfos[1].jointtype = 1;
                                                    vinfos[1].foffset = j1;
                                                    vinfos[1].indices[0] = _ij1[0];
                                                    vinfos[1].indices[1] = _ij1[1];
                                                    vinfos[1].maxsolutions = _nj1;
                                                    vinfos[2].jointtype = 1;
                                                    vinfos[2].foffset = j2;
                                                    vinfos[2].indices[0] = _ij2[0];
                                                    vinfos[2].indices[1] = _ij2[1];
                                                    vinfos[2].maxsolutions = _nj2;
                                                    vinfos[3].jointtype = 1;
                                                    vinfos[3].foffset = j3;
                                                    vinfos[3].indices[0] = _ij3[0];
                                                    vinfos[3].indices[1] = _ij3[1];
                                                    vinfos[3].maxsolutions = _nj3;
                                                    vinfos[4].jointtype = 1;
                                                    vinfos[4].foffset = j4;
                                                    vinfos[4].indices[0] = _ij4[0];
                                                    vinfos[4].indices[1] = _ij4[1];
                                                    vinfos[4].maxsolutions = _nj4;
                                                    vinfos[5].jointtype = 1;
                                                    vinfos[5].foffset = j5;
                                                    vinfos[5].indices[0] = _ij5[0];
                                                    vinfos[5].indices[1] = _ij5[1];
                                                    vinfos[5].maxsolutions = _nj5;
                                                    std::vector<int> vfree(0);
                                                    solutions.AddSolution(vinfos, vfree);
                                                  }
                                                }
                                              }
                                            }
                                          } while (0);
                                          if (bgotonextstatement) {
                                            bool bgotonextstatement = true;
                                            do {
                                              evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs((1.5707963267949) + j4))), 6.28318530717959)));
                                              evalcond[1] = new_r22;
                                              if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000) {
                                                bgotonextstatement = false;
                                                {
                                                  IkReal j5array[1], cj5array[1], sj5array[1];
                                                  bool j5valid[1] = {false};
                                                  _nj5 = 1;
                                                  if (IKabs((-1.0) * new_r21) < IKFAST_ATAN2_MAGTHRESH && IKabs(new_r20) < IKFAST_ATAN2_MAGTHRESH &&
                                                      IKabs(IKsqr((-1.0) * new_r21) + IKsqr(new_r20) - 1) <= IKFAST_SINCOS_THRESH) {
                                                    continue;
                                                  }
                                                  j5array[0] = IKatan2(((-1.0) * new_r21), new_r20);
                                                  sj5array[0] = IKsin(j5array[0]);
                                                  cj5array[0] = IKcos(j5array[0]);
                                                  if (j5array[0] > IKPI)
                                                    j5array[0] -= IK2PI;
                                                  else if (j5array[0] < -IKPI)
                                                    j5array[0] += IK2PI;
                                                  j5valid[0] = true;
                                                  for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                    if (!j5valid[ij5])
                                                      continue;
                                                    _ij5[0] = ij5;
                                                    _ij5[1] = -1;
                                                    for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                      if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                          IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                        j5valid[iij5] = false;
                                                        _ij5[1] = iij5;
                                                        break;
                                                      }
                                                    }
                                                    j5 = j5array[ij5];
                                                    cj5 = cj5array[ij5];
                                                    sj5 = sj5array[ij5];
                                                    {
                                                      IkReal evalcond[8];
                                                      IkReal x272 = IKcos(j5);
                                                      IkReal x273 = IKsin(j5);
                                                      IkReal x274 = ((1.0) * sj3);
                                                      IkReal x275 = ((1.0) * x272);
                                                      evalcond[0] = (x273 + new_r21);
                                                      evalcond[1] = (((-1.0) * x275) + new_r20);
                                                      evalcond[2] = ((sj3 * x272) + new_r01);
                                                      evalcond[3] = ((sj3 * x273) + new_r00);
                                                      evalcond[4] = (new_r10 + (new_r02 * x273));
                                                      evalcond[5] = (((-1.0) * cj3 * x275) + new_r11);
                                                      evalcond[6] = (((-1.0) * x273) + ((-1.0) * new_r00 * x274) + (cj3 * new_r10));
                                                      evalcond[7] = (((-1.0) * new_r01 * x274) + (cj3 * new_r11) + ((-1.0) * x275));
                                                      if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                          IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                          IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                          IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                                        continue;
                                                      }
                                                    }

                                                    {
                                                      std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                      vinfos[0].jointtype = 1;
                                                      vinfos[0].foffset = j0;
                                                      vinfos[0].indices[0] = _ij0[0];
                                                      vinfos[0].indices[1] = _ij0[1];
                                                      vinfos[0].maxsolutions = _nj0;
                                                      vinfos[1].jointtype = 1;
                                                      vinfos[1].foffset = j1;
                                                      vinfos[1].indices[0] = _ij1[0];
                                                      vinfos[1].indices[1] = _ij1[1];
                                                      vinfos[1].maxsolutions = _nj1;
                                                      vinfos[2].jointtype = 1;
                                                      vinfos[2].foffset = j2;
                                                      vinfos[2].indices[0] = _ij2[0];
                                                      vinfos[2].indices[1] = _ij2[1];
                                                      vinfos[2].maxsolutions = _nj2;
                                                      vinfos[3].jointtype = 1;
                                                      vinfos[3].foffset = j3;
                                                      vinfos[3].indices[0] = _ij3[0];
                                                      vinfos[3].indices[1] = _ij3[1];
                                                      vinfos[3].maxsolutions = _nj3;
                                                      vinfos[4].jointtype = 1;
                                                      vinfos[4].foffset = j4;
                                                      vinfos[4].indices[0] = _ij4[0];
                                                      vinfos[4].indices[1] = _ij4[1];
                                                      vinfos[4].maxsolutions = _nj4;
                                                      vinfos[5].jointtype = 1;
                                                      vinfos[5].foffset = j5;
                                                      vinfos[5].indices[0] = _ij5[0];
                                                      vinfos[5].indices[1] = _ij5[1];
                                                      vinfos[5].maxsolutions = _nj5;
                                                      std::vector<int> vfree(0);
                                                      solutions.AddSolution(vinfos, vfree);
                                                    }
                                                  }
                                                }
                                              }
                                            } while (0);
                                            if (bgotonextstatement) {
                                              bool bgotonextstatement = true;
                                              do {
                                                evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs(j4))), 6.28318530717959)));
                                                evalcond[1] = new_r20;
                                                evalcond[2] = new_r02;
                                                evalcond[3] = new_r12;
                                                evalcond[4] = new_r21;
                                                if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000 && IKabs(evalcond[2]) < 0.0000050000000000 &&
                                                    IKabs(evalcond[3]) < 0.0000050000000000 && IKabs(evalcond[4]) < 0.0000050000000000) {
                                                  bgotonextstatement = false;
                                                  {
                                                    IkReal j5array[1], cj5array[1], sj5array[1];
                                                    bool j5valid[1] = {false};
                                                    _nj5 = 1;
                                                    IkReal x276 = ((1.0) * new_r01);
                                                    if (IKabs(((-1.0) * cj3 * x276) + ((-1.0) * new_r00 * sj3)) < IKFAST_ATAN2_MAGTHRESH &&
                                                        IKabs(((-1.0) * sj3 * x276) + (cj3 * new_r00)) < IKFAST_ATAN2_MAGTHRESH &&
                                                        IKabs(IKsqr(((-1.0) * cj3 * x276) + ((-1.0) * new_r00 * sj3)) + IKsqr(((-1.0) * sj3 * x276) + (cj3 * new_r00)) - 1) <=
                                                          IKFAST_SINCOS_THRESH) {
                                                      continue;
                                                    }
                                                    j5array[0] = IKatan2((((-1.0) * cj3 * x276) + ((-1.0) * new_r00 * sj3)), (((-1.0) * sj3 * x276) + (cj3 * new_r00)));
                                                    sj5array[0] = IKsin(j5array[0]);
                                                    cj5array[0] = IKcos(j5array[0]);
                                                    if (j5array[0] > IKPI) {
                                                      j5array[0] -= IK2PI;
                                                    } else if (j5array[0] < -IKPI) {
                                                      j5array[0] += IK2PI;
                                                    }
                                                    j5valid[0] = true;
                                                    for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                      if (!j5valid[ij5])
                                                        continue;
                                                      _ij5[0] = ij5;
                                                      _ij5[1] = -1;
                                                      for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                        if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                            IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                          j5valid[iij5] = false;
                                                          _ij5[1] = iij5;
                                                          break;
                                                        }
                                                      }
                                                      j5 = j5array[ij5];
                                                      cj5 = cj5array[ij5];
                                                      sj5 = sj5array[ij5];
                                                      {
                                                        IkReal evalcond[8];
                                                        IkReal x277 = IKsin(j5);
                                                        IkReal x278 = IKcos(j5);
                                                        IkReal x279 = ((1.0) * sj3);
                                                        IkReal x280 = ((1.0) * x278);
                                                        IkReal x281 = (sj3 * x277);
                                                        IkReal x282 = ((1.0) * x277);
                                                        IkReal x283 = (cj3 * x280);
                                                        evalcond[0] = ((new_r11 * sj3) + x277 + (cj3 * new_r01));
                                                        evalcond[1] = ((sj3 * x278) + new_r01 + (cj3 * x277));
                                                        evalcond[2] = ((new_r10 * sj3) + (cj3 * new_r00) + ((-1.0) * x280));
                                                        evalcond[3] = (((-1.0) * new_r00 * x279) + (cj3 * new_r10) + ((-1.0) * x282));
                                                        evalcond[4] = (((-1.0) * new_r01 * x279) + (cj3 * new_r11) + ((-1.0) * x280));
                                                        evalcond[5] = (x281 + ((-1.0) * x283) + new_r00);
                                                        evalcond[6] = (x281 + ((-1.0) * x283) + new_r11);
                                                        evalcond[7] = (((-1.0) * cj3 * x282) + ((-1.0) * x278 * x279) + new_r10);
                                                        if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                            IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                            IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                            IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                                          continue;
                                                        }
                                                      }

                                                      {
                                                        std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                        vinfos[0].jointtype = 1;
                                                        vinfos[0].foffset = j0;
                                                        vinfos[0].indices[0] = _ij0[0];
                                                        vinfos[0].indices[1] = _ij0[1];
                                                        vinfos[0].maxsolutions = _nj0;
                                                        vinfos[1].jointtype = 1;
                                                        vinfos[1].foffset = j1;
                                                        vinfos[1].indices[0] = _ij1[0];
                                                        vinfos[1].indices[1] = _ij1[1];
                                                        vinfos[1].maxsolutions = _nj1;
                                                        vinfos[2].jointtype = 1;
                                                        vinfos[2].foffset = j2;
                                                        vinfos[2].indices[0] = _ij2[0];
                                                        vinfos[2].indices[1] = _ij2[1];
                                                        vinfos[2].maxsolutions = _nj2;
                                                        vinfos[3].jointtype = 1;
                                                        vinfos[3].foffset = j3;
                                                        vinfos[3].indices[0] = _ij3[0];
                                                        vinfos[3].indices[1] = _ij3[1];
                                                        vinfos[3].maxsolutions = _nj3;
                                                        vinfos[4].jointtype = 1;
                                                        vinfos[4].foffset = j4;
                                                        vinfos[4].indices[0] = _ij4[0];
                                                        vinfos[4].indices[1] = _ij4[1];
                                                        vinfos[4].maxsolutions = _nj4;
                                                        vinfos[5].jointtype = 1;
                                                        vinfos[5].foffset = j5;
                                                        vinfos[5].indices[0] = _ij5[0];
                                                        vinfos[5].indices[1] = _ij5[1];
                                                        vinfos[5].maxsolutions = _nj5;
                                                        std::vector<int> vfree(0);
                                                        solutions.AddSolution(vinfos, vfree);
                                                      }
                                                    }
                                                  }
                                                }
                                              } while (0);
                                              if (bgotonextstatement) {
                                                bool bgotonextstatement = true;
                                                do {
                                                  evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs((-3.14159265358979) + j4))), 6.28318530717959)));
                                                  evalcond[1] = new_r20;
                                                  evalcond[2] = new_r02;
                                                  evalcond[3] = new_r12;
                                                  evalcond[4] = new_r21;
                                                  if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000 &&
                                                      IKabs(evalcond[2]) < 0.0000050000000000 && IKabs(evalcond[3]) < 0.0000050000000000 &&
                                                      IKabs(evalcond[4]) < 0.0000050000000000) {
                                                    bgotonextstatement = false;
                                                    {
                                                      IkReal j5array[1], cj5array[1], sj5array[1];
                                                      bool j5valid[1] = {false};
                                                      _nj5 = 1;
                                                      IkReal x284 = ((1.0) * sj3);
                                                      if (IKabs(((-1.0) * new_r00 * x284) + (cj3 * new_r01)) < IKFAST_ATAN2_MAGTHRESH &&
                                                          IKabs(((-1.0) * cj3 * new_r00) + ((-1.0) * new_r01 * x284)) < IKFAST_ATAN2_MAGTHRESH &&
                                                          IKabs(IKsqr(((-1.0) * new_r00 * x284) + (cj3 * new_r01)) + IKsqr(((-1.0) * cj3 * new_r00) + ((-1.0) * new_r01 * x284)) -
                                                                1) <= IKFAST_SINCOS_THRESH) {
                                                        continue;
                                                      }
                                                      j5array[0] = IKatan2((((-1.0) * new_r00 * x284) + (cj3 * new_r01)), (((-1.0) * cj3 * new_r00) + ((-1.0) * new_r01 * x284)));
                                                      sj5array[0] = IKsin(j5array[0]);
                                                      cj5array[0] = IKcos(j5array[0]);
                                                      if (j5array[0] > IKPI) {
                                                        j5array[0] -= IK2PI;
                                                      } else if (j5array[0] < -IKPI) {
                                                        j5array[0] += IK2PI;
                                                      }
                                                      j5valid[0] = true;
                                                      for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                        if (!j5valid[ij5])
                                                          continue;
                                                        _ij5[0] = ij5;
                                                        _ij5[1] = -1;
                                                        for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                          if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                              IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                            j5valid[iij5] = false;
                                                            _ij5[1] = iij5;
                                                            break;
                                                          }
                                                        }
                                                        j5 = j5array[ij5];
                                                        cj5 = cj5array[ij5];
                                                        sj5 = sj5array[ij5];
                                                        {
                                                          IkReal evalcond[8];
                                                          IkReal x285 = IKsin(j5);
                                                          IkReal x286 = IKcos(j5);
                                                          IkReal x287 = ((1.0) * sj3);
                                                          IkReal x288 = ((1.0) * x285);
                                                          IkReal x289 = (sj3 * x286);
                                                          IkReal x290 = ((1.0) * x286);
                                                          IkReal x291 = (cj3 * x288);
                                                          evalcond[0] = ((new_r10 * sj3) + x286 + (cj3 * new_r00));
                                                          evalcond[1] = ((new_r11 * sj3) + (cj3 * new_r01) + ((-1.0) * x288));
                                                          evalcond[2] = ((sj3 * x285) + (cj3 * x286) + new_r00);
                                                          evalcond[3] = (((-1.0) * new_r00 * x287) + (cj3 * new_r10) + ((-1.0) * x288));
                                                          evalcond[4] = (((-1.0) * x290) + ((-1.0) * new_r01 * x287) + (cj3 * new_r11));
                                                          evalcond[5] = (((-1.0) * x291) + x289 + new_r01);
                                                          evalcond[6] = (((-1.0) * x291) + x289 + new_r10);
                                                          evalcond[7] = (((-1.0) * cj3 * x290) + ((-1.0) * x285 * x287) + new_r11);
                                                          if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                              IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                              IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                              IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                                            continue;
                                                          }
                                                        }

                                                        {
                                                          std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                          vinfos[0].jointtype = 1;
                                                          vinfos[0].foffset = j0;
                                                          vinfos[0].indices[0] = _ij0[0];
                                                          vinfos[0].indices[1] = _ij0[1];
                                                          vinfos[0].maxsolutions = _nj0;
                                                          vinfos[1].jointtype = 1;
                                                          vinfos[1].foffset = j1;
                                                          vinfos[1].indices[0] = _ij1[0];
                                                          vinfos[1].indices[1] = _ij1[1];
                                                          vinfos[1].maxsolutions = _nj1;
                                                          vinfos[2].jointtype = 1;
                                                          vinfos[2].foffset = j2;
                                                          vinfos[2].indices[0] = _ij2[0];
                                                          vinfos[2].indices[1] = _ij2[1];
                                                          vinfos[2].maxsolutions = _nj2;
                                                          vinfos[3].jointtype = 1;
                                                          vinfos[3].foffset = j3;
                                                          vinfos[3].indices[0] = _ij3[0];
                                                          vinfos[3].indices[1] = _ij3[1];
                                                          vinfos[3].maxsolutions = _nj3;
                                                          vinfos[4].jointtype = 1;
                                                          vinfos[4].foffset = j4;
                                                          vinfos[4].indices[0] = _ij4[0];
                                                          vinfos[4].indices[1] = _ij4[1];
                                                          vinfos[4].maxsolutions = _nj4;
                                                          vinfos[5].jointtype = 1;
                                                          vinfos[5].foffset = j5;
                                                          vinfos[5].indices[0] = _ij5[0];
                                                          vinfos[5].indices[1] = _ij5[1];
                                                          vinfos[5].maxsolutions = _nj5;
                                                          std::vector<int> vfree(0);
                                                          solutions.AddSolution(vinfos, vfree);
                                                        }
                                                      }
                                                    }
                                                  }
                                                } while (0);
                                                if (bgotonextstatement) {
                                                  bool bgotonextstatement = true;
                                                  do {
                                                    evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs(j3))), 6.28318530717959)));
                                                    evalcond[1] = new_r12;
                                                    if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000) {
                                                      bgotonextstatement = false;
                                                      {
                                                        IkReal j5array[1], cj5array[1], sj5array[1];
                                                        bool j5valid[1] = {false};
                                                        _nj5 = 1;
                                                        if (IKabs(new_r10) < IKFAST_ATAN2_MAGTHRESH && IKabs(new_r11) < IKFAST_ATAN2_MAGTHRESH &&
                                                            IKabs(IKsqr(new_r10) + IKsqr(new_r11) - 1) <= IKFAST_SINCOS_THRESH) {
                                                          continue;
                                                        }
                                                        j5array[0] = IKatan2(new_r10, new_r11);
                                                        sj5array[0] = IKsin(j5array[0]);
                                                        cj5array[0] = IKcos(j5array[0]);
                                                        if (j5array[0] > IKPI) {
                                                          j5array[0] -= IK2PI;
                                                        } else if (j5array[0] < -IKPI) {
                                                          j5array[0] += IK2PI;
                                                        }
                                                        j5valid[0] = true;
                                                        for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                          if (!j5valid[ij5])
                                                            continue;
                                                          _ij5[0] = ij5;
                                                          _ij5[1] = -1;
                                                          for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                            if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                              j5valid[iij5] = false;
                                                              _ij5[1] = iij5;
                                                              break;
                                                            }
                                                          }
                                                          j5 = j5array[ij5];
                                                          cj5 = cj5array[ij5];
                                                          sj5 = sj5array[ij5];
                                                          {
                                                            IkReal evalcond[8];
                                                            IkReal x292 = IKcos(j5);
                                                            IkReal x293 = IKsin(j5);
                                                            IkReal x294 = ((1.0) * sj4);
                                                            IkReal x295 = ((1.0) * x292);
                                                            IkReal x296 = ((1.0) * x293);
                                                            evalcond[0] = ((new_r02 * x292) + new_r20);
                                                            evalcond[1] = (((-1.0) * x296) + new_r10);
                                                            evalcond[2] = (((-1.0) * x295) + new_r11);
                                                            evalcond[3] = ((cj4 * x293) + new_r01);
                                                            evalcond[4] = (new_r21 + ((-1.0) * new_r02 * x296));
                                                            evalcond[5] = (((-1.0) * cj4 * x295) + new_r00);
                                                            evalcond[6] = ((cj4 * new_r01) + ((-1.0) * new_r21 * x294) + x293);
                                                            evalcond[7] = ((cj4 * new_r00) + ((-1.0) * new_r20 * x294) + ((-1.0) * x295));
                                                            if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                                IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                                IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                                              continue;
                                                            }
                                                          }

                                                          {
                                                            std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                            vinfos[0].jointtype = 1;
                                                            vinfos[0].foffset = j0;
                                                            vinfos[0].indices[0] = _ij0[0];
                                                            vinfos[0].indices[1] = _ij0[1];
                                                            vinfos[0].maxsolutions = _nj0;
                                                            vinfos[1].jointtype = 1;
                                                            vinfos[1].foffset = j1;
                                                            vinfos[1].indices[0] = _ij1[0];
                                                            vinfos[1].indices[1] = _ij1[1];
                                                            vinfos[1].maxsolutions = _nj1;
                                                            vinfos[2].jointtype = 1;
                                                            vinfos[2].foffset = j2;
                                                            vinfos[2].indices[0] = _ij2[0];
                                                            vinfos[2].indices[1] = _ij2[1];
                                                            vinfos[2].maxsolutions = _nj2;
                                                            vinfos[3].jointtype = 1;
                                                            vinfos[3].foffset = j3;
                                                            vinfos[3].indices[0] = _ij3[0];
                                                            vinfos[3].indices[1] = _ij3[1];
                                                            vinfos[3].maxsolutions = _nj3;
                                                            vinfos[4].jointtype = 1;
                                                            vinfos[4].foffset = j4;
                                                            vinfos[4].indices[0] = _ij4[0];
                                                            vinfos[4].indices[1] = _ij4[1];
                                                            vinfos[4].maxsolutions = _nj4;
                                                            vinfos[5].jointtype = 1;
                                                            vinfos[5].foffset = j5;
                                                            vinfos[5].indices[0] = _ij5[0];
                                                            vinfos[5].indices[1] = _ij5[1];
                                                            vinfos[5].maxsolutions = _nj5;
                                                            std::vector<int> vfree(0);
                                                            solutions.AddSolution(vinfos, vfree);
                                                          }
                                                        }
                                                      }
                                                    }
                                                  } while (0);
                                                  if (bgotonextstatement) {
                                                    bool bgotonextstatement = true;
                                                    do {
                                                      evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs((-3.14159265358979) + j3))), 6.28318530717959)));
                                                      evalcond[1] = new_r12;
                                                      if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000) {
                                                        bgotonextstatement = false;
                                                        {
                                                          IkReal j5eval[3];
                                                          sj3 = 0;
                                                          cj3 = -1.0;
                                                          j3 = 3.14159265358979;
                                                          j5eval[0] = new_r02;
                                                          j5eval[1] = IKsign(new_r02);
                                                          j5eval[2] = ((IKabs(new_r20)) + (IKabs(new_r21)));
                                                          if (IKabs(j5eval[0]) < 0.0000010000000000 || IKabs(j5eval[1]) < 0.0000010000000000 ||
                                                              IKabs(j5eval[2]) < 0.0000010000000000) {
                                                            {
                                                              IkReal j5eval[1];
                                                              sj3 = 0;
                                                              cj3 = -1.0;
                                                              j3 = 3.14159265358979;
                                                              j5eval[0] = new_r02;
                                                              if (IKabs(j5eval[0]) < 0.0000010000000000) {
                                                                {
                                                                  IkReal j5eval[2];
                                                                  sj3 = 0;
                                                                  cj3 = -1.0;
                                                                  j3 = 3.14159265358979;
                                                                  j5eval[0] = new_r02;
                                                                  j5eval[1] = cj4;
                                                                  if (IKabs(j5eval[0]) < 0.0000010000000000 || IKabs(j5eval[1]) < 0.0000010000000000) {
                                                                    {
                                                                      IkReal evalcond[4];
                                                                      bool bgotonextstatement = true;
                                                                      do {
                                                                        evalcond[0] = ((-3.14159265358979) +
                                                                                       (IKfmod(((3.14159265358979) + (IKabs((-1.5707963267949) + j4))), 6.28318530717959)));
                                                                        evalcond[1] = new_r22;
                                                                        evalcond[2] = new_r01;
                                                                        evalcond[3] = new_r00;
                                                                        if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000 &&
                                                                            IKabs(evalcond[2]) < 0.0000050000000000 && IKabs(evalcond[3]) < 0.0000050000000000) {
                                                                          bgotonextstatement = false;
                                                                          {
                                                                            IkReal j5array[1], cj5array[1], sj5array[1];
                                                                            bool j5valid[1] = {false};
                                                                            _nj5 = 1;
                                                                            if (IKabs(new_r21) < IKFAST_ATAN2_MAGTHRESH && IKabs((-1.0) * new_r20) < IKFAST_ATAN2_MAGTHRESH &&
                                                                                IKabs(IKsqr(new_r21) + IKsqr((-1.0) * new_r20) - 1) <= IKFAST_SINCOS_THRESH) {
                                                                              continue;
                                                                            }
                                                                            j5array[0] = IKatan2(new_r21, ((-1.0) * new_r20));
                                                                            sj5array[0] = IKsin(j5array[0]);
                                                                            cj5array[0] = IKcos(j5array[0]);
                                                                            if (j5array[0] > IKPI) {
                                                                              j5array[0] -= IK2PI;
                                                                            } else if (j5array[0] < -IKPI) {
                                                                              j5array[0] += IK2PI;
                                                                            }
                                                                            j5valid[0] = true;
                                                                            for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                                              if (!j5valid[ij5]) {
                                                                                continue;
                                                                              }
                                                                              _ij5[0] = ij5;
                                                                              _ij5[1] = -1;
                                                                              for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                                                if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                                    IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                                  j5valid[iij5] = false;
                                                                                  _ij5[1] = iij5;
                                                                                  break;
                                                                                }
                                                                              }
                                                                              j5 = j5array[ij5];
                                                                              cj5 = cj5array[ij5];
                                                                              sj5 = sj5array[ij5];
                                                                              {
                                                                                IkReal evalcond[4];
                                                                                IkReal x297 = IKcos(j5);
                                                                                IkReal x298 = ((1.0) * (IKsin(j5)));
                                                                                evalcond[0] = (x297 + new_r20);
                                                                                evalcond[1] = (((-1.0) * x298) + new_r21);
                                                                                evalcond[2] = (((-1.0) * x298) + ((-1.0) * new_r10));
                                                                                evalcond[3] = (((-1.0) * x297) + ((-1.0) * new_r11));
                                                                                if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                                    IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH) {
                                                                                  continue;
                                                                                }
                                                                              }

                                                                              {
                                                                                std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                                                vinfos[0].jointtype = 1;
                                                                                vinfos[0].foffset = j0;
                                                                                vinfos[0].indices[0] = _ij0[0];
                                                                                vinfos[0].indices[1] = _ij0[1];
                                                                                vinfos[0].maxsolutions = _nj0;
                                                                                vinfos[1].jointtype = 1;
                                                                                vinfos[1].foffset = j1;
                                                                                vinfos[1].indices[0] = _ij1[0];
                                                                                vinfos[1].indices[1] = _ij1[1];
                                                                                vinfos[1].maxsolutions = _nj1;
                                                                                vinfos[2].jointtype = 1;
                                                                                vinfos[2].foffset = j2;
                                                                                vinfos[2].indices[0] = _ij2[0];
                                                                                vinfos[2].indices[1] = _ij2[1];
                                                                                vinfos[2].maxsolutions = _nj2;
                                                                                vinfos[3].jointtype = 1;
                                                                                vinfos[3].foffset = j3;
                                                                                vinfos[3].indices[0] = _ij3[0];
                                                                                vinfos[3].indices[1] = _ij3[1];
                                                                                vinfos[3].maxsolutions = _nj3;
                                                                                vinfos[4].jointtype = 1;
                                                                                vinfos[4].foffset = j4;
                                                                                vinfos[4].indices[0] = _ij4[0];
                                                                                vinfos[4].indices[1] = _ij4[1];
                                                                                vinfos[4].maxsolutions = _nj4;
                                                                                vinfos[5].jointtype = 1;
                                                                                vinfos[5].foffset = j5;
                                                                                vinfos[5].indices[0] = _ij5[0];
                                                                                vinfos[5].indices[1] = _ij5[1];
                                                                                vinfos[5].maxsolutions = _nj5;
                                                                                std::vector<int> vfree(0);
                                                                                solutions.AddSolution(vinfos, vfree);
                                                                              }
                                                                            }
                                                                          }
                                                                        }
                                                                      } while (0);
                                                                      if (bgotonextstatement) {
                                                                        bool bgotonextstatement = true;
                                                                        do {
                                                                          evalcond[0] = ((-3.14159265358979) +
                                                                                         (IKfmod(((3.14159265358979) + (IKabs((1.5707963267949) + j4))), 6.28318530717959)));
                                                                          evalcond[1] = new_r22;
                                                                          evalcond[2] = new_r01;
                                                                          evalcond[3] = new_r00;
                                                                          if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000 &&
                                                                              IKabs(evalcond[2]) < 0.0000050000000000 && IKabs(evalcond[3]) < 0.0000050000000000) {
                                                                            bgotonextstatement = false;
                                                                            {
                                                                              IkReal j5array[1], cj5array[1], sj5array[1];
                                                                              bool j5valid[1] = {false};
                                                                              _nj5 = 1;
                                                                              if (IKabs((-1.0) * new_r21) < IKFAST_ATAN2_MAGTHRESH && IKabs(new_r20) < IKFAST_ATAN2_MAGTHRESH &&
                                                                                  IKabs(IKsqr((-1.0) * new_r21) + IKsqr(new_r20) - 1) <= IKFAST_SINCOS_THRESH) {
                                                                                continue;
                                                                              }
                                                                              j5array[0] = IKatan2(((-1.0) * new_r21), new_r20);
                                                                              sj5array[0] = IKsin(j5array[0]);
                                                                              cj5array[0] = IKcos(j5array[0]);
                                                                              if (j5array[0] > IKPI) {
                                                                                j5array[0] -= IK2PI;
                                                                              } else if (j5array[0] < -IKPI) {
                                                                                j5array[0] += IK2PI;
                                                                              }
                                                                              j5valid[0] = true;
                                                                              for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                                                if (!j5valid[ij5]) {
                                                                                  continue;
                                                                                }
                                                                                _ij5[0] = ij5;
                                                                                _ij5[1] = -1;
                                                                                for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                                                  if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                                      IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                                    j5valid[iij5] = false;
                                                                                    _ij5[1] = iij5;
                                                                                    break;
                                                                                  }
                                                                                }
                                                                                j5 = j5array[ij5];
                                                                                cj5 = cj5array[ij5];
                                                                                sj5 = sj5array[ij5];
                                                                                {
                                                                                  IkReal evalcond[4];
                                                                                  IkReal x299 = IKsin(j5);
                                                                                  IkReal x300 = ((1.0) * (IKcos(j5)));
                                                                                  evalcond[0] = (x299 + new_r21);
                                                                                  evalcond[1] = (new_r20 + ((-1.0) * x300));
                                                                                  evalcond[2] = (((-1.0) * x299) + ((-1.0) * new_r10));
                                                                                  evalcond[3] = (((-1.0) * new_r11) + ((-1.0) * x300));
                                                                                  if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                                      IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH) {
                                                                                    continue;
                                                                                  }
                                                                                }

                                                                                {
                                                                                  std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                                                  vinfos[0].jointtype = 1;
                                                                                  vinfos[0].foffset = j0;
                                                                                  vinfos[0].indices[0] = _ij0[0];
                                                                                  vinfos[0].indices[1] = _ij0[1];
                                                                                  vinfos[0].maxsolutions = _nj0;
                                                                                  vinfos[1].jointtype = 1;
                                                                                  vinfos[1].foffset = j1;
                                                                                  vinfos[1].indices[0] = _ij1[0];
                                                                                  vinfos[1].indices[1] = _ij1[1];
                                                                                  vinfos[1].maxsolutions = _nj1;
                                                                                  vinfos[2].jointtype = 1;
                                                                                  vinfos[2].foffset = j2;
                                                                                  vinfos[2].indices[0] = _ij2[0];
                                                                                  vinfos[2].indices[1] = _ij2[1];
                                                                                  vinfos[2].maxsolutions = _nj2;
                                                                                  vinfos[3].jointtype = 1;
                                                                                  vinfos[3].foffset = j3;
                                                                                  vinfos[3].indices[0] = _ij3[0];
                                                                                  vinfos[3].indices[1] = _ij3[1];
                                                                                  vinfos[3].maxsolutions = _nj3;
                                                                                  vinfos[4].jointtype = 1;
                                                                                  vinfos[4].foffset = j4;
                                                                                  vinfos[4].indices[0] = _ij4[0];
                                                                                  vinfos[4].indices[1] = _ij4[1];
                                                                                  vinfos[4].maxsolutions = _nj4;
                                                                                  vinfos[5].jointtype = 1;
                                                                                  vinfos[5].foffset = j5;
                                                                                  vinfos[5].indices[0] = _ij5[0];
                                                                                  vinfos[5].indices[1] = _ij5[1];
                                                                                  vinfos[5].maxsolutions = _nj5;
                                                                                  std::vector<int> vfree(0);
                                                                                  solutions.AddSolution(vinfos, vfree);
                                                                                }
                                                                              }
                                                                            }
                                                                          }
                                                                        } while (0);
                                                                        if (bgotonextstatement) {
                                                                          bool bgotonextstatement = true;
                                                                          do {
                                                                            evalcond[0] = IKabs(new_r02);
                                                                            evalcond[1] = new_r20;
                                                                            evalcond[2] = new_r21;
                                                                            if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000 &&
                                                                                IKabs(evalcond[2]) < 0.0000050000000000) {
                                                                              bgotonextstatement = false;
                                                                              {
                                                                                IkReal j5array[1], cj5array[1], sj5array[1];
                                                                                bool j5valid[1] = {false};
                                                                                _nj5 = 1;
                                                                                if (IKabs(cj4 * new_r01) < IKFAST_ATAN2_MAGTHRESH &&
                                                                                    IKabs((-1.0) * new_r11) < IKFAST_ATAN2_MAGTHRESH &&
                                                                                    IKabs(IKsqr(cj4 * new_r01) + IKsqr((-1.0) * new_r11) - 1) <= IKFAST_SINCOS_THRESH) {
                                                                                  continue;
                                                                                }
                                                                                j5array[0] = IKatan2((cj4 * new_r01), ((-1.0) * new_r11));
                                                                                sj5array[0] = IKsin(j5array[0]);
                                                                                cj5array[0] = IKcos(j5array[0]);
                                                                                if (j5array[0] > IKPI) {
                                                                                  j5array[0] -= IK2PI;
                                                                                } else if (j5array[0] < -IKPI) {
                                                                                  j5array[0] += IK2PI;
                                                                                }
                                                                                j5valid[0] = true;
                                                                                for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                                                  if (!j5valid[ij5]) {
                                                                                    continue;
                                                                                  }
                                                                                  _ij5[0] = ij5;
                                                                                  _ij5[1] = -1;
                                                                                  for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                                                    if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                                        IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                                      j5valid[iij5] = false;
                                                                                      _ij5[1] = iij5;
                                                                                      break;
                                                                                    }
                                                                                  }
                                                                                  j5 = j5array[ij5];
                                                                                  cj5 = cj5array[ij5];
                                                                                  sj5 = sj5array[ij5];
                                                                                  {
                                                                                    IkReal evalcond[6];
                                                                                    IkReal x301 = IKsin(j5);
                                                                                    IkReal x302 = IKcos(j5);
                                                                                    IkReal x303 = ((1.0) * new_r00);
                                                                                    IkReal x304 = ((1.0) * new_r01);
                                                                                    IkReal x305 = ((1.0) * x302);
                                                                                    evalcond[0] = (((-1.0) * cj4 * x304) + x301);
                                                                                    evalcond[1] = (((-1.0) * x301) + ((-1.0) * new_r10));
                                                                                    evalcond[2] = (((-1.0) * new_r11) + ((-1.0) * x305));
                                                                                    evalcond[3] = ((cj4 * x301) + ((-1.0) * x304));
                                                                                    evalcond[4] = (((-1.0) * cj4 * x305) + ((-1.0) * x303));
                                                                                    evalcond[5] = (((-1.0) * cj4 * x303) + ((-1.0) * x305));
                                                                                    if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH ||
                                                                                        IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                                        IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH ||
                                                                                        IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                                                        IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH ||
                                                                                        IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH) {
                                                                                      continue;
                                                                                    }
                                                                                  }

                                                                                  {
                                                                                    std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                                                    vinfos[0].jointtype = 1;
                                                                                    vinfos[0].foffset = j0;
                                                                                    vinfos[0].indices[0] = _ij0[0];
                                                                                    vinfos[0].indices[1] = _ij0[1];
                                                                                    vinfos[0].maxsolutions = _nj0;
                                                                                    vinfos[1].jointtype = 1;
                                                                                    vinfos[1].foffset = j1;
                                                                                    vinfos[1].indices[0] = _ij1[0];
                                                                                    vinfos[1].indices[1] = _ij1[1];
                                                                                    vinfos[1].maxsolutions = _nj1;
                                                                                    vinfos[2].jointtype = 1;
                                                                                    vinfos[2].foffset = j2;
                                                                                    vinfos[2].indices[0] = _ij2[0];
                                                                                    vinfos[2].indices[1] = _ij2[1];
                                                                                    vinfos[2].maxsolutions = _nj2;
                                                                                    vinfos[3].jointtype = 1;
                                                                                    vinfos[3].foffset = j3;
                                                                                    vinfos[3].indices[0] = _ij3[0];
                                                                                    vinfos[3].indices[1] = _ij3[1];
                                                                                    vinfos[3].maxsolutions = _nj3;
                                                                                    vinfos[4].jointtype = 1;
                                                                                    vinfos[4].foffset = j4;
                                                                                    vinfos[4].indices[0] = _ij4[0];
                                                                                    vinfos[4].indices[1] = _ij4[1];
                                                                                    vinfos[4].maxsolutions = _nj4;
                                                                                    vinfos[5].jointtype = 1;
                                                                                    vinfos[5].foffset = j5;
                                                                                    vinfos[5].indices[0] = _ij5[0];
                                                                                    vinfos[5].indices[1] = _ij5[1];
                                                                                    vinfos[5].maxsolutions = _nj5;
                                                                                    std::vector<int> vfree(0);
                                                                                    solutions.AddSolution(vinfos, vfree);
                                                                                  }
                                                                                }
                                                                              }
                                                                            }
                                                                          } while (0);
                                                                          if (bgotonextstatement) {
                                                                            bool bgotonextstatement = true;
                                                                            do {
                                                                              evalcond[0] = ((IKabs(new_r20)) + (IKabs(new_r21)));
                                                                              if (IKabs(evalcond[0]) < 0.0000050000000000) {
                                                                                bgotonextstatement = false;
                                                                                {
                                                                                  IkReal j5array[1], cj5array[1], sj5array[1];
                                                                                  bool j5valid[1] = {false};
                                                                                  _nj5 = 1;
                                                                                  if (IKabs((-1.0) * new_r10) < IKFAST_ATAN2_MAGTHRESH &&
                                                                                      IKabs((-1.0) * new_r11) < IKFAST_ATAN2_MAGTHRESH &&
                                                                                      IKabs(IKsqr((-1.0) * new_r10) + IKsqr((-1.0) * new_r11) - 1) <= IKFAST_SINCOS_THRESH) {
                                                                                    continue;
                                                                                  }
                                                                                  j5array[0] = IKatan2(((-1.0) * new_r10), ((-1.0) * new_r11));
                                                                                  sj5array[0] = IKsin(j5array[0]);
                                                                                  cj5array[0] = IKcos(j5array[0]);
                                                                                  if (j5array[0] > IKPI) {
                                                                                    j5array[0] -= IK2PI;
                                                                                  } else if (j5array[0] < -IKPI) {
                                                                                    j5array[0] += IK2PI;
                                                                                  }
                                                                                  j5valid[0] = true;
                                                                                  for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                                                    if (!j5valid[ij5]) {
                                                                                      continue;
                                                                                    }
                                                                                    _ij5[0] = ij5;
                                                                                    _ij5[1] = -1;
                                                                                    for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                                                      if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                                          IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                                        j5valid[iij5] = false;
                                                                                        _ij5[1] = iij5;
                                                                                        break;
                                                                                      }
                                                                                    }
                                                                                    j5 = j5array[ij5];
                                                                                    cj5 = cj5array[ij5];
                                                                                    sj5 = sj5array[ij5];
                                                                                    {
                                                                                      IkReal evalcond[6];
                                                                                      IkReal x306 = IKsin(j5);
                                                                                      IkReal x307 = IKcos(j5);
                                                                                      IkReal x308 = ((-1.0) * x307);
                                                                                      evalcond[0] = x306;
                                                                                      evalcond[1] = (new_r22 * x306);
                                                                                      evalcond[2] = x308;
                                                                                      evalcond[3] = (new_r22 * x308);
                                                                                      evalcond[4] = (((-1.0) * x306) + ((-1.0) * new_r10));
                                                                                      evalcond[5] = (((-1.0) * x307) + ((-1.0) * new_r11));
                                                                                      if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH ||
                                                                                          IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                                          IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH ||
                                                                                          IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                                                          IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH ||
                                                                                          IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH) {
                                                                                        continue;
                                                                                      }
                                                                                    }

                                                                                    {
                                                                                      std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                                                      vinfos[0].jointtype = 1;
                                                                                      vinfos[0].foffset = j0;
                                                                                      vinfos[0].indices[0] = _ij0[0];
                                                                                      vinfos[0].indices[1] = _ij0[1];
                                                                                      vinfos[0].maxsolutions = _nj0;
                                                                                      vinfos[1].jointtype = 1;
                                                                                      vinfos[1].foffset = j1;
                                                                                      vinfos[1].indices[0] = _ij1[0];
                                                                                      vinfos[1].indices[1] = _ij1[1];
                                                                                      vinfos[1].maxsolutions = _nj1;
                                                                                      vinfos[2].jointtype = 1;
                                                                                      vinfos[2].foffset = j2;
                                                                                      vinfos[2].indices[0] = _ij2[0];
                                                                                      vinfos[2].indices[1] = _ij2[1];
                                                                                      vinfos[2].maxsolutions = _nj2;
                                                                                      vinfos[3].jointtype = 1;
                                                                                      vinfos[3].foffset = j3;
                                                                                      vinfos[3].indices[0] = _ij3[0];
                                                                                      vinfos[3].indices[1] = _ij3[1];
                                                                                      vinfos[3].maxsolutions = _nj3;
                                                                                      vinfos[4].jointtype = 1;
                                                                                      vinfos[4].foffset = j4;
                                                                                      vinfos[4].indices[0] = _ij4[0];
                                                                                      vinfos[4].indices[1] = _ij4[1];
                                                                                      vinfos[4].maxsolutions = _nj4;
                                                                                      vinfos[5].jointtype = 1;
                                                                                      vinfos[5].foffset = j5;
                                                                                      vinfos[5].indices[0] = _ij5[0];
                                                                                      vinfos[5].indices[1] = _ij5[1];
                                                                                      vinfos[5].maxsolutions = _nj5;
                                                                                      std::vector<int> vfree(0);
                                                                                      solutions.AddSolution(vinfos, vfree);
                                                                                    }
                                                                                  }
                                                                                }
                                                                              }
                                                                            } while (0);
                                                                            if (bgotonextstatement) {
                                                                              bool bgotonextstatement = true;
                                                                              do {
                                                                                if (1) {
                                                                                  bgotonextstatement = false;
                                                                                  continue; // branch miss [j5]
                                                                                }
                                                                              } while (0);
                                                                              if (bgotonextstatement) {
                                                                              }
                                                                            }
                                                                          }
                                                                        }
                                                                      }
                                                                    }

                                                                  } else {
                                                                    {
                                                                      IkReal j5array[1], cj5array[1], sj5array[1];
                                                                      bool j5valid[1] = {false};
                                                                      _nj5 = 1;
                                                                      CheckValue<IkReal> x309 = IKPowWithIntegerCheck(new_r02, -1);
                                                                      if (!x309.valid) {
                                                                        continue;
                                                                      }
                                                                      CheckValue<IkReal> x310 = IKPowWithIntegerCheck(cj4, -1);
                                                                      if (!x310.valid) {
                                                                        continue;
                                                                      }
                                                                      if (IKabs((-1.0) * new_r21 * (x309.value)) < IKFAST_ATAN2_MAGTHRESH &&
                                                                          IKabs((-1.0) * new_r00 * (x310.value)) < IKFAST_ATAN2_MAGTHRESH &&
                                                                          IKabs(IKsqr((-1.0) * new_r21 * (x309.value)) + IKsqr((-1.0) * new_r00 * (x310.value)) - 1) <=
                                                                            IKFAST_SINCOS_THRESH) {
                                                                        continue;
                                                                      }
                                                                      j5array[0] = IKatan2(((-1.0) * new_r21 * (x309.value)), ((-1.0) * new_r00 * (x310.value)));
                                                                      sj5array[0] = IKsin(j5array[0]);
                                                                      cj5array[0] = IKcos(j5array[0]);
                                                                      if (j5array[0] > IKPI) {
                                                                        j5array[0] -= IK2PI;
                                                                      } else if (j5array[0] < -IKPI) {
                                                                        j5array[0] += IK2PI;
                                                                      }
                                                                      j5valid[0] = true;
                                                                      for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                                        if (!j5valid[ij5]) {
                                                                          continue;
                                                                        }
                                                                        _ij5[0] = ij5;
                                                                        _ij5[1] = -1;
                                                                        for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                                          if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                              IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                            j5valid[iij5] = false;
                                                                            _ij5[1] = iij5;
                                                                            break;
                                                                          }
                                                                        }
                                                                        j5 = j5array[ij5];
                                                                        cj5 = cj5array[ij5];
                                                                        sj5 = sj5array[ij5];
                                                                        {
                                                                          IkReal evalcond[8];
                                                                          IkReal x311 = IKsin(j5);
                                                                          IkReal x312 = IKcos(j5);
                                                                          IkReal x313 = ((1.0) * new_r00);
                                                                          IkReal x314 = ((1.0) * new_r01);
                                                                          IkReal x315 = ((1.0) * sj4);
                                                                          IkReal x316 = ((1.0) * x312);
                                                                          evalcond[0] = ((new_r02 * x311) + new_r21);
                                                                          evalcond[1] = (((-1.0) * new_r02 * x316) + new_r20);
                                                                          evalcond[2] = (((-1.0) * new_r10) + ((-1.0) * x311));
                                                                          evalcond[3] = (((-1.0) * x316) + ((-1.0) * new_r11));
                                                                          evalcond[4] = ((cj4 * x311) + ((-1.0) * x314));
                                                                          evalcond[5] = (((-1.0) * cj4 * x316) + ((-1.0) * x313));
                                                                          evalcond[6] = (((-1.0) * new_r21 * x315) + ((-1.0) * cj4 * x314) + x311);
                                                                          evalcond[7] = (((-1.0) * new_r20 * x315) + ((-1.0) * cj4 * x313) + ((-1.0) * x316));
                                                                          if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                              IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                                              IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                                              IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                                                            continue;
                                                                          }
                                                                        }

                                                                        {
                                                                          std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                                          vinfos[0].jointtype = 1;
                                                                          vinfos[0].foffset = j0;
                                                                          vinfos[0].indices[0] = _ij0[0];
                                                                          vinfos[0].indices[1] = _ij0[1];
                                                                          vinfos[0].maxsolutions = _nj0;
                                                                          vinfos[1].jointtype = 1;
                                                                          vinfos[1].foffset = j1;
                                                                          vinfos[1].indices[0] = _ij1[0];
                                                                          vinfos[1].indices[1] = _ij1[1];
                                                                          vinfos[1].maxsolutions = _nj1;
                                                                          vinfos[2].jointtype = 1;
                                                                          vinfos[2].foffset = j2;
                                                                          vinfos[2].indices[0] = _ij2[0];
                                                                          vinfos[2].indices[1] = _ij2[1];
                                                                          vinfos[2].maxsolutions = _nj2;
                                                                          vinfos[3].jointtype = 1;
                                                                          vinfos[3].foffset = j3;
                                                                          vinfos[3].indices[0] = _ij3[0];
                                                                          vinfos[3].indices[1] = _ij3[1];
                                                                          vinfos[3].maxsolutions = _nj3;
                                                                          vinfos[4].jointtype = 1;
                                                                          vinfos[4].foffset = j4;
                                                                          vinfos[4].indices[0] = _ij4[0];
                                                                          vinfos[4].indices[1] = _ij4[1];
                                                                          vinfos[4].maxsolutions = _nj4;
                                                                          vinfos[5].jointtype = 1;
                                                                          vinfos[5].foffset = j5;
                                                                          vinfos[5].indices[0] = _ij5[0];
                                                                          vinfos[5].indices[1] = _ij5[1];
                                                                          vinfos[5].maxsolutions = _nj5;
                                                                          std::vector<int> vfree(0);
                                                                          solutions.AddSolution(vinfos, vfree);
                                                                        }
                                                                      }
                                                                    }
                                                                  }
                                                                }

                                                              } else {
                                                                {
                                                                  IkReal j5array[1], cj5array[1], sj5array[1];
                                                                  bool j5valid[1] = {false};
                                                                  _nj5 = 1;
                                                                  CheckValue<IkReal> x317 = IKPowWithIntegerCheck(new_r02, -1);
                                                                  if (!x317.valid) {
                                                                    continue;
                                                                  }
                                                                  if (IKabs((-1.0) * new_r10) < IKFAST_ATAN2_MAGTHRESH && IKabs(new_r20 * (x317.value)) < IKFAST_ATAN2_MAGTHRESH &&
                                                                      IKabs(IKsqr((-1.0) * new_r10) + IKsqr(new_r20 * (x317.value)) - 1) <= IKFAST_SINCOS_THRESH) {
                                                                    continue;
                                                                  }
                                                                  j5array[0] = IKatan2(((-1.0) * new_r10), (new_r20 * (x317.value)));
                                                                  sj5array[0] = IKsin(j5array[0]);
                                                                  cj5array[0] = IKcos(j5array[0]);
                                                                  if (j5array[0] > IKPI) {
                                                                    j5array[0] -= IK2PI;
                                                                  } else if (j5array[0] < -IKPI) {
                                                                    j5array[0] += IK2PI;
                                                                  }
                                                                  j5valid[0] = true;
                                                                  for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                                    if (!j5valid[ij5]) {
                                                                      continue;
                                                                    }
                                                                    _ij5[0] = ij5;
                                                                    _ij5[1] = -1;
                                                                    for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                                      if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                          IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                        j5valid[iij5] = false;
                                                                        _ij5[1] = iij5;
                                                                        break;
                                                                      }
                                                                    }
                                                                    j5 = j5array[ij5];
                                                                    cj5 = cj5array[ij5];
                                                                    sj5 = sj5array[ij5];
                                                                    {
                                                                      IkReal evalcond[8];
                                                                      IkReal x318 = IKsin(j5);
                                                                      IkReal x319 = IKcos(j5);
                                                                      IkReal x320 = ((1.0) * new_r00);
                                                                      IkReal x321 = ((1.0) * new_r01);
                                                                      IkReal x322 = ((1.0) * sj4);
                                                                      IkReal x323 = ((1.0) * x319);
                                                                      evalcond[0] = ((new_r02 * x318) + new_r21);
                                                                      evalcond[1] = (((-1.0) * new_r02 * x323) + new_r20);
                                                                      evalcond[2] = (((-1.0) * new_r10) + ((-1.0) * x318));
                                                                      evalcond[3] = (((-1.0) * x323) + ((-1.0) * new_r11));
                                                                      evalcond[4] = ((cj4 * x318) + ((-1.0) * x321));
                                                                      evalcond[5] = (((-1.0) * x320) + ((-1.0) * cj4 * x323));
                                                                      evalcond[6] = (x318 + ((-1.0) * new_r21 * x322) + ((-1.0) * cj4 * x321));
                                                                      evalcond[7] = (((-1.0) * x323) + ((-1.0) * cj4 * x320) + ((-1.0) * new_r20 * x322));
                                                                      if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                          IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                                          IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                                          IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                                                        continue;
                                                                      }
                                                                    }

                                                                    {
                                                                      std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                                      vinfos[0].jointtype = 1;
                                                                      vinfos[0].foffset = j0;
                                                                      vinfos[0].indices[0] = _ij0[0];
                                                                      vinfos[0].indices[1] = _ij0[1];
                                                                      vinfos[0].maxsolutions = _nj0;
                                                                      vinfos[1].jointtype = 1;
                                                                      vinfos[1].foffset = j1;
                                                                      vinfos[1].indices[0] = _ij1[0];
                                                                      vinfos[1].indices[1] = _ij1[1];
                                                                      vinfos[1].maxsolutions = _nj1;
                                                                      vinfos[2].jointtype = 1;
                                                                      vinfos[2].foffset = j2;
                                                                      vinfos[2].indices[0] = _ij2[0];
                                                                      vinfos[2].indices[1] = _ij2[1];
                                                                      vinfos[2].maxsolutions = _nj2;
                                                                      vinfos[3].jointtype = 1;
                                                                      vinfos[3].foffset = j3;
                                                                      vinfos[3].indices[0] = _ij3[0];
                                                                      vinfos[3].indices[1] = _ij3[1];
                                                                      vinfos[3].maxsolutions = _nj3;
                                                                      vinfos[4].jointtype = 1;
                                                                      vinfos[4].foffset = j4;
                                                                      vinfos[4].indices[0] = _ij4[0];
                                                                      vinfos[4].indices[1] = _ij4[1];
                                                                      vinfos[4].maxsolutions = _nj4;
                                                                      vinfos[5].jointtype = 1;
                                                                      vinfos[5].foffset = j5;
                                                                      vinfos[5].indices[0] = _ij5[0];
                                                                      vinfos[5].indices[1] = _ij5[1];
                                                                      vinfos[5].maxsolutions = _nj5;
                                                                      std::vector<int> vfree(0);
                                                                      solutions.AddSolution(vinfos, vfree);
                                                                    }
                                                                  }
                                                                }
                                                              }
                                                            }

                                                          } else {
                                                            {
                                                              IkReal j5array[1], cj5array[1], sj5array[1];
                                                              bool j5valid[1] = {false};
                                                              _nj5 = 1;
                                                              CheckValue<IkReal> x324 = IKatan2WithCheck(IkReal((-1.0) * new_r21), IkReal(new_r20), IKFAST_ATAN2_MAGTHRESH);
                                                              if (!x324.valid)
                                                                continue;
                                                              CheckValue<IkReal> x325 = IKPowWithIntegerCheck(IKsign(new_r02), -1);
                                                              if (!x325.valid)
                                                                continue;
                                                              j5array[0] = ((-1.5707963267949) + (x324.value) + ((1.5707963267949) * (x325.value)));
                                                              sj5array[0] = IKsin(j5array[0]);
                                                              cj5array[0] = IKcos(j5array[0]);
                                                              if (j5array[0] > IKPI) {
                                                                j5array[0] -= IK2PI;
                                                              } else if (j5array[0] < -IKPI) {
                                                                j5array[0] += IK2PI;
                                                              }
                                                              j5valid[0] = true;
                                                              for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                                if (!j5valid[ij5]) {
                                                                  continue;
                                                                }
                                                                _ij5[0] = ij5;
                                                                _ij5[1] = -1;
                                                                for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                                  if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                      IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                    j5valid[iij5] = false;
                                                                    _ij5[1] = iij5;
                                                                    break;
                                                                  }
                                                                }
                                                                j5 = j5array[ij5];
                                                                cj5 = cj5array[ij5];
                                                                sj5 = sj5array[ij5];
                                                                {
                                                                  IkReal evalcond[8];
                                                                  IkReal x326 = IKsin(j5);
                                                                  IkReal x327 = IKcos(j5);
                                                                  IkReal x328 = ((1.0) * new_r00);
                                                                  IkReal x329 = ((1.0) * new_r01);
                                                                  IkReal x330 = ((1.0) * sj4);
                                                                  IkReal x331 = ((1.0) * x327);
                                                                  evalcond[0] = (new_r21 + (new_r02 * x326));
                                                                  evalcond[1] = (new_r20 + ((-1.0) * new_r02 * x331));
                                                                  evalcond[2] = (((-1.0) * x326) + ((-1.0) * new_r10));
                                                                  evalcond[3] = (((-1.0) * new_r11) + ((-1.0) * x331));
                                                                  evalcond[4] = (((-1.0) * x329) + (cj4 * x326));
                                                                  evalcond[5] = (((-1.0) * cj4 * x331) + ((-1.0) * x328));
                                                                  evalcond[6] = (((-1.0) * new_r21 * x330) + x326 + ((-1.0) * cj4 * x329));
                                                                  evalcond[7] = (((-1.0) * new_r20 * x330) + ((-1.0) * x331) + ((-1.0) * cj4 * x328));
                                                                  if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                      IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                                      IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                                      IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                                                    continue;
                                                                  }
                                                                }

                                                                {
                                                                  std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                                  vinfos[0].jointtype = 1;
                                                                  vinfos[0].foffset = j0;
                                                                  vinfos[0].indices[0] = _ij0[0];
                                                                  vinfos[0].indices[1] = _ij0[1];
                                                                  vinfos[0].maxsolutions = _nj0;
                                                                  vinfos[1].jointtype = 1;
                                                                  vinfos[1].foffset = j1;
                                                                  vinfos[1].indices[0] = _ij1[0];
                                                                  vinfos[1].indices[1] = _ij1[1];
                                                                  vinfos[1].maxsolutions = _nj1;
                                                                  vinfos[2].jointtype = 1;
                                                                  vinfos[2].foffset = j2;
                                                                  vinfos[2].indices[0] = _ij2[0];
                                                                  vinfos[2].indices[1] = _ij2[1];
                                                                  vinfos[2].maxsolutions = _nj2;
                                                                  vinfos[3].jointtype = 1;
                                                                  vinfos[3].foffset = j3;
                                                                  vinfos[3].indices[0] = _ij3[0];
                                                                  vinfos[3].indices[1] = _ij3[1];
                                                                  vinfos[3].maxsolutions = _nj3;
                                                                  vinfos[4].jointtype = 1;
                                                                  vinfos[4].foffset = j4;
                                                                  vinfos[4].indices[0] = _ij4[0];
                                                                  vinfos[4].indices[1] = _ij4[1];
                                                                  vinfos[4].maxsolutions = _nj4;
                                                                  vinfos[5].jointtype = 1;
                                                                  vinfos[5].foffset = j5;
                                                                  vinfos[5].indices[0] = _ij5[0];
                                                                  vinfos[5].indices[1] = _ij5[1];
                                                                  vinfos[5].maxsolutions = _nj5;
                                                                  std::vector<int> vfree(0);
                                                                  solutions.AddSolution(vinfos, vfree);
                                                                }
                                                              }
                                                            }
                                                          }
                                                        }
                                                      }
                                                    } while (0);
                                                    if (bgotonextstatement) {
                                                      bool bgotonextstatement = true;
                                                      do {
                                                        evalcond[0] = ((IKabs(new_r20)) + (IKabs(new_r21)));
                                                        if (IKabs(evalcond[0]) < 0.0000050000000000) {
                                                          bgotonextstatement = false;
                                                          {
                                                            IkReal j5eval[1];
                                                            new_r21 = 0;
                                                            new_r20 = 0;
                                                            new_r02 = 0;
                                                            new_r12 = 0;
                                                            j5eval[0] = 1.0;
                                                            if (IKabs(j5eval[0]) < 0.0000000100000000) {
                                                              continue; // no
                                                                        // branches
                                                                        // [j5]

                                                            } else {
                                                              IkReal op[2 + 1], zeror[2];
                                                              int numroots;
                                                              op[0] = 1.0;
                                                              op[1] = 0;
                                                              op[2] = -1.0;
                                                              polyroots2(op, zeror, numroots);
                                                              IkReal j5array[2], cj5array[2], sj5array[2], tempj5array[1];
                                                              int numsolutions = 0;
                                                              for (int ij5 = 0; ij5 < numroots; ++ij5) {
                                                                IkReal htj5 = zeror[ij5];
                                                                tempj5array[0] = ((2.0) * (atan(htj5)));
                                                                for (int kj5 = 0; kj5 < 1; ++kj5) {
                                                                  j5array[numsolutions] = tempj5array[kj5];
                                                                  if (j5array[numsolutions] > IKPI) {
                                                                    j5array[numsolutions] -= IK2PI;
                                                                  } else if (j5array[numsolutions] < -IKPI) {
                                                                    j5array[numsolutions] += IK2PI;
                                                                  }
                                                                  sj5array[numsolutions] = IKsin(j5array[numsolutions]);
                                                                  cj5array[numsolutions] = IKcos(j5array[numsolutions]);
                                                                  numsolutions++;
                                                                }
                                                              }
                                                              bool j5valid[2] = {true, true};
                                                              _nj5 = 2;
                                                              for (int ij5 = 0; ij5 < numsolutions; ++ij5) {
                                                                if (!j5valid[ij5]) {
                                                                  continue;
                                                                }
                                                                j5 = j5array[ij5];
                                                                cj5 = cj5array[ij5];
                                                                sj5 = sj5array[ij5];
                                                                htj5 = IKtan(j5 / 2);

                                                                _ij5[0] = ij5;
                                                                _ij5[1] = -1;
                                                                for (int iij5 = ij5 + 1; iij5 < numsolutions; ++iij5) {
                                                                  if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                      IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                    j5valid[iij5] = false;
                                                                    _ij5[1] = iij5;
                                                                    break;
                                                                  }
                                                                }
                                                                {
                                                                  std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                                  vinfos[0].jointtype = 1;
                                                                  vinfos[0].foffset = j0;
                                                                  vinfos[0].indices[0] = _ij0[0];
                                                                  vinfos[0].indices[1] = _ij0[1];
                                                                  vinfos[0].maxsolutions = _nj0;
                                                                  vinfos[1].jointtype = 1;
                                                                  vinfos[1].foffset = j1;
                                                                  vinfos[1].indices[0] = _ij1[0];
                                                                  vinfos[1].indices[1] = _ij1[1];
                                                                  vinfos[1].maxsolutions = _nj1;
                                                                  vinfos[2].jointtype = 1;
                                                                  vinfos[2].foffset = j2;
                                                                  vinfos[2].indices[0] = _ij2[0];
                                                                  vinfos[2].indices[1] = _ij2[1];
                                                                  vinfos[2].maxsolutions = _nj2;
                                                                  vinfos[3].jointtype = 1;
                                                                  vinfos[3].foffset = j3;
                                                                  vinfos[3].indices[0] = _ij3[0];
                                                                  vinfos[3].indices[1] = _ij3[1];
                                                                  vinfos[3].maxsolutions = _nj3;
                                                                  vinfos[4].jointtype = 1;
                                                                  vinfos[4].foffset = j4;
                                                                  vinfos[4].indices[0] = _ij4[0];
                                                                  vinfos[4].indices[1] = _ij4[1];
                                                                  vinfos[4].maxsolutions = _nj4;
                                                                  vinfos[5].jointtype = 1;
                                                                  vinfos[5].foffset = j5;
                                                                  vinfos[5].indices[0] = _ij5[0];
                                                                  vinfos[5].indices[1] = _ij5[1];
                                                                  vinfos[5].maxsolutions = _nj5;
                                                                  std::vector<int> vfree(0);
                                                                  solutions.AddSolution(vinfos, vfree);
                                                                }
                                                              }
                                                            }
                                                          }
                                                        }
                                                      } while (0);
                                                      if (bgotonextstatement) {
                                                        bool bgotonextstatement = true;
                                                        do {
                                                          if (1) {
                                                            bgotonextstatement = false;
                                                            continue; // branch
                                                                      // miss
                                                                      // [j5]
                                                          }
                                                        } while (0);
                                                        if (bgotonextstatement) {
                                                        }
                                                      }
                                                    }
                                                  }
                                                }
                                              }
                                            }
                                          }
                                        }
                                      }
                                    }

                                  } else {
                                    {
                                      IkReal j5array[1], cj5array[1], sj5array[1];
                                      bool j5valid[1] = {false};
                                      _nj5 = 1;
                                      CheckValue<IkReal> x333 = IKPowWithIntegerCheck(sj4, -1);
                                      if (!x333.valid)
                                        continue;
                                      IkReal x332 = x333.value;
                                      CheckValue<IkReal> x334 = IKPowWithIntegerCheck(cj3, -1);
                                      if (!x334.valid)
                                        continue;
                                      CheckValue<IkReal> x335 = IKPowWithIntegerCheck(cj4, -1);
                                      if (!x335.valid)
                                        continue;
                                      if (IKabs(x332 * (x334.value) * (x335.value) * ((new_r20 * sj3) + ((-1.0) * new_r01 * sj4))) < IKFAST_ATAN2_MAGTHRESH &&
                                          IKabs((-1.0) * new_r20 * x332) < IKFAST_ATAN2_MAGTHRESH &&
                                          IKabs(IKsqr(x332 * (x334.value) * (x335.value) * ((new_r20 * sj3) + ((-1.0) * new_r01 * sj4))) + IKsqr((-1.0) * new_r20 * x332) - 1) <=
                                            IKFAST_SINCOS_THRESH) {
                                        continue;
                                      }
                                      j5array[0] = IKatan2((x332 * (x334.value) * (x335.value) * ((new_r20 * sj3) + ((-1.0) * new_r01 * sj4))), ((-1.0) * new_r20 * x332));
                                      sj5array[0] = IKsin(j5array[0]);
                                      cj5array[0] = IKcos(j5array[0]);
                                      if (j5array[0] > IKPI)
                                        j5array[0] -= IK2PI;
                                      else if (j5array[0] < -IKPI)
                                        j5array[0] += IK2PI;
                                      j5valid[0] = true;
                                      for (int ij5 = 0; ij5 < 1; ++ij5) {
                                        if (!j5valid[ij5])
                                          continue;
                                        _ij5[0] = ij5;
                                        _ij5[1] = -1;
                                        for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                          if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                              IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                            j5valid[iij5] = false;
                                            _ij5[1] = iij5;
                                            break;
                                          }
                                        }
                                        j5 = j5array[ij5];
                                        cj5 = cj5array[ij5];
                                        sj5 = sj5array[ij5];
                                        {
                                          IkReal evalcond[12];
                                          IkReal x336 = IKsin(j5);
                                          IkReal x337 = IKcos(j5);
                                          IkReal x338 = (cj3 * new_r00);
                                          IkReal x339 = (cj3 * cj4);
                                          IkReal x340 = (cj4 * sj3);
                                          IkReal x341 = ((1.0) * sj3);
                                          IkReal x342 = ((1.0) * sj4);
                                          IkReal x343 = ((1.0) * x337);
                                          IkReal x344 = (sj3 * x336);
                                          IkReal x345 = ((1.0) * x336);
                                          evalcond[0] = ((sj4 * x337) + new_r20);
                                          evalcond[1] = (((-1.0) * x336 * x342) + new_r21);
                                          evalcond[2] = ((new_r11 * sj3) + (cj4 * x336) + (cj3 * new_r01));
                                          evalcond[3] = (((-1.0) * new_r00 * x341) + (cj3 * new_r10) + ((-1.0) * x345));
                                          evalcond[4] = ((cj3 * new_r11) + ((-1.0) * new_r01 * x341) + ((-1.0) * x343));
                                          evalcond[5] = ((x336 * x339) + (sj3 * x337) + new_r01);
                                          evalcond[6] = ((new_r10 * sj3) + ((-1.0) * cj4 * x343) + x338);
                                          evalcond[7] = (x344 + new_r00 + ((-1.0) * x339 * x343));
                                          evalcond[8] = (((-1.0) * cj3 * x343) + (x336 * x340) + new_r11);
                                          evalcond[9] = (((-1.0) * cj3 * x345) + ((-1.0) * x340 * x343) + new_r10);
                                          evalcond[10] = ((new_r01 * x339) + (new_r11 * x340) + x336 + ((-1.0) * new_r21 * x342));
                                          evalcond[11] = ((new_r10 * x340) + ((-1.0) * new_r20 * x342) + (cj4 * x338) + ((-1.0) * x343));
                                          if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                              IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                              IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                              IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH ||
                                              IKabs(evalcond[8]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[9]) > IKFAST_EVALCOND_THRESH ||
                                              IKabs(evalcond[10]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[11]) > IKFAST_EVALCOND_THRESH) {
                                            continue;
                                          }
                                        }

                                        {
                                          std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                          vinfos[0].jointtype = 1;
                                          vinfos[0].foffset = j0;
                                          vinfos[0].indices[0] = _ij0[0];
                                          vinfos[0].indices[1] = _ij0[1];
                                          vinfos[0].maxsolutions = _nj0;
                                          vinfos[1].jointtype = 1;
                                          vinfos[1].foffset = j1;
                                          vinfos[1].indices[0] = _ij1[0];
                                          vinfos[1].indices[1] = _ij1[1];
                                          vinfos[1].maxsolutions = _nj1;
                                          vinfos[2].jointtype = 1;
                                          vinfos[2].foffset = j2;
                                          vinfos[2].indices[0] = _ij2[0];
                                          vinfos[2].indices[1] = _ij2[1];
                                          vinfos[2].maxsolutions = _nj2;
                                          vinfos[3].jointtype = 1;
                                          vinfos[3].foffset = j3;
                                          vinfos[3].indices[0] = _ij3[0];
                                          vinfos[3].indices[1] = _ij3[1];
                                          vinfos[3].maxsolutions = _nj3;
                                          vinfos[4].jointtype = 1;
                                          vinfos[4].foffset = j4;
                                          vinfos[4].indices[0] = _ij4[0];
                                          vinfos[4].indices[1] = _ij4[1];
                                          vinfos[4].maxsolutions = _nj4;
                                          vinfos[5].jointtype = 1;
                                          vinfos[5].foffset = j5;
                                          vinfos[5].indices[0] = _ij5[0];
                                          vinfos[5].indices[1] = _ij5[1];
                                          vinfos[5].maxsolutions = _nj5;
                                          std::vector<int> vfree(0);
                                          solutions.AddSolution(vinfos, vfree);
                                        }
                                      }
                                    }
                                  }
                                }

                              } else {
                                {
                                  IkReal j5array[1], cj5array[1], sj5array[1];
                                  bool j5valid[1] = {false};
                                  _nj5 = 1;
                                  CheckValue<IkReal> x347 = IKPowWithIntegerCheck(sj4, -1);
                                  if (!x347.valid)
                                    continue;
                                  IkReal x346 = x347.value;
                                  CheckValue<IkReal> x348 = IKPowWithIntegerCheck(sj3, -1);
                                  if (!x348.valid)
                                    continue;
                                  if (IKabs(x346 * (x348.value) * (((-1.0) * cj3 * cj4 * new_r20) + ((-1.0) * new_r00 * sj4))) < IKFAST_ATAN2_MAGTHRESH &&
                                      IKabs((-1.0) * new_r20 * x346) < IKFAST_ATAN2_MAGTHRESH &&
                                      IKabs(IKsqr(x346 * (x348.value) * (((-1.0) * cj3 * cj4 * new_r20) + ((-1.0) * new_r00 * sj4))) + IKsqr((-1.0) * new_r20 * x346) - 1) <=
                                        IKFAST_SINCOS_THRESH) {
                                    continue;
                                  }
                                  j5array[0] = IKatan2((x346 * (x348.value) * (((-1.0) * cj3 * cj4 * new_r20) + ((-1.0) * new_r00 * sj4))), ((-1.0) * new_r20 * x346));
                                  sj5array[0] = IKsin(j5array[0]);
                                  cj5array[0] = IKcos(j5array[0]);
                                  if (j5array[0] > IKPI)
                                    j5array[0] -= IK2PI;
                                  else if (j5array[0] < -IKPI)
                                    j5array[0] += IK2PI;
                                  j5valid[0] = true;
                                  for (int ij5 = 0; ij5 < 1; ++ij5) {
                                    if (!j5valid[ij5])
                                      continue;
                                    _ij5[0] = ij5;
                                    _ij5[1] = -1;
                                    for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                      if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                          IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                        j5valid[iij5] = false;
                                        _ij5[1] = iij5;
                                        break;
                                      }
                                    }
                                    j5 = j5array[ij5];
                                    cj5 = cj5array[ij5];
                                    sj5 = sj5array[ij5];
                                    {
                                      IkReal evalcond[12];
                                      IkReal x349 = IKsin(j5);
                                      IkReal x350 = IKcos(j5);
                                      IkReal x351 = (cj3 * new_r00);
                                      IkReal x352 = (cj3 * cj4);
                                      IkReal x353 = (cj4 * sj3);
                                      IkReal x354 = ((1.0) * sj3);
                                      IkReal x355 = ((1.0) * sj4);
                                      IkReal x356 = ((1.0) * x350);
                                      IkReal x357 = (sj3 * x349);
                                      IkReal x358 = ((1.0) * x349);
                                      evalcond[0] = ((sj4 * x350) + new_r20);
                                      evalcond[1] = (((-1.0) * x349 * x355) + new_r21);
                                      evalcond[2] = ((new_r11 * sj3) + (cj4 * x349) + (cj3 * new_r01));
                                      evalcond[3] = (((-1.0) * x358) + (cj3 * new_r10) + ((-1.0) * new_r00 * x354));
                                      evalcond[4] = (((-1.0) * x356) + ((-1.0) * new_r01 * x354) + (cj3 * new_r11));
                                      evalcond[5] = ((sj3 * x350) + (x349 * x352) + new_r01);
                                      evalcond[6] = ((new_r10 * sj3) + x351 + ((-1.0) * cj4 * x356));
                                      evalcond[7] = (x357 + new_r00 + ((-1.0) * x352 * x356));
                                      evalcond[8] = (((-1.0) * cj3 * x356) + (x349 * x353) + new_r11);
                                      evalcond[9] = (((-1.0) * cj3 * x358) + ((-1.0) * x353 * x356) + new_r10);
                                      evalcond[10] = ((new_r01 * x352) + x349 + ((-1.0) * new_r21 * x355) + (new_r11 * x353));
                                      evalcond[11] = (((-1.0) * x356) + (cj4 * x351) + (new_r10 * x353) + ((-1.0) * new_r20 * x355));
                                      if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                          IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                          IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                          IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH ||
                                          IKabs(evalcond[8]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[9]) > IKFAST_EVALCOND_THRESH ||
                                          IKabs(evalcond[10]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[11]) > IKFAST_EVALCOND_THRESH) {
                                        continue;
                                      }
                                    }

                                    {
                                      std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                      vinfos[0].jointtype = 1;
                                      vinfos[0].foffset = j0;
                                      vinfos[0].indices[0] = _ij0[0];
                                      vinfos[0].indices[1] = _ij0[1];
                                      vinfos[0].maxsolutions = _nj0;
                                      vinfos[1].jointtype = 1;
                                      vinfos[1].foffset = j1;
                                      vinfos[1].indices[0] = _ij1[0];
                                      vinfos[1].indices[1] = _ij1[1];
                                      vinfos[1].maxsolutions = _nj1;
                                      vinfos[2].jointtype = 1;
                                      vinfos[2].foffset = j2;
                                      vinfos[2].indices[0] = _ij2[0];
                                      vinfos[2].indices[1] = _ij2[1];
                                      vinfos[2].maxsolutions = _nj2;
                                      vinfos[3].jointtype = 1;
                                      vinfos[3].foffset = j3;
                                      vinfos[3].indices[0] = _ij3[0];
                                      vinfos[3].indices[1] = _ij3[1];
                                      vinfos[3].maxsolutions = _nj3;
                                      vinfos[4].jointtype = 1;
                                      vinfos[4].foffset = j4;
                                      vinfos[4].indices[0] = _ij4[0];
                                      vinfos[4].indices[1] = _ij4[1];
                                      vinfos[4].maxsolutions = _nj4;
                                      vinfos[5].jointtype = 1;
                                      vinfos[5].foffset = j5;
                                      vinfos[5].indices[0] = _ij5[0];
                                      vinfos[5].indices[1] = _ij5[1];
                                      vinfos[5].maxsolutions = _nj5;
                                      std::vector<int> vfree(0);
                                      solutions.AddSolution(vinfos, vfree);
                                    }
                                  }
                                }
                              }
                            }

                          } else {
                            {
                              IkReal j5array[1], cj5array[1], sj5array[1];
                              bool j5valid[1] = {false};
                              _nj5 = 1;
                              CheckValue<IkReal> x359 = IKPowWithIntegerCheck(IKsign(sj4), -1);
                              if (!x359.valid)
                                continue;
                              CheckValue<IkReal> x360 = IKatan2WithCheck(IkReal(new_r21), IkReal((-1.0) * new_r20), IKFAST_ATAN2_MAGTHRESH);
                              if (!x360.valid)
                                continue;
                              j5array[0] = ((-1.5707963267949) + ((1.5707963267949) * (x359.value)) + (x360.value));
                              sj5array[0] = IKsin(j5array[0]);
                              cj5array[0] = IKcos(j5array[0]);
                              if (j5array[0] > IKPI)
                                j5array[0] -= IK2PI;
                              else if (j5array[0] < -IKPI)
                                j5array[0] += IK2PI;
                              j5valid[0] = true;
                              for (int ij5 = 0; ij5 < 1; ++ij5) {
                                if (!j5valid[ij5])
                                  continue;
                                _ij5[0] = ij5;
                                _ij5[1] = -1;
                                for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                  if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                      IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                    j5valid[iij5] = false;
                                    _ij5[1] = iij5;
                                    break;
                                  }
                                }
                                j5 = j5array[ij5];
                                cj5 = cj5array[ij5];
                                sj5 = sj5array[ij5];
                                {
                                  IkReal evalcond[12];
                                  IkReal x361 = IKsin(j5);
                                  IkReal x362 = IKcos(j5);
                                  IkReal x363 = (cj3 * new_r00);
                                  IkReal x364 = (cj3 * cj4);
                                  IkReal x365 = (cj4 * sj3);
                                  IkReal x366 = ((1.0) * sj3);
                                  IkReal x367 = ((1.0) * sj4);
                                  IkReal x368 = ((1.0) * x362);
                                  IkReal x369 = (sj3 * x361);
                                  IkReal x370 = ((1.0) * x361);
                                  evalcond[0] = ((sj4 * x362) + new_r20);
                                  evalcond[1] = (((-1.0) * x361 * x367) + new_r21);
                                  evalcond[2] = ((new_r11 * sj3) + (cj4 * x361) + (cj3 * new_r01));
                                  evalcond[3] = (((-1.0) * new_r00 * x366) + ((-1.0) * x370) + (cj3 * new_r10));
                                  evalcond[4] = (((-1.0) * x368) + (cj3 * new_r11) + ((-1.0) * new_r01 * x366));
                                  evalcond[5] = ((x361 * x364) + (sj3 * x362) + new_r01);
                                  evalcond[6] = (((-1.0) * cj4 * x368) + (new_r10 * sj3) + x363);
                                  evalcond[7] = (x369 + ((-1.0) * x364 * x368) + new_r00);
                                  evalcond[8] = ((x361 * x365) + new_r11 + ((-1.0) * cj3 * x368));
                                  evalcond[9] = (((-1.0) * cj3 * x370) + new_r10 + ((-1.0) * x365 * x368));
                                  evalcond[10] = (x361 + (new_r01 * x364) + (new_r11 * x365) + ((-1.0) * new_r21 * x367));
                                  evalcond[11] = (((-1.0) * new_r20 * x367) + ((-1.0) * x368) + (cj4 * x363) + (new_r10 * x365));
                                  if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH ||
                                      IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                      IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[8]) > IKFAST_EVALCOND_THRESH ||
                                      IKabs(evalcond[9]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[10]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[11]) > IKFAST_EVALCOND_THRESH) {
                                    continue;
                                  }
                                }

                                {
                                  std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                  vinfos[0].jointtype = 1;
                                  vinfos[0].foffset = j0;
                                  vinfos[0].indices[0] = _ij0[0];
                                  vinfos[0].indices[1] = _ij0[1];
                                  vinfos[0].maxsolutions = _nj0;
                                  vinfos[1].jointtype = 1;
                                  vinfos[1].foffset = j1;
                                  vinfos[1].indices[0] = _ij1[0];
                                  vinfos[1].indices[1] = _ij1[1];
                                  vinfos[1].maxsolutions = _nj1;
                                  vinfos[2].jointtype = 1;
                                  vinfos[2].foffset = j2;
                                  vinfos[2].indices[0] = _ij2[0];
                                  vinfos[2].indices[1] = _ij2[1];
                                  vinfos[2].maxsolutions = _nj2;
                                  vinfos[3].jointtype = 1;
                                  vinfos[3].foffset = j3;
                                  vinfos[3].indices[0] = _ij3[0];
                                  vinfos[3].indices[1] = _ij3[1];
                                  vinfos[3].maxsolutions = _nj3;
                                  vinfos[4].jointtype = 1;
                                  vinfos[4].foffset = j4;
                                  vinfos[4].indices[0] = _ij4[0];
                                  vinfos[4].indices[1] = _ij4[1];
                                  vinfos[4].maxsolutions = _nj4;
                                  vinfos[5].jointtype = 1;
                                  vinfos[5].foffset = j5;
                                  vinfos[5].indices[0] = _ij5[0];
                                  vinfos[5].indices[1] = _ij5[1];
                                  vinfos[5].maxsolutions = _nj5;
                                  std::vector<int> vfree(0);
                                  solutions.AddSolution(vinfos, vfree);
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }

              } else {
                {
                  IkReal j5array[1], cj5array[1], sj5array[1];
                  bool j5valid[1] = {false};
                  _nj5 = 1;
                  CheckValue<IkReal> x371 = IKPowWithIntegerCheck(IKsign(sj4), -1);
                  if (!x371.valid)
                    continue;
                  CheckValue<IkReal> x372 = IKatan2WithCheck(IkReal(new_r21), IkReal((-1.0) * new_r20), IKFAST_ATAN2_MAGTHRESH);
                  if (!x372.valid)
                    continue;
                  j5array[0] = ((-1.5707963267949) + ((1.5707963267949) * (x371.value)) + (x372.value));
                  sj5array[0] = IKsin(j5array[0]);
                  cj5array[0] = IKcos(j5array[0]);
                  if (j5array[0] > IKPI)
                    j5array[0] -= IK2PI;
                  else if (j5array[0] < -IKPI)
                    j5array[0] += IK2PI;
                  j5valid[0] = true;
                  for (int ij5 = 0; ij5 < 1; ++ij5) {
                    if (!j5valid[ij5])
                      continue;
                    _ij5[0] = ij5;
                    _ij5[1] = -1;
                    for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                      if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH && IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                        j5valid[iij5] = false;
                        _ij5[1] = iij5;
                        break;
                      }
                    }
                    j5 = j5array[ij5];
                    cj5 = cj5array[ij5];
                    sj5 = sj5array[ij5];
                    {
                      IkReal evalcond[2];
                      evalcond[0] = ((sj4 * (IKcos(j5))) + new_r20);
                      evalcond[1] = (((-1.0) * sj4 * (IKsin(j5))) + new_r21);
                      if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH) {
                        continue;
                      }
                    }

                    {
                      IkReal j3eval[3];
                      j3eval[0] = sj4;
                      j3eval[1] = IKsign(sj4);
                      j3eval[2] = ((IKabs(new_r12)) + (IKabs(new_r02)));
                      if (IKabs(j3eval[0]) < 0.0000010000000000 || IKabs(j3eval[1]) < 0.0000010000000000 || IKabs(j3eval[2]) < 0.0000010000000000) {
                        {
                          IkReal j3eval[2];
                          j3eval[0] = cj5;
                          j3eval[1] = sj4;
                          if (IKabs(j3eval[0]) < 0.0000010000000000 || IKabs(j3eval[1]) < 0.0000010000000000) {
                            {
                              IkReal evalcond[5];
                              bool bgotonextstatement = true;
                              do {
                                evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs((-1.5707963267949) + j5))), 6.28318530717959)));
                                evalcond[1] = new_r20;
                                if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000) {
                                  bgotonextstatement = false;
                                  {
                                    IkReal j3array[1], cj3array[1], sj3array[1];
                                    bool j3valid[1] = {false};
                                    _nj3 = 1;
                                    if (IKabs((-1.0) * new_r00) < IKFAST_ATAN2_MAGTHRESH && IKabs(new_r10) < IKFAST_ATAN2_MAGTHRESH &&
                                        IKabs(IKsqr((-1.0) * new_r00) + IKsqr(new_r10) - 1) <= IKFAST_SINCOS_THRESH) {
                                      continue;
                                    }
                                    j3array[0] = IKatan2(((-1.0) * new_r00), new_r10);
                                    sj3array[0] = IKsin(j3array[0]);
                                    cj3array[0] = IKcos(j3array[0]);
                                    if (j3array[0] > IKPI)
                                      j3array[0] -= IK2PI;
                                    else if (j3array[0] < -IKPI)
                                      j3array[0] += IK2PI;
                                    j3valid[0] = true;
                                    for (int ij3 = 0; ij3 < 1; ++ij3) {
                                      if (!j3valid[ij3])
                                        continue;
                                      _ij3[0] = ij3;
                                      _ij3[1] = -1;
                                      for (int iij3 = ij3 + 1; iij3 < 1; ++iij3) {
                                        if (j3valid[iij3] && IKabs(cj3array[ij3] - cj3array[iij3]) < IKFAST_SOLUTION_THRESH &&
                                            IKabs(sj3array[ij3] - sj3array[iij3]) < IKFAST_SOLUTION_THRESH) {
                                          j3valid[iij3] = false;
                                          _ij3[1] = iij3;
                                          break;
                                        }
                                      }
                                      j3 = j3array[ij3];
                                      cj3 = cj3array[ij3];
                                      sj3 = sj3array[ij3];
                                      {
                                        IkReal evalcond[18];
                                        IkReal x373 = IKsin(j3);
                                        IkReal x374 = IKcos(j3);
                                        IkReal x375 = ((1.0) * sj4);
                                        IkReal x376 = (new_r22 * x373);
                                        IkReal x377 = (new_r00 * x374);
                                        IkReal x378 = (new_r11 * x373);
                                        IkReal x379 = (new_r02 * x374);
                                        IkReal x380 = (new_r01 * x374);
                                        IkReal x381 = (new_r12 * x373);
                                        IkReal x382 = ((1.0) * x373);
                                        IkReal x383 = (new_r10 * x373);
                                        evalcond[0] = (x373 + new_r00);
                                        evalcond[1] = (new_r01 + (new_r22 * x374));
                                        evalcond[2] = (x376 + new_r11);
                                        evalcond[3] = (((-1.0) * x374) + new_r10);
                                        evalcond[4] = (((-1.0) * x374 * x375) + new_r02);
                                        evalcond[5] = (((-1.0) * x373 * x375) + new_r12);
                                        evalcond[6] = (x383 + x377);
                                        evalcond[7] = ((new_r12 * x374) + ((-1.0) * new_r02 * x382));
                                        evalcond[8] = (((-1.0) * new_r01 * x382) + (new_r11 * x374));
                                        evalcond[9] = (x380 + x378 + new_r22);
                                        evalcond[10] = ((-1.0) + ((-1.0) * new_r00 * x382) + (new_r10 * x374));
                                        evalcond[11] = ((new_r10 * x376) + (new_r22 * x377));
                                        evalcond[12] = (((-1.0) * x375) + x381 + x379);
                                        evalcond[13] = (((-1.0) * x375 * x383) + ((-1.0) * x375 * x377));
                                        evalcond[14] = ((new_r12 * x376) + ((-1.0) * new_r22 * x375) + (new_r22 * x379));
                                        evalcond[15] = (((-1.0) * cj4 * new_r21) + ((-1.0) * x375 * x380) + ((-1.0) * x375 * x378));
                                        evalcond[16] = ((1.0) + ((-1.0) * sj4 * x375) + (new_r22 * x380) + (new_r11 * x376));
                                        evalcond[17] = ((1.0) + ((-1.0) * x375 * x381) + ((-1.0) * (new_r22 * new_r22)) + ((-1.0) * x375 * x379));
                                        if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                            IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                            IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                            IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH ||
                                            IKabs(evalcond[8]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[9]) > IKFAST_EVALCOND_THRESH ||
                                            IKabs(evalcond[10]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[11]) > IKFAST_EVALCOND_THRESH ||
                                            IKabs(evalcond[12]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[13]) > IKFAST_EVALCOND_THRESH ||
                                            IKabs(evalcond[14]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[15]) > IKFAST_EVALCOND_THRESH ||
                                            IKabs(evalcond[16]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[17]) > IKFAST_EVALCOND_THRESH) {
                                          continue;
                                        }
                                      }

                                      {
                                        std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                        vinfos[0].jointtype = 1;
                                        vinfos[0].foffset = j0;
                                        vinfos[0].indices[0] = _ij0[0];
                                        vinfos[0].indices[1] = _ij0[1];
                                        vinfos[0].maxsolutions = _nj0;
                                        vinfos[1].jointtype = 1;
                                        vinfos[1].foffset = j1;
                                        vinfos[1].indices[0] = _ij1[0];
                                        vinfos[1].indices[1] = _ij1[1];
                                        vinfos[1].maxsolutions = _nj1;
                                        vinfos[2].jointtype = 1;
                                        vinfos[2].foffset = j2;
                                        vinfos[2].indices[0] = _ij2[0];
                                        vinfos[2].indices[1] = _ij2[1];
                                        vinfos[2].maxsolutions = _nj2;
                                        vinfos[3].jointtype = 1;
                                        vinfos[3].foffset = j3;
                                        vinfos[3].indices[0] = _ij3[0];
                                        vinfos[3].indices[1] = _ij3[1];
                                        vinfos[3].maxsolutions = _nj3;
                                        vinfos[4].jointtype = 1;
                                        vinfos[4].foffset = j4;
                                        vinfos[4].indices[0] = _ij4[0];
                                        vinfos[4].indices[1] = _ij4[1];
                                        vinfos[4].maxsolutions = _nj4;
                                        vinfos[5].jointtype = 1;
                                        vinfos[5].foffset = j5;
                                        vinfos[5].indices[0] = _ij5[0];
                                        vinfos[5].indices[1] = _ij5[1];
                                        vinfos[5].maxsolutions = _nj5;
                                        std::vector<int> vfree(0);
                                        solutions.AddSolution(vinfos, vfree);
                                      }
                                    }
                                  }
                                }
                              } while (0);
                              if (bgotonextstatement) {
                                bool bgotonextstatement = true;
                                do {
                                  evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs((1.5707963267949) + j5))), 6.28318530717959)));
                                  evalcond[1] = new_r20;
                                  if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000) {
                                    bgotonextstatement = false;
                                    {
                                      IkReal j3array[1], cj3array[1], sj3array[1];
                                      bool j3valid[1] = {false};
                                      _nj3 = 1;
                                      if (IKabs(new_r00) < IKFAST_ATAN2_MAGTHRESH && IKabs((-1.0) * new_r10) < IKFAST_ATAN2_MAGTHRESH &&
                                          IKabs(IKsqr(new_r00) + IKsqr((-1.0) * new_r10) - 1) <= IKFAST_SINCOS_THRESH) {
                                        continue;
                                      }
                                      j3array[0] = IKatan2(new_r00, ((-1.0) * new_r10));
                                      sj3array[0] = IKsin(j3array[0]);
                                      cj3array[0] = IKcos(j3array[0]);
                                      if (j3array[0] > IKPI)
                                        j3array[0] -= IK2PI;
                                      else if (j3array[0] < -IKPI)
                                        j3array[0] += IK2PI;
                                      j3valid[0] = true;
                                      for (int ij3 = 0; ij3 < 1; ++ij3) {
                                        if (!j3valid[ij3])
                                          continue;
                                        _ij3[0] = ij3;
                                        _ij3[1] = -1;
                                        for (int iij3 = ij3 + 1; iij3 < 1; ++iij3) {
                                          if (j3valid[iij3] && IKabs(cj3array[ij3] - cj3array[iij3]) < IKFAST_SOLUTION_THRESH &&
                                              IKabs(sj3array[ij3] - sj3array[iij3]) < IKFAST_SOLUTION_THRESH) {
                                            j3valid[iij3] = false;
                                            _ij3[1] = iij3;
                                            break;
                                          }
                                        }
                                        j3 = j3array[ij3];
                                        cj3 = cj3array[ij3];
                                        sj3 = sj3array[ij3];
                                        {
                                          IkReal evalcond[18];
                                          IkReal x384 = IKcos(j3);
                                          IkReal x385 = IKsin(j3);
                                          IkReal x386 = ((1.0) * sj4);
                                          IkReal x387 = ((1.0) * new_r22);
                                          IkReal x388 = (new_r22 * x385);
                                          IkReal x389 = (new_r00 * x384);
                                          IkReal x390 = (new_r11 * x385);
                                          IkReal x391 = (new_r01 * x384);
                                          IkReal x392 = (new_r02 * x384);
                                          IkReal x393 = (new_r12 * x385);
                                          IkReal x394 = ((1.0) * x385);
                                          IkReal x395 = (new_r10 * x385);
                                          evalcond[0] = (x384 + new_r10);
                                          evalcond[1] = (((-1.0) * x394) + new_r00);
                                          evalcond[2] = (((-1.0) * x384 * x386) + new_r02);
                                          evalcond[3] = (new_r12 + ((-1.0) * x385 * x386));
                                          evalcond[4] = (((-1.0) * x384 * x387) + new_r01);
                                          evalcond[5] = (new_r11 + ((-1.0) * x385 * x387));
                                          evalcond[6] = (x389 + x395);
                                          evalcond[7] = ((new_r12 * x384) + ((-1.0) * new_r02 * x394));
                                          evalcond[8] = (((-1.0) * new_r01 * x394) + (new_r11 * x384));
                                          evalcond[9] = ((1.0) + (new_r10 * x384) + ((-1.0) * new_r00 * x394));
                                          evalcond[10] = ((new_r22 * x389) + (new_r10 * x388));
                                          evalcond[11] = (x393 + x392 + ((-1.0) * x386));
                                          evalcond[12] = (x391 + x390 + ((-1.0) * x387));
                                          evalcond[13] = (((-1.0) * x386 * x395) + ((-1.0) * x386 * x389));
                                          evalcond[14] = ((new_r12 * x388) + (new_r22 * x392) + ((-1.0) * new_r22 * x386));
                                          evalcond[15] = ((-1.0) + (sj4 * sj4) + (new_r11 * x388) + (new_r22 * x391));
                                          evalcond[16] = (((-1.0) * x386 * x391) + ((-1.0) * x386 * x390) + (new_r22 * sj4));
                                          evalcond[17] = ((1.0) + ((-1.0) * x386 * x393) + ((-1.0) * x386 * x392) + ((-1.0) * new_r22 * x387));
                                          if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                              IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                              IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                              IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH ||
                                              IKabs(evalcond[8]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[9]) > IKFAST_EVALCOND_THRESH ||
                                              IKabs(evalcond[10]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[11]) > IKFAST_EVALCOND_THRESH ||
                                              IKabs(evalcond[12]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[13]) > IKFAST_EVALCOND_THRESH ||
                                              IKabs(evalcond[14]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[15]) > IKFAST_EVALCOND_THRESH ||
                                              IKabs(evalcond[16]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[17]) > IKFAST_EVALCOND_THRESH) {
                                            continue;
                                          }
                                        }

                                        {
                                          std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                          vinfos[0].jointtype = 1;
                                          vinfos[0].foffset = j0;
                                          vinfos[0].indices[0] = _ij0[0];
                                          vinfos[0].indices[1] = _ij0[1];
                                          vinfos[0].maxsolutions = _nj0;
                                          vinfos[1].jointtype = 1;
                                          vinfos[1].foffset = j1;
                                          vinfos[1].indices[0] = _ij1[0];
                                          vinfos[1].indices[1] = _ij1[1];
                                          vinfos[1].maxsolutions = _nj1;
                                          vinfos[2].jointtype = 1;
                                          vinfos[2].foffset = j2;
                                          vinfos[2].indices[0] = _ij2[0];
                                          vinfos[2].indices[1] = _ij2[1];
                                          vinfos[2].maxsolutions = _nj2;
                                          vinfos[3].jointtype = 1;
                                          vinfos[3].foffset = j3;
                                          vinfos[3].indices[0] = _ij3[0];
                                          vinfos[3].indices[1] = _ij3[1];
                                          vinfos[3].maxsolutions = _nj3;
                                          vinfos[4].jointtype = 1;
                                          vinfos[4].foffset = j4;
                                          vinfos[4].indices[0] = _ij4[0];
                                          vinfos[4].indices[1] = _ij4[1];
                                          vinfos[4].maxsolutions = _nj4;
                                          vinfos[5].jointtype = 1;
                                          vinfos[5].foffset = j5;
                                          vinfos[5].indices[0] = _ij5[0];
                                          vinfos[5].indices[1] = _ij5[1];
                                          vinfos[5].maxsolutions = _nj5;
                                          std::vector<int> vfree(0);
                                          solutions.AddSolution(vinfos, vfree);
                                        }
                                      }
                                    }
                                  }
                                } while (0);
                                if (bgotonextstatement) {
                                  bool bgotonextstatement = true;
                                  do {
                                    evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs(j4))), 6.28318530717959)));
                                    evalcond[1] = new_r20;
                                    evalcond[2] = new_r02;
                                    evalcond[3] = new_r12;
                                    evalcond[4] = new_r21;
                                    if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000 && IKabs(evalcond[2]) < 0.0000050000000000 &&
                                        IKabs(evalcond[3]) < 0.0000050000000000 && IKabs(evalcond[4]) < 0.0000050000000000) {
                                      bgotonextstatement = false;
                                      {
                                        IkReal j3array[1], cj3array[1], sj3array[1];
                                        bool j3valid[1] = {false};
                                        _nj3 = 1;
                                        IkReal x396 = ((1.0) * new_r01);
                                        if (IKabs(((-1.0) * cj5 * x396) + ((-1.0) * new_r00 * sj5)) < IKFAST_ATAN2_MAGTHRESH &&
                                            IKabs((cj5 * new_r00) + ((-1.0) * sj5 * x396)) < IKFAST_ATAN2_MAGTHRESH &&
                                            IKabs(IKsqr(((-1.0) * cj5 * x396) + ((-1.0) * new_r00 * sj5)) + IKsqr((cj5 * new_r00) + ((-1.0) * sj5 * x396)) - 1) <=
                                              IKFAST_SINCOS_THRESH) {
                                          continue;
                                        }
                                        j3array[0] = IKatan2((((-1.0) * cj5 * x396) + ((-1.0) * new_r00 * sj5)), ((cj5 * new_r00) + ((-1.0) * sj5 * x396)));
                                        sj3array[0] = IKsin(j3array[0]);
                                        cj3array[0] = IKcos(j3array[0]);
                                        if (j3array[0] > IKPI)
                                          j3array[0] -= IK2PI;
                                        else if (j3array[0] < -IKPI)
                                          j3array[0] += IK2PI;
                                        j3valid[0] = true;
                                        for (int ij3 = 0; ij3 < 1; ++ij3) {
                                          if (!j3valid[ij3])
                                            continue;
                                          _ij3[0] = ij3;
                                          _ij3[1] = -1;
                                          for (int iij3 = ij3 + 1; iij3 < 1; ++iij3) {
                                            if (j3valid[iij3] && IKabs(cj3array[ij3] - cj3array[iij3]) < IKFAST_SOLUTION_THRESH &&
                                                IKabs(sj3array[ij3] - sj3array[iij3]) < IKFAST_SOLUTION_THRESH) {
                                              j3valid[iij3] = false;
                                              _ij3[1] = iij3;
                                              break;
                                            }
                                          }
                                          j3 = j3array[ij3];
                                          cj3 = cj3array[ij3];
                                          sj3 = sj3array[ij3];
                                          {
                                            IkReal evalcond[8];
                                            IkReal x397 = IKcos(j3);
                                            IkReal x398 = IKsin(j3);
                                            IkReal x399 = ((1.0) * cj5);
                                            IkReal x400 = (sj5 * x398);
                                            IkReal x401 = (sj5 * x397);
                                            IkReal x402 = ((1.0) * x398);
                                            IkReal x403 = (x397 * x399);
                                            evalcond[0] = ((new_r11 * x398) + sj5 + (new_r01 * x397));
                                            evalcond[1] = ((cj5 * x398) + x401 + new_r01);
                                            evalcond[2] = (x400 + new_r00 + ((-1.0) * x403));
                                            evalcond[3] = (x400 + new_r11 + ((-1.0) * x403));
                                            evalcond[4] = (((-1.0) * x399) + (new_r00 * x397) + (new_r10 * x398));
                                            evalcond[5] = (((-1.0) * x398 * x399) + new_r10 + ((-1.0) * x401));
                                            evalcond[6] = (((-1.0) * sj5) + ((-1.0) * new_r00 * x402) + (new_r10 * x397));
                                            evalcond[7] = ((new_r11 * x397) + ((-1.0) * x399) + ((-1.0) * new_r01 * x402));
                                            if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                              continue;
                                            }
                                          }

                                          {
                                            std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                            vinfos[0].jointtype = 1;
                                            vinfos[0].foffset = j0;
                                            vinfos[0].indices[0] = _ij0[0];
                                            vinfos[0].indices[1] = _ij0[1];
                                            vinfos[0].maxsolutions = _nj0;
                                            vinfos[1].jointtype = 1;
                                            vinfos[1].foffset = j1;
                                            vinfos[1].indices[0] = _ij1[0];
                                            vinfos[1].indices[1] = _ij1[1];
                                            vinfos[1].maxsolutions = _nj1;
                                            vinfos[2].jointtype = 1;
                                            vinfos[2].foffset = j2;
                                            vinfos[2].indices[0] = _ij2[0];
                                            vinfos[2].indices[1] = _ij2[1];
                                            vinfos[2].maxsolutions = _nj2;
                                            vinfos[3].jointtype = 1;
                                            vinfos[3].foffset = j3;
                                            vinfos[3].indices[0] = _ij3[0];
                                            vinfos[3].indices[1] = _ij3[1];
                                            vinfos[3].maxsolutions = _nj3;
                                            vinfos[4].jointtype = 1;
                                            vinfos[4].foffset = j4;
                                            vinfos[4].indices[0] = _ij4[0];
                                            vinfos[4].indices[1] = _ij4[1];
                                            vinfos[4].maxsolutions = _nj4;
                                            vinfos[5].jointtype = 1;
                                            vinfos[5].foffset = j5;
                                            vinfos[5].indices[0] = _ij5[0];
                                            vinfos[5].indices[1] = _ij5[1];
                                            vinfos[5].maxsolutions = _nj5;
                                            std::vector<int> vfree(0);
                                            solutions.AddSolution(vinfos, vfree);
                                          }
                                        }
                                      }
                                    }
                                  } while (0);
                                  if (bgotonextstatement) {
                                    bool bgotonextstatement = true;
                                    do {
                                      evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs((-3.14159265358979) + j4))), 6.28318530717959)));
                                      evalcond[1] = new_r20;
                                      evalcond[2] = new_r02;
                                      evalcond[3] = new_r12;
                                      evalcond[4] = new_r21;
                                      if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000 && IKabs(evalcond[2]) < 0.0000050000000000 &&
                                          IKabs(evalcond[3]) < 0.0000050000000000 && IKabs(evalcond[4]) < 0.0000050000000000) {
                                        bgotonextstatement = false;
                                        {
                                          IkReal j3array[1], cj3array[1], sj3array[1];
                                          bool j3valid[1] = {false};
                                          _nj3 = 1;
                                          IkReal x404 = ((1.0) * cj5);
                                          if (IKabs(((-1.0) * new_r00 * sj5) + ((-1.0) * new_r01 * x404)) < IKFAST_ATAN2_MAGTHRESH &&
                                              IKabs((new_r01 * sj5) + ((-1.0) * new_r00 * x404)) < IKFAST_ATAN2_MAGTHRESH &&
                                              IKabs(IKsqr(((-1.0) * new_r00 * sj5) + ((-1.0) * new_r01 * x404)) + IKsqr((new_r01 * sj5) + ((-1.0) * new_r00 * x404)) - 1) <=
                                                IKFAST_SINCOS_THRESH) {
                                            continue;
                                          }
                                          j3array[0] = IKatan2((((-1.0) * new_r00 * sj5) + ((-1.0) * new_r01 * x404)), ((new_r01 * sj5) + ((-1.0) * new_r00 * x404)));
                                          sj3array[0] = IKsin(j3array[0]);
                                          cj3array[0] = IKcos(j3array[0]);
                                          if (j3array[0] > IKPI)
                                            j3array[0] -= IK2PI;
                                          else if (j3array[0] < -IKPI)
                                            j3array[0] += IK2PI;
                                          j3valid[0] = true;
                                          for (int ij3 = 0; ij3 < 1; ++ij3) {
                                            if (!j3valid[ij3])
                                              continue;
                                            _ij3[0] = ij3;
                                            _ij3[1] = -1;
                                            for (int iij3 = ij3 + 1; iij3 < 1; ++iij3) {
                                              if (j3valid[iij3] && IKabs(cj3array[ij3] - cj3array[iij3]) < IKFAST_SOLUTION_THRESH &&
                                                  IKabs(sj3array[ij3] - sj3array[iij3]) < IKFAST_SOLUTION_THRESH) {
                                                j3valid[iij3] = false;
                                                _ij3[1] = iij3;
                                                break;
                                              }
                                            }
                                            j3 = j3array[ij3];
                                            cj3 = cj3array[ij3];
                                            sj3 = sj3array[ij3];
                                            {
                                              IkReal evalcond[8];
                                              IkReal x405 = IKsin(j3);
                                              IkReal x406 = IKcos(j3);
                                              IkReal x407 = ((1.0) * sj5);
                                              IkReal x408 = ((1.0) * cj5);
                                              IkReal x409 = (cj5 * x405);
                                              IkReal x410 = ((1.0) * x405);
                                              IkReal x411 = (x406 * x407);
                                              evalcond[0] = (cj5 + (new_r10 * x405) + (new_r00 * x406));
                                              evalcond[1] = ((cj5 * x406) + (sj5 * x405) + new_r00);
                                              evalcond[2] = (x409 + new_r01 + ((-1.0) * x411));
                                              evalcond[3] = (x409 + new_r10 + ((-1.0) * x411));
                                              evalcond[4] = ((new_r11 * x405) + (new_r01 * x406) + ((-1.0) * x407));
                                              evalcond[5] = (((-1.0) * x406 * x408) + new_r11 + ((-1.0) * x405 * x407));
                                              evalcond[6] = ((new_r10 * x406) + ((-1.0) * new_r00 * x410) + ((-1.0) * x407));
                                              evalcond[7] = ((new_r11 * x406) + ((-1.0) * x408) + ((-1.0) * new_r01 * x410));
                                              if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                  IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                  IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                  IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                                continue;
                                              }
                                            }

                                            {
                                              std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                              vinfos[0].jointtype = 1;
                                              vinfos[0].foffset = j0;
                                              vinfos[0].indices[0] = _ij0[0];
                                              vinfos[0].indices[1] = _ij0[1];
                                              vinfos[0].maxsolutions = _nj0;
                                              vinfos[1].jointtype = 1;
                                              vinfos[1].foffset = j1;
                                              vinfos[1].indices[0] = _ij1[0];
                                              vinfos[1].indices[1] = _ij1[1];
                                              vinfos[1].maxsolutions = _nj1;
                                              vinfos[2].jointtype = 1;
                                              vinfos[2].foffset = j2;
                                              vinfos[2].indices[0] = _ij2[0];
                                              vinfos[2].indices[1] = _ij2[1];
                                              vinfos[2].maxsolutions = _nj2;
                                              vinfos[3].jointtype = 1;
                                              vinfos[3].foffset = j3;
                                              vinfos[3].indices[0] = _ij3[0];
                                              vinfos[3].indices[1] = _ij3[1];
                                              vinfos[3].maxsolutions = _nj3;
                                              vinfos[4].jointtype = 1;
                                              vinfos[4].foffset = j4;
                                              vinfos[4].indices[0] = _ij4[0];
                                              vinfos[4].indices[1] = _ij4[1];
                                              vinfos[4].maxsolutions = _nj4;
                                              vinfos[5].jointtype = 1;
                                              vinfos[5].foffset = j5;
                                              vinfos[5].indices[0] = _ij5[0];
                                              vinfos[5].indices[1] = _ij5[1];
                                              vinfos[5].maxsolutions = _nj5;
                                              std::vector<int> vfree(0);
                                              solutions.AddSolution(vinfos, vfree);
                                            }
                                          }
                                        }
                                      }
                                    } while (0);
                                    if (bgotonextstatement) {
                                      bool bgotonextstatement = true;
                                      do {
                                        evalcond[0] = ((IKabs(new_r12)) + (IKabs(new_r02)));
                                        if (IKabs(evalcond[0]) < 0.0000050000000000) {
                                          bgotonextstatement = false;
                                          {
                                            IkReal j3eval[1];
                                            new_r02 = 0;
                                            new_r12 = 0;
                                            new_r20 = 0;
                                            new_r21 = 0;
                                            j3eval[0] = ((IKabs(new_r11)) + (IKabs(new_r01)));
                                            if (IKabs(j3eval[0]) < 0.0000010000000000) {
                                              {
                                                IkReal j3eval[1];
                                                new_r02 = 0;
                                                new_r12 = 0;
                                                new_r20 = 0;
                                                new_r21 = 0;
                                                j3eval[0] = ((IKabs(new_r10)) + (IKabs(new_r00)));
                                                if (IKabs(j3eval[0]) < 0.0000010000000000) {
                                                  {
                                                    IkReal j3eval[1];
                                                    new_r02 = 0;
                                                    new_r12 = 0;
                                                    new_r20 = 0;
                                                    new_r21 = 0;
                                                    j3eval[0] = ((IKabs(new_r11 * new_r22)) + (IKabs(new_r01 * new_r22)));
                                                    if (IKabs(j3eval[0]) < 0.0000010000000000) {
                                                      continue; // no branches
                                                                // [j3]

                                                    } else {
                                                      {
                                                        IkReal j3array[2], cj3array[2], sj3array[2];
                                                        bool j3valid[2] = {false};
                                                        _nj3 = 2;
                                                        CheckValue<IkReal> x413 = IKatan2WithCheck(IkReal(new_r01 * new_r22), IkReal(new_r11 * new_r22), IKFAST_ATAN2_MAGTHRESH);
                                                        if (!x413.valid)
                                                          continue;
                                                        IkReal x412 = x413.value;
                                                        j3array[0] = ((-1.0) * x412);
                                                        sj3array[0] = IKsin(j3array[0]);
                                                        cj3array[0] = IKcos(j3array[0]);
                                                        j3array[1] = ((3.14159265358979) + ((-1.0) * x412));
                                                        sj3array[1] = IKsin(j3array[1]);
                                                        cj3array[1] = IKcos(j3array[1]);
                                                        if (j3array[0] > IKPI) {
                                                          j3array[0] -= IK2PI;
                                                        } else if (j3array[0] < -IKPI) {
                                                          j3array[0] += IK2PI;
                                                        }
                                                        j3valid[0] = true;
                                                        if (j3array[1] > IKPI) {
                                                          j3array[1] -= IK2PI;
                                                        } else if (j3array[1] < -IKPI) {
                                                          j3array[1] += IK2PI;
                                                        }
                                                        j3valid[1] = true;
                                                        for (int ij3 = 0; ij3 < 2; ++ij3) {
                                                          if (!j3valid[ij3])
                                                            continue;
                                                          _ij3[0] = ij3;
                                                          _ij3[1] = -1;
                                                          for (int iij3 = ij3 + 1; iij3 < 2; ++iij3) {
                                                            if (j3valid[iij3] && IKabs(cj3array[ij3] - cj3array[iij3]) < IKFAST_SOLUTION_THRESH &&
                                                                IKabs(sj3array[ij3] - sj3array[iij3]) < IKFAST_SOLUTION_THRESH) {
                                                              j3valid[iij3] = false;
                                                              _ij3[1] = iij3;
                                                              break;
                                                            }
                                                          }
                                                          j3 = j3array[ij3];
                                                          cj3 = cj3array[ij3];
                                                          sj3 = sj3array[ij3];
                                                          {
                                                            IkReal evalcond[5];
                                                            IkReal x414 = IKsin(j3);
                                                            IkReal x415 = IKcos(j3);
                                                            IkReal x416 = (new_r00 * x415);
                                                            IkReal x417 = (new_r10 * x414);
                                                            IkReal x418 = ((1.0) * x414);
                                                            evalcond[0] = ((new_r11 * x414) + (new_r01 * x415));
                                                            evalcond[1] = (x416 + x417);
                                                            evalcond[2] = ((new_r10 * x415) + ((-1.0) * new_r00 * x418));
                                                            evalcond[3] = ((new_r11 * x415) + ((-1.0) * new_r01 * x418));
                                                            evalcond[4] = ((new_r22 * x416) + (new_r22 * x417));
                                                            if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                                IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH) {
                                                              continue;
                                                            }
                                                          }

                                                          {
                                                            std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                            vinfos[0].jointtype = 1;
                                                            vinfos[0].foffset = j0;
                                                            vinfos[0].indices[0] = _ij0[0];
                                                            vinfos[0].indices[1] = _ij0[1];
                                                            vinfos[0].maxsolutions = _nj0;
                                                            vinfos[1].jointtype = 1;
                                                            vinfos[1].foffset = j1;
                                                            vinfos[1].indices[0] = _ij1[0];
                                                            vinfos[1].indices[1] = _ij1[1];
                                                            vinfos[1].maxsolutions = _nj1;
                                                            vinfos[2].jointtype = 1;
                                                            vinfos[2].foffset = j2;
                                                            vinfos[2].indices[0] = _ij2[0];
                                                            vinfos[2].indices[1] = _ij2[1];
                                                            vinfos[2].maxsolutions = _nj2;
                                                            vinfos[3].jointtype = 1;
                                                            vinfos[3].foffset = j3;
                                                            vinfos[3].indices[0] = _ij3[0];
                                                            vinfos[3].indices[1] = _ij3[1];
                                                            vinfos[3].maxsolutions = _nj3;
                                                            vinfos[4].jointtype = 1;
                                                            vinfos[4].foffset = j4;
                                                            vinfos[4].indices[0] = _ij4[0];
                                                            vinfos[4].indices[1] = _ij4[1];
                                                            vinfos[4].maxsolutions = _nj4;
                                                            vinfos[5].jointtype = 1;
                                                            vinfos[5].foffset = j5;
                                                            vinfos[5].indices[0] = _ij5[0];
                                                            vinfos[5].indices[1] = _ij5[1];
                                                            vinfos[5].maxsolutions = _nj5;
                                                            std::vector<int> vfree(0);
                                                            solutions.AddSolution(vinfos, vfree);
                                                          }
                                                        }
                                                      }
                                                    }
                                                  }

                                                } else {
                                                  {
                                                    IkReal j3array[2], cj3array[2], sj3array[2];
                                                    bool j3valid[2] = {false};
                                                    _nj3 = 2;
                                                    CheckValue<IkReal> x420 = IKatan2WithCheck(IkReal(new_r00), IkReal(new_r10), IKFAST_ATAN2_MAGTHRESH);
                                                    if (!x420.valid)
                                                      continue;
                                                    IkReal x419 = x420.value;
                                                    j3array[0] = ((-1.0) * x419);
                                                    sj3array[0] = IKsin(j3array[0]);
                                                    cj3array[0] = IKcos(j3array[0]);
                                                    j3array[1] = ((3.14159265358979) + ((-1.0) * x419));
                                                    sj3array[1] = IKsin(j3array[1]);
                                                    cj3array[1] = IKcos(j3array[1]);
                                                    if (j3array[0] > IKPI) {
                                                      j3array[0] -= IK2PI;
                                                    } else if (j3array[0] < -IKPI) {
                                                      j3array[0] += IK2PI;
                                                    }
                                                    j3valid[0] = true;
                                                    if (j3array[1] > IKPI) {
                                                      j3array[1] -= IK2PI;
                                                    } else if (j3array[1] < -IKPI) {
                                                      j3array[1] += IK2PI;
                                                    }
                                                    j3valid[1] = true;
                                                    for (int ij3 = 0; ij3 < 2; ++ij3) {
                                                      if (!j3valid[ij3])
                                                        continue;
                                                      _ij3[0] = ij3;
                                                      _ij3[1] = -1;
                                                      for (int iij3 = ij3 + 1; iij3 < 2; ++iij3) {
                                                        if (j3valid[iij3] && IKabs(cj3array[ij3] - cj3array[iij3]) < IKFAST_SOLUTION_THRESH &&
                                                            IKabs(sj3array[ij3] - sj3array[iij3]) < IKFAST_SOLUTION_THRESH) {
                                                          j3valid[iij3] = false;
                                                          _ij3[1] = iij3;
                                                          break;
                                                        }
                                                      }
                                                      j3 = j3array[ij3];
                                                      cj3 = cj3array[ij3];
                                                      sj3 = sj3array[ij3];
                                                      {
                                                        IkReal evalcond[5];
                                                        IkReal x421 = IKcos(j3);
                                                        IkReal x422 = IKsin(j3);
                                                        IkReal x423 = (new_r22 * x422);
                                                        IkReal x424 = (new_r22 * x421);
                                                        IkReal x425 = ((1.0) * x422);
                                                        evalcond[0] = ((new_r11 * x422) + (new_r01 * x421));
                                                        evalcond[1] = (((-1.0) * new_r00 * x425) + (new_r10 * x421));
                                                        evalcond[2] = ((new_r11 * x421) + ((-1.0) * new_r01 * x425));
                                                        evalcond[3] = ((new_r11 * x423) + (new_r01 * x424));
                                                        evalcond[4] = ((new_r10 * x423) + (new_r00 * x424));
                                                        if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                            IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                            IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH) {
                                                          continue;
                                                        }
                                                      }

                                                      {
                                                        std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                        vinfos[0].jointtype = 1;
                                                        vinfos[0].foffset = j0;
                                                        vinfos[0].indices[0] = _ij0[0];
                                                        vinfos[0].indices[1] = _ij0[1];
                                                        vinfos[0].maxsolutions = _nj0;
                                                        vinfos[1].jointtype = 1;
                                                        vinfos[1].foffset = j1;
                                                        vinfos[1].indices[0] = _ij1[0];
                                                        vinfos[1].indices[1] = _ij1[1];
                                                        vinfos[1].maxsolutions = _nj1;
                                                        vinfos[2].jointtype = 1;
                                                        vinfos[2].foffset = j2;
                                                        vinfos[2].indices[0] = _ij2[0];
                                                        vinfos[2].indices[1] = _ij2[1];
                                                        vinfos[2].maxsolutions = _nj2;
                                                        vinfos[3].jointtype = 1;
                                                        vinfos[3].foffset = j3;
                                                        vinfos[3].indices[0] = _ij3[0];
                                                        vinfos[3].indices[1] = _ij3[1];
                                                        vinfos[3].maxsolutions = _nj3;
                                                        vinfos[4].jointtype = 1;
                                                        vinfos[4].foffset = j4;
                                                        vinfos[4].indices[0] = _ij4[0];
                                                        vinfos[4].indices[1] = _ij4[1];
                                                        vinfos[4].maxsolutions = _nj4;
                                                        vinfos[5].jointtype = 1;
                                                        vinfos[5].foffset = j5;
                                                        vinfos[5].indices[0] = _ij5[0];
                                                        vinfos[5].indices[1] = _ij5[1];
                                                        vinfos[5].maxsolutions = _nj5;
                                                        std::vector<int> vfree(0);
                                                        solutions.AddSolution(vinfos, vfree);
                                                      }
                                                    }
                                                  }
                                                }
                                              }

                                            } else {
                                              {
                                                IkReal j3array[2], cj3array[2], sj3array[2];
                                                bool j3valid[2] = {false};
                                                _nj3 = 2;
                                                CheckValue<IkReal> x427 = IKatan2WithCheck(IkReal(new_r01), IkReal(new_r11), IKFAST_ATAN2_MAGTHRESH);
                                                if (!x427.valid)
                                                  continue;
                                                IkReal x426 = x427.value;
                                                j3array[0] = ((-1.0) * x426);
                                                sj3array[0] = IKsin(j3array[0]);
                                                cj3array[0] = IKcos(j3array[0]);
                                                j3array[1] = ((3.14159265358979) + ((-1.0) * x426));
                                                sj3array[1] = IKsin(j3array[1]);
                                                cj3array[1] = IKcos(j3array[1]);
                                                if (j3array[0] > IKPI)
                                                  j3array[0] -= IK2PI;
                                                else if (j3array[0] < -IKPI)
                                                  j3array[0] += IK2PI;
                                                j3valid[0] = true;
                                                if (j3array[1] > IKPI)
                                                  j3array[1] -= IK2PI;
                                                else if (j3array[1] < -IKPI)
                                                  j3array[1] += IK2PI;
                                                j3valid[1] = true;
                                                for (int ij3 = 0; ij3 < 2; ++ij3) {
                                                  if (!j3valid[ij3])
                                                    continue;
                                                  _ij3[0] = ij3;
                                                  _ij3[1] = -1;
                                                  for (int iij3 = ij3 + 1; iij3 < 2; ++iij3) {
                                                    if (j3valid[iij3] && IKabs(cj3array[ij3] - cj3array[iij3]) < IKFAST_SOLUTION_THRESH &&
                                                        IKabs(sj3array[ij3] - sj3array[iij3]) < IKFAST_SOLUTION_THRESH) {
                                                      j3valid[iij3] = false;
                                                      _ij3[1] = iij3;
                                                      break;
                                                    }
                                                  }
                                                  j3 = j3array[ij3];
                                                  cj3 = cj3array[ij3];
                                                  sj3 = sj3array[ij3];
                                                  {
                                                    IkReal evalcond[5];
                                                    IkReal x428 = IKcos(j3);
                                                    IkReal x429 = IKsin(j3);
                                                    IkReal x430 = (new_r22 * x429);
                                                    IkReal x431 = (new_r22 * x428);
                                                    IkReal x432 = ((1.0) * x429);
                                                    evalcond[0] = ((new_r10 * x429) + (new_r00 * x428));
                                                    evalcond[1] = (((-1.0) * new_r00 * x432) + (new_r10 * x428));
                                                    evalcond[2] = (((-1.0) * new_r01 * x432) + (new_r11 * x428));
                                                    evalcond[3] = ((new_r01 * x431) + (new_r11 * x430));
                                                    evalcond[4] = ((new_r00 * x431) + (new_r10 * x430));
                                                    if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                        IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                        IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH) {
                                                      continue;
                                                    }
                                                  }

                                                  {
                                                    std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                    vinfos[0].jointtype = 1;
                                                    vinfos[0].foffset = j0;
                                                    vinfos[0].indices[0] = _ij0[0];
                                                    vinfos[0].indices[1] = _ij0[1];
                                                    vinfos[0].maxsolutions = _nj0;
                                                    vinfos[1].jointtype = 1;
                                                    vinfos[1].foffset = j1;
                                                    vinfos[1].indices[0] = _ij1[0];
                                                    vinfos[1].indices[1] = _ij1[1];
                                                    vinfos[1].maxsolutions = _nj1;
                                                    vinfos[2].jointtype = 1;
                                                    vinfos[2].foffset = j2;
                                                    vinfos[2].indices[0] = _ij2[0];
                                                    vinfos[2].indices[1] = _ij2[1];
                                                    vinfos[2].maxsolutions = _nj2;
                                                    vinfos[3].jointtype = 1;
                                                    vinfos[3].foffset = j3;
                                                    vinfos[3].indices[0] = _ij3[0];
                                                    vinfos[3].indices[1] = _ij3[1];
                                                    vinfos[3].maxsolutions = _nj3;
                                                    vinfos[4].jointtype = 1;
                                                    vinfos[4].foffset = j4;
                                                    vinfos[4].indices[0] = _ij4[0];
                                                    vinfos[4].indices[1] = _ij4[1];
                                                    vinfos[4].maxsolutions = _nj4;
                                                    vinfos[5].jointtype = 1;
                                                    vinfos[5].foffset = j5;
                                                    vinfos[5].indices[0] = _ij5[0];
                                                    vinfos[5].indices[1] = _ij5[1];
                                                    vinfos[5].maxsolutions = _nj5;
                                                    std::vector<int> vfree(0);
                                                    solutions.AddSolution(vinfos, vfree);
                                                  }
                                                }
                                              }
                                            }
                                          }
                                        }
                                      } while (0);
                                      if (bgotonextstatement) {
                                        bool bgotonextstatement = true;
                                        do {
                                          if (1) {
                                            bgotonextstatement = false;
                                            continue; // branch miss [j3]
                                          }
                                        } while (0);
                                        if (bgotonextstatement) {
                                        }
                                      }
                                    }
                                  }
                                }
                              }
                            }

                          } else {
                            {
                              IkReal j3array[1], cj3array[1], sj3array[1];
                              bool j3valid[1] = {false};
                              _nj3 = 1;
                              CheckValue<IkReal> x434 = IKPowWithIntegerCheck(sj4, -1);
                              if (!x434.valid)
                                continue;
                              IkReal x433 = x434.value;
                              CheckValue<IkReal> x435 = IKPowWithIntegerCheck(cj5, -1);
                              if (!x435.valid)
                                continue;
                              if (IKabs(x433 * (x435.value) * (((-1.0) * cj4 * new_r02 * sj5) + ((-1.0) * new_r01 * sj4))) < IKFAST_ATAN2_MAGTHRESH &&
                                  IKabs(new_r02 * x433) < IKFAST_ATAN2_MAGTHRESH &&
                                  IKabs(IKsqr(x433 * (x435.value) * (((-1.0) * cj4 * new_r02 * sj5) + ((-1.0) * new_r01 * sj4))) + IKsqr(new_r02 * x433) - 1) <=
                                    IKFAST_SINCOS_THRESH) {
                                continue;
                              }
                              j3array[0] = IKatan2((x433 * (x435.value) * (((-1.0) * cj4 * new_r02 * sj5) + ((-1.0) * new_r01 * sj4))), (new_r02 * x433));
                              sj3array[0] = IKsin(j3array[0]);
                              cj3array[0] = IKcos(j3array[0]);
                              if (j3array[0] > IKPI)
                                j3array[0] -= IK2PI;
                              else if (j3array[0] < -IKPI)
                                j3array[0] += IK2PI;
                              j3valid[0] = true;
                              for (int ij3 = 0; ij3 < 1; ++ij3) {
                                if (!j3valid[ij3])
                                  continue;
                                _ij3[0] = ij3;
                                _ij3[1] = -1;
                                for (int iij3 = ij3 + 1; iij3 < 1; ++iij3) {
                                  if (j3valid[iij3] && IKabs(cj3array[ij3] - cj3array[iij3]) < IKFAST_SOLUTION_THRESH &&
                                      IKabs(sj3array[ij3] - sj3array[iij3]) < IKFAST_SOLUTION_THRESH) {
                                    j3valid[iij3] = false;
                                    _ij3[1] = iij3;
                                    break;
                                  }
                                }
                                j3 = j3array[ij3];
                                cj3 = cj3array[ij3];
                                sj3 = sj3array[ij3];
                                {
                                  IkReal evalcond[18];
                                  IkReal x436 = IKcos(j3);
                                  IkReal x437 = IKsin(j3);
                                  IkReal x438 = ((1.0) * cj5);
                                  IkReal x439 = ((1.0) * cj4);
                                  IkReal x440 = ((1.0) * sj4);
                                  IkReal x441 = ((1.0) * sj5);
                                  IkReal x442 = (cj4 * x436);
                                  IkReal x443 = (cj4 * x437);
                                  IkReal x444 = (cj5 * x437);
                                  IkReal x445 = (new_r11 * x437);
                                  IkReal x446 = (new_r02 * x436);
                                  IkReal x447 = (new_r01 * x436);
                                  IkReal x448 = (new_r12 * x437);
                                  IkReal x449 = (new_r00 * x436);
                                  IkReal x450 = ((1.0) * x437);
                                  IkReal x451 = (new_r10 * x437);
                                  evalcond[0] = (((-1.0) * x436 * x440) + new_r02);
                                  evalcond[1] = (((-1.0) * x437 * x440) + new_r12);
                                  evalcond[2] = ((new_r12 * x436) + ((-1.0) * new_r02 * x450));
                                  evalcond[3] = (x444 + (sj5 * x442) + new_r01);
                                  evalcond[4] = (((-1.0) * x440) + x448 + x446);
                                  evalcond[5] = ((cj4 * sj5) + x445 + x447);
                                  evalcond[6] = (((-1.0) * x438 * x442) + new_r00 + (sj5 * x437));
                                  evalcond[7] = (((-1.0) * x436 * x438) + (sj5 * x443) + new_r11);
                                  evalcond[8] = (((-1.0) * x441) + ((-1.0) * new_r00 * x450) + (new_r10 * x436));
                                  evalcond[9] = (((-1.0) * x438) + ((-1.0) * new_r01 * x450) + (new_r11 * x436));
                                  evalcond[10] = (((-1.0) * cj4 * x438) + x449 + x451);
                                  evalcond[11] = (((-1.0) * x438 * x443) + ((-1.0) * x436 * x441) + new_r10);
                                  evalcond[12] = ((new_r12 * x443) + (new_r02 * x442) + ((-1.0) * new_r22 * x440));
                                  evalcond[13] = ((new_r01 * x442) + sj5 + (new_r11 * x443) + ((-1.0) * new_r21 * x440));
                                  evalcond[14] = (((-1.0) * x440 * x451) + ((-1.0) * new_r20 * x439) + ((-1.0) * x440 * x449));
                                  evalcond[15] = (((-1.0) * x440 * x445) + ((-1.0) * x440 * x447) + ((-1.0) * new_r21 * x439));
                                  evalcond[16] = ((1.0) + ((-1.0) * x440 * x446) + ((-1.0) * x440 * x448) + ((-1.0) * new_r22 * x439));
                                  evalcond[17] = ((new_r00 * x442) + ((-1.0) * x438) + (new_r10 * x443) + ((-1.0) * new_r20 * x440));
                                  if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH ||
                                      IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                      IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[8]) > IKFAST_EVALCOND_THRESH ||
                                      IKabs(evalcond[9]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[10]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[11]) > IKFAST_EVALCOND_THRESH ||
                                      IKabs(evalcond[12]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[13]) > IKFAST_EVALCOND_THRESH ||
                                      IKabs(evalcond[14]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[15]) > IKFAST_EVALCOND_THRESH ||
                                      IKabs(evalcond[16]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[17]) > IKFAST_EVALCOND_THRESH) {
                                    continue;
                                  }
                                }

                                {
                                  std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                  vinfos[0].jointtype = 1;
                                  vinfos[0].foffset = j0;
                                  vinfos[0].indices[0] = _ij0[0];
                                  vinfos[0].indices[1] = _ij0[1];
                                  vinfos[0].maxsolutions = _nj0;
                                  vinfos[1].jointtype = 1;
                                  vinfos[1].foffset = j1;
                                  vinfos[1].indices[0] = _ij1[0];
                                  vinfos[1].indices[1] = _ij1[1];
                                  vinfos[1].maxsolutions = _nj1;
                                  vinfos[2].jointtype = 1;
                                  vinfos[2].foffset = j2;
                                  vinfos[2].indices[0] = _ij2[0];
                                  vinfos[2].indices[1] = _ij2[1];
                                  vinfos[2].maxsolutions = _nj2;
                                  vinfos[3].jointtype = 1;
                                  vinfos[3].foffset = j3;
                                  vinfos[3].indices[0] = _ij3[0];
                                  vinfos[3].indices[1] = _ij3[1];
                                  vinfos[3].maxsolutions = _nj3;
                                  vinfos[4].jointtype = 1;
                                  vinfos[4].foffset = j4;
                                  vinfos[4].indices[0] = _ij4[0];
                                  vinfos[4].indices[1] = _ij4[1];
                                  vinfos[4].maxsolutions = _nj4;
                                  vinfos[5].jointtype = 1;
                                  vinfos[5].foffset = j5;
                                  vinfos[5].indices[0] = _ij5[0];
                                  vinfos[5].indices[1] = _ij5[1];
                                  vinfos[5].maxsolutions = _nj5;
                                  std::vector<int> vfree(0);
                                  solutions.AddSolution(vinfos, vfree);
                                }
                              }
                            }
                          }
                        }

                      } else {
                        {
                          IkReal j3array[1], cj3array[1], sj3array[1];
                          bool j3valid[1] = {false};
                          _nj3 = 1;
                          CheckValue<IkReal> x452 = IKPowWithIntegerCheck(IKsign(sj4), -1);
                          if (!x452.valid)
                            continue;
                          CheckValue<IkReal> x453 = IKatan2WithCheck(IkReal(new_r12), IkReal(new_r02), IKFAST_ATAN2_MAGTHRESH);
                          if (!x453.valid)
                            continue;
                          j3array[0] = ((-1.5707963267949) + ((1.5707963267949) * (x452.value)) + (x453.value));
                          sj3array[0] = IKsin(j3array[0]);
                          cj3array[0] = IKcos(j3array[0]);
                          if (j3array[0] > IKPI)
                            j3array[0] -= IK2PI;
                          else if (j3array[0] < -IKPI)
                            j3array[0] += IK2PI;
                          j3valid[0] = true;
                          for (int ij3 = 0; ij3 < 1; ++ij3) {
                            if (!j3valid[ij3])
                              continue;
                            _ij3[0] = ij3;
                            _ij3[1] = -1;
                            for (int iij3 = ij3 + 1; iij3 < 1; ++iij3) {
                              if (j3valid[iij3] && IKabs(cj3array[ij3] - cj3array[iij3]) < IKFAST_SOLUTION_THRESH &&
                                  IKabs(sj3array[ij3] - sj3array[iij3]) < IKFAST_SOLUTION_THRESH) {
                                j3valid[iij3] = false;
                                _ij3[1] = iij3;
                                break;
                              }
                            }
                            j3 = j3array[ij3];
                            cj3 = cj3array[ij3];
                            sj3 = sj3array[ij3];
                            {
                              IkReal evalcond[18];
                              IkReal x454 = IKcos(j3);
                              IkReal x455 = IKsin(j3);
                              IkReal x456 = ((1.0) * cj5);
                              IkReal x457 = ((1.0) * cj4);
                              IkReal x458 = ((1.0) * sj4);
                              IkReal x459 = ((1.0) * sj5);
                              IkReal x460 = (cj4 * x454);
                              IkReal x461 = (cj4 * x455);
                              IkReal x462 = (cj5 * x455);
                              IkReal x463 = (new_r11 * x455);
                              IkReal x464 = (new_r02 * x454);
                              IkReal x465 = (new_r01 * x454);
                              IkReal x466 = (new_r12 * x455);
                              IkReal x467 = (new_r00 * x454);
                              IkReal x468 = ((1.0) * x455);
                              IkReal x469 = (new_r10 * x455);
                              evalcond[0] = (((-1.0) * x454 * x458) + new_r02);
                              evalcond[1] = (((-1.0) * x455 * x458) + new_r12);
                              evalcond[2] = ((new_r12 * x454) + ((-1.0) * new_r02 * x468));
                              evalcond[3] = (x462 + new_r01 + (sj5 * x460));
                              evalcond[4] = (((-1.0) * x458) + x466 + x464);
                              evalcond[5] = ((cj4 * sj5) + x463 + x465);
                              evalcond[6] = (((-1.0) * x456 * x460) + new_r00 + (sj5 * x455));
                              evalcond[7] = (((-1.0) * x454 * x456) + new_r11 + (sj5 * x461));
                              evalcond[8] = ((new_r10 * x454) + ((-1.0) * x459) + ((-1.0) * new_r00 * x468));
                              evalcond[9] = ((new_r11 * x454) + ((-1.0) * new_r01 * x468) + ((-1.0) * x456));
                              evalcond[10] = (((-1.0) * cj4 * x456) + x469 + x467);
                              evalcond[11] = (((-1.0) * x456 * x461) + ((-1.0) * x454 * x459) + new_r10);
                              evalcond[12] = ((new_r12 * x461) + ((-1.0) * new_r22 * x458) + (new_r02 * x460));
                              evalcond[13] = (((-1.0) * new_r21 * x458) + (new_r11 * x461) + sj5 + (new_r01 * x460));
                              evalcond[14] = (((-1.0) * new_r20 * x457) + ((-1.0) * x458 * x469) + ((-1.0) * x458 * x467));
                              evalcond[15] = (((-1.0) * new_r21 * x457) + ((-1.0) * x458 * x463) + ((-1.0) * x458 * x465));
                              evalcond[16] = ((1.0) + ((-1.0) * new_r22 * x457) + ((-1.0) * x458 * x466) + ((-1.0) * x458 * x464));
                              evalcond[17] = ((new_r10 * x461) + (new_r00 * x460) + ((-1.0) * new_r20 * x458) + ((-1.0) * x456));
                              if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH ||
                                  IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                  IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[8]) > IKFAST_EVALCOND_THRESH ||
                                  IKabs(evalcond[9]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[10]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[11]) > IKFAST_EVALCOND_THRESH ||
                                  IKabs(evalcond[12]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[13]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[14]) > IKFAST_EVALCOND_THRESH ||
                                  IKabs(evalcond[15]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[16]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[17]) > IKFAST_EVALCOND_THRESH) {
                                continue;
                              }
                            }

                            {
                              std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                              vinfos[0].jointtype = 1;
                              vinfos[0].foffset = j0;
                              vinfos[0].indices[0] = _ij0[0];
                              vinfos[0].indices[1] = _ij0[1];
                              vinfos[0].maxsolutions = _nj0;
                              vinfos[1].jointtype = 1;
                              vinfos[1].foffset = j1;
                              vinfos[1].indices[0] = _ij1[0];
                              vinfos[1].indices[1] = _ij1[1];
                              vinfos[1].maxsolutions = _nj1;
                              vinfos[2].jointtype = 1;
                              vinfos[2].foffset = j2;
                              vinfos[2].indices[0] = _ij2[0];
                              vinfos[2].indices[1] = _ij2[1];
                              vinfos[2].maxsolutions = _nj2;
                              vinfos[3].jointtype = 1;
                              vinfos[3].foffset = j3;
                              vinfos[3].indices[0] = _ij3[0];
                              vinfos[3].indices[1] = _ij3[1];
                              vinfos[3].maxsolutions = _nj3;
                              vinfos[4].jointtype = 1;
                              vinfos[4].foffset = j4;
                              vinfos[4].indices[0] = _ij4[0];
                              vinfos[4].indices[1] = _ij4[1];
                              vinfos[4].maxsolutions = _nj4;
                              vinfos[5].jointtype = 1;
                              vinfos[5].foffset = j5;
                              vinfos[5].indices[0] = _ij5[0];
                              vinfos[5].indices[1] = _ij5[1];
                              vinfos[5].maxsolutions = _nj5;
                              std::vector<int> vfree(0);
                              solutions.AddSolution(vinfos, vfree);
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }

          } else {
            {
              IkReal j3array[1], cj3array[1], sj3array[1];
              bool j3valid[1] = {false};
              _nj3 = 1;
              CheckValue<IkReal> x470 = IKPowWithIntegerCheck(IKsign(sj4), -1);
              if (!x470.valid)
                continue;
              CheckValue<IkReal> x471 = IKatan2WithCheck(IkReal(new_r12), IkReal(new_r02), IKFAST_ATAN2_MAGTHRESH);
              if (!x471.valid)
                continue;
              j3array[0] = ((-1.5707963267949) + ((1.5707963267949) * (x470.value)) + (x471.value));
              sj3array[0] = IKsin(j3array[0]);
              cj3array[0] = IKcos(j3array[0]);
              if (j3array[0] > IKPI)
                j3array[0] -= IK2PI;
              else if (j3array[0] < -IKPI)
                j3array[0] += IK2PI;
              j3valid[0] = true;
              for (int ij3 = 0; ij3 < 1; ++ij3) {
                if (!j3valid[ij3])
                  continue;
                _ij3[0] = ij3;
                _ij3[1] = -1;
                for (int iij3 = ij3 + 1; iij3 < 1; ++iij3) {
                  if (j3valid[iij3] && IKabs(cj3array[ij3] - cj3array[iij3]) < IKFAST_SOLUTION_THRESH && IKabs(sj3array[ij3] - sj3array[iij3]) < IKFAST_SOLUTION_THRESH) {
                    j3valid[iij3] = false;
                    _ij3[1] = iij3;
                    break;
                  }
                }
                j3 = j3array[ij3];
                cj3 = cj3array[ij3];
                sj3 = sj3array[ij3];
                {
                  IkReal evalcond[8];
                  IkReal x472 = IKcos(j3);
                  IkReal x473 = IKsin(j3);
                  IkReal x474 = ((1.0) * sj4);
                  IkReal x475 = ((1.0) * cj4);
                  IkReal x476 = (new_r02 * x472);
                  IkReal x477 = (new_r12 * x473);
                  evalcond[0] = (((-1.0) * x472 * x474) + new_r02);
                  evalcond[1] = (((-1.0) * x473 * x474) + new_r12);
                  evalcond[2] = ((new_r12 * x472) + ((-1.0) * new_r02 * x473));
                  evalcond[3] = (((-1.0) * x474) + x476 + x477);
                  evalcond[4] = ((cj4 * x477) + (cj4 * x476) + ((-1.0) * new_r22 * x474));
                  evalcond[5] = (((-1.0) * new_r00 * x472 * x474) + ((-1.0) * new_r10 * x473 * x474) + ((-1.0) * new_r20 * x475));
                  evalcond[6] = (((-1.0) * new_r21 * x475) + ((-1.0) * new_r11 * x473 * x474) + ((-1.0) * new_r01 * x472 * x474));
                  evalcond[7] = ((1.0) + ((-1.0) * new_r22 * x475) + ((-1.0) * x474 * x477) + ((-1.0) * x474 * x476));
                  if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH ||
                      IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                      IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                    continue;
                  }
                }

                {
                  IkReal j5eval[3];
                  j5eval[0] = sj4;
                  j5eval[1] = IKsign(sj4);
                  j5eval[2] = ((IKabs(new_r20)) + (IKabs(new_r21)));
                  if (IKabs(j5eval[0]) < 0.0000010000000000 || IKabs(j5eval[1]) < 0.0000010000000000 || IKabs(j5eval[2]) < 0.0000010000000000) {
                    {
                      IkReal j5eval[2];
                      j5eval[0] = sj3;
                      j5eval[1] = sj4;
                      if (IKabs(j5eval[0]) < 0.0000010000000000 || IKabs(j5eval[1]) < 0.0000010000000000) {
                        {
                          IkReal j5eval[3];
                          j5eval[0] = cj3;
                          j5eval[1] = cj4;
                          j5eval[2] = sj4;
                          if (IKabs(j5eval[0]) < 0.0000010000000000 || IKabs(j5eval[1]) < 0.0000010000000000 || IKabs(j5eval[2]) < 0.0000010000000000) {
                            {
                              IkReal evalcond[5];
                              bool bgotonextstatement = true;
                              do {
                                evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs((-1.5707963267949) + j3))), 6.28318530717959)));
                                evalcond[1] = new_r02;
                                if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000) {
                                  bgotonextstatement = false;
                                  {
                                    IkReal j5eval[3];
                                    sj3 = 1.0;
                                    cj3 = 0;
                                    j3 = 1.5707963267949;
                                    j5eval[0] = sj4;
                                    j5eval[1] = IKsign(sj4);
                                    j5eval[2] = ((IKabs(new_r20)) + (IKabs(new_r21)));
                                    if (IKabs(j5eval[0]) < 0.0000010000000000 || IKabs(j5eval[1]) < 0.0000010000000000 || IKabs(j5eval[2]) < 0.0000010000000000) {
                                      {
                                        IkReal j5eval[3];
                                        sj3 = 1.0;
                                        cj3 = 0;
                                        j3 = 1.5707963267949;
                                        j5eval[0] = cj4;
                                        j5eval[1] = IKsign(cj4);
                                        j5eval[2] = ((IKabs(new_r11)) + (IKabs(new_r10)));
                                        if (IKabs(j5eval[0]) < 0.0000010000000000 || IKabs(j5eval[1]) < 0.0000010000000000 || IKabs(j5eval[2]) < 0.0000010000000000) {
                                          {
                                            IkReal j5eval[1];
                                            sj3 = 1.0;
                                            cj3 = 0;
                                            j3 = 1.5707963267949;
                                            j5eval[0] = sj4;
                                            if (IKabs(j5eval[0]) < 0.0000010000000000) {
                                              {
                                                IkReal evalcond[4];
                                                bool bgotonextstatement = true;
                                                do {
                                                  evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs(j4))), 6.28318530717959)));
                                                  evalcond[1] = new_r20;
                                                  evalcond[2] = new_r12;
                                                  evalcond[3] = new_r21;
                                                  if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000 &&
                                                      IKabs(evalcond[2]) < 0.0000050000000000 && IKabs(evalcond[3]) < 0.0000050000000000) {
                                                    bgotonextstatement = false;
                                                    {
                                                      IkReal j5array[1], cj5array[1], sj5array[1];
                                                      bool j5valid[1] = {false};
                                                      _nj5 = 1;
                                                      if (IKabs((-1.0) * new_r11) < IKFAST_ATAN2_MAGTHRESH && IKabs(new_r10) < IKFAST_ATAN2_MAGTHRESH &&
                                                          IKabs(IKsqr((-1.0) * new_r11) + IKsqr(new_r10) - 1) <= IKFAST_SINCOS_THRESH) {
                                                        continue;
                                                      }
                                                      j5array[0] = IKatan2(((-1.0) * new_r11), new_r10);
                                                      sj5array[0] = IKsin(j5array[0]);
                                                      cj5array[0] = IKcos(j5array[0]);
                                                      if (j5array[0] > IKPI) {
                                                        j5array[0] -= IK2PI;
                                                      } else if (j5array[0] < -IKPI) {
                                                        j5array[0] += IK2PI;
                                                      }
                                                      j5valid[0] = true;
                                                      for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                        if (!j5valid[ij5])
                                                          continue;
                                                        _ij5[0] = ij5;
                                                        _ij5[1] = -1;
                                                        for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                          if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                              IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                            j5valid[iij5] = false;
                                                            _ij5[1] = iij5;
                                                            break;
                                                          }
                                                        }
                                                        j5 = j5array[ij5];
                                                        cj5 = cj5array[ij5];
                                                        sj5 = sj5array[ij5];
                                                        {
                                                          IkReal evalcond[4];
                                                          IkReal x478 = IKsin(j5);
                                                          IkReal x479 = ((1.0) * (IKcos(j5)));
                                                          evalcond[0] = (x478 + new_r11);
                                                          evalcond[1] = (((-1.0) * x479) + new_r10);
                                                          evalcond[2] = (((-1.0) * x478) + ((-1.0) * new_r00));
                                                          evalcond[3] = (((-1.0) * x479) + ((-1.0) * new_r01));
                                                          if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                              IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH) {
                                                            continue;
                                                          }
                                                        }

                                                        {
                                                          std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                          vinfos[0].jointtype = 1;
                                                          vinfos[0].foffset = j0;
                                                          vinfos[0].indices[0] = _ij0[0];
                                                          vinfos[0].indices[1] = _ij0[1];
                                                          vinfos[0].maxsolutions = _nj0;
                                                          vinfos[1].jointtype = 1;
                                                          vinfos[1].foffset = j1;
                                                          vinfos[1].indices[0] = _ij1[0];
                                                          vinfos[1].indices[1] = _ij1[1];
                                                          vinfos[1].maxsolutions = _nj1;
                                                          vinfos[2].jointtype = 1;
                                                          vinfos[2].foffset = j2;
                                                          vinfos[2].indices[0] = _ij2[0];
                                                          vinfos[2].indices[1] = _ij2[1];
                                                          vinfos[2].maxsolutions = _nj2;
                                                          vinfos[3].jointtype = 1;
                                                          vinfos[3].foffset = j3;
                                                          vinfos[3].indices[0] = _ij3[0];
                                                          vinfos[3].indices[1] = _ij3[1];
                                                          vinfos[3].maxsolutions = _nj3;
                                                          vinfos[4].jointtype = 1;
                                                          vinfos[4].foffset = j4;
                                                          vinfos[4].indices[0] = _ij4[0];
                                                          vinfos[4].indices[1] = _ij4[1];
                                                          vinfos[4].maxsolutions = _nj4;
                                                          vinfos[5].jointtype = 1;
                                                          vinfos[5].foffset = j5;
                                                          vinfos[5].indices[0] = _ij5[0];
                                                          vinfos[5].indices[1] = _ij5[1];
                                                          vinfos[5].maxsolutions = _nj5;
                                                          std::vector<int> vfree(0);
                                                          solutions.AddSolution(vinfos, vfree);
                                                        }
                                                      }
                                                    }
                                                  }
                                                } while (0);
                                                if (bgotonextstatement) {
                                                  bool bgotonextstatement = true;
                                                  do {
                                                    evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs((-3.14159265358979) + j4))), 6.28318530717959)));
                                                    evalcond[1] = new_r20;
                                                    evalcond[2] = new_r12;
                                                    evalcond[3] = new_r21;
                                                    if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000 &&
                                                        IKabs(evalcond[2]) < 0.0000050000000000 && IKabs(evalcond[3]) < 0.0000050000000000) {
                                                      bgotonextstatement = false;
                                                      {
                                                        IkReal j5array[1], cj5array[1], sj5array[1];
                                                        bool j5valid[1] = {false};
                                                        _nj5 = 1;
                                                        if (IKabs(new_r11) < IKFAST_ATAN2_MAGTHRESH && IKabs((-1.0) * new_r10) < IKFAST_ATAN2_MAGTHRESH &&
                                                            IKabs(IKsqr(new_r11) + IKsqr((-1.0) * new_r10) - 1) <= IKFAST_SINCOS_THRESH) {
                                                          continue;
                                                        }
                                                        j5array[0] = IKatan2(new_r11, ((-1.0) * new_r10));
                                                        sj5array[0] = IKsin(j5array[0]);
                                                        cj5array[0] = IKcos(j5array[0]);
                                                        if (j5array[0] > IKPI) {
                                                          j5array[0] -= IK2PI;
                                                        } else if (j5array[0] < -IKPI) {
                                                          j5array[0] += IK2PI;
                                                        }
                                                        j5valid[0] = true;
                                                        for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                          if (!j5valid[ij5])
                                                            continue;
                                                          _ij5[0] = ij5;
                                                          _ij5[1] = -1;
                                                          for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                            if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                              j5valid[iij5] = false;
                                                              _ij5[1] = iij5;
                                                              break;
                                                            }
                                                          }
                                                          j5 = j5array[ij5];
                                                          cj5 = cj5array[ij5];
                                                          sj5 = sj5array[ij5];
                                                          {
                                                            IkReal evalcond[4];
                                                            IkReal x480 = IKcos(j5);
                                                            IkReal x481 = ((1.0) * (IKsin(j5)));
                                                            evalcond[0] = (x480 + new_r10);
                                                            evalcond[1] = (((-1.0) * x481) + new_r11);
                                                            evalcond[2] = (((-1.0) * x481) + ((-1.0) * new_r00));
                                                            evalcond[3] = (((-1.0) * x480) + ((-1.0) * new_r01));
                                                            if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH) {
                                                              continue;
                                                            }
                                                          }

                                                          {
                                                            std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                            vinfos[0].jointtype = 1;
                                                            vinfos[0].foffset = j0;
                                                            vinfos[0].indices[0] = _ij0[0];
                                                            vinfos[0].indices[1] = _ij0[1];
                                                            vinfos[0].maxsolutions = _nj0;
                                                            vinfos[1].jointtype = 1;
                                                            vinfos[1].foffset = j1;
                                                            vinfos[1].indices[0] = _ij1[0];
                                                            vinfos[1].indices[1] = _ij1[1];
                                                            vinfos[1].maxsolutions = _nj1;
                                                            vinfos[2].jointtype = 1;
                                                            vinfos[2].foffset = j2;
                                                            vinfos[2].indices[0] = _ij2[0];
                                                            vinfos[2].indices[1] = _ij2[1];
                                                            vinfos[2].maxsolutions = _nj2;
                                                            vinfos[3].jointtype = 1;
                                                            vinfos[3].foffset = j3;
                                                            vinfos[3].indices[0] = _ij3[0];
                                                            vinfos[3].indices[1] = _ij3[1];
                                                            vinfos[3].maxsolutions = _nj3;
                                                            vinfos[4].jointtype = 1;
                                                            vinfos[4].foffset = j4;
                                                            vinfos[4].indices[0] = _ij4[0];
                                                            vinfos[4].indices[1] = _ij4[1];
                                                            vinfos[4].maxsolutions = _nj4;
                                                            vinfos[5].jointtype = 1;
                                                            vinfos[5].foffset = j5;
                                                            vinfos[5].indices[0] = _ij5[0];
                                                            vinfos[5].indices[1] = _ij5[1];
                                                            vinfos[5].maxsolutions = _nj5;
                                                            std::vector<int> vfree(0);
                                                            solutions.AddSolution(vinfos, vfree);
                                                          }
                                                        }
                                                      }
                                                    }
                                                  } while (0);
                                                  if (bgotonextstatement) {
                                                    bool bgotonextstatement = true;
                                                    do {
                                                      evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs((-1.5707963267949) + j4))), 6.28318530717959)));
                                                      evalcond[1] = new_r22;
                                                      evalcond[2] = new_r11;
                                                      evalcond[3] = new_r10;
                                                      if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000 &&
                                                          IKabs(evalcond[2]) < 0.0000050000000000 && IKabs(evalcond[3]) < 0.0000050000000000) {
                                                        bgotonextstatement = false;
                                                        {
                                                          IkReal j5array[1], cj5array[1], sj5array[1];
                                                          bool j5valid[1] = {false};
                                                          _nj5 = 1;
                                                          if (IKabs(new_r21) < IKFAST_ATAN2_MAGTHRESH && IKabs((-1.0) * new_r20) < IKFAST_ATAN2_MAGTHRESH &&
                                                              IKabs(IKsqr(new_r21) + IKsqr((-1.0) * new_r20) - 1) <= IKFAST_SINCOS_THRESH) {
                                                            continue;
                                                          }
                                                          j5array[0] = IKatan2(new_r21, ((-1.0) * new_r20));
                                                          sj5array[0] = IKsin(j5array[0]);
                                                          cj5array[0] = IKcos(j5array[0]);
                                                          if (j5array[0] > IKPI) {
                                                            j5array[0] -= IK2PI;
                                                          } else if (j5array[0] < -IKPI) {
                                                            j5array[0] += IK2PI;
                                                          }
                                                          j5valid[0] = true;
                                                          for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                            if (!j5valid[ij5])
                                                              continue;
                                                            _ij5[0] = ij5;
                                                            _ij5[1] = -1;
                                                            for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                              if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                  IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                j5valid[iij5] = false;
                                                                _ij5[1] = iij5;
                                                                break;
                                                              }
                                                            }
                                                            j5 = j5array[ij5];
                                                            cj5 = cj5array[ij5];
                                                            sj5 = sj5array[ij5];
                                                            {
                                                              IkReal evalcond[4];
                                                              IkReal x482 = IKcos(j5);
                                                              IkReal x483 = ((1.0) * (IKsin(j5)));
                                                              evalcond[0] = (x482 + new_r20);
                                                              evalcond[1] = (((-1.0) * x483) + new_r21);
                                                              evalcond[2] = (((-1.0) * x483) + ((-1.0) * new_r00));
                                                              evalcond[3] = (((-1.0) * x482) + ((-1.0) * new_r01));
                                                              if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                  IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH) {
                                                                continue;
                                                              }
                                                            }

                                                            {
                                                              std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                              vinfos[0].jointtype = 1;
                                                              vinfos[0].foffset = j0;
                                                              vinfos[0].indices[0] = _ij0[0];
                                                              vinfos[0].indices[1] = _ij0[1];
                                                              vinfos[0].maxsolutions = _nj0;
                                                              vinfos[1].jointtype = 1;
                                                              vinfos[1].foffset = j1;
                                                              vinfos[1].indices[0] = _ij1[0];
                                                              vinfos[1].indices[1] = _ij1[1];
                                                              vinfos[1].maxsolutions = _nj1;
                                                              vinfos[2].jointtype = 1;
                                                              vinfos[2].foffset = j2;
                                                              vinfos[2].indices[0] = _ij2[0];
                                                              vinfos[2].indices[1] = _ij2[1];
                                                              vinfos[2].maxsolutions = _nj2;
                                                              vinfos[3].jointtype = 1;
                                                              vinfos[3].foffset = j3;
                                                              vinfos[3].indices[0] = _ij3[0];
                                                              vinfos[3].indices[1] = _ij3[1];
                                                              vinfos[3].maxsolutions = _nj3;
                                                              vinfos[4].jointtype = 1;
                                                              vinfos[4].foffset = j4;
                                                              vinfos[4].indices[0] = _ij4[0];
                                                              vinfos[4].indices[1] = _ij4[1];
                                                              vinfos[4].maxsolutions = _nj4;
                                                              vinfos[5].jointtype = 1;
                                                              vinfos[5].foffset = j5;
                                                              vinfos[5].indices[0] = _ij5[0];
                                                              vinfos[5].indices[1] = _ij5[1];
                                                              vinfos[5].maxsolutions = _nj5;
                                                              std::vector<int> vfree(0);
                                                              solutions.AddSolution(vinfos, vfree);
                                                            }
                                                          }
                                                        }
                                                      }
                                                    } while (0);
                                                    if (bgotonextstatement) {
                                                      bool bgotonextstatement = true;
                                                      do {
                                                        evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs((1.5707963267949) + j4))), 6.28318530717959)));
                                                        evalcond[1] = new_r22;
                                                        evalcond[2] = new_r11;
                                                        evalcond[3] = new_r10;
                                                        if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000 &&
                                                            IKabs(evalcond[2]) < 0.0000050000000000 && IKabs(evalcond[3]) < 0.0000050000000000) {
                                                          bgotonextstatement = false;
                                                          {
                                                            IkReal j5array[1], cj5array[1], sj5array[1];
                                                            bool j5valid[1] = {false};
                                                            _nj5 = 1;
                                                            if (IKabs((-1.0) * new_r21) < IKFAST_ATAN2_MAGTHRESH && IKabs(new_r20) < IKFAST_ATAN2_MAGTHRESH &&
                                                                IKabs(IKsqr((-1.0) * new_r21) + IKsqr(new_r20) - 1) <= IKFAST_SINCOS_THRESH) {
                                                              continue;
                                                            }
                                                            j5array[0] = IKatan2(((-1.0) * new_r21), new_r20);
                                                            sj5array[0] = IKsin(j5array[0]);
                                                            cj5array[0] = IKcos(j5array[0]);
                                                            if (j5array[0] > IKPI) {
                                                              j5array[0] -= IK2PI;
                                                            } else if (j5array[0] < -IKPI) {
                                                              j5array[0] += IK2PI;
                                                            }
                                                            j5valid[0] = true;
                                                            for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                              if (!j5valid[ij5]) {
                                                                continue;
                                                              }
                                                              _ij5[0] = ij5;
                                                              _ij5[1] = -1;
                                                              for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                                if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                    IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                  j5valid[iij5] = false;
                                                                  _ij5[1] = iij5;
                                                                  break;
                                                                }
                                                              }
                                                              j5 = j5array[ij5];
                                                              cj5 = cj5array[ij5];
                                                              sj5 = sj5array[ij5];
                                                              {
                                                                IkReal evalcond[4];
                                                                IkReal x484 = IKsin(j5);
                                                                IkReal x485 = ((1.0) * (IKcos(j5)));
                                                                evalcond[0] = (x484 + new_r21);
                                                                evalcond[1] = (((-1.0) * x485) + new_r20);
                                                                evalcond[2] = (((-1.0) * x484) + ((-1.0) * new_r00));
                                                                evalcond[3] = (((-1.0) * x485) + ((-1.0) * new_r01));
                                                                if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                    IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH) {
                                                                  continue;
                                                                }
                                                              }

                                                              {
                                                                std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                                vinfos[0].jointtype = 1;
                                                                vinfos[0].foffset = j0;
                                                                vinfos[0].indices[0] = _ij0[0];
                                                                vinfos[0].indices[1] = _ij0[1];
                                                                vinfos[0].maxsolutions = _nj0;
                                                                vinfos[1].jointtype = 1;
                                                                vinfos[1].foffset = j1;
                                                                vinfos[1].indices[0] = _ij1[0];
                                                                vinfos[1].indices[1] = _ij1[1];
                                                                vinfos[1].maxsolutions = _nj1;
                                                                vinfos[2].jointtype = 1;
                                                                vinfos[2].foffset = j2;
                                                                vinfos[2].indices[0] = _ij2[0];
                                                                vinfos[2].indices[1] = _ij2[1];
                                                                vinfos[2].maxsolutions = _nj2;
                                                                vinfos[3].jointtype = 1;
                                                                vinfos[3].foffset = j3;
                                                                vinfos[3].indices[0] = _ij3[0];
                                                                vinfos[3].indices[1] = _ij3[1];
                                                                vinfos[3].maxsolutions = _nj3;
                                                                vinfos[4].jointtype = 1;
                                                                vinfos[4].foffset = j4;
                                                                vinfos[4].indices[0] = _ij4[0];
                                                                vinfos[4].indices[1] = _ij4[1];
                                                                vinfos[4].maxsolutions = _nj4;
                                                                vinfos[5].jointtype = 1;
                                                                vinfos[5].foffset = j5;
                                                                vinfos[5].indices[0] = _ij5[0];
                                                                vinfos[5].indices[1] = _ij5[1];
                                                                vinfos[5].maxsolutions = _nj5;
                                                                std::vector<int> vfree(0);
                                                                solutions.AddSolution(vinfos, vfree);
                                                              }
                                                            }
                                                          }
                                                        }
                                                      } while (0);
                                                      if (bgotonextstatement) {
                                                        bool bgotonextstatement = true;
                                                        do {
                                                          evalcond[0] = ((IKabs(new_r20)) + (IKabs(new_r21)));
                                                          if (IKabs(evalcond[0]) < 0.0000050000000000) {
                                                            bgotonextstatement = false;
                                                            {
                                                              IkReal j5array[1], cj5array[1], sj5array[1];
                                                              bool j5valid[1] = {false};
                                                              _nj5 = 1;
                                                              if (IKabs((-1.0) * new_r00) < IKFAST_ATAN2_MAGTHRESH && IKabs((-1.0) * new_r01) < IKFAST_ATAN2_MAGTHRESH &&
                                                                  IKabs(IKsqr((-1.0) * new_r00) + IKsqr((-1.0) * new_r01) - 1) <= IKFAST_SINCOS_THRESH) {
                                                                continue;
                                                              }
                                                              j5array[0] = IKatan2(((-1.0) * new_r00), ((-1.0) * new_r01));
                                                              sj5array[0] = IKsin(j5array[0]);
                                                              cj5array[0] = IKcos(j5array[0]);
                                                              if (j5array[0] > IKPI) {
                                                                j5array[0] -= IK2PI;
                                                              } else if (j5array[0] < -IKPI) {
                                                                j5array[0] += IK2PI;
                                                              }
                                                              j5valid[0] = true;
                                                              for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                                if (!j5valid[ij5]) {
                                                                  continue;
                                                                }
                                                                _ij5[0] = ij5;
                                                                _ij5[1] = -1;
                                                                for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                                  if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                      IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                    j5valid[iij5] = false;
                                                                    _ij5[1] = iij5;
                                                                    break;
                                                                  }
                                                                }
                                                                j5 = j5array[ij5];
                                                                cj5 = cj5array[ij5];
                                                                sj5 = sj5array[ij5];
                                                                {
                                                                  IkReal evalcond[6];
                                                                  IkReal x486 = IKsin(j5);
                                                                  IkReal x487 = IKcos(j5);
                                                                  IkReal x488 = ((-1.0) * x487);
                                                                  evalcond[0] = x486;
                                                                  evalcond[1] = (new_r22 * x486);
                                                                  evalcond[2] = x488;
                                                                  evalcond[3] = (new_r22 * x488);
                                                                  evalcond[4] = (((-1.0) * x486) + ((-1.0) * new_r00));
                                                                  evalcond[5] = (((-1.0) * x487) + ((-1.0) * new_r01));
                                                                  if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                      IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                                      IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH) {
                                                                    continue;
                                                                  }
                                                                }

                                                                {
                                                                  std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                                  vinfos[0].jointtype = 1;
                                                                  vinfos[0].foffset = j0;
                                                                  vinfos[0].indices[0] = _ij0[0];
                                                                  vinfos[0].indices[1] = _ij0[1];
                                                                  vinfos[0].maxsolutions = _nj0;
                                                                  vinfos[1].jointtype = 1;
                                                                  vinfos[1].foffset = j1;
                                                                  vinfos[1].indices[0] = _ij1[0];
                                                                  vinfos[1].indices[1] = _ij1[1];
                                                                  vinfos[1].maxsolutions = _nj1;
                                                                  vinfos[2].jointtype = 1;
                                                                  vinfos[2].foffset = j2;
                                                                  vinfos[2].indices[0] = _ij2[0];
                                                                  vinfos[2].indices[1] = _ij2[1];
                                                                  vinfos[2].maxsolutions = _nj2;
                                                                  vinfos[3].jointtype = 1;
                                                                  vinfos[3].foffset = j3;
                                                                  vinfos[3].indices[0] = _ij3[0];
                                                                  vinfos[3].indices[1] = _ij3[1];
                                                                  vinfos[3].maxsolutions = _nj3;
                                                                  vinfos[4].jointtype = 1;
                                                                  vinfos[4].foffset = j4;
                                                                  vinfos[4].indices[0] = _ij4[0];
                                                                  vinfos[4].indices[1] = _ij4[1];
                                                                  vinfos[4].maxsolutions = _nj4;
                                                                  vinfos[5].jointtype = 1;
                                                                  vinfos[5].foffset = j5;
                                                                  vinfos[5].indices[0] = _ij5[0];
                                                                  vinfos[5].indices[1] = _ij5[1];
                                                                  vinfos[5].maxsolutions = _nj5;
                                                                  std::vector<int> vfree(0);
                                                                  solutions.AddSolution(vinfos, vfree);
                                                                }
                                                              }
                                                            }
                                                          }
                                                        } while (0);
                                                        if (bgotonextstatement) {
                                                          bool bgotonextstatement = true;
                                                          do {
                                                            if (1) {
                                                              bgotonextstatement = false;
                                                              continue; // branch
                                                                        // miss
                                                                        // [j5]
                                                            }
                                                          } while (0);
                                                          if (bgotonextstatement) {
                                                          }
                                                        }
                                                      }
                                                    }
                                                  }
                                                }
                                              }

                                            } else {
                                              {
                                                IkReal j5array[1], cj5array[1], sj5array[1];
                                                bool j5valid[1] = {false};
                                                _nj5 = 1;
                                                CheckValue<IkReal> x489 = IKPowWithIntegerCheck(sj4, -1);
                                                if (!x489.valid)
                                                  continue;
                                                if (IKabs((-1.0) * new_r00) < IKFAST_ATAN2_MAGTHRESH && IKabs((-1.0) * new_r20 * (x489.value)) < IKFAST_ATAN2_MAGTHRESH &&
                                                    IKabs(IKsqr((-1.0) * new_r00) + IKsqr((-1.0) * new_r20 * (x489.value)) - 1) <= IKFAST_SINCOS_THRESH) {
                                                  continue;
                                                }
                                                j5array[0] = IKatan2(((-1.0) * new_r00), ((-1.0) * new_r20 * (x489.value)));
                                                sj5array[0] = IKsin(j5array[0]);
                                                cj5array[0] = IKcos(j5array[0]);
                                                if (j5array[0] > IKPI)
                                                  j5array[0] -= IK2PI;
                                                else if (j5array[0] < -IKPI)
                                                  j5array[0] += IK2PI;
                                                j5valid[0] = true;
                                                for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                  if (!j5valid[ij5])
                                                    continue;
                                                  _ij5[0] = ij5;
                                                  _ij5[1] = -1;
                                                  for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                    if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                        IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                      j5valid[iij5] = false;
                                                      _ij5[1] = iij5;
                                                      break;
                                                    }
                                                  }
                                                  j5 = j5array[ij5];
                                                  cj5 = cj5array[ij5];
                                                  sj5 = sj5array[ij5];
                                                  {
                                                    IkReal evalcond[8];
                                                    IkReal x490 = IKsin(j5);
                                                    IkReal x491 = IKcos(j5);
                                                    IkReal x492 = ((1.0) * sj4);
                                                    IkReal x493 = ((1.0) * x491);
                                                    IkReal x494 = ((1.0) * x490);
                                                    evalcond[0] = ((sj4 * x491) + new_r20);
                                                    evalcond[1] = ((cj4 * x490) + new_r11);
                                                    evalcond[2] = (((-1.0) * x490 * x492) + new_r21);
                                                    evalcond[3] = (((-1.0) * cj4 * x493) + new_r10);
                                                    evalcond[4] = (((-1.0) * x494) + ((-1.0) * new_r00));
                                                    evalcond[5] = (((-1.0) * x493) + ((-1.0) * new_r01));
                                                    evalcond[6] = ((cj4 * new_r11) + ((-1.0) * new_r21 * x492) + x490);
                                                    evalcond[7] = (((-1.0) * new_r20 * x492) + ((-1.0) * x493) + (cj4 * new_r10));
                                                    if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                        IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                        IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                        IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                                      continue;
                                                    }
                                                  }

                                                  {
                                                    std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                    vinfos[0].jointtype = 1;
                                                    vinfos[0].foffset = j0;
                                                    vinfos[0].indices[0] = _ij0[0];
                                                    vinfos[0].indices[1] = _ij0[1];
                                                    vinfos[0].maxsolutions = _nj0;
                                                    vinfos[1].jointtype = 1;
                                                    vinfos[1].foffset = j1;
                                                    vinfos[1].indices[0] = _ij1[0];
                                                    vinfos[1].indices[1] = _ij1[1];
                                                    vinfos[1].maxsolutions = _nj1;
                                                    vinfos[2].jointtype = 1;
                                                    vinfos[2].foffset = j2;
                                                    vinfos[2].indices[0] = _ij2[0];
                                                    vinfos[2].indices[1] = _ij2[1];
                                                    vinfos[2].maxsolutions = _nj2;
                                                    vinfos[3].jointtype = 1;
                                                    vinfos[3].foffset = j3;
                                                    vinfos[3].indices[0] = _ij3[0];
                                                    vinfos[3].indices[1] = _ij3[1];
                                                    vinfos[3].maxsolutions = _nj3;
                                                    vinfos[4].jointtype = 1;
                                                    vinfos[4].foffset = j4;
                                                    vinfos[4].indices[0] = _ij4[0];
                                                    vinfos[4].indices[1] = _ij4[1];
                                                    vinfos[4].maxsolutions = _nj4;
                                                    vinfos[5].jointtype = 1;
                                                    vinfos[5].foffset = j5;
                                                    vinfos[5].indices[0] = _ij5[0];
                                                    vinfos[5].indices[1] = _ij5[1];
                                                    vinfos[5].maxsolutions = _nj5;
                                                    std::vector<int> vfree(0);
                                                    solutions.AddSolution(vinfos, vfree);
                                                  }
                                                }
                                              }
                                            }
                                          }

                                        } else {
                                          {
                                            IkReal j5array[1], cj5array[1], sj5array[1];
                                            bool j5valid[1] = {false};
                                            _nj5 = 1;
                                            CheckValue<IkReal> x495 = IKPowWithIntegerCheck(IKsign(cj4), -1);
                                            if (!x495.valid)
                                              continue;
                                            CheckValue<IkReal> x496 = IKatan2WithCheck(IkReal((-1.0) * new_r11), IkReal(new_r10), IKFAST_ATAN2_MAGTHRESH);
                                            if (!x496.valid)
                                              continue;
                                            j5array[0] = ((-1.5707963267949) + ((1.5707963267949) * (x495.value)) + (x496.value));
                                            sj5array[0] = IKsin(j5array[0]);
                                            cj5array[0] = IKcos(j5array[0]);
                                            if (j5array[0] > IKPI)
                                              j5array[0] -= IK2PI;
                                            else if (j5array[0] < -IKPI)
                                              j5array[0] += IK2PI;
                                            j5valid[0] = true;
                                            for (int ij5 = 0; ij5 < 1; ++ij5) {
                                              if (!j5valid[ij5])
                                                continue;
                                              _ij5[0] = ij5;
                                              _ij5[1] = -1;
                                              for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                    IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                  j5valid[iij5] = false;
                                                  _ij5[1] = iij5;
                                                  break;
                                                }
                                              }
                                              j5 = j5array[ij5];
                                              cj5 = cj5array[ij5];
                                              sj5 = sj5array[ij5];
                                              {
                                                IkReal evalcond[8];
                                                IkReal x497 = IKsin(j5);
                                                IkReal x498 = IKcos(j5);
                                                IkReal x499 = ((1.0) * sj4);
                                                IkReal x500 = ((1.0) * x498);
                                                IkReal x501 = ((1.0) * x497);
                                                evalcond[0] = ((sj4 * x498) + new_r20);
                                                evalcond[1] = ((cj4 * x497) + new_r11);
                                                evalcond[2] = (((-1.0) * x497 * x499) + new_r21);
                                                evalcond[3] = (((-1.0) * cj4 * x500) + new_r10);
                                                evalcond[4] = (((-1.0) * x501) + ((-1.0) * new_r00));
                                                evalcond[5] = (((-1.0) * x500) + ((-1.0) * new_r01));
                                                evalcond[6] = ((cj4 * new_r11) + ((-1.0) * new_r21 * x499) + x497);
                                                evalcond[7] = (((-1.0) * new_r20 * x499) + (cj4 * new_r10) + ((-1.0) * x500));
                                                if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                    IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                    IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                    IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                                  continue;
                                                }
                                              }

                                              {
                                                std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                vinfos[0].jointtype = 1;
                                                vinfos[0].foffset = j0;
                                                vinfos[0].indices[0] = _ij0[0];
                                                vinfos[0].indices[1] = _ij0[1];
                                                vinfos[0].maxsolutions = _nj0;
                                                vinfos[1].jointtype = 1;
                                                vinfos[1].foffset = j1;
                                                vinfos[1].indices[0] = _ij1[0];
                                                vinfos[1].indices[1] = _ij1[1];
                                                vinfos[1].maxsolutions = _nj1;
                                                vinfos[2].jointtype = 1;
                                                vinfos[2].foffset = j2;
                                                vinfos[2].indices[0] = _ij2[0];
                                                vinfos[2].indices[1] = _ij2[1];
                                                vinfos[2].maxsolutions = _nj2;
                                                vinfos[3].jointtype = 1;
                                                vinfos[3].foffset = j3;
                                                vinfos[3].indices[0] = _ij3[0];
                                                vinfos[3].indices[1] = _ij3[1];
                                                vinfos[3].maxsolutions = _nj3;
                                                vinfos[4].jointtype = 1;
                                                vinfos[4].foffset = j4;
                                                vinfos[4].indices[0] = _ij4[0];
                                                vinfos[4].indices[1] = _ij4[1];
                                                vinfos[4].maxsolutions = _nj4;
                                                vinfos[5].jointtype = 1;
                                                vinfos[5].foffset = j5;
                                                vinfos[5].indices[0] = _ij5[0];
                                                vinfos[5].indices[1] = _ij5[1];
                                                vinfos[5].maxsolutions = _nj5;
                                                std::vector<int> vfree(0);
                                                solutions.AddSolution(vinfos, vfree);
                                              }
                                            }
                                          }
                                        }
                                      }

                                    } else {
                                      {
                                        IkReal j5array[1], cj5array[1], sj5array[1];
                                        bool j5valid[1] = {false};
                                        _nj5 = 1;
                                        CheckValue<IkReal> x502 = IKPowWithIntegerCheck(IKsign(sj4), -1);
                                        if (!x502.valid)
                                          continue;
                                        CheckValue<IkReal> x503 = IKatan2WithCheck(IkReal(new_r21), IkReal((-1.0) * new_r20), IKFAST_ATAN2_MAGTHRESH);
                                        if (!x503.valid)
                                          continue;
                                        j5array[0] = ((-1.5707963267949) + ((1.5707963267949) * (x502.value)) + (x503.value));
                                        sj5array[0] = IKsin(j5array[0]);
                                        cj5array[0] = IKcos(j5array[0]);
                                        if (j5array[0] > IKPI)
                                          j5array[0] -= IK2PI;
                                        else if (j5array[0] < -IKPI)
                                          j5array[0] += IK2PI;
                                        j5valid[0] = true;
                                        for (int ij5 = 0; ij5 < 1; ++ij5) {
                                          if (!j5valid[ij5])
                                            continue;
                                          _ij5[0] = ij5;
                                          _ij5[1] = -1;
                                          for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                            if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                              j5valid[iij5] = false;
                                              _ij5[1] = iij5;
                                              break;
                                            }
                                          }
                                          j5 = j5array[ij5];
                                          cj5 = cj5array[ij5];
                                          sj5 = sj5array[ij5];
                                          {
                                            IkReal evalcond[8];
                                            IkReal x504 = IKsin(j5);
                                            IkReal x505 = IKcos(j5);
                                            IkReal x506 = ((1.0) * sj4);
                                            IkReal x507 = ((1.0) * x505);
                                            IkReal x508 = ((1.0) * x504);
                                            evalcond[0] = ((sj4 * x505) + new_r20);
                                            evalcond[1] = ((cj4 * x504) + new_r11);
                                            evalcond[2] = (((-1.0) * x504 * x506) + new_r21);
                                            evalcond[3] = (((-1.0) * cj4 * x507) + new_r10);
                                            evalcond[4] = (((-1.0) * x508) + ((-1.0) * new_r00));
                                            evalcond[5] = (((-1.0) * x507) + ((-1.0) * new_r01));
                                            evalcond[6] = (((-1.0) * new_r21 * x506) + (cj4 * new_r11) + x504);
                                            evalcond[7] = ((cj4 * new_r10) + ((-1.0) * new_r20 * x506) + ((-1.0) * x507));
                                            if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                              continue;
                                            }
                                          }

                                          {
                                            std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                            vinfos[0].jointtype = 1;
                                            vinfos[0].foffset = j0;
                                            vinfos[0].indices[0] = _ij0[0];
                                            vinfos[0].indices[1] = _ij0[1];
                                            vinfos[0].maxsolutions = _nj0;
                                            vinfos[1].jointtype = 1;
                                            vinfos[1].foffset = j1;
                                            vinfos[1].indices[0] = _ij1[0];
                                            vinfos[1].indices[1] = _ij1[1];
                                            vinfos[1].maxsolutions = _nj1;
                                            vinfos[2].jointtype = 1;
                                            vinfos[2].foffset = j2;
                                            vinfos[2].indices[0] = _ij2[0];
                                            vinfos[2].indices[1] = _ij2[1];
                                            vinfos[2].maxsolutions = _nj2;
                                            vinfos[3].jointtype = 1;
                                            vinfos[3].foffset = j3;
                                            vinfos[3].indices[0] = _ij3[0];
                                            vinfos[3].indices[1] = _ij3[1];
                                            vinfos[3].maxsolutions = _nj3;
                                            vinfos[4].jointtype = 1;
                                            vinfos[4].foffset = j4;
                                            vinfos[4].indices[0] = _ij4[0];
                                            vinfos[4].indices[1] = _ij4[1];
                                            vinfos[4].maxsolutions = _nj4;
                                            vinfos[5].jointtype = 1;
                                            vinfos[5].foffset = j5;
                                            vinfos[5].indices[0] = _ij5[0];
                                            vinfos[5].indices[1] = _ij5[1];
                                            vinfos[5].maxsolutions = _nj5;
                                            std::vector<int> vfree(0);
                                            solutions.AddSolution(vinfos, vfree);
                                          }
                                        }
                                      }
                                    }
                                  }
                                }
                              } while (0);
                              if (bgotonextstatement) {
                                bool bgotonextstatement = true;
                                do {
                                  evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs((1.5707963267949) + j3))), 6.28318530717959)));
                                  evalcond[1] = new_r02;
                                  if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000) {
                                    bgotonextstatement = false;
                                    {
                                      IkReal j5array[1], cj5array[1], sj5array[1];
                                      bool j5valid[1] = {false};
                                      _nj5 = 1;
                                      if (IKabs(new_r00) < IKFAST_ATAN2_MAGTHRESH && IKabs(new_r01) < IKFAST_ATAN2_MAGTHRESH &&
                                          IKabs(IKsqr(new_r00) + IKsqr(new_r01) - 1) <= IKFAST_SINCOS_THRESH) {
                                        continue;
                                      }
                                      j5array[0] = IKatan2(new_r00, new_r01);
                                      sj5array[0] = IKsin(j5array[0]);
                                      cj5array[0] = IKcos(j5array[0]);
                                      if (j5array[0] > IKPI)
                                        j5array[0] -= IK2PI;
                                      else if (j5array[0] < -IKPI)
                                        j5array[0] += IK2PI;
                                      j5valid[0] = true;
                                      for (int ij5 = 0; ij5 < 1; ++ij5) {
                                        if (!j5valid[ij5])
                                          continue;
                                        _ij5[0] = ij5;
                                        _ij5[1] = -1;
                                        for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                          if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                              IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                            j5valid[iij5] = false;
                                            _ij5[1] = iij5;
                                            break;
                                          }
                                        }
                                        j5 = j5array[ij5];
                                        cj5 = cj5array[ij5];
                                        sj5 = sj5array[ij5];
                                        {
                                          IkReal evalcond[8];
                                          IkReal x509 = IKcos(j5);
                                          IkReal x510 = IKsin(j5);
                                          IkReal x511 = ((1.0) * cj4);
                                          IkReal x512 = ((1.0) * sj4);
                                          IkReal x513 = ((1.0) * x509);
                                          IkReal x514 = ((1.0) * x510);
                                          evalcond[0] = ((sj4 * x509) + new_r20);
                                          evalcond[1] = (new_r00 + ((-1.0) * x514));
                                          evalcond[2] = (new_r01 + ((-1.0) * x513));
                                          evalcond[3] = (new_r21 + ((-1.0) * x510 * x512));
                                          evalcond[4] = ((cj4 * x510) + ((-1.0) * new_r11));
                                          evalcond[5] = (((-1.0) * new_r10) + ((-1.0) * x509 * x511));
                                          evalcond[6] = (((-1.0) * new_r11 * x511) + x510 + ((-1.0) * new_r21 * x512));
                                          evalcond[7] = (((-1.0) * new_r10 * x511) + ((-1.0) * new_r20 * x512) + ((-1.0) * x513));
                                          if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                              IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                              IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                              IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                            continue;
                                          }
                                        }

                                        {
                                          std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                          vinfos[0].jointtype = 1;
                                          vinfos[0].foffset = j0;
                                          vinfos[0].indices[0] = _ij0[0];
                                          vinfos[0].indices[1] = _ij0[1];
                                          vinfos[0].maxsolutions = _nj0;
                                          vinfos[1].jointtype = 1;
                                          vinfos[1].foffset = j1;
                                          vinfos[1].indices[0] = _ij1[0];
                                          vinfos[1].indices[1] = _ij1[1];
                                          vinfos[1].maxsolutions = _nj1;
                                          vinfos[2].jointtype = 1;
                                          vinfos[2].foffset = j2;
                                          vinfos[2].indices[0] = _ij2[0];
                                          vinfos[2].indices[1] = _ij2[1];
                                          vinfos[2].maxsolutions = _nj2;
                                          vinfos[3].jointtype = 1;
                                          vinfos[3].foffset = j3;
                                          vinfos[3].indices[0] = _ij3[0];
                                          vinfos[3].indices[1] = _ij3[1];
                                          vinfos[3].maxsolutions = _nj3;
                                          vinfos[4].jointtype = 1;
                                          vinfos[4].foffset = j4;
                                          vinfos[4].indices[0] = _ij4[0];
                                          vinfos[4].indices[1] = _ij4[1];
                                          vinfos[4].maxsolutions = _nj4;
                                          vinfos[5].jointtype = 1;
                                          vinfos[5].foffset = j5;
                                          vinfos[5].indices[0] = _ij5[0];
                                          vinfos[5].indices[1] = _ij5[1];
                                          vinfos[5].maxsolutions = _nj5;
                                          std::vector<int> vfree(0);
                                          solutions.AddSolution(vinfos, vfree);
                                        }
                                      }
                                    }
                                  }
                                } while (0);
                                if (bgotonextstatement) {
                                  bool bgotonextstatement = true;
                                  do {
                                    evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs((-1.5707963267949) + j4))), 6.28318530717959)));
                                    evalcond[1] = new_r22;
                                    if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000) {
                                      bgotonextstatement = false;
                                      {
                                        IkReal j5array[1], cj5array[1], sj5array[1];
                                        bool j5valid[1] = {false};
                                        _nj5 = 1;
                                        if (IKabs(new_r21) < IKFAST_ATAN2_MAGTHRESH && IKabs((-1.0) * new_r20) < IKFAST_ATAN2_MAGTHRESH &&
                                            IKabs(IKsqr(new_r21) + IKsqr((-1.0) * new_r20) - 1) <= IKFAST_SINCOS_THRESH) {
                                          continue;
                                        }
                                        j5array[0] = IKatan2(new_r21, ((-1.0) * new_r20));
                                        sj5array[0] = IKsin(j5array[0]);
                                        cj5array[0] = IKcos(j5array[0]);
                                        if (j5array[0] > IKPI)
                                          j5array[0] -= IK2PI;
                                        else if (j5array[0] < -IKPI)
                                          j5array[0] += IK2PI;
                                        j5valid[0] = true;
                                        for (int ij5 = 0; ij5 < 1; ++ij5) {
                                          if (!j5valid[ij5])
                                            continue;
                                          _ij5[0] = ij5;
                                          _ij5[1] = -1;
                                          for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                            if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                              j5valid[iij5] = false;
                                              _ij5[1] = iij5;
                                              break;
                                            }
                                          }
                                          j5 = j5array[ij5];
                                          cj5 = cj5array[ij5];
                                          sj5 = sj5array[ij5];
                                          {
                                            IkReal evalcond[8];
                                            IkReal x515 = IKcos(j5);
                                            IkReal x516 = IKsin(j5);
                                            IkReal x517 = ((1.0) * sj3);
                                            IkReal x518 = ((1.0) * x516);
                                            IkReal x519 = ((1.0) * x515);
                                            evalcond[0] = (x515 + new_r20);
                                            evalcond[1] = (new_r21 + ((-1.0) * x518));
                                            evalcond[2] = ((sj3 * x515) + new_r01);
                                            evalcond[3] = ((sj3 * x516) + new_r00);
                                            evalcond[4] = (((-1.0) * cj3 * x519) + new_r11);
                                            evalcond[5] = (((-1.0) * new_r02 * x518) + new_r10);
                                            evalcond[6] = ((cj3 * new_r10) + ((-1.0) * new_r00 * x517) + ((-1.0) * x518));
                                            evalcond[7] = (((-1.0) * new_r01 * x517) + (cj3 * new_r11) + ((-1.0) * x519));
                                            if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                              continue;
                                            }
                                          }

                                          {
                                            std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                            vinfos[0].jointtype = 1;
                                            vinfos[0].foffset = j0;
                                            vinfos[0].indices[0] = _ij0[0];
                                            vinfos[0].indices[1] = _ij0[1];
                                            vinfos[0].maxsolutions = _nj0;
                                            vinfos[1].jointtype = 1;
                                            vinfos[1].foffset = j1;
                                            vinfos[1].indices[0] = _ij1[0];
                                            vinfos[1].indices[1] = _ij1[1];
                                            vinfos[1].maxsolutions = _nj1;
                                            vinfos[2].jointtype = 1;
                                            vinfos[2].foffset = j2;
                                            vinfos[2].indices[0] = _ij2[0];
                                            vinfos[2].indices[1] = _ij2[1];
                                            vinfos[2].maxsolutions = _nj2;
                                            vinfos[3].jointtype = 1;
                                            vinfos[3].foffset = j3;
                                            vinfos[3].indices[0] = _ij3[0];
                                            vinfos[3].indices[1] = _ij3[1];
                                            vinfos[3].maxsolutions = _nj3;
                                            vinfos[4].jointtype = 1;
                                            vinfos[4].foffset = j4;
                                            vinfos[4].indices[0] = _ij4[0];
                                            vinfos[4].indices[1] = _ij4[1];
                                            vinfos[4].maxsolutions = _nj4;
                                            vinfos[5].jointtype = 1;
                                            vinfos[5].foffset = j5;
                                            vinfos[5].indices[0] = _ij5[0];
                                            vinfos[5].indices[1] = _ij5[1];
                                            vinfos[5].maxsolutions = _nj5;
                                            std::vector<int> vfree(0);
                                            solutions.AddSolution(vinfos, vfree);
                                          }
                                        }
                                      }
                                    }
                                  } while (0);
                                  if (bgotonextstatement) {
                                    bool bgotonextstatement = true;
                                    do {
                                      evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs((1.5707963267949) + j4))), 6.28318530717959)));
                                      evalcond[1] = new_r22;
                                      if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000) {
                                        bgotonextstatement = false;
                                        {
                                          IkReal j5array[1], cj5array[1], sj5array[1];
                                          bool j5valid[1] = {false};
                                          _nj5 = 1;
                                          if (IKabs((-1.0) * new_r21) < IKFAST_ATAN2_MAGTHRESH && IKabs(new_r20) < IKFAST_ATAN2_MAGTHRESH &&
                                              IKabs(IKsqr((-1.0) * new_r21) + IKsqr(new_r20) - 1) <= IKFAST_SINCOS_THRESH) {
                                            continue;
                                          }
                                          j5array[0] = IKatan2(((-1.0) * new_r21), new_r20);
                                          sj5array[0] = IKsin(j5array[0]);
                                          cj5array[0] = IKcos(j5array[0]);
                                          if (j5array[0] > IKPI)
                                            j5array[0] -= IK2PI;
                                          else if (j5array[0] < -IKPI)
                                            j5array[0] += IK2PI;
                                          j5valid[0] = true;
                                          for (int ij5 = 0; ij5 < 1; ++ij5) {
                                            if (!j5valid[ij5])
                                              continue;
                                            _ij5[0] = ij5;
                                            _ij5[1] = -1;
                                            for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                              if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                  IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                j5valid[iij5] = false;
                                                _ij5[1] = iij5;
                                                break;
                                              }
                                            }
                                            j5 = j5array[ij5];
                                            cj5 = cj5array[ij5];
                                            sj5 = sj5array[ij5];
                                            {
                                              IkReal evalcond[8];
                                              IkReal x520 = IKcos(j5);
                                              IkReal x521 = IKsin(j5);
                                              IkReal x522 = ((1.0) * sj3);
                                              IkReal x523 = ((1.0) * x520);
                                              evalcond[0] = (x521 + new_r21);
                                              evalcond[1] = (((-1.0) * x523) + new_r20);
                                              evalcond[2] = ((sj3 * x520) + new_r01);
                                              evalcond[3] = ((sj3 * x521) + new_r00);
                                              evalcond[4] = ((new_r02 * x521) + new_r10);
                                              evalcond[5] = (((-1.0) * cj3 * x523) + new_r11);
                                              evalcond[6] = (((-1.0) * x521) + ((-1.0) * new_r00 * x522) + (cj3 * new_r10));
                                              evalcond[7] = (((-1.0) * x523) + ((-1.0) * new_r01 * x522) + (cj3 * new_r11));
                                              if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                  IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                  IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                  IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                                continue;
                                              }
                                            }

                                            {
                                              std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                              vinfos[0].jointtype = 1;
                                              vinfos[0].foffset = j0;
                                              vinfos[0].indices[0] = _ij0[0];
                                              vinfos[0].indices[1] = _ij0[1];
                                              vinfos[0].maxsolutions = _nj0;
                                              vinfos[1].jointtype = 1;
                                              vinfos[1].foffset = j1;
                                              vinfos[1].indices[0] = _ij1[0];
                                              vinfos[1].indices[1] = _ij1[1];
                                              vinfos[1].maxsolutions = _nj1;
                                              vinfos[2].jointtype = 1;
                                              vinfos[2].foffset = j2;
                                              vinfos[2].indices[0] = _ij2[0];
                                              vinfos[2].indices[1] = _ij2[1];
                                              vinfos[2].maxsolutions = _nj2;
                                              vinfos[3].jointtype = 1;
                                              vinfos[3].foffset = j3;
                                              vinfos[3].indices[0] = _ij3[0];
                                              vinfos[3].indices[1] = _ij3[1];
                                              vinfos[3].maxsolutions = _nj3;
                                              vinfos[4].jointtype = 1;
                                              vinfos[4].foffset = j4;
                                              vinfos[4].indices[0] = _ij4[0];
                                              vinfos[4].indices[1] = _ij4[1];
                                              vinfos[4].maxsolutions = _nj4;
                                              vinfos[5].jointtype = 1;
                                              vinfos[5].foffset = j5;
                                              vinfos[5].indices[0] = _ij5[0];
                                              vinfos[5].indices[1] = _ij5[1];
                                              vinfos[5].maxsolutions = _nj5;
                                              std::vector<int> vfree(0);
                                              solutions.AddSolution(vinfos, vfree);
                                            }
                                          }
                                        }
                                      }
                                    } while (0);
                                    if (bgotonextstatement) {
                                      bool bgotonextstatement = true;
                                      do {
                                        evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs(j4))), 6.28318530717959)));
                                        evalcond[1] = new_r20;
                                        evalcond[2] = new_r02;
                                        evalcond[3] = new_r12;
                                        evalcond[4] = new_r21;
                                        if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000 && IKabs(evalcond[2]) < 0.0000050000000000 &&
                                            IKabs(evalcond[3]) < 0.0000050000000000 && IKabs(evalcond[4]) < 0.0000050000000000) {
                                          bgotonextstatement = false;
                                          {
                                            IkReal j5array[1], cj5array[1], sj5array[1];
                                            bool j5valid[1] = {false};
                                            _nj5 = 1;
                                            IkReal x524 = ((1.0) * new_r01);
                                            if (IKabs(((-1.0) * cj3 * x524) + ((-1.0) * new_r00 * sj3)) < IKFAST_ATAN2_MAGTHRESH &&
                                                IKabs(((-1.0) * sj3 * x524) + (cj3 * new_r00)) < IKFAST_ATAN2_MAGTHRESH &&
                                                IKabs(IKsqr(((-1.0) * cj3 * x524) + ((-1.0) * new_r00 * sj3)) + IKsqr(((-1.0) * sj3 * x524) + (cj3 * new_r00)) - 1) <=
                                                  IKFAST_SINCOS_THRESH) {
                                              continue;
                                            }
                                            j5array[0] = IKatan2((((-1.0) * cj3 * x524) + ((-1.0) * new_r00 * sj3)), (((-1.0) * sj3 * x524) + (cj3 * new_r00)));
                                            sj5array[0] = IKsin(j5array[0]);
                                            cj5array[0] = IKcos(j5array[0]);
                                            if (j5array[0] > IKPI)
                                              j5array[0] -= IK2PI;
                                            else if (j5array[0] < -IKPI)
                                              j5array[0] += IK2PI;
                                            j5valid[0] = true;
                                            for (int ij5 = 0; ij5 < 1; ++ij5) {
                                              if (!j5valid[ij5])
                                                continue;
                                              _ij5[0] = ij5;
                                              _ij5[1] = -1;
                                              for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                    IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                  j5valid[iij5] = false;
                                                  _ij5[1] = iij5;
                                                  break;
                                                }
                                              }
                                              j5 = j5array[ij5];
                                              cj5 = cj5array[ij5];
                                              sj5 = sj5array[ij5];
                                              {
                                                IkReal evalcond[8];
                                                IkReal x525 = IKsin(j5);
                                                IkReal x526 = IKcos(j5);
                                                IkReal x527 = ((1.0) * sj3);
                                                IkReal x528 = ((1.0) * x526);
                                                IkReal x529 = (sj3 * x525);
                                                IkReal x530 = ((1.0) * x525);
                                                IkReal x531 = (cj3 * x528);
                                                evalcond[0] = ((new_r11 * sj3) + x525 + (cj3 * new_r01));
                                                evalcond[1] = ((sj3 * x526) + new_r01 + (cj3 * x525));
                                                evalcond[2] = ((new_r10 * sj3) + ((-1.0) * x528) + (cj3 * new_r00));
                                                evalcond[3] = (((-1.0) * new_r00 * x527) + ((-1.0) * x530) + (cj3 * new_r10));
                                                evalcond[4] = (((-1.0) * x528) + ((-1.0) * new_r01 * x527) + (cj3 * new_r11));
                                                evalcond[5] = (((-1.0) * x531) + x529 + new_r00);
                                                evalcond[6] = (((-1.0) * x531) + x529 + new_r11);
                                                evalcond[7] = (((-1.0) * x526 * x527) + ((-1.0) * cj3 * x530) + new_r10);
                                                if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                    IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                    IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                    IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                                  continue;
                                                }
                                              }

                                              {
                                                std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                vinfos[0].jointtype = 1;
                                                vinfos[0].foffset = j0;
                                                vinfos[0].indices[0] = _ij0[0];
                                                vinfos[0].indices[1] = _ij0[1];
                                                vinfos[0].maxsolutions = _nj0;
                                                vinfos[1].jointtype = 1;
                                                vinfos[1].foffset = j1;
                                                vinfos[1].indices[0] = _ij1[0];
                                                vinfos[1].indices[1] = _ij1[1];
                                                vinfos[1].maxsolutions = _nj1;
                                                vinfos[2].jointtype = 1;
                                                vinfos[2].foffset = j2;
                                                vinfos[2].indices[0] = _ij2[0];
                                                vinfos[2].indices[1] = _ij2[1];
                                                vinfos[2].maxsolutions = _nj2;
                                                vinfos[3].jointtype = 1;
                                                vinfos[3].foffset = j3;
                                                vinfos[3].indices[0] = _ij3[0];
                                                vinfos[3].indices[1] = _ij3[1];
                                                vinfos[3].maxsolutions = _nj3;
                                                vinfos[4].jointtype = 1;
                                                vinfos[4].foffset = j4;
                                                vinfos[4].indices[0] = _ij4[0];
                                                vinfos[4].indices[1] = _ij4[1];
                                                vinfos[4].maxsolutions = _nj4;
                                                vinfos[5].jointtype = 1;
                                                vinfos[5].foffset = j5;
                                                vinfos[5].indices[0] = _ij5[0];
                                                vinfos[5].indices[1] = _ij5[1];
                                                vinfos[5].maxsolutions = _nj5;
                                                std::vector<int> vfree(0);
                                                solutions.AddSolution(vinfos, vfree);
                                              }
                                            }
                                          }
                                        }
                                      } while (0);
                                      if (bgotonextstatement) {
                                        bool bgotonextstatement = true;
                                        do {
                                          evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs((-3.14159265358979) + j4))), 6.28318530717959)));
                                          evalcond[1] = new_r20;
                                          evalcond[2] = new_r02;
                                          evalcond[3] = new_r12;
                                          evalcond[4] = new_r21;
                                          if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000 && IKabs(evalcond[2]) < 0.0000050000000000 &&
                                              IKabs(evalcond[3]) < 0.0000050000000000 && IKabs(evalcond[4]) < 0.0000050000000000) {
                                            bgotonextstatement = false;
                                            {
                                              IkReal j5array[1], cj5array[1], sj5array[1];
                                              bool j5valid[1] = {false};
                                              _nj5 = 1;
                                              IkReal x532 = ((1.0) * sj3);
                                              if (IKabs(((-1.0) * new_r00 * x532) + (cj3 * new_r01)) < IKFAST_ATAN2_MAGTHRESH &&
                                                  IKabs(((-1.0) * cj3 * new_r00) + ((-1.0) * new_r01 * x532)) < IKFAST_ATAN2_MAGTHRESH &&
                                                  IKabs(IKsqr(((-1.0) * new_r00 * x532) + (cj3 * new_r01)) + IKsqr(((-1.0) * cj3 * new_r00) + ((-1.0) * new_r01 * x532)) - 1) <=
                                                    IKFAST_SINCOS_THRESH) {
                                                continue;
                                              }
                                              j5array[0] = IKatan2((((-1.0) * new_r00 * x532) + (cj3 * new_r01)), (((-1.0) * cj3 * new_r00) + ((-1.0) * new_r01 * x532)));
                                              sj5array[0] = IKsin(j5array[0]);
                                              cj5array[0] = IKcos(j5array[0]);
                                              if (j5array[0] > IKPI)
                                                j5array[0] -= IK2PI;
                                              else if (j5array[0] < -IKPI)
                                                j5array[0] += IK2PI;
                                              j5valid[0] = true;
                                              for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                if (!j5valid[ij5])
                                                  continue;
                                                _ij5[0] = ij5;
                                                _ij5[1] = -1;
                                                for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                  if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                      IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                    j5valid[iij5] = false;
                                                    _ij5[1] = iij5;
                                                    break;
                                                  }
                                                }
                                                j5 = j5array[ij5];
                                                cj5 = cj5array[ij5];
                                                sj5 = sj5array[ij5];
                                                {
                                                  IkReal evalcond[8];
                                                  IkReal x533 = IKsin(j5);
                                                  IkReal x534 = IKcos(j5);
                                                  IkReal x535 = ((1.0) * sj3);
                                                  IkReal x536 = ((1.0) * x533);
                                                  IkReal x537 = (sj3 * x534);
                                                  IkReal x538 = ((1.0) * x534);
                                                  IkReal x539 = (cj3 * x536);
                                                  evalcond[0] = ((new_r10 * sj3) + x534 + (cj3 * new_r00));
                                                  evalcond[1] = ((new_r11 * sj3) + ((-1.0) * x536) + (cj3 * new_r01));
                                                  evalcond[2] = ((sj3 * x533) + new_r00 + (cj3 * x534));
                                                  evalcond[3] = (((-1.0) * new_r00 * x535) + ((-1.0) * x536) + (cj3 * new_r10));
                                                  evalcond[4] = (((-1.0) * x538) + ((-1.0) * new_r01 * x535) + (cj3 * new_r11));
                                                  evalcond[5] = (((-1.0) * x539) + x537 + new_r01);
                                                  evalcond[6] = (((-1.0) * x539) + x537 + new_r10);
                                                  evalcond[7] = (((-1.0) * x533 * x535) + ((-1.0) * cj3 * x538) + new_r11);
                                                  if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                      IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                      IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                      IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                                    continue;
                                                  }
                                                }

                                                {
                                                  std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                  vinfos[0].jointtype = 1;
                                                  vinfos[0].foffset = j0;
                                                  vinfos[0].indices[0] = _ij0[0];
                                                  vinfos[0].indices[1] = _ij0[1];
                                                  vinfos[0].maxsolutions = _nj0;
                                                  vinfos[1].jointtype = 1;
                                                  vinfos[1].foffset = j1;
                                                  vinfos[1].indices[0] = _ij1[0];
                                                  vinfos[1].indices[1] = _ij1[1];
                                                  vinfos[1].maxsolutions = _nj1;
                                                  vinfos[2].jointtype = 1;
                                                  vinfos[2].foffset = j2;
                                                  vinfos[2].indices[0] = _ij2[0];
                                                  vinfos[2].indices[1] = _ij2[1];
                                                  vinfos[2].maxsolutions = _nj2;
                                                  vinfos[3].jointtype = 1;
                                                  vinfos[3].foffset = j3;
                                                  vinfos[3].indices[0] = _ij3[0];
                                                  vinfos[3].indices[1] = _ij3[1];
                                                  vinfos[3].maxsolutions = _nj3;
                                                  vinfos[4].jointtype = 1;
                                                  vinfos[4].foffset = j4;
                                                  vinfos[4].indices[0] = _ij4[0];
                                                  vinfos[4].indices[1] = _ij4[1];
                                                  vinfos[4].maxsolutions = _nj4;
                                                  vinfos[5].jointtype = 1;
                                                  vinfos[5].foffset = j5;
                                                  vinfos[5].indices[0] = _ij5[0];
                                                  vinfos[5].indices[1] = _ij5[1];
                                                  vinfos[5].maxsolutions = _nj5;
                                                  std::vector<int> vfree(0);
                                                  solutions.AddSolution(vinfos, vfree);
                                                }
                                              }
                                            }
                                          }
                                        } while (0);
                                        if (bgotonextstatement) {
                                          bool bgotonextstatement = true;
                                          do {
                                            evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs(j3))), 6.28318530717959)));
                                            evalcond[1] = new_r12;
                                            if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000) {
                                              bgotonextstatement = false;
                                              {
                                                IkReal j5array[1], cj5array[1], sj5array[1];
                                                bool j5valid[1] = {false};
                                                _nj5 = 1;
                                                if (IKabs(new_r10) < IKFAST_ATAN2_MAGTHRESH && IKabs(new_r11) < IKFAST_ATAN2_MAGTHRESH &&
                                                    IKabs(IKsqr(new_r10) + IKsqr(new_r11) - 1) <= IKFAST_SINCOS_THRESH) {
                                                  continue;
                                                }
                                                j5array[0] = IKatan2(new_r10, new_r11);
                                                sj5array[0] = IKsin(j5array[0]);
                                                cj5array[0] = IKcos(j5array[0]);
                                                if (j5array[0] > IKPI)
                                                  j5array[0] -= IK2PI;
                                                else if (j5array[0] < -IKPI)
                                                  j5array[0] += IK2PI;
                                                j5valid[0] = true;
                                                for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                  if (!j5valid[ij5])
                                                    continue;
                                                  _ij5[0] = ij5;
                                                  _ij5[1] = -1;
                                                  for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                    if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                        IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                      j5valid[iij5] = false;
                                                      _ij5[1] = iij5;
                                                      break;
                                                    }
                                                  }
                                                  j5 = j5array[ij5];
                                                  cj5 = cj5array[ij5];
                                                  sj5 = sj5array[ij5];
                                                  {
                                                    IkReal evalcond[8];
                                                    IkReal x540 = IKcos(j5);
                                                    IkReal x541 = IKsin(j5);
                                                    IkReal x542 = ((1.0) * sj4);
                                                    IkReal x543 = ((1.0) * x540);
                                                    IkReal x544 = ((1.0) * x541);
                                                    evalcond[0] = ((new_r02 * x540) + new_r20);
                                                    evalcond[1] = (((-1.0) * x544) + new_r10);
                                                    evalcond[2] = (((-1.0) * x543) + new_r11);
                                                    evalcond[3] = ((cj4 * x541) + new_r01);
                                                    evalcond[4] = (new_r21 + ((-1.0) * new_r02 * x544));
                                                    evalcond[5] = (new_r00 + ((-1.0) * cj4 * x543));
                                                    evalcond[6] = ((cj4 * new_r01) + ((-1.0) * new_r21 * x542) + x541);
                                                    evalcond[7] = ((cj4 * new_r00) + ((-1.0) * new_r20 * x542) + ((-1.0) * x543));
                                                    if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                        IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                        IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                        IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                                      continue;
                                                    }
                                                  }

                                                  {
                                                    std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                    vinfos[0].jointtype = 1;
                                                    vinfos[0].foffset = j0;
                                                    vinfos[0].indices[0] = _ij0[0];
                                                    vinfos[0].indices[1] = _ij0[1];
                                                    vinfos[0].maxsolutions = _nj0;
                                                    vinfos[1].jointtype = 1;
                                                    vinfos[1].foffset = j1;
                                                    vinfos[1].indices[0] = _ij1[0];
                                                    vinfos[1].indices[1] = _ij1[1];
                                                    vinfos[1].maxsolutions = _nj1;
                                                    vinfos[2].jointtype = 1;
                                                    vinfos[2].foffset = j2;
                                                    vinfos[2].indices[0] = _ij2[0];
                                                    vinfos[2].indices[1] = _ij2[1];
                                                    vinfos[2].maxsolutions = _nj2;
                                                    vinfos[3].jointtype = 1;
                                                    vinfos[3].foffset = j3;
                                                    vinfos[3].indices[0] = _ij3[0];
                                                    vinfos[3].indices[1] = _ij3[1];
                                                    vinfos[3].maxsolutions = _nj3;
                                                    vinfos[4].jointtype = 1;
                                                    vinfos[4].foffset = j4;
                                                    vinfos[4].indices[0] = _ij4[0];
                                                    vinfos[4].indices[1] = _ij4[1];
                                                    vinfos[4].maxsolutions = _nj4;
                                                    vinfos[5].jointtype = 1;
                                                    vinfos[5].foffset = j5;
                                                    vinfos[5].indices[0] = _ij5[0];
                                                    vinfos[5].indices[1] = _ij5[1];
                                                    vinfos[5].maxsolutions = _nj5;
                                                    std::vector<int> vfree(0);
                                                    solutions.AddSolution(vinfos, vfree);
                                                  }
                                                }
                                              }
                                            }
                                          } while (0);
                                          if (bgotonextstatement) {
                                            bool bgotonextstatement = true;
                                            do {
                                              evalcond[0] = ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs((-3.14159265358979) + j3))), 6.28318530717959)));
                                              evalcond[1] = new_r12;
                                              if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000) {
                                                bgotonextstatement = false;
                                                {
                                                  IkReal j5eval[3];
                                                  sj3 = 0;
                                                  cj3 = -1.0;
                                                  j3 = 3.14159265358979;
                                                  j5eval[0] = new_r02;
                                                  j5eval[1] = IKsign(new_r02);
                                                  j5eval[2] = ((IKabs(new_r20)) + (IKabs(new_r21)));
                                                  if (IKabs(j5eval[0]) < 0.0000010000000000 || IKabs(j5eval[1]) < 0.0000010000000000 || IKabs(j5eval[2]) < 0.0000010000000000) {
                                                    {
                                                      IkReal j5eval[1];
                                                      sj3 = 0;
                                                      cj3 = -1.0;
                                                      j3 = 3.14159265358979;
                                                      j5eval[0] = new_r02;
                                                      if (IKabs(j5eval[0]) < 0.0000010000000000) {
                                                        {
                                                          IkReal j5eval[2];
                                                          sj3 = 0;
                                                          cj3 = -1.0;
                                                          j3 = 3.14159265358979;
                                                          j5eval[0] = new_r02;
                                                          j5eval[1] = cj4;
                                                          if (IKabs(j5eval[0]) < 0.0000010000000000 || IKabs(j5eval[1]) < 0.0000010000000000) {
                                                            {
                                                              IkReal evalcond[4];
                                                              bool bgotonextstatement = true;
                                                              do {
                                                                evalcond[0] =
                                                                  ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs((-1.5707963267949) + j4))), 6.28318530717959)));
                                                                evalcond[1] = new_r22;
                                                                evalcond[2] = new_r01;
                                                                evalcond[3] = new_r00;
                                                                if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000 &&
                                                                    IKabs(evalcond[2]) < 0.0000050000000000 && IKabs(evalcond[3]) < 0.0000050000000000) {
                                                                  bgotonextstatement = false;
                                                                  {
                                                                    IkReal j5array[1], cj5array[1], sj5array[1];
                                                                    bool j5valid[1] = {false};
                                                                    _nj5 = 1;
                                                                    if (IKabs(new_r21) < IKFAST_ATAN2_MAGTHRESH && IKabs((-1.0) * new_r20) < IKFAST_ATAN2_MAGTHRESH &&
                                                                        IKabs(IKsqr(new_r21) + IKsqr((-1.0) * new_r20) - 1) <= IKFAST_SINCOS_THRESH) {
                                                                      continue;
                                                                    }
                                                                    j5array[0] = IKatan2(new_r21, ((-1.0) * new_r20));
                                                                    sj5array[0] = IKsin(j5array[0]);
                                                                    cj5array[0] = IKcos(j5array[0]);
                                                                    if (j5array[0] > IKPI) {
                                                                      j5array[0] -= IK2PI;
                                                                    } else if (j5array[0] < -IKPI) {
                                                                      j5array[0] += IK2PI;
                                                                    }
                                                                    j5valid[0] = true;
                                                                    for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                                      if (!j5valid[ij5]) {
                                                                        continue;
                                                                      }
                                                                      _ij5[0] = ij5;
                                                                      _ij5[1] = -1;
                                                                      for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                                        if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                            IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                          j5valid[iij5] = false;
                                                                          _ij5[1] = iij5;
                                                                          break;
                                                                        }
                                                                      }
                                                                      j5 = j5array[ij5];
                                                                      cj5 = cj5array[ij5];
                                                                      sj5 = sj5array[ij5];
                                                                      {
                                                                        IkReal evalcond[4];
                                                                        IkReal x545 = IKcos(j5);
                                                                        IkReal x546 = ((1.0) * (IKsin(j5)));
                                                                        evalcond[0] = (x545 + new_r20);
                                                                        evalcond[1] = (((-1.0) * x546) + new_r21);
                                                                        evalcond[2] = (((-1.0) * x546) + ((-1.0) * new_r10));
                                                                        evalcond[3] = (((-1.0) * x545) + ((-1.0) * new_r11));
                                                                        if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                            IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH) {
                                                                          continue;
                                                                        }
                                                                      }

                                                                      {
                                                                        std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                                        vinfos[0].jointtype = 1;
                                                                        vinfos[0].foffset = j0;
                                                                        vinfos[0].indices[0] = _ij0[0];
                                                                        vinfos[0].indices[1] = _ij0[1];
                                                                        vinfos[0].maxsolutions = _nj0;
                                                                        vinfos[1].jointtype = 1;
                                                                        vinfos[1].foffset = j1;
                                                                        vinfos[1].indices[0] = _ij1[0];
                                                                        vinfos[1].indices[1] = _ij1[1];
                                                                        vinfos[1].maxsolutions = _nj1;
                                                                        vinfos[2].jointtype = 1;
                                                                        vinfos[2].foffset = j2;
                                                                        vinfos[2].indices[0] = _ij2[0];
                                                                        vinfos[2].indices[1] = _ij2[1];
                                                                        vinfos[2].maxsolutions = _nj2;
                                                                        vinfos[3].jointtype = 1;
                                                                        vinfos[3].foffset = j3;
                                                                        vinfos[3].indices[0] = _ij3[0];
                                                                        vinfos[3].indices[1] = _ij3[1];
                                                                        vinfos[3].maxsolutions = _nj3;
                                                                        vinfos[4].jointtype = 1;
                                                                        vinfos[4].foffset = j4;
                                                                        vinfos[4].indices[0] = _ij4[0];
                                                                        vinfos[4].indices[1] = _ij4[1];
                                                                        vinfos[4].maxsolutions = _nj4;
                                                                        vinfos[5].jointtype = 1;
                                                                        vinfos[5].foffset = j5;
                                                                        vinfos[5].indices[0] = _ij5[0];
                                                                        vinfos[5].indices[1] = _ij5[1];
                                                                        vinfos[5].maxsolutions = _nj5;
                                                                        std::vector<int> vfree(0);
                                                                        solutions.AddSolution(vinfos, vfree);
                                                                      }
                                                                    }
                                                                  }
                                                                }
                                                              } while (0);
                                                              if (bgotonextstatement) {
                                                                bool bgotonextstatement = true;
                                                                do {
                                                                  evalcond[0] =
                                                                    ((-3.14159265358979) + (IKfmod(((3.14159265358979) + (IKabs((1.5707963267949) + j4))), 6.28318530717959)));
                                                                  evalcond[1] = new_r22;
                                                                  evalcond[2] = new_r01;
                                                                  evalcond[3] = new_r00;
                                                                  if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000 &&
                                                                      IKabs(evalcond[2]) < 0.0000050000000000 && IKabs(evalcond[3]) < 0.0000050000000000) {
                                                                    bgotonextstatement = false;
                                                                    {
                                                                      IkReal j5array[1], cj5array[1], sj5array[1];
                                                                      bool j5valid[1] = {false};
                                                                      _nj5 = 1;
                                                                      if (IKabs((-1.0) * new_r21) < IKFAST_ATAN2_MAGTHRESH && IKabs(new_r20) < IKFAST_ATAN2_MAGTHRESH &&
                                                                          IKabs(IKsqr((-1.0) * new_r21) + IKsqr(new_r20) - 1) <= IKFAST_SINCOS_THRESH) {
                                                                        continue;
                                                                      }
                                                                      j5array[0] = IKatan2(((-1.0) * new_r21), new_r20);
                                                                      sj5array[0] = IKsin(j5array[0]);
                                                                      cj5array[0] = IKcos(j5array[0]);
                                                                      if (j5array[0] > IKPI) {
                                                                        j5array[0] -= IK2PI;
                                                                      } else if (j5array[0] < -IKPI) {
                                                                        j5array[0] += IK2PI;
                                                                      }
                                                                      j5valid[0] = true;
                                                                      for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                                        if (!j5valid[ij5]) {
                                                                          continue;
                                                                        }
                                                                        _ij5[0] = ij5;
                                                                        _ij5[1] = -1;
                                                                        for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                                          if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                              IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                            j5valid[iij5] = false;
                                                                            _ij5[1] = iij5;
                                                                            break;
                                                                          }
                                                                        }
                                                                        j5 = j5array[ij5];
                                                                        cj5 = cj5array[ij5];
                                                                        sj5 = sj5array[ij5];
                                                                        {
                                                                          IkReal evalcond[4];
                                                                          IkReal x547 = IKsin(j5);
                                                                          IkReal x548 = ((1.0) * (IKcos(j5)));
                                                                          evalcond[0] = (x547 + new_r21);
                                                                          evalcond[1] = (((-1.0) * x548) + new_r20);
                                                                          evalcond[2] = (((-1.0) * x547) + ((-1.0) * new_r10));
                                                                          evalcond[3] = (((-1.0) * x548) + ((-1.0) * new_r11));
                                                                          if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                              IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH) {
                                                                            continue;
                                                                          }
                                                                        }

                                                                        {
                                                                          std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                                          vinfos[0].jointtype = 1;
                                                                          vinfos[0].foffset = j0;
                                                                          vinfos[0].indices[0] = _ij0[0];
                                                                          vinfos[0].indices[1] = _ij0[1];
                                                                          vinfos[0].maxsolutions = _nj0;
                                                                          vinfos[1].jointtype = 1;
                                                                          vinfos[1].foffset = j1;
                                                                          vinfos[1].indices[0] = _ij1[0];
                                                                          vinfos[1].indices[1] = _ij1[1];
                                                                          vinfos[1].maxsolutions = _nj1;
                                                                          vinfos[2].jointtype = 1;
                                                                          vinfos[2].foffset = j2;
                                                                          vinfos[2].indices[0] = _ij2[0];
                                                                          vinfos[2].indices[1] = _ij2[1];
                                                                          vinfos[2].maxsolutions = _nj2;
                                                                          vinfos[3].jointtype = 1;
                                                                          vinfos[3].foffset = j3;
                                                                          vinfos[3].indices[0] = _ij3[0];
                                                                          vinfos[3].indices[1] = _ij3[1];
                                                                          vinfos[3].maxsolutions = _nj3;
                                                                          vinfos[4].jointtype = 1;
                                                                          vinfos[4].foffset = j4;
                                                                          vinfos[4].indices[0] = _ij4[0];
                                                                          vinfos[4].indices[1] = _ij4[1];
                                                                          vinfos[4].maxsolutions = _nj4;
                                                                          vinfos[5].jointtype = 1;
                                                                          vinfos[5].foffset = j5;
                                                                          vinfos[5].indices[0] = _ij5[0];
                                                                          vinfos[5].indices[1] = _ij5[1];
                                                                          vinfos[5].maxsolutions = _nj5;
                                                                          std::vector<int> vfree(0);
                                                                          solutions.AddSolution(vinfos, vfree);
                                                                        }
                                                                      }
                                                                    }
                                                                  }
                                                                } while (0);
                                                                if (bgotonextstatement) {
                                                                  bool bgotonextstatement = true;
                                                                  do {
                                                                    evalcond[0] = IKabs(new_r02);
                                                                    evalcond[1] = new_r20;
                                                                    evalcond[2] = new_r21;
                                                                    if (IKabs(evalcond[0]) < 0.0000050000000000 && IKabs(evalcond[1]) < 0.0000050000000000 &&
                                                                        IKabs(evalcond[2]) < 0.0000050000000000) {
                                                                      bgotonextstatement = false;
                                                                      {
                                                                        IkReal j5array[1], cj5array[1], sj5array[1];
                                                                        bool j5valid[1] = {false};
                                                                        _nj5 = 1;
                                                                        if (IKabs(cj4 * new_r01) < IKFAST_ATAN2_MAGTHRESH && IKabs((-1.0) * new_r11) < IKFAST_ATAN2_MAGTHRESH &&
                                                                            IKabs(IKsqr(cj4 * new_r01) + IKsqr((-1.0) * new_r11) - 1) <= IKFAST_SINCOS_THRESH) {
                                                                          continue;
                                                                        }
                                                                        j5array[0] = IKatan2((cj4 * new_r01), ((-1.0) * new_r11));
                                                                        sj5array[0] = IKsin(j5array[0]);
                                                                        cj5array[0] = IKcos(j5array[0]);
                                                                        if (j5array[0] > IKPI) {
                                                                          j5array[0] -= IK2PI;
                                                                        } else if (j5array[0] < -IKPI) {
                                                                          j5array[0] += IK2PI;
                                                                        }
                                                                        j5valid[0] = true;
                                                                        for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                                          if (!j5valid[ij5]) {
                                                                            continue;
                                                                          }
                                                                          _ij5[0] = ij5;
                                                                          _ij5[1] = -1;
                                                                          for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                                            if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                                IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                              j5valid[iij5] = false;
                                                                              _ij5[1] = iij5;
                                                                              break;
                                                                            }
                                                                          }
                                                                          j5 = j5array[ij5];
                                                                          cj5 = cj5array[ij5];
                                                                          sj5 = sj5array[ij5];
                                                                          {
                                                                            IkReal evalcond[6];
                                                                            IkReal x549 = IKsin(j5);
                                                                            IkReal x550 = IKcos(j5);
                                                                            IkReal x551 = ((1.0) * new_r00);
                                                                            IkReal x552 = ((1.0) * new_r01);
                                                                            IkReal x553 = ((1.0) * x550);
                                                                            evalcond[0] = (x549 + ((-1.0) * cj4 * x552));
                                                                            evalcond[1] = (((-1.0) * x549) + ((-1.0) * new_r10));
                                                                            evalcond[2] = (((-1.0) * x553) + ((-1.0) * new_r11));
                                                                            evalcond[3] = ((cj4 * x549) + ((-1.0) * x552));
                                                                            evalcond[4] = (((-1.0) * x551) + ((-1.0) * cj4 * x553));
                                                                            evalcond[5] = (((-1.0) * x553) + ((-1.0) * cj4 * x551));
                                                                            if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                                IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                                                IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH) {
                                                                              continue;
                                                                            }
                                                                          }

                                                                          {
                                                                            std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                                            vinfos[0].jointtype = 1;
                                                                            vinfos[0].foffset = j0;
                                                                            vinfos[0].indices[0] = _ij0[0];
                                                                            vinfos[0].indices[1] = _ij0[1];
                                                                            vinfos[0].maxsolutions = _nj0;
                                                                            vinfos[1].jointtype = 1;
                                                                            vinfos[1].foffset = j1;
                                                                            vinfos[1].indices[0] = _ij1[0];
                                                                            vinfos[1].indices[1] = _ij1[1];
                                                                            vinfos[1].maxsolutions = _nj1;
                                                                            vinfos[2].jointtype = 1;
                                                                            vinfos[2].foffset = j2;
                                                                            vinfos[2].indices[0] = _ij2[0];
                                                                            vinfos[2].indices[1] = _ij2[1];
                                                                            vinfos[2].maxsolutions = _nj2;
                                                                            vinfos[3].jointtype = 1;
                                                                            vinfos[3].foffset = j3;
                                                                            vinfos[3].indices[0] = _ij3[0];
                                                                            vinfos[3].indices[1] = _ij3[1];
                                                                            vinfos[3].maxsolutions = _nj3;
                                                                            vinfos[4].jointtype = 1;
                                                                            vinfos[4].foffset = j4;
                                                                            vinfos[4].indices[0] = _ij4[0];
                                                                            vinfos[4].indices[1] = _ij4[1];
                                                                            vinfos[4].maxsolutions = _nj4;
                                                                            vinfos[5].jointtype = 1;
                                                                            vinfos[5].foffset = j5;
                                                                            vinfos[5].indices[0] = _ij5[0];
                                                                            vinfos[5].indices[1] = _ij5[1];
                                                                            vinfos[5].maxsolutions = _nj5;
                                                                            std::vector<int> vfree(0);
                                                                            solutions.AddSolution(vinfos, vfree);
                                                                          }
                                                                        }
                                                                      }
                                                                    }
                                                                  } while (0);
                                                                  if (bgotonextstatement) {
                                                                    bool bgotonextstatement = true;
                                                                    do {
                                                                      evalcond[0] = ((IKabs(new_r20)) + (IKabs(new_r21)));
                                                                      if (IKabs(evalcond[0]) < 0.0000050000000000) {
                                                                        bgotonextstatement = false;
                                                                        {
                                                                          IkReal j5array[1], cj5array[1], sj5array[1];
                                                                          bool j5valid[1] = {false};
                                                                          _nj5 = 1;
                                                                          if (IKabs((-1.0) * new_r10) < IKFAST_ATAN2_MAGTHRESH &&
                                                                              IKabs((-1.0) * new_r11) < IKFAST_ATAN2_MAGTHRESH &&
                                                                              IKabs(IKsqr((-1.0) * new_r10) + IKsqr((-1.0) * new_r11) - 1) <= IKFAST_SINCOS_THRESH) {
                                                                            continue;
                                                                          }
                                                                          j5array[0] = IKatan2(((-1.0) * new_r10), ((-1.0) * new_r11));
                                                                          sj5array[0] = IKsin(j5array[0]);
                                                                          cj5array[0] = IKcos(j5array[0]);
                                                                          if (j5array[0] > IKPI) {
                                                                            j5array[0] -= IK2PI;
                                                                          } else if (j5array[0] < -IKPI) {
                                                                            j5array[0] += IK2PI;
                                                                          }
                                                                          j5valid[0] = true;
                                                                          for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                                            if (!j5valid[ij5]) {
                                                                              continue;
                                                                            }
                                                                            _ij5[0] = ij5;
                                                                            _ij5[1] = -1;
                                                                            for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                                              if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                                  IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                                j5valid[iij5] = false;
                                                                                _ij5[1] = iij5;
                                                                                break;
                                                                              }
                                                                            }
                                                                            j5 = j5array[ij5];
                                                                            cj5 = cj5array[ij5];
                                                                            sj5 = sj5array[ij5];
                                                                            {
                                                                              IkReal evalcond[6];
                                                                              IkReal x554 = IKsin(j5);
                                                                              IkReal x555 = IKcos(j5);
                                                                              IkReal x556 = ((-1.0) * x555);
                                                                              evalcond[0] = x554;
                                                                              evalcond[1] = (new_r22 * x554);
                                                                              evalcond[2] = x556;
                                                                              evalcond[3] = (new_r22 * x556);
                                                                              evalcond[4] = (((-1.0) * x554) + ((-1.0) * new_r10));
                                                                              evalcond[5] = (((-1.0) * x555) + ((-1.0) * new_r11));
                                                                              if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                                  IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                                                  IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH) {
                                                                                continue;
                                                                              }
                                                                            }

                                                                            {
                                                                              std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                                              vinfos[0].jointtype = 1;
                                                                              vinfos[0].foffset = j0;
                                                                              vinfos[0].indices[0] = _ij0[0];
                                                                              vinfos[0].indices[1] = _ij0[1];
                                                                              vinfos[0].maxsolutions = _nj0;
                                                                              vinfos[1].jointtype = 1;
                                                                              vinfos[1].foffset = j1;
                                                                              vinfos[1].indices[0] = _ij1[0];
                                                                              vinfos[1].indices[1] = _ij1[1];
                                                                              vinfos[1].maxsolutions = _nj1;
                                                                              vinfos[2].jointtype = 1;
                                                                              vinfos[2].foffset = j2;
                                                                              vinfos[2].indices[0] = _ij2[0];
                                                                              vinfos[2].indices[1] = _ij2[1];
                                                                              vinfos[2].maxsolutions = _nj2;
                                                                              vinfos[3].jointtype = 1;
                                                                              vinfos[3].foffset = j3;
                                                                              vinfos[3].indices[0] = _ij3[0];
                                                                              vinfos[3].indices[1] = _ij3[1];
                                                                              vinfos[3].maxsolutions = _nj3;
                                                                              vinfos[4].jointtype = 1;
                                                                              vinfos[4].foffset = j4;
                                                                              vinfos[4].indices[0] = _ij4[0];
                                                                              vinfos[4].indices[1] = _ij4[1];
                                                                              vinfos[4].maxsolutions = _nj4;
                                                                              vinfos[5].jointtype = 1;
                                                                              vinfos[5].foffset = j5;
                                                                              vinfos[5].indices[0] = _ij5[0];
                                                                              vinfos[5].indices[1] = _ij5[1];
                                                                              vinfos[5].maxsolutions = _nj5;
                                                                              std::vector<int> vfree(0);
                                                                              solutions.AddSolution(vinfos, vfree);
                                                                            }
                                                                          }
                                                                        }
                                                                      }
                                                                    } while (0);
                                                                    if (bgotonextstatement) {
                                                                      bool bgotonextstatement = true;
                                                                      do {
                                                                        if (1) {
                                                                          bgotonextstatement = false;
                                                                          continue; // branch miss [j5]
                                                                        }
                                                                      } while (0);
                                                                      if (bgotonextstatement) {
                                                                      }
                                                                    }
                                                                  }
                                                                }
                                                              }
                                                            }

                                                          } else {
                                                            {
                                                              IkReal j5array[1], cj5array[1], sj5array[1];
                                                              bool j5valid[1] = {false};
                                                              _nj5 = 1;
                                                              CheckValue<IkReal> x557 = IKPowWithIntegerCheck(new_r02, -1);
                                                              if (!x557.valid)
                                                                continue;
                                                              CheckValue<IkReal> x558 = IKPowWithIntegerCheck(cj4, -1);
                                                              if (!x558.valid)
                                                                continue;
                                                              if (IKabs((-1.0) * new_r21 * (x557.value)) < IKFAST_ATAN2_MAGTHRESH &&
                                                                  IKabs((-1.0) * new_r00 * (x558.value)) < IKFAST_ATAN2_MAGTHRESH &&
                                                                  IKabs(IKsqr((-1.0) * new_r21 * (x557.value)) + IKsqr((-1.0) * new_r00 * (x558.value)) - 1) <=
                                                                    IKFAST_SINCOS_THRESH) {
                                                                continue;
                                                              }
                                                              j5array[0] = IKatan2(((-1.0) * new_r21 * (x557.value)), ((-1.0) * new_r00 * (x558.value)));
                                                              sj5array[0] = IKsin(j5array[0]);
                                                              cj5array[0] = IKcos(j5array[0]);
                                                              if (j5array[0] > IKPI) {
                                                                j5array[0] -= IK2PI;
                                                              } else if (j5array[0] < -IKPI) {
                                                                j5array[0] += IK2PI;
                                                              }
                                                              j5valid[0] = true;
                                                              for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                                if (!j5valid[ij5]) {
                                                                  continue;
                                                                }
                                                                _ij5[0] = ij5;
                                                                _ij5[1] = -1;
                                                                for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                                  if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                      IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                    j5valid[iij5] = false;
                                                                    _ij5[1] = iij5;
                                                                    break;
                                                                  }
                                                                }
                                                                j5 = j5array[ij5];
                                                                cj5 = cj5array[ij5];
                                                                sj5 = sj5array[ij5];
                                                                {
                                                                  IkReal evalcond[8];
                                                                  IkReal x559 = IKsin(j5);
                                                                  IkReal x560 = IKcos(j5);
                                                                  IkReal x561 = ((1.0) * new_r00);
                                                                  IkReal x562 = ((1.0) * new_r01);
                                                                  IkReal x563 = ((1.0) * sj4);
                                                                  IkReal x564 = ((1.0) * x560);
                                                                  evalcond[0] = ((new_r02 * x559) + new_r21);
                                                                  evalcond[1] = (((-1.0) * new_r02 * x564) + new_r20);
                                                                  evalcond[2] = (((-1.0) * x559) + ((-1.0) * new_r10));
                                                                  evalcond[3] = (((-1.0) * new_r11) + ((-1.0) * x564));
                                                                  evalcond[4] = ((cj4 * x559) + ((-1.0) * x562));
                                                                  evalcond[5] = (((-1.0) * cj4 * x564) + ((-1.0) * x561));
                                                                  evalcond[6] = (((-1.0) * cj4 * x562) + x559 + ((-1.0) * new_r21 * x563));
                                                                  evalcond[7] = (((-1.0) * new_r20 * x563) + ((-1.0) * cj4 * x561) + ((-1.0) * x564));
                                                                  if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                      IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                                      IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                                      IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                                                    continue;
                                                                  }
                                                                }

                                                                {
                                                                  std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                                  vinfos[0].jointtype = 1;
                                                                  vinfos[0].foffset = j0;
                                                                  vinfos[0].indices[0] = _ij0[0];
                                                                  vinfos[0].indices[1] = _ij0[1];
                                                                  vinfos[0].maxsolutions = _nj0;
                                                                  vinfos[1].jointtype = 1;
                                                                  vinfos[1].foffset = j1;
                                                                  vinfos[1].indices[0] = _ij1[0];
                                                                  vinfos[1].indices[1] = _ij1[1];
                                                                  vinfos[1].maxsolutions = _nj1;
                                                                  vinfos[2].jointtype = 1;
                                                                  vinfos[2].foffset = j2;
                                                                  vinfos[2].indices[0] = _ij2[0];
                                                                  vinfos[2].indices[1] = _ij2[1];
                                                                  vinfos[2].maxsolutions = _nj2;
                                                                  vinfos[3].jointtype = 1;
                                                                  vinfos[3].foffset = j3;
                                                                  vinfos[3].indices[0] = _ij3[0];
                                                                  vinfos[3].indices[1] = _ij3[1];
                                                                  vinfos[3].maxsolutions = _nj3;
                                                                  vinfos[4].jointtype = 1;
                                                                  vinfos[4].foffset = j4;
                                                                  vinfos[4].indices[0] = _ij4[0];
                                                                  vinfos[4].indices[1] = _ij4[1];
                                                                  vinfos[4].maxsolutions = _nj4;
                                                                  vinfos[5].jointtype = 1;
                                                                  vinfos[5].foffset = j5;
                                                                  vinfos[5].indices[0] = _ij5[0];
                                                                  vinfos[5].indices[1] = _ij5[1];
                                                                  vinfos[5].maxsolutions = _nj5;
                                                                  std::vector<int> vfree(0);
                                                                  solutions.AddSolution(vinfos, vfree);
                                                                }
                                                              }
                                                            }
                                                          }
                                                        }

                                                      } else {
                                                        {
                                                          IkReal j5array[1], cj5array[1], sj5array[1];
                                                          bool j5valid[1] = {false};
                                                          _nj5 = 1;
                                                          CheckValue<IkReal> x565 = IKPowWithIntegerCheck(new_r02, -1);
                                                          if (!x565.valid)
                                                            continue;
                                                          if (IKabs((-1.0) * new_r10) < IKFAST_ATAN2_MAGTHRESH && IKabs(new_r20 * (x565.value)) < IKFAST_ATAN2_MAGTHRESH &&
                                                              IKabs(IKsqr((-1.0) * new_r10) + IKsqr(new_r20 * (x565.value)) - 1) <= IKFAST_SINCOS_THRESH) {
                                                            continue;
                                                          }
                                                          j5array[0] = IKatan2(((-1.0) * new_r10), (new_r20 * (x565.value)));
                                                          sj5array[0] = IKsin(j5array[0]);
                                                          cj5array[0] = IKcos(j5array[0]);
                                                          if (j5array[0] > IKPI) {
                                                            j5array[0] -= IK2PI;
                                                          } else if (j5array[0] < -IKPI) {
                                                            j5array[0] += IK2PI;
                                                          }
                                                          j5valid[0] = true;
                                                          for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                            if (!j5valid[ij5])
                                                              continue;
                                                            _ij5[0] = ij5;
                                                            _ij5[1] = -1;
                                                            for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                              if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                                  IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                                j5valid[iij5] = false;
                                                                _ij5[1] = iij5;
                                                                break;
                                                              }
                                                            }
                                                            j5 = j5array[ij5];
                                                            cj5 = cj5array[ij5];
                                                            sj5 = sj5array[ij5];
                                                            {
                                                              IkReal evalcond[8];
                                                              IkReal x566 = IKsin(j5);
                                                              IkReal x567 = IKcos(j5);
                                                              IkReal x568 = ((1.0) * new_r00);
                                                              IkReal x569 = ((1.0) * new_r01);
                                                              IkReal x570 = ((1.0) * sj4);
                                                              IkReal x571 = ((1.0) * x567);
                                                              evalcond[0] = ((new_r02 * x566) + new_r21);
                                                              evalcond[1] = (((-1.0) * new_r02 * x571) + new_r20);
                                                              evalcond[2] = (((-1.0) * x566) + ((-1.0) * new_r10));
                                                              evalcond[3] = (((-1.0) * x571) + ((-1.0) * new_r11));
                                                              evalcond[4] = ((cj4 * x566) + ((-1.0) * x569));
                                                              evalcond[5] = (((-1.0) * cj4 * x571) + ((-1.0) * x568));
                                                              evalcond[6] = (((-1.0) * cj4 * x569) + x566 + ((-1.0) * new_r21 * x570));
                                                              evalcond[7] = (((-1.0) * cj4 * x568) + ((-1.0) * x571) + ((-1.0) * new_r20 * x570));
                                                              if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                                  IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                                  IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                                  IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                                                continue;
                                                              }
                                                            }

                                                            {
                                                              std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                              vinfos[0].jointtype = 1;
                                                              vinfos[0].foffset = j0;
                                                              vinfos[0].indices[0] = _ij0[0];
                                                              vinfos[0].indices[1] = _ij0[1];
                                                              vinfos[0].maxsolutions = _nj0;
                                                              vinfos[1].jointtype = 1;
                                                              vinfos[1].foffset = j1;
                                                              vinfos[1].indices[0] = _ij1[0];
                                                              vinfos[1].indices[1] = _ij1[1];
                                                              vinfos[1].maxsolutions = _nj1;
                                                              vinfos[2].jointtype = 1;
                                                              vinfos[2].foffset = j2;
                                                              vinfos[2].indices[0] = _ij2[0];
                                                              vinfos[2].indices[1] = _ij2[1];
                                                              vinfos[2].maxsolutions = _nj2;
                                                              vinfos[3].jointtype = 1;
                                                              vinfos[3].foffset = j3;
                                                              vinfos[3].indices[0] = _ij3[0];
                                                              vinfos[3].indices[1] = _ij3[1];
                                                              vinfos[3].maxsolutions = _nj3;
                                                              vinfos[4].jointtype = 1;
                                                              vinfos[4].foffset = j4;
                                                              vinfos[4].indices[0] = _ij4[0];
                                                              vinfos[4].indices[1] = _ij4[1];
                                                              vinfos[4].maxsolutions = _nj4;
                                                              vinfos[5].jointtype = 1;
                                                              vinfos[5].foffset = j5;
                                                              vinfos[5].indices[0] = _ij5[0];
                                                              vinfos[5].indices[1] = _ij5[1];
                                                              vinfos[5].maxsolutions = _nj5;
                                                              std::vector<int> vfree(0);
                                                              solutions.AddSolution(vinfos, vfree);
                                                            }
                                                          }
                                                        }
                                                      }
                                                    }

                                                  } else {
                                                    {
                                                      IkReal j5array[1], cj5array[1], sj5array[1];
                                                      bool j5valid[1] = {false};
                                                      _nj5 = 1;
                                                      CheckValue<IkReal> x572 = IKatan2WithCheck(IkReal((-1.0) * new_r21), IkReal(new_r20), IKFAST_ATAN2_MAGTHRESH);
                                                      if (!x572.valid)
                                                        continue;
                                                      CheckValue<IkReal> x573 = IKPowWithIntegerCheck(IKsign(new_r02), -1);
                                                      if (!x573.valid)
                                                        continue;
                                                      j5array[0] = ((-1.5707963267949) + (x572.value) + ((1.5707963267949) * (x573.value)));
                                                      sj5array[0] = IKsin(j5array[0]);
                                                      cj5array[0] = IKcos(j5array[0]);
                                                      if (j5array[0] > IKPI) {
                                                        j5array[0] -= IK2PI;
                                                      } else if (j5array[0] < -IKPI) {
                                                        j5array[0] += IK2PI;
                                                      }
                                                      j5valid[0] = true;
                                                      for (int ij5 = 0; ij5 < 1; ++ij5) {
                                                        if (!j5valid[ij5])
                                                          continue;
                                                        _ij5[0] = ij5;
                                                        _ij5[1] = -1;
                                                        for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                                          if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                              IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                            j5valid[iij5] = false;
                                                            _ij5[1] = iij5;
                                                            break;
                                                          }
                                                        }
                                                        j5 = j5array[ij5];
                                                        cj5 = cj5array[ij5];
                                                        sj5 = sj5array[ij5];
                                                        {
                                                          IkReal evalcond[8];
                                                          IkReal x574 = IKsin(j5);
                                                          IkReal x575 = IKcos(j5);
                                                          IkReal x576 = ((1.0) * new_r00);
                                                          IkReal x577 = ((1.0) * new_r01);
                                                          IkReal x578 = ((1.0) * sj4);
                                                          IkReal x579 = ((1.0) * x575);
                                                          evalcond[0] = (new_r21 + (new_r02 * x574));
                                                          evalcond[1] = (((-1.0) * new_r02 * x579) + new_r20);
                                                          evalcond[2] = (((-1.0) * x574) + ((-1.0) * new_r10));
                                                          evalcond[3] = (((-1.0) * x579) + ((-1.0) * new_r11));
                                                          evalcond[4] = (((-1.0) * x577) + (cj4 * x574));
                                                          evalcond[5] = (((-1.0) * cj4 * x579) + ((-1.0) * x576));
                                                          evalcond[6] = (((-1.0) * cj4 * x577) + x574 + ((-1.0) * new_r21 * x578));
                                                          evalcond[7] = (((-1.0) * cj4 * x576) + ((-1.0) * x579) + ((-1.0) * new_r20 * x578));
                                                          if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH ||
                                                              IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH ||
                                                              IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                                              IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH) {
                                                            continue;
                                                          }
                                                        }

                                                        {
                                                          std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                          vinfos[0].jointtype = 1;
                                                          vinfos[0].foffset = j0;
                                                          vinfos[0].indices[0] = _ij0[0];
                                                          vinfos[0].indices[1] = _ij0[1];
                                                          vinfos[0].maxsolutions = _nj0;
                                                          vinfos[1].jointtype = 1;
                                                          vinfos[1].foffset = j1;
                                                          vinfos[1].indices[0] = _ij1[0];
                                                          vinfos[1].indices[1] = _ij1[1];
                                                          vinfos[1].maxsolutions = _nj1;
                                                          vinfos[2].jointtype = 1;
                                                          vinfos[2].foffset = j2;
                                                          vinfos[2].indices[0] = _ij2[0];
                                                          vinfos[2].indices[1] = _ij2[1];
                                                          vinfos[2].maxsolutions = _nj2;
                                                          vinfos[3].jointtype = 1;
                                                          vinfos[3].foffset = j3;
                                                          vinfos[3].indices[0] = _ij3[0];
                                                          vinfos[3].indices[1] = _ij3[1];
                                                          vinfos[3].maxsolutions = _nj3;
                                                          vinfos[4].jointtype = 1;
                                                          vinfos[4].foffset = j4;
                                                          vinfos[4].indices[0] = _ij4[0];
                                                          vinfos[4].indices[1] = _ij4[1];
                                                          vinfos[4].maxsolutions = _nj4;
                                                          vinfos[5].jointtype = 1;
                                                          vinfos[5].foffset = j5;
                                                          vinfos[5].indices[0] = _ij5[0];
                                                          vinfos[5].indices[1] = _ij5[1];
                                                          vinfos[5].maxsolutions = _nj5;
                                                          std::vector<int> vfree(0);
                                                          solutions.AddSolution(vinfos, vfree);
                                                        }
                                                      }
                                                    }
                                                  }
                                                }
                                              }
                                            } while (0);
                                            if (bgotonextstatement) {
                                              bool bgotonextstatement = true;
                                              do {
                                                evalcond[0] = ((IKabs(new_r20)) + (IKabs(new_r21)));
                                                if (IKabs(evalcond[0]) < 0.0000050000000000) {
                                                  bgotonextstatement = false;
                                                  {
                                                    IkReal j5eval[1];
                                                    new_r21 = 0;
                                                    new_r20 = 0;
                                                    new_r02 = 0;
                                                    new_r12 = 0;
                                                    j5eval[0] = 1.0;
                                                    if (IKabs(j5eval[0]) < 0.0000000100000000) {
                                                      continue; // no branches
                                                                // [j5]

                                                    } else {
                                                      IkReal op[2 + 1], zeror[2];
                                                      int numroots;
                                                      op[0] = 1.0;
                                                      op[1] = 0;
                                                      op[2] = -1.0;
                                                      polyroots2(op, zeror, numroots);
                                                      IkReal j5array[2], cj5array[2], sj5array[2], tempj5array[1];
                                                      int numsolutions = 0;
                                                      for (int ij5 = 0; ij5 < numroots; ++ij5) {
                                                        IkReal htj5 = zeror[ij5];
                                                        tempj5array[0] = ((2.0) * (atan(htj5)));
                                                        for (int kj5 = 0; kj5 < 1; ++kj5) {
                                                          j5array[numsolutions] = tempj5array[kj5];
                                                          if (j5array[numsolutions] > IKPI) {
                                                            j5array[numsolutions] -= IK2PI;
                                                          } else if (j5array[numsolutions] < -IKPI) {
                                                            j5array[numsolutions] += IK2PI;
                                                          }
                                                          sj5array[numsolutions] = IKsin(j5array[numsolutions]);
                                                          cj5array[numsolutions] = IKcos(j5array[numsolutions]);
                                                          numsolutions++;
                                                        }
                                                      }
                                                      bool j5valid[2] = {true, true};
                                                      _nj5 = 2;
                                                      for (int ij5 = 0; ij5 < numsolutions; ++ij5) {
                                                        if (!j5valid[ij5])
                                                          continue;
                                                        j5 = j5array[ij5];
                                                        cj5 = cj5array[ij5];
                                                        sj5 = sj5array[ij5];
                                                        htj5 = IKtan(j5 / 2);

                                                        _ij5[0] = ij5;
                                                        _ij5[1] = -1;
                                                        for (int iij5 = ij5 + 1; iij5 < numsolutions; ++iij5) {
                                                          if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                                              IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                                            j5valid[iij5] = false;
                                                            _ij5[1] = iij5;
                                                            break;
                                                          }
                                                        }
                                                        {
                                                          std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                                          vinfos[0].jointtype = 1;
                                                          vinfos[0].foffset = j0;
                                                          vinfos[0].indices[0] = _ij0[0];
                                                          vinfos[0].indices[1] = _ij0[1];
                                                          vinfos[0].maxsolutions = _nj0;
                                                          vinfos[1].jointtype = 1;
                                                          vinfos[1].foffset = j1;
                                                          vinfos[1].indices[0] = _ij1[0];
                                                          vinfos[1].indices[1] = _ij1[1];
                                                          vinfos[1].maxsolutions = _nj1;
                                                          vinfos[2].jointtype = 1;
                                                          vinfos[2].foffset = j2;
                                                          vinfos[2].indices[0] = _ij2[0];
                                                          vinfos[2].indices[1] = _ij2[1];
                                                          vinfos[2].maxsolutions = _nj2;
                                                          vinfos[3].jointtype = 1;
                                                          vinfos[3].foffset = j3;
                                                          vinfos[3].indices[0] = _ij3[0];
                                                          vinfos[3].indices[1] = _ij3[1];
                                                          vinfos[3].maxsolutions = _nj3;
                                                          vinfos[4].jointtype = 1;
                                                          vinfos[4].foffset = j4;
                                                          vinfos[4].indices[0] = _ij4[0];
                                                          vinfos[4].indices[1] = _ij4[1];
                                                          vinfos[4].maxsolutions = _nj4;
                                                          vinfos[5].jointtype = 1;
                                                          vinfos[5].foffset = j5;
                                                          vinfos[5].indices[0] = _ij5[0];
                                                          vinfos[5].indices[1] = _ij5[1];
                                                          vinfos[5].maxsolutions = _nj5;
                                                          std::vector<int> vfree(0);
                                                          solutions.AddSolution(vinfos, vfree);
                                                        }
                                                      }
                                                    }
                                                  }
                                                }
                                              } while (0);
                                              if (bgotonextstatement) {
                                                bool bgotonextstatement = true;
                                                do {
                                                  if (1) {
                                                    bgotonextstatement = false;
                                                    continue; // branch miss
                                                              // [j5]
                                                  }
                                                } while (0);
                                                if (bgotonextstatement) {
                                                }
                                              }
                                            }
                                          }
                                        }
                                      }
                                    }
                                  }
                                }
                              }
                            }

                          } else {
                            {
                              IkReal j5array[1], cj5array[1], sj5array[1];
                              bool j5valid[1] = {false};
                              _nj5 = 1;
                              CheckValue<IkReal> x581 = IKPowWithIntegerCheck(sj4, -1);
                              if (!x581.valid)
                                continue;
                              IkReal x580 = x581.value;
                              CheckValue<IkReal> x582 = IKPowWithIntegerCheck(cj3, -1);
                              if (!x582.valid)
                                continue;
                              CheckValue<IkReal> x583 = IKPowWithIntegerCheck(cj4, -1);
                              if (!x583.valid)
                                continue;
                              if (IKabs(x580 * (x582.value) * (x583.value) * ((new_r20 * sj3) + ((-1.0) * new_r01 * sj4))) < IKFAST_ATAN2_MAGTHRESH &&
                                  IKabs((-1.0) * new_r20 * x580) < IKFAST_ATAN2_MAGTHRESH &&
                                  IKabs(IKsqr(x580 * (x582.value) * (x583.value) * ((new_r20 * sj3) + ((-1.0) * new_r01 * sj4))) + IKsqr((-1.0) * new_r20 * x580) - 1) <=
                                    IKFAST_SINCOS_THRESH) {
                                continue;
                              }
                              j5array[0] = IKatan2((x580 * (x582.value) * (x583.value) * ((new_r20 * sj3) + ((-1.0) * new_r01 * sj4))), ((-1.0) * new_r20 * x580));
                              sj5array[0] = IKsin(j5array[0]);
                              cj5array[0] = IKcos(j5array[0]);
                              if (j5array[0] > IKPI)
                                j5array[0] -= IK2PI;
                              else if (j5array[0] < -IKPI)
                                j5array[0] += IK2PI;
                              j5valid[0] = true;
                              for (int ij5 = 0; ij5 < 1; ++ij5) {
                                if (!j5valid[ij5])
                                  continue;
                                _ij5[0] = ij5;
                                _ij5[1] = -1;
                                for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                                  if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                      IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                    j5valid[iij5] = false;
                                    _ij5[1] = iij5;
                                    break;
                                  }
                                }
                                j5 = j5array[ij5];
                                cj5 = cj5array[ij5];
                                sj5 = sj5array[ij5];
                                {
                                  IkReal evalcond[12];
                                  IkReal x584 = IKsin(j5);
                                  IkReal x585 = IKcos(j5);
                                  IkReal x586 = (cj3 * new_r00);
                                  IkReal x587 = (cj3 * cj4);
                                  IkReal x588 = (cj4 * sj3);
                                  IkReal x589 = ((1.0) * sj3);
                                  IkReal x590 = ((1.0) * sj4);
                                  IkReal x591 = ((1.0) * x585);
                                  IkReal x592 = (sj3 * x584);
                                  IkReal x593 = ((1.0) * x584);
                                  evalcond[0] = ((sj4 * x585) + new_r20);
                                  evalcond[1] = (((-1.0) * x584 * x590) + new_r21);
                                  evalcond[2] = ((new_r11 * sj3) + (cj4 * x584) + (cj3 * new_r01));
                                  evalcond[3] = (((-1.0) * x593) + (cj3 * new_r10) + ((-1.0) * new_r00 * x589));
                                  evalcond[4] = (((-1.0) * new_r01 * x589) + ((-1.0) * x591) + (cj3 * new_r11));
                                  evalcond[5] = ((x584 * x587) + (sj3 * x585) + new_r01);
                                  evalcond[6] = (((-1.0) * cj4 * x591) + (new_r10 * sj3) + x586);
                                  evalcond[7] = (((-1.0) * x587 * x591) + x592 + new_r00);
                                  evalcond[8] = ((x584 * x588) + ((-1.0) * cj3 * x591) + new_r11);
                                  evalcond[9] = (((-1.0) * x588 * x591) + ((-1.0) * cj3 * x593) + new_r10);
                                  evalcond[10] = (((-1.0) * new_r21 * x590) + x584 + (new_r01 * x587) + (new_r11 * x588));
                                  evalcond[11] = (((-1.0) * new_r20 * x590) + (cj4 * x586) + ((-1.0) * x591) + (new_r10 * x588));
                                  if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH ||
                                      IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                      IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[8]) > IKFAST_EVALCOND_THRESH ||
                                      IKabs(evalcond[9]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[10]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[11]) > IKFAST_EVALCOND_THRESH) {
                                    continue;
                                  }
                                }

                                {
                                  std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                                  vinfos[0].jointtype = 1;
                                  vinfos[0].foffset = j0;
                                  vinfos[0].indices[0] = _ij0[0];
                                  vinfos[0].indices[1] = _ij0[1];
                                  vinfos[0].maxsolutions = _nj0;
                                  vinfos[1].jointtype = 1;
                                  vinfos[1].foffset = j1;
                                  vinfos[1].indices[0] = _ij1[0];
                                  vinfos[1].indices[1] = _ij1[1];
                                  vinfos[1].maxsolutions = _nj1;
                                  vinfos[2].jointtype = 1;
                                  vinfos[2].foffset = j2;
                                  vinfos[2].indices[0] = _ij2[0];
                                  vinfos[2].indices[1] = _ij2[1];
                                  vinfos[2].maxsolutions = _nj2;
                                  vinfos[3].jointtype = 1;
                                  vinfos[3].foffset = j3;
                                  vinfos[3].indices[0] = _ij3[0];
                                  vinfos[3].indices[1] = _ij3[1];
                                  vinfos[3].maxsolutions = _nj3;
                                  vinfos[4].jointtype = 1;
                                  vinfos[4].foffset = j4;
                                  vinfos[4].indices[0] = _ij4[0];
                                  vinfos[4].indices[1] = _ij4[1];
                                  vinfos[4].maxsolutions = _nj4;
                                  vinfos[5].jointtype = 1;
                                  vinfos[5].foffset = j5;
                                  vinfos[5].indices[0] = _ij5[0];
                                  vinfos[5].indices[1] = _ij5[1];
                                  vinfos[5].maxsolutions = _nj5;
                                  std::vector<int> vfree(0);
                                  solutions.AddSolution(vinfos, vfree);
                                }
                              }
                            }
                          }
                        }

                      } else {
                        {
                          IkReal j5array[1], cj5array[1], sj5array[1];
                          bool j5valid[1] = {false};
                          _nj5 = 1;
                          CheckValue<IkReal> x595 = IKPowWithIntegerCheck(sj4, -1);
                          if (!x595.valid)
                            continue;
                          IkReal x594 = x595.value;
                          CheckValue<IkReal> x596 = IKPowWithIntegerCheck(sj3, -1);
                          if (!x596.valid)
                            continue;
                          if (IKabs(x594 * (x596.value) * (((-1.0) * cj3 * cj4 * new_r20) + ((-1.0) * new_r00 * sj4))) < IKFAST_ATAN2_MAGTHRESH &&
                              IKabs((-1.0) * new_r20 * x594) < IKFAST_ATAN2_MAGTHRESH &&
                              IKabs(IKsqr(x594 * (x596.value) * (((-1.0) * cj3 * cj4 * new_r20) + ((-1.0) * new_r00 * sj4))) + IKsqr((-1.0) * new_r20 * x594) - 1) <=
                                IKFAST_SINCOS_THRESH) {
                            continue;
                          }
                          j5array[0] = IKatan2((x594 * (x596.value) * (((-1.0) * cj3 * cj4 * new_r20) + ((-1.0) * new_r00 * sj4))), ((-1.0) * new_r20 * x594));
                          sj5array[0] = IKsin(j5array[0]);
                          cj5array[0] = IKcos(j5array[0]);
                          if (j5array[0] > IKPI)
                            j5array[0] -= IK2PI;
                          else if (j5array[0] < -IKPI)
                            j5array[0] += IK2PI;
                          j5valid[0] = true;
                          for (int ij5 = 0; ij5 < 1; ++ij5) {
                            if (!j5valid[ij5])
                              continue;
                            _ij5[0] = ij5;
                            _ij5[1] = -1;
                            for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                              if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH &&
                                  IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                                j5valid[iij5] = false;
                                _ij5[1] = iij5;
                                break;
                              }
                            }
                            j5 = j5array[ij5];
                            cj5 = cj5array[ij5];
                            sj5 = sj5array[ij5];
                            {
                              IkReal evalcond[12];
                              IkReal x597 = IKsin(j5);
                              IkReal x598 = IKcos(j5);
                              IkReal x599 = (cj3 * new_r00);
                              IkReal x600 = (cj3 * cj4);
                              IkReal x601 = (cj4 * sj3);
                              IkReal x602 = ((1.0) * sj3);
                              IkReal x603 = ((1.0) * sj4);
                              IkReal x604 = ((1.0) * x598);
                              IkReal x605 = (sj3 * x597);
                              IkReal x606 = ((1.0) * x597);
                              evalcond[0] = ((sj4 * x598) + new_r20);
                              evalcond[1] = (((-1.0) * x597 * x603) + new_r21);
                              evalcond[2] = ((new_r11 * sj3) + (cj3 * new_r01) + (cj4 * x597));
                              evalcond[3] = ((cj3 * new_r10) + ((-1.0) * x606) + ((-1.0) * new_r00 * x602));
                              evalcond[4] = ((cj3 * new_r11) + ((-1.0) * x604) + ((-1.0) * new_r01 * x602));
                              evalcond[5] = ((x597 * x600) + new_r01 + (sj3 * x598));
                              evalcond[6] = ((new_r10 * sj3) + x599 + ((-1.0) * cj4 * x604));
                              evalcond[7] = (((-1.0) * x600 * x604) + x605 + new_r00);
                              evalcond[8] = (((-1.0) * cj3 * x604) + (x597 * x601) + new_r11);
                              evalcond[9] = (((-1.0) * cj3 * x606) + new_r10 + ((-1.0) * x601 * x604));
                              evalcond[10] = ((new_r11 * x601) + x597 + (new_r01 * x600) + ((-1.0) * new_r21 * x603));
                              evalcond[11] = (((-1.0) * new_r20 * x603) + (new_r10 * x601) + ((-1.0) * x604) + (cj4 * x599));
                              if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH ||
                                  IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                                  IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[8]) > IKFAST_EVALCOND_THRESH ||
                                  IKabs(evalcond[9]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[10]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[11]) > IKFAST_EVALCOND_THRESH) {
                                continue;
                              }
                            }

                            {
                              std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                              vinfos[0].jointtype = 1;
                              vinfos[0].foffset = j0;
                              vinfos[0].indices[0] = _ij0[0];
                              vinfos[0].indices[1] = _ij0[1];
                              vinfos[0].maxsolutions = _nj0;
                              vinfos[1].jointtype = 1;
                              vinfos[1].foffset = j1;
                              vinfos[1].indices[0] = _ij1[0];
                              vinfos[1].indices[1] = _ij1[1];
                              vinfos[1].maxsolutions = _nj1;
                              vinfos[2].jointtype = 1;
                              vinfos[2].foffset = j2;
                              vinfos[2].indices[0] = _ij2[0];
                              vinfos[2].indices[1] = _ij2[1];
                              vinfos[2].maxsolutions = _nj2;
                              vinfos[3].jointtype = 1;
                              vinfos[3].foffset = j3;
                              vinfos[3].indices[0] = _ij3[0];
                              vinfos[3].indices[1] = _ij3[1];
                              vinfos[3].maxsolutions = _nj3;
                              vinfos[4].jointtype = 1;
                              vinfos[4].foffset = j4;
                              vinfos[4].indices[0] = _ij4[0];
                              vinfos[4].indices[1] = _ij4[1];
                              vinfos[4].maxsolutions = _nj4;
                              vinfos[5].jointtype = 1;
                              vinfos[5].foffset = j5;
                              vinfos[5].indices[0] = _ij5[0];
                              vinfos[5].indices[1] = _ij5[1];
                              vinfos[5].maxsolutions = _nj5;
                              std::vector<int> vfree(0);
                              solutions.AddSolution(vinfos, vfree);
                            }
                          }
                        }
                      }
                    }

                  } else {
                    {
                      IkReal j5array[1], cj5array[1], sj5array[1];
                      bool j5valid[1] = {false};
                      _nj5 = 1;
                      CheckValue<IkReal> x607 = IKPowWithIntegerCheck(IKsign(sj4), -1);
                      if (!x607.valid)
                        continue;
                      CheckValue<IkReal> x608 = IKatan2WithCheck(IkReal(new_r21), IkReal((-1.0) * new_r20), IKFAST_ATAN2_MAGTHRESH);
                      if (!x608.valid)
                        continue;
                      j5array[0] = ((-1.5707963267949) + ((1.5707963267949) * (x607.value)) + (x608.value));
                      sj5array[0] = IKsin(j5array[0]);
                      cj5array[0] = IKcos(j5array[0]);
                      if (j5array[0] > IKPI)
                        j5array[0] -= IK2PI;
                      else if (j5array[0] < -IKPI)
                        j5array[0] += IK2PI;
                      j5valid[0] = true;
                      for (int ij5 = 0; ij5 < 1; ++ij5) {
                        if (!j5valid[ij5])
                          continue;
                        _ij5[0] = ij5;
                        _ij5[1] = -1;
                        for (int iij5 = ij5 + 1; iij5 < 1; ++iij5) {
                          if (j5valid[iij5] && IKabs(cj5array[ij5] - cj5array[iij5]) < IKFAST_SOLUTION_THRESH && IKabs(sj5array[ij5] - sj5array[iij5]) < IKFAST_SOLUTION_THRESH) {
                            j5valid[iij5] = false;
                            _ij5[1] = iij5;
                            break;
                          }
                        }
                        j5 = j5array[ij5];
                        cj5 = cj5array[ij5];
                        sj5 = sj5array[ij5];
                        {
                          IkReal evalcond[12];
                          IkReal x609 = IKsin(j5);
                          IkReal x610 = IKcos(j5);
                          IkReal x611 = (cj3 * new_r00);
                          IkReal x612 = (cj3 * cj4);
                          IkReal x613 = (cj4 * sj3);
                          IkReal x614 = ((1.0) * sj3);
                          IkReal x615 = ((1.0) * sj4);
                          IkReal x616 = ((1.0) * x610);
                          IkReal x617 = (sj3 * x609);
                          IkReal x618 = ((1.0) * x609);
                          evalcond[0] = ((sj4 * x610) + new_r20);
                          evalcond[1] = (((-1.0) * x609 * x615) + new_r21);
                          evalcond[2] = ((new_r11 * sj3) + (cj3 * new_r01) + (cj4 * x609));
                          evalcond[3] = (((-1.0) * x618) + (cj3 * new_r10) + ((-1.0) * new_r00 * x614));
                          evalcond[4] = (((-1.0) * x616) + (cj3 * new_r11) + ((-1.0) * new_r01 * x614));
                          evalcond[5] = ((x609 * x612) + (sj3 * x610) + new_r01);
                          evalcond[6] = ((new_r10 * sj3) + ((-1.0) * cj4 * x616) + x611);
                          evalcond[7] = (((-1.0) * x612 * x616) + x617 + new_r00);
                          evalcond[8] = (((-1.0) * cj3 * x616) + (x609 * x613) + new_r11);
                          evalcond[9] = (((-1.0) * cj3 * x618) + ((-1.0) * x613 * x616) + new_r10);
                          evalcond[10] = (((-1.0) * new_r21 * x615) + (new_r11 * x613) + x609 + (new_r01 * x612));
                          evalcond[11] = ((new_r10 * x613) + ((-1.0) * x616) + ((-1.0) * new_r20 * x615) + (cj4 * x611));
                          if (IKabs(evalcond[0]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[1]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[2]) > IKFAST_EVALCOND_THRESH ||
                              IKabs(evalcond[3]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[4]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[5]) > IKFAST_EVALCOND_THRESH ||
                              IKabs(evalcond[6]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[7]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[8]) > IKFAST_EVALCOND_THRESH ||
                              IKabs(evalcond[9]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[10]) > IKFAST_EVALCOND_THRESH || IKabs(evalcond[11]) > IKFAST_EVALCOND_THRESH) {
                            continue;
                          }
                        }

                        {
                          std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(6);
                          vinfos[0].jointtype = 1;
                          vinfos[0].foffset = j0;
                          vinfos[0].indices[0] = _ij0[0];
                          vinfos[0].indices[1] = _ij0[1];
                          vinfos[0].maxsolutions = _nj0;
                          vinfos[1].jointtype = 1;
                          vinfos[1].foffset = j1;
                          vinfos[1].indices[0] = _ij1[0];
                          vinfos[1].indices[1] = _ij1[1];
                          vinfos[1].maxsolutions = _nj1;
                          vinfos[2].jointtype = 1;
                          vinfos[2].foffset = j2;
                          vinfos[2].indices[0] = _ij2[0];
                          vinfos[2].indices[1] = _ij2[1];
                          vinfos[2].maxsolutions = _nj2;
                          vinfos[3].jointtype = 1;
                          vinfos[3].foffset = j3;
                          vinfos[3].indices[0] = _ij3[0];
                          vinfos[3].indices[1] = _ij3[1];
                          vinfos[3].maxsolutions = _nj3;
                          vinfos[4].jointtype = 1;
                          vinfos[4].foffset = j4;
                          vinfos[4].indices[0] = _ij4[0];
                          vinfos[4].indices[1] = _ij4[1];
                          vinfos[4].maxsolutions = _nj4;
                          vinfos[5].jointtype = 1;
                          vinfos[5].foffset = j5;
                          vinfos[5].indices[0] = _ij5[0];
                          vinfos[5].indices[1] = _ij5[1];
                          vinfos[5].maxsolutions = _nj5;
                          std::vector<int> vfree(0);
                          solutions.AddSolution(vinfos, vfree);
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}
#endif // __CCONVERTED__