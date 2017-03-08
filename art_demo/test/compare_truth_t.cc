#include "nusimdata/SimulationBase/MCParticle.h"
#include "nusimdata/SimulationBase/MCTruth.h"

#include "compare.hh"

#include <cassert>

int main()
{
  using namespace simb;
  MCParticle nu(0, 14, { });
  MCParticle lep(1, -13, { });
  MCTruth t1, t2, t3;
  assert(t1 == t2);
  t2.SetOrigin(kUnknown);
  assert(t1 == t2);
  t2.Add(nu);
  assert(t2 != t1);
  t1.Add(nu);
  assert(t1 == t2);
  t1.Add(lep);
  assert(t1 != t2);
  t2.Add(nu);
  assert(t2 != t1);
  t1.SetOrigin(kBeamNeutrino);
  assert(t1 != t2);
  t2.SetOrigin(kBeamNeutrino);
  assert(t2 != t1);
  t2.SetOrigin(kSuperNovaNeutrino);
  assert(t2 != t1);
  t1 = t2;
  assert(t1 == t2);
  t3 = t1;
  t1.SetNeutrino(1, 1, 2, 1, 1, 1, 1.0, 2.0, 1.0, 1.0);
  assert(t1 != t2);
  t2.SetNeutrino(1, 1, 2, 1, 1, 1, 1.0, 2.0, 1.0, 1.0);
  assert(t2 == t1);
  t3.SetNeutrino(1, 1, 2, 1, 1, 1, 1.0, 3.0, 1.0, 1.0);
  assert(t3 != t1);
}
