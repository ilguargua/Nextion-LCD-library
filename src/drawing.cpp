/* drawing.cpp
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
 * This file include the following class methods:
 * 
 * Public:
 * - writeStr()
 * - drawArea()
 * - drawLine()
 * - drawCircle()
 * - addWavePoint()
 * - addWaveBytes()
 * - waveUpdtEn()
 *  
*/



#include <Arduino.h>
#include "nxt_lcd.h"


/*
 * writeStr() - write string to display.
 * "msg" is the string to be displayed (can reside in program memory using the F() macro), "x" and "y" are
 * coordinate of upper left corner, "w" and "h" the width and heigth of msg box, "font" is the ID of one
 * of the (preloaded) font, "fCol" and "Bcol" are foreground and background color, "Xcen" is the horizontal
 * centering (0-left,1-center,2-right), "Ycen" is vertical centering (0-up,1-center,2-down). You can omit the
 * last 5 parameters, see nxt_lcd.h for defaults. strlen(msg) + 42 <= NXT_BUF_SIZE, or an error is returned.
 */
uint8_t NxtLcd::writeStr(const char* msg, uint16_t x,uint16_t y,uint16_t w, 
                             uint16_t h,uint8_t font,uint16_t fCol,
                             uint16_t bCol, uint8_t xCen, uint8_t yCen)
{
    if(initialized == 0) return notInit;
    if(strlen(msg) + 42 > NXT_BUF_SIZE) return dataTooBig;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("xstr %u,%u,%u,%u,%u,%u,%u,%u,%u,1,\"%s\"%c%c%c"),x,y,w,h,
             font,fCol,bCol,xCen,yCen,msg,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"xstr %u,%u,%u,%u,%u,%u,%u,%u,%u,1,\"%s\"%c%c%c",x,y,w,h,
             font,fCol,bCol,xCen,yCen,msg,NXT_MSG_END);
#endif    
    //serialLogInt("xstr sendBuf len",strlen((char *)sendBuf));
    return writeBuf();
}   


/*
 * drawArea() - draw a rectangle to display.
 * "x" and "y" are coordinate of upper left corner, "w" and "h" the width and heigth of the box. "filled" specify
 * if the area will be filled (1) or empty (0). "color" is the color used to fill/draw area (RGB 565)
 */
uint8_t NxtLcd::drawArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h,uint16_t color, uint8_t filled){
    if(initialized == 0) return notInit;
    char    cmd[5];
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    if(filled > 0) strncpy_P(cmd,PSTR("fill"),5);
    else{
        strncpy_P(cmd,PSTR("draw"),5);
        w += x;
        h += y;
    } 
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("%s %u,%u,%u,%u,%u%c%c%c"),cmd,x,y,w,h,color,NXT_MSG_END);
#else    
    if(filled > 0) strncpy(cmd,"fill",5);
    else{
        strncpy(cmd,"draw",5);
        w += x;
        h += y;
    } 
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"%s %u,%u,%u,%u,%u%c%c%c",cmd,x,y,w,h,color,NXT_MSG_END);
#endif    
    return writeBuf();
}

/*
 * drawLine() - draw a line from coordinate "x","y" to coordinate "x1","y1", using "color" as ink
 */
uint8_t NxtLcd::drawLine(uint16_t x, uint16_t y, uint16_t x1, uint16_t y1,uint16_t color){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("line %u,%u,%u,%u,%u%c%c%c"),x,y,x1,y1,color,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"line %u,%u,%u,%u,%u%c%c%c",x,y,x1,y1,color,NXT_MSG_END);
#endif    
    return writeBuf();
}


/*
 * drawCircle() - draw a circle with center at coordinate "x","y" having radius "r" and using "color"
 * as ink. filled used as in drawArea()
 * 
 */
uint8_t NxtLcd::drawCircle(uint16_t x, uint16_t y, uint16_t r, uint16_t color, uint8_t filled){
    if(initialized == 0) return notInit;
    char    cmd[5];
    bufReset(sendBuf);

#ifdef ARDUINO_ARCH_AVR
    if(filled > 0) strncpy_P(cmd,PSTR("cirs"),5);
    else strncpy_P(cmd,PSTR("cir"),4);
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("%s %u,%u,%u,%u%c%c%c"),cmd,x,y,r,color,NXT_MSG_END);
#else    
    if(filled > 0) strncpy(cmd,"cirs",5);
    else strncpy(cmd,"cir",4);
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"%s %u,%u,%u,%u%c%c%c",cmd,x,y,r,color,NXT_MSG_END);
#endif    
    return writeBuf();
}


/*
 * addWavePoint() - add a single point to the wave object "waveId" at channel "ch". 
 * wave object can have up to 4 channels, see 'ch' property in Nextion editor (can't be changed at runtime). 
 * "value" limit is 0 to 255, or 0 to wave object heigth, if you want to see the whole trace.  
 */
uint8_t NxtLcd::addWavePoint(uint8_t waveId,uint8_t ch, uint8_t value){
    if(initialized == 0) return notInit;
    if(ch > 3) return invalidData;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("add %u,%u,%u%c%c%c"),waveId,ch,value,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"add %u,%u,%u%c%c%c",waveId,ch,value,NXT_MSG_END);
#endif    
    return writeBuf();
}


/*
 * addWaveBytes() - add "len" bytes to wave object channel at once. In this case len can exceed the
 * NXT_BUF_SIZE limit, if so the transmission of data will be splitted in chunks that fits
 * into the buffer. 
 */
uint8_t NxtLcd::addWaveBytes(uint8_t waveId,uint8_t ch, uint8_t* bytes, uint16_t len){
    if(initialized == 0) return notInit;
    if(ch > 3) return invalidData;
    uint8_t res = replyCmdFail;
    uint16_t chkSize = len;
    //uint8_t iter = (len / NXT_BUF_SIZE) + 1;//1;
    uint8_t cnt = 0;
    while(len > 0){
        bufReset(sendBuf);
        if(cnt > 0) delay(20); //after 1° run we need a little delay
        chkSize = (len > NXT_BUF_SIZE) ? NXT_BUF_SIZE : len;
#ifdef ARDUINO_ARCH_AVR
        snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("addt %u,%u,%u%c%c%c"),waveId,ch,chkSize,NXT_MSG_END);
#else    
        snprintf((char *)sendBuf,NXT_BUF_SIZE,"addt %u,%u,%u%c%c%c",waveId,ch,chkSize,NXT_MSG_END);
#endif    
        //serialLogStr("addWaveBytes sendBuf",sendBuf);
        //This seems critical for the timing
        res = writeBuf(replyTDReady,100);
        if(res != replyCmdOk){
          //serialLogInt("addWaveBytes res",res);  
          //serialLogInt("addWaveBytes iter",iter);
          //serialLogInt("addWaveBytes cnt",cnt);
          //serialLogInt("addWaveBytes len",len);
          //serialLogInt("addWaveBytes chkSize",chkSize);
          
          return res;  
        } 
        bufReset(sendBuf);
        memcpy(sendBuf,&bytes[cnt],chkSize);
        res = writeBuf(replyTDEnd,20,chkSize);
        if(res != replyCmdOk) return res;
        cnt += chkSize;
        len -= chkSize;
        //iter--;
    }
    return replyCmdOk;
}


/*
 *  clearWaveCh() - clear data of channel "ch" in "waveId"
 */
uint8_t NxtLcd::clearWaveCh(uint8_t waveId, uint8_t ch){
    if(initialized == 0) return notInit;
    if(ch > 3 && ch != 255) return invalidData;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("cle %u,%u%c%c%c"),waveId,ch,NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"add %u,%u%c%c%c",waveId,ch,NXT_MSG_END);
#endif    
    return writeBuf();    
}

/*
 *  waveUpdtEn() - enable (en=1) or disable(en=0) the screen updating of the wave objects on current
 *  page
 */
uint8_t NxtLcd::waveUpdtEn(uint8_t en){
    if(initialized == 0) return notInit;
    if(en > 1 ) return invalidData;
    char cmd[9];
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    if(en == 1) strncpy_P(cmd,PSTR("ref_star"),9);
    else strncpy_P(cmd,PSTR("ref_stop"),9);
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("%s%c%c%c"),cmd,NXT_MSG_END);
#else    
    if(en == 1) strncpy(cmd,"ref_star",9);
    else strncpy(cmd,"ref_stop",9);
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"%s%c%c%c",cmd,NXT_MSG_END);
#endif    
    return writeBuf();    
}




#ifdef ARDUINO_ARCH_AVR

uint8_t NxtLcd::writeStr(const __FlashStringHelper* msg, uint16_t x,uint16_t y,uint16_t w, 
                             uint16_t h,uint8_t font,uint16_t fCol,
                             uint16_t bCol, uint8_t xCen, uint8_t yCen)
{
    if(initialized == 0) return notInit;
    if(strlen_P((char *)msg) + 42 > NXT_BUF_SIZE) return dataTooBig;
    bufReset(sendBuf);
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("xstr %u,%u,%u,%u,%u,%u,%u,%u,%u,1,\"%S\"%c%c%c"),x,y,w,h,
             font,fCol,bCol,xCen,yCen,msg,NXT_MSG_END);
    serialLogInt("xstr pgm, msg len",strlen_P((char *)msg));
    return writeBuf();
}   


#endif  
