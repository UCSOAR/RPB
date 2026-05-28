/*
 * AirbrakesTask.hpp
 *
 *  Created on: May 4, 2026
 *      Author: Local user
 */

#ifndef PERIPHTASKS_INC_AirbrakesTASK_HPP_
#define PERIPHTASKS_INC_AirbrakesTASK_HPP_


#include "Task.hpp"
#include "SystemDefines.hpp"
#include <stdint.h>
#include "airbrakes.hpp"
#include "stm32g4xx.h"
#include "main.h"

/* Enums ------------------------------------------------------------------*/
enum AIRBRAKES_TASK_COMMANDS
{
	AIRBRAKES_COMMAND_NONE = 0,
    AIRBRAKES_COMMAND_SET_LEVEL,
	AIRBRAKES_COMMAND_OPEN,
	AIRBRAKES_COMMAND_CLOSE
};

/* Macros ------------------------------------------------------------------*/

extern SPI_HandleTypeDef hspi2;
extern TIM_HandleTypeDef htim3;
extern ADC_HandleTypeDef hadc2;
/* Class ------------------------------------------------------------------*/
class AirbrakesTask : public Task
{
public:
    static AirbrakesTask &Inst()
    {
        static AirbrakesTask inst;
        return inst;
    }

    void InitTask();

protected:
    static void RunTask(void *pvParams)
    {
        AirbrakesTask::Inst().Run(pvParams);
    } // Static Task Interface, passes control to the instance Run();

    void Run(void *pvParams); // Main run code
    void HandleCommand(Command &cm);
    bool HandleAirbrakesCommands();

private:
    // Private Functions
    AirbrakesTask();                                  // Private constructor
    AirbrakesTask(const AirbrakesTask &);            // Prevent copy-construction
    AirbrakesTask &operator=(const AirbrakesTask &); // Prevent assignment


	AirbrakesDriver airbrakesDriver = {&htim3, &hadc2,
			Servo_Enable_GPIO_Output_GPIO_Port, Servo_Enable_GPIO_Output_Pin,
			Latch_Resent_GPIO_Output_GPIO_Port, Latch_Resent_GPIO_Output_Pin,
			Latch_Status_GPIO_Input_GPIO_Port, Latch_Status_GPIO_Input_Pin, 71.806};

	int storedLevel = 0;
	bool goodToOpen = false;

};


#endif /* PERIPHTASKS_INC_AirbrakesTASK_HPP_ */
