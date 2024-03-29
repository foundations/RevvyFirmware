#ifndef COMPONENT_COMMUNICATION_OBSERVER_H_
#define COMPONENT_COMMUNICATION_OBSERVER_H_

#ifndef COMPONENT_TYPES_COMMUNICATION_OBSERVER_H_
#define COMPONENT_TYPES_COMMUNICATION_OBSERVER_H_

#include <stdbool.h>


#endif /* COMPONENT_TYPES_COMMUNICATION_OBSERVER_H_ */

void CommunicationObserver_Run_OnInit(void);
void CommunicationObserver_Run_OnMessageMissed(void);
void CommunicationObserver_Run_OnMessageReceived(void);
void CommunicationObserver_Run_Enable(void);
void CommunicationObserver_Call_ErrorLimitReached(void);
void CommunicationObserver_Write_Enabled(bool value);

#endif /* COMPONENT_COMMUNICATION_OBSERVER_H_ */
