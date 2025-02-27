#include "Airport_Control.hpp"

// If the submodels require specific initialization (e.g., initial planes in the system), 
// we can define their instances here. However, in Cadmium coupled models, submodels are 
// usually referenced by type in the template, and their default constructors (if any) 
// are used. 

// This .cpp file can remain mostly empty if all coupling definitions are in the header. 
// If needed, one could instantiate the top model here for simulation, for example:

#include <cadmium/engine/pdevs_runner.hpp>
#include <cadmium/logger/common_loggers.hpp>
#include <iostream>

int main() {
    // Define the simulation time type (Cadmium typically uses NDTime for discrete-event simulation time).
    using TIME = NDTime;  // assuming NDTime is provided by Cadmium's engine

    // Define the top model using the coupled model template instantiated with TIME
    using AirportControlModel = Airport_Control::model<TIME>;

    // Create a runner to simulate until a certain time (for example, 12 hours represented in whatever time unit NDTime uses)
    cadmium::engine::runner<TIME, AirportControlModel> r{ {0} };  // start at time 0
    r.run_until(TIME("12:00:00:000"));  // run simulation until 12:00:00.000 (hh:mm:ss:msc) or appropriate format for NDTime

    return 0;
}
