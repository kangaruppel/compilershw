#include <stdio.h>

int compute ()
{
  int result = 0;
  int a = 2;
  int b = 3;
  int c = 4 + a + b;
  int d = 5;
  int e;
  d = a * 0;
  //scanf("%d", &d);
  e = 4 * d;
  d = 7 * d;
  result += a;
  result += b;
  result *= c;
  result /= 2;
  result += d;
  return result;
}
