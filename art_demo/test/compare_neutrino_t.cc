#include "nusimdata/SimulationBase/MCNeutrino.h"

#include "compare.hh"

#include <cassert>

namespace {
  struct simple_neutrino_t {
    operator simb::MCNeutrino() {
      return
        simb::MCNeutrino(nu,
                         lep,
                         ccnc,
                         mode,
                         interactionType,
                         target,
                         nucleon,
                         quark,
                         w,
                         x,
                         y,
                         qsqr);
    }
    simple_neutrino_t() :
      nu(),
      lep(),
      ccnc(std::numeric_limits<int>::min()),
      mode(std::numeric_limits<int>::min()),
      interactionType(std::numeric_limits<int>::min()),
      target(std::numeric_limits<int>::min()),
      nucleon(std::numeric_limits<int>::min()),
      quark(std::numeric_limits<int>::min()),
      w(std::numeric_limits<double>::min()),
      x(std::numeric_limits<double>::min()),
      y(std::numeric_limits<double>::min()),
      qsqr(std::numeric_limits<double>::min())
      {
      }

    simb::MCParticle nu;
    simb::MCParticle lep;
    int ccnc;
    int mode;
    int interactionType;
    int target;
    int nucleon;
    int quark;
    double w;
    double x;
    double y;
    double qsqr;
  };
}

int main()
{
  using namespace simb;
  simple_neutrino_t sn1, sn2;
  MCNeutrino n1, n3(sn1);
  assert(n1 == n3);
  assert(n1 == sn2);
  MCParticle const nu(0, 14, { });
  MCParticle const lep(1, -13, { });
  sn2.nu = nu;
  assert(n1 != sn2);
  sn2 = sn1;
  assert(n1 == sn2);
  sn2.lep = lep;
  assert(n1 != sn2);
  sn2 = sn1;
  assert(n1 == sn2);
  sn2.ccnc = 27;
  assert(n1 != sn2);
  sn2 = sn1;
  assert(n1 == sn2);
  sn2.mode = 2;
  assert(n1 != sn2);
  sn2 = sn1;
  assert(n1 == sn2);
  sn2.interactionType = 2;
  assert(n1 != sn2);
  sn2 = sn1;
  assert(n1 == sn2);
  sn2.target = 2;
  assert(n1 != sn2);
  sn2 = sn1;
  assert(n1 == sn2);
  sn2.nucleon = 2;
  assert(n1 != sn2);
  sn2 = sn1;
  assert(n1 == sn2);
  sn2.quark = 2;
  assert(n1 != sn2);
  sn2 = sn1;
  assert(n1 == sn2);
  sn2.w = 2.0;
  assert(n1 != sn2);
  sn2 = sn1;
  assert(n1 == sn2);
  sn2.x = 2.0;
  assert(n1 != sn2);
  sn2 = sn1;
  assert(n1 == sn2);
  sn2.y = 2.0;
  assert(n1 != sn2);
  sn2 = sn1;
  assert(n1 == sn2);
  sn2.qsqr = 2.0;
  assert(n1 != sn2);
}
