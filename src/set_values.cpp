/* set_values.cpp
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
 *  -setString()
 *  -setNumeric()
 *  -setFloat()
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
 * setString() - Set the 'txt' property of an object. Supported objects are (type):
 *               - text (116)
 *               - scrolling text (55)
 *               - button (98)
 *               - dual state button (53)
 *               - QR code (58)
 *               - variable (52), if 'sta' property is set to string
 * objects can be addressed in the following forms :
 *  - page name, object name
 *  - page id, objcet id
 *  - object name (obj. have to be in current page)
 *  - object id (obj. have to be in current page)
 * Please note that above notation can not be mixed,you can use names or id but not both in the same
 * function. Id save more memory, but especially on first attempt, are more hard to remember.
 * Names (if you set meaningful names of objects in editor) are more easy to get, but use of
 * course more memory (at least flash memory, if you use the F() macro for constant string).
 * To save memory, on AVR platform names can be constant program space strings (F("mystring")).
 * "value" parameter is a pointer to a string to be write. String length must not exceed 
 * NXT_BUF_SIZE-(strlen(page name))-strlen(obj name)-10 in case of page/obj name or
 * NXT_BUF_SIZE-16 in case of numeric ID used, otherwise an error is returned.
 * Please note that all text objects have a 'txt_maxl' property (that can not be changed at runtime)
 * that limit the string length. In case exceed, string will be truncated (by display itself).
 */
uint8_t NxtLcd::setString(const char* page,const char* field,const char* value){
    if(initialized == 0) return notInit;
    if(strlen(page)+strlen(field)+strlen(value)+11 > NXT_BUF_SIZE) return dataTooBig;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("%s.%s.txt=\"%s\"%c%c%c"),page,field,value,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"%s.%s.txt=\"%s\"%c%c%c",page,field,value,NXT_MSG_END);
#endif
    return writeBuf();
}


uint8_t NxtLcd::setString(uint8_t page,uint8_t field,const char* value){
    if(initialized == 0) return notInit;
    if(strlen(value)+21 > NXT_BUF_SIZE) return dataTooBig;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("p[%u].b[%u].txt=\"%s\"%c%c%c"),page,field,value,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"p[%u].b[%u].txt=\"%s\"%c%c%c",page,field,value,NXT_MSG_END);
#endif    
    return writeBuf();
}

uint8_t NxtLcd::setString(const char* field,const char* value){
    if(initialized == 0) return notInit;
    if(strlen(field)+strlen(value)+10 > NXT_BUF_SIZE) return dataTooBig;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("%s.txt=\"%s\"%c%c%c"),field,value,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"%s.txt=\"%s\"%c%c%c",field,value,NXT_MSG_END);
#endif
    return writeBuf();
}

uint8_t NxtLcd::setString(uint8_t field,const char* value){
    if(initialized == 0) return notInit;
    if(strlen(value)+15 > NXT_BUF_SIZE) return dataTooBig;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("b[%u].txt=\"%s\"%c%c%c"),field,value,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"b[%u].txt=\"%s\"%c%c%c",field,value,NXT_MSG_END);
#endif    
    return writeBuf();    
}


/*
 * setNumeric() - Set the 'val' property of an object. Supported objects are (type): 
 *                - Number (54) ; 32bit signed value
 *                - Progress bar (106) ; value limits 0/100
 *                - Gauge (122) ; value limits 0/360
 *                - Slider (1); value limits 'minval'/'maxval' properties
 *                - variable (52), if 'sta' property is set to number ; limits as Number obj.
 *                - dual state button (53) ; limits 0/1 for unpressed/pressed
 *                - Checkbox (56) ; limits 0/1 for unchecked/checked
 *                - Radio (57) ; limits 0/1 for unchecked/checked
 * For xfloat object, use the setFloat() method.
 * objects can be addressed as above.
 * Functions do not check the value limits
 */
uint8_t NxtLcd::setNumeric(const char* page,const char* field,long value){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("%s.%s.val=%lu%c%c%c"),page,field,value,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"%s.%s.val=%lu%c%c%c",page,field,value,NXT_MSG_END);
#endif    
    return writeBuf();
}


uint8_t NxtLcd::setNumeric(uint8_t page,uint8_t field,long value){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("p[%u].b[%u].val=%lu%c%c%c"),page,field,value,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"p[%u].b[%u].val=%lu%c%c%c",page,field,value,NXT_MSG_END);
#endif    
    return writeBuf();
}

uint8_t NxtLcd::setNumeric(const char* field,long value){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("%s.val=%lu%c%c%c"),field,value,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"%s.val=%lu%c%c%c",field,value,NXT_MSG_END);
#endif    
    return writeBuf();
}


uint8_t NxtLcd::setNumeric(uint8_t field,long value){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("b[%u].val=%lu%c%c%c"),field,value,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"b[%u].val=%lu%c%c%c",field,value,NXT_MSG_END);
#endif    
    return writeBuf();
}



/*
 * setFloat() - Set the 'val' property of an Xfloat object.
 * Despite of the name, float on nextion display doesn't exist. The value passed to the 
 * functions and by them writed on 'val' object property is a signed 32bit integer. The
 * 'vvs0' and 'vvs1' properties of the object fix the length of integral and fractional part,
 * these are the "intSize" and "frctSize" parameter of the functions. So, the value 123456 will
 * be show on display as '123.456' in case 'vvs0'=3 and 'vvs1'=3, or '012.3456' in case of
 * 'vvs0'=3 and 'vvs1'=4, and so on. Last 2 parameter can be omitted, or setted to 0 for leave 'vvs0'
 * and 'vvs1' inalterated. 
 * objects can be addressed as above.
 */
uint8_t NxtLcd::setFloat(const char* page, const char* field,
                             long value, uint8_t intSize, uint8_t frctSize){
    if(initialized == 0) return notInit;
    uint8_t res = replyCmdFail;
    if(intSize > 0){
        bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
        snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("%s.%s.vvs0=%u%c%c%c"),page,field,intSize,NXT_MSG_END);
#else    
        snprintf((char *)sendBuf,NXT_BUF_SIZE,"%s.%s.vvs0=%u%c%c%c",page,field,intSize,NXT_MSG_END);
#endif    
        res = writeBuf();
        if(res != replyCmdOk) return res;
    }
    if(frctSize > 0){
        bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
        snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("%s.%s.vvs1=%u%c%c%c"),page,field,frctSize,NXT_MSG_END);
#else    
        snprintf((char *)sendBuf,NXT_BUF_SIZE,"%s.%s.vvs1=%u%c%c%c",page,field,frctSize,NXT_MSG_END);
#endif    
        res = writeBuf();
        if(res != replyCmdOk) return res;//replyCmdFail;
    }
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("%s.%s.val=%ld%c%c%c"),page,field,
             value,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"%s.%s.val=%ld%c%c%c",page,field,
             value,NXT_MSG_END);
#endif    
    res = writeBuf();
    if(res != replyCmdOk) return res;//replyCmdFail;    
    return replyCmdOk;
}


uint8_t NxtLcd::setFloat(uint8_t page, uint8_t field,
                             long value, uint8_t intSize, uint8_t frctSize){
    if(initialized == 0) return notInit;
    uint8_t res = replyCmdFail;
    if(intSize > 0){
        bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
        snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("p[%u].b[%u].vvs0=%u%c%c%c"),page,field,intSize,NXT_MSG_END);
#else    
        snprintf((char *)sendBuf,NXT_BUF_SIZE,"p[%u].b[%u].vvs0=%u%c%c%c",page,field,intSize,NXT_MSG_END);
#endif    
        res = writeBuf();
        if(res != replyCmdOk) return res;//replyCmdFail;
    }
    if(frctSize > 0){
        bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
        snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("p[%u].b[%u].vvs1=%u%c%c%c"),page,field,frctSize,NXT_MSG_END);
#else    
        snprintf((char *)sendBuf,NXT_BUF_SIZE,"p[%u].b[%u].vvs1=%u%c%c%c",page,field,frctSize,NXT_MSG_END);
#endif    
        res = writeBuf();
        if(res != replyCmdOk) return res;//replyCmdFail;
    }
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("p[%u].b[%u].val=%ld%c%c%c"),page,field,
             value,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"p[%u].b[%u].val=%ld%c%c%c",page,field,
             value,NXT_MSG_END);
#endif    
    res = writeBuf();
    if(res != replyCmdOk) return res;//replyCmdFail;    
    return replyCmdOk;
}

uint8_t NxtLcd::setFloat(const char* field,
                             long value, uint8_t intSize, uint8_t frctSize){
    if(initialized == 0) return notInit;
    uint8_t res = replyCmdFail;
    if(intSize > 0){
        bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
        snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("%s.vvs0=%u%c%c%c"),field,intSize,NXT_MSG_END);
#else    
        snprintf((char *)sendBuf,NXT_BUF_SIZE,"%s.vvs0=%u%c%c%c",field,intSize,NXT_MSG_END);
#endif    
        res = writeBuf();
        if(res != replyCmdOk) return res;//replyCmdFail;
    }
    if(frctSize > 0){
        bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
        snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("%s.vvs1=%u%c%c%c"),field,frctSize,NXT_MSG_END);
#else    
        snprintf((char *)sendBuf,NXT_BUF_SIZE,"%s.vvs1=%u%c%c%c",field,frctSize,NXT_MSG_END);
#endif    
        res = writeBuf();
        if(res != replyCmdOk) return res;//replyCmdFail;
    }
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("%s.val=%ld%c%c%c"),field,
             value,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"%s.val=%ld%c%c%c",field,
             value,NXT_MSG_END);
#endif    
    res = writeBuf();
    if(res != replyCmdOk) return res;//replyCmdFail;    
    return replyCmdOk;
}


uint8_t NxtLcd::setFloat(uint8_t field,
                             long value, uint8_t intSize, uint8_t frctSize){
    if(initialized == 0) return notInit;
    uint8_t res = replyCmdFail;
    if(intSize > 0){
        bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
        snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("b[%u].vvs0=%u%c%c%c"),field,intSize,NXT_MSG_END);
#else    
        snprintf((char *)sendBuf,NXT_BUF_SIZE,"b[%u].vvs0=%u%c%c%c",field,intSize,NXT_MSG_END);
#endif    
        res = writeBuf();
        if(res != replyCmdOk) return res;//replyCmdFail;
    }
    if(frctSize > 0){
        bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
        snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("b[%u].vvs1=%u%c%c%c"),field,frctSize,NXT_MSG_END);
#else    
        snprintf((char *)sendBuf,NXT_BUF_SIZE,"b[%u].vvs1=%u%c%c%c",field,frctSize,NXT_MSG_END);
#endif    
        res = writeBuf();
        if(res != replyCmdOk) return res;//replyCmdFail;
    }
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("b[%u].val=%ld%c%c%c"),field,
             value,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"b[%u].val=%ld%c%c%c",field,
             value,NXT_MSG_END);
#endif    
    res = writeBuf();
    if(res != replyCmdOk) return res;//replyCmdFail;    
    return replyCmdOk;
}


#ifdef ARDUINO_ARCH_AVR

uint8_t NxtLcd::setString(const __FlashStringHelper* page,
                         const __FlashStringHelper* field,const __FlashStringHelper* value)
{
    if(initialized == 0) return notInit;
    if(strlen_P((char *)page)+strlen_P((char *)field)+strlen_P((char *)value)+11 > NXT_BUF_SIZE) return dataTooBig;
    bufReset(sendBuf);
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("%S.%S.txt=\"%S\"%c%c%c"),page,field,value,NXT_MSG_END);
    return writeBuf();
}

uint8_t NxtLcd::setString(uint8_t page,uint8_t field,const __FlashStringHelper* value){
    if(initialized == 0) return notInit;
    if(strlen_P((char *)value)+21 > NXT_BUF_SIZE) return dataTooBig;
    bufReset(sendBuf);
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("p[%u].b[%u].txt=\"%S\"%c%c%c"),page,field,value,NXT_MSG_END);
    return writeBuf();
}

uint8_t NxtLcd::setString(const __FlashStringHelper* field,const __FlashStringHelper* value)
{
    if(initialized == 0) return notInit;
    if(strlen_P((char *)field)+strlen_P((char *)value)+10 > NXT_BUF_SIZE) return dataTooBig;
    bufReset(sendBuf);
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("%S.txt=\"%S\"%c%c%c"),field,value,NXT_MSG_END);
    return writeBuf();
}

uint8_t NxtLcd::setString(uint8_t field,const __FlashStringHelper* value){
    if(initialized == 0) return notInit;
    if(strlen_P((char *)value)+15 > NXT_BUF_SIZE) return dataTooBig;
    bufReset(sendBuf);
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("b[%u].txt=\"%S\"%c%c%c"),field,value,NXT_MSG_END);
    return writeBuf();
}


uint8_t NxtLcd::setNumeric(const __FlashStringHelper* page,
                         const __FlashStringHelper* field,long value)
{
    bufReset(sendBuf);
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("%S.%S.val=%lu%c%c%c"),page,field,value,NXT_MSG_END);
    return writeBuf();    
}

uint8_t NxtLcd::setNumeric(const __FlashStringHelper* field,long value)
{
    bufReset(sendBuf);
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("%S.val=%lu%c%c%c"),field,value,NXT_MSG_END);
    return writeBuf();    
}

uint8_t NxtLcd::setFloat(const __FlashStringHelper* page, const __FlashStringHelper* field,
                             long value, uint8_t intSize, uint8_t frctSize){
    if(initialized == 0) return notInit;
    uint8_t res = replyCmdFail;
    if(intSize > 0){
        bufReset(sendBuf);
        snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("%S.%S.vvs0=%u%c%c%c"),page,field,intSize,NXT_MSG_END);
        res = writeBuf();
        if(res != replyCmdOk) return res;//replyCmdFail;
    }
    if(frctSize > 0){
        bufReset(sendBuf);
        snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("%S.%S.vvs1=%u%c%c%c"),page,field,frctSize,NXT_MSG_END);
        res = writeBuf();
        if(res != replyCmdOk) return res;//replyCmdFail;
    }
    bufReset(sendBuf);
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("%S.%S.val=%ld%c%c%c"),page,field,
             value,NXT_MSG_END);
    res = writeBuf();
    if(res != replyCmdOk) return res;//replyCmdFail;    
    return replyCmdOk;
}


uint8_t NxtLcd::setFloat(const __FlashStringHelper* field,
                             long value, uint8_t intSize, uint8_t frctSize){
    if(initialized == 0) return notInit;
    uint8_t res = replyCmdFail;
    if(intSize > 0){
        bufReset(sendBuf);
        snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("%S.vvs0=%u%c%c%c"),field,intSize,NXT_MSG_END);
        res = writeBuf();
        if(res != replyCmdOk) return res;//replyCmdFail;
    }
    if(frctSize > 0){
        bufReset(sendBuf);
        snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("%S.vvs1=%u%c%c%c"),field,frctSize,NXT_MSG_END);
        res = writeBuf();
        if(res != replyCmdOk) return res;//replyCmdFail;
    }
    bufReset(sendBuf);
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("%S.val=%ld%c%c%c"),field,
             value,NXT_MSG_END);
    res = writeBuf();
    if(res != replyCmdOk) return res;//replyCmdFail;    
    return replyCmdOk;
}




#endif    
