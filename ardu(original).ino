#include <Servo.h>

#define ENA1 10
#define ENA2 11
#define IN1 9
#define IN2 4
#define IN3 7
#define IN4 8

bool left = false, right = false;
bool ld = false, rd = false;

// IN1 HIGH -- IN2 LOW --> ENG FORWARD <-- IN3 HIGH -- IN4 LOW 
// IN1 LOW -- IN2 HIGH --> ENG BACKWARD <-- IN3 LOW -- IN4 HIGH
// IN1 LOW -- IN2 LOW --> STOP <-- IN3 LOW -- IN4 LOW

// ENA -- SPEEEEEEEED

typedef struct {
    Servo s;
    int pin;
    int pos;
} servo;

servo kicker;
servo hook;
servo camera1;

int pins[5] = {A5, A4, A3, A2, A1};
int state[5] = {0, 0, 0, 0, 0};
int speed[3] = {50, 150, 255};
int actualSpeed = speed[1];

void initComponents(void);  //default function

// MOTOR -----------------------------------------------------------
  void lEngine(int speed, bool direction);
  void rEngine(int speed, bool direction);
  void moveForward(int speed);
  void moveBackward(int speed);
  void turnLeft(int speed);
  void turnRight(int speed);
  void engineStop(void);

// SERVO -----------------------------------------------------------
  int servoUp(servo* ser, int offset);
  int servoDown(servo* ser, int offset);
  void servoZero(servo* ser);
  void servoMax(servo* ser);

void initComponents(void) {
  kicker.pin = 5;
  kicker.pos = 90;
  hook.s.write(hook.pos);
  kicker.s.attach(kicker.pin);
  hook.pin = 3;
  hook.pos = 150;
  hook.s.write(hook.pos);
  hook.s.attach(hook.pin);
  camera1.pin = 6;
  camera1.pos = 170;
  hook.s.write(hook.pos);
  camera1.s.attach(camera1.pin);
  pinMode(A0, OUTPUT);
  analogWrite(A0, 255);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA1, OUTPUT);
  pinMode(ENA2, OUTPUT); 
  pinMode(2, OUTPUT);
  for (int i = 0; i < 5; ++i) {
    pinMode(pins[i], INPUT);
  }
  Serial.begin(9600); // for debugging
}

void lEngine(int speed, bool direction) {
  if (speed > 255) speed = 255;
  if (speed <= 0) speed = 1;

  if (direction) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  }

  analogWrite(ENA1, speed);
}

void rEngine(int speed, bool direction) {
  if (speed > 255)
    speed = 255;
  if (speed <= 0)
    speed = 1;

  if (direction) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }else{
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }

  analogWrite(ENA2, speed);
}

void moveForward(int speed) {
  if (left == true || right == true)  {
    left = false;
    right = false;
    return;
  }
  left = true;
  right = true;
   ld = false;
   rd = false;
}

void moveBackward(int speed) {
  if (left == true || right == true)  {
    left = false;
    right = false;
    return;
  }
  left = true;
  right = true;
  ld = true;
  rd = true;
}

void turnRight(int speed) {
  if (left == true || right == true)  {
    left = false;
    right = false;
    return;
  }
  left = true;
  right = true;
   ld = false;
   rd = true;
}

void turnLeft(int speed) {
  if (left == true || right == true)  {
    left = false;
    right = false;
    return;
  }
  left = true;
  right = true;
   ld = true;
   rd = false;
}

void engineStop(void) {
  left = false;
  right = false;
  ld = false; 
  rd = false;
  lEngine(0, 0);
  rEngine(0, 0);
}

int servoUp(servo* ser, int offset) {
  if (ser->pos > (180 - offset))
    return -1;
  ser->pos += offset;
  ser->s.write(ser->pos); 
  return 0;
}

int servoDown(servo* ser, int offset) {
  if (ser->pos < offset)
    return -1;
  ser->pos -= offset;
  ser->s.write(ser->pos); 
  return 0;
}

void servoSet(servo* ser, int value) {
  ser->pos = value;
  ser->s.write(ser->pos);
}

void servoZero(servo* ser) {
  ser->pos = 0;
  ser->s.write(ser->pos);
}

void servoMax(servo* ser) {
  ser->pos = 180;
  ser->s.write(ser->pos);
}

void kickerKick(servo* ser) {
  for (int i = 0; i < 30; i += 5) {
    servoDown(ser, i);
    delay(15);
  }
  delay(1000);
  for (int i = 0; i < 30; i += 5) {
    servoUp(ser, i);
    delay(15);
  }
}

void setup() {
	initComponents();
	Serial.println("INIT Complete.");
  /*
  lEngine(255, 0);
  delay(5000);
  lEngine(0, 0);
  rEngine(255, 1);
  delay(5000);
  lEngine(255, 1);
  rEngine(255, 0);*/
  //servoZero(&hook);
  left = false;
  right = false;
}

int lastCMD = 0;

int turncounter = 0;

void loop() {
  for (int i = 3; i >= 0; --i) {
    state[i] = digitalRead(pins[i]);
  }

  if (digitalRead(pins[4]) == 1
    && lastCMD == 0
    ) {
    int number = state[0];
    number += (state[1] << 1);
    number += (state[2] << 2);
    number += (state[3] << 3);
    Serial.print(number);
    Serial.println("");
    switch (number) {
      case (1):
        digitalWrite(2, HIGH);
        break;
      case (4):
        moveForward(actualSpeed);
        break;
      case (5):
        servoSet(&hook, 40);
        break;
      case (2):
        turnLeft(actualSpeed);
        break;
      case (3):
        kickerKick(&kicker);
        break;
      case (6):
        turnRight(actualSpeed);
        break;
      case (7):
        actualSpeed = speed[0];
        break;
      case (8):
        actualSpeed = speed[1];
        break;
      case (9):
        actualSpeed = speed[2];
        break;
      case (12):
        digitalWrite(2, LOW);
        break;
      case (13):
        moveBackward(speed[2]);
        break;
      case (10):
        servoSet(&hook, 150);
        break;
      case (11):
        servoUp(&camera1, 20);
        break;
      case (14):
        servoDowno(&camera1, 20);
        break;
      case (15):
        servoSet(&hook, 90);
        break;
      default:
        engineStop();
        break;
      }

      lastCMD = number;
  } else {
    if (digitalRead(pins[4]) == 0) {
      lastCMD = 0;
      //engineStop();
    }
  }

  if (left) {
    if (ld) {
      lEngine(speed, 0);
    } else {
      lEngine(speed, 1);
    }
  }
  else {
    lEngine(0, 0);
  }
  if (right) {
    if (rd) {
      rEngine(speed, 1);
    } else {
      rEngine(speed, 0);
    }
  }
  else {
    rEngine(0, 0);
  }

  if (left == true && right == true && rd != ld && turncounter < 1000) {
    ++turncounter;
    if (turncounter >= 1000) {
      turncounter = 0;
      left = false;
      right = false;
      //engineStop();
    }
  }
}



