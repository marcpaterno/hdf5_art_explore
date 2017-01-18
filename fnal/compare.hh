#ifndef COMPARE_HH
#define COMPARE_HH
////////////////////////////////////////////////////////////////////////
// compare.hh
//
// Comparators for various data products.
//
// Note all tests are for equality, not identity or equivalence.
//
////////////////////////////////////////////////////////////////////////
#include "canvas/Persistency/Common/Assns.h"
#include "cetlib/compiler_macros.h"

#include <algorithm>
#include <iterator>
#include <type_traits>

#ifdef CET_NO_STD_CBEGIN_CEND
#define CBEGIN begin
#define CEND end
#else
#define CBEGIN cbegin
#define CEND cend
#endif

namespace art {
  template <typename A, typename B>
  bool operator == (Assns<A, B> const & left,
                    Assns<A, B> const & right);

  template <typename A, typename B>
  bool operator != (Assns<A, B> const & left,
                    Assns<A, B> const & right);


  template <typename A, typename B, typename D>
  typename std::enable_if<!std::is_void<D>::value, bool>::type
  operator == (Assns<A, B, D> const & left,
               Assns<A, B, D> const & right);

  template <typename A, typename B, typename D>
  typename std::enable_if<!std::is_void<D>::value, bool>::type
  operator != (Assns<A, B, D> const & left,
               Assns<A, B, D> const & right);
}

namespace recob {
  class Vertex;
  class Cluster;

  bool
  operator == (Vertex const & left,
               Vertex const & right);

  bool
  operator != (Vertex const & left,
               Vertex const & right);

  bool
  operator == (Cluster const & left,
               Cluster const & right);

  bool
  operator != (Cluster const & left,
               Cluster const & right);
}

namespace sim {
  class MCHit;
  class MCHitCollection;

  bool
  operator == (MCHit const & left,
               MCHit const & right);

  bool
  operator != (MCHit const & left,
               MCHit const & right);

  bool
  operator == (MCHitCollection const & left,
               MCHitCollection const & right);

  bool
  operator != (MCHitCollection const & left,
               MCHitCollection const & right);
}

namespace simb {
  class MCTrajectory;
  class MCParticle;
  class MCNeutrino;
  class MCTruth;

  bool
  operator == (MCTrajectory const & left,
               MCTrajectory const & right);

  bool
  operator != (MCTrajectory const & left,
               MCTrajectory const & right);

  bool
  operator == (MCParticle const & left,
               MCParticle const & right);

  bool
  operator != (MCParticle const & left,
               MCParticle const & right);

  bool
  operator == (MCNeutrino const & left,
               MCNeutrino const & right);

  bool
  operator != (MCNeutrino const & left,
               MCNeutrino const & right);

  bool
  operator == (MCTruth const & left,
               MCTruth const & right);

  bool
  operator != (MCTruth const & left,
               MCTruth const & right);
}

////////////////////////////////////////////////////////////////////////
// Template and inline implementations.

////////////////////////////////////
// namespace art

template <typename A, typename B>
inline
bool art::operator == (Assns<A, B> const & left,
                       Assns<A, B> const & right)
{
  using std::CBEGIN;
  using std::CEND;
  return left.size() == right.size() &&
    std::equal(CBEGIN(left), CEND(left), CBEGIN(right));
}

template <typename A, typename B>
inline
bool art::operator != (Assns<A, B> const & left,
                  Assns<A, B> const & right)
{
  return !(left == right);
}

template <typename A, typename B, typename D>
typename std::enable_if<!std::is_void<D>::value, bool>::type
art::operator == (Assns<A, B, D> const & left,
                  Assns<A, B, D> const & right)
{
  if (left.size() != right.size()) return false;
  for (auto i = 0UL, sz = left.size(); i != sz; ++i) {
    if (!(left.data(i) == right.data(i) &&
          left[i] == right[i])) {
      return false;
    }
  }
  return true;
}

template <typename A, typename B, typename D>
inline
typename std::enable_if<!std::is_void<D>::value, bool>::type
art::operator != (Assns<A, B, D> const & left,
                  Assns<A, B, D> const & right)
{
  return !(left == right);
}

////////////////////////////////////
// namespace recob
inline
bool
recob::operator != (recob::Vertex const & left,
                    recob::Vertex const & right)
{
  return !(left == right);
}

inline
bool
recob::operator != (recob::Cluster const & left,
                    recob::Cluster const & right)
{
  return !(left == right);
}

////////////////////////////////////
// namespace sim

inline
bool
sim::operator != (MCHit const & left,
                  MCHit const & right)
{
  return !(left == right);
}

inline
bool
sim::operator != (MCHitCollection const & left,
                  MCHitCollection const & right)
{
  return !(left == right);
}

////////////////////////////////////
// namespace simb

inline
bool
simb::operator != (MCTrajectory const & left,
                   MCTrajectory const & right)
{
  return !(left == right);
}

inline
bool
simb::operator != (MCParticle const & left,
                   MCParticle const & right)
{
  return !(left == right);
}

inline
bool
simb::operator != (MCNeutrino const & left,
                   MCNeutrino const & right)
{
  return !(left == right);
}

inline
bool
simb::operator != (MCTruth const & left,
                   MCTruth const & right)
{
  return !(left == right);
}


#endif /* COMPARE_HH */
