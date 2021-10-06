int a[10];
int b[2];

int main() {
  {
    int a[5];
    a[1] = 3;
    b[1] = a[1];
  }
  b[0] = a[2];
  a[1] = 4;
  return a[1];
}
