/*
 * google1.c - find solution(s) for the equation: wwwdot - google = dotcom
 *             (google labs aptitude test)
 * 
 * Vino Fernando Crescini  <jcrescin@cit.uws.edu.au>
 * 20 Sept 2004
 *
 */

#include <stdio.h>

#define CPS(A0, A1, A2, A3, A4, A5) \
  ((A0 * 100000) + (A1 * 10000) + (A2 * 1000) + (A3 * 100) + (A4 * 10) + A5)

#ifndef SHOW_ALL
  #define CPR(A0, A1) if (A0 == A1) continue
#else
  #define CPR(A0, A1)
#endif

int main()
{
  int i0,i1,i2,i3,i4,i5,i6,i7,i8;
  int count = 0;

  /* w (can't be zero) */
  for (i0 = 1; i0 <= 9; i0++) {
    /* d (can't be zero) */
    for (i1 = 1; i1 <= 9; i1++) {
      CPR(i1, i0);
      /* o */
      for (i2 = 0; i2 <= 9; i2++) {
        CPR(i2, i0);
        CPR(i2, i1);
        /* t */
        for (i3 = 0; i3 <= 9; i3++) {
          CPR(i3, i0);
          CPR(i3, i1);
          CPR(i3, i2);
          /* g (can't be zero */
          for (i4 = 1; i4 <= 9; i4++) {
            CPR(i4, i0);
            CPR(i4, i1);
            CPR(i4, i2);
            CPR(i4, i3);
            /* l */
            for (i5 = 0; i5 <= 9; i5++) {
              CPR(i5, i0);
              CPR(i5, i1);
              CPR(i5, i2);
              CPR(i5, i3);
              CPR(i5, i4);
              /* e */
              for (i6 = 0; i6 <= 9; i6++) {
                CPR(i6, i0);
                CPR(i6, i1);
                CPR(i6, i2);
                CPR(i6, i3);
                CPR(i6, i4);
                CPR(i6, i5);
                /* c */
                for (i7 = 0; i7 <= 9; i7++) {
                  CPR(i7, i0);
                  CPR(i7, i1);
                  CPR(i7, i2);
                  CPR(i7, i3);
                  CPR(i7, i4);
                  CPR(i7, i5);
                  CPR(i7, i6);
                  /* m */
                  for (i8 = 0; i8 <= 9; i8++) {
                    CPR(i8, i0);
                    CPR(i8, i1);
                    CPR(i8, i2);
                    CPR(i8, i3);
                    CPR(i8, i4);
                    CPR(i8, i5);
                    CPR(i8, i6);
                    CPR(i8, i7);
                    /* wwwdot - google = dotcom */
                    if ((CPS(i0, i0, i0, i1, i2, i3) - CPS(i4, i2, i2, i4, i5, i6)) == CPS(i1, i2, i3, i7, i2, i8)) {
                      printf("%6d:\n", ++count);
                      printf("        \'w\' = %d\n", i0);
                      printf("        \'d\' = %d\n", i1);
                      printf("        \'o\' = %d\n", i2);
                      printf("        \'t\' = %d\n", i3);
                      printf("        \'g\' = %d\n", i4);
                      printf("        \'l\' = %d\n", i5);
                      printf("        \'e\' = %d\n", i6);
                      printf("        \'c\' = %d\n", i7);
                      printf("        \'m\' = %d\n", i8);
                      printf("\n");
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
  return 0;
}
