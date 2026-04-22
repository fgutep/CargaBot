// generated from rosidl_generator_c/resource/idl__functions.h.em
// with input from lab1_msgs:srv/PlayRecording.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "lab1_msgs/srv/play_recording.h"


#ifndef LAB1_MSGS__SRV__DETAIL__PLAY_RECORDING__FUNCTIONS_H_
#define LAB1_MSGS__SRV__DETAIL__PLAY_RECORDING__FUNCTIONS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdlib.h>

#include "rosidl_runtime_c/action_type_support_struct.h"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_runtime_c/service_type_support_struct.h"
#include "rosidl_runtime_c/type_description/type_description__struct.h"
#include "rosidl_runtime_c/type_description/type_source__struct.h"
#include "rosidl_runtime_c/type_hash.h"
#include "rosidl_runtime_c/visibility_control.h"
#include "lab1_msgs/msg/rosidl_generator_c__visibility_control.h"

#include "lab1_msgs/srv/detail/play_recording__struct.h"

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
const rosidl_type_hash_t *
lab1_msgs__srv__PlayRecording__get_type_hash(
  const rosidl_service_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
const rosidl_runtime_c__type_description__TypeDescription *
lab1_msgs__srv__PlayRecording__get_type_description(
  const rosidl_service_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
const rosidl_runtime_c__type_description__TypeSource *
lab1_msgs__srv__PlayRecording__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
const rosidl_runtime_c__type_description__TypeSource__Sequence *
lab1_msgs__srv__PlayRecording__get_type_description_sources(
  const rosidl_service_type_support_t * type_support);

/// Initialize srv/PlayRecording message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * lab1_msgs__srv__PlayRecording_Request
 * )) before or use
 * lab1_msgs__srv__PlayRecording_Request__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
bool
lab1_msgs__srv__PlayRecording_Request__init(lab1_msgs__srv__PlayRecording_Request * msg);

/// Finalize srv/PlayRecording message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
void
lab1_msgs__srv__PlayRecording_Request__fini(lab1_msgs__srv__PlayRecording_Request * msg);

/// Create srv/PlayRecording message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * lab1_msgs__srv__PlayRecording_Request__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
lab1_msgs__srv__PlayRecording_Request *
lab1_msgs__srv__PlayRecording_Request__create(void);

/// Destroy srv/PlayRecording message.
/**
 * It calls
 * lab1_msgs__srv__PlayRecording_Request__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
void
lab1_msgs__srv__PlayRecording_Request__destroy(lab1_msgs__srv__PlayRecording_Request * msg);

/// Check for srv/PlayRecording message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
bool
lab1_msgs__srv__PlayRecording_Request__are_equal(const lab1_msgs__srv__PlayRecording_Request * lhs, const lab1_msgs__srv__PlayRecording_Request * rhs);

/// Copy a srv/PlayRecording message.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source message pointer.
 * \param[out] output The target message pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer is null
 *   or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
bool
lab1_msgs__srv__PlayRecording_Request__copy(
  const lab1_msgs__srv__PlayRecording_Request * input,
  lab1_msgs__srv__PlayRecording_Request * output);

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
const rosidl_type_hash_t *
lab1_msgs__srv__PlayRecording_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
const rosidl_runtime_c__type_description__TypeDescription *
lab1_msgs__srv__PlayRecording_Request__get_type_description(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
const rosidl_runtime_c__type_description__TypeSource *
lab1_msgs__srv__PlayRecording_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
const rosidl_runtime_c__type_description__TypeSource__Sequence *
lab1_msgs__srv__PlayRecording_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support);

/// Initialize array of srv/PlayRecording messages.
/**
 * It allocates the memory for the number of elements and calls
 * lab1_msgs__srv__PlayRecording_Request__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
bool
lab1_msgs__srv__PlayRecording_Request__Sequence__init(lab1_msgs__srv__PlayRecording_Request__Sequence * array, size_t size);

/// Finalize array of srv/PlayRecording messages.
/**
 * It calls
 * lab1_msgs__srv__PlayRecording_Request__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
void
lab1_msgs__srv__PlayRecording_Request__Sequence__fini(lab1_msgs__srv__PlayRecording_Request__Sequence * array);

/// Create array of srv/PlayRecording messages.
/**
 * It allocates the memory for the array and calls
 * lab1_msgs__srv__PlayRecording_Request__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
lab1_msgs__srv__PlayRecording_Request__Sequence *
lab1_msgs__srv__PlayRecording_Request__Sequence__create(size_t size);

/// Destroy array of srv/PlayRecording messages.
/**
 * It calls
 * lab1_msgs__srv__PlayRecording_Request__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
void
lab1_msgs__srv__PlayRecording_Request__Sequence__destroy(lab1_msgs__srv__PlayRecording_Request__Sequence * array);

/// Check for srv/PlayRecording message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
bool
lab1_msgs__srv__PlayRecording_Request__Sequence__are_equal(const lab1_msgs__srv__PlayRecording_Request__Sequence * lhs, const lab1_msgs__srv__PlayRecording_Request__Sequence * rhs);

/// Copy an array of srv/PlayRecording messages.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source array pointer.
 * \param[out] output The target array pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer
 *   is null or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
bool
lab1_msgs__srv__PlayRecording_Request__Sequence__copy(
  const lab1_msgs__srv__PlayRecording_Request__Sequence * input,
  lab1_msgs__srv__PlayRecording_Request__Sequence * output);

/// Initialize srv/PlayRecording message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * lab1_msgs__srv__PlayRecording_Response
 * )) before or use
 * lab1_msgs__srv__PlayRecording_Response__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
bool
lab1_msgs__srv__PlayRecording_Response__init(lab1_msgs__srv__PlayRecording_Response * msg);

/// Finalize srv/PlayRecording message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
void
lab1_msgs__srv__PlayRecording_Response__fini(lab1_msgs__srv__PlayRecording_Response * msg);

/// Create srv/PlayRecording message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * lab1_msgs__srv__PlayRecording_Response__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
lab1_msgs__srv__PlayRecording_Response *
lab1_msgs__srv__PlayRecording_Response__create(void);

/// Destroy srv/PlayRecording message.
/**
 * It calls
 * lab1_msgs__srv__PlayRecording_Response__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
void
lab1_msgs__srv__PlayRecording_Response__destroy(lab1_msgs__srv__PlayRecording_Response * msg);

/// Check for srv/PlayRecording message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
bool
lab1_msgs__srv__PlayRecording_Response__are_equal(const lab1_msgs__srv__PlayRecording_Response * lhs, const lab1_msgs__srv__PlayRecording_Response * rhs);

/// Copy a srv/PlayRecording message.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source message pointer.
 * \param[out] output The target message pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer is null
 *   or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
bool
lab1_msgs__srv__PlayRecording_Response__copy(
  const lab1_msgs__srv__PlayRecording_Response * input,
  lab1_msgs__srv__PlayRecording_Response * output);

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
const rosidl_type_hash_t *
lab1_msgs__srv__PlayRecording_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
const rosidl_runtime_c__type_description__TypeDescription *
lab1_msgs__srv__PlayRecording_Response__get_type_description(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
const rosidl_runtime_c__type_description__TypeSource *
lab1_msgs__srv__PlayRecording_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
const rosidl_runtime_c__type_description__TypeSource__Sequence *
lab1_msgs__srv__PlayRecording_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support);

/// Initialize array of srv/PlayRecording messages.
/**
 * It allocates the memory for the number of elements and calls
 * lab1_msgs__srv__PlayRecording_Response__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
bool
lab1_msgs__srv__PlayRecording_Response__Sequence__init(lab1_msgs__srv__PlayRecording_Response__Sequence * array, size_t size);

/// Finalize array of srv/PlayRecording messages.
/**
 * It calls
 * lab1_msgs__srv__PlayRecording_Response__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
void
lab1_msgs__srv__PlayRecording_Response__Sequence__fini(lab1_msgs__srv__PlayRecording_Response__Sequence * array);

/// Create array of srv/PlayRecording messages.
/**
 * It allocates the memory for the array and calls
 * lab1_msgs__srv__PlayRecording_Response__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
lab1_msgs__srv__PlayRecording_Response__Sequence *
lab1_msgs__srv__PlayRecording_Response__Sequence__create(size_t size);

/// Destroy array of srv/PlayRecording messages.
/**
 * It calls
 * lab1_msgs__srv__PlayRecording_Response__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
void
lab1_msgs__srv__PlayRecording_Response__Sequence__destroy(lab1_msgs__srv__PlayRecording_Response__Sequence * array);

/// Check for srv/PlayRecording message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
bool
lab1_msgs__srv__PlayRecording_Response__Sequence__are_equal(const lab1_msgs__srv__PlayRecording_Response__Sequence * lhs, const lab1_msgs__srv__PlayRecording_Response__Sequence * rhs);

/// Copy an array of srv/PlayRecording messages.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source array pointer.
 * \param[out] output The target array pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer
 *   is null or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
bool
lab1_msgs__srv__PlayRecording_Response__Sequence__copy(
  const lab1_msgs__srv__PlayRecording_Response__Sequence * input,
  lab1_msgs__srv__PlayRecording_Response__Sequence * output);

/// Initialize srv/PlayRecording message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * lab1_msgs__srv__PlayRecording_Event
 * )) before or use
 * lab1_msgs__srv__PlayRecording_Event__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
bool
lab1_msgs__srv__PlayRecording_Event__init(lab1_msgs__srv__PlayRecording_Event * msg);

/// Finalize srv/PlayRecording message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
void
lab1_msgs__srv__PlayRecording_Event__fini(lab1_msgs__srv__PlayRecording_Event * msg);

/// Create srv/PlayRecording message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * lab1_msgs__srv__PlayRecording_Event__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
lab1_msgs__srv__PlayRecording_Event *
lab1_msgs__srv__PlayRecording_Event__create(void);

/// Destroy srv/PlayRecording message.
/**
 * It calls
 * lab1_msgs__srv__PlayRecording_Event__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
void
lab1_msgs__srv__PlayRecording_Event__destroy(lab1_msgs__srv__PlayRecording_Event * msg);

/// Check for srv/PlayRecording message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
bool
lab1_msgs__srv__PlayRecording_Event__are_equal(const lab1_msgs__srv__PlayRecording_Event * lhs, const lab1_msgs__srv__PlayRecording_Event * rhs);

/// Copy a srv/PlayRecording message.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source message pointer.
 * \param[out] output The target message pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer is null
 *   or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
bool
lab1_msgs__srv__PlayRecording_Event__copy(
  const lab1_msgs__srv__PlayRecording_Event * input,
  lab1_msgs__srv__PlayRecording_Event * output);

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
const rosidl_type_hash_t *
lab1_msgs__srv__PlayRecording_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
const rosidl_runtime_c__type_description__TypeDescription *
lab1_msgs__srv__PlayRecording_Event__get_type_description(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
const rosidl_runtime_c__type_description__TypeSource *
lab1_msgs__srv__PlayRecording_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
const rosidl_runtime_c__type_description__TypeSource__Sequence *
lab1_msgs__srv__PlayRecording_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support);

/// Initialize array of srv/PlayRecording messages.
/**
 * It allocates the memory for the number of elements and calls
 * lab1_msgs__srv__PlayRecording_Event__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
bool
lab1_msgs__srv__PlayRecording_Event__Sequence__init(lab1_msgs__srv__PlayRecording_Event__Sequence * array, size_t size);

/// Finalize array of srv/PlayRecording messages.
/**
 * It calls
 * lab1_msgs__srv__PlayRecording_Event__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
void
lab1_msgs__srv__PlayRecording_Event__Sequence__fini(lab1_msgs__srv__PlayRecording_Event__Sequence * array);

/// Create array of srv/PlayRecording messages.
/**
 * It allocates the memory for the array and calls
 * lab1_msgs__srv__PlayRecording_Event__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
lab1_msgs__srv__PlayRecording_Event__Sequence *
lab1_msgs__srv__PlayRecording_Event__Sequence__create(size_t size);

/// Destroy array of srv/PlayRecording messages.
/**
 * It calls
 * lab1_msgs__srv__PlayRecording_Event__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
void
lab1_msgs__srv__PlayRecording_Event__Sequence__destroy(lab1_msgs__srv__PlayRecording_Event__Sequence * array);

/// Check for srv/PlayRecording message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
bool
lab1_msgs__srv__PlayRecording_Event__Sequence__are_equal(const lab1_msgs__srv__PlayRecording_Event__Sequence * lhs, const lab1_msgs__srv__PlayRecording_Event__Sequence * rhs);

/// Copy an array of srv/PlayRecording messages.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source array pointer.
 * \param[out] output The target array pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer
 *   is null or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_lab1_msgs
bool
lab1_msgs__srv__PlayRecording_Event__Sequence__copy(
  const lab1_msgs__srv__PlayRecording_Event__Sequence * input,
  lab1_msgs__srv__PlayRecording_Event__Sequence * output);
#ifdef __cplusplus
}
#endif

#endif  // LAB1_MSGS__SRV__DETAIL__PLAY_RECORDING__FUNCTIONS_H_
