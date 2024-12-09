#include "behaviortree_cpp_v3/behavior_tree.h"
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"

class CheckAmmunitionCount : public BT::ConditionNode
{
public:
  CheckAmmunitionCount(const std::string &name, const BT::NodeConfiguration &config)
      : BT::ConditionNode(name, config)
  {
    // 创建一个ROS节点并设置参数
    node_ = rclcpp::Node::make_shared("check_ammunition_count");
    node_->declare_parameter<int>("ammunition_count", 400);  // 初始化弹丸数量

    // 创建订阅器来接收弹丸数量
    ammunition_subscriber_ = node_->create_subscription<std_msgs::msg::Int32>(
        "/my_ammunition", 10,
        std::bind(&CheckAmmunitionCount::onAmmunitionReceived, this, std::placeholders::_1));

        // 创建一个线程来处理ROS事件循环
        spin_thread_ = std::thread([this]() {rclcpp::spin(node_);});

    RCLCPP_INFO(node_->get_logger(), "CheckAmmunitionCount node created and subscribed to my_ammunition topic.");
  }

  static BT::PortsList providedPorts()
  {
    return {
        BT::InputPort<int>("threshold", 30, "Minimum ammunition threshold")};
  }

  BT::NodeStatus tick() override
  {
    // 获取阈值
    int threshold;
    if (!getInput("threshold", threshold))
    {
      RCLCPP_ERROR(node_->get_logger(), "Threshold not provided");
      return BT::NodeStatus::FAILURE;
    }

    // 如果弹丸数量尚未接收到，返回RUNNING
    if (!has_received_ammo_)
    {
      RCLCPP_INFO(node_->get_logger(), "Waiting for ammunition data...");
      return BT::NodeStatus::RUNNING;  // 等待数据
      //return BT::NodeStatus::SUCCESS;  // 等待数据
    }

    RCLCPP_INFO(node_->get_logger(), "Current ammunition count: %d", ammunition_count_);

    // 判断弹丸数量是否低于阈值
    if (ammunition_count_ < threshold)
    {
      RCLCPP_WARN(node_->get_logger(), "Ammunition below threshold!");
      return BT::NodeStatus::FAILURE;
    }

    return BT::NodeStatus::SUCCESS;
  }

  ~CheckAmmunitionCount()
  {
    rclcpp::shutdown();
    if (spin_thread_.joinable())
    {
      spin_thread_.join();
    }
  }


private:
  rclcpp::Node::SharedPtr node_;
  rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr ammunition_subscriber_;
  int ammunition_count_ = 0;  // 使用类成员变量来存储弹丸数量
  bool has_received_ammo_ = false;  // 标记是否接收到弹丸数据
  std::thread spin_thread_; // 声明 spin_thread_ 变量

  // 订阅到弹丸数量后回调
  void onAmmunitionReceived(const std_msgs::msg::Int32::SharedPtr msg)
  {
    ammunition_count_ = msg->data;
    has_received_ammo_ = true;
    RCLCPP_INFO(node_->get_logger(), "Received ammunition count: %d", ammunition_count_);
  }
};
