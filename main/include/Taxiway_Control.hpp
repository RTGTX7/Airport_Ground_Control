#ifndef TAXIWAY_CONTROL_HPP
#define TAXIWAY_CONTROL_HPP

#include <cadmium/modeling/coupled_model.hpp>
#include <cadmium/modeling/ports.hpp>
#include "Conflict_Avoidance.hpp"
#include "FlightInfo.hpp"

// 定义 Taxiway_Control 的端口
struct Taxiway_Control_defs {
    struct in_plane_landed : public cadmium::in_port<FlightInfo> {};  // 接收降落完成的飞机
    struct in_departure_request : public cadmium::in_port<FlightInfo> {};  // 接收准备起飞的飞机
    struct out_ready_for_takeoff : public cadmium::out_port<FlightInfo> {};  // 发送可起飞的飞机
    struct out_arrival_done : public cadmium::out_port<FlightInfo> {};  // 发送滑行完成的飞机
};

// Taxiway_Control 耦合模型
struct Taxiway_Control {
    // **1. 定义输入和输出端口**
    using input_ports = std::tuple<
        Taxiway_Control_defs::in_plane_landed, 
        Taxiway_Control_defs::in_departure_request
    >;
    using output_ports = std::tuple<
        Taxiway_Control_defs::out_ready_for_takeoff, 
        Taxiway_Control_defs::out_arrival_done
    >;

    // **2. 定义子模型 (Conflict_Avoidance)**
    using submodels = cadmium::modeling::models_tuple<Conflict_Avoidance>;

    // **3. 外部输入耦合 (EIC)**
    using eics = std::tuple<
        cadmium::modeling::EIC<Taxiway_Control_defs::in_plane_landed, Conflict_Avoidance, Conflict_Avoidance_defs::in_landed_aircraft>,
        cadmium::modeling::EIC<Taxiway_Control_defs::in_departure_request, Conflict_Avoidance, Conflict_Avoidance_defs::in_departing_aircraft>
    >;

    // **4. 外部输出耦合 (EOC)**
    using eocs = std::tuple<
        cadmium::modeling::EOC<Conflict_Avoidance, Conflict_Avoidance_defs::out_to_takeoff_queue, Taxiway_Control_defs::out_ready_for_takeoff>,
        cadmium::modeling::EOC<Conflict_Avoidance, Conflict_Avoidance_defs::out_to_gate, Taxiway_Control_defs::out_arrival_done>
    >;

    // **5. 内部耦合 (IC)** -> 由于此模型只有一个子模型 Conflict_Avoidance，因此无内部耦合

    // **6. 组合 Coupled Model**
    template<typename TIME>
    using model = cadmium::modeling::coupled_model<TIME, input_ports, output_ports, submodels, eics, eocs>;
};

#endif // TAXIWAY_CONTROL_HPP
