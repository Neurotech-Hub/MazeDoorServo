/* Servo control prototype for maze door -- Kepecs Lab
by Matt Gaidica, PhD (gaidica@wustl.edu)
*/

#include <Servo.h>

Servo myservo;

int closedPos = 90;
int openPos = 90;
int midPos = 90;
int curPos = 90;
int incBy = 5;
int doorDelay = 5; // ms
bool initializationComplete = false;

int PIR_GPIO = 2;
bool pirState = false;

void setup() {
  pinMode(PIR_GPIO, INPUT);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  servoWrite(midPos);
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  Serial.println("\n\n\nHello, Maze.");
  Serial.print("Setting door to mid pos: ");
  Serial.println(midPos);
  Serial.println("Use 'o' or 'c' to set closed pos (then open pos), 'e' to exit...");

  performInitialization();
}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    switch (command) {
      case 'o': // Up arrow command
        Serial.println("Opening door...");
        servoWrite(openPos);
        break;
      case 'c':
        Serial.println("Closing door...");
        servoWrite(closedPos);
        break;
      default:
        break; // do nothing
    }
  }
  
  if (pirState == false && digitalRead(PIR_GPIO) == 0) {
    pirState = true;
    servoWrite(openPos);
  }
  if (pirState && digitalRead(PIR_GPIO)) {
    pirState = false;
  }
}

// this is the only place myservo.write() should be called
void servoWrite(int toPos) {
  // Restrict toPos to the range 0 to 180
  toPos = constrain(toPos, 0, 180);
  if (toPos != curPos) {
    int direction = (toPos > curPos) ? 1 : -1;
    for (int newPos = curPos; newPos != toPos; newPos += direction) {
      myservo.write(newPos);
      delay(doorDelay);
    }
    // Set the final position
    myservo.write(toPos);
    // Update the current position
    curPos = toPos;
  }
}

void performInitialization() {
  int currentPos = 0; // Variable to keep track of which position (closed or open) is being set

  while (!initializationComplete) {
    if (Serial.available() > 0) {
      char command = Serial.read();
      // Serial.println(command);
      // Handle line breaks if received
      if (command == '\n') {
        continue; // Skip processing line breaks
      }
      switch (command) {
        case 'o': // Up arrow command
          if (currentPos == 0) {
            closedPos = min(closedPos + incBy, 180); // Increment closedPos, but limit to 255
            Serial.print("Closed position: ");
            Serial.println(closedPos);
          } else if (currentPos == 1) {
            openPos = min(openPos + incBy, 180); // Increment openPos, but limit to 255
            Serial.print("Open position: ");
            Serial.println(openPos);
          }
          break;
        case 'c': // Down arrow command
          if (currentPos == 0) {
            closedPos = max(closedPos - incBy, 0);   // Decrement closedPos, but not below 0
            Serial.print("Closed position: ");
            Serial.println(closedPos);
          } else if (currentPos == 1) {
            openPos = max(openPos - incBy, 0);   // Decrement openPos, but not below 0
            Serial.print("Open position: ");
            Serial.println(openPos);
          }
          break;
        case 'e': // Enter command
          if (currentPos == 0) {
            Serial.println("Closed position set.");
            currentPos = 1; // Switch to setting openPos
          } else if (currentPos == 1) {
            Serial.println("Open position set. Initialization complete.");
            Serial.print("Closed pos: ");
            Serial.println(closedPos);
            Serial.print("Open pos: ");
            Serial.println(openPos);
            initializationComplete = true;
            // You can add any further code or logic here after initialization
          }
          break;
        default:
          Serial.println("Invalid command.");
      }
      // update position after input
      if (currentPos == 0) {
        servoWrite(closedPos);
      } else {
        servoWrite(openPos);
      }
    }
  }
}
