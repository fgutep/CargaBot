// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from lab1_msgs:msg/RobotAction.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lab1_msgs/msg/robot_action.hpp"


#ifndef LAB1_MSGS__MSG__DETAIL__ROBOT_ACTION__STRUCT_HPP_
#define LAB1_MSGS__MSG__DETAIL__ROBOT_ACTION__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__lab1_msgs__msg__RobotAction __attribute__((deprecated))
#else
# define DEPRECATED__lab1_msgs__msg__RobotAction __declspec(deprecated)
#endif

namespace lab1_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct RobotAction_
{
  using Type = RobotAction_<ContainerAllocator>;

  explicit RobotAction_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->action = "";
      this->timestamp = 0.0;
    }
  }

  explicit RobotAction_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : action(_alloc)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->action = "";
      this->timestamp = 0.0;
    }
  }

  // field types and members
  using _action_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _action_type action;
  using _timestamp_type =
    double;
  _timestamp_type timestamp;

  // setters for named parameter idiom
  Type & set__action(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->action = _arg;
    return *this;
  }
  Type & set__timestamp(
    const double & _arg)
  {
    this->timestamp = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    lab1_msgs::msg::RobotAction_<ContainerAllocator> *;
  using ConstRawPtr =
    const lab1_msgs::msg::RobotAction_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<lab1_msgs::msg::RobotAction_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<lab1_msgs::msg::RobotAction_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      lab1_msgs::msg::RobotAction_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<lab1_msgs::msg::RobotAction_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      lab1_msgs::msg::RobotAction_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<lab1_msgs::msg::RobotAction_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<lab1_msgs::msg::RobotAction_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<lab1_msgs::msg::RobotAction_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__lab1_msgs__msg__RobotAction
    std::shared_ptr<lab1_msgs::msg::RobotAction_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__lab1_msgs__msg__RobotAction
    std::shared_ptr<lab1_msgs::msg::RobotAction_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const RobotAction_ & other) const
  {
    if (this->action != other.action) {
      return false;
    }
    if (this->timestamp != other.timestamp) {
      return false;
    }
    return true;
  }
  bool operator!=(const RobotAction_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct RobotAction_

// alias to use template instance with default allocator
using RobotAction =
  lab1_msgs::msg::RobotAction_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace lab1_msgs

#endif  // LAB1_MSGS__MSG__DETAIL__ROBOT_ACTION__STRUCT_HPP_
