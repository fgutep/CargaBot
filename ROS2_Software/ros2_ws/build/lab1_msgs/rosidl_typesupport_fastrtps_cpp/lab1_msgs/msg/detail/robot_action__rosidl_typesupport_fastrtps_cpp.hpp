// generated from rosidl_typesupport_fastrtps_cpp/resource/idl__rosidl_typesupport_fastrtps_cpp.hpp.em
// with input from lab1_msgs:msg/RobotAction.idl
// generated code does not contain a copyright notice

#ifndef LAB1_MSGS__MSG__DETAIL__ROBOT_ACTION__ROSIDL_TYPESUPPORT_FASTRTPS_CPP_HPP_
#define LAB1_MSGS__MSG__DETAIL__ROBOT_ACTION__ROSIDL_TYPESUPPORT_FASTRTPS_CPP_HPP_

#include <cstddef>
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_interface/macros.h"
#include "lab1_msgs/msg/rosidl_typesupport_fastrtps_cpp__visibility_control.h"
#include "lab1_msgs/msg/detail/robot_action__struct.hpp"

#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# ifdef __clang__
#  pragma clang diagnostic ignored "-Wdeprecated-register"
#  pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
# endif
#endif
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif

#include "fastcdr/Cdr.h"

namespace lab1_msgs
{

namespace msg
{

namespace typesupport_fastrtps_cpp
{

bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_lab1_msgs
cdr_serialize(
  const lab1_msgs::msg::RobotAction & ros_message,
  eprosima::fastcdr::Cdr & cdr);

bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_lab1_msgs
cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  lab1_msgs::msg::RobotAction & ros_message);

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_lab1_msgs
get_serialized_size(
  const lab1_msgs::msg::RobotAction & ros_message,
  size_t current_alignment);

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_lab1_msgs
max_serialized_size_RobotAction(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_lab1_msgs
cdr_serialize_key(
  const lab1_msgs::msg::RobotAction & ros_message,
  eprosima::fastcdr::Cdr &);

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_lab1_msgs
get_serialized_size_key(
  const lab1_msgs::msg::RobotAction & ros_message,
  size_t current_alignment);

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_lab1_msgs
max_serialized_size_key_RobotAction(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

}  // namespace typesupport_fastrtps_cpp

}  // namespace msg

}  // namespace lab1_msgs

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_lab1_msgs
const rosidl_message_type_support_t *
  ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, lab1_msgs, msg, RobotAction)();

#ifdef __cplusplus
}
#endif

#endif  // LAB1_MSGS__MSG__DETAIL__ROBOT_ACTION__ROSIDL_TYPESUPPORT_FASTRTPS_CPP_HPP_
