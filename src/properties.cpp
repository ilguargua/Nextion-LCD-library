/* properties.cpp
 * 
 * Arduino platform library for Itead Nextion displays
 * Instruction set : https://nextion.tech/instruction-set/
 * 
 * Library implements almost of the basic and ehnached display function
 * but none (yet) of the professional ones.
 * 
 * Please read nxt_lcd.h for some more info
 * 
 * (c) Guarguaglini Alessandro - ilguargua@gmail.com
 * 
 * This file include the following methods:
 * 
 * Public:
 * - setProperty()
 * - getProperty()
 * - setBkcmd()
 * - setNTSTimer()
 * - setNSSTimer()
 * - setTouchAW()
 * - setSerialAW()
 * - setSleep()
 * - setWakeUpPage()
 * - setDate()
 * - setTime()
 * 
 * All this functions deal with display system variables, see sysPropNames and 
 * sysPropNdx_t in nxt_lcd.h for the usable properties. Complete list can be found
 * at https://nextion.tech/instruction-set/#s6
 * 
*/


#include <Arduino.h>
#include "nxt_lcd.h"


/*
 * setDim() - set backligth intensity, range 0/100
*/
uint8_t NxtLcd::setDim(uint8_t value){
    if(initialized == 0) return notInit;
    if(value > 100) return invalidData;
    uint8_t res =setProperty(nxt_dim,value);
    if(res == replyCmdOk){
        sysProp[nxt_dim] = value;
        return res;
    }
    return replyCmdFail;
}


/*
 * setBkcmd() - set the 'bkcmd' property of display.
 *   - 0 is Off – no pass/fail will be returned.
 *   - 1 is OnSuccess, only when last serial command successful.
 *   - 2 is OnFailure, only when last serial command failed (default)
 *   - 3 is Always, returns 0x00 to 0x23 result of serial command.
 */
uint8_t NxtLcd::setBkcmd(uint8_t value){
    if(initialized == 0) return notInit;
    if(value > 3) return invalidData;
    uint8_t res = setProperty(nxt_bkcmd,value);
    if(res == replyCmdOk){
        sysProp[nxt_bkcmd] = value;
        return res;
    }
    return replyCmdFail;
}

/*
 *  setNTSTimer() - set the no touch sleep timer wtith the value val (in seconds)
 */ 
uint8_t NxtLcd::setNTSTimer(uint16_t val){
    if(initialized == 0) return notInit;
    uint8_t res = setProperty(nxt_thsp,val);
    if(res == replyCmdOk){
        sysProp[nxt_thsp] = val;
        return res;
    }
    return replyCmdFail;
}


/*
 *  setTouchAW() - set the auto awake on touch when in sleep mode. 0:disable; 1:enable
 *  When display is awaked, 3 events are sent back : 
 *  1) cmdSleepOff        0x87
 *  2) cmdTouchXYaw       0x67, with x,y coords and event=1 (pressed)
 *  3) cmdTouchXYaw       0x67, with x,y coords and event=0 (released)
 */ 
uint8_t NxtLcd::setTouchAW(uint8_t val){
    if(initialized == 0) return notInit;
    if(val > 1) return invalidData;
    uint8_t res = setProperty(nxt_thup,val);
    if(res == replyCmdOk){
        sysProp[nxt_thup] = val;
        return res;
    }
    return replyCmdFail;
}


/*
 *  setNSSTimer() - set the no serial sleep timer wtith the value val (in seconds)
 */ 
uint8_t NxtLcd::setNSSTimer(uint16_t val){
    if(initialized == 0) return notInit;
    uint8_t res = setProperty(nxt_ussp,val);
    if(res == replyCmdOk){
        sysProp[nxt_ussp] = val;
        return res;
    }
    return replyCmdFail;
}

/*
 *  setSerialAW() - set the auto awake on serial trafic when in sleep mode. 0:disable; 1:enable
 */
uint8_t NxtLcd::setSerialAW(uint8_t val){
    if(initialized == 0) return notInit;
    if(val > 1) return invalidData;
    uint8_t res = setProperty(nxt_usup,val);
    if(res == replyCmdOk){
        sysProp[nxt_usup] = val;
        return res;
    }
    return replyCmdFail;
}

/*
 *  setSleep() - set dispaly to sleep mode (val=1) or awake it (val=0)
 */
uint8_t NxtLcd::setSleep(uint8_t val){
    if(initialized == 0) return notInit;
    if(val > 1) return invalidData;
    uint8_t res = setProperty(nxt_sleep,val);
    if(res == replyCmdOk){
        sysProp[nxt_sleep] = val;
        return res;
    }
    return replyCmdFail;
}

/*
 *  setWakeUpPage() - set the page to switch to when display wake up
 */
uint8_t NxtLcd::setWakeUpPage(uint8_t page){
    if(initialized == 0) return notInit;
    uint8_t res = setProperty(nxt_wup,page);
    if(res == replyCmdOk){
        sysProp[nxt_wup] = page;
        return res;
    }
    return replyCmdFail;
}


/*
 * setDate() - set the internal RTC date.
 * "day" is the day of the month, range 0/31
 * "month" is the month of the year, range 0/12
 * "year" is 4 digit year in range of 2000/2099
 * parameter are checked as above by the function, but not for coerency, es. you could
 * set the date as 31 Feb 2012, I have not checked yet what happen in this case
 */
uint8_t NxtLcd::setDate(uint8_t day,uint8_t month,uint16_t year){
    if(initialized == 0) return notInit;
    if(dispType < nxt_enhanced) return notSupported;
    if(day < 1 || day > 31){
        return invalidData;
    }
    if(month < 1 || month > 12){
        return invalidData;
    }
    if(year < 2000 || year > 2099){
        return invalidData;
    }
    //bufReset(sendBuf);
    uint8_t res = setProperty(nxt_rtc0,year);
    if(res != replyCmdOk) return res;
    res = setProperty(nxt_rtc1,month);
    if(res != replyCmdOk) return res;
    res = setProperty(nxt_rtc2,day);
    if(res != replyCmdOk) return res;
    sysProp[nxt_rtc0] = year;
    sysProp[nxt_rtc1] = month;
    sysProp[nxt_rtc2] = day;
    return replyCmdOk;
}


/*
 * setTime() - set the internal RTC clock
 * hour range is 0/23
 * minute and second range is 0/59
 */
uint8_t NxtLcd::setTime(uint8_t hour,uint8_t minute,uint8_t second){
    if(initialized == 0) return notInit;
    if(dispType < nxt_enhanced) return notSupported;
    if(hour > 23){
        return invalidData;
    }
    if(minute > 59){
        return invalidData;
    }
    if(second > 59){
        return invalidData;
    }
    //bufReset(sendBuf);
    uint8_t res = setProperty(nxt_rtc3,hour);
    if(res != replyCmdOk) return res;
    res = setProperty(nxt_rtc4,minute);
    if(res != replyCmdOk) return res;    
    res = setProperty(nxt_rtc5,second);
    if(res != replyCmdOk) return res;    
    sysProp[nxt_rtc3] = hour;
    sysProp[nxt_rtc4] = minute;
    sysProp[nxt_rtc5] = second;
    return replyCmdOk;
}




/*
 * setProperty() - set the display system variable indicate in "prop" to "value"
 * "prop" can be a char* or an int, check sysPropNames array and sysPropNdx_t enum in nxt_lcd.h
 * Complete list of properties that can be setted can be found here :
 *  - https://nextion.tech/instruction-set/#s6
 * The above methods uses setProperty() to do the job.
 * 
 */
uint8_t NxtLcd::setProperty(const char* prop, uint16_t value){
    if(initialized == 0) return notInit;
    uint8_t propNdx = chkProperty(prop);
    if(propNdx == 255) return invalidData;
    uint8_t res;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("%s=%u%c%c%c"),prop,value,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"%s=%u%c%c%c",prop,value,NXT_MSG_END);
#endif    
    res = writeBuf();
    if(res == replyCmdOk){
        sysProp[propNdx] = value;
        return res;
    }
    return replyCmdFail;
}

uint8_t NxtLcd::setProperty(uint8_t prop, uint16_t value){
    if(initialized == 0) return notInit;
    uint8_t propNdx = chkProperty(prop);
    if(propNdx == 255) return invalidData;
    uint8_t res;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("%s=%u%c%c%c"),sysPropNames[propNdx],value,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"%s=%u%c%c%c",sysPropNames[propNdx],value,NXT_MSG_END);
#endif    
    res = writeBuf();
    if(res == replyCmdOk){
        sysProp[propNdx] = value;
        return res;
    }
    return replyCmdFail;
}




/*
 * getProperty() - get the display system variable indicate in "prop" to "value"
 * "prop" can be a char* or an int, check sysPropNames array and sysPropNdx_t enum in nxt_lcd.h
 * Complete list of properties that can be setted can be found here :
 *  - https://nextion.tech/instruction-set/#s6
 * 
 */
uint8_t NxtLcd::getProperty(const char* prop, uint16_t* value, uint8_t loc){
    if(initialized == 0) return notInit;
    uint8_t propNdx = chkProperty(prop);
    if(propNdx == 255) return invalidData;
    if(loc > 0){
        (*value) = sysProp[propNdx];
        return replyCmdOk;
    }
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("get %s%c%c%c"),prop,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"get %s%c%c%c",prop,NXT_MSG_END);
#endif    
    uint8_t ret = writeBuf(replyGetNum);//readBuf();
    if(ret == replyCmdOk ){
        memcpy(value,&recvBuf[1],2);
        sysProp[propNdx] = (*value);
    }
    return ret;
}


uint8_t NxtLcd::getProperty(uint8_t prop, uint16_t* value, uint8_t loc){
    if(initialized == 0) return notInit;
    uint8_t propNdx = chkProperty(prop);
    if(propNdx == 255) return invalidData;
    if(loc > 0){
        (*value) = sysProp[propNdx];
        return replyCmdOk;
    }
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("get %S%c%c%c"),sysPropNames[propNdx],NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"get %s%c%c%c",sysPropNames[propNdx],NXT_MSG_END);
#endif    
    uint8_t ret = writeBuf(replyGetNum);//readBuf();
    if(ret == replyCmdOk ){
        memcpy(value,&recvBuf[1],2);
        sysProp[propNdx] = (*value);
    }
    return ret;
}


