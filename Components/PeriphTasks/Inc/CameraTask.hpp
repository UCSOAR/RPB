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
enum Camera_TASK_COMMANDS
{
	Camera_COMMAND_NONE = 0,
    Camera_COMMAND_SELECT,
	Camera_COMMAND_VIDEO_ENABLE,
	Camera_COMMAND_VIDEO_DISABLE,
	Camera_COMMAND_START_RECORDING,
	Camera_COMMAND_STOP_RECORDING,
	Camera_COMMAND_POWER_ON,
	Camera_COMMAND_POWER_OFF
};

/* Macros ------------------------------------------------------------------*/

extern SPI_HandleTypeDef hspi2;
//extern UART_HandleTypeDef huart1;
//extern UART_HandleTypeDef huart2;
//extern UART_HandleTypeDef huart3;

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
