#ifndef AIRPORT_CONTROL_HPP
#define AIRPORT_CONTROL_HPP

#include <cadmium/modeling/coupled_model.hpp>
#include <cadmium/modeling/ports.hpp>
// Include submodel headers (which should be Coupled models themselves)
#include "Landing_Control.hpp"
#include "Taxiway_Control.hpp"
#include "Takeoff_Control.hpp"

// (Optionally include data structures for messages, e.g., Flight information)
#include "FlightInfo.hpp"  // Assume FlightInfo is a struct defining flight data (ID, type, etc.)

// The top-level Airport_Control coupled model
struct Airport_Control {
    // **1. Define input and output ports for the Airport_Control coupled model**
    struct in_new_arrival : public cadmium::in_port<FlightInfo> {};
    struct in_new_departure : public cadmium::in_port<FlightInfo> {};
    struct out_departure_complete : public cadmium::out_port<FlightInfo> {};
    struct out_arrival_complete : public cadmium::out_port<FlightInfo> {};

    // Group the input ports and output ports into tuples (required by Cadmium)
    using input_ports = std::tuple<in_new_arrival, in_new_departure>;
    using output_ports = std::tuple<out_departure_complete, out_arrival_complete>;

    // **2. List submodels (child models) that make up this coupled model**
    // We assume Landing_Control, Taxiway_Control, Takeoff_Control are defined 
    // elsewhere as coupled models (with their own input/output port definitions).
    using submodels = cadmium::modeling::models_tuple<Landing_Control, Taxiway_Control, Takeoff_Control>;

    // **3. Define External Input Couplings (EIC): connect top-level input ports to submodel input ports**
    // - New arriving flights go to the Landing_Control subsystem.
    // - New departing flights go to the Taxiway_Control subsystem (to start taxiing out).
    using eics = std::tuple<
        // EIC< TopPort, Submodel, SubmodelPort >
        cadmium::modeling::EIC< in_new_arrival, Landing_Control, Landing_Control_defs::in_new_flight>,
        cadmium::modeling::EIC< in_new_departure, Taxiway_Control, Taxiway_Control_defs::in_departure_request>
    >;

    // **4. Define External Output Couplings (EOC): connect submodel output ports to top-level output ports**
    // - When a plane has taken off (departure complete) in Takeoff_Control, send it to the top output.
    // - When a plane has finished landing and arrived at gate in Taxiway_Control, send it to the top output.
    using eocs = std::tuple<
        cadmium::modeling::EOC< Takeoff_Control, Takeoff_Control_defs::out_takeoff_done, out_departure_complete>,
        cadmium::modeling::EOC< Taxiway_Control, Taxiway_Control_defs::out_arrival_done, out_arrival_complete>
    >;

    // **5. Define Internal Couplings (IC): connect outputs of submodels to inputs of other submodels**
    // - From Landing_Control to Taxiway_Control: once a plane lands (with assigned gate), it enters the taxiway.
    // - From Taxiway_Control to Takeoff_Control: once a plane reaches the runway (ready for takeoff), hand it to takeoff control.
    using ics = std::tuple<
        cadmium::modeling::IC< Landing_Control, Landing_Control_defs::out_landed_to_taxi, Taxiway_Control, Taxiway_Control_defs::in_plane_landed>,
        cadmium::modeling::IC< Taxiway_Control, Taxiway_Control_defs::out_ready_for_takeoff, Takeoff_Control, Takeoff_Control_defs::in_takeoff_request>
    >;

    // **6. Coupled model type definition**: combine all components (ports, submodels, couplings) into the final model type.
    // Cadmium's coupled_model template uses the TIME type, which we assume to be defined (e.g., using TIME = NDTime).
    template<typename TIME>
    using model = cadmium::modeling::coupled_model<TIME, input_ports, output_ports, submodels, eics, eocs, ics>;
};

#endif // AIRPORT_CONTROL_HPP
