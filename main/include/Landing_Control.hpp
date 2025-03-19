#ifndef LANDING_CONTROL_HPP
#define LANDING_CONTROL_HPP

#include "cadmium/modeling/devs/coupled.hpp"
#include "cadmium/lib/iestream.hpp"
#include "Landing_Queue.hpp"
#include "ParkingQueue.hpp"
#include "MaintenanceCheck.hpp" // 新增

using namespace cadmium;

struct Landing_Control : public Coupled {
    Landing_Control(const std::string& id)
      : Coupled(id)
    {
        std::string input_file = "./airplane.txt";

        // 1) 输入流
        auto plane_input_reader = addComponent<lib::IEStream<Plane>>(
            "plane_input_reader", 
            input_file.c_str()
        );

        // 2) 降落队列
        auto landing_queue = addComponent<LandingQueue>("landing_queue");
        
        // 3) 停机坪队列
        auto parking_queue = addComponent<ParkingQueue>("parking_queue");

        // 4) 维护检查
        auto maintenance_check = addComponent<MaintenanceCheck>("maintenance_check");

        // ---------- 耦合关系 -------------
        // plane_input_reader -> landing_queue
        addCoupling(plane_input_reader->out, landing_queue->plane_in);
        // landing_queue -> parking_queue
        addCoupling(landing_queue->landed_out, parking_queue->plane_in);
        // parking_queue -> maintenance_check
        addCoupling(parking_queue->parking_done_out, maintenance_check->plane_in);

        // maintenance_check -> 耦合模型的输出端口
        addIC(maintenance_check->plane_out, addOutPort<Plane>("final_out"));
        // 这样在顶层，这个耦合模型就有一个名字叫"final_out"的输出端口
        // 后续可以在日志、或别的耦合里使用
    }
};

#endif
