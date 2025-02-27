#ifndef TAKEOFF_QUEUE_HPP
#define TAKEOFF_QUEUE_HPP

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>
#include <queue>
#include <limits>
#include "FlightInfo.hpp"

// 定义 Takeoff_Queue 模型的输入和输出端口
struct Takeoff_Queue_defs {
    struct in_new_flight : public cadmium::in_port<FlightInfo> {};  // 接收新加入的待起飞飞机
    struct out_takeoff_done : public cadmium::out_port<FlightInfo> {};  // 发送起飞完成的飞机
};

// Takeoff_Queue 是一个 Atomic Model，管理 FIFO 排队的飞机
template<typename TIME>
class Takeoff_Queue {
public:
    // **1. 定义输入和输出端口**
    using input_ports = std::tuple<typename Takeoff_Queue_defs::in_new_flight>;
    using output_ports = std::tuple<typename Takeoff_Queue_defs::out_takeoff_done>;

    // **2. 定义状态**
    struct state_type {
        std::queue<FlightInfo> queue;  // FIFO 队列
        bool active;  // 当前是否有飞机在等待起飞
        TIME next_internal;  // 下一个内部事件的时间（即下一个飞机何时起飞）
    };
    state_type state;

    // **3. 初始化构造函数**
    Takeoff_Queue() {
        state.active = false;
        state.next_internal = std::numeric_limits<TIME>::infinity();  // 初始无事件
    }

    // **4. 外部事件 (External Transition Function)**
    void external_transition(TIME e, typename cadmium::make_message_bags<input_ports>::type mbs) {
        for (const auto& flight : cadmium::get_messages<typename Takeoff_Queue_defs::in_new_flight>(mbs)) {
            state.queue.push(flight);  // 新飞机加入队列
        }
        if (!state.active && !state.queue.empty()) {
            state.active = true;
            state.next_internal = TIME("00:00:30:000");  // 假设每架飞机起飞需要 30 秒
        }
    }

    // **5. 内部事件 (Internal Transition Function)**
    void internal_transition() {
        if (!state.queue.empty()) {
            state.queue.pop();  // 移除已起飞的飞机
        }
        if (!state.queue.empty()) {
            state.next_internal = TIME("00:00:30:000");  // 继续处理下一个飞机
        } else {
            state.active = false;
            state.next_internal = std::numeric_limits<TIME>::infinity();
        }
    }

    // **6. 输出函数 (Output Function)**
    typename cadmium::make_message_bags<output_ports>::type output() const {
        typename cadmium::make_message_bags<output_ports>::type bags;
        if (!state.queue.empty()) {
            cadmium::get_messages<typename Takeoff_Queue_defs::out_takeoff_done>(bags).push_back(state.queue.front());
        }
        return bags;
    }

    // **7. 时间推进函数 (Time Advance Function)**
    TIME time_advance() const {
        return state.next_internal;
    }
};

#endif // TAKEOFF_QUEUE_HPP
