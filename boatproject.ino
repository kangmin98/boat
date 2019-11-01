#include <SoftwareSerial.h>
#include <Servo.h>
#include <NewPing.h>

#define MAX_DISTANCE 400 

NewPing sonarR(A5, A4, MAX_DISTANCE); 
NewPing sonarF(A3, A2, MAX_DISTANCE);
NewPing sonarL(A1, A0, MAX_DISTANCE);

SoftwareSerial mySerial(11, 12); // RX, TX

Servo smoter;

int moter1 = 6;
int moter2 = 4;

int directionAngle;
int directionAngleC;

int F, L, R;

char c[20];
int data[20];
int yaw;

//메소드
void rudder(int angle){
  if(angle > 40){
    angle = 40;
  }else if(angle < -40){
    angle = -40;
  }
  int a = angle + 100;
  smoter.write(-a);
}
void go(int directionAngle, int currentAngle){
  int delta = directionAngle-currentAngle;
  if(delta > 180){
    delta = delta - 360;
  }else if(delta < -180){
    delta = delta + 360;
  }

  rudder(delta);
}
int yawC(){
  int i = 0;
  while (mySerial.available()) {
    c[i] = mySerial.read();
    data[i] = int(c[i]);
    i++;
  }
  int yaw = data[6]*256+data[7]+50;
  yaw = yaw/100;
  return yaw;
}


//시작
void setup() {
  smoter.attach(5);
  rudder(0);

  delay(500);

  Serial.begin(115200);
  mySerial.begin(115200);

  directionAngle = yawC();
  directionAngleC = directionAngle;

  delay(500);
  
  pinMode(moter1, OUTPUT);
  pinMode(moter2, OUTPUT);  
  analogWrite(moter1, 25);
  analogWrite(moter2, 0);  
}
  

void loop() {
  if(directionAngleC == directionAngle){    //앞
    F=sonarF.ping_cm();
    Serial.print("F");
    Serial.println(F);
    if(F < 200 && F > 10){
      L = sonarL.ping_cm();
      Serial.print("L");
      Serial.println(L);
      R = sonarR.ping_cm();
      Serial.print("R");
      Serial.println(R);
      if(L > 200 || L < 10){
        Serial.println("LLLLLLLLLLL");
        directionAngleC = directionAngleC - 90;
        while(yawC() > directionAngleC){
          go(directionAngleC, yawC());
          delay(500);
        }
      }else if(R > 200 || R < 10){
        Serial.println("RRRRRRRRR");
        directionAngleC = directionAngleC + 90;
        while(yawC() < directionAngleC){
          go(directionAngleC, yawC());
          delay(500);
        }
      }else{
        analogWrite(moter1, 0);
        analogWrite(moter2, 0);
      }    
    }
  }else if(directionAngleC == directionAngle - 90){ //왼쪽 
    R=sonarR.ping_cm();
    if(R > 200){             
      directionAngleC = directionAngleC + 90;  
      Serial.println("FFFFFFFFFFF");   
      while(yawC() < directionAngleC){
        go(directionAngleC, yawC());
        delay(500);
      }   
    }
  }else if(directionAngleC == directionAngle + 90){ //오른쪽 
    L=sonarL.ping_cm();
    if(L > 200){      
      directionAngleC = directionAngleC - 90; 
      Serial.println("FFFFFFFFFFFFFFFFFFF");
      while(yawC() > directionAngleC){
          go(directionAngleC, yawC());
          delay(500);
      }      
    }
  }
  yaw=yawC();
  go(directionAngleC, yaw);
  Serial.println(yaw);
  delay(100);
  analogWrite(moter1, 25);
  analogWrite(moter2, 0);
}
