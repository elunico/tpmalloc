#include "malloc.h"
#include "tpsupport.h"

//#include "image.h"

#ifdef TPMALLOC_SPECIAL_START
int _start()
#else
int main()
#endif
{
  //  int* a = malloc(sizeof(int) * 1000);
  //  for (int i = 0; i < 1000; i++)
  //    a[i] = i + 100;
  //  free(a);
  //
  //  a = malloc(sizeof(int) * 10000);
  //  for (int i = 0; i < 10000; i++)
  //    a[i] = i + 100;
  //  free(a);

  //  image_t image;
  //  I_init_image(&image, 200, 200);
  //  I_fill_image_pixels(&image, C_make_color(200, 0, 0));
  //  color_t pixel = I_get_pixel(&image, 145, 198);
  //  printf("Pixel at 145, 198 = (%d, %d, %d)\n", pixel.red, pixel.green, pixel.blue);
  //  I_fini_image(&image);

//  image_t images[45];
//  for (int i = 0; i < 45; i++) {
//    image_t* image = &(images[i]);
//    I_init_image(image, 200, 200);
//    if (image->pixels == NULL) {
//      putsd("Error out of memory\n");
//      abort();
//    }
//    I_fill_image_pixels(image, C_make_color(200, 0, 0));
//    color_t pixel = I_get_pixel(image, 145, 198);
//    putsd("Pixel at 145, 198 = (");
//    puti(pixel.red);
//    putsd(", ");
//    puti(pixel.green);
//    putsd(", ");
//    puti(pixel.blue);
//    putsd(")\n");
//  }
//  for (int i = 0; i < 45; i++) {
//    //    printf("Freeing %p\n", &(images[i]));
//    I_fini_image(&(images[i]));
//  }

  void* region = malloc(4000); // order 12
  memset(region, 123, 4000);
  void* smaller = realloc(region, 400); // order 9
  memset(smaller, 123, 400);
  free(smaller);
  putsd("Done\n");
}
