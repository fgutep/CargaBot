// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from lab1_msgs:srv/PlayRecording.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lab1_msgs/srv/play_recording.hpp"


#ifndef LAB1_MSGS__SRV__DETAIL__PLAY_RECORDING__BUILDER_HPP_
#define LAB1_MSGS__SRV__DETAIL__PLAY_RECORDING__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "lab1_msgs/srv/detail/play_recording__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace lab1_msgs
{

namespace srv
{

namespace builder
{

class Init_PlayRecording_Request_filename
{
public:
  Init_PlayRecording_Request_filename()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::lab1_msgs::srv::PlayRecording_Request filename(::lab1_msgs::srv::PlayRecording_Request::_filename_type arg)
  {
    msg_.filename = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lab1_msgs::srv::PlayRecording_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::lab1_msgs::srv::PlayRecording_Request>()
{
  return lab1_msgs::srv::builder::Init_PlayRecording_Request_filename();
}

}  // namespace lab1_msgs


namespace lab1_msgs
{

namespace srv
{

namespace builder
{

class Init_PlayRecording_Response_message
{
public:
  explicit Init_PlayRecording_Response_message(::lab1_msgs::srv::PlayRecording_Response & msg)
  : msg_(msg)
  {}
  ::lab1_msgs::srv::PlayRecording_Response message(::lab1_msgs::srv::PlayRecording_Response::_message_type arg)
  {
    msg_.message = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lab1_msgs::srv::PlayRecording_Response msg_;
};

class Init_PlayRecording_Response_success
{
public:
  Init_PlayRecording_Response_success()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_PlayRecording_Response_message success(::lab1_msgs::srv::PlayRecording_Response::_success_type arg)
  {
    msg_.success = std::move(arg);
    return Init_PlayRecording_Response_message(msg_);
  }

private:
  ::lab1_msgs::srv::PlayRecording_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::lab1_msgs::srv::PlayRecording_Response>()
{
  return lab1_msgs::srv::builder::Init_PlayRecording_Response_success();
}

}  // namespace lab1_msgs


namespace lab1_msgs
{

namespace srv
{

namespace builder
{

class Init_PlayRecording_Event_response
{
public:
  explicit Init_PlayRecording_Event_response(::lab1_msgs::srv::PlayRecording_Event & msg)
  : msg_(msg)
  {}
  ::lab1_msgs::srv::PlayRecording_Event response(::lab1_msgs::srv::PlayRecording_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::lab1_msgs::srv::PlayRecording_Event msg_;
};

class Init_PlayRecording_Event_request
{
public:
  explicit Init_PlayRecording_Event_request(::lab1_msgs::srv::PlayRecording_Event & msg)
  : msg_(msg)
  {}
  Init_PlayRecording_Event_response request(::lab1_msgs::srv::PlayRecording_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_PlayRecording_Event_response(msg_);
  }

private:
  ::lab1_msgs::srv::PlayRecording_Event msg_;
};

class Init_PlayRecording_Event_info
{
public:
  Init_PlayRecording_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_PlayRecording_Event_request info(::lab1_msgs::srv::PlayRecording_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_PlayRecording_Event_request(msg_);
  }

private:
  ::lab1_msgs::srv::PlayRecording_Event msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::lab1_msgs::srv::PlayRecording_Event>()
{
  return lab1_msgs::srv::builder::Init_PlayRecording_Event_info();
}

}  // namespace lab1_msgs

#endif  // LAB1_MSGS__SRV__DETAIL__PLAY_RECORDING__BUILDER_HPP_
