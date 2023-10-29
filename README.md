# self_nodes

This project is used to forward the message from local to cloud and the cloud will send it back directly. After so , we can measure the delay caused by the network compared without the nodes.
When applying this to your project, you need to modify the topic/msg_type according to the code.
This project contains two kinds of nodes. One is talker which should be deployed to your local. The other one is listener which should be deployed in cloud.

If you want to test this project directly, you can post message to the topic mannually. For example:
```shell
ros2 topic pub --once /self_autoware_msg_receive std_msgs/msg/String "{data: abc}"
```
