#ifndef COMPONENT_MASTER_COMMUNICATION_INTERFACE_H_
#define COMPONENT_MASTER_COMMUNICATION_INTERFACE_H_

#ifndef COMPONENT_TYPES_MASTER_COMMUNICATION_INTERFACE_H_
#define COMPONENT_TYPES_MASTER_COMMUNICATION_INTERFACE_H_

#include <stdint.h>
#include <stdio.h>


typedef struct {
    uint8_t* payload;
    size_t size;
} MasterMessage_t;

typedef struct {
    MasterMessage_t default_response;
    MasterMessage_t rx_overflow_response;
    uint32_t rx_timeout;
} MasterCommunicationInterface_Config_t;

#endif /* COMPONENT_TYPES_MASTER_COMMUNICATION_INTERFACE_H_ */

void MasterCommunicationInterface_Run_OnInit(void);
void MasterCommunicationInterface_Run_SetResponse(MasterMessage_t response);
void MasterCommunicationInterface_Call_RxTimeout(void);
void MasterCommunicationInterface_Call_OnMessageReceived(MasterMessage_t message);
void MasterCommunicationInterface_Call_LogError(void);
void MasterCommunicationInterface_Read_Configuration(MasterCommunicationInterface_Config_t* value);

#endif /* COMPONENT_MASTER_COMMUNICATION_INTERFACE_H_ */
