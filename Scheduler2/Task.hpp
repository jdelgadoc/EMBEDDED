#ifndef TASK_HPP_
#define TASK_HPP_

#include <ti/devices/msp432p4xx/inc/msp.h>


#define NO_ERR 0
#define RET_ERR 1

// - This structure defines the Message Information
struct SMS {
    uint8_t u8Sender;
    uint8_t u8Receiver;
    uint8_t u8Type;
    uint16_t u16Data;
};

class Task
{
	public:
		Task();
		virtual uint8_t     run(void){return(0);};
		virtual uint8_t     setup(void){return(0);};
		bool                IsTaskFinished(void){return m_bIsFinished;};
		bool                IsTaskToExec(void){return m_bToExec;};
		void                SetToExec(bool iNewToExec){this->m_bToExec = iNewToExec;};
		void                SetTaskID(uint8_t i_u8NewID){m_u8TaskID = i_u8NewID;};
		uint8_t             GetTaskID(void){return m_u8TaskID;};
		virtual void        ProcessSMS(SMS iNewSMS){};
		void                SetIsPeriodic(bool i_bNewPeriodic){m_bIsPeriodic = i_bNewPeriodic;};
		uint8_t             GetIsPeriodic(void){return m_bIsPeriodic;};
		uint8_t             GetTaskPriority(void) {return m_u8Priority;};
		void                SetTaskPriority(uint8_t i_u8NewPriority){m_u8Priority = i_u8NewPriority;};

	private:
	   uint8_t m_u8TaskID;
	   bool m_bIsPeriodic;
	   static uint8_t m_u8NextTaskID;
	   uint8_t m_u8Priority;
	   bool m_bToExec;
	protected:

	   bool m_bIsFinished;
};

#endif /* TASK_HPP_ */
