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

#include "h5fnal.h"

#define MASTER_RUN_CONTAINER    "master_run_container"
#define BADNAME                 "MCTRUTH"     // TODO: Replace this with a good name

using namespace art;
using namespace std;
using namespace simb;

int main(int argc, char* argv[]) {

  size_t totalTruths = 0L;
  hid_t   fid 		= H5FNAL_BAD_HID_T;
  hid_t   fapl_id 	= H5FNAL_BAD_HID_T;
  hid_t   master_id = H5FNAL_BAD_HID_T;
  hid_t   run_id 	= H5FNAL_BAD_HID_T;
  hid_t   subrun_id = H5FNAL_BAD_HID_T;
  hid_t   event_id 	= H5FNAL_BAD_HID_T;
  int prevRun 		= -1;
  int prevSubRun 	= -1;
  h5fnal_vect_truth_t *h5vtruth = NULL;
 
  InputTag mchits_tag { "mchitfinder" };
  InputTag vertex_tag { "linecluster" };
  InputTag assns_tag  { "linecluster" };
  InputTag truths_tag { "generator" };

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
  if (NULL == (h5vtruth = (h5fnal_vect_truth_t *)calloc(1, sizeof(h5fnal_vect_truth_t))))
    H5FNAL_PROGRAM_ERROR("could not get memory for HDF5 data product struct");

  // The gallery::Event object acts as a cursor into the stream of events.
  // A newly-constructed gallery::Event is at the start if its stream.
  // Use gallery::Event::atEnd() to check if you've reached the end of the stream.
  // Use gallery::Event::next() to go to the next event.

  // Loop over all the events in the root file
  for (gallery::Event ev(filenames); !ev.atEnd(); ev.next()) {
    auto const& aux = ev.eventAuxiliary();

    vector<h5fnal_truth_t> truths;
    vector<h5fnal_trajectory_t> trajectories;
    vector<h5fnal_daughter_t> daughters;
    vector<h5fnal_particle_t> particles;
    vector<h5fnal_neutrino_t> neutrinos;

    h5fnal_vect_truth_data_t truth_data;

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
    std::vector<simb::MCTruth> const& rootTruths = *ev.getValidHandle<vector<simb::MCTruth>>(truths_tag);

    // Create the Vector of MC Truth via h5fnal.
    // This will create a group containing datasets. The name for this group should be something like:
    // "MCHitCollections_mchitfinder_"
    // The empty string following the 2nd underscore indicates and empty 'product instance name'.
    // There is no need to represent the 'process name' because that is a top-level of the file entity -- in the root group.
    // TODO: Update the name (using a cheap, hard-coded name for now)
    if (h5fnal_create_v_mc_truth(event_id, BADNAME, h5vtruth) < 0)
      H5FNAL_PROGRAM_ERROR("could not create HDF5 data product");

    // Iterate through all truths in the vector
    totalTruths += rootTruths.size();
    cout << "Number of truths in vector: " << rootTruths.size() << endl;
    for (unsigned n = 0; n < rootTruths.size(); n++) {

        simb::MCTruth t = rootTruths[n];
        h5fnal_truth_t truth;

        // Copy origin
        truth.origin = static_cast<h5fnal_origin_t>(t.Origin());

        // Copy particles
        for (int i = 0; i < t.NParticles(); i++) {
            const simb::MCParticle& p = t.GetParticle(i);
            h5fnal_particle_t particle;

            particle.status     = p.StatusCode();
            particle.track_id   = p.TrackId();
            particle.pdg_code   = p.PdgCode();
            particle.mother     = p.Mother();
            particle.mass       = p.Mass();
            particle.weight     = p.Weight();
            particle.gvtx_x     = p.Gvx();
            particle.gvtx_y     = p.Gvy();
            particle.gvtx_z     = p.Gvz();
            particle.gvtx_t     = p.Gvt();
            particle.rescatter  = p.Rescatter();

            // Get polarization TVector3 and set fields
            const TVector3& polarization = p.Polarization();
            particle.polarization_x = polarization.x();
            particle.polarization_y = polarization.y();
            particle.polarization_z = polarization.z();

            // TODO: Process string handling here
            cout << "Process: " << p.Process() << endl;
            cout << "End Process: " << p.EndProcess() << endl;

            cout << "Added particle" << endl;
            particles.push_back(particle);

            // Copy daughters for this particle
            for (int j = 0; j < p.NumberDaughters(); j++) {
                h5fnal_daughter_t daughter;

                daughter.track_id     = p.Daughter(i);

                cout << "Added daughter" << endl;
                daughters.push_back(daughter);
            }

            // Copy trajectories
            for (unsigned int j = 0; j < p.NumberTrajectoryPoints(); j++) {
                h5fnal_trajectory_t trajectory;

                trajectory.Vx   = p.Vx(j);
                trajectory.Vy   = p.Vy(j);
                trajectory.Vz   = p.Vz(j);
                trajectory.T    = p.T(j);

                trajectory.Px   = p.Px(j);
                trajectory.Py   = p.Py(j);
                trajectory.Pz   = p.Pz(j);
                trajectory.E    = p.E(j);

                cout << "Added trajectory" << endl;
                trajectories.push_back(trajectory);
            }
        }

        // Copy neutrino data
        if (t.NeutrinoSet()) {
            const simb::MCNeutrino& n = t.GetNeutrino();
            h5fnal_neutrino_t neutrino;

            //neutrino.nu                 = ?
            //neutrino.lepton             = ?
            neutrino.mode               = n.Mode();
            neutrino.interaction_type   = n.InteractionType();
            neutrino.ccnc               = n.CCNC();
            neutrino.target             = n.Target();
            neutrino.hit_nuc            = n.HitNuc();
            neutrino.hit_quark          = n.HitQuark();
            neutrino.w                  = n.W();
            neutrino.x                  = n.X();
            neutrino.y                  = n.Y();
            neutrino.q_sqr              = n.QSqr();

            // TODO: fill in (might use association dset instead)
            //neutrino.nu                 = 0 ;
            //neutrino.lepton             = 0 ;

            truth.neutrino_index = truths.size();

            cout << "Added neutrino" << endl;
            neutrinos.push_back(neutrino);
        }
        else
            truth.neutrino_index = -1;

        // TODO: Add use size() to fixup index values

        truths.push_back(truth);

    }

    // Write the data to the HDF5 data product
    truth_data.n_truths         = truths.size();
    truth_data.truths           = &truths[0];
    truth_data.n_trajectories   = trajectories.size();
    truth_data.trajectories     = &trajectories[0];
    truth_data.n_daughters      = daughters.size();
    truth_data.daughters        = &daughters[0];
    truth_data.n_particles      = particles.size();
    truth_data.particles        = &particles[0];
    truth_data.n_neutrinos      = neutrinos.size();
    truth_data.neutrinos        = &neutrinos[0];

    if (h5fnal_append_truths(h5vtruth, &truth_data) < 0)
      H5FNAL_PROGRAM_ERROR("could not write truths to the HDF5 data product");

    /* Close the event and HDF5 data product */
    if (h5fnal_close_v_mc_truth(h5vtruth) < 0)
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

  free(h5vtruth);

  cout << "Wrote " << totalTruths << " TOTAL truths to the HDF5 file." << endl;
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
    h5fnal_close_v_mc_truth(h5vtruth);
  } H5E_END_TRY;

  free(h5vtruth);

  std::cout << "*** FAILURE ***\n";
  exit(EXIT_FAILURE);
}
