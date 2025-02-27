#include <cadmium/engine/pdevs_runner.hpp>
#include <cadmium/logger/common_loggers.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include "Airport_Control.hpp"
#include "FlightInfo.hpp"

using namespace std;
using TIME = NDTime;

int main() {
    // 1. 打开 input_data.txt
    ifstream infile("input_data.txt");
    if (!infile) {
        cerr << "Error: Cannot open input_data.txt" << endl;
        return 1;
    }

    // 2. 创建 Airport_Control 实例
    using AirportModel = Airport_Control::model<TIME>;
    cadmium::engine::runner<TIME, AirportModel> r{{0}};

    // 3. 解析 input_data.txt
    string line;
    while (getline(infile, line)) {
        stringstream ss(line);
        string type, flight_id, airline, aircraft_type, time;
        int fuel_level = 0;
        
        ss >> type >> flight_id >> airline >> aircraft_type >> time;
        
        if (type == "Arrival") {
            ss >> fuel_level;  // 进港飞机有燃油信息
            FlightInfo flight(flight_id, airline, aircraft_type, time, false, fuel_level);
            
            // 送入 Landing_Control
            r.run_until_passivate();
            r.insert_input(Airport_Control_defs::in_new_arrival{}, flight);
        } 
        else if (type == "Departure") {
            FlightInfo flight(flight_id, airline, aircraft_type, time, true);
            
            // 送入 Takeoff_Control
            r.run_until_passivate();
            r.insert_input(Airport_Control_defs::in_new_departure{}, flight);
        }
    }

    // 4. 运行模拟
    r.run_until(TIME("12:00:00:000"));

    return 0;
}
