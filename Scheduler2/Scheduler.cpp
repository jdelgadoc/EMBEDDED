#include "Scheduler.hpp"

// - Scheduler constructor
Scheduler::Scheduler()
{
    m_u8OpenSlots = static_cast<uint8_t>(NUMBER_OF_SLOTS);
    m_u8NextSlot = 0;
    for(int index = 0; index < NUMBER_OF_SLOTS; index++)
    {
        m_aSchedule[index].pToAttach = (uintptr_t) 0; // Init to an invalid pointer
        m_aSMSQ[index].u8Type = 0; //invalid message code type
    }
    return;
}

void Scheduler::AttachMessage(SMS iNewSMS){

    m_aSMSQ[m_u8SMSIndex]=iNewSMS;
    m_u8SMSIndex++;
}
void Scheduler::ProcessSMS(SMS iNewSMS){
    if(iNewSMS.u8Type==50) //add sender to next Schedule
    {
        m_aSchedule[iNewSMS.u8Sender].pToAttach->SetToExec(true);
    }
}

void    Scheduler::GetMessage(void){
    int l_iNextSMSSlot = 0U;
    while(l_iNextSMSSlot < NUMBER_OF_SLOTS)
    {
        if(m_aSMSQ[l_iNextSMSSlot].u8Type != 0){
            if(m_aSMSQ[l_iNextSMSSlot].u8Receiver != 0){  // SCHEDULER ID = 0
                 m_aSchedule[m_aSMSQ[l_iNextSMSSlot].u8Receiver].pToAttach->ProcessSMS(m_aSMSQ[l_iNextSMSSlot]);
            }else{
                    this->ProcessSMS(m_aSMSQ[l_iNextSMSSlot]);
            }
        }
        l_iNextSMSSlot++;
    }
    ClearSMSQ();
}

void    Scheduler::ClearSMSQ(void){
    int l_iNextSMSSlot = 0U;
    while(l_iNextSMSSlot < NUMBER_OF_SLOTS)
    {
        m_aSMSQ[l_iNextSMSSlot].u8Type = 0U; //invalidate all messages
        l_iNextSMSSlot++;

    }
    m_u8SMSIndex = 0U;
}
// - The attach function, inserts the task into the schedule slots.
uint8_t Scheduler::attach(Task * i_ToAttach, uint64_t i_u64TickInterval)
{
    uint8_t l_ErrorCode = NO_ERR;
    st_TaskInfo l_st_StructToAttach;

    l_st_StructToAttach.pToAttach = i_ToAttach;
    l_st_StructToAttach.bIsPeriodic = true;
    l_st_StructToAttach.u64ticks = this->m_u64ticks;
    l_st_StructToAttach.u64TickInterval = 0;
	l_st_StructToAttach.u64TickIntervalInitValue = i_u64TickInterval;


    if((m_u8OpenSlots>0) && (m_u8NextSlot < NUMBER_OF_SLOTS))
    {
        m_aSchedule[i_ToAttach->GetTaskID()] =  l_st_StructToAttach;
    }
    else
    {
        l_ErrorCode = RET_ERR;
    }
    return l_ErrorCode;
}

uint8_t Scheduler::attach(Task * i_ToAttach)
{
    uint8_t l_ErrorCode = NO_ERR;
    st_TaskInfo l_st_StructToAttach;

    l_st_StructToAttach.pToAttach = i_ToAttach;
    l_st_StructToAttach.bIsPeriodic = false;
    l_st_StructToAttach.u64ticks = this->m_u64ticks;
    l_st_StructToAttach.u64TickInterval = 0;
    l_st_StructToAttach.u64TickIntervalInitValue = 0;


    if((m_u8OpenSlots>0) && (m_u8NextSlot < NUMBER_OF_SLOTS))
    {
        m_aSchedule[i_ToAttach->GetTaskID()] =  l_st_StructToAttach;
        m_u8OpenSlots--;
    }
    else
    {
        l_ErrorCode = RET_ERR;
    }
    return l_ErrorCode;
}
// - Execute the current schedule
uint8_t Scheduler::run(void)
{
    int l_iNextTaskSlot = 0U;
    Task * l_pNextTask = (uintptr_t) 0;
    uint8_t l_u8ReturnCode = NO_ERR;



    while(l_iNextTaskSlot < NUMBER_OF_SLOTS)
    {
    	l_pNextTask = static_cast<Task *> (m_aNextSchedule[l_iNextTaskSlot].pToAttach);
		if(l_pNextTask != ((uintptr_t) 0))
		{
		    l_pNextTask->run();
//			if(m_aSchedule[l_pNextTask->GetTaskID()].u64TickInterval == 0){
//				l_pNextTask->run();
//			}
//			m_aSchedule[l_iNextTaskSlot].u64TickInterval ++;
//
//			if(m_aSchedule[l_iNextTaskSlot].u64TickInterval > m_aSchedule[l_iNextTaskSlot].u64TickIntervalInitValue) {
//				m_aSchedule[l_iNextTaskSlot].u64TickInterval = 0;
//			}
		    l_iNextTaskSlot++;
		}else{
		    break;
		}
    }
    ClearNextSchedule();


    return l_u8ReturnCode;
}
// - Execute the setup function for all tasks

void Scheduler::ClearNextSchedule(void){
    for(int index = 0; index < NUMBER_OF_SLOTS; index++)
    {
        m_aNextSchedule[index].pToAttach = (uintptr_t) 0; // Init to an invalid pointer
    }
    m_u8NextSlot = 0U;
    m_u8OpenSlots = static_cast<uint8_t>(NUMBER_OF_SLOTS);

    return;
}
uint8_t Scheduler::setup(void)
{
    int l_iNextTaskSlot = 0U;
    Task * l_pNextTask = (uintptr_t) 0;
    uint8_t l_u8ReturnCode = NO_ERR;

    while(l_iNextTaskSlot < NUMBER_OF_SLOTS)
    {
        l_pNextTask = static_cast<Task *> (m_aSchedule[l_iNextTaskSlot].pToAttach);
        if(l_pNextTask != ((uintptr_t) 0))
        {
            l_pNextTask->setup();
        }
        l_iNextTaskSlot++;
    }
    return l_u8ReturnCode;
}

uint8_t Scheduler::CalculateNextSchedule(void)
{
    int l_iNextTaskSlot = 0U;
    Task * l_pNextTask = (uintptr_t) 0;


    while(l_iNextTaskSlot < NUMBER_OF_SLOTS)
    {
        l_pNextTask = static_cast<Task *> (m_aSchedule[l_iNextTaskSlot].pToAttach);
        if(l_pNextTask != ((uintptr_t) 0))
        {
            if (m_aSchedule[l_iNextTaskSlot].bIsPeriodic)
            {
                if(m_aSchedule[l_iNextTaskSlot].u64TickInterval == 0){
                    m_aNextSchedule[m_u8NextSlot]=m_aSchedule[l_iNextTaskSlot];

                    m_u8NextSlot++;
                    m_u8OpenSlots--;
                }
                m_aSchedule[l_iNextTaskSlot].u64TickInterval ++;

                if(m_aSchedule[l_iNextTaskSlot].u64TickInterval > m_aSchedule[l_iNextTaskSlot].u64TickIntervalInitValue) {
                    m_aSchedule[l_iNextTaskSlot].u64TickInterval = 0;
                }
            }else{
                if(m_aSchedule[l_iNextTaskSlot].pToAttach->IsTaskToExec()){
                    m_aNextSchedule[m_u8NextSlot]=m_aSchedule[l_iNextTaskSlot];
                    m_aSchedule[l_iNextTaskSlot].pToAttach->SetToExec(false); //wait until task is ready to execute again
                    m_u8NextSlot++;
                    m_u8OpenSlots--;
                }
            }
        }
        l_iNextTaskSlot++;
    }
    return(NO_ERR);
}
uint8_t Scheduler::SortScheduleByPriority(Task * i_pSchedule)
{
    return(NO_ERR);
}
