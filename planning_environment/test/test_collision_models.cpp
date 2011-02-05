/*********************************************************************
* Software License Agreement (BSD License)
* 
*  Copyright (c) 2008, Willow Garage, Inc.
*  All rights reserved.
* 
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
* 
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Willow Garage nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
* 
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

/** \author Ioan Sucan */

#include <planning_environment/models/collision_models.h>
#include <planning_models/kinematic_state.h>
#include <ros/time.h>
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <ros/package.h>

static const std::string rel_path = "/test_urdf/robot.xml";

static const std::string FLOATING_JOINT_XML = 
  "<value>"
  "<array>"
  "<data>"
  "<value>"
  "<struct>"
  "<member>"
  "<name>name</name>"
  "<value><string>base_joint</string></value>"
  "</member>"
  "<member>"
  "<name>parent_frame_id</name>"
  "<value><string>base_footprint</string></value>"
  "</member>"
  "<member>"
  "<name>child_frame_id</name>"
  "<value><string>base_footprint</string></value>"
  "</member>"
  "<member>"
  "<name>type</name>"
  "<value><string>Floating</string></value>"
  "</member>"
  "</struct>"
  "</value>"
  "</data>"
  "</array>"
  "</value>";

static const std::string RIGHT_ARM_GROUP_XML = 
  "<value>"
  "<array>"
  "<data>"
  "<value>"
  "<struct>"
  "<member>"
  "<name>name</name>"
  "<value><string>right_arm</string></value>"
  "</member>"
  "<member>"
  "<name>base_link</name>"
  "<value><string>torso_lift_link</string></value>"
  "</member>"
  "<member>"
  "<name>tip_link</name>"
  "<value><string>r_wrist_roll_link</string></value>"
  "</member>"
  "</struct>"
  "</value>"
  "</data>"
  "</array>"
  "</value>";

class TestCollisionModels : public testing::Test 
{
protected:
  
  virtual void SetUp() {

    full_path_ = ros::package::getPath("planning_models")+rel_path;
    
    std::string com = "rosparam set robot_description -t "+full_path_;

    int ok = system(com.c_str());
    
    if(ok != 0) {
      ROS_WARN_STREAM("Setting parameter system call not ok");
    }

    int offset1=0;
    
    //this will be used by other tests unless another joint is pushed
    XmlRpc::XmlRpcValue floating_multi_dof_joint(FLOATING_JOINT_XML, &offset1);
    
    ASSERT_TRUE(floating_multi_dof_joint.valid());
    ASSERT_EQ(floating_multi_dof_joint.getType(),XmlRpc::XmlRpcValue::TypeArray); 
    
    nh_.setParam("robot_description_planning/multi_dof_joints", floating_multi_dof_joint);
    
    //and these groups
    offset1 = 0;
    XmlRpc::XmlRpcValue planning_groups(RIGHT_ARM_GROUP_XML, &offset1);
    
    ASSERT_TRUE(planning_groups.valid());
    ASSERT_EQ(planning_groups.getType(),XmlRpc::XmlRpcValue::TypeArray); 
    
    nh_.setParam("robot_description_planning/groups", planning_groups);
  }
  
protected:

  ros::NodeHandle nh_;
  std::string full_path_;
};

TEST_F(TestCollisionModels, Loading) 
{
  planning_environment::CollisionModels cm("robot_description");
  
  ASSERT_TRUE(cm.getKinematicModel() != NULL);
  ASSERT_TRUE(cm.getCollisionSpace() != NULL);
}

TEST_F(TestCollisionModels, InCollisionWithNodisables) 
{
  planning_environment::CollisionModels cm("robot_description");
  
  planning_models::KinematicState state(cm.getKinematicModel());

  state.setKinematicStateToDefault();

  EXPECT_TRUE(cm.isKinematicStateInCollision(state));
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  ros::init(argc, argv, "test_collision_models");
    
  return RUN_ALL_TESTS();
}

