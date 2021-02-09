//code developed by team !ABHIMANYU { MNNIT allahabad }

// defining IR sensor pins
//#define ircheck 7
#define ir1  13
#define ir2  A0
#define ir3  A1
#define ir4  A2
#define ir5  A3
#define ir6  A4
#define ir7  A5
#define ir8  12

int ir[8];                      
// ir[0] -> rightfar
// ir[1] ->
// ir[2] -> rightnear
// ir[3] -> rightcenter
// ir[4] -> leftcenter
// ir[5] -> leftnear
// ir[6] ->
// ir[7] -> leftfar
int irr[8];

// defining motor pins
#define ML1 5
#define ML2 6
#define MR1 10
#define MR2 11

// defining led pins
#define Red 8
#define Green 9
#define Blue 1

#define S2 2
#define coloroutput 4
#define S3 3

int leapTime = 150;
int v = 170;
int error = 0, prev_error = 0;
int Kp = 18, Kd = 8; // KP=14, kD=6
int weights[8] = { -10, -7, -5, -1, 1, 5, 7, 10};
int avg_error = 0, sensors = 0, output = 0, leftmot = 0, rightmot = 0;

int pathLength=0;
int readLength=0;
int C=0,G=0,R=0;
int red=0,green=0;
int cLength=0;
int stage=0;

unsigned long t;

char temp,color[10]={},path[30]={};


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
  //pinMode(ircheck, INPUT);

  pinMode(ML1, OUTPUT);
  pinMode(ML2, OUTPUT);
  pinMode(MR1, OUTPUT);
  pinMode(MR2, OUTPUT);
  pinMode(S2,OUTPUT);
  pinMode(S3,OUTPUT);
  pinMode(coloroutput,INPUT);
  pinMode(Green, OUTPUT);
  pinMode(Red, OUTPUT);
  pinMode(Blue,OUTPUT);
  digitalWrite(Blue,LOW);
  digitalWrite(Red,LOW);
  digitalWrite(Green, LOW);
  //Serial.begin(9600);
}

void loop() {

  readSensors();   //// '0' -> white      '1' -> black

  if ((ir[0] && ir[1] && ir[2]) || (ir[5] && ir[7]) || 
      (ir[0] && ir[1] && ir[2] && ir[3] && ir[4] && ir[7]) )
  {
    Serial.println("going for LSR");
    for(int i=0; i<8 ; i++)
       irr[i] = 0;
    t = millis();
    while (millis() - t < 180) {
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
    printPath();
    LSR();  
    printPath();// when we get any junction point. it will deside the priorirty of turn by (LSR)
  }
  else if((!ir[0] && !ir[1] &&  !ir[2] && !ir[3] && !ir[4] && !ir[5] &&  !ir[6] && !ir[7]))
  {
    Serial.println("Going for coloraround");
    printPath();
    coloraround();
    printPath();
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


  if (digitalRead(ir1) && digitalRead(ir2) && digitalRead(ir4) && digitalRead(ir6) && digitalRead(ir8))
    done();
    
  else if (irr[7] && irr[6] )
  { 
    t = millis();
    while (millis() - t < 80)
      straight();
     path[pathLength]='L';
    turnLeft();
    
  }

  else if (irr[0] && irr[1] ) {              // case '4' or case'6' 
    if (digitalRead(ir1) || digitalRead(ir2) || digitalRead(ir3) || digitalRead(ir4) || digitalRead(ir5) || digitalRead(ir6) || digitalRead(ir7) || digitalRead(ir8)) 
    {
      //////Serial.println("case '4' \n stright");
       path[pathLength]='S';
      straight();
       // only case'6'
    }
    else {
      t = millis();
      while (millis() - t < 80)
        straight();
      //////Serial.println("case '6'");
      path[pathLength]='R';
      turnRight();
      
    }
  }
  pathLength++;
  if(!stage)
    {
        if(path[pathLength-2]=='B' && pathLength>=3)
          shortpath();
    }
}
  
void coloraround(){
            // case'7'
    //////Serial.println("case '7'");
      t=millis();
      while(millis()-t<180)
      {
        readSensors();
       straight();
      }
     
   if(digitalRead(ir1)||digitalRead(ir2)||digitalRead(ir3)||digitalRead(ir4)||digitalRead(ir5)||digitalRead(ir6)||digitalRead(ir7)||digitalRead(ir8))
     {
    analogWrite(ML1, 0);
    analogWrite(ML2, v);
    analogWrite(MR1, 0);
    analogWrite(MR2, v);
    delay(10);
     
       Serial.println("going for color check");
       led();
      } 
   else  {
    Serial.println("turn around");
      path[pathLength]='B';
      pathLength++;
     turnAround();
   }
}



void done() {
  Serial.print("######DONE#######");
  analogWrite(ML1, 0);
  analogWrite(ML2, 0);
  analogWrite(MR1, 0);
  analogWrite(MR2, 0);
    
  if(!stage)
  {
    delay(800);
  analogWrite(ML1, 0);
  analogWrite(ML2, v);
  analogWrite(MR1, 0);
  analogWrite(MR2, v);
  delay(300);
  analogWrite(ML1, 0);
  analogWrite(ML2, 0);
  analogWrite(MR1, 0);
  analogWrite(MR2, 0);
  delay(500);
    stage++;
    cLength--;
    pathLength--;
    reversePath();
    turnLeft();       /////////////////////////////////////////////
     readLength=pathLength;
      replay();
  }
  else 
  {
    digitalWrite(Blue,HIGH);
    delay(500);
    digitalWrite(Blue,LOW);
    delay(200);
    done();
  }
}


void straight() {    /////////////////////////////////////////// By PID controlling
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
    //if(digitalRead(ircheck))
       //break;
    //////Serial.println("In while 2");
  }
  analogWrite(ML1, v);   // this thing is due overshoot of motors
  analogWrite(ML2, 0);
  analogWrite(MR1, 0);
  analogWrite(MR2, v);
  delay(10);

    /*analogWrite(ML1, 0);
    analogWrite(ML2, v);
    analogWrite(MR1, 0);
    analogWrite(MR2, v-20);
    delay(200);
    
    analogWrite(ML1, v);
    analogWrite(ML2, 0);
    analogWrite(MR1, v);
    analogWrite(MR2, 0);
    delay(10);*/
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
    //if(digitalRead(ircheck))
       //break;
    //////Serial.println("In while 2 ");
  }
  analogWrite(ML1, 0);   // this thing is due overshoot of motors
  analogWrite(ML2, v);
  analogWrite(MR1, v);
  analogWrite(MR2, 0);
  delay(10);
    /*analogWrite(ML1, 0);
    analogWrite(ML2, v);
    analogWrite(MR1, 0);
    analogWrite(MR2, v-20);
    delay(200);
    
    analogWrite(ML1, v);
    analogWrite(ML2, 0);
    analogWrite(MR1, v);
    analogWrite(MR2, 0);
    delay(10);*/
  //////Serial.println("turn right ends");
}


void turnAround() {
  //////Serial.println("turn around starts");
  analogWrite(ML1, v);
  analogWrite(ML2, 0);
  analogWrite(MR1, v);
  analogWrite(MR2, 0);
  delay(100);
  while (!digitalRead(ir3) && !digitalRead(ir4) && !digitalRead(ir5) && !digitalRead(ir6) && !digitalRead(ir7)) {
    analogWrite(ML1, v);
    analogWrite(ML2, 0);
    analogWrite(MR1, 0);
    analogWrite(MR2, v);
    //if(digitalRead(ircheck))
       //break;
    //////Serial.println("In while");
  }
  analogWrite(ML1, 0);   // this thing is due overshoot of motors
  analogWrite(ML2, v);
  analogWrite(MR1, v);
  analogWrite(MR2, 0);
  delay(10);

    /*analogWrite(ML1, 0);
    analogWrite(ML2, v);
    analogWrite(MR1, 0);
    analogWrite(MR2, v);
    delay(200);
    analogWrite(ML1, v);
    analogWrite(ML2, 0);
    analogWrite(MR1, v);
    analogWrite(MR2, 0);
    delay(10);*/
  //////Serial.println("turn around ends");
}
void led()
{
 C=0; R=0; G=0;
 //analogWrite(ML1,LOW);
 //analogWrite(ML2,v);
 //analogWrite(MR1,LOW);
 //analogWrite(MR2,v);
 //delay(10);
 int t1=millis();
  while(!C)
  {
            analogWrite(ML1,20);
            analogWrite(ML2,LOW);
            analogWrite(MR1,20);
            analogWrite(MR2,LOW);
            readcolour(); 
            if(millis()-t1>250)
              break;
        }
        analogWrite(ML1,0);
        analogWrite(MR1,0);
     if(G)
     {
      if(!stage)
      {
        digitalWrite(Green,HIGH);
        delay(500);
        digitalWrite(Green,LOW);
        color[cLength++]='G';
      }
      else 
      {
        if(color[cLength]=='G')
           done();
        else
        {
        digitalWrite(Green,HIGH);
        delay(500);
        digitalWrite(Green,LOW);
        }
      }
     }
     
     else if(R)
     {
      if(!stage)
      {
      digitalWrite(Red,HIGH);
      delay(500);
      digitalWrite(Red,LOW);     
      color[cLength++]='R';
      }
      else 
      {
        if(color[cLength]=='R')
        done();
        else 
        {
          digitalWrite(Red,HIGH);
          delay(500);
          digitalWrite(Red,LOW);  
        }
       } 
      }
} 
  
void readcolour() 
{
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  red=pulseIn(coloroutput,LOW);
  delay(10);
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  green=pulseIn(coloroutput,LOW);
  Serial.print("RED   ");
  Serial.print(red);
  Serial.print("     GREEN    ");
  Serial.print(green);
  if((green< 20 || red< 20)) ///////////////////////     requires tuning    //////////////////////////////////////////
  {
  if(red<green && green >10)
   {
    R++;
    C++;
   }
  else if(green<red && red >10)
    {
      G++;
      C++;
    }
  }  
  else
  Serial.println("   Black or white"); 
}

void reversePath()
{ 
  for(int i=0 ;i<=pathLength; i++)
  {
    if(path[i]=='L')
    path[i]='R';
    else if(path[i]=='R')
    path[i]='L';
  }
  printPath();
}

void shortpath()
{
  int shortDone=0;
  if(path[pathLength-3]=='L' && path[pathLength-1]=='R'){     // case 1       .--------
    pathLength-=3;                                            //                       |
    path[pathLength]='B';                                     //                       |
    //Serial.println("test1");                                //                       |
    shortDone=1;                                              //                       |
  }
   
  if(path[pathLength-3]=='L' && path[pathLength-1]=='S' && shortDone==0){    // case 2   .------------->
    pathLength-=3;                                                           //                 |
    path[pathLength]='R';                                                    //                 |
    //Serial.println("test2");                                               //                 |
    shortDone=1;                                                             //                 |
  }
   
  if(path[pathLength-3]=='R' && path[pathLength-1]=='L' && shortDone==0){  // case 3       ---------.   
    pathLength-=3;                                                         //              |
    path[pathLength]='B';                                                  //              |
    //Serial.println("test3");                                             //              |
    shortDone=1;                                                           //              |
  }
  
   
  if(path[pathLength-3]=='S' && path[pathLength-1]=='L' && shortDone==0){   //case 4       .
    pathLength-=3;                                                          //             |
    path[pathLength]='R';                                                   //             |------->
    //Serial.println("test4");                                              //             |
    shortDone=1;                                                            //             |
  }
     
  if(path[pathLength-3]=='S' && path[pathLength-1]=='S' && shortDone==0){  // case 5                .
    pathLength-=3;                                                         //                       |
    path[pathLength]='B';                                                  //               ._______|
    //Serial.println("test5");                                             //                       |
    shortDone=1;                                                           //                       |  
  }
    if(path[pathLength-3]=='L' && path[pathLength-1]=='L' && shortDone==0){   // case 6         ^
    pathLength-=3;                                                            //                |
    path[pathLength]='S';                                                     //         .------|
    //Serial.println("test6");                                                //                |
    shortDone=1;                                                              //                |
  }
  path[pathLength+2]=NULL;
  path[pathLength+3]=NULL;
  pathLength++;
  //Serial.print("Path length: ");
  //Serial.println(pathLength);
  //printPath();
}




void printPath(){
  Serial.println("+++++++++++++++++");
  int x=0;
  while(x<=pathLength)
  Serial.print(path[x++]);
  Serial.print("      ");
  Serial.println(pathLength);
  Serial.println("+++++++++++++++++");

}


void replay(){
 
 readSensors();   //// '0' -> white      '1' -> black

  if ((ir[0] && ir[1] && ir[2]) || (ir[5] && ir[7]) || 
      (ir[0] && ir[1] && ir[2] && ir[3] && ir[4] && ir[7]) )
  {
    Serial.print("      going for LSR of replay     turnning ---");
    Serial.print(path[readLength]);
    Serial.print("     ");
    Serial.println(readLength);
    for(int i=0; i<8 ; i++)
       irr[i] = 0;
    t = millis();
    while (millis() - t < 180) {
      readSensors();
      for (int i = 0; i < 8; i++)
      {
        if (irr[i] == 0)
          irr[i] = ir[i];
      }
      straight();
    }
    
    if(path[readLength]=='L'){
     t = millis();
    while (millis() - t < 80)
      straight();
    turnLeft();
    }
    else if(path[readLength]=='R'){
     t = millis();
    while (millis() - t < 80)
      straight();
    turnRight();
    }
    else if(path[readLength]=='S'){
     straight();
    }
    readLength--;
  }
   
  else if((!ir[0] && !ir[1] &&  !ir[2] && !ir[3] && !ir[4] && !ir[5] &&  !ir[6] && !ir[7]))
  {
    coloraround();
  
  }

  
  else {
    //////Serial.println("going straight");
    straight();
  }

   replay();
}
   
  
