#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <vector>

#include "canvas/Utilities/InputTag.h"
#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Persistency/Provenance/EventAuxiliary.h"
#include "gallery/Event.h"
#include "gallery/ValidHandle.h"
#include "lardataobj/RecoBase/Cluster.h"
#include "lardataobj/RecoBase/Vertex.h"
#include "lardataobj/MCBase/MCHitCollection.h"

#include "h5fnal.h"

#define FILE_NAME   "vmchc.h5"
#define RUN_NAME    "testrun"
#define EVENT_NAME  "testevent"
#define VECTOR_NAME "vomchc"

using namespace art;
using namespace std;

int main(int argc, char* argv[]) {

  /********
   * ROOT *
   ********/
  InputTag mchits_tag { "mchitfinder" };
  InputTag vertex_tag { "linecluster" };
  InputTag assns_tag  { "linecluster" };
  vector<string> filenames { argv+1, argv+argc }; // filenames from command line
  if (filenames.empty()) {
    std::cerr << "Please supply one or more input filenames\n";
    return 1;
  }

  // The gallery::Event object acts as a cursor into the stream of events.
  // A newly-constructed gallery::Event is at the start if its stream.
  // Use gallery::Event::atEnd() to check if you've reached the end of the stream.
  // Use gallery::Event::next() to go to the next event.

  for (gallery::Event ev(filenames); !ev.atEnd(); ev.next()) {
    // getValidHandle() is preferred to getByLabel(), for both art and
    // gallery use. It does not require in-your-face error handling.
    std::vector<sim::MCHitCollection> const& mchits = *ev.getValidHandle<vector<sim::MCHitCollection>>(mchits_tag);

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
    auto const& aux = ev.eventAuxiliary();
    std::cout << "Processing event: " << aux.run()
              << ',' << aux.subRun()
              << ',' << aux.event() << '\n';
    std::cout << "number of MCHitCollection objects: " << mchits.size() << '\n';
    std::size_t nhits = 0UL;
    for (auto const&  hitcol : mchits) { nhits += hitcol.size(); }
    std::cout << "total number of hits: " << nhits << '\n';
  }

  /********
   * HDF5 *
   ********/
  hid_t   fid = -1;
  hid_t   fapl_id = -1;
  hid_t   run_id = -1;
  hid_t   event_id = -1;

  /* Create the file */
  if((fapl_id = H5Pcreate(H5P_FILE_ACCESS)) < 0)
    H5FNAL_HDF5_ERROR
  if(H5Pset_libver_bounds(fapl_id, H5F_LIBVER_LATEST, H5F_LIBVER_LATEST) < 0)
    H5FNAL_HDF5_ERROR
  if((fid = H5Fcreate(FILE_NAME, H5F_ACC_TRUNC, H5P_DEFAULT, fapl_id)) < 0)
    H5FNAL_HDF5_ERROR

  /* Create the run and event */
  if((run_id = h5fnal_create_run(fid, RUN_NAME)) < 0)
    H5FNAL_PROGRAM_ERROR("could not create run")
  if((event_id = h5fnal_create_event(run_id, EVENT_NAME)) < 0)
    H5FNAL_PROGRAM_ERROR("could not create event")

  /* Close everything */
  if(h5fnal_close_run(run_id) < 0)
    H5FNAL_PROGRAM_ERROR("could not close run")
  if(h5fnal_close_event(event_id) < 0)
    H5FNAL_PROGRAM_ERROR("could not close event")
  if(H5Pclose(fapl_id) < 0)
    H5FNAL_HDF5_ERROR
  if(H5Fclose(fid) < 0)
    H5FNAL_HDF5_ERROR

  std::cout << "*** SUCCESS ***\n";

  exit(EXIT_SUCCESS);

error:
  std::cout << "*** FAILURE ***\n";

  H5E_BEGIN_TRY {
    h5fnal_close_run(run_id);
    h5fnal_close_event(event_id);
    H5Pclose(fapl_id);
    H5Fclose(fid);
  } H5E_END_TRY;

  exit(EXIT_FAILURE);
}
