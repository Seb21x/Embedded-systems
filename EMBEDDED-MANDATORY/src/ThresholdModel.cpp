#include "ThresholdModel.h"
#include <cmath>
#include <algorithm>
namespace { float clamp(float x,float lo,float hi){return std::max(lo,std::min(x,hi));} }
void ThresholdModel::update(float raw,bool selectHigh,float minimum,float maximum){
        const float x=minimum+clamp(raw,0,1)*(maximum-minimum);
        knob=x;
        if(selectHigh!=highSelected){highSelected=selectHigh;captured=false;havePrevious=false;}
        const float target=highSelected?high:low;
        if(!captured && (std::fabs(x-target)<=0.5f ||
           (havePrevious && (previous-target)*(x-target)<=0))) captured=true;
        previous=x;havePrevious=true;
        if(!captured)return;
        const float rounded=std::round(x*2)/2;
        if(highSelected)high=clamp(rounded,low+2,maximum);
        else low=clamp(rounded,minimum,high-2);
    }
