/*
 * rrrc_worklogic.h
 *
 * Created: 2/14/2019 11:38:39 AM
 *  Author: User
 */ 


#ifndef RRRC_WORKLOGIC_H_
#define RRRC_WORKLOGIC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define ERROR_ID_HARD_FAULT        (0u)
#define ERROR_ID_STACK_OVERFLOW    (1u)
#define ERROR_ID_ASSERTION_FAILURE (2u)
#define ERROR_ID_TEST_ERROR        (3u)
#define ERROR_ID_IMU_ERROR         (4u)
#define ERROR_ID_MASTER_I2C_ERROR  (5u)

#include "generated_runtime.h"

void RRRC_ProcessLogic_Init(void);
void RRRC_ProcessLogic_xTask(void* user_data);

#define COMM_HANDLER_COUNT  ((uint8_t) 0x41u)
const Comm_CommandHandler_t communicationHandlers[COMM_HANDLER_COUNT];

void UpdateMcuStatus_Gyroscope(const IMU_RawSample_t* sample);
void UpdateMcuStatus_YawAngle(int32_t angle, int32_t relativeAngle);

#ifdef __cplusplus
}
#endif

#endif /* RRRC_WORKLOGIC_H_ */

