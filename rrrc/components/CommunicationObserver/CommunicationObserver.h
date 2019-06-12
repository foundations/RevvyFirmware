/*
 * CommunicationObserver.h
 *
 * Created: 2019. 05. 07. 21:27:41
 *  Author: Dániel Buga
 */ 


#ifndef COMMUNICATION_OBSERVER_H_
#define COMMUNICATION_OBSERVER_H_

void CommunicationObserver_Run_OnInit(void);
void CommunicationObserver_Run_Enable(void);
void CommunicationObserver_Run_OnMessageMissed(void);
void CommunicationObserver_Run_OnMessageReceived(void);

void CommunicationObserver_Call_ErrorThresholdReached(void);

#endif /* COMMUNICATION_OBSERVER_H_ */
