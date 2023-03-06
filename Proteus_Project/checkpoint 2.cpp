#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRPS.h>


//Declarations for encoders & motors
<<<<<<< Updated upstream
AnalogInputPin CdS_cell (FEHIO::P1_0);
DigitalEncoder right_encoder(FEHIO::P0_0);
DigitalEncoder left_encoder(FEHIO::P3_0);
FEHMotor right_motor(FEHMotor::Motor1,9.0);
FEHMotor left_motor(FEHMotor::Motor0,9.0);
DigitalInputPin micro_left(FEHIO::P3_1);
DigitalInputPin micro_right(FEHIO::P0_1);
=======
AnalogInputPin CdS_cell (FEHIO::P0_1);
DigitalEncoder right_encoder(FEHIO::P1_0);
DigitalEncoder left_encoder(FEHIO::P1_1);
FEHMotor right_motor(FEHMotor::Motor1,9.0);
FEHMotor left_motor(FEHMotor::Motor0,9.0);
DigitalInputPin micro_left(FEHIO::P0_0);
DigitalInputPin micro_right(FEHIO::P3_0);
>>>>>>> Stashed changes

void turnLeft(int tcount, int tpercent);
void turnRight(int tcount, int tpercent);
void moveForward(int counts,int percent);
void moveBackward(int counts,int percent);
<<<<<<< Updated upstream
void turnOnlyLeft(int tcount, int tpercent);
void turnOnlyRight(int tcount, int tpercent);
=======
>>>>>>> Stashed changes
void zero();
void boardingPass(boolean red);


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

<<<<<<< Updated upstream
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

=======
>>>>>>> Stashed changes
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

void boardingPass(boolean red)
{
<<<<<<< Updated upstream
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

=======
    /* Robot starts on nose of the plane facing towards the, moves to press the correct button, and then stops when the button is pressed
    */
    if(red)
    {

    }
    else
    {

    }
>>>>>>> Stashed changes
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
    
    LCD.WriteLine("Moving Foward to Ramp");

     //move to ramp, 
    moveForward(550,percent);

    LCD.WriteLine("Moving Up Ramp")
    //move up ramp, 12'
    moveForward(600,upRampPercent);

<<<<<<< Updated upstream
    //line following to light at nose of plane
=======
    LCD.WriteLine("Moving Towards Pass Port Stamp");

    //move forward towards passport stamp, 8'
    moveForward(400,percent);

    LCD.Clear(BLACK);

    LCD.WriteLine("Turning Left");

    //90*
    turnLeft(tcount,tpercent);

    LCD.WriteLine("Moving Foward");

    //towards plane
    moveForward(500,percent);

    LCD.WriteLine("Turning Left");

    //robot is now facing away from kiosk after a 90* turn
    turnLeft(tcount,tpercent);

    LCD.WriteLine("Moving Backward");

    //get over plane nose to read value
    moveBackward(520,percent);
>>>>>>> Stashed changes

    //read value of CdS cell to determine boarding pass
    int val = CdS_Cell.Value();
    boolean red = (val<#####);

    boardingPass(red);
}