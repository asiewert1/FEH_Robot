#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRPS.h>


//Declarations for encoders & motors
AnalogInputPin CdS_cell (FEHIO::P0_1);
DigitalEncoder right_encoder(FEHIO::P1_0);
DigitalEncoder left_encoder(FEHIO::P1_1);
FEHMotor right_motor(FEHMotor::Motor1,9.0);
FEHMotor left_motor(FEHMotor::Motor0,9.0);
DigitalInputPin micro_left(FEHIO::P0_0);
DigitalInputPin micro_right(FEHIO::P3_0);

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

void turnRight(int tcount, int tpercent)
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

void turnLeft(int tcount, int tpercent)
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

int main(){

    float x, y; //for touch screen

    //Initialize the screen
    LCD.Clear(BLACK);
    LCD.SetFontColor(WHITE);

    LCD.WriteLine("Test");
    LCD.WriteLine("Touch the screen");
    while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
    while(LCD.Touch(&x,&y)); //Wait for screen to be unpressed  


    //count for turns. will turn 90 deg
    int tcount= 225;
    int tpercent=25;

    int upRampPercent= 40;
    int percent= 25;


    //wait for light to turn on
    while(CdS_cell.Value()>2){}

     //move to ramp, 
    moveForward(550,percent);
    
    //move up ramp, 12'
    moveForward(600,upRampPercent);

    //move forward towards passport stamp, 8'
    moveForward(400,percent);

    //90*
    turnLeft(tcount,tpercent);

    //6'
    moveForward(300,percent);

    //robot is now facing away from kiosk after a 90* turn
    turnLeft(tcount,tpercent);

    //13'
    moveBackward(520,percent);

    //while the switches are both unpressed
    while (micro_right.Value() && micro_left.Value())
    {
        //move backward
        right_motor.SetPercent(-20);
        left_motor.SetPercent(-20);
    }

    //16'
    moveForward(640,percent);

    //90*
    turnLeft(tcount,tpercent);

    //4'
    moveForward(330,percent);

    //90*
    turnRight(tcount,tpercent);

    //down ramp, 12'
    moveForward(480,percent);

    //12'
    moveForward(620,percent);
     
}