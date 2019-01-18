﻿/*******************************************************************************
* Copyright 2018 ROBOTIS CO., LTD.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

/* Authors: Darby Lim, Hye-Jong KIM, Ryan Shim, Yong-Ho Na */

#include "../include/open_manipulator_6dof_libs/OpenManipulator.h"

OPEN_MANIPULATOR::OPEN_MANIPULATOR()
{}
OPEN_MANIPULATOR::~OPEN_MANIPULATOR()
{}

void OPEN_MANIPULATOR::initManipulator(bool using_platform, STRING usb_port, STRING baud_rate, float control_loop_time)
{
    ////////// manipulator parameter initialization

    addWorld("world",   // world name
             "joint1"); // child name

    addJoint("joint1", // my name
             "world",  // parent name
             "joint2", // child name
             RM_MATH::makeVector3(0.012, 0.0, 0.017), // relative position
             RM_MATH::convertRPYToRotation(0.0, 0.0, 0.0), // relative orientation
             Z_AXIS, // axis of rotation
             11,     // actuator id
             M_PI,   // max joint limit (3.14 rad)
             -M_PI); // min joint limit (-3.14 rad)


    addJoint("joint2", // my name
             "joint1", // parent name
             "joint3", // child name
             RM_MATH::makeVector3(0.0, 0.0, 0.0595), // relative position
             RM_MATH::convertRPYToRotation(0.0, 0.0, 0.0), // relative orientation
             Y_AXIS, // axis of rotation
             12,     // actuator id
             2.1,    // max joint limit (2.0 rad)
             -2.05);  // min joint limit (-2.06 rad)

    addJoint("joint3", // my name
             "joint2", // parent name
             "joint4", // child name
             RM_MATH::makeVector3(0.0, 0.0, 0.124), // relative position
             RM_MATH::convertRPYToRotation(0.0, 0.0, 0.0), // relative orientation
             Y_AXIS, // axis of rotation
             13,     // actuator id
             2.16,    // max joint limit (2.1 rad)
             -1.9);  // min joint limit (-2.0 rad)

    addJoint("joint4", // my name
             "joint3", // parent name
             "joint5",   // child name
             RM_MATH::makeVector3(0.0, 0.0, 0.045), // relative position
             RM_MATH::convertRPYToRotation(0.0, 0.0, 0.0), // relative orientation
             Z_AXIS, // axis of rotation
             14,     // actuator id
             M_PI,   // max joint limit (3.14 rad)
             -M_PI); // min joint limit (-3.14 rad)

    addJoint("joint5", // my name
             "joint4", // parent name
             "joint6",   // child name
             RM_MATH::makeVector3(0.0, 0.0, 0.0595), // relative position
             RM_MATH::convertRPYToRotation(0.0, 0.0, 0.0), // relative orientation
             Y_AXIS, // axis of rotation
             15,     // actuator id
             2.08,    // max joint limit (2.0 rad)
             -1.98);  // min joint limit (-2.0 rad)

    addJoint("joint6", // my name
             "joint5", // parent name
             "gripper",   // child name
             RM_MATH::makeVector3(0.0, 0.0, 0.0475), // relative position
             RM_MATH::convertRPYToRotation(0.0, 0.0, 0.0), // relative orientation
             Z_AXIS, // axis of rotation
             16,     // actuator id
             M_PI,    // max joint limit (3.14 rad)
             -M_PI);  // min joint limit (-3.14 rad)

    addTool("gripper",   // my name
            "joint6", // parent name
            RM_MATH::makeVector3(0.0, 0.0, 0.115), // relative position
            RM_MATH::convertRPYToRotation(0.0, 0.0, 0.0), // relative orientation
            17,     // actuator id
            0.010,  // max gripper limit (0.01 m)
            -0.010, // min gripper limit (-0.01 m)
            -0.015); // Change unit from `meter` to `radian`


    ////////// kinematics init.
    kinematics_ = new KINEMATICS::CR_SRJacobian_Solver();
    //kinematics_ = new KINEMATICS::CR_Position_Only_Jacobian_Solver();
    addKinematics(kinematics_);

    if(using_platform)
    {
        ////////// joint actuator init.
        //    actuator_ = new DYNAMIXEL::JointDynamixel();
        actuator_ = new DYNAMIXEL::JointDynamixelProfileControl(control_loop_time);
        // communication setting argument
        STRING dxl_comm_arg[2] = {usb_port, baud_rate};
        void *p_dxl_comm_arg = &dxl_comm_arg;

        // set joint actuator id
        jointDxlId.push_back(11);
        jointDxlId.push_back(12);
        jointDxlId.push_back(13);
        jointDxlId.push_back(14);
        jointDxlId.push_back(15);
        jointDxlId.push_back(16);
        addJointActuator(JOINT_DYNAMIXEL, actuator_, jointDxlId, p_dxl_comm_arg);

        // set joint actuator control mode
        STRING joint_dxl_mode_arg = "position_mode";
        void *p_joint_dxl_mode_arg = &joint_dxl_mode_arg;
        jointActuatorSetMode(JOINT_DYNAMIXEL, jointDxlId, p_joint_dxl_mode_arg);

        ////////// tool actuator init.
        tool_ = new DYNAMIXEL::GripperDynamixel();

        uint8_t gripperDxlId = 17;
        addToolActuator(TOOL_DYNAMIXEL, tool_, gripperDxlId, p_dxl_comm_arg);


        // set gripper actuator control mode
        STRING gripper_dxl_mode_arg = "current_based_position_mode";
        void *p_gripper_dxl_mode_arg = &gripper_dxl_mode_arg;
        toolActuatorSetMode(TOOL_DYNAMIXEL, p_gripper_dxl_mode_arg);

        // set gripper actuator parameter
        STRING gripper_dxl_opt_arg[2];
        void *p_gripper_dxl_opt_arg = &gripper_dxl_opt_arg;
        gripper_dxl_opt_arg[0] = "Profile_Acceleration";
        gripper_dxl_opt_arg[1] = "40";  //20
        toolActuatorSetMode(TOOL_DYNAMIXEL, p_gripper_dxl_opt_arg);

        gripper_dxl_opt_arg[0] = "Profile_Velocity";
        gripper_dxl_opt_arg[1] = "200";
        toolActuatorSetMode(TOOL_DYNAMIXEL, p_gripper_dxl_opt_arg);

        ////////// all actuator enable
        allActuatorEnable();
        receiveAllJointActuatorValue();
        receiveAllToolActuatorValue();
    }
    ////////// drawing path
    addCustomTrajectory(DRAWING_LINE, &line_);
    addCustomTrajectory(DRAWING_CIRCLE, &circle_);
    addCustomTrajectory(DRAWING_RHOMBUS, &rhombus_);
    addCustomTrajectory(DRAWING_HEART, &heart_);
}

void OPEN_MANIPULATOR::communicationProcessToActuator(JointWayPoint goal_joint_value, JointWayPoint goal_tool_value)
{
    receiveAllJointActuatorValue();
    receiveAllToolActuatorValue();
    if(goal_joint_value.size()) sendAllJointActuatorValue(goal_joint_value);
    if(goal_tool_value.size())  sendAllToolActuatorValue(goal_tool_value);

    forwardKinematics();
}

void OPEN_MANIPULATOR::calculationProcess(double tick_time, JointWayPoint* goal_joint_value, JointWayPoint* goal_tool_value)
{
    *goal_joint_value = getJointGoalValueFromTrajectoryTickTime(tick_time);
    *goal_tool_value  = getToolGoalValue();
}
