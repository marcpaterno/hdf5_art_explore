#include "lardataobj/RecoBase/Cluster.h"

#include "compare.hh"

namespace {
  struct simple_cluster_t {
    operator recob::Cluster () const {
      return
        recob::Cluster(start_wire,
                       sigma_start_wire,
                       start_tick,
                       sigma_start_tick,
                       start_charge,
                       start_angle,
                       start_opening,
                       end_wire,
                       sigma_end_wire,
                       end_tick,
                       sigma_end_tick,
                       end_charge,
                       end_angle,
                       end_opening,
                       integral,
                       integral_stddev,
                       summedADC,
                       summedADC_stddev,
                       n_hits,
                       multiple_hit_density,
                       width,
                       ID,
                       view,
                       plane,
                       recob::Cluster::Sentry);
    }

    explicit simple_cluster_t(recob::Cluster::ID_t id = recob::Cluster::InvalidID) :
      start_wire(0.0f),
      sigma_start_wire(0.0f),
      start_tick(0.0f),
      sigma_start_tick(0.f),
      start_charge(0.0f),
      start_angle(0.0f),
      start_opening(0.0f),
      end_wire(0.0f),
      sigma_end_wire(0.0f),
      end_tick(0.0f),
      sigma_end_tick(0.0f),
      end_charge(0.0f),
      end_angle(0.0f),
      end_opening(0.0f),
      integral(0.0f),
      integral_stddev(0.0f),
      summedADC(0.0f),
      summedADC_stddev(0.0f),
      n_hits(0u),
      multiple_hit_density(0.0f),
      width(0.0f),
      ID(id),
      view(geo::kUnknown),
      plane()
      {
      }

    float start_wire;
    float sigma_start_wire;
    float start_tick;
    float sigma_start_tick;
    float start_charge;
    float start_angle;
    float start_opening;
    float end_wire;
    float sigma_end_wire;
    float end_tick;
    float sigma_end_tick;
    float end_charge;
    float end_angle;
    float end_opening;
    float integral;
    float integral_stddev;
    float summedADC;
    float summedADC_stddev;
    unsigned int n_hits;
    float multiple_hit_density;
    float width;
    recob::Cluster::ID_t ID;
    geo::View_t view;
    geo::PlaneID plane;

  };
}

int main()
{
  using namespace recob;
  Cluster c1;
  simple_cluster_t sc1, sc2;
  Cluster c3 { sc1 };
  assert(c1 == c3);
  // Change start_wire.
  sc2.start_wire = 1.0;
  assert(c3 != sc2);
  // Reset and change sigma_start_wire.
  sc2 = sc1;
  assert(c3 == sc2);
  sc2.sigma_start_wire = 1.0;
  assert(c3 != sc2);
  // Reset and change start_tick.
  sc2 = sc1;
  assert(c3 == sc2);
  sc2.start_tick = 1.0;
  assert(c3 != sc2);
  // Reset and change sigma_start_tick.
  sc2 = sc1;
  assert(c3 == sc2);
  sc2.sigma_start_tick = 1.0;
  assert(c3 != sc2);
  // Reset and change start_charge.
  sc2 = sc1;
  assert(c3 == sc2);
  sc2.start_charge = 1.0;
  assert(c3 != sc2);
  // Reset and change start_angle.
  sc2 = sc1;
  assert(c3 == sc2);
  sc2.start_angle = 1.0;
  assert(c3 != sc2);
  // Reset and change start_opening.
  sc2 = sc1;
  assert(c3 == sc2);
  sc2.start_opening = 1.0;
  assert(c3 != sc2);
  // Reset and change end_wire.
  sc2 = sc1;
  assert(c3 == sc2);
  sc2.end_wire = 1.0;
  assert(c3 != sc2);
  // Reset and change sigma_end_wire.
  sc2 = sc1;
  assert(c3 == sc2);
  sc2.sigma_end_wire = 1.0;
  assert(c3 != sc2);
  // Reset and change end_tick.
  sc2 = sc1;
  assert(c3 == sc2);
  sc2.end_tick = 1.0;
  assert(c3 != sc2);
  // Reset and change sigma_end_tick.
  sc2 = sc1;
  assert(c3 == sc2);
  sc2.sigma_end_tick = 1.0;
  assert(c3 != sc2);
  // Reset and change end_charge.
  sc2 = sc1;
  assert(c3 == sc2);
  sc2.end_charge = 1.0;
  assert(c3 != sc2);
  // Reset and change end_angle.
  sc2 = sc1;
  assert(c3 == sc2);
  sc2.end_angle = 1.0;
  assert(c3 != sc2);
  // Reset and change end_opening.
  sc2 = sc1;
  assert(c3 == sc2);
  sc2.end_opening = 1.0;
  assert(c3 != sc2);
  // Reset and change integral.
  sc2 = sc1;
  assert(c3 == sc2);
  sc2.integral = 1.0;
  assert(c3 != sc2);
  // Reset and change integral_stddev.
  sc2 = sc1;
  assert(c3 == sc2);
  sc2.integral_stddev = 1.0;
  assert(c3 != sc2);
  // Reset and change summedADC.
  sc2 = sc1;
  assert(c3 == sc2);
  sc2.summedADC = 1.0;
  assert(c3 != sc2);
  // Reset and change summedADC_stddev.
  sc2 = sc1;
  assert(c3 == sc2);
  sc2.summedADC_stddev = 1.0;
  assert(c3 != sc2);
  // Reset and change n_hits.
  sc2 = sc1;
  assert(c3 == sc2);
  sc2.n_hits = 3;
  assert(c3 != sc2);
  // Reset and change multiple_hit_density.
  sc2 = sc1;
  assert(c3 == sc2);
  sc2.multiple_hit_density = 1.0;
  assert(c3 != sc2);
  // Reset and change width.
  sc2 = sc1;
  assert(c3 == sc2);
  sc2.width = 1.0;
  assert(c3 != sc2);
  // Reset and change ID;
  sc2 = sc1;
  assert(c3 == sc2);
  sc2.ID = 27;
  assert(c3 != sc2);
  // Reset and change view;
  sc2 = sc1;
  assert(c3 == sc2);
  sc2.view = geo::kV;
  assert(c3 != sc2);
  // Reset and change plane;
  sc2 = sc1;
  assert(c3 == sc2);
  sc2.plane = { 1, 1, geo::kV };
  assert(c3 != sc2);
}
