#ifndef OTR_h
#define OTR_h
#include "HID.h"

#if !defined(_USING_HID)

#warning "Using legacy HID core (non pluggable)"

#else
typedef struct
{
    uint8_t mob[2]; 
    uint8_t keys[6];
} OTReport;

class OTR_ 
{
public:
    void setup(void);
    void loop(uint8_t INTERVAL);
    OTReport _OTR;
    OTR_(void);
};
extern OTR_ otori_key;
#endif
#endif