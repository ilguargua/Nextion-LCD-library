/* event_handle.ino
 * 
 * Arduino platform library for Itead Nextion displays
 * Instruction set : https://nextion.tech/instruction-set/
 * 
 * Library implements almost of the basic and ehnached display function
 * but none (yet) of the professional ones.
 * 
 * (c) Guarguaglini Alessandro - ilguargua@gmail.com
 *
 * little example to prove some library functionality.
 * 
 * Sketch have been tested with an NX4024K032 display connected to an Arduino nano
 * 
 * With nextion editor set up a page that have at least :
 * - 1 Numeric entry (type 54)
 * - 1 slider (type 1)
 * - 1 gauge (type 122)
 * - 1 progress bar (type 106)
 * - 6 radio buttons (type 57)
 * - 1 wave widget (type 0) (suggested dimensions 140x70 at least)
 *
 * take note of id of the above widgets and replace the value in the following #define .
 * Chek out the box of "send component id" in the touch press event window of all radio buttons. Do the same
 * for touch release event of the slider.
 * We try to handle mutually exclusive radio buttons (as they should be, otherwise the only difference
 * from chekboxes is the shape!) while updating update of wave, gauge and progress bar. Moving the slider
 * will update current value to numeric box.
 * PLEASE NOTE : this is not the best way to handle this situation, it would be better done writing code
 * into the Nextion editor, but is here just to demostrate event handling use.
*/

#include <SoftwareSerial.h>
#include <nxt_lcd.h>

#define numbBox     3
#define slider      8
#define gauge       6
#define prgBar      5
#define wave        7

SoftwareSerial intf(4,5);  //we use software serial for display, so Serial can be used for debug
                           //In this case: blu wire to pin D4, yellow wire to pin D5 
NxtLcd      lcd(&intf);    //creating the NxtLcd instance

const uint8_t rdBtns[6] = {10,16,17,18,19,20}; //these are the id of radio buttons

uint8_t waveBuf[150]; //an array to hold value for sine wave

void setup(){
    lcd.init(38400,nxt_enhanced);  //initializing the display. Use the same baudrate that you set in the editor
    
    //populating the array with values to show sinw wave
    uint16_t wc = 0;
    for(double i=1.00; wc<150;i += 0.01){
        double a = sin(M_PI * 2 * 2 * i) * 100;
        waveBuf[wc] = 50 + (int(a)/3);
        wc++;
    }    
    

}


void loop(){
    static uint32_t gaugeUpd = 0; //used for time interval of gauge update
    static uint16_t sec = 0;      //value showed in gauge
    static uint32_t waveUpd = 0;  //used for time interval of wave update
    static uint8_t slVal = 0;     //progress bar value  
//    uint32_t ev = 0;
    static uint8_t wCnt = 0;      //counter for wavw data array
    nxtEvent_t evData;            //struct to hold incoming events
    
    while(lcd.ckEvents(&evData) == 1){        //check if there are incoming events
        if(evData.evCode == cmdTouchCompEv){  //if we have componnt touch event
            if(evData.page_X == 0){           //check if we are on the right page
                uint8_t isRbtn = 0;
                for(uint8_t i=0;i<6;i++){
                    if(evData.compId_Y == rdBtns[i]){  //check if the component id is a radio btn
                        isRbtn = 1;
                        break;
                    }
                }   
                if(isRbtn > 0){     //if a radio btn is clicked, set this to 1 and all other to 0
                    for(uint8_t i=0;i<6;i++){
                        if(evData.compId_Y == rdBtns[i]) lcd.setNumeric(rdBtns[i],1);
                        else lcd.setNumeric(rdBtns[i],0);
                    }
                }   
                if(evData.compId_Y == slider){
                    uint8_t val;
                    lcd.getNumeric(slider,&val,1);
                    lcd.setNumeric(numbBox,val);
                }
            } //if page0
        }
    }
    
    if(millis() > waveUpd + 100){     //update wave widget every 100 ms
        lcd.addWavePoint(wave,1,waveBuf[wCnt]);
        wCnt++;
        if(wCnt == 150) wCnt = 0;
        waveUpd = millis();
        lcd.setNumeric(prgBar,slVal);
        slVal++;
        if(slVal == 100) slVal = 0;
    }
    
    if(millis() > gaugeUpd+1000){  //update gauge every second
        gaugeUpd = millis();
        sec += 6;
        if(sec > 360) sec = 0;
        lcd.setNumeric(gauge,sec);
    }    
    

}
