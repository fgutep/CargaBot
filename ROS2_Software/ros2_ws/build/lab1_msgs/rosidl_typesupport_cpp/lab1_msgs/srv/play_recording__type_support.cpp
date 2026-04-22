// generated from rosidl_typesupport_cpp/resource/idl__type_support.cpp.em
// with input from lab1_msgs:srv/PlayRecording.idl
// generated code does not contain a copyright notice

#include "cstddef"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "lab1_msgs/srv/detail/play_recording__functions.h"
#include "lab1_msgs/srv/detail/play_recording__struct.hpp"
#include "rosidl_typesupport_cpp/identifier.hpp"
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_c/type_support_map.h"
#include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
#include "rosidl_typesupport_cpp/visibility_control.h"
#include "rosidl_typesupport_interface/macros.h"

namespace lab1_msgs
{

namespace srv
{

namespace rosidl_typesupport_cpp
{

typedef struct _PlayRecording_Request_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _PlayRecording_Request_type_support_ids_t;

static const _PlayRecording_Request_type_support_ids_t _PlayRecording_Request_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _PlayRecording_Request_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _PlayRecording_Request_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _PlayRecording_Request_type_support_symbol_names_t _PlayRecording_Request_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, lab1_msgs, srv, PlayRecording_Request)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, lab1_msgs, srv, PlayRecording_Request)),
  }
};

typedef struct _PlayRecording_Request_type_support_data_t
{
  void * data[2];
} _PlayRecording_Request_type_support_data_t;

static _PlayRecording_Request_type_support_data_t _PlayRecording_Request_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _PlayRecording_Request_message_typesupport_map = {
  2,
  "lab1_msgs",
  &_PlayRecording_Request_message_typesupport_ids.typesupport_identifier[0],
  &_PlayRecording_Request_message_typesupport_symbol_names.symbol_name[0],
  &_PlayRecording_Request_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t PlayRecording_Request_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_PlayRecording_Request_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &lab1_msgs__srv__PlayRecording_Request__get_type_hash,
  &lab1_msgs__srv__PlayRecording_Request__get_type_description,
  &lab1_msgs__srv__PlayRecording_Request__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace lab1_msgs

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<lab1_msgs::srv::PlayRecording_Request>()
{
  return &::lab1_msgs::srv::rosidl_typesupport_cpp::PlayRecording_Request_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, lab1_msgs, srv, PlayRecording_Request)() {
  return get_message_type_support_handle<lab1_msgs::srv::PlayRecording_Request>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "lab1_msgs/srv/detail/play_recording__functions.h"
// already included above
// #include "lab1_msgs/srv/detail/play_recording__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace lab1_msgs
{

namespace srv
{

namespace rosidl_typesupport_cpp
{

typedef struct _PlayRecording_Response_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _PlayRecording_Response_type_support_ids_t;

static const _PlayRecording_Response_type_support_ids_t _PlayRecording_Response_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _PlayRecording_Response_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _PlayRecording_Response_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _PlayRecording_Response_type_support_symbol_names_t _PlayRecording_Response_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, lab1_msgs, srv, PlayRecording_Response)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, lab1_msgs, srv, PlayRecording_Response)),
  }
};

typedef struct _PlayRecording_Response_type_support_data_t
{
  void * data[2];
} _PlayRecording_Response_type_support_data_t;

static _PlayRecording_Response_type_support_data_t _PlayRecording_Response_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _PlayRecording_Response_message_typesupport_map = {
  2,
  "lab1_msgs",
  &_PlayRecording_Response_message_typesupport_ids.typesupport_identifier[0],
  &_PlayRecording_Response_message_typesupport_symbol_names.symbol_name[0],
  &_PlayRecording_Response_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t PlayRecording_Response_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_PlayRecording_Response_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &lab1_msgs__srv__PlayRecording_Response__get_type_hash,
  &lab1_msgs__srv__PlayRecording_Response__get_type_description,
  &lab1_msgs__srv__PlayRecording_Response__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace lab1_msgs

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<lab1_msgs::srv::PlayRecording_Response>()
{
  return &::lab1_msgs::srv::rosidl_typesupport_cpp::PlayRecording_Response_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, lab1_msgs, srv, PlayRecording_Response)() {
  return get_message_type_support_handle<lab1_msgs::srv::PlayRecording_Response>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "lab1_msgs/srv/detail/play_recording__functions.h"
// already included above
// #include "lab1_msgs/srv/detail/play_recording__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace lab1_msgs
{

namespace srv
{

namespace rosidl_typesupport_cpp
{

typedef struct _PlayRecording_Event_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _PlayRecording_Event_type_support_ids_t;

static const _PlayRecording_Event_type_support_ids_t _PlayRecording_Event_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _PlayRecording_Event_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _PlayRecording_Event_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _PlayRecording_Event_type_support_symbol_names_t _PlayRecording_Event_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, lab1_msgs, srv, PlayRecording_Event)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, lab1_msgs, srv, PlayRecording_Event)),
  }
};

typedef struct _PlayRecording_Event_type_support_data_t
{
  void * data[2];
} _PlayRecording_Event_type_support_data_t;

static _PlayRecording_Event_type_support_data_t _PlayRecording_Event_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _PlayRecording_Event_message_typesupport_map = {
  2,
  "lab1_msgs",
  &_PlayRecording_Event_message_typesupport_ids.typesupport_identifier[0],
  &_PlayRecording_Event_message_typesupport_symbol_names.symbol_name[0],
  &_PlayRecording_Event_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t PlayRecording_Event_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_PlayRecording_Event_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &lab1_msgs__srv__PlayRecording_Event__get_type_hash,
  &lab1_msgs__srv__PlayRecording_Event__get_type_description,
  &lab1_msgs__srv__PlayRecording_Event__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace lab1_msgs

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<lab1_msgs::srv::PlayRecording_Event>()
{
  return &::lab1_msgs::srv::rosidl_typesupport_cpp::PlayRecording_Event_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, lab1_msgs, srv, PlayRecording_Event)() {
  return get_message_type_support_handle<lab1_msgs::srv::PlayRecording_Event>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
#include "rosidl_runtime_c/service_type_support_struct.h"
#include "rosidl_typesupport_cpp/service_type_support.hpp"
// already included above
// #include "lab1_msgs/srv/detail/play_recording__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
#include "rosidl_typesupport_cpp/service_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace lab1_msgs
{

namespace srv
{

namespace rosidl_typesupport_cpp
{

typedef struct _PlayRecording_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _PlayRecording_type_support_ids_t;

static const _PlayRecording_type_support_ids_t _PlayRecording_service_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _PlayRecording_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _PlayRecording_type_support_symbol_names_t;
#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _PlayRecording_type_support_symbol_names_t _PlayRecording_service_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, lab1_msgs, srv, PlayRecording)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, lab1_msgs, srv, PlayRecording)),
  }
};

typedef struct _PlayRecording_type_support_data_t
{
  void * data[2];
} _PlayRecording_type_support_data_t;

static _PlayRecording_type_support_data_t _PlayRecording_service_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _PlayRecording_service_typesupport_map = {
  2,
  "lab1_msgs",
  &_PlayRecording_service_typesupport_ids.typesupport_identifier[0],
  &_PlayRecording_service_typesupport_symbol_names.symbol_name[0],
  &_PlayRecording_service_typesupport_data.data[0],
};

static const rosidl_service_type_support_t PlayRecording_service_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_PlayRecording_service_typesupport_map),
  ::rosidl_typesupport_cpp::get_service_typesupport_handle_function,
  ::rosidl_typesupport_cpp::get_message_type_support_handle<lab1_msgs::srv::PlayRecording_Request>(),
  ::rosidl_typesupport_cpp::get_message_type_support_handle<lab1_msgs::srv::PlayRecording_Response>(),
  ::rosidl_typesupport_cpp::get_message_type_support_handle<lab1_msgs::srv::PlayRecording_Event>(),
  &::rosidl_typesupport_cpp::service_create_event_message<lab1_msgs::srv::PlayRecording>,
  &::rosidl_typesupport_cpp::service_destroy_event_message<lab1_msgs::srv::PlayRecording>,
  &lab1_msgs__srv__PlayRecording__get_type_hash,
  &lab1_msgs__srv__PlayRecording__get_type_description,
  &lab1_msgs__srv__PlayRecording__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace lab1_msgs

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_service_type_support_t *
get_service_type_support_handle<lab1_msgs::srv::PlayRecording>()
{
  return &::lab1_msgs::srv::rosidl_typesupport_cpp::PlayRecording_service_type_support_handle;
}

}  // namespace rosidl_typesupport_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_cpp, lab1_msgs, srv, PlayRecording)() {
  return ::rosidl_typesupport_cpp::get_service_type_support_handle<lab1_msgs::srv::PlayRecording>();
}

#ifdef __cplusplus
}
#endif
