/*
 * CANMessageHandler.cpp
 *
 *  Created on: May 3, 2026
 *      Author: Local user
 */


#include "CANTask.hpp"
#include "CameraTask.hpp"
#include "AirbrakesTask.hpp"

/* @brief Handle user-defined CANBus messages.
 * @return true if any message found.
 */
bool CANTask::HandleCANCommands() {
	bool foundone = false;
	{
		RPB_AIR_BRAKES_COMMAND cmd;
		if(dau.ReadMessageByLogIndex(_RPB_AIR_BRAKES_COMMAND_LOGINDEX, (uint8_t*)&cmd, sizeof(cmd))) {
			SOAR_PRINT("got airbrakes cmd %d\n",cmd.openAirBrakes);
			if(cmd.openAirBrakes) {
				AirbrakesTask::Inst().SendCommand({TASK_SPECIFIC_COMMAND,AIRBRAKES_COMMAND_OPEN});

			} else {
				AirbrakesTask::Inst().SendCommand({TASK_SPECIFIC_COMMAND,AIRBRAKES_COMMAND_CLOSE});


			}


		}
		foundone= true;
	}

	{
		RPB_FROM_DAQ_AIR_BRAKES_LEVEL cmd;
		if(dau.ReadMessageByLogIndex(_RPB_FROM_DAQ_AIR_BRAKES_LEVEL_LOGINDEX, (uint8_t*)&cmd, sizeof(cmd))) {
			SOAR_PRINT("got airbrakes lvl cmd %d\n",cmd.level);
			Command cm = {TASK_SPECIFIC_COMMAND,AIRBRAKES_COMMAND_SET_LEVEL};
			cm.CopyDataToCommand(&cmd.level, sizeof(cmd.level));
			AirbrakesTask::Inst().SendCommandReference(cm);


		}
		foundone= true;
	}

	{
		RPB_CAMERA_POWER_COMMAND cmd;
		if(dau.ReadMessageByLogIndex(_RPB_CAMERA_POWER_COMMAND_LOGINDEX, (uint8_t*)&cmd, sizeof(cmd))) {
			SOAR_PRINT("got cam power cmd %d\n",cmd.camera);
			if(cmd.enable) {
				Command cm = {TASK_SPECIFIC_COMMAND,CAMERA_COMMAND_POWER_ON};
				cm.CopyDataToCommand((uint8_t*)&cmd.camera, sizeof(cmd.camera));
				CameraTask::Inst().SendCommandReference(cm);
			} else {
				Command cm = {TASK_SPECIFIC_COMMAND,CAMERA_COMMAND_POWER_OFF};
				cm.CopyDataToCommand((uint8_t*)&cmd.camera, sizeof(cmd.camera));
				CameraTask::Inst().SendCommandReference(cm);
			}
		}
		foundone = true;
	}
	{
		RPB_CAMERA_RECORDING_COMMAND cmd;
		if(dau.ReadMessageByLogIndex(_RPB_CAMERA_RECORDING_COMMAND_LOGINDEX, (uint8_t*)&cmd, sizeof(cmd))) {
			SOAR_PRINT("got cam record cmd %d\n",cmd.record);
			if(cmd.record) {
				Command cm = {TASK_SPECIFIC_COMMAND,CAMERA_COMMAND_START_RECORDING};
				cm.CopyDataToCommand((uint8_t*)&cmd.camera, sizeof(cmd.camera));
				CameraTask::Inst().SendCommandReference(cm);
			} else {
				Command cm = {TASK_SPECIFIC_COMMAND,CAMERA_COMMAND_STOP_RECORDING};
				cm.CopyDataToCommand((uint8_t*)&cmd.camera, sizeof(cmd.camera));
				CameraTask::Inst().SendCommandReference(cm);
			}
			foundone = true;
		}
	}

	{
		RPB_CAMERA_SWITCH_COMMAND cmd;
		if(dau.ReadMessageByLogIndex(_RPB_CAMERA_SWITCH_COMMAND_LOGINDEX, (uint8_t*)&cmd,sizeof(cmd))) {
			SOAR_PRINT("got switch cmd %d\n",cmd.camera);
			Command cm = {TASK_SPECIFIC_COMMAND,CAMERA_COMMAND_SELECT};
			cm.CopyDataToCommand(&cmd.camera, sizeof(cmd.camera));
			CameraTask::Inst().SendCommandReference(cm);
		}
	}

	return foundone;
};
