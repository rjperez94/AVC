#include <QTRSensors.h>

#define NUM_SENSORS	8   // constant value

QTRSensorsRC qtr((unsigned char[]) {
  2,4,5,6,7,8,9,10}
, NUM_SENSORS);

float KP = 0.1; //floating-point proportional constant
float KD = 1; //floating-point derivative constant
int M1 = 100; //base motor speed
int M2 = 100; //base motor speed
int maxMotorValue = 255; //maximum motor speed



unsigned int sensors[8];
unsigned int sensorsSimple[8];

int pwm_b = 3;  //PWM control for motor outputs 1 and 2 is on digital pin 3
int pwm_a = 11;  //PWM control for motor outputs 3 and 4 is on digital pin 11
int dir_b = 12;  //dir control for motor outputs 1 and 2 is on digital pin 12
int dir_a = 13;  //dir control for motor outputs 3 and 4 is on digital pin 13

int lastError = 0; //Variable to store the last error

int count;
double average;

int loopCount = 0;
char quad = '1';

void setup()
{
  // optional: wait for some input from the user, such as  a button press
  // optional: signal that the calibration phase is now over and wait for further
  Serial.begin(9600);
  // input from the user, such as a button press
  digitalWrite(dir_a, HIGH);   // select motor direction
  digitalWrite(dir_b, LOW);   // in this case both forward
  analogWrite(pwm_a, 40);     
  analogWrite(pwm_b, 40);
  // then start calibration phase and move the sensors over both
  // reflectance extremes they will encounter in your application:
  int i;
  for (i = 0; i < 40; i++)  // make the calibration take about 5 seconds
  {
    qtr.calibrate();
    delay(20);
  }

  // optional: signal that the calibration phase is now over and wait for further
  // input from the user, such as a button press
  Stop();
}

void loop()
{
 double dist;dist=43325*pow((double)(analogRead(A0)),-1.1129);// double sensorRead = analogRead(A0);

  int longRangeSensorReadLeft = analogRead(A1);//
  int longRangeSensorReadRight = analogRead(A2);



  int position = qtr.readLine(sensors, QTR_EMITTERS_ON, false);
  
  //for(int i =0; i < 8; i++){
    //Serial.println
  //}
  
  findAverage();
  int error = position - 3500;

  int motorSpeed = KP * error + KD * (error - lastError);
  lastError = error;

  int numberOfSensorsCovered = count;
  if (numberOfSensorsCovered == 0){
    numberOfSensorsCovered = 2;
  }

  int m1Speed = M1/numberOfSensorsCovered + motorSpeed;
  int m2Speed = M2/numberOfSensorsCovered - motorSpeed;
  if (m1Speed < 0) m1Speed = 0;
  if (m2Speed < 0) m2Speed = 0;
  //if (m1Speed > maxMotorValue) m1Speed = maxMotorValue;
  //if (m2Speed > maxMotorValue) m2Speed = maxMotorValue;

  if (longRangeSensorReadLeft > 200 && longRangeSensorReadRight > 200){
    quad = '4';  
  }

  if (quad == '4') {

    double error = longRangeSensorReadLeft - longRangeSensorReadRight;

    //Lots of space on one side
    if (longRangeSensorReadLeft < 150 || longRangeSensorReadRight < 150){
      //Turn left
      if (longRangeSensorReadLeft > longRangeSensorReadRight){

        analogWrite(pwm_a, 60);     
        analogWrite(pwm_b, 20);
        delay(1100);
        analogWrite(pwm_a, 50);     
        analogWrite(pwm_b, 50);
        delay(200);
        analogWrite(pwm_a, 0);     
        analogWrite(pwm_b, 0);
        delay(5000);

      }
      //Turn Right
      else if (longRangeSensorReadRight > longRangeSensorReadLeft){

        analogWrite(pwm_a, 20);     
        analogWrite(pwm_b, 60);
        delay(1100);
        analogWrite(pwm_a, 50);     
        analogWrite(pwm_b, 50);
        delay(200);
        analogWrite(pwm_a, 0);     
        analogWrite(pwm_b, 0);
        delay(5000);

      }

    }
    else
    {

      if (error > 200) error = 200;

      int m1Speed = 50 - 0.2*error;
      int m2Speed = 50 + 0.2*error;

      // set motor speeds using the two motor speed variables above
      analogWrite(pwm_a, m2Speed);     
      analogWrite(pwm_b, m1Speed);

    }
    // if (sensorRead<500 && error=0){
//        digitalWrite(dir_a, HIGH);  
//        digitalWrite(dir_b, LOW);   
//        analogWrite(pwm_a, 40);     
//        analogWrite(pwm_b, 40);
//        delay(5000);
      
//    }

  }
  else if(dist < 100){
    quad = '3';
    TurnAround();
    delay(400);
    qtr.readLine(sensors, QTR_EMITTERS_ON, false);
    findAverage();
    while(average != 4.5){
      qtr.readLine(sensors, QTR_EMITTERS_ON, false);
      findAverage();
    }

    Stop();
  }


  else if(isLeft()){ 
    quad = '2';
    digitalWrite(dir_a, LOW);
    analogWrite(pwm_a, 30);     
    analogWrite(pwm_b, 80);
    delay(300);
    qtr.readLine(sensors, QTR_EMITTERS_ON, false);
    findAverage();
    while(average != 4.5){  
      qtr.readLine(sensors, QTR_EMITTERS_ON, false);
      findAverage();
    }
    digitalWrite(dir_a, HIGH);
  }

  //Gone off the end of the line
  else if (count == 0){

    TurnAround();

    while(average != 4.5){
      qtr.readLine(sensors, QTR_EMITTERS_ON, false);
      findAverage();
    }

    Stop();
  }
  else{
    // set motor speeds using the two motor speed variables above
    analogWrite(pwm_a, m2Speed);     
    analogWrite(pwm_b, m1Speed);

  }  

  loopCount++;
  if (loopCount =10){
    SendToZB(quad);
    loopCount = 0;
  }

}

void TurnAround(){
  digitalWrite(dir_a, HIGH);
  digitalWrite(dir_b, LOW);   
  analogWrite(pwm_a, 40);     
  analogWrite(pwm_b, 40);
}

void Stop(){
  digitalWrite(dir_a, HIGH);
  digitalWrite(dir_b, HIGH);   
  analogWrite(pwm_a, 0);     
  analogWrite(pwm_b, 0);
}

boolean isLeft(){
  if((sensorsSimple[0] == 0) && (sensorsSimple[1] == 0)) return true;
  return false; 
}

int findAverage(){
  count = 0;
  for (int i = 0 ; i < NUM_SENSORS ;i++){
    if(sensors[i] > 100){
      sensorsSimple[i] = 1;
    }
    else{
      sensorsSimple[i] = 0;
    }
  }

  int sum = 0;

  for (int i = 0; i < NUM_SENSORS; i++){
    if(sensorsSimple[i] == 0) {
      count ++;
      sum += (i + 1);

    }
  }
  if (count == 0){
    average = 0;
  }
  else{
    average = (double) sum / count;
  }
}

void SendToZB(char quad)
{
  char frame_to_ZB[64];
  char data_to_send [16];
  int n_frame_to_ZB;
  int j;
  unsigned int check_sum_total = 0;
  unsigned int CRC = 0;  

  //delimiter 
  frame_to_ZB[0] = 0x7E;
  // length of array
  frame_to_ZB[1] = 0x00;
  frame_to_ZB[2] = 0x1E; //length
  //API ID
  frame_to_ZB[3] = 0x10; // transmission frame
  frame_to_ZB[4] = 0x01;
  // destination 64 bit address - broadcast
  frame_to_ZB[5] = 0x00;
  frame_to_ZB[6] = 0x13;
  frame_to_ZB[7] = 0xA2;
  frame_to_ZB[8] = 0x00;
  frame_to_ZB[9] = 0x40;
  frame_to_ZB[10] = 0x6A;
  frame_to_ZB[11] = 0x40;
  frame_to_ZB[12] = 0xA4;
  // destination 16 bit address - broadcast
  frame_to_ZB[13] = 0xFF;
  frame_to_ZB[14] = 0xFE;
  // number of hops
  frame_to_ZB[15] = 0x00;
  // option
  frame_to_ZB[16] = 0x01;

  // data 
  data_to_send[0] = 'q';
  data_to_send[1] = 'u';
  data_to_send[2] = 'a';
  data_to_send[3] = 'd';
  data_to_send[4] = 'r';
  data_to_send[5] = 'a';
  data_to_send[6] = 'n';
  data_to_send[7] = 't';
  data_to_send[8] = quad;
  data_to_send[9] = '-';
  data_to_send[10] = 'G';
  data_to_send[11] = 'R';
  data_to_send[12] = 'O';
  data_to_send[13] = 'U';
  data_to_send[14] = 'P';
  data_to_send[15] = '3';



  for (j=17; j < 33; j++)
  {
    frame_to_ZB[j] = data_to_send[j - 17]; // data to send
  }
  n_frame_to_ZB = 33;

  // add CRC 
  for ( j = 3 ; j < n_frame_to_ZB ; j++) {
    check_sum_total += frame_to_ZB[j];
  }
  check_sum_total = check_sum_total & 0xFF;
  CRC = 0xFF - check_sum_total;
  frame_to_ZB[n_frame_to_ZB] = CRC;
  n_frame_to_ZB += 1;

  //Serial.println("transmitting data:");

  for (j =0; j < n_frame_to_ZB; j++) {
    Serial.write(frame_to_ZB[j]);
    // Serial.print(frame_to_ZB[j], HEX);
    // Serial.print("INSIDE FOR LOOP");
  }
  //Serial.println("DONE");
}

