int main() {
  int x = 1;
  {
    x = 2;
    int x = 3;
  }
  x = 4;
  return x;
}