#include "lemlib/api.hpp" // IWYU pragma: keep
# include "main.cpp"

// defining motors 

pros::Motor main_intake (11); // main intake in port 11
pros:: Motor bridge (12); //bridge motor in port 12

// controller 
void opcontrol() {

  while (true) {
    // if R1 is pressed
    if (master.get_digital(E_CONTROLLER_DIGITAL_R1)) { // move intake and bridge if right 1st bumper is pressed
      main_intake.move(100);

      if (master.get_digital(E_CONTROLLER_DIGITAL_R2)) { // if right 2st bumper is also pressed 
        bridge.move(-100);
      } else {
        bridge.move(100);
      }

    }
    else { // stop intake 
      main_intake.move(0);
      bridge.move(100);
    }
    pros::delay(2);

  }

void opcontrol(){

    // if l1 is pressed 
    if (master.get_digital(E_CONTROLLER_DIGITAL_L1)) { // move intake and bridge in reverse if left 1st bumper is pressed
      main_intake.move(-100);
      bridge.move(100)
    }
    else {
      main_intake.move(0);
      bridge.move(100);
    }
    pros::delay(2);
    }


}
