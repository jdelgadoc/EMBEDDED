/*
 * LED.hpp
 *
 *  Created on: Aug 31, 2016
 *      Author: eortiz
 */

#ifndef LED_HPP_
#define LED_HPP_
#define __NOP __nop
#include <ti/devices/msp432p4xx/inc/msp.h>
#include "Task.hpp"


class LED : public Task
{
    public:
        LED(uint16_t,uint8_t i_u8NewID);
        uint16_t m_u16BITN;
        virtual uint8_t run(void);
        virtual uint8_t setup(void);
        virtual void ProcessSMS(SMS iNewSMS);
    protected:
    private:
};

#endif /* LED_HPP_ */
