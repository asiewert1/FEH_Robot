#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRPS.h>

//for line following
#define STRAIGHT 22
#define FIX 10

//line following states
#define LINE_ON_RIGHT 0
#define LINE_ON_LEFT 1

//coordinate of the light
#define NOSE_X
#define NOSE_Y

// RPS Delay time
#define RPS_WAIT_TIME_IN_SEC 0.35

// Shaft encoding counts for CrayolaBots
#define COUNTS_PER_INCH 40.5
#define COUNTS_PER_DEGREE 2.48

/* Defines for how long each pulse should be and at what motor power. 
These value will normally be small, but you should play around with the values to find what works best */
#define PULSE_TIME 0.25
#define PULSE_POWER 20.0

// Define for the motor power while driving (not pulsing)
#define POWER 20.0

#define HEADING_TOLERANCE 2

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

void turnLeft(int tcount, int tpercent);
void turnRight(int tcount, int tpercent);
void moveForward(int counts,int percent);
void moveBackward(int counts,int percent);
void turnOnlyLeft(int tcount, int tpercent);
void turnOnlyRight(int tcount, int tpercent);
void zero();
void boardingPass(bool red);

class RPS_Methods{

    private:

    public:

    /*
    * Pulse forward a short distance using time
    */
    void pulse_forward(int percent, float seconds)
    {
        // Set both motors to desired percent
        right_motor.SetPercent(percent);
        left_motor.SetPercent(percent);

        // Wait for the correct number of seconds
        Sleep(seconds);

        // Turn off motors
        right_motor.Stop();
        left_motor.Stop();
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
        while (RPS.X() > -1 && (RPS.X() < x_coordinate - 1 || RPS.X() > x_coordinate + 1))
        {
            if (RPS.X() < x_coordinate - 1)
            {
                // Pulse the motors for a short duration in the correct direction
                pulse_forward(power, PULSE_TIME);
            }
            else if (RPS.X() > x_coordinate+1)
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
        while (RPS.Y() > -1 && (RPS.Y() < y_coordinate - 1 || RPS.Y() > y_coordinate + 1))
        {
            if (RPS.Y() < y_coordinate - 1)
            {
                // Pulse the motors for a short duration in the correct direction
                pulse_forward(power, PULSE_TIME);
            }
            else if (RPS.Y() > y_coordinate+1)
            {
                // Pulse the motors for a short duration in the correct direction
                pulse_forward(-power, PULSE_TIME);
            }
            Sleep(RPS_WAIT_TIME_IN_SEC);
        }
    }


    /*
    * Use RPS to move to the desired heading
    */
    void check_heading(float heading)
    {
        // If the robot is at the desired heading(
        if ((RPS.Heading() - heading > -HEADING_TOLERANCE) && (RPS.Heading() - heading < HEADING_TOLERANCE))
        {
            LCD.WriteLine("Robot at correct heading");
        }
        // If the robot is more than two (HEADING_TOLERANCE) to the right
        while (RPS.Heading() - heading < -HEADING_TOLERANCE)
        {
            right_motor.SetPercent(PULSE_POWER);
            Sleep(PULSE_TIME);
        }

        // If more than two (HEADING TOLERANCE) to the left
        while (RPS.Heading() - heading > HEADING_TOLERANCE)
        {
            left_motor.SetPercent(PULSE_POWER);
            Sleep(PULSE_TIME);
        }

        // You will need to fill out this one yourself and take into account
        // checking for proper RPS data and the edge conditions
        //(when you want the robot to go to 0 degrees or close to 0 degrees)
        

        /*
            SUGGESTED ALGORITHM:
            1. Check the current orientation of the QR code and the desired orientation of the QR code
            2. Check if the robot is within the desired threshold for the heading based on the orientation
            3. Pulse in the correct direction based on the orientation
        */
    }

    int go()
    {

        // COMPLETE CODE HERE TO READ SD CARD FOR LOGGED X AND Y DATA POINTS
        FEHFile *fptr = SD.FOpen("RPS_POIs.txt", "r");
        SD.FScanf(fptr, "%f%f", &A_x, &A_y);


        SD.FClose(fptr);

        turn_90_counts = 240;
        turn_180_counts = 480;

        
        // Open file pointer for writing
        fptr = SD.FOpen("RESULTS.txt", "w");
        
        // COMPLETE CODE HERE TO WRITE EXPECTED AND ACTUAL POSITION INFORMATION TO SD CARD
        SD.FPrintf(fptr, "Expected A Position: %f %f %f\n", A_x,A_y,A_heading);
        SD.FPrintf(fptr, "Actual A Position:   %f %f %f\n\n", RPS.X(), RPS.Y(),RPS.Heading());

        // B --> C
        check_x(C_x, MINUS);
        check_y();
        check_heading();
        Sleep(1.0);

        // Close file pointer
        SD.FClose(fptr);

        return 0;
    }
}


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

    
    moveForward(300,percent);

    //180 turn
    turnRight(350,percent);

    //line following to light at nose of plane
    followLine();

    RPS_Methods position;
    position.check_x();
    position.check_y();
    position.check_heading();

    //read value of CdS cell to determine boarding pass
    float val = CdS_cell.Value();
    bool red = (val<.55);

    if(red){
        LCD.Clear(RED);
    }
    else{
        LCD.Clear(BLUE);
    }

    boardingPass(red);
}