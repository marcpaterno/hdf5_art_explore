#include <cstring>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <vector>

#include "canvas/Utilities/InputTag.h"
#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Persistency/Provenance/EventAuxiliary.h"
#include "canvas/Persistency/Provenance/ProductID.h"
#include "gallery/Event.h"
#include "gallery/ValidHandle.h"
#include "lardataobj/RecoBase/Cluster.h"
#include "lardataobj/RecoBase/Vertex.h"
#include "lardataobj/RecoBase/Hit.h"

#include "compare.hh"

#include "h5fnal.h"

#define MASTER_RUN_CONTAINER    "master_run_container"
#define BADNAME                 "ASSNS"         // TODO: Replace this with a good name

using namespace art;
using namespace std;

/* We can't do simple compare here since gallery can't create Ptrs. Instead,
 * we'll just compare the individual data fields.
 */
hbool_t
compare_hdf5_assns(hid_t loc_id, unsigned run, unsigned subrun, unsigned event, 
        art::Assns<recob::Cluster, recob::Hit> root_assns)
{
    string  run_name = std::to_string(run);
    string  subrun_name = std::to_string(subrun);
    string  event_name = std::to_string(event);
    hid_t   run_id = -1;
    hid_t   subrun_id = -1;
    hid_t   event_id = -1;
    h5fnal_assns_t *assns = NULL;
    h5fnal_assns_data_t *data = NULL;
    hsize_t u;
    hbool_t same = TRUE;

    // Open run, sub-run, and event
    if ((run_id = h5fnal_open_run(loc_id, run_name.c_str())) < 0)
        H5FNAL_PROGRAM_ERROR("could not open run")
    if ((subrun_id = h5fnal_open_run(run_id, subrun_name.c_str())) < 0)
        H5FNAL_PROGRAM_ERROR("could not open sub-run")
    if ((event_id = h5fnal_open_event(subrun_id, event_name.c_str())) < 0)
        H5FNAL_PROGRAM_ERROR("could not open event")

    // Open the data product
    if (NULL == (assns = (h5fnal_assns_t *)calloc(1, sizeof(h5fnal_assns_t))))
        H5FNAL_PROGRAM_ERROR("could not get memory for assns")
    if (h5fnal_open_assns(event_id, BADNAME, assns) < 0)
        H5FNAL_PROGRAM_ERROR("could not open assns")

    // Read all the data
    if (NULL == (data = (h5fnal_assns_data_t *)calloc(1, sizeof(h5fnal_assns_data_t))))
        H5FNAL_PROGRAM_ERROR("could not get memory for assns data")
    if (h5fnal_read_all_assns(assns, data) < 0)
        H5FNAL_PROGRAM_ERROR("could not read assns data from the file")

    // Compare with Root Assns
    if (data->n != root_assns.size())
        same = FALSE;
    else {
        u = 0;
        for (auto const& p : root_assns) {
            if (   data->pairs[u].left_process_index != p.first.id().processIndex()
                || data->pairs[u].left_product_index != p.first.id().productIndex()
                || data->pairs[u].left_key           != p.first.key()
                || data->pairs[u].right_process_index != p.second.id().processIndex()
                || data->pairs[u].right_product_index != p.second.id().productIndex()
                || data->pairs[u].right_key           != p.second.key()
                )
            same = FALSE;
            break;
            u++;
        }
    }
#if 0
    else {
        auto iter = root_assns.begin();
        for (u = 0; u < data->n && iter != root_assns.end(); u++, iter++)
        {
        }
    }
#endif

    // Close everything
    if (h5fnal_close_run(run_id) < 0)
        H5FNAL_PROGRAM_ERROR("could not close run")
    if (h5fnal_close_run(subrun_id) < 0)
        H5FNAL_PROGRAM_ERROR("could not close run")
    if (h5fnal_close_event(event_id) < 0)
        H5FNAL_PROGRAM_ERROR("could not close event")
    if (h5fnal_close_assns(assns) < 0)
        H5FNAL_PROGRAM_ERROR("could not close assns")
    if (h5fnal_free_assns_mem_data(data) < 0)
        H5FNAL_PROGRAM_ERROR("could not free assns data");
    free(assns);
    free(data);

    return same;

error:
    H5E_BEGIN_TRY {
        h5fnal_close_run(run_id);
        h5fnal_close_run(subrun_id);
        h5fnal_close_event(event_id);
        h5fnal_close_assns(assns);
    } H5E_END_TRY;
    if (data )
        h5fnal_free_assns_mem_data(data);
    free(assns);
    free(data);

    return FALSE;
}

int main(int argc, char* argv[]) {

  hid_t   fid 		= H5FNAL_BAD_HID_T;
  hid_t   master_id = H5FNAL_BAD_HID_T;
 
  InputTag mchits_tag { "mchitfinder" };
  InputTag vertex_tag { "linecluster" };
  InputTag assns_tag  { "linecluster" };

  // Get file names from the command line.
  // file name 1: root file
  // file name 2: HDF5 file
  vector<string> filenames { argv+1, argv+argc };
  if (2 != filenames.size()) {
    std::cerr << "Please supply input and output filenames\n";
    exit(EXIT_FAILURE);
  }

  /* Open the HDF5 file */
  string h5FileName = filenames.back();
  filenames.pop_back();
  if ((fid = H5Fopen(h5FileName.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT)) < 0)
    H5FNAL_HDF5_ERROR;

  /* Open the master run container */
  if ((master_id = h5fnal_open_run(fid, MASTER_RUN_CONTAINER)) < 0)
    H5FNAL_PROGRAM_ERROR("could not open master run containing group");

  // The gallery::Event object acts as a cursor into the stream of events.
  // A newly-constructed gallery::Event is at the start if its stream.
  // Use gallery::Event::atEnd() to check if you've reached the end of the stream.
  // Use gallery::Event::next() to go to the next event.

  // For each event, open the corresponding data product in the HDF5 file
  // and read the data into a new vector of MCHitCollection, then compare
  // the two data products.
  for (gallery::Event ev(filenames); !ev.atEnd(); ev.next()) {
    hbool_t same = FALSE;
    auto const& aux = ev.eventAuxiliary();
    std::cout << "Processing event " << aux.run()
              << ',' << aux.subRun()
              << ',' << aux.event()
              << ": ";
  
    // getValidHandle() is preferred to getByLabel(), for both art and
    // gallery use. It does not require in-your-face error handling.
    auto const& root_clusters_hits =  *ev.getValidHandle<art::Assns<recob::Cluster, recob::Hit>>(assns_tag); 

    // Open the data product in the event in the HDF5 file and compare the data with the Root data.
    same = compare_hdf5_assns(master_id, aux.run(), aux.subRun(), aux.event(), root_clusters_hits);

    if (same)
        cout << "equal" << endl;
    else
        cout << "*** BADNESS: NOT EQUAL ***" << endl;
  }

  /* Clean up */
  if (H5Fclose(fid) < 0)
    H5FNAL_HDF5_ERROR;
  if (h5fnal_close_run(master_id) < 0)
    H5FNAL_PROGRAM_ERROR("could not close master run container")

  std::cout << "*** SUCCESS ***\n";
  exit(EXIT_SUCCESS);

error:

  H5E_BEGIN_TRY {
    H5Fclose(fid);
    h5fnal_close_run(master_id);
  } H5E_END_TRY;

  std::cout << "*** FAILURE ***\n";
  exit(EXIT_FAILURE);
}

