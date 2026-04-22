#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
from lab1_msgs.srv import PlayRecording
import time
import os

class TurtleBotPlayerNode(Node):
    def __init__(self):
        super().__init__("turtle_bot_player")
        
        self.vel_publisher = self.create_publisher(Twist, "turtlebot_cmdVel", 10)
        
        self.service = self.create_service(
            PlayRecording,
            'play_recording',
            self.play_recording_callback
        )
        
        
        self.action_map = {
            'w': {'linear_x': 0.5, 'angular_z': 0.0},
            's': {'linear_x': -0.5, 'angular_z': 0.0},
            'a': {'linear_x': 0.0, 'angular_z': 0.5},
            'd': {'linear_x': 0.0, 'angular_z': -0.5},
            'stop': {'linear_x': 0.0, 'angular_z': 0.0},
        }
        
        self.is_playing = False
        
        self.get_logger().info("TurtleBot Player Node has been started.")
        self.get_logger().info("Service 'play_recording' is ready.")
    
    def play_recording_callback(self, request, response):
        
        filename = request.filename
        
        self.get_logger().info(f"Received request to play: {filename}")
        
        if self.is_playing:
            response.success = False
            response.message = "Already playing a recording. Please wait."
            self.get_logger().warn(response.message)
            return response
        
        if not os.path.exists(filename):
            response.success = False
            response.message = f"File not found: {filename}"
            self.get_logger().error(response.message)
            return response
        
        self.is_playing = True
        
        try:
            actions = self.load_actions_from_file(filename)
            
            if not actions:
                response.success = False
                response.message = "No valid actions found in file"
                self.is_playing = False
                return response
            
            
            grouped_actions = self.group_consecutive_actions(actions)
            
            self.get_logger().info(f"Loaded {len(actions)} actions, grouped into {len(grouped_actions)} movements")
            self.replay_actions(grouped_actions)
            
            response.success = True
            response.message = f"Successfully replayed {len(grouped_actions)} movements"
            self.get_logger().info(response.message)
            
        except Exception as e:
            response.success = False
            response.message = f"Error playing recording: {str(e)}"
            self.get_logger().error(response.message)
        
        finally:
            self.is_playing = False
        
        return response
    
    def load_actions_from_file(self, filename):
        
        actions = []
        
        try:
            with open(filename, 'r', encoding='utf-8') as f:
                lines = f.readlines()
                
                start_reading = False
                
                for line in lines:
                    if '----' in line:
                        start_reading = True
                        continue
                    
                    if start_reading and '=' in line:
                        break
                    
                    if start_reading and '|' in line:
                        parts = line.split('|')
                        if len(parts) >= 3:
                            timestamp_part = parts[0].split('.')
                            if len(timestamp_part) >= 2:
                                try:
                                    timestamp = float('.'.join(timestamp_part[-2:]))
                                    key = parts[1].strip()
                                    
                                    if key in self.action_map:
                                        actions.append({
                                            'key': key,
                                            'timestamp': timestamp
                                        })
                                
                                except ValueError:
                                    continue
            
            self.get_logger().info(f"Parsed {len(actions)} actions from file")
            return actions
        
        except Exception as e:
            self.get_logger().error(f"Error loading file: {str(e)}")
            return []
    
    def group_consecutive_actions(self, actions):
        
        if not actions:
            return []
        
        grouped = []
        current_key = actions[0]['key']
        start_time = actions[0]['timestamp']
        
        for i in range(1, len(actions)):
            if actions[i]['key'] != current_key:
                
                duration = actions[i]['timestamp'] - start_time
                grouped.append({
                    'key': current_key,
                    'duration': duration
                })
                
                
                current_key = actions[i]['key']
                start_time = actions[i]['timestamp']
        
        
        grouped.append({
            'key': current_key,
            'duration': 0.5
        })
        
        return grouped
    
    def replay_actions(self, actions):
        
        self.get_logger().info("Starting playback...")
        
        for i, action in enumerate(actions, 1):
            key = action['key']
            duration = action['duration']
            
            
            twist = Twist()
            twist.linear.x = self.action_map[key]['linear_x']
            twist.angular.z = self.action_map[key]['angular_z']
            
            action_name = "Detenido" if key == 'stop' else key
            self.get_logger().info(
                f"Action {i}/{len(actions)}: {action_name} "
                f"(duration: {duration:.2f}s, linear: {twist.linear.x}, angular: {twist.angular.z})"
            )
            
            
            start_time = time.time()
            while (time.time() - start_time) < duration:
                self.vel_publisher.publish(twist)
                time.sleep(0.05)  
        
        
        twist = Twist()
        self.vel_publisher.publish(twist)
        self.get_logger().info("Playback completed. Robot stopped.")

def main(args=None):
    rclpy.init(args=args)
    node = TurtleBotPlayerNode()
    
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        twist = Twist()
        node.vel_publisher.publish(twist)
        node.destroy_node()
        rclpy.shutdown()

if __name__ == "__main__":
    main()