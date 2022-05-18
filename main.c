#include "tpsupport.h"
#include "malloc.h"


int main()
{
  int* a = malloc(sizeof(int) * 1000);
  for (int i = 0; i < 1000; i++)
    a[i] = i + 100;
  free(a);

  a = malloc(sizeof(int) * 10000);
  for (int i = 0; i < 10000; i++)
    a[i] = i + 100;
  free(a);
}
