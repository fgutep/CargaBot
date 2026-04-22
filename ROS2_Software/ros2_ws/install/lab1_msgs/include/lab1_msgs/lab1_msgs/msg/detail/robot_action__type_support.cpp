// generated from rosidl_typesupport_introspection_cpp/resource/idl__type_support.cpp.em
// with input from lab1_msgs:msg/RobotAction.idl
// generated code does not contain a copyright notice

#include "array"
#include "cstddef"
#include "string"
#include "vector"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_interface/macros.h"
#include "lab1_msgs/msg/detail/robot_action__functions.h"
#include "lab1_msgs/msg/detail/robot_action__struct.hpp"
#include "rosidl_typesupport_introspection_cpp/field_types.hpp"
#include "rosidl_typesupport_introspection_cpp/identifier.hpp"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"
#include "rosidl_typesupport_introspection_cpp/message_type_support_decl.hpp"
#include "rosidl_typesupport_introspection_cpp/visibility_control.h"

namespace lab1_msgs
{

namespace msg
{

namespace rosidl_typesupport_introspection_cpp
{

void RobotAction_init_function(
  void * message_memory, rosidl_runtime_cpp::MessageInitialization _init)
{
  new (message_memory) lab1_msgs::msg::RobotAction(_init);
}

void RobotAction_fini_function(void * message_memory)
{
  auto typed_message = static_cast<lab1_msgs::msg::RobotAction *>(message_memory);
  typed_message->~RobotAction();
}

static const ::rosidl_typesupport_introspection_cpp::MessageMember RobotAction_message_member_array[2] = {
  {
    "action",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(lab1_msgs::msg::RobotAction, action),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  },
  {
    "timestamp",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    nullptr,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(lab1_msgs::msg::RobotAction, timestamp),  // bytes offset in struct
    nullptr,  // default value
    nullptr,  // size() function pointer
    nullptr,  // get_const(index) function pointer
    nullptr,  // get(index) function pointer
    nullptr,  // fetch(index, &value) function pointer
    nullptr,  // assign(index, value) function pointer
    nullptr  // resize(index) function pointer
  }
};

static const ::rosidl_typesupport_introspection_cpp::MessageMembers RobotAction_message_members = {
  "lab1_msgs::msg",  // message namespace
  "RobotAction",  // message name
  2,  // number of fields
  sizeof(lab1_msgs::msg::RobotAction),
  false,  // has_any_key_member_
  RobotAction_message_member_array,  // message members
  RobotAction_init_function,  // function to initialize message memory (memory has to be allocated)
  RobotAction_fini_function  // function to terminate message instance (will not free memory)
};

static const rosidl_message_type_support_t RobotAction_message_type_support_handle = {
  ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  &RobotAction_message_members,
  get_message_typesupport_handle_function,
  &lab1_msgs__msg__RobotAction__get_type_hash,
  &lab1_msgs__msg__RobotAction__get_type_description,
  &lab1_msgs__msg__RobotAction__get_type_description_sources,
};

}  // namespace rosidl_typesupport_introspection_cpp

}  // namespace msg

}  // namespace lab1_msgs


namespace rosidl_typesupport_introspection_cpp
{

template<>
ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<lab1_msgs::msg::RobotAction>()
{
  return &::lab1_msgs::msg::rosidl_typesupport_introspection_cpp::RobotAction_message_type_support_handle;
}

}  // namespace rosidl_typesupport_introspection_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, lab1_msgs, msg, RobotAction)() {
  return &::lab1_msgs::msg::rosidl_typesupport_introspection_cpp::RobotAction_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif
