// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from lab1_msgs:msg/RobotAction.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lab1_msgs/msg/robot_action.hpp"


#ifndef LAB1_MSGS__MSG__DETAIL__ROBOT_ACTION__BUILDER_HPP_
#define LAB1_MSGS__MSG__DETAIL__ROBOT_ACTION__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "lab1_msgs/msg/detail/robot_action__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace lab1_msgs
{

namespace msg
{

namespace builder
{

class Init_RobotAction_timestamp
{
public:
  explicit Init_RobotAction_timestamp(::lab1_msgs::msg::RobotAction & msg)
  : msg_(msg)
  {}
  ::lab1_msgs::msg::RobotAction timestamp(::lab1_msgs::msg::RobotAction::_timestamp_type arg)
  {
    msg_.timestamp = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lab1_msgs::msg::RobotAction msg_;
};

class Init_RobotAction_action
{
public:
  Init_RobotAction_action()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_RobotAction_timestamp action(::lab1_msgs::msg::RobotAction::_action_type arg)
  {
    msg_.action = std::move(arg);
    return Init_RobotAction_timestamp(msg_);
  }

private:
  ::lab1_msgs::msg::RobotAction msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::lab1_msgs::msg::RobotAction>()
{
  return lab1_msgs::msg::builder::Init_RobotAction_action();
}

}  // namespace lab1_msgs

#endif  // LAB1_MSGS__MSG__DETAIL__ROBOT_ACTION__BUILDER_HPP_
