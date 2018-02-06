int compute (int a, int b)
{
  int result = 0;

  result += 2 * a;
  result += a * 3;
  result += a * 16;
  result += 8 * a;
  result -= b / 2;
  result -= b / 4;
  result -= b / 8;
  return result;
}
