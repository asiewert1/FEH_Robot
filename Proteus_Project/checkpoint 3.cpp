#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRPS.h>

//Declarations for encoders & motors
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

void turnLeft(int tcount, int tpercent);
void turnRight(int tcount, int tpercent);
void moveForward(int counts,int percent);
void moveBackward(int counts,int percent);
void turnOnlyLeft(int tcount, int tpercent);
void turnOnlyRight(int tcount, int tpercent);
void zero();
void boardingPass(bool red);

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

void turnOnlyRight(int tcount, int tpercent){
    // reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // motor percents 
    
    left_motor.SetPercent(tpercent);

    // keep running motors while average of turn counts is less then tcount
    while(left_encoder.Counts() < tcount);

    // turn off motors
    zero();
}

void turnOnlyLeft(int tcount, int tpercent){
    // reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // motor percents 
    
    right_motor.SetPercent(tpercent);
    

    // keep running motors while average of turn counts is less then tcount
    while(right_encoder.Counts() < tcount);

    // turn off motors
    zero();
}
void print_CdS()
{
    while(true)
    {
        LCD.WriteAt(CdS_cell.Value(), 240, 20);
        LCD.Clear();
    }
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

void boardingPass(bool red)
{
    /* Robot starts on nose of the plane facing towards the ramps,
    * moves to press the correct button, and then stops when the
    * button is pressed
    */

    //90 deg
    turnLeft(225);

    //move in front of respective button
    if(red) 
    {
        moveForward(300,percent);
    }
    else
    {
        moveForward(500,percent);
    }

    //now back is facing kiosk, 90*
    turnRight(225);
    
    //while the switches are both unpressed back up into ticket booth
    while (micro_right.Value() && micro_left.Value())
    {
        //move backward
        right_motor.SetPercent(20);
        left_motor.SetPercent(20);
    }

    //return to same point
    if(red) 
    {
        moveForward();
    }
    else
    {
        moveForward();
    }


}

void followLine(){
    int state = LINE_ON_LEFT; // Set the initial state

    LCD.Clear();

    LCD.WriteAt(rightOpt.Value(), 0, 0);
    LCD.WriteAt(middleOpt.Value(), 0, 20);
    LCD.WriteAt(leftOpt.Value(), 0, 40);

    while (CdS_cell.Value()>1&& micro_left.Value() && micro_right.Value()){
        //while not over the light and the microswitches are unpressed
        switch(state) {
            case LINE_ON_RIGHT:
            //right turn
                right_motor.SetPercent(STRAIGHT); //goes slower
                left_motor.SetPercent(FIX);
                LCD.Clear();
                LCD.WriteAt("Line on right", 0, 60);
        
            /* Drive */

            if (leftOpt.Value()<2.8) {

                state = LINE_ON_LEFT; // update a new state
            }

            /* Code for if left sensor is on the line */

            break;
            
            // If the left sensor is on the line...
            case LINE_ON_LEFT:

            //turn left
            left_motor.SetPercent(STRAIGHT);
            right_motor.SetPercent(FIX);
            LCD.Clear();
            LCD.WriteAt("Line on left", 0, 60);

            if(rightOpt.Value()<2.8) 
            {   //sees line w left
                state = LINE_ON_RIGHT;
            }
            
            break;

            default: // Error. Something is very wrong.
                zero();
            break;
        }
    }

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
    int tpercent=-25;

    int upRampPercent= -40;
    int percent= -25;

    LCD.Clear(BLACK);

    //wait for light to turn on

    LCD.WriteLine("Waiting for Light");

    
    while(CdS_cell.Value()>2){}

    LCD.WriteLine("Light Seen");

    moveForward(200,percent);

    turnOnlyLeft(50,percent);

    moveForward(300,percent);

    LCD.WriteLine("Getting correct lever");

    int correctLever= RPS.GetCorrectLetter();

    if(correctLever==0){
        //left
        moveForward(100,percent);
    }
    else if(correctLever==1){
        //middle
        moveForward(250,percent);
    }
    else{
        //right
        moveForward(400,percent);
    }

    turnLeft(tcount,tpercent);

    moveForward(100,percent);

    //stop at lever
    zero();

    //flip lever

    Sleep(5200);

    //flip lever back
}