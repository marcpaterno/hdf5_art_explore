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

#include "compare.hh"

#include "h5fnal.h"

#define MASTER_RUN_CONTAINER    "master_run_container"
#define BADNAME                 "REPLACEME"     // TODO: Replace this with a good name

using namespace art;
using namespace std;

void
get_hdf5_hits(hid_t loc_id, unsigned run, unsigned subrun, unsigned event, std::vector<sim::MCHitCollection> &hdf5_mchits)
{
    string  run_name = std::to_string(run);
    string  subrun_name = std::to_string(subrun);
    string  event_name = std::to_string(event);
    hid_t   run_id = -1;
    hid_t   subrun_id = -1;
    hid_t   event_id = -1;
    h5fnal_vect_hitcoll_t *vector = NULL;
    h5fnal_vect_hitcoll_data_t *data = NULL;
    hsize_t hc;

    // Open run, sub-run, and event
    if ((run_id = h5fnal_open_run(loc_id, run_name.c_str())) < 0)
        H5FNAL_PROGRAM_ERROR("could not open run")
    if ((subrun_id = h5fnal_open_run(run_id, subrun_name.c_str())) < 0)
        H5FNAL_PROGRAM_ERROR("could not open sub-run")
    if ((event_id = h5fnal_open_event(subrun_id, event_name.c_str())) < 0)
        H5FNAL_PROGRAM_ERROR("could not open event")

    // Open the data product
    if (NULL == (vector = (h5fnal_vect_hitcoll_t *)calloc(1, sizeof(h5fnal_vect_hitcoll_t))))
        H5FNAL_PROGRAM_ERROR("could not get memory for vector")
    if (h5fnal_open_v_mc_hit_collection(event_id, BADNAME, vector) < 0)
        H5FNAL_PROGRAM_ERROR("could not open vector of mc hit collection")

    // Read all the data
    if (NULL == (data = (h5fnal_vect_hitcoll_data_t *)calloc(1, sizeof(h5fnal_vect_hitcoll_data_t))))
        H5FNAL_PROGRAM_ERROR("could not get memory for hit collection data")
    if (h5fnal_read_all_hits(vector, data) < 0)
        H5FNAL_PROGRAM_ERROR("could not read hit collection data from the file")

    // Convert to MCHitCollections and add to the vector
    for (hc = 0; hc < data->n_hit_collections; hc++)
    {
        hsize_t start;
        hsize_t end;
        hsize_t v;

        // Create a new hit collection in the vector
        hdf5_mchits.emplace_back(data->hit_collections[hc].channel);

        // Loop over the appropriate hits
        start = data->hit_collections[hc].start;
        end = start + data->hit_collections[hc].count;
        for (v = start; v < end; v++) {
            sim::MCHit hit;

            // Create the hit
            hit.SetCharge(data->hits[v].charge, data->hits[v].peak_amp);
            hit.SetTime(data->hits[v].signal_time, data->hits[v].signal_width);
            float vtx[] = {data->hits[v].part_vertex_x, data->hits[v].part_vertex_y, data->hits[v].part_vertex_z};
            hit.SetParticleInfo(vtx, data->hits[v].part_energy, data->hits[v].part_track_id);

            // Add the hit
            hdf5_mchits.back().push_back(hit);
        } // end loop over his
    } // end loop over hit collections

#if 0
    prev_channel = (unsigned)-1;
    for (i = 0; i < n_hits; i++) {
        /* If the channel changed, add the old hit collection and create
         * a new one.
         */
        if (hits[i].channel != prev_channel) {
            hdf5_mchits.emplace_back(hits[i].channel);
            prev_channel = hits[i].channel;
        }

        /* Add the hit to the current hit collection */
        sim::MCHit hit;
        hit.SetCharge(hits[i].charge, hits[i].peak_amp);
        hit.SetTime(hits[i].signal_time, hits[i].signal_width);
        float vtx[] = {hits[i].part_vertex_x, hits[i].part_vertex_y, hits[i].part_vertex_z};
        hit.SetParticleInfo(vtx, hits[i].part_energy, hits[i].part_track_id);
        hdf5_mchits.back().push_back(hit);
    }
#endif

    // Close everything
    if (h5fnal_close_run(run_id) < 0)
        H5FNAL_PROGRAM_ERROR("could not close run")
    if (h5fnal_close_run(subrun_id) < 0)
        H5FNAL_PROGRAM_ERROR("could not close run")
    if (h5fnal_close_event(event_id) < 0)
        H5FNAL_PROGRAM_ERROR("could not close event")
    if (h5fnal_close_v_mc_hit_collection(vector) < 0)
        H5FNAL_PROGRAM_ERROR("could not close vector")
    // TODO: clean up data
    free(vector);
    free(data);

    return;

error:
    H5E_BEGIN_TRY {
        h5fnal_close_run(run_id);
        h5fnal_close_run(subrun_id);
        h5fnal_close_event(event_id);
        h5fnal_close_v_mc_hit_collection(vector);
    } H5E_END_TRY;
    free(vector);
    free(data);

    return;
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

    auto const& aux = ev.eventAuxiliary();
    std::cout << "Processing event " << aux.run()
              << ',' << aux.subRun()
              << ',' << aux.event()
              << ": ";
  
    // getValidHandle() is preferred to getByLabel(), for both art and
    // gallery use. It does not require in-your-face error handling.
    std::vector<sim::MCHitCollection> const& root_mchits = *ev.getValidHandle<vector<sim::MCHitCollection>>(mchits_tag);

    // Open the data product in the event in the HDF5 file and get all the data out.
    std::vector<sim::MCHitCollection> hdf5_mchits;
    get_hdf5_hits(master_id, aux.run(), aux.subRun(), aux.event(), hdf5_mchits);

    if (root_mchits == hdf5_mchits)
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
