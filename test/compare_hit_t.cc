#include "lardataobj/MCBase/MCHit.h"

#include "compare.hh"

#include <cassert>

int main()
{
  using namespace sim;
  using std::swap;
  MCHit h1, h2;
  assert(h2 == h1);
  h2.SetCharge(kINVALID_FLOAT, kINVALID_FLOAT);
  assert(h2 == h1);
  h2.SetCharge(1.0, kINVALID_FLOAT);
  assert(h2 != h1);
  h1.SetCharge(kINVALID_FLOAT, 1.0);
  assert(h2 != h1);
  h1.Reset();
  assert(h2 != h1);
  h2.Reset();
  assert(h2 == h1);
  h2.SetTime(1.0, kINVALID_FLOAT);
  assert(h2 != h1);
  h2.SetTime(kINVALID_FLOAT, 1.0);
  assert(h2 != h1);
  h2.Reset();
  float v[3] = { kINVALID_FLOAT, kINVALID_FLOAT, kINVALID_FLOAT };
  h2.SetParticleInfo(v, kINVALID_FLOAT, 28);
  assert(h2 != h1);
  h2.SetParticleInfo(v, kINVALID_FLOAT, kINVALID_INT);
  assert(h2 == h1);
  h2.SetParticleInfo(v, 1.0, kINVALID_INT);
  assert(h2 != h1);
  v[0] = 1.0;
  h2.SetParticleInfo(v, kINVALID_FLOAT, kINVALID_INT);
  assert(h2 != h1);
  swap(v[0], v[1]);
  h2.SetParticleInfo(v, kINVALID_FLOAT, kINVALID_INT);
  assert(h2 != h1);
  swap(v[1], v[2]);
  h2.SetParticleInfo(v, kINVALID_FLOAT, kINVALID_INT);
  assert(h2 != h1);
}
