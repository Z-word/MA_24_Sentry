#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/int32.hpp>
#include <geometry_msgs/msg/point.hpp>
#include <rcl_interfaces/msg/set_parameters_result.hpp>  // 包含 SetParametersResult
#include <functional>  // std::function

class MySerialNode : public rclcpp::Node {
public:
    MySerialNode() : Node("my_serial") {
        // 声明参数并创建发布器、定时器等
        this->declare_parameter("my_hp", 100);        // 默认血量
        this->declare_parameter("my_ammunition", 400); // 默认弹丸数量
        this->declare_parameter("enemy_x", 10.0);     // 默认敌人X位置
        this->declare_parameter("enemy_y", 5.0);      // 默认敌人Y位置
        this->declare_parameter("enemy_z", 0.0);      // 默认敌人Z位置

        // 创建发布器
        hp_publisher_ = this->create_publisher<std_msgs::msg::Int32>("/my_hp", 10);
        ammunition_publisher_ = this->create_publisher<std_msgs::msg::Int32>("/my_ammunition", 10);
        enemy_point_publisher_ = this->create_publisher<geometry_msgs::msg::Point>("/enemy_point", 10);

        // 创建定时器，每秒发布数据
        timer_ = this->create_wall_timer(
            std::chrono::seconds(1),
            std::bind(&MySerialNode::publishData, this));

        // 注册参数变化回调
        auto callback_handle = this->add_on_set_parameters_callback(
            std::bind(&MySerialNode::onParameterChanged, this, std::placeholders::_1));
        
        // 如果不打算使用返回值，可以选择简单地丢弃它。
        (void)callback_handle;
    }

private:
    // 修改回调函数签名，返回 SetParametersResult
    rcl_interfaces::msg::SetParametersResult onParameterChanged(
        const std::vector<rclcpp::Parameter> &parameters) {
        
        rcl_interfaces::msg::SetParametersResult result;
        
        // 遍历每个参数并记录变化
        for (const auto &param : parameters) {
            RCLCPP_INFO(this->get_logger(), "Parameter %s changed to %s", 
                        param.get_name().c_str(), param.value_to_string().c_str());
        }
        
        result.successful = true;  // 返回成功
        result.reason = "Successfully handled parameter changes";  // 说明
        return result;  // 返回 SetParametersResult
    }

    // 发布数据
    void publishData() {
        auto hp_msg = std_msgs::msg::Int32();
        hp_msg.data = this->get_parameter("my_hp").as_int();  // 从参数获取血量
        hp_publisher_->publish(hp_msg);

        auto ammunition_msg = std_msgs::msg::Int32();
        ammunition_msg.data = this->get_parameter("my_ammunition").as_int();  // 从参数获取弹丸数量
        ammunition_publisher_->publish(ammunition_msg);

        auto enemy_msg = geometry_msgs::msg::Point();
        enemy_msg.x = this->get_parameter("enemy_x").as_double();  // 从参数获取敌人位置
        enemy_msg.y = this->get_parameter("enemy_y").as_double();
        enemy_msg.z = this->get_parameter("enemy_z").as_double();
        enemy_point_publisher_->publish(enemy_msg);
    }

    // 发布器和定时器
    rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr hp_publisher_;
    rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr ammunition_publisher_;
    rclcpp::Publisher<geometry_msgs::msg::Point>::SharedPtr enemy_point_publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MySerialNode>());
    rclcpp::shutdown();
    return 0;
}
