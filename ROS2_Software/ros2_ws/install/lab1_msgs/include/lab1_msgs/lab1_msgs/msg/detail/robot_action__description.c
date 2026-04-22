// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from lab1_msgs:msg/RobotAction.idl
// generated code does not contain a copyright notice

#include "lab1_msgs/msg/detail/robot_action__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
const rosidl_type_hash_t *
lab1_msgs__msg__RobotAction__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xda, 0x79, 0x7f, 0xa8, 0xf0, 0xf5, 0xbe, 0x00,
      0x32, 0xe3, 0x3e, 0xbe, 0x51, 0xd0, 0x74, 0x2c,
      0x66, 0x7e, 0x94, 0x05, 0xf5, 0xbc, 0x5b, 0x51,
      0xab, 0x2e, 0xb5, 0x5f, 0x39, 0x07, 0x2a, 0xf2,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char lab1_msgs__msg__RobotAction__TYPE_NAME[] = "lab1_msgs/msg/RobotAction";

// Define type names, field names, and default values
static char lab1_msgs__msg__RobotAction__FIELD_NAME__action[] = "action";
static char lab1_msgs__msg__RobotAction__FIELD_NAME__timestamp[] = "timestamp";

static rosidl_runtime_c__type_description__Field lab1_msgs__msg__RobotAction__FIELDS[] = {
  {
    {lab1_msgs__msg__RobotAction__FIELD_NAME__action, 6, 6},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_STRING,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {lab1_msgs__msg__RobotAction__FIELD_NAME__timestamp, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
lab1_msgs__msg__RobotAction__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {lab1_msgs__msg__RobotAction__TYPE_NAME, 25, 25},
      {lab1_msgs__msg__RobotAction__FIELDS, 2, 2},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "string action\n"
  "float64 timestamp";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
lab1_msgs__msg__RobotAction__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {lab1_msgs__msg__RobotAction__TYPE_NAME, 25, 25},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 31, 31},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
lab1_msgs__msg__RobotAction__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *lab1_msgs__msg__RobotAction__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
