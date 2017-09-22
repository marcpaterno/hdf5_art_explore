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

static herr_t
member_compare(simb::MCTruth root_truth, simb::MCTruth hdf5_truth)
{
    hbool_t comparable = FALSE;

    cout << endl;
    cout << "*** BADNESS SUMMARY ***" << endl;

    if (root_truth.Origin() != hdf5_truth.Origin())
        cout << "BAD: Origins differ" << endl;

    if (root_truth.NeutrinoSet() != hdf5_truth.NeutrinoSet()) {
        comparable = FALSE;
        cout << "BAD: NeutrinoSet() values differ" << endl;
    }
    else {
        comparable = TRUE;
    }

    if (comparable && true == root_truth.NeutrinoSet())
        if (root_truth.GetNeutrino() != hdf5_truth.GetNeutrino())
            cout << "BAD: Neutrino data differ" << endl;

    if (root_truth.NParticles() != hdf5_truth.NParticles()) {
        comparable = FALSE;
        cout << "BAD: Number of particles differ: " << root_truth.NParticles() << " v "<< hdf5_truth.NParticles() << endl;
    }
    else {
        comparable = TRUE;
    }

    if (comparable)
        for (int i = 0; i < root_truth.NParticles(); i++) {
            auto rp  = root_truth.GetParticle(i);
            auto h5p = hdf5_truth.GetParticle(i);
            if (rp.TrackId() != h5p.TrackId())
                cout << "BAD: Particle " << i << " track ID differs" << endl;
            if (rp.StatusCode() != h5p.StatusCode())
                cout << "BAD: Particle " << i << " status code differs" << endl;
            if (rp.PdgCode() != h5p.PdgCode())
                cout << "BAD: Particle " << i << " PDG code differs" << endl;
            if (rp.Mother() != h5p.Mother())
                cout << "BAD: Particle " << i << " mother differs" << endl;
            if (rp.Process() != h5p.Process())
                cout << "BAD: Particle " << i << " process differs" << endl;
#if 0
            if (rp.() != h5p.())
                cout << "BAD: Particle " << i << " differs" << endl;
#endif
        }

#if 0
            if (root_truth.GetParticle(i) != hdf5_truth.GetParticle(i))
                cout << "BAD: Particle " << i << " differs" << endl;
#endif

    return H5FNAL_SUCCESS;
} /* end member_compare() */


static void
get_hdf5_truths(hid_t loc_id, unsigned run, unsigned subrun, unsigned event, std::vector<simb::MCTruth> &hdf5_truths)
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
        cout << "Particles: start: " << p_start << " end: " << p_end << endl;
        if (p_start != -1)
            for (hssize_t v = p_start; v <= p_end; v++ ) {

                h5fnal_particle_t p = data->particles[v];
                hssize_t start;
                hssize_t end;

                /* ctor */
                simb::MCParticle newParticle(
                    p.track_id,
                    p.pdg_code,
                    "primary",
                    p.mother,
                    p.mass,
                    p.status);

                /* set weight */
                newParticle.SetWeight(p.weight);

                /* TODO: set end process */
                newParticle.SetEndProcess("");

                /* set polarization */
                TVector3 pol(p.polarization_x, p.polarization_y, p.polarization_z);
                newParticle.SetPolarization(pol);

                /* set rescatter */
                newParticle.SetRescatter(p.rescatter);

                /* set gvtx */
                newParticle.SetGvtx(p.gvtx_x, p.gvtx_y, p.gvtx_z, p.gvtx_t);

                /* TODO: set trajectories */
                start = p.trajectory_start_index;
                end   = p.trajectory_end_index;
                cout << "Trajectories: start: " << start << " end: " << end << endl;
                if (p_start != -1)
                    for (hssize_t w = start; w <= end; w++ ) {
                        h5fnal_trajectory_t traj = data->trajectories[v];

                        TLorentzVector pos(traj.Vx, traj.Vy, traj.Vz, traj.T);
                        TLorentzVector mo(traj.Px, traj.Py, traj.Pz, traj.E);

                        newParticle.AddTrajectoryPoint(pos, mo);
                    }


                /* TODO: set daughters */
                start = p.daughter_start_index;
                end   = p.daughter_end_index;
                cout << "Daughters: start: " << start << " end: " << end << endl;


                /* add the constructed particle to the data product */
                newTruth.Add(newParticle);
            } /* end particle construction / add loop */

        // Set the neutrino data
        if (t.neutrino_index >= 0) {
            hsize_t ni = static_cast<hsize_t>(t.neutrino_index);
            h5fnal_neutrino_t n = data->neutrinos[ni];

// Have to construct the MCParticles first since we need to get refs here
#if 0
            t.SetNeutrino(  // simb::MCParticle &nu
                            // simb::MCParticle &lep
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
#endif
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
  hid_t   master_id = H5FNAL_BAD_HID_T;
 
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
    get_hdf5_truths(master_id, aux.run(), aux.subRun(), aux.event(), hdf5_truths);

    auto const t2 = system_clock::now();

    root_times.push_back(duration_cast<microseconds>(t1 - t0));
    hdf_times.push_back(duration_cast<microseconds>(t2 - t1));

    // Check to see if the MCTruths are the same.
    // We really only need the ==, but while debugging the member_compare()
    // function proved helpful and was left in place.
    if (root_truths == hdf5_truths)
        cout << "equal" << endl;
    else {
        if (root_truths.size() != hdf5_truths.size())
            cout << "BAD: vectors not same size" << endl;
        else {
            for (unsigned int u = 0; u < root_truths.size(); u++) {
                if (member_compare(root_truths[u], hdf5_truths[u]) < 0)
                    H5FNAL_PROGRAM_ERROR("error when comparing MCTruth members");
            }
        }

        // Complain
        cout << "*** BADNESS: NOT EQUAL ***" << endl;
    }
  }

  /* Clean up */
  if (H5Fclose(fid) < 0)
    H5FNAL_HDF5_ERROR;
  if (h5fnal_close_run(master_id) < 0)
    H5FNAL_PROGRAM_ERROR("could not close master run container")

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
  } H5E_END_TRY;

  std::cout << "*** FAILURE ***\n";
  exit(EXIT_FAILURE);
}

