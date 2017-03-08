#include "lardataobj/RecoBase/Vertex.h"

#include "compare.hh"

#include <cassert>

int main()
{
  using namespace recob;
  double f[3] = {5.7, 5.8, 5.9 };
  Vertex v1(f, 27);
  auto v2 = v1;
  assert(v2 == v1);
  Vertex v3(f, 31);
  assert(v3 != v1);
  f[2] = 6.1;
  Vertex v4(f, 27);
  assert(v4 != v1);
  assert(v4 != v3);
}
