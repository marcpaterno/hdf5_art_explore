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
  vector<string> filenames { "dune.root" }; // multiple files are allowed.


  // The gallery::Event object acts as a cursor into the stream of events.
  // A newly-constructed gallery::Event is at the start if its stream.
  // Use gallery::Event::atEnd() to check if you've reached the end of the stream.
  // Use gallery::Event::next() to go to the next event.

  for (gallery::Event ev(filenames); !ev.atEnd(); ev.next()) {
    // getValidHandle() is preferred to getByLabel(), for both art and
    // gallery use. It does not require in-your-face error handling.
    auto const& mctruths = *ev.getValidHandle<vector<simb::MCTruth>>(mctruths_tag);
    if (!mctruths.empty()) {
      std::cout << mctruths[0].NParticles() << '\n';
    }

    // Uncomment the next two if you need them.
    // using vertices_t = vector<recob::Vertex>;
    // using clusters_t = vector<recob::Cluster>;
    using vertex_cluster_assns_t = art::Assns<recob::Cluster, recob::Vertex, unsigned short>;

    auto const& assns = *ev.getValidHandle<vertex_cluster_assns_t>(assns_tag);
    for (size_t i = 0; i < assns.size(); ++i) {
      // We are relying on the fact that this particlular Assns
      // contains only valid (thus dereferenceable) Ptrs.
      auto const& cluster = *(assns[i].first);
      auto const& vertex  = *(assns[i].second);
      auto data           = assns.data(i);
      // now use cluster and vertex as needed...
      std::cout << cluster.ID() << ", "
                << vertex.ID() << ", "
                << data
                << '\n';
    }
  }
}
