// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from lab1_msgs:srv/PlayRecording.idl
// generated code does not contain a copyright notice
#include "lab1_msgs/srv/detail/play_recording__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"

// Include directives for member types
// Member `filename`
#include "rosidl_runtime_c/string_functions.h"

bool
lab1_msgs__srv__PlayRecording_Request__init(lab1_msgs__srv__PlayRecording_Request * msg)
{
  if (!msg) {
    return false;
  }
  // filename
  if (!rosidl_runtime_c__String__init(&msg->filename)) {
    lab1_msgs__srv__PlayRecording_Request__fini(msg);
    return false;
  }
  return true;
}

void
lab1_msgs__srv__PlayRecording_Request__fini(lab1_msgs__srv__PlayRecording_Request * msg)
{
  if (!msg) {
    return;
  }
  // filename
  rosidl_runtime_c__String__fini(&msg->filename);
}

bool
lab1_msgs__srv__PlayRecording_Request__are_equal(const lab1_msgs__srv__PlayRecording_Request * lhs, const lab1_msgs__srv__PlayRecording_Request * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // filename
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->filename), &(rhs->filename)))
  {
    return false;
  }
  return true;
}

bool
lab1_msgs__srv__PlayRecording_Request__copy(
  const lab1_msgs__srv__PlayRecording_Request * input,
  lab1_msgs__srv__PlayRecording_Request * output)
{
  if (!input || !output) {
    return false;
  }
  // filename
  if (!rosidl_runtime_c__String__copy(
      &(input->filename), &(output->filename)))
  {
    return false;
  }
  return true;
}

lab1_msgs__srv__PlayRecording_Request *
lab1_msgs__srv__PlayRecording_Request__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  lab1_msgs__srv__PlayRecording_Request * msg = (lab1_msgs__srv__PlayRecording_Request *)allocator.allocate(sizeof(lab1_msgs__srv__PlayRecording_Request), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(lab1_msgs__srv__PlayRecording_Request));
  bool success = lab1_msgs__srv__PlayRecording_Request__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
lab1_msgs__srv__PlayRecording_Request__destroy(lab1_msgs__srv__PlayRecording_Request * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    lab1_msgs__srv__PlayRecording_Request__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
lab1_msgs__srv__PlayRecording_Request__Sequence__init(lab1_msgs__srv__PlayRecording_Request__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  lab1_msgs__srv__PlayRecording_Request * data = NULL;

  if (size) {
    data = (lab1_msgs__srv__PlayRecording_Request *)allocator.zero_allocate(size, sizeof(lab1_msgs__srv__PlayRecording_Request), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = lab1_msgs__srv__PlayRecording_Request__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        lab1_msgs__srv__PlayRecording_Request__fini(&data[i - 1]);
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
lab1_msgs__srv__PlayRecording_Request__Sequence__fini(lab1_msgs__srv__PlayRecording_Request__Sequence * array)
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
      lab1_msgs__srv__PlayRecording_Request__fini(&array->data[i]);
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

lab1_msgs__srv__PlayRecording_Request__Sequence *
lab1_msgs__srv__PlayRecording_Request__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  lab1_msgs__srv__PlayRecording_Request__Sequence * array = (lab1_msgs__srv__PlayRecording_Request__Sequence *)allocator.allocate(sizeof(lab1_msgs__srv__PlayRecording_Request__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = lab1_msgs__srv__PlayRecording_Request__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
lab1_msgs__srv__PlayRecording_Request__Sequence__destroy(lab1_msgs__srv__PlayRecording_Request__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    lab1_msgs__srv__PlayRecording_Request__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
lab1_msgs__srv__PlayRecording_Request__Sequence__are_equal(const lab1_msgs__srv__PlayRecording_Request__Sequence * lhs, const lab1_msgs__srv__PlayRecording_Request__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!lab1_msgs__srv__PlayRecording_Request__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
lab1_msgs__srv__PlayRecording_Request__Sequence__copy(
  const lab1_msgs__srv__PlayRecording_Request__Sequence * input,
  lab1_msgs__srv__PlayRecording_Request__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(lab1_msgs__srv__PlayRecording_Request);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    lab1_msgs__srv__PlayRecording_Request * data =
      (lab1_msgs__srv__PlayRecording_Request *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!lab1_msgs__srv__PlayRecording_Request__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          lab1_msgs__srv__PlayRecording_Request__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!lab1_msgs__srv__PlayRecording_Request__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `message`
// already included above
// #include "rosidl_runtime_c/string_functions.h"

bool
lab1_msgs__srv__PlayRecording_Response__init(lab1_msgs__srv__PlayRecording_Response * msg)
{
  if (!msg) {
    return false;
  }
  // success
  // message
  if (!rosidl_runtime_c__String__init(&msg->message)) {
    lab1_msgs__srv__PlayRecording_Response__fini(msg);
    return false;
  }
  return true;
}

void
lab1_msgs__srv__PlayRecording_Response__fini(lab1_msgs__srv__PlayRecording_Response * msg)
{
  if (!msg) {
    return;
  }
  // success
  // message
  rosidl_runtime_c__String__fini(&msg->message);
}

bool
lab1_msgs__srv__PlayRecording_Response__are_equal(const lab1_msgs__srv__PlayRecording_Response * lhs, const lab1_msgs__srv__PlayRecording_Response * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // success
  if (lhs->success != rhs->success) {
    return false;
  }
  // message
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->message), &(rhs->message)))
  {
    return false;
  }
  return true;
}

bool
lab1_msgs__srv__PlayRecording_Response__copy(
  const lab1_msgs__srv__PlayRecording_Response * input,
  lab1_msgs__srv__PlayRecording_Response * output)
{
  if (!input || !output) {
    return false;
  }
  // success
  output->success = input->success;
  // message
  if (!rosidl_runtime_c__String__copy(
      &(input->message), &(output->message)))
  {
    return false;
  }
  return true;
}

lab1_msgs__srv__PlayRecording_Response *
lab1_msgs__srv__PlayRecording_Response__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  lab1_msgs__srv__PlayRecording_Response * msg = (lab1_msgs__srv__PlayRecording_Response *)allocator.allocate(sizeof(lab1_msgs__srv__PlayRecording_Response), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(lab1_msgs__srv__PlayRecording_Response));
  bool success = lab1_msgs__srv__PlayRecording_Response__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
lab1_msgs__srv__PlayRecording_Response__destroy(lab1_msgs__srv__PlayRecording_Response * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    lab1_msgs__srv__PlayRecording_Response__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
lab1_msgs__srv__PlayRecording_Response__Sequence__init(lab1_msgs__srv__PlayRecording_Response__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  lab1_msgs__srv__PlayRecording_Response * data = NULL;

  if (size) {
    data = (lab1_msgs__srv__PlayRecording_Response *)allocator.zero_allocate(size, sizeof(lab1_msgs__srv__PlayRecording_Response), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = lab1_msgs__srv__PlayRecording_Response__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        lab1_msgs__srv__PlayRecording_Response__fini(&data[i - 1]);
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
lab1_msgs__srv__PlayRecording_Response__Sequence__fini(lab1_msgs__srv__PlayRecording_Response__Sequence * array)
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
      lab1_msgs__srv__PlayRecording_Response__fini(&array->data[i]);
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

lab1_msgs__srv__PlayRecording_Response__Sequence *
lab1_msgs__srv__PlayRecording_Response__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  lab1_msgs__srv__PlayRecording_Response__Sequence * array = (lab1_msgs__srv__PlayRecording_Response__Sequence *)allocator.allocate(sizeof(lab1_msgs__srv__PlayRecording_Response__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = lab1_msgs__srv__PlayRecording_Response__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
lab1_msgs__srv__PlayRecording_Response__Sequence__destroy(lab1_msgs__srv__PlayRecording_Response__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    lab1_msgs__srv__PlayRecording_Response__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
lab1_msgs__srv__PlayRecording_Response__Sequence__are_equal(const lab1_msgs__srv__PlayRecording_Response__Sequence * lhs, const lab1_msgs__srv__PlayRecording_Response__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!lab1_msgs__srv__PlayRecording_Response__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
lab1_msgs__srv__PlayRecording_Response__Sequence__copy(
  const lab1_msgs__srv__PlayRecording_Response__Sequence * input,
  lab1_msgs__srv__PlayRecording_Response__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(lab1_msgs__srv__PlayRecording_Response);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    lab1_msgs__srv__PlayRecording_Response * data =
      (lab1_msgs__srv__PlayRecording_Response *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!lab1_msgs__srv__PlayRecording_Response__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          lab1_msgs__srv__PlayRecording_Response__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!lab1_msgs__srv__PlayRecording_Response__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `info`
#include "service_msgs/msg/detail/service_event_info__functions.h"
// Member `request`
// Member `response`
// already included above
// #include "lab1_msgs/srv/detail/play_recording__functions.h"

bool
lab1_msgs__srv__PlayRecording_Event__init(lab1_msgs__srv__PlayRecording_Event * msg)
{
  if (!msg) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__init(&msg->info)) {
    lab1_msgs__srv__PlayRecording_Event__fini(msg);
    return false;
  }
  // request
  if (!lab1_msgs__srv__PlayRecording_Request__Sequence__init(&msg->request, 0)) {
    lab1_msgs__srv__PlayRecording_Event__fini(msg);
    return false;
  }
  // response
  if (!lab1_msgs__srv__PlayRecording_Response__Sequence__init(&msg->response, 0)) {
    lab1_msgs__srv__PlayRecording_Event__fini(msg);
    return false;
  }
  return true;
}

void
lab1_msgs__srv__PlayRecording_Event__fini(lab1_msgs__srv__PlayRecording_Event * msg)
{
  if (!msg) {
    return;
  }
  // info
  service_msgs__msg__ServiceEventInfo__fini(&msg->info);
  // request
  lab1_msgs__srv__PlayRecording_Request__Sequence__fini(&msg->request);
  // response
  lab1_msgs__srv__PlayRecording_Response__Sequence__fini(&msg->response);
}

bool
lab1_msgs__srv__PlayRecording_Event__are_equal(const lab1_msgs__srv__PlayRecording_Event * lhs, const lab1_msgs__srv__PlayRecording_Event * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__are_equal(
      &(lhs->info), &(rhs->info)))
  {
    return false;
  }
  // request
  if (!lab1_msgs__srv__PlayRecording_Request__Sequence__are_equal(
      &(lhs->request), &(rhs->request)))
  {
    return false;
  }
  // response
  if (!lab1_msgs__srv__PlayRecording_Response__Sequence__are_equal(
      &(lhs->response), &(rhs->response)))
  {
    return false;
  }
  return true;
}

bool
lab1_msgs__srv__PlayRecording_Event__copy(
  const lab1_msgs__srv__PlayRecording_Event * input,
  lab1_msgs__srv__PlayRecording_Event * output)
{
  if (!input || !output) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__copy(
      &(input->info), &(output->info)))
  {
    return false;
  }
  // request
  if (!lab1_msgs__srv__PlayRecording_Request__Sequence__copy(
      &(input->request), &(output->request)))
  {
    return false;
  }
  // response
  if (!lab1_msgs__srv__PlayRecording_Response__Sequence__copy(
      &(input->response), &(output->response)))
  {
    return false;
  }
  return true;
}

lab1_msgs__srv__PlayRecording_Event *
lab1_msgs__srv__PlayRecording_Event__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  lab1_msgs__srv__PlayRecording_Event * msg = (lab1_msgs__srv__PlayRecording_Event *)allocator.allocate(sizeof(lab1_msgs__srv__PlayRecording_Event), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(lab1_msgs__srv__PlayRecording_Event));
  bool success = lab1_msgs__srv__PlayRecording_Event__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
lab1_msgs__srv__PlayRecording_Event__destroy(lab1_msgs__srv__PlayRecording_Event * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    lab1_msgs__srv__PlayRecording_Event__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
lab1_msgs__srv__PlayRecording_Event__Sequence__init(lab1_msgs__srv__PlayRecording_Event__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  lab1_msgs__srv__PlayRecording_Event * data = NULL;

  if (size) {
    data = (lab1_msgs__srv__PlayRecording_Event *)allocator.zero_allocate(size, sizeof(lab1_msgs__srv__PlayRecording_Event), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = lab1_msgs__srv__PlayRecording_Event__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        lab1_msgs__srv__PlayRecording_Event__fini(&data[i - 1]);
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
lab1_msgs__srv__PlayRecording_Event__Sequence__fini(lab1_msgs__srv__PlayRecording_Event__Sequence * array)
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
      lab1_msgs__srv__PlayRecording_Event__fini(&array->data[i]);
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

lab1_msgs__srv__PlayRecording_Event__Sequence *
lab1_msgs__srv__PlayRecording_Event__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  lab1_msgs__srv__PlayRecording_Event__Sequence * array = (lab1_msgs__srv__PlayRecording_Event__Sequence *)allocator.allocate(sizeof(lab1_msgs__srv__PlayRecording_Event__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = lab1_msgs__srv__PlayRecording_Event__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
lab1_msgs__srv__PlayRecording_Event__Sequence__destroy(lab1_msgs__srv__PlayRecording_Event__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    lab1_msgs__srv__PlayRecording_Event__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
lab1_msgs__srv__PlayRecording_Event__Sequence__are_equal(const lab1_msgs__srv__PlayRecording_Event__Sequence * lhs, const lab1_msgs__srv__PlayRecording_Event__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!lab1_msgs__srv__PlayRecording_Event__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
lab1_msgs__srv__PlayRecording_Event__Sequence__copy(
  const lab1_msgs__srv__PlayRecording_Event__Sequence * input,
  lab1_msgs__srv__PlayRecording_Event__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(lab1_msgs__srv__PlayRecording_Event);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    lab1_msgs__srv__PlayRecording_Event * data =
      (lab1_msgs__srv__PlayRecording_Event *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!lab1_msgs__srv__PlayRecording_Event__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          lab1_msgs__srv__PlayRecording_Event__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!lab1_msgs__srv__PlayRecording_Event__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
