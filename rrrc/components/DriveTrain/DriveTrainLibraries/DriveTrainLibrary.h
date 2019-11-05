#ifndef DRIVE_TRAIN_LIBRARY_H_
#define DRIVE_TRAIN_LIBRARY_H_

#include "../DriveTrainInternal.h"

typedef enum {
    DriveTrainLibraryStatus_Ok,
    DriveTrainLibraryStatus_InputError_Length,
    DriveTrainLibraryStatus_InputError_Value
} DriveTrainLibraryStatus_t;

typedef struct
{
    const char* name;
    DriveTrainLibraryStatus_t (*Init)(const ByteArray_t config);
    DriveTrainLibraryStatus_t (*DeInit)(void);
    DriveTrainLibraryStatus_t (*Update)(void);

    DriveTrainLibraryStatus_t (*Command)(const ByteArray_t data);
} DriveTrainLibrary_t;

#endif /* DRIVE_TRAIN_LIBRARY_H_ */
