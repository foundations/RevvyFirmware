/*
 * MasterCommunication.c
 *
 * Created: 2019. 05. 06. 20:16:07
 *  Author: Dániel Buga
 */ 

#include "MasterCommunication.h"

void MasterCommunication_Run_OnInit(const Comm_CommandHandler_t* commandTable, size_t commandTableSize)
{
    Comm_Init(commandTable, commandTableSize);
}
