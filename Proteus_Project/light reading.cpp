#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRPS.h>

AnalogInputPin CdS_cell (FEHIO::P1_0);
DigitalEncoder right_encoder(FEHIO::P0_0);
DigitalEncoder left_encoder(FEHIO::P3_0);
FEHMotor right_motor(FEHMotor::Motor1,9.0);
FEHMotor left_motor(FEHMotor::Motor0,9.0);
DigitalInputPin micro_left(FEHIO::P3_1);
DigitalInputPin micro_right(FEHIO::P0_1);
AnalogInputPin rightOpt(FEHIO::P2_0); //put right in 2_2
AnalogInputPin leftOpt(FEHIO::P2_2);    //left in 2_0
AnalogInputPin middleOpt(FEHIO::P2_1);
FEHMotor servo(FEHMotor::Motor3,9.0);
DigitalInputPin micro_front(FEHIO::P1_1);

int main(){
    while(true){
       LCD.Clear();
       LCD.PrintLine(CdS_cell.Value());
       LCD.PrintLine(RPS.X()); 
       LCD.PrintLine(RPS.Y()); 
       LCD.PrintLine(RPS.Heading());
       Sleep(1000);
    }

}