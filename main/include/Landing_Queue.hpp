#ifndef LANDING_QUEUE_HPP
#define LANDING_QUEUE_HPP

#include <iostream>
#include <string>
#include <deque>   // 由queue改成deque
#include <vector>
#include <sstream>
#include "cadmium/modeling/devs/atomic.hpp"
#include <limits>
#include <stdexcept>
#include <cstdio>
#include <iomanip>

using namespace cadmium;

/**
 * Plane 结构与 operator<<、operator>> 同原先，保持不变
 */
struct Plane {
    std::string status;        
    std::string flightNumber;  
    std::string airline;       
    std::string aircraftType;  
    std::string arrivalTime;   
    int fuel;                  

    Plane()
        : status(""), flightNumber(""), airline(""),
          aircraftType(""), arrivalTime(""), fuel(0) {}
};

inline std::ostream& operator<<(std::ostream& os, const Plane& p) {
    os << p.status << ","
       << p.flightNumber << ","
       << p.airline << ","
       << p.aircraftType << ","
       << p.arrivalTime << ","
       << p.fuel;
    return os;
}

inline std::istream& operator>>(std::istream& is, Plane& plane) {
    std::string line;
    if (!std::getline(is, line)) {
        return is; // EOF or read error
    }
    // 跳过空行或注释行
    if (line.empty() || line[0] == '#') {
        return is;
    }

    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> tokens;
    while (std::getline(ss, token, ',')) {
        tokens.push_back(token);
    }

    if (tokens.size() < 5) {
        // 不合规行，可自行处理
        return is;
    }

    plane.status        = tokens[0];
    plane.flightNumber  = tokens[1];
    plane.airline       = tokens[2];
    plane.aircraftType  = tokens[3];
    plane.arrivalTime   = tokens[4];
    plane.fuel          = 0;
    if (tokens.size() >= 6) {
        try {
            plane.fuel = std::stoi(tokens[5]);
        } catch (...) {
            plane.fuel = 0;
        }
    }

    return is;
}

/**
 * 这里实现LandingQueue时，用std::deque来支持“紧急插队”
 */
struct LandingQueueState {
    // 由 queue 改成 deque
    std::deque<Plane> planeQueue;  
    bool busy;   
    double sigma; 

    LandingQueueState()
        : busy(false),
          sigma(std::numeric_limits<double>::infinity()) {}
};

inline std::ostream& operator<<(std::ostream &out, const LandingQueueState& s) {
    out << "LandingQueueState("
        << "busy=" << s.busy << ","
        << "queue_size=" << s.planeQueue.size() << ","
        << "sigma=" << s.sigma << ")";
    return out;
}

class LandingQueue : public Atomic<LandingQueueState> {
public:
    Port<Plane> plane_in;    // 有飞机到达
    Port<Plane> landed_out;  // 有飞机完成降落

    // 固定降落时间
    const double landing_time = 10.0;

    LandingQueue(const std::string& id)
        : Atomic<LandingQueueState>(id, LandingQueueState())
    {
        plane_in    = addInPort<Plane>("plane_in");
        landed_out  = addOutPort<Plane>("landed_out");
    }

    // internalTransition
    void internalTransition(LandingQueueState& s) const override {
        // 弹出队头飞机(它降落完成)
        if (!s.planeQueue.empty()) {
            s.planeQueue.pop_front();  // 注意这里是 pop_front()
        }
        // 如果仍有飞机，则继续下一架
        if (!s.planeQueue.empty()) {
            s.busy = true;
            s.sigma = landing_time;
        } else {
            s.busy = false;
            s.sigma = std::numeric_limits<double>::infinity();
        }
    }

    // externalTransition
    void externalTransition(LandingQueueState& s, double e) const override {
        if (s.sigma != std::numeric_limits<double>::infinity()) {
            s.sigma -= e;
            if (s.sigma < 0) s.sigma = 0;
        }

        // 将到达的飞机加入队列；
        // 如果 fuel<5，则紧急插队放在前面，否则正常排后面
        for (auto &newPlane : plane_in->getBag()) {
            if (newPlane.fuel < 5) {
                // 紧急插队
                s.planeQueue.push_front(newPlane);
            } else {
                // 普通情况
                s.planeQueue.push_back(newPlane);
            }
        }

        // 若原本不忙且队列非空，则开始降落
        if (!s.busy && !s.planeQueue.empty()) {
            s.busy = true;
            s.sigma = landing_time;
        }
    }

    // confluentTransition
    void confluentTransition(LandingQueueState& s, double e) const override {
        internalTransition(s);
        externalTransition(s, 0.0);
    }

    // output
    void output(const LandingQueueState& s) const override {
        if (!s.planeQueue.empty()) {
            // 输出队头飞机，表示它刚好落地完成
            Plane landedPlane = s.planeQueue.front();
            landed_out->addMessage(landedPlane);
        }
    }

    double timeAdvance(const LandingQueueState& s) const override {
        return s.sigma;
    }

};

#endif // LANDING_QUEUE_HPP
