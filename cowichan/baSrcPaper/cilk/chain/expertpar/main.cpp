/*
 * chain: chain all problems
 *
 * input:
 *   nelts: the number of elements
 *   randmat_seed: random number generator of cells to retain
 *   thresh_percent: percentage of cells to retain
 *   winnow_nelts: the number of points to select
 *
 * output:
 *   result: a real vector, whose values are the result of the final product
 */
#include <cilk/cilk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common.h"

int is_bench = 0;

extern double *product_result;

int main(int argc, char** argv) {
  int nelts, randmat_seed, thresh_percent, winnow_nelts, i;

  if (argc >= 2) {
    for (int a = 0; a < argc; a++){
      if (!strcmp(argv[a], "--is_bench")) {
        is_bench = 1;
      }
    }
  }

  scanf("%d%d%d%d", &nelts, &randmat_seed, &thresh_percent, &winnow_nelts);
  
  randmat(nelts, nelts, randmat_seed); cilk_sync;
  thresh(nelts, nelts, thresh_percent); cilk_sync;
  winnow(nelts, nelts, winnow_nelts); cilk_sync;
  outer(winnow_nelts); cilk_sync;
  product(winnow_nelts); cilk_sync;

  if (!is_bench) {
    for (i = 0; i < winnow_nelts; i++) {
      printf("%g ", product_result[i]);
    }
    printf("\n");
  }

  return 0;
}

