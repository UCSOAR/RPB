/*
 * CANMessageHandler.cpp
 *
 *  Created on: May 3, 2026
 *      Author: Local user
 */


#include "CANTask.hpp"
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
				AirbrakesTask::Inst().SendCommand({TASK_SPECIFIC_COMMAND, Airbrakes_COMMAND_ENABLE});
			} else {
				AirbrakesTask::Inst().SendCommand({TASK_SPECIFIC_COMMAND, Airbrakes_COMMAND_DISABLE});
			}
		}
		foundone= true;
	}

	{
		RPB_CAMERA_POWER_COMMAND cmd;
		if(dau.ReadMessageByLogIndex(_RPB_CAMERA_POWER_COMMAND_LOGINDEX, (uint8_t*)&cmd, sizeof(cmd))) {
			SOAR_PRINT("got cam power cmd %d\n",cmd.cameraOn);
		}
		foundone = true;
	}
	{
		RPB_CAMERA_RECORDING_COMMAND cmd;
		if(dau.ReadMessageByLogIndex(_RPB_CAMERA_RECORDING_COMMAND_LOGINDEX, (uint8_t*)&cmd, sizeof(cmd))) {
			SOAR_PRINT("got cam record cmd %d\n",cmd.startRecording);
		}
		foundone = true;
	}


	return foundone;
};
