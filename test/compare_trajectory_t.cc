#include "nusimdata/SimulationBase/MCTrajectory.h"

#include "compare.hh"

#include "TLorentzVector.h"

#include <cassert>

int main()
{
  using namespace simb;
  TLorentzVector p(1.1, 2.1, 3.1, 4.1), m(1.2, 2.2, 3.2, 4.2);
  MCTrajectory t1, t2, t3(p, m);
  assert(t2 == t1);
  assert(t3 != t2);
  t2.Add(p, m);
  assert(t2 != t1);
  assert(t2 == t3);
  t1.Add(p, m, "hadElastic");
  assert(t1 != t2);
  t2.Add(p, m, "hadElastic");
  assert(t2 != t3);
  t3.Add(p, m, "hadElastic");
  assert(t2 == t3);
  t2.Add(p, m, "hadElastic");
  assert(t2 != t3);
  t3.Add(p, m, "pi-InElastic");
  assert(t3 != t2);
  t2.Add(p, m);
  p.SetX(p.X() + 1.0);
  t3.Add(p, m);
  assert(t3 != t2);
}
