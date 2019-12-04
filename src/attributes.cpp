/* attributes.cpp
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
 * This file include the following methods :
 * 
 * public:
 * - setObjAttr()
 * - getObjAttr()
 * - setBackColor()
 * - setForeColor()
 * - formatNumb()
 * 
 * For convenience, all functions can be called in 4 different way:
 * - <page_name>,<object_name>      This is global way to set the property
 * - <page_id>,<object_id>          As above, but use numeric id
 * - <object_name>                  In this case object must reside on current page
 * - <object_id>                    As above, but using id
 * On AVR architeture <page_name> and <object_name> can reside in 
 * program memory, i.e. you can use the F("string") macro
 * 
 *  Common attributes are:
 *  - id(*)        object Id
 *  - type(*)      object type (es. Number = 54)
 *  - vscope(*)    scope 0=local 1=global
 *  - sta(*)       background fill 0=crop image; 1=solid color; 2=image
 *  - style(*)     box style 0=flat; 1=border; 2=3D_down; 3=3D_up
 *  - font         object font id of preloaded fonts
 *  - bco          background color, RGB565
 *  - pco          foreground color, RGB565
 *  - xcen         horizontal alignment 0=left; 1=center; 2=right
 *  - ycen         vertical alignment 0=up; 1=center; 2=down
 *  - isbr         word wrap 0=false; 1=true
 *  - spax(-)      font horizontal spacing, range 0-255
 *  - spay(-)      font vertical spacing, range 0-255
 *  - x(*)         object box top left x coordinate
 *  - y(*)         object box top left y coordinate  
 *  - w(*)         object box width
 *  - h(*)         object box height
 *  
 *  Numeric objects attributes:
 *  - lenth        number of digits 0=auto; n=0 prefix if shorter
 *  - format       number format 0=decimal; 1=currency; 2=hex
 * 
 *  Text objects attributes:
 *  - pw           text type 0=character; 1=password
 *  - txt_maxl(*)  text max length 
 * 
 *  Xfloat objects attributes:
 *  - vvs0         digit of integral part
 *  - vvs1         digit of fractional part 
 * 
 *  Wave objects attributes:
 *  - gdc          grid color
 *  - gdw          grid horizontal spacing
 *  - gdh          grid vertical spacing
 *  - pcoN         trace color, with N in range 0-3 for channel number
 *  - dis          data scaling, range 10-1000
 * 
 *  Button objects attributes:
 *  - bco2         background color when pressed
 *  - pco2         text color when pressed
 * 
 *  Slider objects attributes:
 *  - wid          cursor width, range fro 0: no cursor to 255:auto
 *  - hig          cursor height, range as above
 *  - maxval       Max value, 0-65535
 *  - minval       Min value, 0-65535
 * 
 *  Gauge objects attributes:
 *  - wid          Gauge thinkness, max:20
 * 
 *  Timer objects attributes:
 *  - tim          Set time in ms, range 50-65535
 *  - en           timer enable, 1:enable; 0:disable
 * 
 * 
 *  (*)read-only attribute  (-)read-only on some objects 
 * 
*/


#include <Arduino.h>
#include "nxt_lcd.h"


/*
 * getObjAttr() - return an object numeric attribute. 
 * 
 * objects are addressed as pageName,objName or pageId,objId or objName or objId , last 2
 * acts on current page.
 */
uint8_t NxtLcd::getObjAttr(const char* page, const char* obj, const char* attr, uint16_t* value){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("get %s.%s.%s%c%c%c"),page,obj,attr,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"get %s.%s.%s%c%c%c",page,obj,attr,NXT_MSG_END);
#endif    
    uint8_t ret = writeBuf(replyGetNum);//readBuf();
    if(ret == replyCmdOk ){
        memcpy(value,&recvBuf[1],sizeof(uint16_t));
    }
    return ret;
}

uint8_t NxtLcd::getObjAttr(uint8_t page, uint8_t obj, const char* attr, uint16_t* value){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("get p[%u].b[%u].%s%c%c%c"),page,obj,attr,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"get p[%u].b[%u].%s%c%c%c",page,obj,attr,NXT_MSG_END);
#endif    
    uint8_t ret = writeBuf(replyGetNum);//readBuf();
    if(ret == replyCmdOk ){
        memcpy(value,&recvBuf[1],sizeof(uint16_t));
    }
    return ret;
}


uint8_t NxtLcd::getObjAttr(const char* obj, const char* attr, uint16_t* value){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("get %s.%s%c%c%c"),obj,attr,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"get %s.%s%c%c%c",obj,attr,NXT_MSG_END);
#endif    
    uint8_t ret = writeBuf(replyGetNum);//readBuf();
    if(ret == replyCmdOk ){
        memcpy(value,&recvBuf[1],sizeof(uint16_t));
    }
    return ret;
}

uint8_t NxtLcd::getObjAttr(uint8_t obj, const char* attr, uint16_t* value){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("get b[%u].%s%c%c%c"),obj,attr,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"get b[%u].%s%c%c%c",obj,attr,NXT_MSG_END);
#endif    
    uint8_t ret = writeBuf(replyGetNum);//readBuf();
    if(ret == replyCmdOk ){
        memcpy(value,&recvBuf[1],sizeof(uint16_t));
    }
    return ret;
}

/*
 * setObjAttr() - set an object numeric attribute. 
 */
uint8_t NxtLcd::setObjAttr(const char* page, const char* obj, const char* attr, uint16_t value){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("%s.%s.%s=%u%c%c%c"),page,obj,attr,value,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"%s.%s.%s=%u%c%c%c",page,obj,attr,value,NXT_MSG_END);
#endif  
    return writeBuf();
}

uint8_t NxtLcd::setObjAttr(uint8_t page, uint8_t obj, const char* attr, uint16_t value){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("p[%u].b[%u].%s=%u%c%c%c"),page,obj,attr,value,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"p[%u].b[%u].%s=%u%c%c%c",page,obj,attr,value,NXT_MSG_END);
#endif    
    return writeBuf();
}


uint8_t NxtLcd::setObjAttr(const char* obj, const char* attr, uint16_t value){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("%s.%s=%u%c%c%c"),obj,attr,value,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"%s.%s=%u%c%c%c",obj,attr,value,NXT_MSG_END);
#endif    
    return writeBuf();
}

uint8_t NxtLcd::setObjAttr(uint8_t obj, const char* attr, uint16_t value){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("b[%u].%s=%u%c%c%c"),obj,attr,value,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"b[%u].%s=%u%c%c%c",obj,attr,value,NXT_MSG_END);
#endif    
    return writeBuf();
}


/*
* setBackColor() - set object background color, a shortcuts for setObjAttr()
*/
uint8_t NxtLcd::setBackColor(const char* page, const char* obj, uint16_t value){
    if(initialized == 0) return notInit;
    return setObjAttr(page,obj,"bco",value);
}

uint8_t NxtLcd::setBackColor(uint8_t page, uint8_t obj, uint16_t value){
    if(initialized == 0) return notInit;
    return setObjAttr(page,obj,"bco",value);
}

uint8_t NxtLcd::setBackColor(const char* obj, uint16_t value){
    if(initialized == 0) return notInit;
    return setObjAttr(obj,"bco",value);
}

uint8_t NxtLcd::setBackColor(uint8_t obj, uint16_t value){
    if(initialized == 0) return notInit;
    return setObjAttr(obj,"bco",value);
}


/*
 * setForeColor() - set object foreground color, a shortcuts for setObjAttr(obj,"pco",value)
 */
uint8_t NxtLcd::setForeColor(const char* page, const char* obj, uint16_t value){
    if(initialized == 0) return notInit;
    return setObjAttr(page,obj,"pco",value);
}

uint8_t NxtLcd::setForeColor(uint8_t page, uint8_t obj, uint16_t value){
    if(initialized == 0) return notInit;
    return setObjAttr(page,obj,"pco",value);
}

uint8_t NxtLcd::setForeColor(const char* obj, uint16_t value){
    if(initialized == 0) return notInit;
    return setObjAttr(obj,"pco",value);
}

uint8_t NxtLcd::setForeColor(uint8_t obj, uint16_t value){
    if(initialized == 0) return notInit;
    return setObjAttr(obj,"pco",value);
}


/*
 * formatNumb() - format numeric objects, setting the 'lenth' and 'format' attributes (see above)
 */
uint8_t NxtLcd::formatNumb(const char* page, const char* obj, uint8_t len, uint8_t format){
    if(initialized == 0) return notInit;
    if(len > 15 || format > 2) return invalidData;
    uint8_t res = setObjAttr(page,obj,"lenth",len);
    if(res != replyCmdOk) return res;
    else return setObjAttr(page,obj,"format",format);
}
    
uint8_t NxtLcd::formatNumb(uint8_t page, uint8_t obj, uint8_t len, uint8_t format){
    if(initialized == 0) return notInit;
    if(len > 15 || format > 2) return invalidData;
    uint8_t res = setObjAttr(page,obj,"lenth",len);
    if(res != replyCmdOk) return res;
    else return setObjAttr(page,obj,"format",format);
}

uint8_t NxtLcd::formatNumb(const char* obj, uint16_t len, uint8_t format){
    if(initialized == 0) return notInit;
    if(len > 15 || format > 2) return invalidData;
    uint8_t res = setObjAttr(obj,"lenth",len);
    if(res != replyCmdOk) return res;
    else return setObjAttr(obj,"format",format);
}

uint8_t NxtLcd::formatNumb(uint8_t obj, uint16_t len, uint8_t format){
    if(initialized == 0) return notInit;
    if(len > 15 || format > 2) return invalidData;
    uint8_t res = setObjAttr(obj,"lenth",len);
    if(res != replyCmdOk) return res;
    else return setObjAttr(obj,"format",format);
}

#ifdef ARDUINO_ARCH_AVR

uint8_t NxtLcd::getObjAttr(const __FlashStringHelper* page, const __FlashStringHelper* obj, 
                               const __FlashStringHelper* attr, uint16_t* value){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("get %S.%S.%S%c%c%c"),page,obj,attr,NXT_MSG_END);
    uint8_t ret = writeBuf(replyGetNum);//readBuf();
    if(ret == replyCmdOk ){
        memcpy(value,&recvBuf[1],sizeof(uint16_t));
    }
    return ret;
}

uint8_t NxtLcd::getObjAttr(uint8_t page, uint8_t obj, const __FlashStringHelper* attr, uint16_t* value){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("get p[%u].b[%u].%S%c%c%c"),page,obj,attr,NXT_MSG_END);
    uint8_t ret = writeBuf(replyGetNum);//readBuf();
    if(ret == replyCmdOk ){
        memcpy(value,&recvBuf[1],sizeof(uint16_t));
    }
    return ret;
}


uint8_t NxtLcd::getObjAttr(const __FlashStringHelper* obj, const __FlashStringHelper* attr, uint16_t* value){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("get %S.%S%c%c%c"),obj,attr,NXT_MSG_END);
    uint8_t ret = writeBuf(replyGetNum);//readBuf();
    if(ret == replyCmdOk ){
        memcpy(value,&recvBuf[1],sizeof(uint16_t));
    }
    return ret;
}

uint8_t NxtLcd::getObjAttr(uint8_t obj, const __FlashStringHelper* attr, uint16_t* value){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("get b[%u].%S%c%c%c"),obj,attr,NXT_MSG_END);
    uint8_t ret = writeBuf(replyGetNum);//readBuf();
    if(ret == replyCmdOk ){
        memcpy(value,&recvBuf[1],sizeof(uint16_t));
    }
    return ret;
}

uint8_t NxtLcd::setObjAttr(const __FlashStringHelper* page, const __FlashStringHelper* obj, 
                               const __FlashStringHelper* attr, uint16_t value){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("%S.%S.%S=%u%c%c%c"),page,obj,attr,value,NXT_MSG_END);
    return writeBuf();
}

uint8_t NxtLcd::setObjAttr(uint8_t page, uint8_t obj, const __FlashStringHelper* attr, uint16_t value){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("p[%u].b[%u].%S=%u%c%c%c"),page,obj,attr,value,NXT_MSG_END);
    return writeBuf();
}


uint8_t NxtLcd::setObjAttr(const __FlashStringHelper* obj, const __FlashStringHelper* attr, uint16_t value){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("%S.%S=%u%c%c%c"),obj,attr,value,NXT_MSG_END);
    return writeBuf();
}

uint8_t NxtLcd::setObjAttr(uint8_t obj, const __FlashStringHelper* attr, uint16_t value){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("b[%u].%S=%u%c%c%c"),obj,attr,value,NXT_MSG_END);
    return writeBuf();
}

uint8_t NxtLcd::setBackColor(const __FlashStringHelper* page, const __FlashStringHelper* obj, uint16_t value){
    if(initialized == 0) return notInit;
    return setObjAttr(page,obj,F("bco"),value);
}

uint8_t NxtLcd::setBackColor(const __FlashStringHelper* obj, uint16_t value){
    if(initialized == 0) return notInit;
    return setObjAttr(obj,F("bco"),value);
}

uint8_t NxtLcd::setForeColor(const __FlashStringHelper* page, const __FlashStringHelper* obj, uint16_t value){
    if(initialized == 0) return notInit;
    return setObjAttr(page,obj,F("pco"),value);
}

uint8_t NxtLcd::setForeColor(const __FlashStringHelper* obj, uint16_t value){
    if(initialized == 0) return notInit;
    return setObjAttr(obj,F("pco"),value);
}

uint8_t NxtLcd::formatNumb(const __FlashStringHelper* page, const __FlashStringHelper* obj, 
                               uint8_t len, uint8_t format){
    if(initialized == 0) return notInit;
    if(len > 15 || format > 2) return invalidData;
    uint8_t res = setObjAttr(page,obj,F("lenth"),len);
    if(res != replyCmdOk) return res;
    else return setObjAttr(page,obj,F("format"),format);
}
    
uint8_t NxtLcd::formatNumb(const __FlashStringHelper* obj, uint16_t len, uint8_t format){
    if(initialized == 0) return notInit;
    if(len > 15 || format > 2) return invalidData;
    uint8_t res = setObjAttr(obj,F("lenth"),len);
    if(res != replyCmdOk) return res;
    else return setObjAttr(obj,F("format"),format);
}


#endif
