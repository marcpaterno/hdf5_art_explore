#include <chrono>
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
#include "nusimdata/SimulationBase/MCTruth.h"

#include "compare.hh"

#include "h5fnal.h"

#define MASTER_RUN_CONTAINER    "master_run_container"
#define BADNAME                 "MCTRUTH"     // TODO: Replace this with a good name

using namespace art;
using namespace std;
using namespace simb;
using namespace std::chrono;

static void
get_hdf5_truths(hid_t loc_id, unsigned run, unsigned subrun, unsigned event, string_dictionary_t *dict, std::vector<simb::MCTruth> &hdf5_truths)
{
    string  run_name = std::to_string(run);
    string  subrun_name = std::to_string(subrun);
    string  event_name = std::to_string(event);
    hid_t   run_id = -1;
    hid_t   subrun_id = -1;
    hid_t   event_id = -1;
    h5fnal_vect_truth_t *vector = NULL;
    h5fnal_vect_truth_data_t *data = NULL;

    // Open run, sub-run, and event
    if ((run_id = h5fnal_open_run(loc_id, run_name.c_str())) < 0)
        H5FNAL_PROGRAM_ERROR("could not open run")
    if ((subrun_id = h5fnal_open_run(run_id, subrun_name.c_str())) < 0)
        H5FNAL_PROGRAM_ERROR("could not open sub-run")
    if ((event_id = h5fnal_open_event(subrun_id, event_name.c_str())) < 0)
        H5FNAL_PROGRAM_ERROR("could not open event")

    // Open the data product
    if (NULL == (vector = (h5fnal_vect_truth_t *)calloc(1, sizeof(h5fnal_vect_truth_t))))
        H5FNAL_PROGRAM_ERROR("could not get memory for vector")
    if (h5fnal_open_v_mc_truth(event_id, BADNAME, vector) < 0)
        H5FNAL_PROGRAM_ERROR("could not open Vector of MCTruth")

    // Read all the data
    if (NULL == (data = (h5fnal_vect_truth_data_t *)calloc(1, sizeof(h5fnal_vect_truth_data_t))))
        H5FNAL_PROGRAM_ERROR("could not get memory for truth data")
    if (h5fnal_read_all_truths(vector, data) < 0)
        H5FNAL_PROGRAM_ERROR("could not read truth data from the file")

    // Convert to MCTruth and add to the vector
    for (hsize_t u = 0; u < data->n_truths; u++)
    {
        simb::MCTruth newTruth;
        h5fnal_truth_t t = data->truths[u];
        hssize_t p_start;
        hssize_t p_end;
        
        // Set the origin
        newTruth.SetOrigin(static_cast<simb::Origin_t>(t.origin));

        // Add particles
        p_start = t.particle_start_index;
        p_end   = t.particle_end_index;
        if (p_start != -1)
            for (hssize_t v = p_start; v <= p_end; v++ ) {

                h5fnal_particle_t p = data->particles[v];
                hssize_t start;
                hssize_t end;
                char *s = NULL;

                /* Get the Process string from the dictionary */
                if (get_string(dict, p.process_index, &s) < 0)
                    H5FNAL_PROGRAM_ERROR("error getting process string");

                /* ctor */
                simb::MCParticle newParticle(
                    p.track_id,
                    p.pdg_code,
                    s,
                    p.mother,
                    p.mass,
                    p.status);

                /* set weight */
                newParticle.SetWeight(p.weight);

                /* Set end process */
                if (get_string(dict, p.endprocess_index, &s) < 0)
                    H5FNAL_PROGRAM_ERROR("error getting end process string");
                newParticle.SetEndProcess(s);

                /* set polarization */
                TVector3 pol(p.polarization_x, p.polarization_y, p.polarization_z);
                newParticle.SetPolarization(pol);

                /* set rescatter */
                newParticle.SetRescatter(p.rescatter);

                /* set gvtx */
                newParticle.SetGvtx(p.gvtx_x, p.gvtx_y, p.gvtx_z, p.gvtx_t);

                /* set trajectories */
                start = p.trajectory_start_index;
                end   = p.trajectory_end_index;
                if (start != -1)
                    for (hssize_t w = start; w <= end; w++ ) {
                        h5fnal_trajectory_t traj = data->trajectories[v];

                        TLorentzVector pos(traj.Vx, traj.Vy, traj.Vz, traj.T);
                        TLorentzVector mo(traj.Px, traj.Py, traj.Pz, traj.E);

                        newParticle.AddTrajectoryPoint(pos, mo);
                    }

                /* set daughters */
                start = p.daughter_start_index;
                end   = p.daughter_end_index;
                if (start != -1)
                    for (hssize_t w = start; w <= end; w++ ) {
                        h5fnal_daughter_t d = data->daughters[v];

                        newParticle.AddDaughter(d.track_id);
                    }

                /* add the constructed particle to the data product */
                newTruth.Add(newParticle);
            } /* end particle construction / add loop */

        // Set the neutrino data
        if (t.neutrino_index >= 0) {
            hsize_t ni = static_cast<hsize_t>(t.neutrino_index);
            h5fnal_neutrino_t n = data->neutrinos[ni];

            // Nu and Lepton particles are determined
            // automatically when this is set.
            newTruth.SetNeutrino(
                            n.ccnc,
                            n.mode,
                            n.interaction_type,
                            n.target,
                            n.hit_nuc,
                            n.hit_quark,
                            n.w,
                            n.x,
                            n.y,
                            n.q_sqr);
        }
        
        // Create a new hit collection in the vector
        hdf5_truths.push_back(newTruth);

    } // end loop over truths

    // Close everything
    if (h5fnal_close_run(run_id) < 0)
        H5FNAL_PROGRAM_ERROR("could not close run")
    if (h5fnal_close_run(subrun_id) < 0)
        H5FNAL_PROGRAM_ERROR("could not close run")
    if (h5fnal_close_event(event_id) < 0)
        H5FNAL_PROGRAM_ERROR("could not close event")
    if (h5fnal_close_v_mc_truth(vector) < 0)
        H5FNAL_PROGRAM_ERROR("could not close vector")
    if (h5fnal_free_truth_mem_data(data) < 0)
        H5FNAL_PROGRAM_ERROR("could not free in-memory truth data");
    free(vector);
    free(data);

    return;

error:
    H5E_BEGIN_TRY {
        h5fnal_close_run(run_id);
        h5fnal_close_run(subrun_id);
        h5fnal_close_event(event_id);
        h5fnal_close_v_mc_truth(vector);
    } H5E_END_TRY;
    if (data )
        h5fnal_free_truth_mem_data(data);
    free(vector);
    free(data);

    return;
}

int main(int argc, char* argv[]) {

  hid_t   fid 		= H5FNAL_BAD_HID_T;
  hid_t   dict_id 	= H5FNAL_BAD_HID_T;
  hid_t   master_id = H5FNAL_BAD_HID_T;

  string_dictionary_t *dict = NULL;

  InputTag mchits_tag { "mchitfinder" };
  InputTag vertex_tag { "linecluster" };
  InputTag assns_tag  { "linecluster" };
  InputTag truths_tag { "generator" };

  vector<microseconds> root_times; // times for reading ROOT
  vector<microseconds> hdf_times;  // times for reading HDF5

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

  /* Open the file-wide string dictionary */
  if (NULL == (dict = (string_dictionary_t *)calloc(1, sizeof(string_dictionary_t))))
    H5FNAL_PROGRAM_ERROR("could not get memory for string dictionary");
  if ((dict_id = open_string_dictionary(fid, dict)) < 0)
    H5FNAL_PROGRAM_ERROR("could not open string dictionary");

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

    auto const t0 = system_clock::now();

    std::vector<simb::MCTruth> const& root_truths = *ev.getValidHandle<vector<simb::MCTruth>>(truths_tag);

    auto const t1 = system_clock::now();

    // Open the data product in the event in the HDF5 file and get all the data out.
    std::vector<simb::MCTruth> hdf5_truths;
    get_hdf5_truths(master_id, aux.run(), aux.subRun(), aux.event(), dict, hdf5_truths);

    auto const t2 = system_clock::now();

    root_times.push_back(duration_cast<microseconds>(t1 - t0));
    hdf_times.push_back(duration_cast<microseconds>(t2 - t1));

    // Check to see if the MCTruths are the same.
    // We really only need the ==, but while debugging the member_compare()
    // function proved helpful and was left in place.
    if (root_truths == hdf5_truths)
        cout << "equal" << endl;
    else
        cout << "*** BADNESS: NOT EQUAL ***" << endl;
  }

  /* Clean up */
  if (close_string_dictionary(dict) < 0)
    H5FNAL_PROGRAM_ERROR("could not close string dictionary")
  if (h5fnal_close_run(master_id) < 0)
    H5FNAL_PROGRAM_ERROR("could not close master run container")
  if (H5Fclose(fid) < 0)
    H5FNAL_HDF5_ERROR;

  // Write out the times to a standard output, in a way easily
  // readable with R (or many other tools).
  std::cout << "root\thdf5\n";
  for (size_t i = 0, sz = root_times.size(); i != sz; ++i) {
    cout << root_times[i].count() << '\t' << hdf_times[i].count() << '\n';
  }

  std::cout << "*** SUCCESS ***\n";
  exit(EXIT_SUCCESS);

error:

  H5E_BEGIN_TRY {
    H5Fclose(fid);
    h5fnal_close_run(master_id);
    if (dict)
      close_string_dictionary(dict);
  } H5E_END_TRY;

  std::cout << "*** FAILURE ***\n";
  exit(EXIT_FAILURE);
}

