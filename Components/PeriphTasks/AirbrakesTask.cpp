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

	uint16_t test = 0;
	uint16_t testlevel = 0;
	while (1)
	{
		/* Process commands */

		test++;

		if(test >= 20) {
			test = 0;
			airbrakesDriver.SetTargetLevel(testlevel);
			testlevel++;
			if(testlevel >= AIRBRAKES_NUM_DEPLOYMENT_LEVELS) {
				testlevel = 0;
			}
		}
		Command cm;
		bool res = qEvtQueue->Receive(cm);
		if (res)
		{
			HandleCommand(cm);
		}

		airbrakesDriver.Adjust();
		osDelay(50);

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

		case Airbrakes_COMMAND_SET_LEVEL: {
			uint8_t lvl = *cm.GetDataPointer();
			airbrakesDriver.SetTargetLevel(lvl);
			break;
		}
		case Airbrakes_COMMAND_ENABLE:
			airbrakesDriver.Enable();
			break;

		case Airbrakes_COMMAND_DISABLE:
			airbrakesDriver.SetTargetLevel(0);
			airbrakesDriver.Disable();
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

