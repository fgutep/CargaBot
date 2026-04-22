// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from lab1_msgs:msg/RobotAction.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lab1_msgs/msg/robot_action.h"


#ifndef LAB1_MSGS__MSG__DETAIL__ROBOT_ACTION__STRUCT_H_
#define LAB1_MSGS__MSG__DETAIL__ROBOT_ACTION__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'action'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/RobotAction in the package lab1_msgs.
typedef struct lab1_msgs__msg__RobotAction
{
  rosidl_runtime_c__String action;
  double timestamp;
} lab1_msgs__msg__RobotAction;

// Struct for a sequence of lab1_msgs__msg__RobotAction.
typedef struct lab1_msgs__msg__RobotAction__Sequence
{
  lab1_msgs__msg__RobotAction * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} lab1_msgs__msg__RobotAction__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // LAB1_MSGS__MSG__DETAIL__ROBOT_ACTION__STRUCT_H_
