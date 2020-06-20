/* commands.cpp
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
 * - cls()
 * - refresh()
 * - click()
 * - touchEn()
 * - setVis()
 * - getPage()
 * - setPageN()
 * - setPageS()
 * 
*/


#include <Arduino.h>
#include "nxt_lcd.h"


/*
 * cls() - clear the screen and put "color" as background
 */
uint8_t NxtLcd::cls(uint16_t color){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("cls %u%c%c%c"),color,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"cls %u%c%c%c",color,NXT_MSG_END);
#endif
    return writeBuf();
}

/*
 * refresh() - refresh an object, bring it foreground. If object id is 0 all page
 * is refreshed. "obj" can be a string, int or pgm string (on AVR)
 */
uint8_t NxtLcd::refresh(const char* obj){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("ref %s%c%c%c"),obj,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"ref %s%c%c%c",obj,NXT_MSG_END);
#endif
    return writeBuf();
}

uint8_t NxtLcd::refresh(uint8_t obj){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("ref %u%c%c%c"),obj,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"ref %u%c%c%c",obj,NXT_MSG_END);
#endif
    return writeBuf();
}


/*
 *  click() - Invoke the press (ev=1) or release (ev=0) event on component "obj"
 *  Note that if you use click on an object (e.g. button), the code wrote in the editor for 
 *  such event will be executed (e.g. page change), but the event will not be notified back,
 *  even if 'send event id' is marked. "obj" can be object name or id a must be on current page.
 */
uint8_t NxtLcd::click(const char* obj, uint8_t ev){
    if(initialized == 0) return notInit;
    if(ev > 1) return invalidData;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("click %s,%u%c%c%c"),obj,ev,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"click %s,%u%c%c%c",obj,ev,NXT_MSG_END);
#endif    
    return writeBuf();
}

uint8_t NxtLcd::click(uint8_t obj, uint8_t ev){
    if(initialized == 0) return notInit;
    if(ev > 1) return invalidData;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("click %u,%u%c%c%c"),obj,ev,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"click %u,%u%c%c%c",obj,ev,NXT_MSG_END);
#endif    
    return writeBuf();
}

/*
 *  touchEn() - enable (en=1) or disable(en=0) the touch feature of an object. "obj"
 *  can be object name or id, and must be on current page. If id=0 the whole page touch
 *  abilities will be enabled or disabled.
 */
uint8_t NxtLcd::touchEn(const char* obj, uint8_t en){
    if(initialized == 0) return notInit;
    if(en > 1) return invalidData;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("tsw %s,%u%c%c%c"),obj,en,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"tsw %s,%u%c%c%c",obj,en,NXT_MSG_END);
#endif    
    return writeBuf();
}

uint8_t NxtLcd::touchEn(uint8_t obj, uint8_t en){
    if(initialized == 0) return notInit;
    if(en > 1) return invalidData;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("tsw %u,%u%c%c%c"),obj,en,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"tsw %u,%u%c%c%c",obj,en,NXT_MSG_END);
#endif    
    return writeBuf();
}

/*
 * setVis() - change visibility of an object, identified by name or id; state 0 hide object, 1 show it.
 * show() and hide() are shortcuts for setVis(obj,1) and setVis(obj,0) defined in nxt_lcd.h
 */
uint8_t NxtLcd::setVis(const char* obj,uint8_t state){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("vis %s,%u%c%c%c"),obj,state,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"vis %s,%u%c%c%c",obj,state,NXT_MSG_END);
#endif    
    return writeBuf();
}


uint8_t NxtLcd::setVis(uint8_t obj,uint8_t state){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("vis %u,%u%c%c%c"),obj,state,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"vis %u,%u%c%c%c",obj,state,NXT_MSG_END);
#endif    
    return writeBuf();
}


/*
 * getPage() - get current page ID.
 * please note, as all others public method of this class, the value is returned into parameter
 * "page", NOT by the function itself
 */
uint8_t NxtLcd::getPage(uint8_t* page){
    if(initialized == 0) return notInit;
    uint16_t pg = 0;
    uint8_t res = getProperty(nxt_dp,&pg);
    if(res == replyCmdOk){
        (*page) = pg;
    }
    return res;
}

/*
 *  setPageS() - set current page, using page name .
 *  Note: seems difficult to overload this func, because a call to lcd.setPage(0) is ambiguos
 *  (or so complains gcc...) so I ended up using 2 different names
 */
uint8_t NxtLcd::setPageS(const char* page){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("page %s%c%c%c"),page,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"page %s%c%c%c",page,NXT_MSG_END);
#endif    
    uint8_t res = writeBuf();
    if(res == replyCmdOk){
        uint16_t pg = 0;
        getProperty(nxt_dp,&pg);
    }
    return res;
}

/*
 *  setPageN() - set current page, using page id.
 *  Note: seems difficult to overload this func, because a call to lcd.setPage(0) is ambiguos
 *  (or so complains gcc...) so I ended up using 2 different names
 */
uint8_t NxtLcd::setPageN(uint8_t page){
    if(initialized == 0) return notInit;
    return setProperty(nxt_dp,page);
}




#ifdef ARDUINO_ARCH_AVR

uint8_t NxtLcd::setVis(const __FlashStringHelper* obj,uint8_t state){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("vis %S,%u%c%c%c"),obj,state,NXT_MSG_END);
    return writeBuf();
}


uint8_t NxtLcd::refresh(const __FlashStringHelper* obj){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("ref %S%c%c%c"),obj,NXT_MSG_END);
    return writeBuf();
}


uint8_t NxtLcd::click(const __FlashStringHelper* obj, uint8_t ev){
    if(initialized == 0) return notInit;
    if(ev > 1) return invalidData;
    bufReset(sendBuf);
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("click %S,%u%c%c%c"),obj,ev,NXT_MSG_END);
    return writeBuf();
}

uint8_t NxtLcd::touchEn(const __FlashStringHelper* obj, uint8_t en){
    if(initialized == 0) return notInit;
    if(en > 1) return invalidData;
    bufReset(sendBuf);
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("tsw %S,%u%c%c%c"),obj,en,NXT_MSG_END);
    return writeBuf();
}

uint8_t NxtLcd::setPageS(const __FlashStringHelper* page){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("page %S%c%c%c"),page,NXT_MSG_END);
    uint8_t res = writeBuf();
    if(res == replyCmdOk){
        uint16_t pg = 0;
        getProperty(nxt_dp,&pg);
    }
    return res;
}


#endif
