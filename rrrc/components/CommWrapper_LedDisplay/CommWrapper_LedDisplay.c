#include "CommWrapper_LedDisplay.h"
#include "utils.h"
#include "utils_assert.h"

/* Begin User Code Section: Declarations */

Comm_Status_t CommWrapper_LedDisplay_GetScenarioTypes_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    (void) commandPayload;
    if (commandSize != 0u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }

    uint32_t len = 0u;
    for (uint32_t i = 0u; i < 6u; i++)
    {
        ASSERT (len < responseBufferSize);

        ByteArray_t destination = {
            .bytes = &response[len + 2u],
            .count = responseBufferSize - len - 2u
        };
        size_t name_length = CommWrapper_LedDisplay_Call_ReadScenarioName(i, destination);
        if (name_length == 0u)
        {
            /* buffer full */
            *responseCount = 0u;
            return Comm_Status_Error_InternalError;
        }

        response[len] = i;
        response[len + 1] = name_length;

        len = len + 2 + name_length;
    }
    *responseCount = len;

    return Comm_Status_Ok;
}

Comm_Status_t CommWrapper_LedDisplay_SetScenarioType_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    (void) response;
    (void) responseCount;
    (void) responseBufferSize;

    if (commandSize != 1u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }

    uint8_t idx = commandPayload[0];
    if (idx >= 6u)
    {
        return Comm_Status_Error_CommandError;
    }

    CommWrapper_LedDisplay_Write_Scenario((RingLedScenario_t) idx);

    return Comm_Status_Ok;
}

Comm_Status_t CommWrapper_LedDisplay_GetRingLedAmount_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    (void) commandPayload;
    if (commandSize != 0u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }

    ASSERT(responseBufferSize >= 1u);

    *response = 12u;
    *responseCount = 1u;

    return Comm_Status_Ok;
}

Comm_Status_t CommWrapper_LedDisplay_SetUserFrame_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    (void) response;
    (void) responseCount;
    (void) responseBufferSize;

    if (commandSize != 12u * sizeof(rgb565_t))
    {
        return Comm_Status_Error_PayloadLengthError;
    }

    for (uint32_t i = 0u; i < 12u; i++)
    {
        uint16_t c = commandPayload[2 * i] | (commandPayload[2 * i + 1] << 8u);
        rgb565_t color = {
            .R = (c & 0xF800u) >> 11u,
            .G = (c & 0x07E0u) >>  5u,
            .B = (c & 0x001Fu) >>  0u
        };
        CommWrapper_LedDisplay_Write_UserFrame(i, rgb565_to_rgb(color));
    }

    return Comm_Status_Ok;
}

/* End User Code Section: Declarations */

__attribute__((weak))
uint8_t CommWrapper_LedDisplay_Call_ReadScenarioName(RingLedScenario_t scenario, ByteArray_t destination)
{
    (void) scenario;
    (void) destination;
    /* Begin User Code Section: ReadScenarioName Start */

    /* End User Code Section: ReadScenarioName Start */
    /* Begin User Code Section: ReadScenarioName End */

    /* End User Code Section: ReadScenarioName End */
    return 0u;
}

__attribute__((weak))
void CommWrapper_LedDisplay_Write_Scenario(const RingLedScenario_t value)
{
    (void) value;
    /* Begin User Code Section: Scenario Start */

    /* End User Code Section: Scenario Start */
    /* Begin User Code Section: Scenario End */

    /* End User Code Section: Scenario End */
}

__attribute__((weak))
void CommWrapper_LedDisplay_Write_UserFrame(uint32_t index, const rgb_t value)
{
    (void) value;
    (void) index;
    ASSERT(index < 12);
    /* Begin User Code Section: UserFrame Start */

    /* End User Code Section: UserFrame Start */
    /* Begin User Code Section: UserFrame End */

    /* End User Code Section: UserFrame End */
}
