#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRPS.h>

//for line following
#define STRAIGHT 15
#define FIX 10

//coordinates for nose of plane light
#define NOSE_X 
#define NOSE_Y

enum LineStates
{
    MIDDLE,
    RIGHT,
    LEFT
};

//Declarations for encoders & motors
AnalogInputPin CdS_cell (FEHIO::P1_0);
DigitalEncoder right_encoder(FEHIO::P0_0);
DigitalEncoder left_encoder(FEHIO::P3_0);
FEHMotor right_motor(FEHMotor::Motor1,9.0);
FEHMotor left_motor(FEHMotor::Motor0,9.0);
DigitalInputPin micro_left(FEHIO::P3_1);
DigitalInputPin micro_right(FEHIO::P0_1);
AnalogInputPin rightOpt(FEHIO::P2_0);
AnalogInputPin leftOpt(FEHIO::P2_2);
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
    turnLeft();

    //move in front of respective button
    if(red) 
    {
        moveForward();
    }
    else
    {
        moveForward();
    }

    //now back is facing kiosk
    turnRight();
    
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

    /* Robot starts on nose of the plane facing towards the, moves to press the correct button, and then stops when the button is pressed
    */
    if(red)
    {

    }
    else
    {

    }
}

void followLine(){
    int state = RIGHT; // Set the initial state

    LCD.Clear();

    LCD.WriteAt(rightOpt.Value(), 0, 0);
    LCD.WriteAt(middleOpt.Value(), 0, 20);
    LCD.WriteAt(leftOpt.Value(), 0, 40);

    switch(state) {
        case MIDDLE:
            right_motor.SetPercent(STRAIGHT);
            left_motor.SetPercent(STRAIGHT);
            LCD.WriteAt("Straight", 0, 60);
    
        /* Drive */

        if (rightOpt.Value()>2.1 ) {

            state = RIGHT; // update a new state
        }
        /* Code for if left sensor is on the line */

        if (leftOpt.Value()>2.1) {

            state = LEFT; // update a new state
        }

        break;
        
        case RIGHT:
        
            left_motor.SetPercent(STRAIGHT);
            right_motor.SetPercent(FIX);
            LCD.WriteAt("RIGHT", 0, 60);

        /* Drive */

        if(middleOpt.Value()>2.1 ) 
        {
            
            state = MIDDLE;
        }
        
        break;
        
        // If the left sensor is on the line...
        case LEFT:

        left_motor.SetPercent(FIX);
        right_motor.SetPercent(STRAIGHT);
        LCD.WriteAt("LEFT", 0, 60);

        if(middleOpt.Value()>2.1) 
        {
            
            state = MIDDLE;
        }
        /* Mirror operation of RIGHT state */
        break;
        
        default: // Error. Something is very wrong.
            left_motor.SetPercent(0);
            right_motor.SetPercent(0);
        break;

    }

    // Sleep a bit 
    Sleep(500);
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

    LCD.Clear(BLACK);

    //wait for light to turn on

    LCD.WriteLine("Waiting for Light");

    
    while(CdS_cell.Value()>2){}

    LCD.WriteLine("Light Seen");

    //is adjusting to be facing ramp
    LCD.WriteLine("Turning to face Ramp");
    turnOnlyRight(415,tpercent);
    moveForward(80,percent);
    turnOnlyLeft(135,tpercent);

    LCD.WriteLine("Moving Foward to Ramp");

     //move to ramp, 
    moveForward(330,percent);

    LCD.WriteLine("Moving Up Ramp")
    //move up ramp, 12'
    moveForward(600,upRampPercent);

    //line following to light at nose of plane
    while (CdS_Cell.Value()>####)
    {
        followLine();
    }
    
    //read value of CdS cell to determine boarding pass
    int val = CdS_Cell.Value();
    bool red = (val<#####);

    boardingPass(red);
}