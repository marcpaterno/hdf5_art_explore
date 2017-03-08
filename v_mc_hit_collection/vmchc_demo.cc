#include <iostream>
#include <string>
#include <vector>

#include "canvas/Utilities/InputTag.h"
#include "canvas/Persistency/Common/Assns.h"
#include "gallery/Event.h"
#include "gallery/ValidHandle.h"
#include "lardataobj/RecoBase/Cluster.h"
#include "lardataobj/RecoBase/Vertex.h"
#include "nusimdata/SimulationBase/MCTruth.h"


using namespace art;
using namespace std;

int main() {

  InputTag mctruths_tag { "generator" };
  InputTag vertex_tag { "linecluster" };
  InputTag assns_tag  { "linecluster" };
  vector<string> filenames { "dune_new.root" }; // multiple files are allowed.


  // The gallery::Event object acts as a cursor into the stream of events.
  // A newly-constructed gallery::Event is at the start if its stream.
  // Use gallery::Event::atEnd() to check if you've reached the end of the stream.
  // Use gallery::Event::next() to go to the next event.

  for (gallery::Event ev(filenames); !ev.atEnd(); ev.next()) {
    // getValidHandle() is preferred to getByLabel(), for both art and
    // gallery use. It does not require in-your-face error handling.
    std::vector<simb::MCTruth> const& mctruths = *ev.getValidHandle<vector<simb::MCTruth>>(mctruths_tag);

    // In C++11 or newer, we recommend use of 'auto' to deduce the
    // type of the value of the expression (here, the return value of
    // the function call and derefencing of the returned ValidHandle<...>
    // object).
    //
    //auto const& mctruths = *ev.getValidHandle<vector<simb::MCTruth>>(mctruths_tag);

    // After the call above, the MCTruth objects have been read into
    // memory, and any of their *const* functions can be invoked. Only
    // const functions can be used, because we have a const reference
    // to the objects. This is part of the design of our object model:
    // objects *read from an Event* are always immutable.

    if (!mctruths.empty()) {
      // This output will not be very interesting if the file you're
      // looking at is the kind of simulation that only contains one
      // particle per MCTruth object.
      std::cout << mctruths[0].NParticles() << '\n';
    }
  }
}
