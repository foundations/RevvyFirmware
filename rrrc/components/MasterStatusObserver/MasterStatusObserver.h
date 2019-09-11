#ifndef COMPONENT_MASTER_STATUS_OBSERVER_H_
#define COMPONENT_MASTER_STATUS_OBSERVER_H_

#include "../MasterCommunication/CommunicationManager.h"

#ifndef COMPONENT_TYPES_MASTER_STATUS_OBSERVER_H_
#define COMPONENT_TYPES_MASTER_STATUS_OBSERVER_H_

#include <stdint.h>

typedef enum {
    MasterStatus_Unknown,
    MasterStatus_Operational,
    MasterStatus_Controlled
} MasterStatus_t;

#endif /* COMPONENT_TYPES_MASTER_STATUS_OBSERVER_H_ */

void MasterStatusObserver_Run_OnInit(void);
Comm_Status_t MasterStatusObserver_SetMasterStatus_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
void MasterStatusObserver_Write_MasterStatus(MasterStatus_t value);

#endif /* COMPONENT_MASTER_STATUS_OBSERVER_H_ */
