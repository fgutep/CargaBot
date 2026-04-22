// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from lab1_msgs:msg/RobotAction.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lab1_msgs/msg/robot_action.hpp"


#ifndef LAB1_MSGS__MSG__DETAIL__ROBOT_ACTION__TRAITS_HPP_
#define LAB1_MSGS__MSG__DETAIL__ROBOT_ACTION__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "lab1_msgs/msg/detail/robot_action__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace lab1_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const RobotAction & msg,
  std::ostream & out)
{
  out << "{";
  // member: action
  {
    out << "action: ";
    rosidl_generator_traits::value_to_yaml(msg.action, out);
    out << ", ";
  }

  // member: timestamp
  {
    out << "timestamp: ";
    rosidl_generator_traits::value_to_yaml(msg.timestamp, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const RobotAction & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: action
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "action: ";
    rosidl_generator_traits::value_to_yaml(msg.action, out);
    out << "\n";
  }

  // member: timestamp
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "timestamp: ";
    rosidl_generator_traits::value_to_yaml(msg.timestamp, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const RobotAction & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace msg

}  // namespace lab1_msgs

namespace rosidl_generator_traits
{

[[deprecated("use lab1_msgs::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const lab1_msgs::msg::RobotAction & msg,
  std::ostream & out, size_t indentation = 0)
{
  lab1_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use lab1_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const lab1_msgs::msg::RobotAction & msg)
{
  return lab1_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<lab1_msgs::msg::RobotAction>()
{
  return "lab1_msgs::msg::RobotAction";
}

template<>
inline const char * name<lab1_msgs::msg::RobotAction>()
{
  return "lab1_msgs/msg/RobotAction";
}

template<>
struct has_fixed_size<lab1_msgs::msg::RobotAction>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<lab1_msgs::msg::RobotAction>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<lab1_msgs::msg::RobotAction>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // LAB1_MSGS__MSG__DETAIL__ROBOT_ACTION__TRAITS_HPP_
