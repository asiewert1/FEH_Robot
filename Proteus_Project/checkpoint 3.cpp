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

void setServoStart(){

    servo.SetPercent(15);

    while(micro_front.Value()){
    }

    servo.SetPercent(-15);
    Sleep(3000);
    servo.Stop();
}

int main(){

    RPS.InitializeTouchMenu();
    //setServoStart();

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

    moveForward(550,percent);

    turnLeft(120,percent);

    moveForward(100,percent);

    LCD.WriteLine("Getting correct lever");

    int correctLever= RPS.GetCorrectLever();

    LCD.WriteLine(correctLever);

    if(correctLever==0){
        //left
        moveForward(100,percent);
    }
    else if(correctLever==1){
        //middle
        moveForward(200,percent);
    }
    else{
        //right
        moveForward(300,percent);
    }

    turnLeft(tcount,tpercent);

    moveForward(100,percent);

    //stop at lever
    zero();

    LCD.WriteLine("Flipping Lever");
    //servo.setPercent(50);
    Sleep(2000);
    //servo.Stop();

    moveBackward(100,percent);
    zero();

    Sleep(5000);
    
    moveForward(100,percent);
    zero();

    //servo.setPercent(-15);
    //servo.Stop();

    //flip lever back
}