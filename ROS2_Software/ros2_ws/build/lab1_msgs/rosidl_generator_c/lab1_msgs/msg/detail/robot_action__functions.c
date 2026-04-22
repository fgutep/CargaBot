// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from lab1_msgs:msg/RobotAction.idl
// generated code does not contain a copyright notice
#include "lab1_msgs/msg/detail/robot_action__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `action`
#include "rosidl_runtime_c/string_functions.h"

bool
lab1_msgs__msg__RobotAction__init(lab1_msgs__msg__RobotAction * msg)
{
  if (!msg) {
    return false;
  }
  // action
  if (!rosidl_runtime_c__String__init(&msg->action)) {
    lab1_msgs__msg__RobotAction__fini(msg);
    return false;
  }
  // timestamp
  return true;
}

void
lab1_msgs__msg__RobotAction__fini(lab1_msgs__msg__RobotAction * msg)
{
  if (!msg) {
    return;
  }
  // action
  rosidl_runtime_c__String__fini(&msg->action);
  // timestamp
}

bool
lab1_msgs__msg__RobotAction__are_equal(const lab1_msgs__msg__RobotAction * lhs, const lab1_msgs__msg__RobotAction * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // action
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->action), &(rhs->action)))
  {
    return false;
  }
  // timestamp
  if (lhs->timestamp != rhs->timestamp) {
    return false;
  }
  return true;
}

bool
lab1_msgs__msg__RobotAction__copy(
  const lab1_msgs__msg__RobotAction * input,
  lab1_msgs__msg__RobotAction * output)
{
  if (!input || !output) {
    return false;
  }
  // action
  if (!rosidl_runtime_c__String__copy(
      &(input->action), &(output->action)))
  {
    return false;
  }
  // timestamp
  output->timestamp = input->timestamp;
  return true;
}

lab1_msgs__msg__RobotAction *
lab1_msgs__msg__RobotAction__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  lab1_msgs__msg__RobotAction * msg = (lab1_msgs__msg__RobotAction *)allocator.allocate(sizeof(lab1_msgs__msg__RobotAction), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(lab1_msgs__msg__RobotAction));
  bool success = lab1_msgs__msg__RobotAction__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
lab1_msgs__msg__RobotAction__destroy(lab1_msgs__msg__RobotAction * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    lab1_msgs__msg__RobotAction__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
lab1_msgs__msg__RobotAction__Sequence__init(lab1_msgs__msg__RobotAction__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  lab1_msgs__msg__RobotAction * data = NULL;

  if (size) {
    data = (lab1_msgs__msg__RobotAction *)allocator.zero_allocate(size, sizeof(lab1_msgs__msg__RobotAction), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = lab1_msgs__msg__RobotAction__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        lab1_msgs__msg__RobotAction__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
lab1_msgs__msg__RobotAction__Sequence__fini(lab1_msgs__msg__RobotAction__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      lab1_msgs__msg__RobotAction__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

lab1_msgs__msg__RobotAction__Sequence *
lab1_msgs__msg__RobotAction__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  lab1_msgs__msg__RobotAction__Sequence * array = (lab1_msgs__msg__RobotAction__Sequence *)allocator.allocate(sizeof(lab1_msgs__msg__RobotAction__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = lab1_msgs__msg__RobotAction__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
lab1_msgs__msg__RobotAction__Sequence__destroy(lab1_msgs__msg__RobotAction__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    lab1_msgs__msg__RobotAction__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
lab1_msgs__msg__RobotAction__Sequence__are_equal(const lab1_msgs__msg__RobotAction__Sequence * lhs, const lab1_msgs__msg__RobotAction__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!lab1_msgs__msg__RobotAction__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
lab1_msgs__msg__RobotAction__Sequence__copy(
  const lab1_msgs__msg__RobotAction__Sequence * input,
  lab1_msgs__msg__RobotAction__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(lab1_msgs__msg__RobotAction);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    lab1_msgs__msg__RobotAction * data =
      (lab1_msgs__msg__RobotAction *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!lab1_msgs__msg__RobotAction__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          lab1_msgs__msg__RobotAction__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!lab1_msgs__msg__RobotAction__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
