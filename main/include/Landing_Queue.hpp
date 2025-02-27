#ifndef LANDING_QUEUE_HPP
#define LANDING_QUEUE_HPP

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>
#include <queue>
#include <vector>
#include <limits>
#include "FlightInfo.hpp"

// 定义 Landing_Queue 模型的输入和输出端口
struct Landing_Queue_defs {
    struct in_landing_request : public cadmium::in_port<FlightInfo> {};  // 接收新的降落请求
    struct out_landing_complete : public cadmium::out_port<FlightInfo> {};  // 降落完成通知
};

// 自定义比较函数，按燃油优先级排序（燃油越少，优先级越高）
struct FuelPriorityComparator {
    bool operator()(const FlightInfo& a, const FlightInfo& b) {
        return a.fuel_level > b.fuel_level;  // 燃油少的飞机优先降落
    }
};

template<typename TIME>
class Landing_Queue {
public:
    // **1. 定义输入和输出端口**
    using input_ports = std::tuple<typename Landing_Queue_defs::in_landing_request>;
    using output_ports = std::tuple<typename Landing_Queue_defs::out_landing_complete>;

    // **2. 定义状态**
    struct state_type {
        std::priority_queue<FlightInfo, std::vector<FlightInfo>, FuelPriorityComparator> landing_queue;  // 优先级队列
        bool active;  // 是否有飞机在等待降落
        TIME next_internal;  // 下一个降落事件的时间
    };
    state_type state;

    // **3. 初始化构造函数**
    Landing_Queue() {
        state.active = false;
        state.next_internal = std::numeric_limits<TIME>::infinity();  // 初始无事件
    }

    // **4. 外部事件 (External Transition Function)**
    void external_transition(TIME e, typename cadmium::make_message_bags<input_ports>::type mbs) {
        for (const auto& flight : cadmium::get_messages<typename Landing_Queue_defs::in_landing_request>(mbs)) {
            state.landing_queue.push(flight);  // 按燃油优先级加入降落队列
        }
        if (!state.active && !state.landing_queue.empty()) {
            state.active = true;
            state.next_internal = TIME("00:01:00:000");  // 假设降落需要 1 分钟
        }
    }

    // **5. 内部事件 (Internal Transition Function)**
    void internal_transition() {
        if (!state.landing_queue.empty()) {
            state.landing_queue.pop();  // 移除已降落的飞机
        }
        if (!state.landing_queue.empty()) {
            state.next_internal = TIME("00:01:00:000");  // 继续处理下一个飞机
        } else {
            state.active = false;
            state.next_internal = std::numeric_limits<TIME>::infinity();
        }
    }

    // **6. 输出函数 (Output Function)**
    typename cadmium::make_message_bags<output_ports>::type output() const {
        typename cadmium::make_message_bags<output_ports>::type bags;
        if (!state.landing_queue.empty()) {
            cadmium::get_messages<typename Landing_Queue_defs::out_landing_complete>(bags).push_back(state.landing_queue.top());
        }
        return bags;
    }

    // **7. 时间推进函数 (Time Advance Function)**
    TIME time_advance() const {
        return state.next_internal;
    }
};

#endif // LANDING_QUEUE_HPP
