// Copyright 2016 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using std::placeholders::_1;

/* This example creates a subclass of Node and uses std::bind() to register a
 * member function as a callback from the timer. */

class MinimalPublisher : public rclcpp::Node
{
public:
  MinimalPublisher()
  : Node("minimal_publisher")
  {
    publisher1_ = this->create_publisher<std_msgs::msg::String>("self_msg_transfer", 10);
    publisher2_ = this->create_publisher<std_msgs::msg::String>("self_msg_sendout", 10);
    subscriber1_ = this->create_subscription<std_msgs::msg::String>(
      "self_autoware_msg_receive", 10, std::bind(&MinimalPublisher::rece_callback, this, _1));
    subscriber2_ = this->create_subscription<std_msgs::msg::String>(
      "self_transfer_back", 10, std::bind(&MinimalPublisher::ack_callback, this, _1));

  }

private:
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher1_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher2_;

  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscriber1_;
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscriber2_;
  void rece_callback(const std_msgs::msg::String & msg) const
  {
    RCLCPP_INFO(this->get_logger(), "One message is received from autoware: '%s'", msg.data.c_str());
    publisher1_->publish(msg);
  }
  void ack_callback(const std_msgs::msg::String & msg) const
  {
    RCLCPP_INFO(this->get_logger(), "One message is received from cloud: '%s'", msg.data.c_str());
    publisher2_->publish(msg);
  }
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MinimalPublisher>());
  rclcpp::shutdown();
  return 0;
}
