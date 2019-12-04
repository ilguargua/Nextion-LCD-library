/* get_values.cpp
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
 * This file contains the following methods:
 * 
 * Public:
 *  -getString()
 *  -getNumeric()
 * 
 * For convenience, all functions can be called in 4 different way:
 * - <page_name>,<object_name>      This is global way to set the property
 * - <page_id>,<object_id>          As above, but use numeric id
 * - <object_name>                  In this case object must reside on current page
 * - <object_id>                    As above, but using id
 * On AVR architeture <page_name> and <object_name>  (and "value" parameter of setString()) can reside in 
 * program memory, i.e. you can use the F("string") macro
*/


#include <Arduino.h>
#include "nxt_lcd.h"

/* 
 * getString() - Get the 'txt' property of an object. Supported objects are same of setString().
 * objects are addressed as usual. "value" must be a pointer to char array, that must to be declared by 
 * user, size is the size of "value", with the same limit as setString() related to NXT_BUF_SIZE. 
 */
uint8_t NxtLcd::getString(const char* page,const char* field,char* value, uint16_t size){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("get %s.%s.txt%c%c%c"),page,field,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"get %s.%s.txt%c%c%c",page,field,NXT_MSG_END);
#endif    
    uint8_t ret = writeBuf(replyGetStr);
    if(ret == replyCmdOk ){
        memset(value,0,size);
        strncpy(value,(char *)&recvBuf[1],min(size,getStrLen));
    }
    return ret;
}

uint8_t NxtLcd::getString(uint8_t page,uint8_t field,char* value,uint16_t size){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("get p[%u].b[%u].txt%c%c%c"),page,field,NXT_MSG_END);
#else
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"get p[%u].b[%u].txt%c%c%c",page,field,NXT_MSG_END);
#endif
    uint8_t ret = writeBuf(replyGetStr);//readBuf();
    if(ret == replyCmdOk ){
        memset(value,0,size);
        strncpy(value,(char *)&recvBuf[1],min(size,getStrLen));
    }
    return ret;
    
}


uint8_t NxtLcd::getString(const char* field,char* value,uint16_t size){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("get %s.txt%c%c%c"),field,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"get %s.txt%c%c%c",field,NXT_MSG_END);
#endif    
    uint8_t ret = writeBuf(replyGetStr);
    if(ret == replyCmdOk ){
        memset(value,0,size);
        strncpy(value,(char *)&recvBuf[1],min(size,getStrLen));
    }
    return ret;
}

uint8_t NxtLcd::getString(uint8_t field,char* value,uint16_t size){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("get b[%u].txt%c%c%c"),field,NXT_MSG_END);
#else
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"get b[%u].txt%c%c%c",field,NXT_MSG_END);
#endif
    uint8_t ret = writeBuf(replyGetStr);//readBuf();
    if(ret == replyCmdOk ){
        memset(value,0,size);
        strncpy(value,(char *)&recvBuf[1],min(size,getStrLen));
    }
    return ret;
}


/* 
 * getNumeric() - Get the 'val' property of an object. Supported objects are same of setNumeric(),
 * objects are addressed as above. "value" can be a pointer to a int8_t, int16_t or int32_t,
 * using sizes of 1,2 or 4.
 */
uint8_t NxtLcd::getNumeric(const char* page,const char* field,void* value,uint8_t size){
    if(initialized == 0) return notInit;
    if(size > 4) return invalidData;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("get %s.%s.val%c%c%c"),page,field,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"get %s.%s.val%c%c%c",page,field,NXT_MSG_END);
#endif    
    uint8_t ret = writeBuf(replyGetNum);//readBuf();
    if(ret == replyCmdOk ){
        memcpy(value,&recvBuf[1],size);
    }
    return ret;
}


uint8_t NxtLcd::getNumeric(uint8_t page,uint8_t field,void* value,uint8_t size){
    if(initialized == 0) return notInit;
    if(size > 4) return invalidData;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("get p[%u].b[%u].val%c%c%c"),page,field,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"get p[%u].b[%u].val%c%c%c",page,field,NXT_MSG_END);
#endif    
    uint8_t ret = writeBuf(replyGetNum);//readBuf();
    if(ret == replyCmdOk ){
        memcpy(value,&recvBuf[1],size);
    }
    return ret;
}

uint8_t NxtLcd::getNumeric(const char* field,void* value,uint8_t size){
    if(initialized == 0) return notInit;
    if(size > 4) return invalidData;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("get %s.val%c%c%c"),field,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"get %s.val%c%c%c",field,NXT_MSG_END);
#endif    
    uint8_t ret = writeBuf(replyGetNum);//readBuf();
    if(ret == replyCmdOk ){
        memcpy(value,&recvBuf[1],size);
    }
    return ret;
}


uint8_t NxtLcd::getNumeric(uint8_t field,void* value,uint8_t size){
    if(initialized == 0) return notInit;
    if(size > 4) return invalidData;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("get b[%u].val%c%c%c"),field,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"get b[%u].val%c%c%c",field,NXT_MSG_END);
#endif    
    uint8_t ret = writeBuf(replyGetNum);//readBuf();
    if(ret == replyCmdOk ){
        memcpy(value,&recvBuf[1],size);
    }
    return ret;
}


#ifdef ARDUINO_ARCH_AVR

uint8_t NxtLcd::getNumeric(const __FlashStringHelper* page,const __FlashStringHelper* field,
                               void* value, uint8_t size){
    if(initialized == 0) return notInit;
    if(size > 4) return invalidData;
    bufReset(sendBuf);
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("get %S.%S.val%c%c%c"),page,field,NXT_MSG_END);
    uint8_t ret = writeBuf(replyGetNum);//readBuf();
    if(ret == replyCmdOk ){
        //memcpy(value,&getNumber,sizeof(uint32_t));
        memcpy(value,&recvBuf[1],size);
    }
    return ret;
}


uint8_t NxtLcd::getNumeric(const __FlashStringHelper* field,void* value, uint8_t size){
    if(initialized == 0) return notInit;
    if(size > 4) return invalidData;
    bufReset(sendBuf);
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("get %S.val%c%c%c"),field,NXT_MSG_END);
    //serialLogStr("getNumeric pgm sendBuf",sendBuf);
    uint8_t ret = writeBuf(replyGetNum);//readBuf();
    if(ret == replyCmdOk ){
        //memcpy(value,&getNumber,sizeof(uint32_t));
        memcpy(value,&recvBuf[1],size);
    }
    return ret;
}

uint8_t NxtLcd::getString(const __FlashStringHelper* page,const __FlashStringHelper* field,
                              char* value,uint16_t size){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("get %S.%S.txt%c%c%c"),page,field,NXT_MSG_END);
    uint8_t ret = writeBuf(replyGetStr);
    if(ret == replyCmdOk ){
        //strncpy(value,getStr,NXT_STR_SIZE);
        memset(value,0,size);
        strncpy(value,(char *)&recvBuf[1],min(size,getStrLen));
    }
    return ret;
}

uint8_t NxtLcd::getString(const __FlashStringHelper* field,char* value,uint16_t size){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("get %S.txt%c%c%c"),field,NXT_MSG_END);
    uint8_t ret = writeBuf(replyGetStr);
    if(ret == replyCmdOk ){
        //strncpy(value,getStr,NXT_STR_SIZE);
        memset(value,0,size);
        strncpy(value,(char *)&recvBuf[1],min(size,getStrLen));
    }
    return ret;
}


#endif    
