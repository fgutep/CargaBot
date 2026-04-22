// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from lab1_msgs:srv/PlayRecording.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lab1_msgs/srv/play_recording.hpp"


#ifndef LAB1_MSGS__SRV__DETAIL__PLAY_RECORDING__TRAITS_HPP_
#define LAB1_MSGS__SRV__DETAIL__PLAY_RECORDING__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "lab1_msgs/srv/detail/play_recording__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace lab1_msgs
{

namespace srv
{

inline void to_flow_style_yaml(
  const PlayRecording_Request & msg,
  std::ostream & out)
{
  out << "{";
  // member: filename
  {
    out << "filename: ";
    rosidl_generator_traits::value_to_yaml(msg.filename, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const PlayRecording_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: filename
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "filename: ";
    rosidl_generator_traits::value_to_yaml(msg.filename, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const PlayRecording_Request & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace lab1_msgs

namespace rosidl_generator_traits
{

[[deprecated("use lab1_msgs::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const lab1_msgs::srv::PlayRecording_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  lab1_msgs::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use lab1_msgs::srv::to_yaml() instead")]]
inline std::string to_yaml(const lab1_msgs::srv::PlayRecording_Request & msg)
{
  return lab1_msgs::srv::to_yaml(msg);
}

template<>
inline const char * data_type<lab1_msgs::srv::PlayRecording_Request>()
{
  return "lab1_msgs::srv::PlayRecording_Request";
}

template<>
inline const char * name<lab1_msgs::srv::PlayRecording_Request>()
{
  return "lab1_msgs/srv/PlayRecording_Request";
}

template<>
struct has_fixed_size<lab1_msgs::srv::PlayRecording_Request>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<lab1_msgs::srv::PlayRecording_Request>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<lab1_msgs::srv::PlayRecording_Request>
  : std::true_type {};

}  // namespace rosidl_generator_traits

namespace lab1_msgs
{

namespace srv
{

inline void to_flow_style_yaml(
  const PlayRecording_Response & msg,
  std::ostream & out)
{
  out << "{";
  // member: success
  {
    out << "success: ";
    rosidl_generator_traits::value_to_yaml(msg.success, out);
    out << ", ";
  }

  // member: message
  {
    out << "message: ";
    rosidl_generator_traits::value_to_yaml(msg.message, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const PlayRecording_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: success
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "success: ";
    rosidl_generator_traits::value_to_yaml(msg.success, out);
    out << "\n";
  }

  // member: message
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "message: ";
    rosidl_generator_traits::value_to_yaml(msg.message, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const PlayRecording_Response & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace lab1_msgs

namespace rosidl_generator_traits
{

[[deprecated("use lab1_msgs::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const lab1_msgs::srv::PlayRecording_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  lab1_msgs::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use lab1_msgs::srv::to_yaml() instead")]]
inline std::string to_yaml(const lab1_msgs::srv::PlayRecording_Response & msg)
{
  return lab1_msgs::srv::to_yaml(msg);
}

template<>
inline const char * data_type<lab1_msgs::srv::PlayRecording_Response>()
{
  return "lab1_msgs::srv::PlayRecording_Response";
}

template<>
inline const char * name<lab1_msgs::srv::PlayRecording_Response>()
{
  return "lab1_msgs/srv/PlayRecording_Response";
}

template<>
struct has_fixed_size<lab1_msgs::srv::PlayRecording_Response>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<lab1_msgs::srv::PlayRecording_Response>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<lab1_msgs::srv::PlayRecording_Response>
  : std::true_type {};

}  // namespace rosidl_generator_traits

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__traits.hpp"

namespace lab1_msgs
{

namespace srv
{

inline void to_flow_style_yaml(
  const PlayRecording_Event & msg,
  std::ostream & out)
{
  out << "{";
  // member: info
  {
    out << "info: ";
    to_flow_style_yaml(msg.info, out);
    out << ", ";
  }

  // member: request
  {
    if (msg.request.size() == 0) {
      out << "request: []";
    } else {
      out << "request: [";
      size_t pending_items = msg.request.size();
      for (auto item : msg.request) {
        to_flow_style_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
    out << ", ";
  }

  // member: response
  {
    if (msg.response.size() == 0) {
      out << "response: []";
    } else {
      out << "response: [";
      size_t pending_items = msg.response.size();
      for (auto item : msg.response) {
        to_flow_style_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const PlayRecording_Event & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: info
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "info:\n";
    to_block_style_yaml(msg.info, out, indentation + 2);
  }

  // member: request
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.request.size() == 0) {
      out << "request: []\n";
    } else {
      out << "request:\n";
      for (auto item : msg.request) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }

  // member: response
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.response.size() == 0) {
      out << "response: []\n";
    } else {
      out << "response:\n";
      for (auto item : msg.response) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const PlayRecording_Event & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace lab1_msgs

namespace rosidl_generator_traits
{

[[deprecated("use lab1_msgs::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const lab1_msgs::srv::PlayRecording_Event & msg,
  std::ostream & out, size_t indentation = 0)
{
  lab1_msgs::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use lab1_msgs::srv::to_yaml() instead")]]
inline std::string to_yaml(const lab1_msgs::srv::PlayRecording_Event & msg)
{
  return lab1_msgs::srv::to_yaml(msg);
}

template<>
inline const char * data_type<lab1_msgs::srv::PlayRecording_Event>()
{
  return "lab1_msgs::srv::PlayRecording_Event";
}

template<>
inline const char * name<lab1_msgs::srv::PlayRecording_Event>()
{
  return "lab1_msgs/srv/PlayRecording_Event";
}

template<>
struct has_fixed_size<lab1_msgs::srv::PlayRecording_Event>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<lab1_msgs::srv::PlayRecording_Event>
  : std::integral_constant<bool, has_bounded_size<lab1_msgs::srv::PlayRecording_Request>::value && has_bounded_size<lab1_msgs::srv::PlayRecording_Response>::value && has_bounded_size<service_msgs::msg::ServiceEventInfo>::value> {};

template<>
struct is_message<lab1_msgs::srv::PlayRecording_Event>
  : std::true_type {};

}  // namespace rosidl_generator_traits

namespace rosidl_generator_traits
{

template<>
inline const char * data_type<lab1_msgs::srv::PlayRecording>()
{
  return "lab1_msgs::srv::PlayRecording";
}

template<>
inline const char * name<lab1_msgs::srv::PlayRecording>()
{
  return "lab1_msgs/srv/PlayRecording";
}

template<>
struct has_fixed_size<lab1_msgs::srv::PlayRecording>
  : std::integral_constant<
    bool,
    has_fixed_size<lab1_msgs::srv::PlayRecording_Request>::value &&
    has_fixed_size<lab1_msgs::srv::PlayRecording_Response>::value
  >
{
};

template<>
struct has_bounded_size<lab1_msgs::srv::PlayRecording>
  : std::integral_constant<
    bool,
    has_bounded_size<lab1_msgs::srv::PlayRecording_Request>::value &&
    has_bounded_size<lab1_msgs::srv::PlayRecording_Response>::value
  >
{
};

template<>
struct is_service<lab1_msgs::srv::PlayRecording>
  : std::true_type
{
};

template<>
struct is_service_request<lab1_msgs::srv::PlayRecording_Request>
  : std::true_type
{
};

template<>
struct is_service_response<lab1_msgs::srv::PlayRecording_Response>
  : std::true_type
{
};

}  // namespace rosidl_generator_traits

#endif  // LAB1_MSGS__SRV__DETAIL__PLAY_RECORDING__TRAITS_HPP_
