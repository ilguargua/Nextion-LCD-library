/* basic_func.cpp
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
 * public :
 * - NxtLcd()
 * - init()
 * - devReset()
 * - ckEvents()
 * 
 * private:
 * - getPropCnt()
 * - chkProperty()
 * - writeBuf()
 * - readBuf()
 * - readEvent()
 * 
*/


#include <Arduino.h>
#include "nxt_lcd.h"


/*********************************************************************************************************
 * class constructor; class can be initialized with a pointer to either a hardware or software serial, es.
 * 
 * using hardware serial:
 * NxtLcd lcd(&Serial);
 * or
 * NxtLcd lcd(&Serial1);
 * 
 * using software serial:
 * #include <SoftwareSerial.h>
 * SoftwareSerial port(4,5);
 * Nextion lcd(&port);
 * 
 * for both:
 * uint8_t res = lcd.init(); //init class with 9600 baudrate, dev. reset and debug on
 * ********************************************************************************************************
 */
NxtLcd::NxtLcd(HardwareSerial *port){
    serial.init(port);
    initialized = 0;
    haveEvent = 0;
}


NxtLcd::NxtLcd(SoftwareSerial *port){
    serial.init(port);
    initialized = 0;
    haveEvent = 0;
}

/*****************************************************************************************************
 * init() - initialize serial port with baudrate indicated, doing an optional reset and setting
 * debug (dbg=1) or not (dbg=0). Almost of the commands, if succesful, does not return anything
 * if debug=0 class does not even check for response, saving a little (NXT_REPLY_WAIT ms) time
 * on each call. When your program does not have any more bug (on mine this almost never!) you can 
 * leave it at 0. See also setBkcmd().
 * "dspType" is display type, range 0-2, 0:basic; 1:ehnached; 2:professional
 * On nextion display baudrate can be changed setting bauds=<baudrate> in first page’s 
 * Preinitialization Event of HMI. 
 * ****************************************************************************************************
 */
uint8_t NxtLcd::init(uint32_t bauds, uint8_t dspType, uint8_t reset, uint8_t dbg){
    serial.begin(bauds);
    initialized = 1;
    dispType = dspType;
    debug = dbg;
    uint8_t res;
    uint8_t propCnt = getPropCnt();
    if(reset) res = devReset();
    if(res != replyCmdOk) return res;
    //char buf[7];
    for(uint8_t i = 0; i < propCnt; i++){
        res = getProperty(i,&sysProp[i]);
        if(res != replyCmdOk) return res;
    }
    return replyCmdOk;
}


/************************************************************************************
 *  getPropCnt() - return the number of valid property for display, different
 *  from basic, to ehnached or professional
 * **********************************************************************************
 */
uint8_t NxtLcd::getPropCnt(){
    uint8_t propCnt = sysPropLen;
    switch(dispType){
        case nxt_basic:
            propCnt = nxtBasicEnd + 1;
            break;
        case nxt_enhanced:
            propCnt = nxtEnhancedEnd + 1;
            break;
    }
    return propCnt;    
}


/****************************************************************************************
 * chkProperty() - check if property can be used. property can be string or integer,
 * check sysPropNames array and sysPropNdx_t enum in nxt_lcd.h
 * 
 */
uint8_t NxtLcd::chkProperty(const char* prop){
    uint8_t propLen = getPropCnt();
    uint8_t propNdx = 255;
    for(uint8_t i = 0; i < propLen; i++){
#ifdef ARDUINO_ARCH_AVR
        if(strncmp_P(prop,sysPropNames[i],NXT_PROP_SIZE) == 0)
#else        
        if(strncmp(prop,sysPropNames[i],NXT_PROP_SIZE) == 0)
#endif
        {
            propNdx = i;
            break;
        }
    }
    return propNdx;
}
    
uint8_t NxtLcd::chkProperty(uint8_t prop){
    if(prop < getPropCnt()) return prop;
    else return 255;
}



/*****************************************************************************************
 * devReset() - reset the display, can be optionally done in init()
 */
uint8_t NxtLcd::devReset(void){
    if(initialized == 0) return notInit;
    bufReset(sendBuf);
#ifdef ARDUINO_ARCH_AVR
    snprintf_P((char *)sendBuf,NXT_BUF_SIZE,PSTR("rest%c%c%c"),NXT_MSG_END);
#else    
    snprintf((char *)sendBuf,NXT_BUF_SIZE,"rest%c%c%c",NXT_MSG_END);
#endif
    uint8_t res = writeBuf(replyStartUp,500);//readBuf();
    if(res == replyCmdOk) return (readBuf()==replyDevReady ? replyCmdOk : replyCmdFail);
    else return replyCmdFail;
}


/******************************************************************************************
 *  writeBuf() - write the prev. setted buffer to lcd device, reading the ev. answer
 *  "wait" are ms to wait for an answer
 *  "size" can be specified in some cases (using transparent mode)
 */
uint8_t NxtLcd::writeBuf(uint8_t expReply, uint16_t wait,uint16_t size){
    if(initialized == 0) return notInit;
    uint8_t ret = replyCmdFail;
    readEvent();
    if( size == 0){
        if(strlen((char *)sendBuf) > 0){
            serial.write((char *)sendBuf,strlen((char *)sendBuf));
            if(expReply > 0 || debug > 0){
                delay(wait); 
            }
            else return replyCmdOk;
        }
        else return invalidData;
    }
    else{
        size_t sent = serial.write((char *)sendBuf,size);
        if(sent != size) return replyCmdFail;
        if(expReply > 0 || debug > 0){
            delay(wait); 
        }
        else return replyCmdOk;
    }
    uint8_t res = readBuf();
    if(res == replyTouchEv || res == replySleepEv || res == replySendMe){      
        readEvent(recvBuf);
        res = readBuf();
    }
    if(expReply > 0){
        if(res == expReply) ret = replyCmdOk;
        else ret = res;          
    }
    else{
        if(res == noReply || res == replyCmdOk) ret = replyCmdOk;
        else ret = res;
    }
    return ret;    
}


/***************************************************************************************
 *  readBuf() - read a telegram from lcd device and store in buffer
 *  "ckevt" if set to 1 we are checking for an event, with limited buffer
 */
uint8_t NxtLcd::readBuf(uint8_t ckevt){
    if(initialized == 0) return notInit;
    static uint16_t cntStatic = 0;
    uint16_t cnt = 0;
    static uint8_t expLen = 3;
    //uint32_t cnt = 0;
    uint8_t         ret = noReply;
    //const char cmdEnd[] = {NXT_MSG_END,'\0'};
    if(ckevt > 0){
        cnt = cntStatic;
        //memset(recvBuf,0,NXT_EV_BUF_SIZE);
    } 
    else {
        bufReset(recvBuf);
        cntStatic = 0;
    } 
    
    while(serial.available()){
        recvBuf[cnt] = serial.read();
        if(cnt == 0){
            switch(recvBuf[0]){
                case cmdTouchCompEv:  //0x65
                    expLen = 6;
                    break;
                case cmdTouchXYaw:    //0x67  
                case cmdTouchXYsl:    //0x68
                    expLen = 8;
                    break;
                case cmdGetStr:       //0x70
                case cmdSendme:       //0x66
                    expLen = 4;
                    break;
                case cmdGetNum:       //0x71
                    expLen = 7;
                    break;
                default:
                    expLen = 3;
            }
        }
        ret = noComplete;
        if(cnt >=expLen && recvBuf[cnt] == 0xFF){ // we can already have a reply
            if(recvBuf[cnt-1] == 0xFF && recvBuf[cnt-2] == 0xFF){ //now we have valid reply
                ret = replyUnknown;
                switch(recvBuf[0]){
                    case cmdFail: //0x00 cmd failure
                        // but 3 0x00 means device startup
                        if(recvBuf[1] == 0x00 && recvBuf[2] == 0x00 && cnt == 5){
                            ret = replyStartUp; 
                            //serialLogStr("replyStartUp hit!!");
                        }
                        if(cnt == 3){
                            ret = replyCmdFail;
                        }
                        break;
                    case cmdOk: //0x01
                        if(cnt == 3){
                            ret = replyCmdOk;
                        }
                        break;
                    case cmdDevReady: //0x88
                        if(cnt == 3){
                            ret = replyDevReady;
                        }
                        break;
                    case cmdInvalidCid:   //0x02
                    case cmdInvalidPage:  //0x03
                    case cmdInvPicId:     //0x04
                    case cmdInvFontId:    //0x05
                        if(cnt == 3){
                            wrongIdCode = recvBuf[0];
                            ret = replyWrongId;
                        }
                        break;
                    case cmdInvVar:  //0x1A
                        if(cnt == 3){
                            ret = replyWrongVar;
                        }
                        break;
                    case cmdBufOvfl:  //0x24
                        if(cnt == 3){
                            delay(NXT_REPLY_WAIT * 2); // wait a little before sending other cmds
                            ret = replyBufOvfl;
                        }
                        break;
                    case cmdTouchCompEv: //0x65
                        //serialLogStr("case cmdTouchCompEv , recvBuf",recvBuf);
                        //serialLogInt("case cmdTouchCompEv , cnt",cnt);
                        if(cnt == 6){
                            lastTouchCode = cmdTouchCompEv;
                            ret = replyTouchEv;
                        }
                        break;
                    case cmdTouchXYaw: //0x67
                        if(cnt == 8){
                            lastTouchCode = cmdTouchXYaw;
                            ret = replyTouchEv;
                        }
                        break;
                    case cmdTouchXYsl: //0x68   
                        if(cnt == 8){
                            lastTouchCode = cmdTouchXYsl;
                            ret = replyTouchEv;
                        }
                        break;
                    case cmdSleepOn:    //0x86
                    case cmdSleepOff:   //0x87
                        //serialLogStr("cmdSleep trig");
                        if(cnt == 3){
                            lastTouchCode = recvBuf[0];
                            ret = replySleepEv;
                        }
                        else lastTouchCode = 11;
                        break;
                    case cmdGetStr: //0x70
                        //serialLogStr("cmdGetStr trig");
                        {
                        getStrLen = cnt - 3;   
                        ret = replyGetStr;
                        } 
                        break;
                    case cmdGetNum: //0x71
                       if(cnt == 7){
                        ret = replyGetNum;
                       } 
                        break;
                    case cmdSendme: //0x66
                        if(cnt == 4){
                            //currPage = recvBuf[1];
                            ret = replySendMe;
                        }
                        break; 
                    case cmdTDEnd: //0xFD
                        if(cnt == 3){
                            ret = replyTDEnd;
                        }
                        break;
                    case cmdTDReady: //0xFE
                        if(cnt == 3){
                            ret = replyTDReady;
                        }
                        break;
                        
                } //switch(recvBuf[0])
                //bufReset(recvBuf);
                //uint8_t myCnt = cnt;
                cnt = 0;
                cntStatic =0;
                expLen = 3;
                //break;
                if(ret == replyUnknown ){
                    wrongIdCode = recvBuf[0];
                    //serialLogHex("readBuf replyUnknown start code",wrongIdCode);
                    //serialLogInt("readBuf replyUnknown cnt",myCnt);
                    //serialLogStr("readBuf replyUnknown recvBuf",(char *)recvBuf);
                    
                } 
                return ret;
            } //if NXT_MSG_END found
        } // if possible answer
        cnt++;
        if(ckevt > 0){
            if(cnt == NXT_EV_BUF_SIZE){
                ret = bufOvfl;
                cnt = 0;
                cntStatic = 0;
                //bufReset(recvBuf);
                memset(recvBuf,0,NXT_EV_BUF_SIZE);
                break;
            }
        }
        else{
            if(cnt == NXT_BUF_SIZE){
                ret = bufOvfl;
                cnt = 0;
                cntStatic = 0;
                bufReset(recvBuf);
                break;
            }
        }
        cntStatic = cnt;
        //if(ret == noComplete ) wrongIdCode = recvBuf[0];
    } //while serial.available
    return ret;
}

/******************************************************************************
 *  readEvent() - read an event, and copy it to lastEvent struct.
 *  "ebuf" - if present, is the buffer to read from, and we already know that
 *  an event is present; if NULL event presence will be checked.
 *  If an event is present, the haveEvent flag is raised
 */
uint8_t NxtLcd::readEvent(uint8_t* ebuf){
    if(initialized == 0) return notInit;
    //static uint8_t evBuf[NXT_EV_BUF_SIZE];
    uint8_t res = noComplete;//readBuf();
    uint8_t* buf; 
    if(ebuf == NULL){
        recvBuf = evtBuf;
        res = readBuf(1);
        buf = evtBuf;
    }
    else buf = ebuf;
    //if(res != noReply) serialLogHex("ckEvents evBuf",(*evBuf));
    if(ebuf != NULL || res == replyTouchEv || res == replySleepEv || res == replySendMe){
        memset(&lastEvent,0,sizeof(nxtEvent_t));
        lastEvent.evCode = buf[0];
        //switch(lastTouchCode){
        switch(buf[0]){
            case cmdTouchCompEv:
                lastEvent.page_X = buf[1];
                lastEvent.compId_Y = buf[2];
                lastEvent.event = buf[3];
                break;
            case cmdTouchXYaw:
            case cmdTouchXYsl:
                lastEvent.page_X = buf[2] | (buf[1] << 8);
                lastEvent.compId_Y = buf[4] | (buf[3] << 8);
                lastEvent.event = buf[5];
                break;
            case cmdSleepOn:
            case cmdSleepOff:
                break;
            case cmdSendme:
                lastEvent.page_X = buf[1];
                break;
        }
        memset(evtBuf,0,NXT_EV_BUF_SIZE);
        haveEvent = 1;
    }
    if(ebuf == NULL){
        recvBuf = sendBuf;
    }
    return res;
}


/**************************************************************************************
 *  ckEvents() - check for incoming event, and copy it to lastEvt struct passed.
 *  after copyng, haveEvent is set to 0
 */
uint8_t NxtLcd::ckEvents(nxtEvent_t* lastEvt){
    if(haveEvent == 0) readEvent();
    if(haveEvent == 1){
        memset(lastEvt,0,sizeof(nxtEvent_t));
        memcpy(lastEvt,&lastEvent,sizeof(nxtEvent_t));
        haveEvent = 0;
        return 1;
    }
    return 0;
}


