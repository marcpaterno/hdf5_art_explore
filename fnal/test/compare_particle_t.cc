#include "nusimdata/SimulationBase/MCParticle.h"

#include "compare.hh"

#include "TLorentzVector.h"

#include <cassert>

int main()
{
  using namespace simb;
  using namespace std::string_literals;
  MCParticle p1, p2;
  assert(p1 == p2);
  p1 = MCParticle(MCParticle::s_uninitialized,
                  MCParticle::s_uninitialized,
                  { });
  p2 = MCParticle(MCParticle::s_uninitialized,
                  MCParticle::s_uninitialized,
                  { });
  assert(p1 == p2);
  p2 = MCParticle(0,
                  MCParticle::s_uninitialized,
                  { });
  assert(p2 != p1);
  p1 = MCParticle(1,
                  MCParticle::s_uninitialized,
                  { });
  assert(p1 != p2);
  p1 = MCParticle(0,
                  -13,
                  { });
  assert(p1 != p2);
  p1 = MCParticle(0,
                  MCParticle::s_uninitialized,
                  "x"s);
  assert(p1 != p2);
  p1 = MCParticle(0,
                  -13,
                  { }, 0);
  p2 = MCParticle(0,
                  -13,
                  { }, 1);
  assert(p2 != p1);
  p2 = MCParticle(0,
                  -13,
                  { }, 0, 77.0);
  assert(p2 != p1);
  p2 = MCParticle(0,
                  -13,
                  { }, 0, -1, 1);
  assert(p2 == p1);
  p2.SetEndProcess("decay"s);
  assert(p2 != p1);
  p1.SetEndProcess("decay"s);
  assert(p2 == p1);
  p1.AddDaughter(12);
  assert(p1 != p2);
  p2.AddDaughter(12);
  assert(p2 == p1);
  TLorentzVector p(1.1, 2.1, 3.1, 4.1), m(1.2, 2.2, 3.2, 4.2);
  p1.AddTrajectoryPoint(p, m);
  assert(p1 != p2);
  p2.AddTrajectoryPoint(p, m);
  assert(p2 == p1);
  p1.AddTrajectoryPoint(p, m);
  assert(p1 != p2);
  p2.AddTrajectoryPoint(p, m, "hadElastic");
  assert(p2 != p1);
  p2 = p1;
  assert(p2 == p1);
  p2.SetGvtx(0.1, 0.2, 0.3, 0.4);
  assert(p2 != p1);
  p1.SetGvtx(0.1, 0.2, 0.3, 0.4);
  assert(p1 == p2);
  p1.SetGvtx(0.1, 0.2, 0.3, 0.5);
  assert(p1 != p2);
  p2 = p1;
  p2.SetRescatter(2);
  assert(p2 != p1);
  p1.SetRescatter(2);
  assert(p1 == p2);
}
