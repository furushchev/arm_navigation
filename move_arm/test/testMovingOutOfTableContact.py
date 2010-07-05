#! /usr/bin/env python

PKG = 'move_arm'

import roslib; roslib.load_manifest(PKG)
import rospy
import planning_environment_msgs.srv
import sys
import unittest
import actionlib
import actionlib_msgs
import math

import sensor_msgs.msg
import mapping_msgs.msg
from mapping_msgs.msg import CollisionObject
from motion_planning_msgs.msg import CollisionOperation
from geometric_shapes_msgs.msg import Shape
from geometry_msgs.msg import Pose, PointStamped
from move_arm_msgs.msg import MoveArmGoal, MoveArmAction
from tf import TransformListener
from motion_planning_msgs.msg import JointConstraint

class TestMotionExecutionBuffer(unittest.TestCase):

    def setUp(self):

        self.tf = TransformListener()
        
        self.move_arm_action_client = actionlib.SimpleActionClient("move_right_arm", MoveArmAction)

        obj_pub = rospy.Publisher('collision_object',CollisionObject)
        
        rospy.init_node('test_motion_execution_buffer')
        
        #let everything settle down
        rospy.sleep(20.)
        
        obj1 = CollisionObject()
    
        obj1.header.stamp = rospy.Time.now()
        obj1.header.frame_id = "r_gripper_palm_link"
        obj1.id = "obj2";
        obj1.operation.operation = mapping_msgs.msg.CollisionObjectOperation.ADD
        obj1.shapes = [Shape() for _ in range(2)]
        obj1.shapes[0].type = Shape.BOX
        obj1.shapes[0].dimensions = [float() for _ in range(3)]
        obj1.shapes[0].dimensions[0] = .3
        obj1.shapes[0].dimensions[1] = .3
        obj1.shapes[0].dimensions[2] = .2
        obj1.poses = [Pose() for _ in range(2)]
        obj1.poses[0].position.x = 0
        obj1.poses[0].position.y = 0
        obj1.poses[0].position.z = -.1
        obj1.poses[0].orientation.x = 0
        obj1.poses[0].orientation.y = 0
        obj1.poses[0].orientation.z = 0
        obj1.poses[0].orientation.w = 1

        obj1.shapes[1].type = Shape.BOX
        obj1.shapes[1].dimensions = [float() for _ in range(3)]
        obj1.shapes[1].dimensions[0] = .3
        obj1.shapes[1].dimensions[1] = .3
        obj1.shapes[1].dimensions[2] = .2
        obj1.poses[1].position.x = 0
        obj1.poses[1].position.y = 0
        obj1.poses[1].position.z = .225
        obj1.poses[1].orientation.x = 0
        obj1.poses[1].orientation.y = 0
        obj1.poses[1].orientation.z = 0
        obj1.poses[1].orientation.w = 1

        
        obj_pub.publish(obj1)

        rospy.sleep(5.0)

    def testMotionExecutionBuffer(self):
        
        global padd_name
        global extra_buffer
        
        #too much trouble to read for now
        allow_padd = .05#rospy.get_param(padd_name)
        

        joint_names = ['%s_%s' % ('r', j) for j in ['shoulder_pan_joint', 'shoulder_lift_joint', 'upper_arm_roll_joint', 'elbow_flex_joint', 'forearm_roll_joint', 'wrist_flex_joint', 'wrist_roll_joint']]
        goal = MoveArmGoal()

        goal.motion_plan_request.goal_constraints.joint_constraints = [JointConstraint() for i in range(len(joint_names))]

        goal.motion_plan_request.group_name = "right_arm"
        goal.motion_plan_request.num_planning_attempts = 1
        goal.motion_plan_request.allowed_planning_time = rospy.Duration(5.)
        goal.motion_plan_request.planner_id = ""
        goal.planner_service_name = "chomp_planner_longrange/plan_path"
        goal.disable_collision_monitoring = True;

        goal.motion_plan_request.goal_constraints.joint_constraints = [JointConstraint() for i in range(len(joint_names))]
        for i in range(len(joint_names)):
            goal.motion_plan_request.goal_constraints.joint_constraints[i].joint_name = joint_names[i]
            goal.motion_plan_request.goal_constraints.joint_constraints[i].position = 0.0
            goal.motion_plan_request.goal_constraints.joint_constraints[i].tolerance_above = 0.08
            goal.motion_plan_request.goal_constraints.joint_constraints[i].tolerance_below = 0.08

            if(False):
                goal.motion_plan_request.goal_constraints.joint_constraints[0].position = -1.0
                goal.motion_plan_request.goal_constraints.joint_constraints[3].position = -0.2
                goal.motion_plan_request.goal_constraints.joint_constraints[5].position = -0.2
            else:
                goal.motion_plan_request.goal_constraints.joint_constraints[0].position = 0.0
                goal.motion_plan_request.goal_constraints.joint_constraints[3].position = -0.2
                goal.motion_plan_request.goal_constraints.joint_constraints[5].position = -0.2

        for x in range(3):

            self.move_arm_action_client.send_goal(goal)
            
            r = rospy.Rate(10)
            
            while True:
                cur_state = self.move_arm_action_client.get_state()
                if(cur_state != actionlib_msgs.msg.GoalStatus.ACTIVE and
                   cur_state != actionlib_msgs.msg.GoalStatus.PENDING):
                    break
                r.sleep()
                
            end_state = self.move_arm_action_client.get_state()
                
            if(end_state == actionlib_msgs.msg.GoalStatus.SUCCEEDED): break
                
        final_state = self.move_arm_action_client.get_state()

        self.assertEqual(final_state,  actionlib_msgs.msg.GoalStatus.SUCCEEDED)

if __name__ == '__main__':

    import rostest
    rostest.unitrun('test_motion_execution_buffer', 'test_motion_execution_buffer', TestMotionExecutionBuffer)


    
