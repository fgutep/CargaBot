#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};



// Corresponds to lab1_msgs__msg__RobotAction

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct RobotAction {

    // This member is not documented.
    #[allow(missing_docs)]
    pub action: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub timestamp: f64,

}



impl Default for RobotAction {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::RobotAction::default())
  }
}

impl rosidl_runtime_rs::Message for RobotAction {
  type RmwMsg = super::msg::rmw::RobotAction;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        action: msg.action.as_str().into(),
        timestamp: msg.timestamp,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        action: msg.action.as_str().into(),
      timestamp: msg.timestamp,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      action: msg.action.to_string(),
      timestamp: msg.timestamp,
    }
  }
}


