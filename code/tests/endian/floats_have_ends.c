/* Test of float endianess, posted in a comment on this thread:
 *
 * http://betterexplained.com/articles/understanding-big-and-little-endian-byte-order/
 *
 * On a little-endian machine, this will swap the sign of the float, with this result:
 * float: 3.141590
 * byte array: D0:F:49:40
 * float: -3.141590
 * byte array: D0:F:49:C0
 *
 * On a big-endian machine, a[0] should be tweaked instead of a[3] for the same effect:
 * float: 3.141590
 * byte array: 40:49:F:D0
 * float: -3.141590
 * byte array: C0:49:F:D0
 */

#include <stdio.h>

int main(int argc, char** argv){
  char *a;
  float f = 3.14159;  // number to start with
 
  a = (char *)&f;   // point a to f's location

  // print float & byte array as hex
  printf("float: %f\n", f);
  printf("byte array: %hhX:%hhX:%hhX:%hhX\n", \
    a[0], a[1], a[2], a[3]); 

  // toggle the sign of f -- using the byte array
  a[3] = ((unsigned int)a[3]) ^ 128;

  //print the numbers again
  printf("float: %f\n", f);
  printf("byte array: %hhX:%hhX:%hhX:%hhX\n", \
    a[0], a[1], a[2], a[3]); 

  return 0;
}
