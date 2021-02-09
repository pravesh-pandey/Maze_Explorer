#define ir1  13
#define ir2  A0
#define ir3  A1
#define ir4  A2
#define ir5  A3
#define ir6  A4
#define ir7  A5
#define ir8  12
int ir[8];                      // ir[0] -> rightfar
// ir[1] ->
// ir[2] -> rightnear
// ir[3] -> rightcenter
// ir[4] -> leftcenter
// ir[5] -> leftnear
// ir[6] ->
// ir[7] -> leftfar
int irr[8];
//// out 4
#define ML1 5
#define ML2 6

#define MR1 10
#define MR2 11

int leapTime = 150;

int v = 170;
int error = 0, prev_error = 0;
float Kp = 18, Kd = 8; // KP=14, kD=6
int weights[8] = { -10, -7, -5, -1, 1, 5, 7, 10};
int avg_error = 0, sensors = 0, output = 0, leftmot = 0, rightmot = 0;

char path[30] = {};
int pathLength;
int readLength;
int replaystage = 0;
unsigned long t;// 0 -> 1st stage   ,,  1 -> going with shorted path


void setup() {
  // put your setup code here, to run once
  pinMode(ir1, INPUT);
  pinMode(ir2, INPUT);
  pinMode(ir3, INPUT);
  pinMode(ir4, INPUT);
  pinMode(ir5, INPUT);
  pinMode(ir6, INPUT);
  pinMode(ir7, INPUT);
  pinMode(ir8, INPUT);

  pinMode(ML1, OUTPUT);
  pinMode(ML2, OUTPUT);
  pinMode(MR1, OUTPUT);
  pinMode(MR2, OUTPUT);

 //Serial.begin(9600);
}

void loop() {

  readSensors();   //// '0' -> white      '1' -> black

  if ((ir[0] && ir[1] && ir[2]) || (ir[5] && ir[7]) || 
      (ir[0] && ir[1] && ir[2] && ir[3] && ir[4] && ir[7]) ||
      (!ir[0] && !ir[1] &&  !ir[2] && !ir[3] && !ir[4] && !ir[5] &&  !ir[6] && !ir[7]))
  {
    Serial.println("going for LSR");
    for(int i=0; i<8 ; i++)
       irr[i] = 0;
    t = millis();
    while (millis() - t < 140) {
      readSensors();
      for (int i = 0; i < 8; i++)
      {
        if (irr[i] == 0)
          irr[i] = ir[i];
      }
      straight();
    }
      for(int i=7; i>=0; i--)
      {
        Serial.print(irr[i]);
        Serial.print("   ");
        }
        Serial.println("");
    LSR();        // when we get any junction point. it will deside the priorirty of turn by (LSR)
  }
  else {
    //////Serial.println("going straight");
    straight();
  }

}


void readSensors() {

  ir[0] = digitalRead(ir1);
  ir[1] = digitalRead(ir2);
  ir[2] = digitalRead(ir3);
  ir[3] = digitalRead(ir4);
  ir[4] = digitalRead(ir5);
  ir[5] = digitalRead(ir6);
  ir[6] = digitalRead(ir7);
  ir[7] = digitalRead(ir8);

  //////Serial.print(ir[0]);
  //////Serial.print("   ");
  //////Serial.print(ir[1]);
  //////Serial.print("   ");
  //////Serial.print(ir[2]);
  //////Serial.print("   ");
  //////Serial.print(ir[3]);
  //////Serial.print("   ");
  //////Serial.print(ir[4]);
  //////Serial.print("   ");
  //////Serial.print(ir[5]);
  //////Serial.print("   ");
  //////Serial.print(ir[6]);
  //////Serial.print("   ");
  //////Serial.print(ir[7]);
  //////Serial.println("   ");
  //////Serial.println("---------------------");
}


void LSR() {    // for junction points


  /* analogWrite(ML1, v);
    analogWrite(ML2, 0);
    analogWrite(MR1, v);
    analogWrite(MR2, 0);
    delay(50);*/

  if (digitalRead(ir1) && digitalRead(ir2) && digitalRead(ir4) && digitalRead(ir6) && digitalRead(ir8))
    done();



  if (irr[7] || irr[6] )
  { t = millis();
    //if(( ir[0] && ir[7]) || ( ir[1] && ir[6])){     // case '1' or case '2'
    //////Serial.println("case '1' or case '2'");
    while (millis() - t < 100)
      straight();
    turnLeft();
  }

  else if (irr[0] || irr[1] ) {              // case '4' or case'6'  and one more hiden case to be find!
    if (digitalRead(ir1) || digitalRead(ir2) || digitalRead(ir3) || digitalRead(ir4) || digitalRead(ir5) || digitalRead(ir6) || digitalRead(ir7) || digitalRead(ir8)) {
      //////Serial.println("case '4' \n stright");
      straight();// only case'6'
    }
    else {
      t = millis();
      while (millis() - t < 100)
        straight();
      //////Serial.println("case '6'");
      turnRight();
    }
  }
  else if (!irr[0] && !irr[1] &&  !irr[2] && !irr[3] && !irr[4] && !irr[5] &&  !irr[6] && !irr[7]) {        // case'7'
    //////Serial.println("case '7'");
    turnAround();
  }
}



void done() {
  Serial.print("######DONE#######");
  analogWrite(ML1, 0);
  analogWrite(ML2, 0);
  analogWrite(MR1, 0);
  analogWrite(MR2, 0);
  done();
}

/*void straight(){ //////////////////////////////////////////////////////////////////////////////////////// without PID

  if(ir[3]==1 && ir[4] ==1)
  {
    analogWrite(ML1, v);
    analogWrite(ML2, 0);
    analogWrite(MR1, v);
    analogWrite(MR2, 0);

    }
  else if(ir[2]==1 && ir[3]==1){
    analogWrite(ML1, v+20);
    analogWrite(ML2, 0);
    analogWrite(MR1, v-20);
    analogWrite(MR2, 0);
    }
  else if(ir[4]==1 && ir[5]==1){
    analogWrite(ML1, v-20);
    analogWrite(ML2, 0);
    analogWrite(MR1, v+20);
    analogWrite(MR2, 0);
    }
  else if(ir[1]==1 && ir[2]==1){
    analogWrite(ML1, v+40);
    analogWrite(ML2, 0);
    analogWrite(MR1, v-40);
    analogWrite(MR2, 0);
    }
  else if(ir[5]==1 && ir[6]==1){
    analogWrite(ML1, v-40);
    analogWrite(ML2, 0);
    analogWrite(MR1, v+40);
    analogWrite(MR2, 0);
    }
  else if(ir[0]==1 )
  { while(digitalRead(ir5)!=1)
  {
    analogWrite(ML1, v);
    analogWrite(ML2, 0);
    analogWrite(MR1, 0);
    analogWrite(MR2, v);
    }}
  else if( ir[7]==1)
  { while(digitalRead(ir4)!=1)
    {analogWrite(ML1, 0);
    analogWrite(ML2, v);
    analogWrite(MR1, v);
    analogWrite(MR2, 0);
    }}

  else {
    analogWrite(ML1, v);
    analogWrite(ML2, 0);
    analogWrite(MR1, v);
    analogWrite(MR2, 0);
    }

  }   ////////////////////////////////////////////////////////////////////////////////////////////////////////

*/
void straight() { //////////////////////////////////////////////////////////////////////////////////////// By PID controlling
  prev_error = avg_error;
  for (int i = 0; i < 8; i++)
  { if (ir[i])
      sensors++;
    error += ir[i] * weights[7 - i];
  }
  avg_error = error / sensors;
  sensors = 0; error = 0;
  output = Kp * avg_error + Kd * (avg_error - prev_error);

  leftmot = v + output;
  rightmot = v - output;
  leftmot = constrain(leftmot, 0, 255);
  rightmot = constrain(rightmot, 0, 255);
  analogWrite(ML1, leftmot);
  analogWrite(ML2, LOW);
  analogWrite(MR1, rightmot);
  analogWrite(MR2, LOW);

  //////Serial.print(leftmot);
  //////Serial.print("          ");
  //////Serial.println(rightmot);
  output = 0;
}



void turnLeft() {
  //////Serial.print("turn left starts");
  while (digitalRead(ir8) || digitalRead(ir7) || digitalRead(ir6) || digitalRead(ir5) || digitalRead(ir4) || digitalRead(ir3) || digitalRead(ir2) || digitalRead(ir1))
  {
    analogWrite(ML1, 0);
    analogWrite(ML2, v);
    analogWrite(MR1, v);
    analogWrite(MR2, 0);
    //////Serial.println("In while 1 ");
  }

  while (!digitalRead(ir5) && !digitalRead(ir4) && !digitalRead(ir3) && !digitalRead(ir2)) {
    analogWrite(ML1, 0);
    analogWrite(ML2, v);
    analogWrite(MR1, v);
    analogWrite(MR2, 0);
    //////Serial.println("In while 2");
  }
  analogWrite(ML1, v);   // this thing is due overshoot of motors
  analogWrite(ML2, 0);
  analogWrite(MR1, 0);
  analogWrite(MR2, v);
  delay(10);
  //////Serial.println("turn left ends");

}


void turnRight() {
  //////Serial.println("turn right starts");
  while (digitalRead(ir8) || digitalRead(ir7) || digitalRead(ir6) || digitalRead(ir5) || digitalRead(ir4) || digitalRead(ir3) || digitalRead(ir2) || digitalRead(ir1)) {
    analogWrite(ML1, v);
    analogWrite(ML2, 0);
    analogWrite(MR1, 0);
    analogWrite(MR2, v);
    //////Serial.println("In while 1 ");
  }

  while ( !digitalRead(ir4) && !digitalRead(ir5) && !digitalRead(ir6) && !digitalRead(ir7) ) {
    analogWrite(ML1, v);
    analogWrite(ML2, 0);
    analogWrite(MR1, 0);
    analogWrite(MR2, v);
    //////Serial.println("In while 2 ");
  }
  analogWrite(ML1, 0);   // this thing is due overshoot of motors
  analogWrite(ML2, v);
  analogWrite(MR1, v);
  analogWrite(MR2, 0);
  delay(10);
  //////Serial.println("turn right ends");
}


void turnAround() {
  //////Serial.println("turn around starts");
  analogWrite(ML1, v);
  analogWrite(ML2, 0);
  analogWrite(MR1, v);
  analogWrite(MR2, 0);
  delay(10);
  while (!digitalRead(ir3) && !digitalRead(ir4) && !digitalRead(ir5) && !digitalRead(ir6) && !digitalRead(ir7)) {
    analogWrite(ML1, v);
    analogWrite(ML2, 0);
    analogWrite(MR1, 0);
    analogWrite(MR2, v);
    //////Serial.println("In while");
  }
  analogWrite(ML1, 0);   // this thing is due overshoot of motors
  analogWrite(ML2, v);
  analogWrite(MR1, v);
  analogWrite(MR2, 0);
  delay(10);
  //////Serial.println("turn around ends");

}
