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

  /* TODO: Move variable declarations and initializations here to handle C-style
   *       error handling without requiring -fpermissive and turning -Werror off.
   */
  /* HDF5 */
  hid_t   fid 		= H5FNAL_BAD_HID_T;
  hid_t   fapl_id 	= H5FNAL_BAD_HID_T;
  hid_t   run_id 	= H5FNAL_BAD_HID_T;
  hid_t   subrun_id 	= H5FNAL_BAD_HID_T;
  hid_t   event_id 	= H5FNAL_BAD_HID_T;
  int prevRun 		= -1;
  int prevSubRun 	= -1;
 
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

  /* Create the file */
  if((fapl_id = H5Pcreate(H5P_FILE_ACCESS)) < 0)
    H5FNAL_HDF5_ERROR;
  if(H5Pset_libver_bounds(fapl_id, H5F_LIBVER_LATEST, H5F_LIBVER_LATEST) < 0)
    H5FNAL_HDF5_ERROR;
  if((fid = H5Fcreate(FILE_NAME, H5F_ACC_TRUNC, H5P_DEFAULT, fapl_id)) < 0)
    H5FNAL_HDF5_ERROR;
  
  // The gallery::Event object acts as a cursor into the stream of events.
  // A newly-constructed gallery::Event is at the start if its stream.
  // Use gallery::Event::atEnd() to check if you've reached the end of the stream.
  // Use gallery::Event::next() to go to the next event.

   
  for (gallery::Event ev(filenames); !ev.atEnd(); ev.next()) {

    auto const& aux = ev.eventAuxiliary();
    std::cout << "Processing event: " << aux.run()
              << ',' << aux.subRun()
              << ',' << aux.event() << '\n';
  
    unsigned int currentRun = aux.run();
    unsigned int currentSubRun = aux.subRun();

    if (currentRun != (int)prevRun) {
      // Create a new run (create name from the integer ID)
      if (run_id != H5FNAL_BAD_HID_T)
        if (h5fnal_close_run(run_id) < 0)
          H5FNAL_PROGRAM_ERROR("could not close run")

      if ((run_id = h5fnal_create_run(fid, std::to_string(currentRun).c_str())) < 0)
        H5FNAL_PROGRAM_ERROR("could not create run");

      // Create a new sub-run (create name from the integer ID)
      if (subrun_id != H5FNAL_BAD_HID_T)
        if (h5fnal_close_run(subrun_id) < 0)
          H5FNAL_PROGRAM_ERROR("could not close sub-run");

      if ((subrun_id = h5fnal_create_run(run_id, std::to_string(currentSubRun).c_str())) < 0)
        H5FNAL_PROGRAM_ERROR("could not create sub-run");

      prevRun = currentRun;
      prevSubRun = currentSubRun;
    } else if (currentSubRun != (int)prevSubRun) {
      // make new group for SubRun in the same run
      if (subrun_id != H5FNAL_BAD_HID_T)
        if (h5fnal_close_run(subrun_id) < 0)
          H5FNAL_PROGRAM_ERROR("could not close sub-run");

      if ((subrun_id = h5fnal_create_run(run_id, std::to_string(currentSubRun).c_str())) < 0)
        H5FNAL_PROGRAM_ERROR("could not create sub-run");

      prevSubRun = currentSubRun;
    }

    // Create a new event (create name from the integer ID)
    unsigned int currentEvent = aux.event();
      if ((event_id = h5fnal_create_event(subrun_id, std::to_string(currentEvent).c_str())) < 0)
        H5FNAL_PROGRAM_ERROR("could not create event");
   
    // getValidHandle() is preferred to getByLabel(), for both art and
    // gallery use. It does not require in-your-face error handling.
    std::vector<sim::MCHitCollection> const& mchits = *ev.getValidHandle<vector<sim::MCHitCollection>>(mchits_tag);

    // call h5fnal::create_v_mc_hit_collection. This will create a group containing datasets. The name for this group should be something like:
    // "MCHitCollections_mchitfinder_"
    // The empty string following the 2nd underscore indicates and empty 'product instance name'.
    // There is no need to represent the 'process name' because that is a top-level of the file entity -- in the root group.

    std::cout << "number of MCHitCollection objects: " << mchits.size() << '\n';
    std::size_t nhits = 0UL;
    for (sim::MCHitCollection const&  hitcol : mchits) {
      // channel for this hit collection
      unsigned int channel = hitcol.Channel();
      
      // pointer to first hit:
      //sim::MCHit const* = hitcol.data();
      // number of hits:
      //unsigned long sz = hitcol.size();
      // Or iterator through all this:
      //for (sim::MCHit const& hit : hitcol) {
        // in here 'hit' is the current sim::MCHit object.
        //float signal_time = hit.PeakTime();
      //}
      nhits += hitcol.size();
    }
    std::cout << "total number of hits: " << nhits << '\n';

    if (h5fnal_close_event(event_id) < 0)
      H5FNAL_PROGRAM_ERROR("could not close event")
  }

  
  if(H5Pclose(fapl_id) < 0)
    H5FNAL_HDF5_ERROR;
  if(H5Fclose(fid) < 0)
    H5FNAL_HDF5_ERROR;
  // These will still be open after the loop.
  if (h5fnal_close_run(run_id) < 0)
    H5FNAL_PROGRAM_ERROR("could not close run")
  if (h5fnal_close_run(subrun_id) < 0)
    H5FNAL_PROGRAM_ERROR("could not close sub-run")

  std::cout << "*** SUCCESS ***\n";

  exit(EXIT_SUCCESS);

error:
  std::cout << "*** FAILURE ***\n";

  H5E_BEGIN_TRY {
    H5Pclose(fapl_id);
    H5Fclose(fid);
    h5fnal_close_run(run_id);
    h5fnal_close_run(subrun_id);
    h5fnal_close_event(event_id);
  } H5E_END_TRY;

  exit(EXIT_FAILURE);
}
