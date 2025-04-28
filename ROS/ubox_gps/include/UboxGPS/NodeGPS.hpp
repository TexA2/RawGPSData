#include "UboxGPS.h"
#include <unistd.h>
#include <chrono>


#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/nav_sat_fix.hpp"
#include "std_msgs/msg/string.hpp"


using namespace std::chrono_literals;

class NodeUbox : public UsbGpsListener , public rclcpp::Node
{
public:
    NodeUbox() : UsbGpsListener(), Node("UboxGPS_pub")
    {
        publisher_ = this->create_publisher<sensor_msgs::msg::NavSatFix>("/fix", 10);

        timer_ = this->create_wall_timer(
            100ms,
            [this]()
            {
                    ReadGpsData();
                    this->publisher_->publish(GNSSdata);
            });
    }

private:
    void GpsDataPrint() override;

    rclcpp::Publisher<sensor_msgs::msg::NavSatFix>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;

    sensor_msgs::msg::NavSatFix GNSSdata;
};


void
NodeUbox::GpsDataPrint()
{
    GNSSdata.header.stamp = this->get_clock()->now();

    GNSSdata.status.status = -1;
    GNSSdata.status.service = 1;

    GNSSdata.latitude  = getLatitude();
    GNSSdata.longitude = getLongitude();
    GNSSdata.altitude  = getAltitude();

    GNSSdata.position_covariance = {0,0,0,
                                    0,0,0,
                                    0,0,0};

    GNSSdata.position_covariance_type = 0;


    std::string str(buffer);

    if ( str.size() > 1)
    {
        auto message = std_msgs::msg::String();
        message.data = str;
        RCLCPP_INFO(this->get_logger(), "Получено: %s", message.data.c_str());
    }
}