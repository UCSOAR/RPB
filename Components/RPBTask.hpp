/**
 ******************************************************************************
 * File Name          : RPBTask.hpp
 * Description        : File System Task for managing USB storage operations
 ******************************************************************************
 */
#ifndef CUBE_SYSTEM_RPB_TASK_HPP_
#define CUBE_SYSTEM_RPB_TASK_HPP_

/* Includes ------------------------------------------------------------------*/
#include "Task.hpp"
#include "SystemDefines.hpp"
#include <stdint.h>
#include "CanAutoNodeDaughter.hpp"
#include "RPBLogs.hpp"
#include "CANBusTargets.hpp"

/* Enums ------------------------------------------------------------------*/
enum RPB_TASK_COMMANDS
{
    RPB_TASK_COMMAND_NONE = 0,
    EVENT_RPB_INIT,
    EVENT_RPB_TEST,
    EVENT_RPB_LOG_DATA,
    EVENT_RPB_CLEANUP
};

/* Macros ------------------------------------------------------------------*/

extern FDCAN_HandleTypeDef hfdcan1;
/* Class ------------------------------------------------------------------*/
class RPBTask : public Task
{
public:
    static RPBTask &Inst()
    {
        static RPBTask inst;
        return inst;
    }

    void InitTask();


    bool CANSendToMotherboardDirect(uint16_t logID, const uint8_t* msg);

protected:
    static void RunTask(void *pvParams)
    {
        RPBTask::Inst().Run(pvParams);
    } // Static Task Interface, passes control to the instance Run();

    void Run(void *pvParams); // Main run code
    void HandleCommand(Command &cm);

private:
    // Private Functions
    RPBTask();                                  // Private constructor
    RPBTask(const RPBTask &);            // Prevent copy-construction
    RPBTask &operator=(const RPBTask &); // Prevent assignment

    CanAutoNodeDaughter dau = {&hfdcan1,RPB_Init,RPB_LogIndexes::_COUNT,123,123,CAN_ROCKET_TARGET_RPB};

};

#endif // CUBE_SYSTEM_RPB_TASK_HPP_
