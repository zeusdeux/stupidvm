// gcc -g -c foo.c && gcc -g foo.c -o foo && objdump -S ./foo

typedef struct {
  double x, y, z;
} dummy_t;

double add2(dummy_t val)
{
  return val.x + val.y + val.z;
}

int add(int x, int y)
{
  int c = 2;
  return x + y + 2;
}

int main(void)
{
  int i = add(6, 8);
  /* add2((dummy_t) { .x = 10.0L, .y = 20.0L, .z = 70.0L}); */
  return 9;
}
