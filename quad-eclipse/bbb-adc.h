
#ifndef VBAT_CPP_
#define VBAT_CPP_

class adc_monitor {
public:
     float vbat_raw;
     float vbat;
     void init();
     void update();
};

#endif