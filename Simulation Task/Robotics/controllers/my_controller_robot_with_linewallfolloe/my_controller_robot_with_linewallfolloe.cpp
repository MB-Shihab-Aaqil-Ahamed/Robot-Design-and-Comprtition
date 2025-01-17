#include <iostream>
#include <sstream>
#include <map>

#include <webots/Robot.hpp>
#include <webots/Motor.hpp>
#include <webots/DistanceSensor.hpp>
#include <webots/PositionSensor.hpp>

#define TIME_STEP 16

using namespace webots;

static double   last_error,error = 0;
static double   left_speed;
static double   right_speed;
static double P = 0;
static double I = 0;
static double D = 0;


double limit(double &val){
  if (val > 10){
    return 10;
  }else if(val < -10){
    return -10;
  }else{
    return val;
  }
}
Robot*robot = new Robot();
DistanceSensor *leftIR = robot->getDistanceSensor("ds_left");
DistanceSensor *midIR = robot->getDistanceSensor("ds_mid");
DistanceSensor *rightIR = robot->getDistanceSensor("ds_right");
DistanceSensor *rightMIR = robot->getDistanceSensor("ds_right_most");
DistanceSensor *leftMIR = robot->getDistanceSensor("ds_left_most");

//DistanceSensor *sensors[5]={leftIR,midIR,rightIR,rightMIR,leftMIR};

Motor *leftMotor1 = robot->getMotor("Motor_1");
Motor *leftMotor2 = robot->getMotor("Motor_2");
Motor *rightMotor1 = robot->getMotor("Motor_3");
Motor *rightMotor2 = robot->getMotor("Motor_4");



void lineFollow(double &kp,double &kd,double &ki,double &baseSpeed){ 
  if ((leftIR->getValue() >= 400) && (rightIR->getValue() >= 400) && (midIR->getValue() < 400)){
      error = 0;
    } else if ((leftMIR->getValue() >= 400) &&(leftIR->getValue() >= 400) && (rightIR->getValue() < 400) && (midIR->getValue() >= 400) && (rightMIR->getValue() >= 400)){
      error = -1;
    } else if ((leftMIR->getValue() >= 400) &&(leftIR->getValue() < 400) && (rightIR->getValue() >= 400) && (midIR->getValue() >= 400) && (rightMIR->getValue() >= 400)){
      error = 1;
    } else if ((leftMIR->getValue() >= 400) && (leftIR->getValue() >= 400) && (rightIR->getValue() >= 400) && (midIR->getValue() >= 400) && (rightMIR->getValue() < 400)){
      error = -2;
    } else if ((leftMIR->getValue() < 400) && (leftIR->getValue() >= 400) && (rightIR->getValue() >=  400) && (midIR->getValue() >= 400)&& (rightMIR->getValue() >= 400)){
      error = 2;
    } else if ((leftMIR->getValue() < 400) && (leftIR->getValue() < 400) && (rightIR->getValue() >=  400) && (midIR->getValue() < 400)&& (rightMIR->getValue() >= 400)){
      error = 0;
    }
    P = error;
    std::cout<<"p = "<<P<<std::endl;
    I = error + I;
    std::cout<<"I = "<<I<<std::endl;
    D = error - last_error;
    std::cout<<"D = "<<D<<std::endl;
    double balance = (kp*P)+(ki*I)+(kd*D);
    std::cout<<"balance = "<<balance<<std::endl;
    last_error = error;
    std::cout<<"last_error = "<<last_error<<std::endl;
    
    left_speed = baseSpeed + balance;
    std::cout<<"left_speed = "<<left_speed<<std::endl;
    
    right_speed = baseSpeed - balance;
    std::cout<<"right_speed = "<<right_speed<<std::endl; 
    
  double correction = ( kd*D + kp*P + ki*I );  
  left_speed = baseSpeed + correction;      
  right_speed = baseSpeed - correction;     
  std::cout<<"left_Most= "<<leftMIR->getValue()<<" left = "<<leftIR->getValue()<< "  mid = "<<midIR->getValue()<<"  right = "<<rightIR->getValue()<<"  right_Most = "<<rightMIR->getValue()<<std::endl;
  if (left_speed > baseSpeed){
      leftMotor1->setVelocity(left_speed);
      rightMotor1->setVelocity(0);
      leftMotor2->setVelocity(left_speed);
      rightMotor2->setVelocity(0);
    }
    
    if (right_speed >  baseSpeed){
      rightMotor1->setVelocity(left_speed);
      leftMotor1->setVelocity(0);
      rightMotor2->setVelocity(left_speed);
      leftMotor2->setVelocity(0);
    }
    
    if (right_speed < 0){
      leftMotor1->setVelocity(left_speed);
      rightMotor1->setVelocity(0);
      leftMotor2->setVelocity(left_speed);
      rightMotor2->setVelocity(0);
    }
    
    if (left_speed < 0){
      rightMotor1->setVelocity(left_speed);
      leftMotor1->setVelocity(0);
      rightMotor2->setVelocity(left_speed);
      leftMotor2->setVelocity(0);
    }
    
    if (left_speed ==  baseSpeed){
      leftMotor1->setVelocity(left_speed);
      rightMotor1->setVelocity(right_speed);
      leftMotor2->setVelocity(left_speed);
      rightMotor2->setVelocity(right_speed);
      
    }
  
  return;
}


int main(int argc, char **argv){
    int timeStep = (int)robot->getBasicTimeStep();
  
    leftMotor1->setPosition(INFINITY);
    leftMotor2->setPosition(INFINITY);
    rightMotor1->setPosition(INFINITY);
    rightMotor2->setPosition(INFINITY);
  

    leftMotor1->setVelocity(0.0);
    leftMotor2->setVelocity(0.0);
    rightMotor1->setVelocity(0.0);
    rightMotor2->setVelocity(0.0);

    leftIR->enable(TIME_STEP);
    midIR->enable(TIME_STEP);
    rightIR->enable(TIME_STEP);
    rightMIR->enable(TIME_STEP);
    leftMIR->enable(TIME_STEP);

    double kp         = 1.71;          // line following parameters
    double kd         = 1.5;
    double ki         = 0.0;
    double baseSpeed  = 6.5;

    while (robot->step(timeStep) != -1){

        
        lineFollow(kp,kd,ki,baseSpeed);

        

    }

    delete robot;
    return 0;


}

