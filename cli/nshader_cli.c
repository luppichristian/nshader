#include <assert.h>
#include <nshader.h>

int main(int argc, char** argv) {
  bool a = nshader_test();
  assert(a);

  // CLI application code goes here
  return 0;
}