#include <cstdlib>
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


typedef struct iter_data_t {
    int     n_runs;
    int     n_subruns;
    int     n_events;
} iter_data_t;

/* H5Literate() callbacks */
herr_t process_run(hid_t gid, const char *name, const H5L_info_t *info, void *op_data);
herr_t process_subrun(hid_t gid, const char *name, const H5L_info_t *info, void *op_data);
herr_t process_event(hid_t gid, const char *name, const H5L_info_t *info, void *op_data);


herr_t
process_run(hid_t gid, const char *name, const H5L_info_t *info, void *op_data)
{
  iter_data_t *stats = (iter_data_t *)op_data;

  stats->n_runs++;

  cout << "Run name: " << name << endl;

  /* Iterate over the sub-runs */
  if (H5Literate_by_name(gid, name, H5_INDEX_CRT_ORDER, H5_ITER_INC, NULL, process_subrun, op_data, H5P_DEFAULT) < 0)
    H5FNAL_PROGRAM_ERROR("iteration over sub-runs failed");

  return 0;
error:
  return -1;
}

herr_t
process_subrun(hid_t gid, const char *name, const H5L_info_t *info, void *op_data)
{
  iter_data_t *stats = (iter_data_t *)op_data;

  stats->n_subruns++;

  cout << "Sub-run name: " << name << endl;

  /* Iterate over the events */
  if (H5Literate_by_name(gid, name, H5_INDEX_CRT_ORDER, H5_ITER_INC, NULL, process_event, op_data, H5P_DEFAULT) < 0)
    H5FNAL_PROGRAM_ERROR("iteration over events failed");

  return 0;
error:
  return -1;
}

herr_t
process_event(hid_t gid, const char *name, const H5L_info_t *info, void *op_data)
{
  iter_data_t *stats = (iter_data_t *)op_data;
  h5fnal_v_mc_hit_coll_t *vector = NULL;
  hssize_t n_hits = 0;
  h5fnal_mc_hit_t *hits = NULL;
  string hitsName;
  hssize_t i;
  unsigned int prev_channel;
  std::vector<sim::MCHitCollection> vmchc;

  stats->n_events++;

  cout << "Event name: " << name;

  /* Open the vector */
  hitsName.append(name);
  hitsName.append("/");
  hitsName.append(BADNAME);
  if (NULL == (vector = (h5fnal_v_mc_hit_coll_t *)calloc(1, sizeof(h5fnal_v_mc_hit_coll_t))))
    H5FNAL_PROGRAM_ERROR("could not get memory for vector")
  if (h5fnal_open_v_mc_hit_collection(gid, hitsName.c_str(), vector) < 0)
    H5FNAL_PROGRAM_ERROR("could not open vector of mc hit collection")

  /* Read the hits */
  if ((n_hits = h5fnal_get_hits_count(vector)) < 0)
    H5FNAL_PROGRAM_ERROR("could not get number of hits from dataset")
  if (NULL == (hits = (h5fnal_mc_hit_t *)calloc(n_hits, sizeof(h5fnal_mc_hit_t))))
    H5FNAL_PROGRAM_ERROR("could allocate memory for hits_out")
  if (h5fnal_read_all_hits(vector, hits) < 0)
    H5FNAL_PROGRAM_ERROR("could not read hits from the file")

  /* Close the vector */
  if (h5fnal_close_v_mc_hit_collection(vector) < 0)
    H5FNAL_PROGRAM_ERROR("could not close vector")

  /* Pack the data into a Vector of MC Hit Collection */
  prev_channel = (unsigned)-1;
  for (i = 0; i < n_hits; i++) {
    /* If the channel changed, add the old hit collection and create
     * a new one.
     */
    if (hits[i].channel != prev_channel) {
      vmchc.emplace_back(hits[i].channel);
      prev_channel = hits[i].channel;
    }

    /* Add the hit to the current hit collection */
    sim::MCHit hit;
    hit.SetCharge(hits[i].charge, hits[i].peak_amp);
    hit.SetTime(hits[i].signal_time, hits[i].signal_width);
    float vtx[] = {hits[i].part_vertex_x, hits[i].part_vertex_y, hits[i].part_vertex_z};
    hit.SetParticleInfo(vtx, hits[i].part_energy, hits[i].part_track_id);
    vmchc.back().push_back(hit);
  }

  free(hits);
  free(vector);

  cout << " Number of hits: " << n_hits << endl;

  return 0;
error:
  H5E_BEGIN_TRY {
    if(vector)
      h5fnal_close_v_mc_hit_collection(vector);
  } H5E_END_TRY;
  free(hits);
  free(vector);

  return -1;
}

int main(int argc, char* argv[]) {

  hid_t fid         = H5FNAL_BAD_HID_T;
  hid_t master_id   = H5FNAL_BAD_HID_T;
  iter_data_t stats;

  // Get the HDF5 filename
  vector<string> filenames { argv+1, argv+argc };
  if (1 != filenames.size()) {
    std::cerr << "Please supply input filename\n";
    return 1;
  }

  /* Open the HDF5 file */
  if ((fid = H5Fopen(filenames[0].c_str(), H5F_ACC_RDONLY, H5P_DEFAULT)) < 0)
    H5FNAL_HDF5_ERROR;

  /* Open the master run container */
  if ((master_id = h5fnal_open_run(fid, MASTER_RUN_CONTAINER)) < 0)
    H5FNAL_PROGRAM_ERROR("could not open master run containing group");

  /* Iterate over the runs
   * Recursion into sub-runs, events, and data products is handled manually
   * since we don't want to chase into the inner structure of the data
   * products by using, say, H5Lvisit().
   */
  memset(&stats, 0, sizeof(iter_data_t));
  if (H5Literate(master_id, H5_INDEX_CRT_ORDER, H5_ITER_INC, NULL, process_run, (void *)&stats) < 0)
    H5FNAL_PROGRAM_ERROR("iteration over runs failed");

  /* Clean up */
  if (H5Fclose(fid) < 0)
    H5FNAL_HDF5_ERROR;
  if (h5fnal_close_run(master_id) < 0)
    H5FNAL_PROGRAM_ERROR("could not close master run container")

  std::cout << "# Runs: " << stats.n_runs << endl;
  std::cout << "# Sub-runs: " << stats.n_subruns << endl;
  std::cout << "# Events: " << stats.n_events << endl;

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
