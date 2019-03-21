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

int32_t RRRC_Init(void);
int32_t RRRC_DeInit(void);

void RRRC_ProcessLogic_xTask(void* user_data);

#ifdef __cplusplus
}
#endif

#endif /* RRRC_WORKLOGIC_H_ */

