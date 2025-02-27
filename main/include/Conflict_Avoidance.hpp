#ifndef CONFLICT_AVOIDANCE_HPP
#define CONFLICT_AVOIDANCE_HPP

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>
#include <queue>
#include <limits>
#include "FlightInfo.hpp"

// **定义 Conflict_Avoidance 模型的输入和输出端口**
struct Conflict_Avoidance_defs {
    struct in_landed_aircraft : public cadmium::in_port<FlightInfo> {};  // 接收降落飞机
    struct in_departing_aircraft : public cadmium::in_port<FlightInfo> {};  // 接收待起飞飞机
    struct out_to_takeoff_queue : public cadmium::out_port<FlightInfo> {};  // 送至起飞队列
    struct out_to_gate : public cadmium::out_port<FlightInfo> {};  // 送至登机口
};

// **Conflict_Avoidance Atomic Model**
template<typename TIME>
class Conflict_Avoidance {
public:
    // **1. 定义输入和输出端口**
    using input_ports = std::tuple<typename Conflict_Avoidance_defs::in_landed_aircraft, typename Conflict_Avoidance_defs::in_departing_aircraft>;
    using output_ports = std::tuple<typename Conflict_Avoidance_defs::out_to_takeoff_queue, typename Conflict_Avoidance_defs::out_to_gate>;

    // **2. 定义状态**
    struct state_type {
        std::queue<FlightInfo> taxiway_queue;  // 滑行道 FIFO 队列
        bool active;  // 是否有正在滑行的飞机
        TIME next_internal;  // 下一个事件的时间
    };
    state_type state;

    // **3. 初始化构造函数**
    Conflict_Avoidance() {
        state.active = false;
        state.next_internal = std::numeric_limits<TIME>::infinity();  // 初始无事件
    }

    // **4. 外部事件 (External Transition Function)**
    void external_transition(TIME e, typename cadmium::make_message_bags<input_ports>::type mbs) {
        // 处理新降落的飞机（入场）
        for (const auto& flight : cadmium::get_messages<typename Conflict_Avoidance_defs::in_landed_aircraft>(mbs)) {
            state.taxiway_queue.push(flight);  // 加入滑行道队列
        }

        // 处理准备起飞的飞机（出场）
        for (const auto& flight : cadmium::get_messages<typename Conflict_Avoidance_defs::in_departing_aircraft>(mbs)) {
            state.taxiway_queue.push(flight);  // 加入滑行道队列
        }

        // 如果当前没有活动，且队列非空，则启动滑行处理
        if (!state.active && !state.taxiway_queue.empty()) {
            state.active = true;
            state.next_internal = TIME("00:00:20:000");  // 假设滑行需要 20 秒
        }
    }

    // **5. 内部事件 (Internal Transition Function)**
    void internal_transition() {
        if (!state.taxiway_queue.empty()) {
            state.taxiway_queue.pop();  // 移除滑行完成的飞机
        }

        if (!state.taxiway_queue.empty()) {
            state.next_internal = TIME("00:00:20:000");  // 继续处理下一个飞机
        } else {
            state.active = false;
            state.next_internal = std::numeric_limits<TIME>::infinity();
        }
    }

    // **6. 输出函数 (Output Function)**
    typename cadmium::make_message_bags<output_ports>::type output() const {
        typename cadmium::make_message_bags<output_ports>::type bags;
        
        if (!state.taxiway_queue.empty()) {
            FlightInfo next_flight = state.taxiway_queue.front();
            if (next_flight.is_departing) {
                cadmium::get_messages<typename Conflict_Avoidance_defs::out_to_takeoff_queue>(bags).push_back(next_flight);
            } else {
                cadmium::get_messages<typename Conflict_Avoidance_defs::out_to_gate>(bags).push_back(next_flight);
            }
        }

        return bags;
    }

    // **7. 时间推进函数 (Time Advance Function)**
    TIME time_advance() const {
        return state.next_internal;
    }
};

#endif // CONFLICT_AVOIDANCE_HPP
