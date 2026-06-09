/*
 * CameraTask.hpp
 *
 *  Created on: May 5, 2026
 *      Author: Local user
 */

#ifndef PERIPHTASKS_INC_CAMERATASK_HPP_
#define PERIPHTASKS_INC_CAMERATASK_HPP_


#include "Task.hpp"
#include "SystemDefines.hpp"
#include <stdint.h>
#include "stm32g4xx.h"
#include "main.h"
#include "MAX7456.hpp"
#include "TMUX1104Driver.hpp"

/* Enums ------------------------------------------------------------------*/
enum CAMERA_TASK_COMMANDS
{
	CAMERA_COMMAND_NONE = 0,
    CAMERA_COMMAND_SELECT,
	CAMERA_COMMAND_VIDEO_ENABLE,
	CAMERA_COMMAND_VIDEO_DISABLE,
	CAMERA_COMMAND_START_RECORDING,
	CAMERA_COMMAND_STOP_RECORDING,
	CAMERA_COMMAND_POWER_ON,
	CAMERA_COMMAND_POWER_OFF,
	CAMERA_COMMAND_SIM_BUTTON
};

/* Macros ------------------------------------------------------------------*/

extern SPI_HandleTypeDef hspi2;

/* Class ------------------------------------------------------------------*/
class CameraTask : public Task
{
public:
    static CameraTask &Inst()
    {
        static CameraTask inst;
        return inst;
    }

    void InitTask();

protected:
    static void RunTask(void *pvParams)
    {
        CameraTask::Inst().Run(pvParams);
    } // Static Task Interface, passes control to the instance Run();

    void Run(void *pvParams); // Main run code
    void HandleCommand(Command &cm);
    bool HandleCameraCommands();

    uint8_t RunCam_CRC8(uint8_t *data, uint8_t len);

    void RunCamCommand(USART_TypeDef* uart, uint8_t action);
private:
    // Private Functions
    CameraTask();                                  // Private constructor
    CameraTask(const CameraTask &);            // Prevent copy-construction
    CameraTask &operator=(const CameraTask &); // Prevent assignment

    MAX7456 osdDriver = {OSD_CS_GPIO_Port,OSD_CS_Pin,&hspi2};
    TMUX1104Driver muxDriver = {Camera_Select_1_GPIO_Port,Camera_Select_1_Pin,Camera_Select_2_GPIO_Port,Camera_Select_2_Pin,Video_Enable_GPIO_Port,Video_Enable_Pin};

};


#endif /* PERIPHTASKS_INC_CAMERATASK_HPP_ */
