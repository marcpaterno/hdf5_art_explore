#include <cstring>
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
  h5fnal_v_mc_hit_coll_t *h5vmchc = NULL;
 
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

  /* Allocate memory for the data product struct */
  if (NULL == (h5vmchc = (h5fnal_v_mc_hit_coll_t *)calloc(1, sizeof(h5fnal_v_mc_hit_coll_t))))
    H5FNAL_PROGRAM_ERROR("could not get memory for struct");

  // The gallery::Event object acts as a cursor into the stream of events.
  // A newly-constructed gallery::Event is at the start if its stream.
  // Use gallery::Event::atEnd() to check if you've reached the end of the stream.
  // Use gallery::Event::next() to go to the next event.

   
  for (gallery::Event ev(filenames); !ev.atEnd(); ev.next()) {
    vector<h5fnal_mc_hit_t> hits;
    auto const& aux = ev.eventAuxiliary();
    std::cout << "Processing event: " << aux.run()
              << ',' << aux.subRun()
              << ',' << aux.event() << '\n';
  
    unsigned int currentRun = aux.run();
    unsigned int currentSubRun = aux.subRun();

    if ((int)currentRun != prevRun) {
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
    }
    else if ((int)currentSubRun != prevSubRun) {
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

    // Create the Vector of MC Hit Collection via h5fnal.
    // This will create a group containing datasets. The name for this group should be something like:
    // "MCHitCollections_mchitfinder_"
    // The empty string following the 2nd underscore indicates and empty 'product instance name'.
    // There is no need to represent the 'process name' because that is a top-level of the file entity -- in the root group.
    memset(h5vmchc, 0, sizeof(h5fnal_v_mc_hit_coll_t));
    if (h5fnal_create_v_mc_hit_collection(event_id, "REPLACEME", h5vmchc) < 0)
      H5FNAL_PROGRAM_ERROR("could not create HDF5 data product");

    // Process all MC Hit Collections
    for (sim::MCHitCollection const&  hitcol : mchits) {
      unsigned int channel = hitcol.Channel();  // channel for this hit collection
      
      // Iterate through all hits
      for (sim::MCHit const& hit : hitcol) {
        h5fnal_mc_hit_t h5hit;

        h5hit.signal_time 	= hit.PeakTime();
        h5hit.signal_width	= hit.PeakWidth();
        h5hit.peak_amp 		= hit.Charge(true);
        h5hit.charge 		= hit.Charge(false);
        h5hit.part_vertex_x	= (hit.PartVertex())[0];
        h5hit.part_vertex_y 	= (hit.PartVertex())[1];
        h5hit.part_vertex_z 	= (hit.PartVertex())[2];
        h5hit.part_energy 	= hit.PartEnergy();
        h5hit.part_track_id 	= hit.PartTrackId();
        h5hit.channel 		= channel;

        hits.push_back(h5hit);
      }
    }

    // Write the data to the HDF5 data product
    if (hits.size() > 0)
      if (h5fnal_write_hits(h5vmchc, hits.size(), &hits[0]) < 0)
        H5FNAL_PROGRAM_ERROR("could not write hits to the HDF5 data product")

    /* Close the event and HDF5 data product */
    if(h5fnal_close_v_mc_hit_collection(h5vmchc) < 0)
      H5FNAL_PROGRAM_ERROR("could not close HDF5 data product");
    if (h5fnal_close_event(event_id) < 0)
      H5FNAL_PROGRAM_ERROR("could not close event");
  }

  /* Clean up */
  if(H5Pclose(fapl_id) < 0)
    H5FNAL_HDF5_ERROR;
  if(H5Fclose(fid) < 0)
    H5FNAL_HDF5_ERROR;
  // These will still be open after the loop.
  if (h5fnal_close_run(run_id) < 0)
    H5FNAL_PROGRAM_ERROR("could not close run")
  if (h5fnal_close_run(subrun_id) < 0)
    H5FNAL_PROGRAM_ERROR("could not close sub-run")

  free(h5vmchc);

  std::cout << "*** SUCCESS ***\n";
  exit(EXIT_SUCCESS);

error:

  H5E_BEGIN_TRY {
    H5Pclose(fapl_id);
    H5Fclose(fid);
    h5fnal_close_run(run_id);
    h5fnal_close_run(subrun_id);
    h5fnal_close_event(event_id);
    h5fnal_close_v_mc_hit_collection(h5vmchc);
  } H5E_END_TRY;

  free(h5vmchc);

  std::cout << "*** FAILURE ***\n";
  exit(EXIT_FAILURE);
}
