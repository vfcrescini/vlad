/* 
 * Vino Crescini
 * jvcrescini@yahoo.com
 * Gauss-Seidel Iteration
 * Code borrowed from John Mathews
 *
 * Input file must be in the following format (ax = b):
 * n
 * a1,1<tab>a1,2<tab>...<tab>a1,n<tab>b1<newline>
 * a2,1<tab>a2,2<tab>...<tab>a2,n<tab>b2<newline>
 * .
 * .
 * .
 * an,1<tab>an,2<tab>...<tab>an,n<tab>bn<newline>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define TOLERANCE     10E-6
#define MAX_ITERATION 1024

int get_input(const char *filename, double ***a, double **b, unsigned int *n);
int check_array(double **a, double *b, unsigned int n);
int solve_matrix(double **a, double *b, double **p, unsigned int n);
void dump_output(double *p, unsigned int n);
void free_array_1(double *a, unsigned int n);
void free_array_2(double **a, unsigned int n);

int main(int argc, char *argv[])
{
  unsigned int n;
  double **a;
  double *b;
  double *p;

  /* check parameter */
  if (argc != 2) {
    fprintf(stderr, "Usage %s filename\n", argv[0]);
    return -1;
  }

  /* read input from file */
  switch(get_input(argv[1], &a, &b, &n)) {
    case -1 :
      fprintf(stderr, "Argument must be a filename\n");
      return -1;
    case -2 :
      fprintf(stderr, "Cannot open %s for reading\n", argv[1]);
      return -2;
    case -3 :
      fprintf(stderr, "Incorrect file format\n");
      return -3;
    case -4 :
      fprintf(stderr, "Malloc error\n");
      return -4;
  }
  
  /* check and solve */
  if (check_array(a, b, n) != 0) 
    fprintf(stderr, "The matrix is not diagonally-dominant\n");
  else if (solve_matrix(a, b, &p, n) != 0)
    fprintf(stderr, "Failed\n");
  else
    dump_output(p, n);
    
  /* cleanup */
  free_array_1(p, n);
  free_array_1(b, n);
  free_array_2(a, n);
  
  return 0;
}

/* gives a pointer to an nxn matrix (a) and an n-array (b) 
 * returns :
 *   0 success
 *  -1 input error 
 *  -2 file open error 
 *  -3 incorrect file format 
 *  -4 memory allocation error */
int get_input(const char *filename, double ***a, double **b, unsigned int *n)
{
  unsigned int r;
  unsigned int c;
  char dummy;
  FILE *in_file;
  
  if (filename == NULL || a == NULL || b == NULL || n == NULL)
    return -1;

  if (!strcmp(filename, "-"))
    in_file = stdin;
  else if ((in_file = fopen(filename, "r")) == NULL)
    return -2;

  /* read value of n */
  if (fscanf(in_file, "%d\n", n) != 1)
    return -3;

  /* allocate space for the arrays */
  *a = (double **) malloc(sizeof(double *) * *n);
  *b = (double *) malloc(sizeof(double) * *n);

  if (*a == NULL || *b == NULL)
    return -4;

  /* read file and load onto arrays, one row at a time */
  for (r = 0; r < *n; r++) {
    /* we must allocate memory for every row */
    if (((*a)[r] = (double *) malloc(sizeof(double) * *n)) == NULL)
      return -4;

    /* first n numbers in a line gets loaded in a */
    for (c = 0; c < *n; c++) {
      /* get the number */
      if (fscanf(in_file, "%lf", &((*a)[r][c])) != 1)
        return -3;
      /* consume delimiter (tab) */
      if (fscanf(in_file, "%c", &dummy) != 1 || dummy != '\t')
        return -3;
    }
    /* last number in a line (n + 1) goes to b */
    if (fscanf(in_file, "%lf", &((*b)[r])) != 1)
      return -3;
    /* let's not forget the newline char */
    if (fscanf(in_file, "%c", &dummy) != 1 || dummy != '\n')
      return -3;
  }

  fclose(in_file);
    
  return 0;
}

/* check for diagonal dominance; return 0 if failed */
int check_array(double **a, double *b, unsigned int n)
{
  unsigned int r;
  unsigned int c;
  double row;

  for (r = 0; r < n; r++) {
    row = 0.0;
    for (c = 0; c < n; c++)
      row += fabs(a[r][c]);
    if (row >= 2.0 * fabs(a[r][r]))
     return -1;
  }

  return 0;
}

/* solve using gauss-seidel method, returns zero if failed, puts result in p */
int solve_matrix(double **a, double *b, double **p, unsigned int n)
{
  double *p_old;
  double sep = 1.0;
  double sum;
  unsigned int r;
  unsigned int c;
  unsigned int count = 1;

  if (a == NULL || b == NULL || p == NULL)
    return -1;

  /* allocate space */
  *p = (double *) malloc(sizeof(double) * n);
  p_old = (double *) malloc(sizeof(double) * n);

  if (*p == NULL || p_old == NULL)
    return -1;

  /* initialise */
  for (r = 0; r < n; r++) {
    (*p)[r] = 0.0;
    p_old[r] = 0.0;
  }

  /* perform the gauss-seidel iteration */
  while (count++ < MAX_ITERATION && sep > TOLERANCE) {
    for (r = 0; r < n; r++) {
      sum = b[r];
      for (c = 0; c < n; c++)
        if (c != r)
	  sum -= a[r][c] * (*p)[c];
      (*p)[r] = sum / a[r][r];
    }
    
    /* convergence criterion */
    sep = 0.0;
    for (r = 0; r < n; r++) {
      sep += fabs((*p)[r] - p_old[r]);
      p_old[r] = (*p)[r];
    }
  }
  return (sep < TOLERANCE) ? 0 : -1;
}

void dump_output(double *p, unsigned int n)
{
  unsigned int i;
  
  if (p != NULL) {
    for (i = 0; i < n; i++)
      printf("p[%d] = %f\n", i, p[i]);
  }
}

/* frees the memory allocated for the one-dimension array */
void free_array_1(double *a, unsigned int n)
{
  if (a != NULL)
    free(a);
}

/* frees the memory allocated for the two-dimension array */
void free_array_2(double **a, unsigned int n)
{
  unsigned int i;
  
  if (a != NULL) {
    for (i = 0; i < n; i++)
      free(a[i]);

    free(a);
  }
}
