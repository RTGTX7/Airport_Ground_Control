#ifndef FLIGHT_INFO_HPP
#define FLIGHT_INFO_HPP

#include <string>
#include <iostream>

struct FlightInfo {
    std::string flight_id;   // 航班编号
    std::string airline;     // 航空公司
    std::string aircraft_type; // 飞机类型
    std::string arrival_time; // 预计到达/起飞时间
    bool is_departing;       // 是否为出港飞机
    int fuel_level;          // 燃油量（仅进港飞机使用）

    // 构造函数
    FlightInfo() : flight_id(""), airline(""), aircraft_type(""), arrival_time(""), is_departing(false), fuel_level(0) {}

    FlightInfo(std::string id, std::string air, std::string type, std::string time, bool depart, int fuel = 0)
        : flight_id(id), airline(air), aircraft_type(type), arrival_time(time), is_departing(depart), fuel_level(fuel) {}

    // 重载 == 运算符
    bool operator==(const FlightInfo& other) const {
        return (flight_id == other.flight_id &&
                airline == other.airline &&
                aircraft_type == other.aircraft_type &&
                arrival_time == other.arrival_time &&
                is_departing == other.is_departing &&
                fuel_level == other.fuel_level);
    }

    // 友元函数，支持输出 FlightInfo
    friend std::ostream& operator<<(std::ostream& os, const FlightInfo& flight) {
        os << "[Flight ID: " << flight.flight_id
           << ", Airline: " << flight.airline
           << ", Aircraft: " << flight.aircraft_type
           << ", Time: " << flight.arrival_time
           << ", Departing: " << (flight.is_departing ? "Yes" : "No");
        if (!flight.is_departing) os << ", Fuel: " << flight.fuel_level;
        os << "]";
        return os;
    }
};

#endif // FLIGHT_INFO_HPP
