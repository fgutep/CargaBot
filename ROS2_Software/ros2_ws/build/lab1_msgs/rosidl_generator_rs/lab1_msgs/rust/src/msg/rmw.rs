#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};


#[link(name = "lab1_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__lab1_msgs__msg__RobotAction() -> *const std::ffi::c_void;
}

#[link(name = "lab1_msgs__rosidl_generator_c")]
extern "C" {
    fn lab1_msgs__msg__RobotAction__init(msg: *mut RobotAction) -> bool;
    fn lab1_msgs__msg__RobotAction__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<RobotAction>, size: usize) -> bool;
    fn lab1_msgs__msg__RobotAction__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<RobotAction>);
    fn lab1_msgs__msg__RobotAction__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<RobotAction>, out_seq: *mut rosidl_runtime_rs::Sequence<RobotAction>) -> bool;
}

// Corresponds to lab1_msgs__msg__RobotAction
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RobotAction {

    // This member is not documented.
    #[allow(missing_docs)]
    pub action: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub timestamp: f64,

}



impl Default for RobotAction {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !lab1_msgs__msg__RobotAction__init(&mut msg as *mut _) {
        panic!("Call to lab1_msgs__msg__RobotAction__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for RobotAction {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lab1_msgs__msg__RobotAction__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lab1_msgs__msg__RobotAction__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { lab1_msgs__msg__RobotAction__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for RobotAction {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for RobotAction where Self: Sized {
  const TYPE_NAME: &'static str = "lab1_msgs/msg/RobotAction";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__lab1_msgs__msg__RobotAction() }
  }
}


