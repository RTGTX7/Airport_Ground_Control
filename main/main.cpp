#include <iostream>
#include <string>
#include <memory>
#include <limits>
#include "Landing_Control.hpp"

// Cadmium v2 相关头文件
#include "cadmium/simulation/root_coordinator.hpp"
#include "cadmium/simulation/logger/stdout.hpp"
#include "cadmium/simulation/logger/csv.hpp"

using namespace cadmium;

int main(int argc, char* argv[]) {
    // 创建顶层模型实例
    auto model = std::make_shared<Landing_Control>("Landing_Control");


    // 创建 RootCoordinator
    auto rootCoordinator = RootCoordinator(model);

    // 设定日志记录器（可以选择 CSV 或 STDOUT）
    // rootCoordinator.setLogger<STDOUTLogger>(";");
    rootCoordinator.setLogger<CSVLogger>("airport_control_log.csv", ";");

    // 启动仿真
    rootCoordinator.start();
    rootCoordinator.simulate(60.1);
    rootCoordinator.stop();

    return 0;
}
