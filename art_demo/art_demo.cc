#include <chrono>
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
using namespace std::chrono;
using namespace std::string_literals;

int main(int argc, char** argv) {
  // When this gets updated to also read an HDF5 file, we should
  // probably be set up to always expect argc == 3, and thus two
  // filenames.
  if (argc == 1) {
    cout << "Please specify the name of one or more art/ROOT input file(s) to "
      "read.\n";
    return 1;
  }

  InputTag mctruths_tag { "generator" };
  // This reads all the command-line arguments into a vector of
  // strings. We don't care about argv[0], the program name.
  vector<string> filenames(argv + 1, argv + argc);

  // The gallery::Event object acts as a cursor into the stream of events.
  // A newly-constructed gallery::Event is at the start if its stream.
  // Use gallery::Event::atEnd() to check if you've reached the end of the stream.
  // Use gallery::Event::next() to go to the next event.

  vector<microseconds> root_times; // times for reading ROOT
  vector<microseconds> hdf_times;  // times for reading HDF5

  for (gallery::Event ev(filenames); !ev.atEnd(); ev.next()) {
    // Do moving on to the next event in the HDF5 file here, before
    // the first timing call.
    auto const t0 = system_clock::now();
    auto  const&  mctruths [[gnu::unused]] =
      *ev.getValidHandle<vector<simb::MCTruth>>(mctruths_tag);
    root_times.push_back(duration_cast<microseconds>(system_clock::now()-t0));
    auto const t1 = system_clock::now();
    // do HDF reading here.
    hdf_times.push_back(duration_cast<microseconds>(system_clock::now()-t1));
  }

  // Write out the times to a standard output, in a way easily
  // readable with R (or many other tools).
  cout << "root\thdf5\n";
  for (size_t i = 0, sz = root_times.size(); i != sz; ++i) {
    cout << root_times[i].count() << '\t' << hdf_times[i].count() << '\n';
  }
}
