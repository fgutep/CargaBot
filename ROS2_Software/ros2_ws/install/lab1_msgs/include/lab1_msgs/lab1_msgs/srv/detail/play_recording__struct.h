// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from lab1_msgs:srv/PlayRecording.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lab1_msgs/srv/play_recording.h"


#ifndef LAB1_MSGS__SRV__DETAIL__PLAY_RECORDING__STRUCT_H_
#define LAB1_MSGS__SRV__DETAIL__PLAY_RECORDING__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'filename'
#include "rosidl_runtime_c/string.h"

/// Struct defined in srv/PlayRecording in the package lab1_msgs.
typedef struct lab1_msgs__srv__PlayRecording_Request
{
  rosidl_runtime_c__String filename;
} lab1_msgs__srv__PlayRecording_Request;

// Struct for a sequence of lab1_msgs__srv__PlayRecording_Request.
typedef struct lab1_msgs__srv__PlayRecording_Request__Sequence
{
  lab1_msgs__srv__PlayRecording_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} lab1_msgs__srv__PlayRecording_Request__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'message'
// already included above
// #include "rosidl_runtime_c/string.h"

/// Struct defined in srv/PlayRecording in the package lab1_msgs.
typedef struct lab1_msgs__srv__PlayRecording_Response
{
  bool success;
  rosidl_runtime_c__String message;
} lab1_msgs__srv__PlayRecording_Response;

// Struct for a sequence of lab1_msgs__srv__PlayRecording_Response.
typedef struct lab1_msgs__srv__PlayRecording_Response__Sequence
{
  lab1_msgs__srv__PlayRecording_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} lab1_msgs__srv__PlayRecording_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  lab1_msgs__srv__PlayRecording_Event__request__MAX_SIZE = 1
};
// response
enum
{
  lab1_msgs__srv__PlayRecording_Event__response__MAX_SIZE = 1
};

/// Struct defined in srv/PlayRecording in the package lab1_msgs.
typedef struct lab1_msgs__srv__PlayRecording_Event
{
  service_msgs__msg__ServiceEventInfo info;
  lab1_msgs__srv__PlayRecording_Request__Sequence request;
  lab1_msgs__srv__PlayRecording_Response__Sequence response;
} lab1_msgs__srv__PlayRecording_Event;

// Struct for a sequence of lab1_msgs__srv__PlayRecording_Event.
typedef struct lab1_msgs__srv__PlayRecording_Event__Sequence
{
  lab1_msgs__srv__PlayRecording_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} lab1_msgs__srv__PlayRecording_Event__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // LAB1_MSGS__SRV__DETAIL__PLAY_RECORDING__STRUCT_H_
