#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRPS.h>

#define RPS_WAIT_TIME_IN_SEC 0.35

#define PULSE_POWER -20
#define PULSE_TIME 0.15

#define X_Light 12.0
#define Y_Light 59.2

#define X_Passport 18.2
#define X_Finish 12.4

#define HEADING_TOLERANCE 1.0

/* Direction along axis which robot is traveling
Examples:
	- if robot is traveling to the upper level, that is a PLUS as the y-coordinate is increasing
	- if robot is traveling to the lower level, that is a MINUS as the y-coordinate is decreasing
*/
#define PLUS 0
#define MINUS 1


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
FEHMotor servo(FEHMotor::Motor3,9.0);
DigitalInputPin micro_front(FEHIO::P1_1);

void turnLeft(int tcount, int tpercent);
void turnRight(int tcount, int tpercent);
void moveForward(int counts,int percent);
void moveBackward(int counts,int percent);
void turnOnlyLeft(int tcount, int tpercent);
void turnOnlyRight(int tcount, int tpercent);
void zero();
void boardingPass(bool red);
void pulse_forward(int percent, float seconds);
void check_x(float x_coordinate, int orientation);
void check_y(float y_coordinate, int orientation);

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

/*
* Pulse forward a short distance using time
*/
void pulse_forward(int percent, float seconds)
{
    // Set both motors to desired percent
    right_motor.SetPercent(percent-4);
    left_motor.SetPercent(percent);

    // Wait for the correct number of seconds
    Sleep(seconds);

    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

/*
* Use RPS to move to the desired x_coordinate based on the orientation of the QR code
*/
void check_x(float x_coordinate, int orientation)
{
    // Determine the direction of the motors based on the orientation of the QR code
    int power = PULSE_POWER;
    if (orientation == MINUS)
    {
        power = -PULSE_POWER;
    }

    // Check if receiving proper RPS coordinates and whether the robot is within an acceptable range
    while (RPS.X() > -1 && (RPS.X() < x_coordinate - .5 || RPS.X() > x_coordinate + .5))
    {
        LCD.WriteLine(RPS.X());
        if (RPS.X() < x_coordinate - .5)
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_forward(power, PULSE_TIME);
        }
        else if (RPS.X() > x_coordinate+.5)
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_forward(-power, PULSE_TIME);
        }
        Sleep(RPS_WAIT_TIME_IN_SEC);
    }
}

/*
* Use RPS to move to the desired y_coordinate based on the orientation of the QR code
*/
void check_y(float y_coordinate, int orientation)
{
    // Determine the direction of the motors based on the orientation of the QR code
    int power = PULSE_POWER;
    if (orientation == MINUS)
    {
        power = -PULSE_POWER;
    }

    // Check if receiving proper RPS coordinates and whether the robot is within an acceptable range
    while (RPS.Y() > -1 && (RPS.Y() < y_coordinate - .5 || RPS.Y() > y_coordinate + .5))
    {   
        LCD.WriteLine(RPS.Y());
        if (RPS.Y() < y_coordinate - .5)
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_forward(power, PULSE_TIME);
        }
        else if (RPS.Y() > y_coordinate+.5)
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_forward(-power, PULSE_TIME);
        }
        Sleep(RPS_WAIT_TIME_IN_SEC);
    }
}
/*
* Pulse counterclockwise a short distance using time
*/
void pulse_counterclockwise(int percent, float seconds)
{
    // Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(-percent);

    // Wait for the correct number of seconds
    Sleep(seconds);

    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();

    Sleep(RPS_WAIT_TIME_IN_SEC);
}

void check_heading(float heading)
{
    // If the robot is at the desired heading(
    if ((RPS.Heading() - heading > -HEADING_TOLERANCE) && (RPS.Heading() - heading < HEADING_TOLERANCE))
    {
        LCD.WriteLine("Robot at correct heading");
    }
    
    while(RPS.Heading() - heading < -HEADING_TOLERANCE||RPS.Heading() - heading > HEADING_TOLERANCE){
        
        if(RPS.Heading()>355)
        {
            LCD.WriteLine("Done");
            break;
        }

        // If the robot is more than two (HEADING_TOLERANCE) to the right
        if(RPS.Heading() - heading < -HEADING_TOLERANCE)
        {
            LCD.WriteLine("pulsing left");
            pulse_counterclockwise(PULSE_POWER,PULSE_TIME/2);
        
        }

        // If more than two (HEADING TOLERANCE) to the left
        else if(RPS.Heading() - heading > HEADING_TOLERANCE)
        {
            LCD.WriteLine("pulsing right");
            pulse_counterclockwise(-PULSE_POWER,PULSE_TIME/2);
        }
        else{
            LCD.WriteLine("correct");
        }
        
    }
    /*
        SUGGESTED ALGORITHM:
        1. Check the current orientation of the QR code and the desired orientation of the QR code
        2. Check if the robot is within the desired threshold for the heading based on the orientation
        3. Pulse in the correct direction based on the orientation
    */
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

void setServoStart(){

    servo.SetPercent(-25);

    while(micro_front.Value()){
    }

    servo.SetPercent(40);
    Sleep(4200);
    servo.Stop();
}

int main(){

    RPS.InitializeTouchMenu();
    setServoStart();

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

    /*
    * LUGGAGE
    */

    //is adjusting to be facing ramp
    LCD.WriteLine("Turning to face Ramp");
    turnOnlyRight(415,tpercent);
    moveForward(80,percent);
    turnOnlyLeft(135,tpercent);

    LCD.WriteLine("Moving Foward to Ramp");

    //move to ramp, 
    moveForward(330,percent);

    LCD.WriteLine("Moving Up Ramp");
    //move up ramp, 12'
    moveForward(600,upRampPercent);

    moveForward(330,percent);

    turnLeft(290,percent);

    //run into wall
    LCD.WriteLine("Running into Wall");
    left_motor.SetPercent(40);
    right_motor.SetPercent(40);
    Sleep(1500);
    zero();

    moveForward(500,percent);

    turnLeft(270,percent);

    moveForward(40,percent);

    //depositing luggage
    servo.SetPercent(-25);

    while(micro_front.Value()){
    }
    servo.Stop();

    /*
    * BOARDING PASS
    */

    moveBackward(50,percent);

    turnRight(265,percent);

    moveForward(160,percent);

    check_x(X_Light,MINUS);

    turnRight(260,percent);

    check_heading(270);

    moveForward(340,percent);

    check_y(Y_Light,PLUS);

    LCD.WriteLine("Getting Light Value");
    //get value of light
    float val= CdS_cell.Value();

    if(val<.5){
        //red
        LCD.WriteLine("Red");
    }
    else{
        LCD.WriteLine("Blue");
    }

    moveBackward(360,percent);

    turnRight(270,percent);

    LCD.WriteLine("Aligning with wall");
    //run into wall to align
    moveBackward(500,percent-10);

    if(val<.5){
        //red
        LCD.WriteLine("Moving Towards Red Button");
        moveForward(830,percent);
    }
    else
    {   //blue
        LCD.WriteLine("Moving Towards Blue Button");
        moveForward(650,percent);
    }

    //back is facing kiosk
    turnRight(260,percent);

    LCD.WriteLine("Backing into button");
    //move backward into the wall to hit the button
    left_motor.SetPercent(40);
    right_motor.SetPercent(40);
    Sleep(4000);

    zero();

    /*
    * PASSPORT
    */ 

    LCD.WriteLine("Moving to Passport Arm");
    //get to passport arm
    if(val<.5){
        //red
        moveForward(450,percent);
        turnOnlyRight(520,-percent);
        moveBackward(100,percent);
        check_x(X_Passport,PLUS);
    }
    else
    {   //blue
        moveForward(330,percent);
        turnLeft(270,percent);
        moveForward(80,percent);
        check_x(X_Passport,PLUS);
    }

    LCD.WriteLine("Flipping passport");
    servo.SetPercent(40);
    Sleep(3200);
    servo.Stop();

    //turn after arm goes up
    turnLeft(75,-15);

    moveForward(65,percent);

    moveBackward(50,percent);

    turnRight(200,-15);

    moveBackward(250,percent);

    turnLeft(165,percent);
    moveForward(255,percent);

    //return passport to down position
    LCD.WriteLine("Returning Passport to down position");
    turnRight(100,percent);

    /*
    * MOVE TO FUEL LEVER
    */

    LCD.WriteLine("Moving to Fuel Levers");
    turnLeft(60,percent);

    moveBackward(200,percent-10);

    //facing fuel levers
    turnRight(260,percent);

    moveForward(300,percent);

    turnLeft(260,percent);

    //back into wall for alignment
    moveBackward(600,percent-10);

    moveForward(75,percent);

    turnRight(260,percent);

    //go down ramp
    LCD.WriteLine("Going down ramp");
    moveForward(900,percent);

    /*
    * FUEL LEVER
    */

    LCD.WriteLine("Getting correct lever:");
    int correctLever= RPS.GetCorrectLever();

    LCD.WriteLine(correctLever);

    //get drawbridge in correct positon
    LCD.WriteLine("Putting drawbridge in correct position");
    servo.SetPercent(-50);
    Sleep(1000);
    servo.Stop();

    //position robot at correct lever
    LCD.WriteLine("Moving to correct fuel lever");
    if(correctLever==0){
        //left, A
        turnLeft(250,percent);
        moveForward(175,percent);
        Sleep(1000);
        turnRight(250,tpercent);
        moveBackward(60,percent);
    }
    else if(correctLever==1){
        //middle, A1
        moveForward(150,percent);
        Sleep(1000);
        turnRight(250,tpercent);
        moveBackward(20,percent);
    }
    else{
        //right, B
        moveForward(130,percent);
    }

    //stop at lever
    zero();  

    LCD.WriteLine("Flipping Lever");
    servo.SetPercent(-50);
    Sleep(1000);
    servo.Stop();

    moveBackward(100,percent);
    zero();

    LCD.WriteLine("Waiting 5 Seconds");

    //moves to down position
    servo.SetPercent(-50);
    while(micro_front.Value()){}
    servo.Stop();

    Sleep(5000);
    
    moveForward(100,percent);
    zero();

    //lifts lever up
    LCD.WriteLine("lifting lever up");
    servo.SetPercent(30);
    Sleep(2100);

    servo.Stop();
    zero();

    moveBackward(100,percent);

    turnLeft(260,percent);

    if(correctLever==0){
        //left, A
    }
    else if(correctLever==1){
        //middle, A1
        moveForward(200,percent);
    }
    else{
        //right, B
        moveForward(300,percent);
    }

    LCD.WriteLine("Getting to common position");
    check_x(X_Finish,PLUS);

    turnRight(125,percent);

    servo.SetPercent(-25);
    Sleep(1000);

    servo.Stop();
   
   LCD.WriteLine("Running into final button");
    moveForward(800,percent-10);
}