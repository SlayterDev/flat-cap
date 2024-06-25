#include <Servo.h>

constexpr auto CMD_PING = "CMD:PING";

constexpr auto CMD_OPEN = "CMD:COVER:OPEN";
constexpr auto CMD_CLOSE = "CMD:COVER:CLOSE";

constexpr auto CMD_LED_ON = "CMD:LED:ON:";
constexpr auto CMD_LED_OFF = "CMD:LED:OFF";
constexpr auto CMD_LED_GET_BRIGHTNESS = "CMD:LED:GETBRIGHT";
constexpr auto RES_LED_BRIGHTNESS = "RES:LED:BRIGHT:";

#define OPEN_POS 200
#define CLOSED_POS 0
#define SERVO_SPEED 30

#define MIN_BRIGHTNESS 0
#define MAX_BRIGHTNESS 255
#define PWM_FREQ 20000

Servo servo;
int servoPin = 9;

byte ledBrightness = 0;
int ledPin = 8;

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
    servo.attach(servoPin);
    state = closed;

    // TODO: Disable built in LEDs

    pinMode(ledPin, OUTPUT);
}

void loop() {
    if (Serial.available() > 0) {
        String cmd = Serial.readStringUntil('\n');
        if (cmd == CMD_PING) {
            handlePing();
            return;
        }
        if (cmd == CMD_OPEN) {
            openCover();
            return;
        }
        if (cmd == CMD_CLOSE) {
            closeCover();
            return;
        }
        if (cmd == CMD_LED_OFF) {
            ledOff();
            return;
        }
        if (cmd.startsWith(CMD_LED_ON)) {
            String arg = cmd.substring(strlen(CMD_LED_ON));
            byte value = (byte)arg.toInt();
            ledOn(value);
            return;
        }
        if (cmd == CMD_LED_GET_BRIGHTNESS) {
            getBrightness();
            return;
        }

        Serial.print("[ERROR] Unknown command:");
        Serial.println(cmd);
    }
}

void handlePing() {
    Serial.println("PONG");
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

    state = closed;
}

void setBrightness() {
    int value = map(ledBrightness, MIN_BRIGHTNESS, MAX_BRIGHTNESS, 0, 1023);
    analogWrite(ledPin, value);
}

void ledOn(byte _brightness) {
    Serial.print("Setting LED brightness to:");
    Serial.println(_brightness);

    ledBrightness = _brightness;
    setBrightness();
}

void ledOff() {
    Serial.println("Turning off LED");
    ledBrightness = 0;
    setBrightness();
}

void getBrightness() {
    Serial.print(RES_LED_BRIGHTNESS);
    Serial.println(ledBrightness);
}
