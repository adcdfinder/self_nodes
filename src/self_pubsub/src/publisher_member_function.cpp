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
#include <condition_variable>
#include <mutex>


#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "sensor_msgs/msg/point_cloud2.hpp"

using std::placeholders::_1;
using namespace std;

/* This example creates a subclass of Node and uses std::bind() to register a
 * member function as a callback from the timer. */

class MinimalPublisher : public rclcpp::Node
{
public:

  MinimalPublisher()
  : Node("minimal_publisher")
  {
    publisher1_ = this->create_publisher<sensor_msgs::msg::PointCloud2>("self_msg_transfer", 10);
    publisher2_ = this->create_publisher<sensor_msgs::msg::PointCloud2>("self_msg_sendout", 10);


    callback_group_subscriber1_ = this->create_callback_group(
      rclcpp::CallbackGroupType::MutuallyExclusive);
    callback_group_subscriber2_ = this->create_callback_group(
      rclcpp::CallbackGroupType::MutuallyExclusive);

    auto sub1_opt = rclcpp::SubscriptionOptions();
    sub1_opt.callback_group = callback_group_subscriber1_;
    auto sub2_opt = rclcpp::SubscriptionOptions();
    sub2_opt.callback_group = callback_group_subscriber2_;

    subscriber1_ = this->create_subscription<sensor_msgs::msg::PointCloud2>(
      "self_autoware_msg_receive", 10, std::bind(&MinimalPublisher::rece_callback, this, _1), sub1_opt);
    subscriber2_ = this->create_subscription<sensor_msgs::msg::PointCloud2>(
      "self_transfer_back", 10, std::bind(&MinimalPublisher::ack_callback, this, _1), sub2_opt);

  }

private:
  rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr publisher1_;
  rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr publisher2_;

  rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr subscriber1_;
  rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr subscriber2_;
  rclcpp::CallbackGroup::SharedPtr callback_group_subscriber1_;
  rclcpp::CallbackGroup::SharedPtr callback_group_subscriber2_;
  mutex m;
  condition_variable cv;
  bool processed = false;

  void rece_callback(const sensor_msgs::msg::PointCloud2 & msg)
  {
    RCLCPP_INFO(this->get_logger(), "One message is received from autoware");
    publisher1_->publish(msg);
    {

      std::unique_lock lk(m);
      cv.wait(lk, [&]{return processed;});
    }
    publisher2_->publish(msg);
    RCLCPP_INFO(this->get_logger(), "Publish to autoware back");
    this->processed = false;

  }
  void ack_callback(const sensor_msgs::msg::PointCloud2 & msg)
  {
    RCLCPP_INFO(this->get_logger(), "One message is received from cloud");
    this->processed = true;
    std::unique_lock lk(m);
    lk.unlock();
    cv.notify_one();
  }
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::executors::MultiThreadedExecutor executor;
  auto tNode = std::make_shared<MinimalPublisher>();
  executor.add_node(tNode);
  executor.spin();
//  rclcpp::spin(std::make_shared<MinimalPublisher>());
  rclcpp::shutdown();
  return 0;
}
