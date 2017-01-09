#include "compare.hh"

// Note all tests are for equality, not equivalence or identity.

////////////////////////////////////
// namespace recob
#include "lardataobj/RecoBase/Vertex.h"
#include "lardataobj/RecoBase/Cluster.h"

bool
recob::operator == (recob::Vertex const & left,
                    recob::Vertex const & right)
{
  using std::CBEGIN;
  using std::CEND;
  return left.ID() == right.ID() &&
    [&]() { double l[3], r[3];
            left.XYZ(l);
            right.XYZ(r);
            return std::equal(CBEGIN(l), CEND(l), CBEGIN(r));
          }();
}

bool
recob::operator == (recob::Cluster const & left,
                    recob::Cluster const & right)
{
  return left.ID() == right.ID() &&
    left.NHits() == right.NHits() &&
    left.StartWire() == right.StartWire() &&
    left.StartTick() == right.StartTick() &&
    left.SigmaStartWire() == right.SigmaStartWire() &&
    left.SigmaStartTick() == right.SigmaStartTick() &&
    left.EndWire() == right.EndWire() &&
    left.EndTick() == right.EndTick() &&
    left.SigmaEndWire() == right.SigmaEndWire() &&
    left.SigmaEndTick() == right.SigmaEndTick() &&
    left.StartCharge() == right.StartCharge() &&
    left.StartAngle() == right.StartAngle() &&
    left.StartOpeningAngle() == right.StartOpeningAngle() &&
    left.EndCharge() == right.EndCharge() &&
    left.EndAngle() == right.EndAngle() &&
    left.EndOpeningAngle() == right.EndOpeningAngle() &&
    left.Integral() == right.Integral() &&
    left.IntegralStdDev() == right.IntegralStdDev() &&
    left.IntegralAverage() == right.IntegralAverage() &&
    left.SummedADC() == right.SummedADC() &&
    left.SummedADCstdDev() == right.SummedADCstdDev() &&
    left.SummedADCaverage() == right.SummedADCaverage() &&
    left.MultipleHitDensity() == right.MultipleHitDensity() &&
    left.Width() == right.Width() &&
    left.View() == right.View() &&
    left.Plane() == right.Plane();
}

#pragma GCC diagnostic ignored "-Wunused-parameter"

////////////////////////////////////
// namespace sim
#include "lardataobj/MCBase/MCHit.h"
#include "lardataobj/MCBase/MCHitCollection.h"

bool
sim::operator == (MCHit const & left,
                  MCHit const & right)
{
  return left.PartTrackId() == right.PartTrackId() &&
    left.PeakTime() == right.PeakTime() &&
    left.PeakWidth() == right.PeakWidth() &&
    left.Charge(false) == right.Charge(false) &&
    left.Charge(true) == right.Charge(true) &&
    left.PartEnergy() == right.PartEnergy() &&
    std::equal(left.PartVertex(), left.PartVertex() + 3, right.PartVertex());
}

bool
sim::operator == (MCHitCollection const & left,
                  MCHitCollection const & right)
{
  return left.Channel() == right.Channel() &&
    static_cast<std::vector<MCHit> >(left) == right;
}

////////////////////////////////////
// namespace simb
#include "nusimdata/SimulationBase/MCTrajectory.h"
#include "nusimdata/SimulationBase/MCParticle.h"
#include "nusimdata/SimulationBase/MCNeutrino.h"
#include "nusimdata/SimulationBase/MCTruth.h"

bool
simb::operator == (MCTrajectory const & left,
                   MCTrajectory const & right)
{
  return left.size() == right.size() &&
    left.TrajectoryProcesses() == right.TrajectoryProcesses() &&
    std::equal(left.begin(), left.end(), right.begin());
}

bool
simb::operator == (MCParticle const & left,
                   MCParticle const & right)
{
  // Comparisons are in an order intended to make an attempt at doing
  // all the cheap checks first.
  return left.StatusCode() == right.StatusCode() &&
    left.TrackId() == right.TrackId() &&
    left.PdgCode() == right.PdgCode() &&
    left.Mother() == right.Mother() &&
    left.Rescatter() == right.Rescatter() &&
    left.Mass() == right.Mass() &&
    left.Weight() == right.Weight() &&
    left.NumberDaughters() == right.NumberDaughters() &&
    left.Process() == right.Process() &&
    left.EndProcess() == right.EndProcess() &&
    left.Polarization() == right.Polarization() &&
    [&](){ bool result = true;
           for (auto sz = left.NumberDaughters(), i = 0;
                i != sz; ++i) {
             if (left.Daughter(i) != right.Daughter(i)) {
               result = false;
               break;
             }
           }
           return result;
         }() &&
    left.GetGvtx() == right.GetGvtx() &&
    left.Trajectory() == right.Trajectory();
}

bool
simb::operator == (MCNeutrino const & left,
                   MCNeutrino const & right)
{
  // Comparisons are in an order intended to make an attempt at doing
  // all the cheap checks first.
  return left.Mode() == right.Mode() &&
    left.InteractionType() == right.InteractionType() &&
    left.CCNC() == right.CCNC() &&
    left.Target() == right.Target() &&
    left.HitNuc() == right.HitNuc() &&
    left.HitQuark() == right.HitQuark() &&
    left.W() == right.W() &&
    left.X() == right.X() &&
    left.Y() == right.Y() &&
    left.QSqr() == right.QSqr() &&
    left.Nu() == right.Nu() &&
    left.Lepton() == right.Lepton();
}

bool
simb::operator == (MCTruth const & left,
                   MCTruth const & right)
{
  // Comparisons are in an order intended to make an attempt at doing
  // all the cheap checks first.
  return left.NeutrinoSet() == right.NeutrinoSet() &&
    left.Origin() == right.Origin() &&
    left.NParticles() == right.NParticles() &&
    left.GetNeutrino() == right.GetNeutrino() &&
    [&](){ bool result = true;
           for (auto sz = left.NParticles(), i = 0;
                i != sz; ++i) {
             if (left.GetParticle(i) != right.GetParticle(i)) {
               result = false;
               break;
             }
           }
           return result;
         }();
}
