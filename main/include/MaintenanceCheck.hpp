#ifndef MAINTENANCE_CHECK_HPP
#define MAINTENANCE_CHECK_HPP

#include <iostream>
#include <string>
#include <limits>
#include "cadmium/modeling/devs/atomic.hpp"
#include "Landing_Queue.hpp" // 为了使用 Plane 结构

/**
 * MaintenanceCheckState:
 * - hasPlane: 是否当前正在检查/维护某架飞机
 * - plane: 如果 hasPlane=true，这里存储被检查的飞机
 * - sigma: 剩余时间
 */
struct MaintenanceCheckState {
    bool hasPlane;
    Plane plane;
    double sigma;

    MaintenanceCheckState()
      : hasPlane(false),
        sigma(std::numeric_limits<double>::infinity()) {}
};

inline std::ostream& operator<<(std::ostream &os, const MaintenanceCheckState& s) {
    os << "MaintenanceCheckState("
       << "hasPlane=" << s.hasPlane << ","
       << "plane={" << s.plane << "},"
       << "sigma=" << s.sigma << ")";
    return os;
}

/**
 * MaintenanceCheck 原子模型：
 * - 输入：plane_in（来自停机坪或其他地方）
 * - 输出：plane_out（检查完成后的飞机）
 * - 逻辑：如果油量 < 30，需要维护/加油时间 15.0，否则立即通过(设个很短的delay，比如1.0)
 */
class MaintenanceCheck : public cadmium::Atomic<MaintenanceCheckState> {
public:
    // 输入输出端口
    cadmium::Port<Plane> plane_in;
    cadmium::Port<Plane> plane_out;

    // 设定两种检查时间
    const double maintenance_time = 15.0; // 油量低时，需要维护
    const double quick_time = 1.0;        // 油量足时，仅快速检查

    // 构造函数
    MaintenanceCheck(const std::string& id)
      : cadmium::Atomic<MaintenanceCheckState>(id, MaintenanceCheckState())
    {
        plane_in  = addInPort<Plane>("plane_in");
        plane_out = addOutPort<Plane>("plane_out");
    }

    // internalTransition: 当检查时间到 -> 输出后，重置状态
    void internalTransition(MaintenanceCheckState &s) const override {
        // 处理结束后，当前不再持有飞机
        s.hasPlane = false;
        s.sigma = std::numeric_limits<double>::infinity();
    }

    // externalTransition: 当有新飞机到达，若当前空闲，则接受并进入“检查”阶段
    void externalTransition(MaintenanceCheckState &s, double e) const override {
        // 如果正在计时，则先减去 e
        if (s.sigma != std::numeric_limits<double>::infinity()) {
            s.sigma -= e;
            if (s.sigma < 0) s.sigma = 0; 
        }

        // 读取输入端口
        for (auto &p : plane_in->getBag()) {
            // 如果目前是空闲，则启动检查
            if (!s.hasPlane) {
                s.plane = p;
                s.hasPlane = true;
                // 判断油量决定检查时间
                if (p.fuel < 30) {
                    s.sigma = maintenance_time;
                } else {
                    s.sigma = quick_time;
                }
            } else {
                // 如果你想模拟一次只能检查一架，那么多余的飞机要么丢弃、
                // 要么排队，可自由发挥
                // 这里演示“丢弃”或“忽略”，也可以自己加队列
                std::cerr << "[MaintenanceCheck] Warning: received another plane while busy. Ignoring.\n";
            }
        }
    }

    // confluentTransition: 同时有internal+external事件时调用
    void confluentTransition(MaintenanceCheckState &s, double e) const override {
        // 通常做法：先 internal，再 external
        internalTransition(s);
        externalTransition(s, 0.0);
    }

    // output: 当 internal transition 触发时，输出当前检查完成的飞机
    void output(const MaintenanceCheckState &s) const override {
        if (s.hasPlane) {
            plane_out->addMessage(s.plane);
        }
    }

    // timeAdvance
    [[nodiscard]] double timeAdvance(const MaintenanceCheckState &s) const override {
        return s.sigma;
    }
};

#endif
