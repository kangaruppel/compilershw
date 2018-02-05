int g;
#include <stdio.h>
int g_incr (int c)
{
  g += c;
  return g;
}
int loop (int a, int b, int c)
{
  int i;
  int ret = 0;
  a += g_incr (c);
  for (i = a; i < b; i++) {
   g_incr (c);
  }
  printf("test\r\n");
  return ret + g;
}

