#include "DriveTrain.h"
#include "utils.h"
#include "utils_assert.h"

#include "DriveTrainLibraries/Dummy/Dummy.h"
#include "DriveTrainLibraries/Differential/Differential.h"

#include <string.h>

static const DriveTrainLibrary_t* libraries[] = {
    &library_dummy,
    &library_differential
};

static bool configuring_eh;
static const DriveTrainLibrary_t* requested_library;
static const DriveTrainLibrary_t* active_library;
static uint8_t command_buffer[6];
static ByteArray_t command;
static DriveTrainLibraryStatus_t command_response;

Comm_Status_t DriveTrain_GetTypes_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    (void) commandPayload;
    if (commandSize != 0u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }

    uint8_t len = 0u;
    for (uint32_t i = 0u; i < ARRAY_SIZE(libraries); i++)
    {
        const DriveTrainLibrary_t* lib = libraries[i];
        size_t name_length = strlen(lib->name);
        if (len + name_length + 2u > responseBufferSize)
        {
            *responseCount = 0u;
            return Comm_Status_Error_InternalError;
        }
        response[len] = i;
        response[len + 1] = name_length;
        memcpy(&response[len + 2], lib->name, name_length);
        len = len + 2 + name_length;
    }
    *responseCount = len;

    return Comm_Status_Ok;
}

Comm_Status_t DriveTrain_Configure_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    (void) response;
    (void) responseBufferSize;
    (void) responseCount;

    if (commandSize == 0u || commandSize > ARRAY_SIZE(command_buffer) + 1u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }

    if (commandPayload[0] > ARRAY_SIZE(libraries))
    {
        return Comm_Status_Error_CommandError;
    }

    requested_library = libraries[commandPayload[0]];
    memcpy(command_buffer, &commandPayload[1], commandSize - 1u);

    command.bytes = command_buffer;
    command.count = commandSize - 1u;

    configuring_eh = true;

    return Comm_Status_Ok;
}

Comm_Status_t DriveTrain_Configure_GetResult(uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    (void) response;
    (void) responseBufferSize;
    (void) responseCount;

    if (configuring_eh)
    {
        return Comm_Status_Pending;
    }
    else
    {
        switch (command_response)
        {
            case DriveTrainLibraryStatus_Ok:
                return Comm_Status_Ok;

            case DriveTrainLibraryStatus_InputError_Length:
                return Comm_Status_Error_PayloadLengthError;

            case DriveTrainLibraryStatus_InputError_Value:
                return Comm_Status_Error_CommandError;

            default:
                return Comm_Status_Error_InternalError;
        }
    }
}

Comm_Status_t DriveTrain_Control_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    (void) response;
    (void) responseBufferSize;
    (void) responseCount;

    const ByteArray_t command = {
        .bytes = commandPayload,
        .count = commandSize
    };

    switch (active_library->Command(command))
    {
        case DriveTrainLibraryStatus_Ok:
            return Comm_Status_Ok;

        case DriveTrainLibraryStatus_InputError_Length:
            return Comm_Status_Error_PayloadLengthError;

        case DriveTrainLibraryStatus_InputError_Value:
            return Comm_Status_Error_CommandError;

        default:
            return Comm_Status_Error_InternalError;
    }
}

void DriveTrain_Run_OnInit(void)
{
    active_library = &library_dummy;

    command.bytes = NULL;
    command.count = 0u;

    configuring_eh = true;
}

void DriveTrain_Run_Update(void)
{
    if (configuring_eh)
    {
        active_library->DeInit();
        active_library = requested_library;

        command_response = active_library->Init(command);
        configuring_eh = false;
    }
    else
    {
        active_library->Update();
    }
}

__attribute__((weak))
float DriveTrain_Read_YawAngle(void)
{
    return 0.0f;
}

__attribute__((weak))
void DriveTrain_Write_MotorUsed(uint32_t index, const bool isAssigned)
{
    (void) index;
    (void) isAssigned;
    ASSERT(index < 6);
}

__attribute__((weak))
void DriveTrain_Write_DriveRequest(uint32_t index, const DriveRequest_t* value)
{
    (void) index;
    ASSERT(value != NULL);
    ASSERT(index < 6);
    /* Begin User Code Section: DriveRequest Start */

    /* End User Code Section: DriveRequest Start */
    /* Begin User Code Section: DriveRequest End */

    /* End User Code Section: DriveRequest End */
}
