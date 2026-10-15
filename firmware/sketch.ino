#include <Servo.h>

const unsigned long T_MS = 3000;

const int PIN_S1 = A0;     
const int PIN_TRIG = 3;    
const int PIN_ECHO = 4;    
const int PIN_LED = 5;     
const int PIN_S3 = 6;      
const int PIN_SERVO = 9;   

const float THRESHOLD = 30.0;

unsigned long n = 0;
bool alarm = false;

Servo barrier;

float readDistance() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  unsigned long us = pulseIn(PIN_ECHO, HIGH, 30000UL);

  if (us == 0) {
    return -1.0;
  }

  return us * 0.034 / 2.0;
}

void setup() {
  Serial.begin(9600);

  pinMode(PIN_S1, INPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_S3, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);

  digitalWrite(PIN_TRIG, LOW);
  digitalWrite(PIN_LED, LOW);

  barrier.attach(PIN_SERVO);
  barrier.write(0); 
}

void loop() {
  n++;

  float s1 = 100.0 * analogRead(PIN_S1) / 1023.0;

  float s2 = readDistance();

  int s3 = !digitalRead(PIN_S3);

  Serial.print("DATA n=");
  Serial.print(n);
  Serial.print(" s1=");
  Serial.print(s1, 2);
  Serial.print(" s2=");
  Serial.print(s2, 1);
  Serial.print(" s3=");
  Serial.println(s3);

  bool validDistance = s2 >= 2.0 && s2 <= 400.0;
  bool condition = validDistance && s2 < THRESHOLD;

  if (condition && !alarm) {
    alarm = true;

    barrier.write(90);
    digitalWrite(PIN_LED, HIGH);

    Serial.print("EVENT n=");
    Serial.print(n);
    Serial.println(" type=THRESHOLD");
  } else if (!condition && alarm) {
    alarm = false;

    barrier.write(0);
    digitalWrite(PIN_LED, LOW);

    Serial.print("EVENT n=");
    Serial.print(n);
    Serial.println(" type=NORMAL");
  }

  delay(T_MS);
}