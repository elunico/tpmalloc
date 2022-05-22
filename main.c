#include "malloc.h"
#include "tpsupport.h"

int main()
{
  void* region = malloc(4000);  // order 12
  memset(region, 123, 4000);
  void* smaller = realloc(region, 400);  // order 9
  memset(smaller, 123, 400);
  free(smaller);
  putsd("Done\n");
}
