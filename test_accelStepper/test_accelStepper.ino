
#include <AccelStepper.h>
#define DIRERCTIONAL_PIN 10
#define MOVE_PIN 11

// Define a stepper and the pins it will use
AccelStepper stepper(AccelStepper::FULL2WIRE, MOVE_PIN, DIRERCTIONAL_PIN);
void setup()
{  
  stepper.setMaxSpeed(150);
  stepper.setAcceleration(100);
}
void loop()
{    
  stepper.moveTo(500);
  while (stepper.currentPosition() != 300) // Full speed up to 300
    stepper.run();
  stepper.stop(); // Stop as fast as possible: sets new target
  stepper.runToPosition(); 
  // Now stopped after quickstop
  // Now go backwards
  stepper.moveTo(-500);
  while (stepper.currentPosition() != 0) // Full speed basck to 0
    stepper.run();
  stepper.stop(); // Stop as fast as possible: sets new target
  stepper.runToPosition(); 
  // Now stopped after quickstop
}
