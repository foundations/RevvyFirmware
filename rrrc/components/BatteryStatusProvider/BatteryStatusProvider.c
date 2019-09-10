#include "BatteryStatusProvider.h"
#include <utils_assert.h>

Comm_Status_t BatteryStatusProvider_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    (void) commandPayload;
    
    if (commandSize != 0u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }

    ASSERT(responseBufferSize >= 3u);

    response[0] = BatteryStatusProvider_Read_IsMainBatteryCharging();
    response[1] = BatteryStatusProvider_Read_MainBatteryLevel();
    response[2] = BatteryStatusProvider_Read_MotorBatteryLevel();
    *responseCount = 3u;

    return Comm_Status_Ok;
}

__attribute__((weak))
uint8_t BatteryStatusProvider_Read_MainBatteryLevel(void)
{
    return 0u;
}

__attribute__((weak))
uint8_t BatteryStatusProvider_Read_MotorBatteryLevel(void)
{
    return 0u;
}

__attribute__((weak))
ChargerState_t BatteryStatusProvider_Read_IsMainBatteryCharging(void)
{
    return ChargerState_NotPluggedIn;
}
