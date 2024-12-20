#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include <rclcpp_components/register_node_macro.hpp>
#include <chrono>

class TopicSubscribe02:public rclcpp::Node
{
public:
  //构造函数，有一个参数为节点名称
  TopicSubscribe02(const rclcpp::NodeOptions & options):Node("xyz", options), clock_(RCL_ROS_TIME)
  {
    // 创建一个订阅者订阅话题
    command_subscribe_ = this->create_subscription<std_msgs::msg::String>(
      "whisper_transcription", 10, std::bind(&TopicSubscribe02::command_callback, this, std::placeholders::_1));
    
    // 创建一个发布者发布话题
    command_publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
    // 创建一个定时器，每10毫秒调用一次timer_callback函数
    timer_ = this->create_wall_timer(
      std::chrono::milliseconds(10), std::bind(&TopicSubscribe02::timer_callback, this));
    
    // 记录开始时间
    linear_start_time_ = clock_.now();
    angular_start_time_ = clock_.now();
  }

private:
  // 声明一个订阅者
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr command_subscribe_;
  // 声明一个发布者
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr command_publisher_;
  // 声明一个定时器
  rclcpp::TimerBase::SharedPtr timer_;
  // 存储最新的twist消息
  geometry_msgs::msg::Twist latest_twist_msg_;
  // 记录线速度和角速度的开始时间
  rclcpp::Time linear_start_time_;
  rclcpp::Time angular_start_time_;
  // 声明一个时钟
  rclcpp::Clock clock_;
  // 收到话题数据的回调函数
  void command_callback(const std_msgs::msg::String::SharedPtr msg)
  {
    if (msg->data == "Forward." || msg->data == "- Forward." || msg->data == "Go ahead." || msg->data == "Go straight." || msg->data == "Go forward." || msg->data == "straight." || msg->data == "Straight." || msg->data == "straight")
    {
      latest_twist_msg_.linear.x = 0.15;
      latest_twist_msg_.angular.z = 0.0;
      linear_start_time_ = clock_.now(); // 重置线速度开始时间
    }
    else if (msg->data == "Backward." || msg->data == "Go back." || msg->data == "Go backward.")
    {
      latest_twist_msg_.linear.x = -0.15;
      latest_twist_msg_.angular.z = 0.0;
      linear_start_time_ = clock_.now(); // 重置线速度开始时间
    }
    else if (msg->data == "Turn left." || msg->data == "Left." || msg->data == "left" || msg->data == "left.")
    {
      latest_twist_msg_.linear.x = 0.0;
      latest_twist_msg_.angular.z = 1.7;
      angular_start_time_ = clock_.now(); // 重置角速度开始时间
    }
    else if (msg->data == "Turn right." || msg->data == "Right." || msg->data == "right." || msg->data == "right" || msg->data == "- Right.")
    {
      latest_twist_msg_.linear.x = 0.0;
      latest_twist_msg_.angular.z = -1.7;
      angular_start_time_ = clock_.now(); // 重置角速度开始时间
    }
    else
    {
      latest_twist_msg_.linear.x = 0.0;
      latest_twist_msg_.angular.z = 0.0;
    }
    
    // 输出日志
    RCLCPP_INFO(this->get_logger(), "收到消息：%s, 更新速度：linear.x=%f, angular.z=%f", msg->data.c_str(), latest_twist_msg_.linear.x, latest_twist_msg_.angular.z);
  }

  // 定时器回调函数
  void timer_callback()
  {
    // 检查线速度是否已经超过3秒
    if (latest_twist_msg_.linear.x != 0.0 && (clock_.now() - linear_start_time_).seconds() > 3.0)
    {
      // 停止发布线速度
      latest_twist_msg_.linear.x = 0.0;
      RCLCPP_INFO(this->get_logger(), "已超过3秒，停止发布线速度命令");
      command_publisher_->publish(latest_twist_msg_);
      // 重置线速度开始时间
      linear_start_time_ = clock_.now();
    }
    // 检查角速度是否已经超过1秒
    else if (latest_twist_msg_.angular.z != 0.0 && (clock_.now() - angular_start_time_).seconds() > 1.0)
    {
      // 停止发布角速度
      latest_twist_msg_.angular.z = 0.0;
      RCLCPP_INFO(this->get_logger(), "已超过1秒，停止发布角速度命令");
      command_publisher_->publish(latest_twist_msg_);
      // 重置角速度开始时间
      angular_start_time_ = clock_.now();
    }
    else
    {
      // 发布最新的twist消息
      command_publisher_->publish(latest_twist_msg_);
    }
  }
};

RCLCPP_COMPONENTS_REGISTER_NODE(TopicSubscribe02)