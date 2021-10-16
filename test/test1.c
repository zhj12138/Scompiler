int a;
int b[2];
int c[2][4];

int func(int n) {
  int d[3];
  d[1] = n;
  a = d[1];
  if (a == 2) {
    b[1] = 3;
  } else {
    b[1] = 5;
  }
  return 0;
}

int main() {
  int c;
  func(2);
  if (b[1] == 3) {
    c = 4;
  } else {
    c = 0;
  }
  return c;
}