/**
 ******************************************************************************
 * File Name          : RPBTask.cpp
 * Description        : File System Task implementation for USB storage operations
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "RPBTask.hpp"
#include "SystemDefines.hpp"
#include <stdint.h>
#include "stm32g4xx_hal.h"

bool RPBTask::CANSendToMotherboardDirect(uint16_t logID, const uint8_t *msg) {
	return dau.SendMessageToMotherboardByLogID(logID, msg);
}

/**
 * @brief Constructor, sets up task
 */
RPBTask::RPBTask() : Task(TASK_RPB_QUEUE_DEPTH_OBJS)
{

}

/**
 * @brief Initialize the RPBTask
 */
void RPBTask::InitTask()
{
	// Make sure the task is not already initialized
	SOAR_ASSERT(rtTaskHandle == nullptr, "Cannot initialize RPB task twice");

	// Start the task
	BaseType_t rtValue =
			xTaskCreate((TaskFunction_t)RPBTask::RunTask,
					(const char *)"RPBTask",
					(uint16_t)TASK_RPB_STACK_DEPTH_WORDS,
					(void *)this,
					(UBaseType_t)TASK_RPB_PRIORITY,
					(TaskHandle_t *)&rtTaskHandle);

	// Ensure creation succeded
	SOAR_ASSERT(rtValue == pdPASS, "RPBTask::InitTask() - xTaskCreate() failed");
}

/**
 * @brief Instance Run loop for the RPB Task, runs on scheduler start as long as the task is initialized.
 * @param pvParams RTOS Passed void parameters, contains a pointer to the object instance, should not be used
 */
void RPBTask::Run(void *pvParams)
{
	SOAR_PRINT("RPBTask::Run() - Starting task\n");

	osDelay(500);




	while (1)
	{
		/* Process commands in blocking mode */

		if(dau.GetCurrentState() == dau.UNINITIALIZED) {
			osDelay(100);
			SOAR_PRINT("attempting joining...\n");
			if (dau.TryRequestingJoiningNetwork()) {
				SOAR_PRINT("joined\n");
			}
		}
		dau.CheckCANCommands();

		Command cm;
		bool res = qEvtQueue->Receive(cm);
		if (res)
		{
			HandleCommand(cm);
		}

		osDelay(10);

		{
			RPB_AIR_BRAKES_COMMAND cmd;
			if(dau.ReadMessageByLogIndex(RPB_RPB_AIR_BRAKES_COMMAND_LOGINDEX, (uint8_t*)&cmd, sizeof(cmd))) {
				SOAR_PRINT("got airbrakes cmd %d\n",cmd.openAirBrakes);
			}
		}

		{
			RPB_CAMERA_POWER_COMMAND cmd;
			if(dau.ReadMessageByLogIndex(RPB_RPB_CAMERA_POWER_COMMAND_LOGINDEX, (uint8_t*)&cmd, sizeof(cmd))) {
				SOAR_PRINT("got cam power cmd %d\n",cmd.cameraOn);
			}
		}
		{
			RPB_CAMERA_RECORDING_COMMAND cmd;
			if(dau.ReadMessageByLogIndex(RPB_RPB_CAMERA_RECORDING_COMMAND_LOGINDEX, (uint8_t*)&cmd, sizeof(cmd))) {
				SOAR_PRINT("got cam record cmd %d\n",cmd.startRecording);
			}
		}

		if(dau.GetCurrentState() == CanAutoNodeDaughter::ERROR) {
			while(1) {
				SOAR_PRINT("canautonode errored out!\n");
				osDelay(500);
			}

		}

	}

}

/**
 * @brief Handles a command
 * @param cm Command reference to handle
 */
void RPBTask::HandleCommand(Command &cm)
{
	// Handle task-specific commands
	if (cm.GetCommand() == TASK_SPECIFIC_COMMAND)
	{
		switch (cm.GetTaskCommand())
		{

		default:
			SOAR_PRINT("RPBTask - Received Unsupported Task Command {%d}\n", cm.GetTaskCommand());
			break;
		}
	}
	else
	{
		// Handle global commands if any
		SOAR_PRINT("RPBTask - Received Unsupported Global Command {%d}\n", cm.GetCommand());
	}

	// No matter what happens, we must reset allocated data
	cm.Reset();
}

