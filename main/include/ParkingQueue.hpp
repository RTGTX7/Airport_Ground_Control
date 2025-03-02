#ifndef PARKING_QUEUE_HPP
#define PARKING_QUEUE_HPP

#include <iostream>
#include <string>
#include <queue>
#include <limits>
#include "cadmium/modeling/devs/atomic.hpp"
#include "Landing_Queue.hpp"  // 为了使用我们定义的 Plane 结构

using namespace cadmium;

/**
 * 停机坪状态结构
 */
struct ParkingQueueState {
    std::queue<Plane> planeQueue;
    bool busy;
    double sigma;

    ParkingQueueState()
        : busy(false),
          sigma(std::numeric_limits<double>::infinity()) {}
};

inline std::ostream& operator<<(std::ostream &os, const ParkingQueueState& s) {
    os << "ParkingQueueState("
       << "busy=" << s.busy << ","
       << "queue_size=" << s.planeQueue.size() << ","
       << "sigma=" << s.sigma << ")";
    return os;
}

/**
 * ParkingQueue 原子模型：
 * - 输入：Plane（落地后的飞机）
 * - 输出：Plane（停机坪流程结束的飞机）
 * - 服务时间: 5.0
 */
class ParkingQueue : public Atomic<ParkingQueueState> {
public:
    Port<Plane> plane_in;         // 接收落地后的飞机
    Port<Plane> parking_done_out; // 停机坪完成后输出飞机

    const double parking_time = 5.0;

    ParkingQueue(const std::string &id)
        : Atomic<ParkingQueueState>(id, ParkingQueueState()) {
        plane_in         = addInPort<Plane>("plane_in");
        parking_done_out = addOutPort<Plane>("parking_done_out");
    }

    void internalTransition(ParkingQueueState &s) const override {
        // 当前服务完成，队头出队
        if (!s.planeQueue.empty()) {
            s.planeQueue.pop();
        }
        if (!s.planeQueue.empty()) {
            s.busy  = true;
            s.sigma = parking_time;
        } else {
            s.busy  = false;
            s.sigma = std::numeric_limits<double>::infinity();
        }
    }

    void externalTransition(ParkingQueueState &s, double e) const override {
        // 先减去 e
        if (s.sigma != std::numeric_limits<double>::infinity()) {
            s.sigma -= e;
            if (s.sigma < 0) s.sigma = 0;
        }
        // 把新来的飞机加到队列
        for (auto &p : plane_in->getBag()) {
            s.planeQueue.push(p);
        }
        // 若空闲，则立即开始处理
        if (!s.busy && !s.planeQueue.empty()) {
            s.busy  = true;
            s.sigma = parking_time;
        }
    }

    void confluentTransition(ParkingQueueState &s, double e) const override {
        internalTransition(s);
        externalTransition(s, 0.0);
    }

    void output(const ParkingQueueState &s) const override {
        // 输出队头代表它刚完成停机处理
        if (!s.planeQueue.empty()) {
            Plane p = s.planeQueue.front();
            parking_done_out->addMessage(p);
        }
    }

    [[nodiscard]] double timeAdvance(const ParkingQueueState &s) const override {
        return s.sigma;
    }
};

#endif
