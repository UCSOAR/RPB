/*
 * AirbrakesTask.cpp
 *
 *  Created on: May 4, 2026
 *      Author: Local user
 */


#include "AirbrakesTask.hpp"
#include "SystemDefines.hpp"
#include <stdint.h>
#include "stm32g4xx_hal.h"
#include "airbrakes.hpp"
#include "random"

/**
 * @brief Constructor, sets up task
 */
AirbrakesTask::AirbrakesTask() : Task(TASK_AIRBRAKES_QUEUE_DEPTH_OBJS)
{

}

/**
 * @brief Initialize the AirbrakesTask
 */
void AirbrakesTask::InitTask()
{
	// Make sure the task is not already initialized
	SOAR_ASSERT(rtTaskHandle == nullptr, "Cannot initialize RPB task twice");

	// Start the task
	BaseType_t rtValue =
			xTaskCreate((TaskFunction_t)AirbrakesTask::RunTask,
					(const char *)"AirbrakesTask",
					(uint16_t)TASK_AIRBRAKES_STACK_DEPTH_WORDS,
					(void *)this,
					(UBaseType_t)TASK_AIRBRAKES_PRIORITY,
					(TaskHandle_t *)&rtTaskHandle);

	// Ensure creation succeded
	SOAR_ASSERT(rtValue == pdPASS, "AirbrakesTask::InitTask() - xTaskCreate() failed");
}

/**
 * @brief Instance Run loop for the airbrakes Task, runs on scheduler start as long as the task is initialized.
 * @param pvParams RTOS Passed void parameters, contains a pointer to the object instance, should not be used
 */
void AirbrakesTask::Run(void *pvParams)
{
	SOAR_PRINT("AirbrakesTask::Run() - Starting task\n");

	osDelay(500);

	airbrakesDriver.Enable();
	while (1)
	{
		/* Process commands */

		Command cm;
		bool res = qEvtQueue->Receive(cm);
		if (res)
		{
			HandleCommand(cm);
		}


		if(!airbrakesDriver.TickControlLoop()) {
			osDelay(100);
			airbrakesDriver.Enable();
		}

		osDelay(10);

	}

}

/**
 * @brief Handles a command
 * @param cm Command reference to handle
 */
void AirbrakesTask::HandleCommand(Command &cm)
{
	// Handle task-specific commands
	if (cm.GetCommand() == TASK_SPECIFIC_COMMAND)
	{
		switch (cm.GetTaskCommand())
		{

		case AIRBRAKES_COMMAND_SET_LEVEL: {
			uint8_t lvl = *cm.GetDataPointer();
			storedLevel = lvl;
			airbrakesDriver.SetTargetLevel(lvl);
			break;
		}
		case AIRBRAKES_COMMAND_OPEN:
			goodToOpen = true;
			airbrakesDriver.SetTargetLevel(storedLevel);
			break;

		case AIRBRAKES_COMMAND_CLOSE:
			goodToOpen = false;
			airbrakesDriver.SetTargetLevel(0);
			break;

		default:
			SOAR_PRINT("AirbrakesTask - Received Unsupported Task Command {%d}\n", cm.GetTaskCommand());
			break;
		}
	}
	else
	{
		// Handle global commands if any
		SOAR_PRINT("AirbrakesTask - Received Unsupported Global Command {%d}\n", cm.GetCommand());
	}

	// No matter what happens, we must reset allocated data
	cm.Reset();
}

