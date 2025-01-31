#include "main.h"
#include "lemlib/api.hpp" // IWYU pragma: keep


/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(2, "I was pressed!");
	} else {
		pros::lcd::clear_line(2);
	}
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	pros::lcd::initialize();
	pros::lcd::set_text(1, "Hello PROS User!");

	pros::lcd::register_btn1_cb(on_center_button);

	
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
	
// controller 
	pros::Controller controller(pros::E_CONTROLLER_MASTER);


	// Config the Motors 
	pros::MotorGroup left_motors({-1, 2, 3}, pros::MotorGearset::blue); // left motors on ports 1 (1st), 2 (2nd), 3 (3rd)
	pros::MotorGroup right_motors({4, -5, -6}, pros::MotorGearset::blue); // right motors on ports 4 (1st), 5 (2nd), 6 (3rd)

	// drivetrain settings
	lemlib::Drivetrain drivetrain(&left_motors, // left motor group
								&right_motors, // right motor group
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
	);							


	


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
		                    sensors,
							&throttle_curve, 
							&steer_curve
	);

	// initialize
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

	
	// drive train code 
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

	// defining motors 

	pros::Motor main_intake (11); // main intake in port 11
	pros:: Motor bridge (12); //bridge motor in port 12


	while (true) {
		// if R1 is pressed
		if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1)) { // move intake and bridge if right 1st bumper is pressed
		main_intake.move(100);

		if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_R2)) { // if right 2st bumper is also pressed 
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

	while(true) {

		// if l1 is pressed 
		if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_L1)) { // move intake and bridge in reverse if left 1st bumper is pressed
			main_intake.move(-100);
			bridge.move(100);
		} 
		else {
			main_intake.move(0);
			bridge.move(100);
		}
		pros::delay(2);
	}
}

