#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};




// Corresponds to lab1_msgs__srv__PlayRecording_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct PlayRecording_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub filename: std::string::String,

}



impl Default for PlayRecording_Request {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::PlayRecording_Request::default())
  }
}

impl rosidl_runtime_rs::Message for PlayRecording_Request {
  type RmwMsg = super::srv::rmw::PlayRecording_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        filename: msg.filename.as_str().into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        filename: msg.filename.as_str().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      filename: msg.filename.to_string(),
    }
  }
}


// Corresponds to lab1_msgs__srv__PlayRecording_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct PlayRecording_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,


    // This member is not documented.
    #[allow(missing_docs)]
    pub message: std::string::String,

}



impl Default for PlayRecording_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::PlayRecording_Response::default())
  }
}

impl rosidl_runtime_rs::Message for PlayRecording_Response {
  type RmwMsg = super::srv::rmw::PlayRecording_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        success: msg.success,
        message: msg.message.as_str().into(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      success: msg.success,
        message: msg.message.as_str().into(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      success: msg.success,
      message: msg.message.to_string(),
    }
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


