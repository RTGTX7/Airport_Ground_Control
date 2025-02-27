#ifndef LANDING_CONTROL_HPP
#define LANDING_CONTROL_HPP

#include <cadmium/modeling/coupled_model.hpp>
#include <cadmium/modeling/ports.hpp>
#include "Landing_Queue.hpp"

// 定义 Landing_Control 端口
struct Landing_Control_defs {
    struct in_new_arrival : public cadmium::in_port<FlightInfo> {};  // 接收新的降落请求
    struct out_landing_done : public cadmium::out_port<FlightInfo> {};  // 飞机降落完成
};

class Landing_Control {
public:
    // **1. 定义输入和输出端口**
    using input_ports = std::tuple<typename Landing_Control_defs::in_new_arrival>;
    using output_ports = std::tuple<typename Landing_Control_defs::out_landing_done>;

    // **2. 定义子模型 (Landing_Queue)**
    using submodels = cadmium::modeling::models_tuple<Landing_Queue>;

    // **3. 外部输入耦合 (EIC)**
    using eics = std::tuple<
        cadmium::modeling::EIC< Landing_Control_defs::in_new_arrival, Landing_Queue, Landing_Queue_defs::in_landing_request >
    >;

    // **4. 外部输出耦合 (EOC)**
    using eocs = std::tuple<
        cadmium::modeling::EOC< Landing_Queue, Landing_Queue_defs::out_landing_complete, Landing_Control_defs::out_landing_done >
    >;

    // **5. 无内部耦合 (IC)**，只有一个子模型，无需内部耦合

    // **6. 组合 Coupled Model**
    template<typename TIME>
    using model = cadmium::modeling::coupled_model<TIME, input_ports, output_ports, submodels, eics, eocs>;
};

#endif // LANDING_CONTROL_HPP
