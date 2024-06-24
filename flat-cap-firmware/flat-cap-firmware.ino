#include <Servo.h>

constexpr auto CMD_OPEN = "CMD:OPEN";
constexpr auto CMD_CLOSE = "CMD:CLOSE";

#define OPEN_POS 200
#define CLOSED_POS 0
#define SERVO_SPEED 30

Servo servo;

enum CoverState {
    open,
    closed
} state;

void setup() {
    Serial.begin(57600);
    while (!Serial) {
        ;
    }
    Serial.flush();

    servo.write(0);
    servo.attach(9);
    state = closed;

    // TODO: Disable built in LEDs

    // TODO: Initialize LED panel
}

void loop() {
    if (Serial.available() > 0) {
        String cmd = Serial.readStringUntil('\n');
        if (cmd == CMD_OPEN) {
            openCover();
        }
        if (cmd == CMD_CLOSE) {
            closeCover();
        }
    }
}

void openCover() {
    Serial.println("Received open command");

    if (state == open) {
        Serial.println("Cover already open");
        return;
    }

    int pos = servo.read();
    for (; pos < OPEN_POS; pos++) {
        servo.write(pos);
        delay(SERVO_SPEED);
    }

    state = open;
}

void closeCover() {
    Serial.println("Received close command");
    
    if (state == closed) {
        Serial.println("Cover already closed");
        return;
    }

    int pos = servo.read();
    for (; pos > CLOSED_POS; pos--) {
        servo.write(pos);
        delay(SERVO_SPEED);
    }

    state = open;
}
