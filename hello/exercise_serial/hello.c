#include <stdio.h>
/* Include omp.h ? */
int main() {
  int id;
  /* Add Opemp pragma */
  {
    id = /* Get Thread ID */
    if (id%2==1)
      printf("Hello world from thread %d, I am odd\n", id);
    else
      printf("Hello world from thread %d, I am even\n", id);
  }
}
