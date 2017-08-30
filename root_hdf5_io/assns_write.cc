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
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/MCBase/MCHitCollection.h"

#include "h5fnal.h"

#define MASTER_RUN_CONTAINER    "master_run_container"
#define BADNAME                 "ASSNS"         // TODO: Replace this with a better name

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
  h5fnal_assns_t *h5assns = NULL;
 
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
  if (NULL == (h5assns = (h5fnal_assns_t *)calloc(1, sizeof(h5fnal_assns_t))))
    H5FNAL_PROGRAM_ERROR("could not get memory for HDF5 data product struct");

  // The gallery::Event object acts as a cursor into the stream of events.
  // A newly-constructed gallery::Event is at the start if its stream.
  // Use gallery::Event::atEnd() to check if you've reached the end of the stream.
  // Use gallery::Event::next() to go to the next event.

  // Loop over all the events in the root file
  for (gallery::Event ev(filenames); !ev.atEnd(); ev.next()) {

    auto const& aux = ev.eventAuxiliary();
    h5fnal_assns_data_t assns_data;

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
    // art::Assns<recob::Cluster, recob::Hit> const& clusters_hits = *ev.getValidHandle<art::Assns<recob::Cluster, recob::Hit>>(assns_tag); 
    auto const& clusters_hits =  *ev.getValidHandle<art::Assns<recob::Cluster, recob::Hit>>(assns_tag); 

    // Create the Assns via h5fnal.
    // The empty string following the 2nd underscore indicates and empty 'product instance name'.
    // There is no need to represent the 'process name' because that is a top-level of the file entity -- in the root group.
    // TODO: Update the name (using a cheap, hard-coded name for now)
    // TODO: What strings should LEFT and RIGHT be?
    if (h5fnal_create_assns(event_id, BADNAME, "recob::Cluster", "recob:Hit", H5T_NATIVE_INT, h5assns) < 0)
      H5FNAL_PROGRAM_ERROR("could not create HDF5 data product");

    // Process all Assns
    for (auto const& p : clusters_hits) {

    } /* end loop over Assns */

    /* Close the event and HDF5 data product */
    if (h5fnal_close_assns(h5assns) < 0)
      H5FNAL_PROGRAM_ERROR("could not close HDF5 data product");
    if (h5fnal_close_event(event_id) < 0)
      H5FNAL_PROGRAM_ERROR("could not close event");
  } /* End of loop over events */

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

  free(h5assns);

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
    h5fnal_close_assns(h5assns);
  } H5E_END_TRY;

  free(h5assns);

  std::cout << "*** FAILURE ***\n";
  exit(EXIT_FAILURE);
}

