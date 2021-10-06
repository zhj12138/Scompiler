int a = 7;
int b = 10;
int main() {
  {
    int a = 999;
    b = a;
  }
  a = 10;
  return b;
}