#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};



#[link(name = "lab1_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lab1_msgs__srv__PlayRecording_Request() -> *const std::ffi::c_void;
}

#[link(name = "lab1_msgs__rosidl_generator_c")]
extern "C" {
    fn lab1_msgs__srv__PlayRecording_Request__init(msg: *mut PlayRecording_Request) -> bool;
    fn lab1_msgs__srv__PlayRecording_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<PlayRecording_Request>, size: usize) -> bool;
    fn lab1_msgs__srv__PlayRecording_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<PlayRecording_Request>);
    fn lab1_msgs__srv__PlayRecording_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<PlayRecording_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<PlayRecording_Request>) -> bool;
}

// Corresponds to lab1_msgs__srv__PlayRecording_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct PlayRecording_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub filename: rosidl_runtime_rs::String,

}



impl Default for PlayRecording_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lab1_msgs__srv__PlayRecording_Request__init(&mut msg as *mut _) {
        panic!("Call to lab1_msgs__srv__PlayRecording_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for PlayRecording_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lab1_msgs__srv__PlayRecording_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lab1_msgs__srv__PlayRecording_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lab1_msgs__srv__PlayRecording_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for PlayRecording_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for PlayRecording_Request where Self: Sized {
  const TYPE_NAME: &'static str = "lab1_msgs/srv/PlayRecording_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lab1_msgs__srv__PlayRecording_Request() }
  }
}


#[link(name = "lab1_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lab1_msgs__srv__PlayRecording_Response() -> *const std::ffi::c_void;
}

#[link(name = "lab1_msgs__rosidl_generator_c")]
extern "C" {
    fn lab1_msgs__srv__PlayRecording_Response__init(msg: *mut PlayRecording_Response) -> bool;
    fn lab1_msgs__srv__PlayRecording_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<PlayRecording_Response>, size: usize) -> bool;
    fn lab1_msgs__srv__PlayRecording_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<PlayRecording_Response>);
    fn lab1_msgs__srv__PlayRecording_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<PlayRecording_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<PlayRecording_Response>) -> bool;
}

// Corresponds to lab1_msgs__srv__PlayRecording_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct PlayRecording_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,


    // This member is not documented.
    #[allow(missing_docs)]
    pub message: rosidl_runtime_rs::String,

}



impl Default for PlayRecording_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lab1_msgs__srv__PlayRecording_Response__init(&mut msg as *mut _) {
        panic!("Call to lab1_msgs__srv__PlayRecording_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for PlayRecording_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lab1_msgs__srv__PlayRecording_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lab1_msgs__srv__PlayRecording_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lab1_msgs__srv__PlayRecording_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for PlayRecording_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for PlayRecording_Response where Self: Sized {
  const TYPE_NAME: &'static str = "lab1_msgs/srv/PlayRecording_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lab1_msgs__srv__PlayRecording_Response() }
  }
}






#[link(name = "lab1_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__lab1_msgs__srv__PlayRecording() -> *const std::ffi::c_void;
}

// Corresponds to lab1_msgs__srv__PlayRecording
#[allow(missing_docs, non_camel_case_types)]
pub struct PlayRecording;

impl rosidl_runtime_rs::Service for PlayRecording {
    type Request = PlayRecording_Request;
    type Response = PlayRecording_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__lab1_msgs__srv__PlayRecording() }
    }
}


