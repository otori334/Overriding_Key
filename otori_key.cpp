#include "otori_key.h" 
#if defined(_USING_HID) 
//#define NOT_A_TRAINING  //本番は負論理で
    #ifdef  NOT_A_TRAINING 
    #define D_NOT 
    #else 
    #define D_NOT   ! 
    #endif 

static uint8_t IN_NUM[6] = {
    B00000010, B00000100, B00001000, //SCK, MO, MI 
    B00100000, B01000000, B10000000  //  9, 10, 11
}; 

static uint8_t OUT_PIN[6] = {
    10,9,7,22,11,4}
; //g,f,d,s,h,a 

extern volatile uint32_t timer0_millis; 
uint32_t time_zero[6]; 
uint32_t THE_WORLD; 
uint8_t BUFFER[2];
uint8_t FLAG[2]; 

static const uint8_t _hidReportDescriptor[] PROGMEM = { 
    0x05, 0x01, //USAGE_PAGE (Generic Desktop)  // 47
    0x09, 0x06, //USAGE (Keyboard) 
    0xa1, 0x01, //COLLECTION (Application) 
    0x85, 0x02, //REPORT_ID (2) 
    0x05, 0x07, //USAGE_PAGE (Keyboard) 
    0x19, 0xe0, //USAGE_MINIMUM (Keyboard LeftControl) 
    0x29, 0xe7, //USAGE_MAXIMUM (Keyboard Right GUI) 
    0x15, 0x00, //LOGICAL_MINIMUM (0) 
    0x25, 0x01, //LOGICAL_MAXIMUM (1) 
    0x75, 0x01, //REPORT_SIZE (1) 
    0x95, 0x08, //REPORT_COUNT (8) 
    0x81, 0x02, //INPUT (Data,Var,Abs) 
    0x95, 0x01, //REPORT_COUNT (1) 
    0x75, 0x08, //REPORT_SIZE (8) 
    0x81, 0x03, //INPUT (Cnst,Var,Abs) 
    0x95, 0x06, //REPORT_COUNT (6) 
    0x75, 0x08, //REPORT_SIZE (8) 
    0x15, 0x00, //LOGICAL_MINIMUM (0) 
    0x25, 0x73, //LOGICAL_MAXIMUM (115) 
    0x05, 0x07, //USAGE_PAGE (Keyboard) 
    0x19, 0x00, //USAGE_MINIMUM (Reserved (no event indicated)) 
    0x29, 0x73, //USAGE_MAXIMUM (Keyboard Application) 
    0x81, 0x00, //INPUT (Data,Ary,Abs) 
    0xc0,       //END_COLLECTION 
}; 
 
OTR_::OTR_(void) 
{ 
    static HIDSubDescriptor node(_hidReportDescriptor, sizeof(_hidReportDescriptor)); 
    HID().AppendDescriptor(&node); 
} 
 
void OTR_::setup(void) 
{ 
    PORTB = B11101111; 
    BUFFER[0] = PINB & B11101110; 
    for (uint8_t i=0;i<6;i++)
    { 
        time_zero[i] = 0; 
        PORTC ^= B10000000; 
        delay(1000); 
    } 
} 
 
void OTR_::loop(uint8_t INTERVAL) 
{ 
    POINT0:
    BUFFER[1] = PINB & B11101110; 
    THE_WORLD = timer0_millis;
    
    for (uint8_t i=0; i<6; i++) 
    {
        if (((!_OTR.keys[i] && D_NOT(IN_NUM[i] & BUFFER[1])) || 
            (_OTR.keys[i] && !D_NOT(IN_NUM[i] & BUFFER[1]))) && 
            THE_WORLD - time_zero[i] > INTERVAL && 
            FLAG[0] & IN_NUM[i])
        {
            FLAG[0] &= ~IN_NUM[i]; 
            FLAG[1] |=  IN_NUM[i]; 
        } 
    }
    
    if (BUFFER[1] ^ BUFFER[0] || FLAG[1])       //変化の有無を確認
    { 
        for (uint8_t i=0;i<6;i++) 
        { 
            if ((IN_NUM[i] & (BUFFER[1] ^ BUFFER[0]) &&
                THE_WORLD - time_zero[i] > INTERVAL) ||
                FLAG[1] & IN_NUM[i])
            { 
                FLAG[0] |=  IN_NUM[i]; 
                FLAG[1] &= ~IN_NUM[i]; 
                time_zero[i] = THE_WORLD; 
                if (D_NOT(IN_NUM[i] & BUFFER[1])) 
                { 
                    _OTR.keys[i] = OUT_PIN[i]; 
                }else{ 
                    _OTR.keys[i] = 0; 
                }
            } 
        }
        HID().SendReport(2,&_OTR,8); 
    }
    BUFFER[0] = BUFFER[1]; 
    goto POINT0;
    
} 

OTR_ otori_key; 
#endif 