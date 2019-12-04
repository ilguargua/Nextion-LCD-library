/* nxt_lcd.h
 * 
 * Arduino platform library for Itead Nextion displays
 * Instruction set : https://nextion.tech/instruction-set/
 * 
 * Library implements almost of the basic and ehnached display function
 * but none (yet) of the professional ones.
 * 
 * (c) Guarguaglini Alessandro - ilguargua@gmail.com
 * 
*/

#ifndef __NXT_LCD_H__
#define __NXT_LCD_H__

#include <SoftwareSerial.h>
#include <HardwareSerial.h>


//anyserial - a small wrapper to use indifferently hardware or software serial
class anySerial{
private:
    HardwareSerial*     hwSerial;
    SoftwareSerial*     swSerial;
public:
    anySerial(void){};
    void   init(HardwareSerial* port){hwSerial=port;swSerial=NULL;};
    void   init(SoftwareSerial* port){hwSerial=NULL;swSerial=port;};
    void   begin(uint32_t baud){if(hwSerial) hwSerial->begin(baud);else swSerial->begin(baud);};
    void   end(void){if(hwSerial) hwSerial->end();else swSerial->end();};
    int    available(void){if(hwSerial) return hwSerial->available();else return swSerial->available();};
    int    read(void){if(hwSerial) return hwSerial->read();else return swSerial->read();};
    size_t write(const char* b, size_t s){if(hwSerial) return hwSerial->write(b,s);else return swSerial->write(b,s);};
};



/*
 * a buffer is used for both writing and reading of display command/reply.
 * The size influence the weight in RAM memory of the NxtLcd instance.
*/
#define NXT_BUF_SIZE              128

#define NXT_EV_BUF_SIZE           10

#define NXT_REPLY_WAIT            20  

#define NXT_MSG_END               0xFF,0xFF,0xFF

#define NXT_PROP_SIZE             7



void serialLogStr(const char *msg, const char *value = NULL);
void serialLogInt(const char *msg, const int32_t value);
void serialLogHex(const char *msg, const int32_t value);


/*
 * colors value as defined in nextion editor.
 * value are in RGB565. Use one of the many online RGB 565 color picker
 * if you want create your own.
*/
typedef enum {
    BLACK =  0,
    BLUE =   31,
    GREEN =  2016,
    GRAY =   33840,
    BROWN =  48192,
    RED =    63488,
    YELLOW = 65504,
    WHITE =  65535
} nextionColor_t;

/*
 * display types
*/
typedef enum {
    nxt_basic,
    nxt_enhanced,
    nxt_prof
} dispType_t;

/*
 * display properties names 
*/
const char sysPropNames[][NXT_PROP_SIZE] PROGMEM{
    "dp",
    "dim",
    "dims",
    "spax",
    "spay",
    "thc",
    "thdra",
    "ussp",
    "thsp",
    "thup",
    "sendxy",
    "delay",
    "sleep",
    "bkcmd",
    "sys0",
    "sys1",
    "sys2",
    "wup",
    "usup",
    "rtc0",
    "rtc1",
    "rtc2",
    "rtc3",
    "rtc4",
    "rtc5",
    "rtc6"
};

typedef enum {
    nxt_dp,
    nxt_dim,
    nxt_dims,
    nxt_spax,
    nxt_spay,
    nxt_thc,
    nxt_thdra,
    nxt_ussp,
    nxt_thsp,
    nxt_thup,
    nxt_sendxy,
    nxt_delay,
    nxt_sleep,
    nxt_bkcmd,
    nxt_sys0,
    nxt_sys1,
    nxt_sys2,
    nxt_wup,
    nxt_usup,
    nxt_rtc0,
    nxt_rtc1,
    nxt_rtc2,
    nxt_rtc3,
    nxt_rtc4,
    nxt_rtc5,
    nxt_rtc6,
    sysPropLen      //this must always be the last!
} sysPropNdx_t;

#define nxtBasicEnd     nxt_usup
#define nxtEnhancedEnd  nxt_rtc6

/*
 * code returned from display in reply of commands
*/
typedef enum {
    cmdFail =           0x00,
    cmdOk =             0x01,
    cmdInvalidCid =     0x02,
    cmdInvalidPage =    0x03,
    cmdInvPicId =       0x04,
    cmdInvFontId =      0x05,
    cmdInvVar =         0x1A,
    cmdBufOvfl =        0x24,
    cmdTouchCompEv =    0x65,
    cmdSendme =         0x66,
    cmdTouchXYaw =      0x67,
    cmdTouchXYsl =      0x68,
    cmdGetStr =         0x70,
    cmdGetNum =         0x71,
    cmdSleepOn =        0x86,
    cmdSleepOff =       0x87,
    cmdDevReady =       0x88,
    cmdTDEnd =          0xFD,
    cmdTDReady =        0xFE
    
} cmdReply_t;


/*
 * format of numeric boxes
*/
typedef enum {
    decimal,
    currency,
    hex
} numFormat_t;

/*
 * reply code returned by all NxtLcd public methods 
*/
 typedef enum {
    //no error reply
    noReply,            //0 no reply from lcd to command, not necessary an error
    replyCmdOk,         //1 cmd success
    //class methods errors
    notInit = 10,       //10 serial not initialized
    noComplete,         //11 some data in buffer, but no complete reply
    bufOvfl,            //12 recvBuf overflow
    invalidData,        //13 - wrong data passed to function (from lib, not device)
    replyUnknown,       //14 reply code unknown - need to be added
    notSupported,       //15 feature not supported
    dataTooBig,         //16 data passed to funct. exceed buffer len
    bugTest,            //17 just for test
    //error reported by nextion device
    replyCmdFail = 20,  //20 cmd failure
    replyWrongId,       //21 wrong component,page,picture or font ID    
    replyWrongVar,      //22 wrong variable
    replyBufOvfl,       //23 nextion serial buffer overflow
    //event replay
    replyDevReady = 30, //30 device ready, sent after initializition (or reset)
    replyTouchEv,       //31 touch event, a button has been pressed or release
    replyStartUp,       //32 device startup
    replySleepEv,       //33 device enter/exit sleep mode
    //reply to user cmds
    replyGetStr = 40,   //40 String received 
    replyGetNum,        //41 number received
    replySendMe,        //42 current page, reply to sendme    
    replyTDReady,       //43 Transparent data ready
    replyTDEnd          //44 Transparent data end
} readCode_t;

/*
 * nxtEvent_t - Structure to hold an incoming event.
 * "evCode" is the returned event code, can be : 0x65 for component touch  
 *                                               0x66 for touch event with display awake
 *                                               0x67 as above with display in sleep
 * "page_X"   hold page id (for 0x65 event), or X coordinate for others touch event
 * "compId_Y" hold component id or Y coordinate
 * "event"    can be 1 (pressed) or 0 (released)
*/
typedef struct {
    uint8_t  evCode;
    uint16_t page_X;
    uint16_t compId_Y;
    uint8_t  event;
} nxtEvent_t;


class NxtLcd{
private:
    anySerial           serial;
    uint8_t             initialized;
    uint8_t             debug;
    uint8_t             sendBuf[NXT_BUF_SIZE];
    uint8_t             evtBuf[NXT_EV_BUF_SIZE];
    uint8_t*            recvBuf=sendBuf;
    uint8_t             lastTouchCode;
    uint8_t             dispType;
    uint8_t             wrongIdCode;
    uint8_t             haveEvent;
    uint16_t            getStrLen;
    uint16_t            sysProp[sysPropLen];
    //uint8_t             nxtBasicEnd = nxt_usup;
    //uint8_t             nxtEnhancedEnd = nxt_rtc6;
    
    uint8_t             getPropCnt(void);
    uint8_t             chkProperty(const char* prop);
    uint8_t             chkProperty(uint8_t prop);
    
    nxtEvent_t        lastEvent;

    
    void                bufReset(uint8_t* buf){memset(buf,0,NXT_BUF_SIZE);};
    uint8_t             readEvent(uint8_t* buf=NULL);
    uint8_t             readBuf(uint8_t ckevt = 0);
    uint8_t             writeBuf(uint8_t expReply = 0, 
                                 uint16_t wait = NXT_REPLY_WAIT,
                                 uint16_t size = 0
                                );

    
public:
    /*
     * Class public interface.
     * Please note : all functions return a readCode_t value, that must to be
     * checked by user, at least if you want to know if command is succesful or not.
    */
    
    NxtLcd(HardwareSerial* port);
    NxtLcd(SoftwareSerial* port);

    
    uint8_t     init(uint32_t baudrate = 9600, uint8_t dispType = 1, uint8_t reset = 1, uint8_t dbg = 1);

    uint8_t     devReset(void);
    
    /*
     * setBkcmd() - set the 'bkcmd' property of display.
     *   - 0 is Off – no pass/fail will be returned.
     *   - 1 is OnSuccess, only when last serial command successful.
     *   - 2 is OnFailure, only when last serial command failed (default)
     *   - 3 is Always, returns 0x00 to 0x23 result of serial command.
    */
    uint8_t     setBkcmd(uint8_t value = 2);

    /*
     * setDim() - set backligth intensity, range 0/100
    */
    uint8_t     setDim(uint8_t value);
    
    
    /*
     * setNTSTimer() - set the sleep on no touch timer."val" in the time in seconds
     * to wait for no touch, 0 to disable.
    */
    uint8_t     setNTSTimer(uint16_t val);
    
    /*
     * setTouchAW() - set the touch auto wake feature - 1:enable; 0:disable
    */
    uint8_t     setTouchAW(uint8_t val);
    
    /*
     * setNSSTimer() - set the sleep on no serial timer, works as setNTSTimer() but for
     * serial communication.
    */
    uint8_t     setNSSTimer(uint16_t val);
    
    
    /*
     * setSerialAW() - set the serial auto wake feature 
    */
    uint8_t     setSerialAW(uint8_t val);
    
    /*
     * setSleep() - Put the display in sleep mode ("val"=1) or wake it ("val"=0)
    */
    uint8_t     setSleep(uint8_t val);
    
    
    uint8_t     setWakeUpPage(uint8_t page);

    /*
     * setProperty() - set the display system variable indicate in "prop" to "value"
     * List of properties that can be setted can be found here :
     *  - https://nextion.tech/instruction-set/#s6
     * The 2 above methods setDim() and setBkcmd() uses setProperty() to do the job.
     * As for others methods, "prop" can be a progmem constant.
    */
    uint8_t     setProperty(const char* prop, uint16_t value);
    uint8_t     setProperty(uint8_t prop, uint16_t value);
    
    
    uint8_t     getProperty(const char* prop, uint16_t* value, uint8_t loc = 0);
    uint8_t     getProperty(uint8_t prop, uint16_t* value, uint8_t loc = 0);
    
    /*
     * cls() - clear the screen and put "color" as background
    */
    uint8_t     cls(uint16_t color);
    
    /*
     * refresh() - refresh an object, bring it foreground. If object id is 0 all page
     * is refreshed
    */
    uint8_t     refresh(const char* obj);
    uint8_t     refresh(uint8_t objId = 0);
    
    /*
     * getObjAttr() - return an object numeric attribute. 
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
     *  (*)read-only attribute  (-)read-only on some objects
     * 
     * objects are addressed as pageName,objName or pageId,objId or objName or objId , last 2
     * acts on current page.
    */
    uint8_t     getObjAttr(const char* page, const char* obj, const char* attr, uint16_t* value);
    uint8_t     getObjAttr(uint8_t page, uint8_t obj, const char* attr, uint16_t* value);
    uint8_t     getObjAttr(const char* obj, const char* attr, uint16_t* value);
    uint8_t     getObjAttr(uint8_t obj, const char* attr, uint16_t* value);
    
    /*
     * setObjAttr() - set an object numeric attribute. See getObjAttr() for details
    */
    uint8_t     setObjAttr(const char* page, const char* obj, const char* attr, uint16_t value);
    uint8_t     setObjAttr(uint8_t page, uint8_t obj, const char* attr, uint16_t value);
    uint8_t     setObjAttr(const char* obj, const char* attr, uint16_t value);
    uint8_t     setObjAttr(uint8_t obj, const char* attr, uint16_t value);    
    
    /*
     * setBackColor() - set object background color, a shortcuts for setObjAttr()
    */
    uint8_t     setBackColor(const char* page, const char* obj, uint16_t value);
    uint8_t     setBackColor(uint8_t page, uint8_t obj, uint16_t value);
    uint8_t     setBackColor(const char* obj, uint16_t value);
    uint8_t     setBackColor(uint8_t obj, uint16_t value);        

    /*
     * setForeColor() - set object foreground color, a shortcuts for setObjAttr()
    */
    uint8_t     setForeColor(const char* page, const char* obj, uint16_t value);
    uint8_t     setForeColor(uint8_t page, uint8_t obj, uint16_t value);
    uint8_t     setForeColor(const char* obj, uint16_t value);
    uint8_t     setForeColor(uint8_t obj, uint16_t value);        

    /*
     * formatNumb() - format numeric objects, setting the 'lenth' and 'format' attributes (see above)
    */
    uint8_t     formatNumb(const char* page, const char* obj, uint8_t len, uint8_t format = decimal);
    uint8_t     formatNumb(uint8_t page, uint8_t obj, uint8_t len, uint8_t format = decimal);
    uint8_t     formatNumb(const char* obj, uint16_t len, uint8_t format = decimal);
    uint8_t     formatNumb(uint8_t obj, uint16_t len, uint8_t format = decimal);            
    
    uint8_t     click(const char* obj, uint8_t ev = 1);
    uint8_t     click(uint8_t obj, uint8_t ev = 1);
    
    uint8_t     touchEn(const char* obj, uint8_t en);
    uint8_t     touchEn(uint8_t obj, uint8_t en);    
    
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
    uint8_t     setString(const char* page,const char* field,const char* value);
    uint8_t     setString(uint8_t page,uint8_t field,const char* value);
    uint8_t     setString(const char* field,const char* value);
    uint8_t     setString(uint8_t field,const char* value);
    
    
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
     * objects can be addressed as above.
     * Functions do not check the value limits
    */
    uint8_t     setNumeric(const char* page,const char* field,long value);
    uint8_t     setNumeric(uint8_t page,uint8_t field,long value);
    uint8_t     setNumeric(const char* field,long value);
    uint8_t     setNumeric(uint8_t field,long value);
    
    
    /*
     * setFloat() - Set the 'val' property of an Xfloat object.
     * Despite of the name, float on nextion display doesn't exist. The value passed to the 
     * functions and by them writed on 'val' object property is a signed 32bit integer. The
     * 'vvs0' and 'vvs1' properties of the object fix the length of integral and fractional part,
     * these are the intSize and frctSize parameter of the functions. So, the value 123456 will
     * be show on display as '123.456' in case 'vvs0'=3 and 'vvs1'=3, or '012.3456' in case of
     * 'vvs0'=3 and 'vvs1'=4, and so on. 
     * objects can be addressed as above.
    */
    uint8_t     setFloat(const char* page, const char* field,
                         long value, uint8_t intSize = 0, uint8_t frctSize = 0);
    uint8_t     setFloat(uint8_t page, uint8_t field,
                         long value, uint8_t intSize = 0, uint8_t frctSize = 0);
    uint8_t     setFloat(const char* field,
                         long value, uint8_t intSize = 0, uint8_t frctSize = 0);
    uint8_t     setFloat(uint8_t field,
                         long value, uint8_t intSize = 0, uint8_t frctSize = 0);    

    /* 
     * getString() - Get the 'txt' property of an object. Supported objects are same of setString().
     * objects are addressed as above. "value" must be a pointer to char array, that must to be declared by 
     * user, size is the size of "value", with the same limit as setString() related to NXT_BUF_SIZE. 
    */
    uint8_t     getString(const char* page,const char* field,char* value,uint16_t size);
    uint8_t     getString(uint8_t page,uint8_t field,char* value,uint16_t size);
    uint8_t     getString(const char* field,char* value,uint16_t size);
    uint8_t     getString(uint8_t field,char* value,uint16_t size);    
    
    /* 
     * getNumeric() - Get the 'val' property of an object. Supported objects are same of setNumeric(),
     * objects are addressed as above. "value" can be a pointer to a int8_t, int16_t or int32_t,
     * using sizes of 1,2 or 4.
    */
    uint8_t     getNumeric(const char* page,const char* field,
                           void* value, uint8_t size);
    uint8_t     getNumeric(uint8_t page,uint8_t field,void* value,uint8_t size);
    uint8_t     getNumeric(const char* field,void* value,uint8_t size);
    uint8_t     getNumeric(uint8_t field,void* value,uint8_t size);
    
    /*
     * addWavePoint() - add a single point to a wave object channel. wave object can have to 4
     * channels, see 'ch' property in Nextion editor (can't be changed at runtime). "value" limit is 
     * 0 to 255, or 0 to wave object heigth, if you want to see the whole trace.  
    */
    uint8_t     addWavePoint(uint8_t waveId,uint8_t ch, uint8_t value);
    
    /*
     * addWaveBytes() - add "len" bytes to wave object channel at once. In this case len can exceed the
     * NXT_BUF_SIZE limit, if so the transmission of data will be splitted in chunks that fits
     * into the buffer. 
    */
    uint8_t     addWaveBytes(uint8_t waveId,uint8_t ch, uint8_t* bytes, uint16_t len);
    
    uint8_t     clearWaveCh(uint8_t waveId, uint8_t ch = 255);
    
    uint8_t     waveUpdtEn(uint8_t en);
    
    
    /*
     * writeStr() - write string to display.
     * msg in the string to be displayed (can reside in program memory using the F() macro), x and y are
     * coordinate of upper left corner, w and h the width and heigth of msg box, font is the ID of one
     * of the (preloaded) font, fCol and Bcol are foreground and background color, Xcen is the horizontal
     * centering (0-left,1-center,2-right), Ycen is vertical centering (0-up,1-center,2-down)
    */
    uint8_t     writeStr(const char* msg, uint16_t x,uint16_t y,uint16_t w, 
                         uint16_t h,uint8_t font = 0,uint16_t fCol = 0,
                         uint16_t bCol = 65535, uint8_t xCen = 1, uint8_t yCen = 1);
    
    /*
     * drawArea() - draw a rectangle to display.
     * x and y are coordinate of upper left corner, w and h the width and heigth of the box. filled specify
     * if the area will be filled (1) or empty (0). color is the color used to fill/draw area (RGB 565)
    */
    uint8_t     drawArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h,uint16_t color, uint8_t filled = 0);
    
    /*
     * drawLine() - draw a line from coordinate x,y to coordinate x1,y1, using color as ink
    */
    uint8_t     drawLine(uint16_t x,uint16_t y,uint16_t x1,uint16_t y1,uint16_t color = 0);
    
    /*
     * drawCircle() - draw a circle with center at coordinate x,y having radius r and using color
     * as ink. filled used as in drawArea()
     * 
    */
    uint8_t     drawCircle(uint16_t x, uint16_t y, uint16_t r, uint16_t color, uint8_t filled = 0);
    
    /*
     * setVis() - change visibility of an object, identified by name or id; state 0 hide object, 1 show it.
     * show() and hide() are shortcuts for setVis(obj,1) and setVis(obj,0)
    */
    uint8_t     setVis(const char* obj,uint8_t state);
    uint8_t     setVis(uint8_t obj,uint8_t state);
    uint8_t     show(const char* obj){return setVis(obj,1);};
    uint8_t     show(uint8_t obj){return setVis(obj,1);};;
    uint8_t     hide(const char* obj){return setVis(obj,0);};
    uint8_t     hide(uint8_t obj){return setVis(obj,0);};;    
    
    uint8_t     getWrongId(void){return wrongIdCode;};
    
    /*
     * setDate() - set the internal RTC date.
     * day is the day of the month, range 0/31
     * month is the month of the year, range 0/12
     * year is 4 digit year in range of 2000/2099
     * parameter are checked as above by the function, but not for coerency, es. you could
     * set the date as 31 Feb 2012, I have not checked yet what happen in this case
    */
    uint8_t     setDate(uint8_t day,uint8_t month,uint16_t year);
    
    /*
     * setTime() - set the internal RTC clock
     * hour range is 0/23
     * minute and second range is 0/59
    */
    uint8_t     setTime(uint8_t hour,uint8_t minute,uint8_t second=0);
    
    /*
     * getPage() - get current page ID.
     * please note, as all others public method of this class, the value is returned into parameter
     * "page", NOT by the function itself
    */
    uint8_t     getPage(uint8_t* page);
    
    /*
     * setPage() - set current page, using page name or ID
    */
    uint8_t     setPageS(const char* page);
    uint8_t     setPageN(uint8_t page);
    

    
    /*
     * ckEvents() - method to be called in main program loop to check if there have been touch
    */
    //uint8_t     ckEvents(uint32_t* evCode=NULL, nxtEvent_t* te=NULL);
    uint8_t     ckEvents(nxtEvent_t* lastEvt);
    
    /*
     * getEvCode() - helper function to get buttons event code
    */
    //uint32_t    setEvCode(uint8_t page, uint8_t obj, uint8_t ev);
    //void        getEvent(uint32_t evCode, nxtEvent_t* te);
    
    
    
#ifdef ARDUINO_ARCH_AVR
    /*
     * These overloaded function are compiled only for AVR platform, the usage is the same as above but
     * use the F() macro instead of const char* to save RAM memory 
    */
    
    uint8_t     setVis(const __FlashStringHelper* obj,uint8_t state);
    uint8_t     show(const __FlashStringHelper* obj){return setVis(obj,1);};
    uint8_t     hide(const __FlashStringHelper* obj){return setVis(obj,0);};
    
    uint8_t     setString(const __FlashStringHelper* page,
                          const __FlashStringHelper* field,const __FlashStringHelper* value);
    uint8_t     setString(uint8_t page,uint8_t field,const __FlashStringHelper* value);
    uint8_t     setString(const __FlashStringHelper* field,const __FlashStringHelper* value);
    uint8_t     setString(uint8_t field,const __FlashStringHelper* value);
    
    uint8_t     setNumeric(const __FlashStringHelper* page,
                           const __FlashStringHelper* field,long value);
    uint8_t     setNumeric(const __FlashStringHelper* field,long value);

    uint8_t     getString(const __FlashStringHelper* field,char* value,uint16_t size);
    uint8_t     getString(const __FlashStringHelper* page,const __FlashStringHelper* field,
                          char* value,uint16_t size);
    
    uint8_t     writeStr(const __FlashStringHelper* msg, uint16_t x,uint16_t y,uint16_t w, 
                         uint16_t h,uint8_t font=0,uint16_t fCol=0,
                         uint16_t bCol = 65535, uint8_t xCen = 1, uint8_t yCen = 1);
    
    uint8_t     getNumeric(const __FlashStringHelper* page,const __FlashStringHelper* field,
                           void* value, uint8_t size);
    uint8_t     getNumeric(const __FlashStringHelper* field,void* value, uint8_t size);

    uint8_t     setPageS(const __FlashStringHelper* page);

    uint8_t     setFloat(const __FlashStringHelper* page, const __FlashStringHelper* field,
                         long value, uint8_t intSize = 0, uint8_t frctSize = 0);
    uint8_t     setFloat(const __FlashStringHelper* field,
                         long value, uint8_t intSize = 0, uint8_t frctSize = 0);  
    
    uint8_t     setProperty(const __FlashStringHelper* prop, uint16_t value);
    
    uint8_t     refresh(const __FlashStringHelper* obj);
    
    uint8_t     getObjAttr(const __FlashStringHelper* page, const __FlashStringHelper* obj, 
                           const __FlashStringHelper* attr, uint16_t* value);
    uint8_t     getObjAttr(uint8_t page, uint8_t obj, const __FlashStringHelper* attr, uint16_t* value);
    uint8_t     getObjAttr(const __FlashStringHelper* obj, const __FlashStringHelper* attr, uint16_t* value);
    uint8_t     getObjAttr(uint8_t obj, const __FlashStringHelper* attr, uint16_t* value);

    uint8_t     setObjAttr(const __FlashStringHelper* page, const __FlashStringHelper* obj, 
                           const __FlashStringHelper* attr, uint16_t value);
    uint8_t     setObjAttr(uint8_t page, uint8_t obj, const __FlashStringHelper* attr, uint16_t value);
    uint8_t     setObjAttr(const __FlashStringHelper* obj, const __FlashStringHelper* attr, uint16_t value);
    uint8_t     setObjAttr(uint8_t obj, const __FlashStringHelper* attr, uint16_t value);  
    
    uint8_t     setBackColor(const __FlashStringHelper* page, const __FlashStringHelper* obj, uint16_t value);
    uint8_t     setBackColor(const __FlashStringHelper* obj, uint16_t value);
    
    uint8_t     setForeColor(const __FlashStringHelper* page, const __FlashStringHelper* obj, uint16_t value);
    uint8_t     setForeColor(const __FlashStringHelper* obj, uint16_t value);
    
    uint8_t     formatNumb(const __FlashStringHelper* page, const __FlashStringHelper* obj, 
                           uint8_t len, uint8_t format = decimal);
    uint8_t     formatNumb(const __FlashStringHelper* obj, uint16_t len, uint8_t format = decimal);
    
    uint8_t     click(const __FlashStringHelper* obj, uint8_t ev = 1);
    
    uint8_t     touchEn(const __FlashStringHelper* obj, uint8_t en);
    
#endif // ARDUINO_ARCH_AVR   
    
};

#endif // __NXT_LCD_H__
