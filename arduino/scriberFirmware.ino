#include <AccelStepper.h>

int yLength;
int xLength;
const int xOptL = 11;
const int xOptR = 10;
const int yOpt = 12;
const int xAxisStep = 4;
const int xAxisDir = 5;
const int yAxisStep = 2;
const int yAxisDir = 3;
const int relayR = 9;
const int relayL = 8;
const int drop = 6;
const int xAxisSleep = 7;
const int yAxisSleep = 13;
AccelStepper yAxis(1, yAxisStep, yAxisDir);
AccelStepper xAxis(1, xAxisStep, xAxisDir);
bool dropped = false;
bool moveX = true;
bool moveY = true;
bool mLed = true;
unsigned long time = millis();

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(5);
  pinMode(yOpt, INPUT);
  pinMode(xOptL, INPUT);
  pinMode(xOptR, INPUT);
  pinMode(drop, INPUT);
  pinMode(relayR, OUTPUT);
  pinMode(relayL, OUTPUT);
  pinMode(xAxisSleep, OUTPUT);
  pinMode(yAxisSleep, OUTPUT);
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  analogWrite(A1, 512);
  xAxis.setMaxSpeed(500);
  yAxis.setMaxSpeed(500);
  yAxis.setAcceleration(60);
  xAxis.setAcceleration(60);
  xAxis.setMinPulseWidth(300);
  yAxis.setMinPulseWidth(300);
  digitalWrite(relayR, LOW);
  digitalWrite(relayL, LOW);
  digitalWrite(xAxisSleep, LOW);
  digitalWrite(yAxisSleep, LOW);
  Serial.println("connected()");
}

void loop() {
  if (digitalRead(drop)){
    if(!dropped){
      dropped = true;
      digitalWrite(relayR, LOW);
      digitalWrite(relayL, LOW);
      yAxis.stop();
      Serial.println("dropped()");
    }
  }
  if(moveX || moveY){
    if(millis() - time >= 250){
    if(mLed){
      analogWrite(A1, 512);
      mLed = false;
    }else{
      analogWrite(A1, 0);
      mLed = true;
    }
    time = millis();
    }
  }else{
    analogWrite(A1, 512);
  }
  if(xAxis.distanceToGo() == 0){
    if(moveX){
      Serial.print("stop(x, ");
      Serial.print(xAxis.currentPosition());
      Serial.println(")");
      moveX = false;
      digitalWrite(xAxisSleep, LOW);
    }
  }
  if(yAxis.distanceToGo() == 0){
    if(moveY){
      digitalWrite(relayR, LOW);
      digitalWrite(relayL, LOW);
      Serial.print("stop(y, "); 
      Serial.print(yAxis.currentPosition());
      Serial.println(")");
      moveY = false;
      digitalWrite(yAxisSleep, LOW);
    }
  }
  xAxis.run();
  yAxis.run();
}

void calibrate(){
  digitalWrite(xAxisSleep, HIGH);
  digitalWrite(yAxisSleep, HIGH);
  delay(2);
  moveX = true;
  moveY = true;
  //move out of start
  yAxis.setSpeed(200);
  xAxis.setSpeed(400);
  while(digitalRead(yOpt) == HIGH){
    yAxis.runSpeed();
  }
  //move to stop
  while(digitalRead(yOpt) == LOW && digitalRead(xOptL) == LOW){
    yAxis.runSpeed();
    xAxis.runSpeed();
  }
  if(digitalRead(yOpt) == LOW){
    while(digitalRead(yOpt) == LOW){
      yAxis.runSpeed();
    } 
  }else{
    while(digitalRead(xOptL) == LOW){
      xAxis.runSpeed();
    } 
  }
  //save end position as Zero
  yAxis.setCurrentPosition(0);
  xAxis.setCurrentPosition(0);
  //go back to find length
  //get out of end sensor
  yAxis.moveTo(-50);
  while(yAxis.distanceToGo() != 0){
    yAxis.run();
  }
  //move to the start sensor
  yAxis.setSpeed(-200);
  xAxis.setSpeed(-400);
  while(digitalRead(yOpt) == LOW && digitalRead(xOptR) == LOW){
    yAxis.runSpeed();
    xAxis.runSpeed();
  }
  if(digitalRead(yOpt) == LOW){
    while(digitalRead(yOpt) == LOW){
      yAxis.runSpeed();
    }
  }else{
    while(digitalRead(xOptR) == LOW){
      xAxis.runSpeed();
    }
  }
  //get length
  yLength = -yAxis.currentPosition();
  xLength = -xAxis.currentPosition();
  mLed = true;
  Serial.print("calibrated(");
  Serial.print(xLength);
  Serial.print(", ");
  Serial.print(yLength);
  Serial.println(")");
  xAxis.moveTo(-xLength/2);
  yAxis.moveTo(-yLength);
}

void serialEvent(){
  String in = Serial.readString();
  parse(in);
}

void parse(String in){
  if(in.length() > 0){
    if(in.charAt(in.length() - 1) == '\n'){
      int pos = 0;
      int prev = 0;
      while(in.charAt(pos) != '('){
          pos++;
      }
      int comm = pos;
      prev = pos + 1;
      pos++;
      while(in.charAt(pos) != ',' && in.charAt(pos) != ')'){
          pos++;
      }
      int paramsN = in.substring(prev, pos).toInt() + 1;
      prev = pos + 1;
      pos++;
      String command[paramsN];
      command[0] = in.substring(0, comm);
      for(int i = 1; i < paramsN; i++){
        if(in.charAt(pos) == ' '){
          prev ++;
        }
        while(in.charAt(pos) != ',' && in.charAt(pos) != ')'){
          pos++;
        }
        command[i] = in.substring(prev, pos);
        prev = pos + 1;
        pos++;
      }
      interpretate(command);
      if(in.charAt(pos) == '\n'){
        parse(in.substring(pos+1, in.length()));
      }
    }else{
      Serial.print("incorrectCommand(\"");
      Serial.print(in);
      Serial.println("\")");
    }
  }
}

void interpretate(String command[]){
  if(command[0].equals("move")){
    int coord = command[2].toInt();
    if(command[1].equals("x")){
      if(coord >= -xLength && coord <= 0){
        xAxis.moveTo(coord);
        moveX = true;
        digitalWrite(xAxisSleep, HIGH);
        delay(1);
        Serial.print("moving(x, ");
        Serial.print(coord);
        Serial.println(")");
      }else{
        Serial.print("incorrectCoordX(");
        Serial.print(xLength);
        Serial.print(", ");
        Serial.print(command[2]);
        Serial.println(")");
      } 
    }else{
      if(coord >= -yLength && coord <= 0){
        yAxis.moveTo(coord);
        moveY = true;
        digitalWrite(yAxisSleep, HIGH);
        delay(1);
        Serial.print("moving(y, ");
        Serial.print(coord);
        Serial.println(")");
      }else{
        Serial.print("incorrectCoordY(");
        Serial.print(yLength);
        Serial.print(", ");
        Serial.print(command[2]);
        Serial.println(")");
      }
    }  
  }else{
    if(command[0].equals("stop")){
      if(command[1].equals("x")){
        xAxis.stop();  
      }else{
        yAxis.stop();
      }
    }else{
      if(command[0].equals("land")){
        analogWrite(A0, 512);
        if(command[1].equals("r")){
          digitalWrite(relayR, HIGH);
        }else{
          digitalWrite(relayL, HIGH);
        }
        delay(100);
        dropped = false;
      }else{
        if(command[0].equals("rise")){
          analogWrite(A0, 0);
          if(command[1].equals("r")){
            digitalWrite(relayR, LOW);
          }else{
            digitalWrite(relayL, LOW);
          }
        }else{
            if(command[0].equals("calibrate")){
              calibrate();      
            }else{
              if(command[0].equals("ping")){
                Serial.println("connected()");
                digitalWrite(relayR, LOW);
                digitalWrite(relayL, LOW);
                xAxis.stop();
                yAxis.stop();
                dropped = false;
                moveX = false;
                moveY = false;
              }else{
                if(command[0].equals("speed")){
                  int speed = command[2].toInt();
                  if(command[1].equals("x")){
                    xAxis.setMaxSpeed(speed);
                  }else{
                    yAxis.setMaxSpeed(speed);
                  }
                }else{
                  Serial.print("incorrectCommand(");
                  Serial.print(command[0]);
                  Serial.print(")");
                }
              
            }
          }
        }
      }
    }
  }
}
