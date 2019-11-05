#include "Dummy.h"

static DriveTrainLibraryStatus_t Dummy_Init(const ByteArray_t config)
{
    if (config.count != 0)
    {
        return DriveTrainLibraryStatus_InputError_Length;
    }

    return DriveTrainLibraryStatus_Ok;
}

static DriveTrainLibraryStatus_t Dummy_DeInit(void)
{
    return DriveTrainLibraryStatus_Ok;
}

static DriveTrainLibraryStatus_t Dummy_Update(void)
{
    return DriveTrainLibraryStatus_Ok;
}

static DriveTrainLibraryStatus_t Dummy_Command(const ByteArray_t config)
{
    (void) config;
    
    return DriveTrainLibraryStatus_InputError_Value;
}

const DriveTrainLibrary_t library_dummy = {
    .name = "Dummy",

    .Init = &Dummy_Init,
    .DeInit = &Dummy_DeInit,
    .Update = &Dummy_Update,

    .Command = &Dummy_Command
};
