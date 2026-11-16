#include "EnvironmentMonitor.h"
int main(){
    // Construct here: LCD initialization can sleep only after the scheduler starts.
    EnvironmentMonitor application;
    application.run();
}
