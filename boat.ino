#include <Servo.h>
#include <stdint.h>
#include <Stepper.h>

Servo winchServo;
Stepper launch_stepper(200, 9, 10, 7, 4);

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(10);

  // boat motors
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(8, OUTPUT);

  // sensor drop
  pinMode(12,OUTPUT);

  // winch
  winchServo.attach(11);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
}

void loop() {
  uint8_t data[2];
  int16_t key;
  int16_t count = 0;

  if (Serial.available() >= 6) {
    count = 0;

    Serial.readBytes(data, 2);
    key = (int16_t)((data[0] << 8) | data[1]);
    
    // run motors
    if (key == 11) {

      Serial.readBytes(data, 2);
      int16_t value1 = (int16_t)((data[0] << 8) | data[1]) * -1; // Motor wiring is reversed
      int left_val = map(abs(value1), 0, 250, 0, 250);

      if (value1 > 0) {
        // Forward - use pin 5
        analogWrite(5, left_val);
        analogWrite(6, 0);
      } else if (value1 < 0) {
        // Reverse - use pin 6
        analogWrite(5, 0);
        analogWrite(6, left_val);
      } else {
        // StopJJ
        analogWrite(5, 0);
        analogWrite(6, 0);
      }

      Serial.readBytes(data, 2);
      int16_t value2 = (int16_t)((data[0] << 8) | data[1]) * -1;
      int right_val = map(abs(value2), 0, 250, 0, 250);

      if (value2 > 0) {
        // Forward - use pin 9
        digitalWrite(8, HIGH);
      } else if (value2 < 0) {
        // Reverse - use pin 10
        digitalWrite(8, LOW);
      } else {
        // Stop
        digitalWrite(8, LOW);
      }

    // sensor drop challenge 5
    } else if (key == 5) {

      Serial.readBytes(data, 2);
      int16_t emag1 = (int16_t)((data[0] << 8) | data[1]);
      Serial.readBytes(data, 2);
      int16_t emag2 = (int16_t)((data[0] << 8) | data[1]);

      digitalWrite(12, HIGH);
      delay(1000);
      digitalWrite(12, LOW);

    } else if (key == 7) {
      Serial.readBytes(data, 2);
      int16_t servoPos = (int16_t)((data[0] << 8) | data[1]);

      winchServo.write(servoPos);
      
      Serial.readBytes(data, 2);
      int16_t winchrot = (int16_t)((data[0] << 8) | data[1]);
      // int winchrotmotor = map(abs(winchrot), 0, 250, 0, 250);

      if (winchrot > 0) {
        // Forward - use pin 9
        digitalWrite(2, HIGH);
        digitalWrite(3, 0);
      } else if (winchrot < 0) {
        // Reverse - use pin 10
        digitalWrite(2, 0);
        digitalWrite(3, HIGH);
      } else {
        // Stop
        digitalWrite(2, 0);
        digitalWrite(3, 0);
      }

    } else if (key == 6) {

      Serial.readBytes(data, 2);
      int16_t launch1 = (int16_t)((data[0] << 8) | data[1]);
      Serial.readBytes(data, 2);
      int16_t launch2 = (int16_t)((data[0] << 8) | data[1]);

      if (launch1 == 1) {
        launch_stepper.setSpeed(100);
        launch_stepper.step(600);
        delay(20);
      }

    }
  }

  count = count + 1;
  if (count > 50) {
    analogWrite(5, 0);
    analogWrite(6, 0);
    digitalWrite(8, LOW);

    winchServo.write(90);
    digitalWrite(2, 0);
    digitalWrite(3, 0);
    launch_stepper.setSpeed(0);
  }

  delay(20);

}
