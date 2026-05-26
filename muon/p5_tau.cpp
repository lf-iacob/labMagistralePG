#include <iostream>
#include <cmath>

void conti(){
    double tau_fin;
    double sys_up=81.41, sys_down=59.7, stat_up=35.6, stat_down=71.1;
    double tau_up=2155, tau_down=2144;
    double err_up, err_down;
    err_up=std::sqrt(sys_up*sys_up + stat_up*stat_up);
    err_down=std::sqrt(sys_down*sys_down + stat_down*stat_down);
    std::cout<<"Tau up: "<<tau_up<<" +- "<<err_up<<std::endl;
    std::cout<<"Tau down: "<<tau_down<<" +- "<<err_down<<std::endl;

    double sum=1/(err_up*err_up) + 1/(err_down*err_down);
   
    tau_fin=(tau_up*(1/(err_up*err_up)) + tau_down*(1/(err_down*err_down)))/sum;


    std::cout<<"Tau finale: "<<tau_fin*0.001<<" +- "<<std::sqrt(1/sum)*0.001<<std::endl;

}
