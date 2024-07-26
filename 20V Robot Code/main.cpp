#include "lemlib/api.hpp" // IWYU pragma: keep

// controller 
pros::Controller controller(pros::E_CONTROLLER_MASTER);


// Config the Motors 
pros::MotorGroup left_motors({-1, 2, 3}, pros::MotorGearset::blue); // left motors on ports 1 (1st), 2 (2nd), 3 (3rd)
pros::MotorGroup right_motors({4, -5, -6}, pros::MotorGearset::blue); // right motors on ports 4 (1st), 5 (2nd), 6 (3rd)

// drivetrain settings
lemlib::Drivetrain drivetrain(&left_motor_group, // left motor group
                              &right_motor_group, // right motor group
                              11.7, // 11.7 inch track width
                              lemlib::Omniwheel::NEW_4, // using new 4" omnis
                              450, // drivetrain rpm is 450
                              2 // horizontal drift is 2 (for now)
);

// NOT CURRENTLY ON BOT
pros::Imu imu(10); 

// tracking wheels 
pros::Rotation vertical_rotation_sensor(7); // create a v5 rotation sensor on port 7
pros::Rotation horizontal_rotation_sensor(8); // create a v5 rotation sensor on port 8


// imu
pros::Imu imu(10);
// vertical tracking wheel encoder
pros::Rotation vertical_rotation_sensor(7); // create a v5 rotation sensor on port 7]
// horizontal tracking wheel encoder
pros::Rotation horizontal_rotation_sensor(8); // create a v5 rotation sensor on port 8
/// horizontal tracking wheel
lemlib::TrackingWheel horizontal_tracking_wheel(&horizontal_rotation_sensor, lemlib::Omniwheel::NEW_2, -5.2);
// vertical tracking wheel
lemlib::TrackingWheel vertical_tracking_wheel(&vertical_rotation_sensor, lemlib::Omniwheel::NEW_2, -3.25);

// odometry settings
lemlib::OdomSensors sensors(&vertical_tracking_wheel, // vertical tracking wheel 1, set to null
                            nullptr, // vertical tracking wheel 2, set to nullptr as we are using IMEs
                            &horizontal_tracking_wheel, // horizontal tracking wheel 1
                            nullptr, // horizontal tracking wheel 2, set to nullptr as we don't have a second one
                            &imu // inertial sensor

//arcade drive
pros::Controller controller(pros::E_CONTROLLER_MASTER);

void opcontrol() {
    // loop forever
    while (true) {
        // get left y and right x positions
        int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int rightX = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);

        // move the robot
        chassis.arcade(leftY, rightX);

        // delay to save resources
        pros::delay(25);
    }
}


// PID 
// lateral PID controller
lemlib::ControllerSettings lateral_controller(10, // proportional gain (kP)
                                              0, // integral gain (kI)
                                              3, // derivative gain (kD)
                                              3, // anti windup
                                              1, // small error range, in inches
                                              100, // small error range timeout, in milliseconds
                                              3, // large error range, in inches
                                              500, // large error range timeout, in milliseconds
                                              20 // maximum acceleration (slew)
);

// angular PID controller
lemlib::ControllerSettings angular_controller(2, // proportional gain (kP)
                                              0, // integral gain (kI)
                                              10, // derivative gain (kD)
                                              3, // anti windup
                                              1, // small error range, in degrees
                                              100, // small error range timeout, in milliseconds
                                              3, // large error range, in degrees
                                              500, // large error range timeout, in milliseconds
                                              0 // maximum acceleration (slew)
);


// input curve for throttle input during driver control
lemlib::ExpoDriveCurve throttle_curve(3, // joystick deadband out of 127
                                     10, // minimum output where drivetrain will move out of 127
                                     1.019 // expo curve gain
);

// input curve for steer input during driver control
lemlib::ExpoDriveCurve steer_curve(3, // joystick deadband out of 127
                                  10, // minimum output where drivetrain will move out of 127
                                  1.019 // expo curve gain
);

// create the chassis
lemlib::Chassis chassis(drivetrain,
                        lateral_controller,
                        angular_controller,
//                        sensors,
                        &throttle_curve, 
                        &steer_curve
);



// initialize function. Runs on program startup
void initialize() {
    pros::lcd::initialize(); // initialize brain screen
    chassis.calibrate(); // calibrate sensors
    // print position to brain screen
    pros::Task screen_task([&]() {
        while (true) {
            // print robot location to the brain screen
            pros::lcd::print(0, "X: %f", chassis.getPose().x); // x
            pros::lcd::print(1, "Y: %f", chassis.getPose().y); // y
            pros::lcd::print(2, "Theta: %f", chassis.getPose().theta); // heading
            // delay to save resources
            pros::delay(20);
        }
    });
}
