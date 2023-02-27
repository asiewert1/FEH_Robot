#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRPS.h>


//Declarations for encoders & motors
AnalogInputPin CdS_cell (FEHIO::P0_1)
DigitalEncoder right_encoder(FEHIO::P0_0);
DigitalEncoder left_encoder(FEHIO::P0_1);
FEHMotor right_motor(FEHMotor::Motor3,9.0);
FEHMotor left_motor(FEHMotor::Motor0,9.0);
DigitalInputPin micro_left(FEHIO::P0_1)
DigitalInputPin micro_right(FEHIO::P0_1)

void turnLeft(int tcount, int tpercent);
void turnRight(int tcount, int tpercent);
void moveForward(int counts,int percent);
void moveBackward(int counts,int percent);
void zero();
void boardingPass();




void moveForward(int counts, int percent){
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    //turn motors off
    zero();
}

void moveBackward(int counts, int percent){
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(-percent);
    left_motor.SetPercent(-percent);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    //turn motors off
    zero();
}

void zero(){
    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

int turnRight(int tcount, int tpercent)
{
    // reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // motor percents 
    right_motor.SetPercent(tpercent);
    left_motor.SetPercent(-tpercent);

    // keep running motors while average of turn counts is less then tcount
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < tcount);

    // turn off motors
    zero();



}

int turnLeft(int tcount, int tpercent)
{
    // reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // motor percents 
    right_motor.SetPercent(-tpercent);
    left_motor.SetPercent(tpercent);

    // keep running motors while average of turn counts is less then tcount
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < tcount);

    // turn off motors
    zero();



}

int main(){

    //wait for light to turn on
    while(CdS_cell.Value()<LIGHT_ON){}

     //move to ramp, 12'
    moveForward();
    
    //move up ramp, 12'
    moveForward();

    //move forward towards passport stamp, 8'
    moveForward();

    //90*
    turnLeft();

    //6'
    moveForward();

    //robot is now facing away from kiosk, 90*
    turnleft();

    //13'
    moveBackward();

    //while the switches are both unpressed
    while (micro_right.Value() && micro_left.Value())
    {
        moveBackward();
    }

    //16'
    moveForward();

    turnLeft();

    //4'
    moveForward();

    turnRight();

    //down ramp, 12'
    moveForward();

    //12'
    moveForward()
     
}
