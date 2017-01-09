#include "lardataobj/MCBase/MCHit.h"
#include "lardataobj/MCBase/MCHitCollection.h"

#include "compare.hh"

#include <cassert>

int main()
{
  using namespace sim;
  MCHit h1;
  MCHitCollection c1, c2, c3(28);
  assert(c2 == c1);
  assert(c3 != c2);
  c1.push_back(h1);
  assert (c2 != c1);
  c2.push_back(h1);
  assert(c2 == c1);
  c1.push_back(h1);
  assert(c2 != c1);
  c2.push_back(h1);
  assert(c2 == c1);
  c1.push_back(h1);
  h1.SetCharge(1.0, 1.0);
  c2.push_back(h1);
  assert(c2 != c1);
}
