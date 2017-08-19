
/*
 * Motor code does not require any libraries
 * Motor control board (Vicmoto) is hardwired
 * to take control signals from Arduino
 * on pins 3,11,12,13
 * Do not use these Arduino pins for anything else
 * 
 * These code makes robot move and stop
 */

int pwm_a = 3;  //PWM control for motor outputs 1 and 2 is on digital pin 3
int pwm_b = 11;  //PWM control for motor outputs 3 and 4 is on digital pin 11
int dir_a = 12;  //dir control for motor outputs 1 and 2 is on digital pin 12
int dir_b = 13;  //dir control for motor outputs 3 and 4 is on digital pin 13
int move_pulse = 100;



void setup()
{
  // initializ motor pins  
  pinMode(pwm_a, OUTPUT);  //Set control pins to be outputs
  pinMode(pwm_b, OUTPUT);
  pinMode(dir_a, OUTPUT);
  pinMode(dir_b, OUTPUT);
  analogWrite(pwm_a, 0);  // set motor voltages 0
  analogWrite(pwm_b, 0);
  Serial.begin(9600);     // for communacations with computer
}


// endlesly loopsvoid loop()
{
  Forward(50);
  //delay(1000);
  
  //delay(2000);
  //Back(500);
  //delay(2000);
  //Left(500);
  //delay(2000);
  //Right(500);
  //delay(2000);
  
}




void Forward(int dt)
{
   digitalWrite(dir_a, HIGH);   // select motor direction
   digitalWrite(dir_b, HIGH);   
   analogWrite(pwm_a, 255);     // set motor voltage to max
   analogWrite(pwm_b, 255);
   delay(dt);                   // wait
   analogWrite(pwm_a, 0);       // set motor voltage to 0
   analogWrite(pwm_b, 0);
   Serial.println(" forward");
}

void Back(int dt)
{
  digitalWrite(dir_a, LOW);   
  digitalWrite(dir_b, LOW);   
  analogWrite(pwm_a, 255);
  analogWrite(pwm_b, 255);
  delay(dt);
  analogWrite(pwm_a, 0);
  analogWrite(pwm_b, 0);
  Serial.println(" back");
}

void Left(int dt)
{
   digitalWrite(dir_a, HIGH);   
   digitalWrite(dir_b, LOW);   
   analogWrite(pwm_a, 255);
   analogWrite(pwm_b, 255);
   delay(dt);
   analogWrite(pwm_a, 0);
   analogWrite(pwm_b, 0);
   Serial.println(" left");
}

void Right(int dt)
{
   digitalWrite(dir_a, LOW);   
   digitalWrite(dir_b, HIGH);   
   analogWrite(pwm_a, 255);
   analogWrite(pwm_b, 255);
   delay(dt);
   analogWrite(pwm_a, 0);
   analogWrite(pwm_b, 0);
   Serial.println(" right");
}

