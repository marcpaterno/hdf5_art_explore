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

#define MASTER_RUN_CONTAINER    "master_run_container"
#define BADNAME                 "REPLACEME"     // TODO: Replace this with a good name

using namespace art;
using namespace std;

int main(int argc, char* argv[]) {

  size_t totalHits = 0L;
  hid_t   fid 		= H5FNAL_BAD_HID_T;
  hid_t   fapl_id 	= H5FNAL_BAD_HID_T;
  hid_t   master_id = H5FNAL_BAD_HID_T;
  hid_t   run_id 	= H5FNAL_BAD_HID_T;
  hid_t   subrun_id = H5FNAL_BAD_HID_T;
  hid_t   event_id 	= H5FNAL_BAD_HID_T;
  int prevRun 		= -1;
  int prevSubRun 	= -1;
  h5fnal_vect_hitcoll_t *h5vmchc = NULL;
 
  InputTag mchits_tag { "mchitfinder" };
  InputTag vertex_tag { "linecluster" };
  InputTag assns_tag  { "linecluster" };

  vector<string> filenames { argv+1, argv+argc }; // filenames from command line
  if (2 != filenames.size()) {
    std::cerr << "Please supply input and output filenames\n";
    exit(EXIT_FAILURE);
  }

  /* Create the HDF5 file */
  string h5FileName = filenames.back();
  filenames.pop_back();
  if ((fapl_id = H5Pcreate(H5P_FILE_ACCESS)) < 0)
    H5FNAL_HDF5_ERROR;
  if (H5Pset_libver_bounds(fapl_id, H5F_LIBVER_LATEST, H5F_LIBVER_LATEST) < 0)
    H5FNAL_HDF5_ERROR;
  if ((fid = H5Fcreate(h5FileName.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, fapl_id)) < 0)
    H5FNAL_HDF5_ERROR;

  /* Create a top-level containing group in which creation order is tracked and indexed.
   * There is no way to do this in the root group, so we can't use that.
   */
  if ((master_id = h5fnal_create_run(fid, MASTER_RUN_CONTAINER, FALSE)) < 0)
    H5FNAL_PROGRAM_ERROR("could not create master run containing group");

  /* Allocate memory for the data product struct
   *
   * This will be re-used for all data products we read in root and create
   * in HDF5 (it's re-initialized on close).
   */
  if (NULL == (h5vmchc = (h5fnal_vect_hitcoll_t *)calloc(1, sizeof(h5fnal_vect_hitcoll_t))))
    H5FNAL_PROGRAM_ERROR("could not get memory for HDF5 data product struct");

  // The gallery::Event object acts as a cursor into the stream of events.
  // A newly-constructed gallery::Event is at the start if its stream.
  // Use gallery::Event::atEnd() to check if you've reached the end of the stream.
  // Use gallery::Event::next() to go to the next event.

  // Loop over all the events in the root file
  for (gallery::Event ev(filenames); !ev.atEnd(); ev.next()) {

    vector<h5fnal_hit_t> hits;
    vector<h5fnal_hitcoll_t> hit_collections;
    auto const& aux = ev.eventAuxiliary();
    hsize_t first_hit;
    h5fnal_vect_hitcoll_data_t hc_data;

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

      if ((run_id = h5fnal_create_run(master_id, std::to_string(currentRun).c_str(), FALSE)) < 0)
        H5FNAL_PROGRAM_ERROR("could not create run");

      // Create a new sub-run (create name from the integer ID)
      if (subrun_id != H5FNAL_BAD_HID_T)
        if (h5fnal_close_run(subrun_id) < 0)
          H5FNAL_PROGRAM_ERROR("could not close sub-run");

      if ((subrun_id = h5fnal_create_run(run_id, std::to_string(currentSubRun).c_str(), FALSE)) < 0)
        H5FNAL_PROGRAM_ERROR("could not create sub-run");

      prevRun = currentRun;
      prevSubRun = currentSubRun;
    }
    else if ((int)currentSubRun != prevSubRun) {
      // make new group for SubRun in the same run
      if (subrun_id != H5FNAL_BAD_HID_T)
        if (h5fnal_close_run(subrun_id) < 0)
          H5FNAL_PROGRAM_ERROR("could not close sub-run");

      if ((subrun_id = h5fnal_create_run(run_id, std::to_string(currentSubRun).c_str(), FALSE)) < 0)
        H5FNAL_PROGRAM_ERROR("could not create sub-run");

      prevSubRun = currentSubRun;
    }

    // Create a new event (create name from the integer ID)
    unsigned int currentEvent = aux.event();
    if ((event_id = h5fnal_create_event(subrun_id, std::to_string(currentEvent).c_str(), FALSE)) < 0)
      H5FNAL_PROGRAM_ERROR("could not create event");
   
    // getValidHandle() is preferred to getByLabel(), for both art and
    // gallery use. It does not require in-your-face error handling.
    std::vector<sim::MCHitCollection> const& mchits = *ev.getValidHandle<vector<sim::MCHitCollection>>(mchits_tag);

    // Create the Vector of MC Hit Collection via h5fnal.
    // This will create a group containing datasets. The name for this group should be something like:
    // "MCHitCollections_mchitfinder_"
    // The empty string following the 2nd underscore indicates and empty 'product instance name'.
    // There is no need to represent the 'process name' because that is a top-level of the file entity -- in the root group.
    // TODO: Update the name (using a cheap, hard-coded name for now)
    if (h5fnal_create_v_mc_hit_collection(event_id, BADNAME, h5vmchc) < 0)
      H5FNAL_PROGRAM_ERROR("could not create HDF5 data product");

    // Process all MC Hit Collections
    first_hit = 0;
    for (sim::MCHitCollection const&  hitcol : mchits) {
      h5fnal_hitcoll_t hc;
      hsize_t hitcount = 0;
      
      // Iterate through all hits
      for (sim::MCHit const& hit : hitcol) {
        h5fnal_hit_t h5hit;

        h5hit.signal_time 	= hit.PeakTime();
        h5hit.signal_width	= hit.PeakWidth();
        h5hit.peak_amp 		= hit.Charge(true);
        h5hit.charge 		= hit.Charge(false);
        h5hit.part_vertex_x	= (hit.PartVertex())[0];
        h5hit.part_vertex_y 	= (hit.PartVertex())[1];
        h5hit.part_vertex_z 	= (hit.PartVertex())[2];
        h5hit.part_energy 	= hit.PartEnergy();
        h5hit.part_track_id 	= hit.PartTrackId();

        hitcount++;
        hits.push_back(h5hit);
      } /* end loop over hits */

      hc.channel = hitcol.Channel();
      hc.count = hitcount;
      hc.start = (hitcount > 0) ? first_hit : 0;
      hit_collections.push_back(hc);

      first_hit += hitcount;

    } /* end loop over hit collections */

    // Write the data to the HDF5 data product
    hc_data.n_hits = hits.size();
    hc_data.n_hit_collections = hit_collections.size();
    hc_data.hits = &hits[0];
    hc_data.hit_collections = &hit_collections[0];

    if (h5fnal_append_hits(h5vmchc, &hc_data) < 0)
      H5FNAL_PROGRAM_ERROR("could not write hits to the HDF5 data product");

    totalHits += hits.size();
    cout << "Wrote " << hits.size() << " hits to the HDF5 file." << endl;

    /* Close the event and HDF5 data product */
    if (h5fnal_close_v_mc_hit_collection(h5vmchc) < 0)
      H5FNAL_PROGRAM_ERROR("could not close HDF5 data product");
    if (h5fnal_close_event(event_id) < 0)
      H5FNAL_PROGRAM_ERROR("could not close event");
  }

  /* Clean up */
  if (H5Pclose(fapl_id) < 0)
    H5FNAL_HDF5_ERROR;
  if (H5Fclose(fid) < 0)
    H5FNAL_HDF5_ERROR;
  if (h5fnal_close_run(master_id) < 0)
    H5FNAL_PROGRAM_ERROR("could not close master run container")
  // These will still be open after the loop.
  if (h5fnal_close_run(run_id) < 0)
    H5FNAL_PROGRAM_ERROR("could not close run")
  if (h5fnal_close_run(subrun_id) < 0)
    H5FNAL_PROGRAM_ERROR("could not close sub-run")

  free(h5vmchc);

  cout << "Wrote " << totalHits << " TOTAL hits to the HDF5 file." << endl;
  std::cout << "*** SUCCESS ***\n";
  exit(EXIT_SUCCESS);

error:

  H5E_BEGIN_TRY {
    H5Pclose(fapl_id);
    H5Fclose(fid);
    h5fnal_close_run(run_id);
    h5fnal_close_run(subrun_id);
    h5fnal_close_event(event_id);
    h5fnal_close_run(master_id);
    h5fnal_close_v_mc_hit_collection(h5vmchc);
  } H5E_END_TRY;

  free(h5vmchc);

  std::cout << "*** FAILURE ***\n";
  exit(EXIT_FAILURE);
}
