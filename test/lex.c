int func(int a) {
  // valid numbers
  int b = 0x7A8F;
  int b = 0X7a8F;
  int c = 0765;
  int d = 729834;
  // invalid numbers
  int e = 0x9FH;
  int f = 0788;

  if (a == 5) {
    b = b + 2 * c / d;
    if (b != 10) {
      b = b + 15;
    }
  } else {
    b = b % 5;
    if (b < 10) {
      c = c + 5;
    }
  }

  while (a <= 10) {
    a = a + 1;
    if (a > 10) continue;
    if (a >= 8) break;
  }

  do {
    a = (!(~a) | (b)) && (c || (d & e));
  } while (a < 99);

  int f = a ? b : c;

  return 999;
}