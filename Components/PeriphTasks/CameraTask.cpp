/*
 * CameraTask.cpp
 *
 *  Created on: May 5, 2026
 *      Author: Local user
 */


#include "CameraTask.hpp"

#include "CameraTask.hpp"
#include "SystemDefines.hpp"
#include <stdint.h>
#include "stm32g4xx_hal.h"
#include "callsign.h"
#include "logo.h"
#include "RPBLogs.hpp"

/**
 * @brief Constructor, sets up task
 */
CameraTask::CameraTask() : Task(TASK_CAMERA_QUEUE_DEPTH_OBJS)
{

}

/**
 * @brief Initialize the CameraTask
 */
void CameraTask::InitTask()
{
	// Make sure the task is not already initialized
	SOAR_ASSERT(rtTaskHandle == nullptr, "Cannot initialize Camera task twice");

	// Start the task
	BaseType_t rtValue =
			xTaskCreate((TaskFunction_t)CameraTask::RunTask,
					(const char *)"CameraTask",
					(uint16_t)TASK_CAMERA_STACK_DEPTH_WORDS,
					(void *)this,
					(UBaseType_t)TASK_CAMERA_PRIORITY,
					(TaskHandle_t *)&rtTaskHandle);

	// Ensure creation succeded
	SOAR_ASSERT(rtValue == pdPASS, "CameraTask::InitTask() - xTaskCreate() failed");
}

/**
 * @brief Instance Run loop for the Camera Task, runs on scheduler start as long as the task is initialized.
 * @param pvParams RTOS Passed void parameters, contains a pointer to the object instance, should not be used
 */
void CameraTask::Run(void *pvParams)
{
	SOAR_PRINT("CameraTask::Run() - Starting task\n");

	osDelay(500);

	bool needToResetOSD = true;

	{
		uint16_t i = 0;
		while(osdDriver.OSD_Status() == 0x00 && i < 1000) { // wait up to a second for osd chip connection
			i++;
			osDelay(1);

		}
	}
	osdDriver.OSD_WriteCustomCharacter(0xD0, logo_tile_0);
	osdDriver.OSD_WriteCustomCharacter(0xD1, logo_tile_1);
	osdDriver.OSD_WriteCustomCharacter(0xD2, logo_tile_2);
	osdDriver.OSD_WriteCustomCharacter(0xD3, logo_tile_3);
	osdDriver.OSD_WriteCustomCharacter(0xD4, logo_tile_4);

	osdDriver.OSD_WriteCustomCharacter(0xE0, callsign_tile_0);
	osdDriver.OSD_WriteCustomCharacter(0xE1, callsign_tile_1);
	osdDriver.OSD_WriteCustomCharacter(0xE2, callsign_tile_2);
	osdDriver.OSD_WriteCustomCharacter(0xE3, callsign_tile_3);
	osdDriver.OSD_WriteCustomCharacter(0xE4, callsign_tile_4);

	while (1)
	{
		/* Process commands */

		Command cm;
		bool res = qEvtQueue->Receive(cm);
		if (res)
		{
			HandleCommand(cm);
		}



		osDelay(100);

		if(osdDriver.OSD_Status() == 0x00) {
			needToResetOSD = true;
		}

		if(needToResetOSD) {
			osdDriver.OSD_Reset();
			osDelay(250);


			osdDriver.OSD_SetStandard(osdDriver.OSD_STANDARD_NTSC);

			osdDriver.OSD_SetBackgroundBrightness(3);

			osdDriver.OSD_SetBlinking(0);
			osdDriver.OSD_SetInverted(0);
			osdDriver.OSD_SetOSDEnabled(1);
			osdDriver.OSD_SetOSDBL(1);

			osdDriver.OSD_DrawLogo(0xE0, 1, 1,5,1);
			osdDriver.OSD_DrawLogo(0xD0, 1, 11,5,1);

			needToResetOSD = false;


		}

	}

}

/**
 * @brief Handles a command
 * @param cm Command reference to handle
 */
void CameraTask::HandleCommand(Command &cm)
{
	// Handle task-specific commands
	if (cm.GetCommand() == TASK_SPECIFIC_COMMAND)
	{
		switch (cm.GetTaskCommand())
		{

		case CAMERA_COMMAND_SELECT: {
			uint8_t cam = *cm.GetDataPointer();
			switch (cam) {
			case 0:
				muxDriver.Select(Camera::CAMERA1);
				return;
			case 1:
				muxDriver.Select(Camera::CAMERA2);
				return;
			case 2:
				muxDriver.Select(Camera::CAMERA3);
				return;
			}
			SOAR_PRINT("Invalid camera %d should be 0-2\n",cam);
			break;

		}

		case CAMERA_COMMAND_VIDEO_DISABLE:
			osdDriver.OSD_SetVideoEnabled(false);
			HAL_GPIO_WritePin(VideoTX_Enable_GPIO_Port,VideoTX_Enable_Pin,GPIO_PIN_RESET);
			break;

		case CAMERA_COMMAND_VIDEO_ENABLE:
			osdDriver.OSD_SetVideoEnabled(true);
			HAL_GPIO_WritePin(VideoTX_Enable_GPIO_Port,VideoTX_Enable_Pin,GPIO_PIN_SET);
			break;

		case CAMERA_COMMAND_START_RECORDING: {
			uint8_t cam = *cm.GetDataPointer();
			USART_TypeDef* uart;
			switch (cam) {
			case 0:
				uart = USART1;
				break;
			case 1:
				uart = USART2;
				break;
			case 2:
				uart = USART3;
				break;
			default:
				SOAR_PRINT("invalid cam %d\n",cam);
				return;
			}
			RunCamCommand(uart, 0x03);
			break;
		}
		case CAMERA_COMMAND_STOP_RECORDING: {
			uint8_t cam = *cm.GetDataPointer();
			USART_TypeDef* uart;
			switch (cam) {
			case 0:
				uart = USART1;
				break;
			case 1:
				uart = USART2;
				break;
			case 2:
				uart = USART3;
				break;
			default:
				SOAR_PRINT("invalid cam %d\n",cam);
				return;
			}
			RunCamCommand(uart, 0x04);

			break;
		}
		case CAMERA_COMMAND_POWER_ON: {
			uint8_t cam = *cm.GetDataPointer();
			switch(cam) {
			case 0:
				HAL_GPIO_WritePin(C1_Enable_GPIO_Port, C1_Enable_Pin, GPIO_PIN_SET);
				break;
			case 1:
				HAL_GPIO_WritePin(C2_Enable_GPIO_Port, C2_Enable_Pin, GPIO_PIN_SET);
				break;
			case 2:
				HAL_GPIO_WritePin(C3_Enable_GPIO_Port, C3_Enable_Pin, GPIO_PIN_SET);
				break;
			default:
				SOAR_PRINT("invalid cam %d\n",cam);
				return;
			}
			break;
		}
		case CAMERA_COMMAND_POWER_OFF: {
			uint8_t cam = *cm.GetDataPointer();
			switch(cam) {
			case 0:
				HAL_GPIO_WritePin(C1_Enable_GPIO_Port, C1_Enable_Pin, GPIO_PIN_RESET);
				break;
			case 1:
				HAL_GPIO_WritePin(C2_Enable_GPIO_Port, C2_Enable_Pin, GPIO_PIN_RESET);
				break;
			case 2:
				HAL_GPIO_WritePin(C3_Enable_GPIO_Port, C3_Enable_Pin, GPIO_PIN_RESET);
				break;
			default:
				SOAR_PRINT("invalid cam %d\n",cam);
				return;
			}
			break;
		}
		case CAMERA_COMMAND_IRC_TRAMP: {
			RPB_IRCTRAMP_SET_COMMAND cmd = *(RPB_IRCTRAMP_SET_COMMAND*)cm.GetDataPointer();
			if(cmd.setFreq) {
				IRCTramp::SetVTXFrequency(cmd.freq, UART5);
				osDelay(10);
			}
			if(cmd.setPower) {
				IRCTramp::SetVTXPower(cmd.power, UART5);
			}
			break;
		}
		default:
			SOAR_PRINT("CameraTask - Received Unsupported Task Command {%d}\n", cm.GetTaskCommand());
			break;
		}
	}
	else
	{
		// Handle global commands if any
		SOAR_PRINT("CameraTask - Received Unsupported Global Command {%d}\n", cm.GetCommand());
	}

	// No matter what happens, we must reset allocated data
	cm.Reset();
}

uint8_t CameraTask::RunCam_CRC8(uint8_t* data, uint8_t len) {
	{
		uint8_t crc = 0;
		for (uint8_t i = 0; i < len; i++) {
			crc ^= data[i];
			for (uint8_t j = 0; j < 8; j++) {
				if (crc & 0x80) {
					crc = (crc << 1) ^ 0xD5;
				} else {
					crc <<= 1;
				}
			}
		}
		return crc;
	}
}

void CameraTask::RunCamCommand(USART_TypeDef* uart, uint8_t action) {

	uint8_t packet[4];

	packet[0] = 0xCC;        // 1. Fixed Header
	packet[1] = 0x01;        // 2. Command ID (0x01 = Camera Control)
	packet[2] = action;   // 3. The action you want to perform

	// 4. Calculate the CRC for the first 3 bytes and append it
	packet[3] = RunCam_CRC8(packet, 3);

	// Transmit via simple blocking mode (100ms timeout)
	for(uint8_t i = 0; i < sizeof(packet); i++) {
		LL_USART_TransmitData8(uart, packet[i]);
	}

}
