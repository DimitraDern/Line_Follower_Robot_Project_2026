#include <Arduino.h>

// =======================
// INITIALIZATION
// =======================
//Pins 8 and 10 for left and right motor to move forward
#define M1_INA  8    
#define M2_INA  10   

// Sensors ADC Pins
#define LEFT_SENSOR_PIN   A2
#define CENTER_SENSOR_PIN A1
#define RIGHT_SENSOR_PIN  A0

//Speeds for search turn and general speed 
#define BASE_SPEED   255 
#define TURN_SPEED    150 
#define SEARCH_SPEED  235 

//Threshold for black and white line 
#define THRESHOLD     390

//PID Parameters
float Kp = 0.30;
float Ki = 0.00015;
float Kd = 2.2;              

float integral   = 0;
float last_error = 0;

// Number of consecutive white frames
// before entering search mode

#define ALL_WHITE_STOP_FRAMES 18 
int all_white_counter = 0;

// Maximum duration of search mode

#define SEARCH_MAX_FRAMES  80 
#define SEARCH_STOP_FRAMES  20 
int  search_counter = 0;
int  stop_counter   = 0;
bool in_search      = false;

String last_seen         = "center";
bool   started           = false;
bool   seen_partial_line = false;

// =======================
// MOTOR HELPERS
// =======================

//Left and right motor speeds seperate, together and stop together
void motor_left(int speed) {
  analogWrite(M1_INA, constrain(speed, 0, 255));
}

void motor_right(int speed) {
  analogWrite(M2_INA, constrain(speed, 0, 255));
}

void set_speeds(int l_speed, int r_speed) {
  motor_left(l_speed);
  motor_right(r_speed);
}

void motor_stop() {
  analogWrite(M1_INA, 0);
  analogWrite(M2_INA, 0);
}

// =======================
// HELPER Functions
// =======================

// Returns true if sensor detects black
bool is_black(int val) {
  return val > THRESHOLD;
}
//Resets all parameters and counters 
void reset_all() {
  integral          = 0;
  last_error        = 0;
  all_white_counter = 0;
  search_counter    = 0;
  stop_counter      = 0;
  in_search         = false;
  last_seen         = "center";
  started           = false;
  seen_partial_line = false;
}

// =======================
// ERROR FUNCTION
// =======================

//Calculate line position error for PID
float get_error(bool b_l, bool b_c, bool b_r) {
  if ( b_c && !b_l && !b_r) return  0.0;
  if ( b_l && !b_c && !b_r) return -1.0;
  if ( b_r && !b_c && !b_l) return  1.0;
  if ( b_c &&  b_l && !b_r) return -0.5;
  if ( b_c &&  b_r && !b_l) return  0.5;
  if ( b_l &&  b_r && !b_c) return  0.0;
  if ( b_l &&  b_c &&  b_r) return  0.0;
  return last_error;
}

// =======================
// SETUP
// =======================
void setup() {
  // Configure motor pins as outputs
  pinMode(M1_INA, OUTPUT);
  pinMode(M2_INA, OUTPUT);
  motor_stop();
  Serial.begin(9600);
}

// =======================
// LOOP
// =======================

void loop() {
  //Read sensor values and convert to black and white states
  int l = analogRead(LEFT_SENSOR_PIN);
  int c = analogRead(CENTER_SENSOR_PIN);
  int r = analogRead(RIGHT_SENSOR_PIN);

  bool b_l = is_black(l);
  bool b_c = is_black(c);
  bool b_r = is_black(r);

  // At least one sensor detects black
  bool any_black = b_l || b_c || b_r;

  // No sensor detects black
  bool all_white = !any_black;

  // All sensors detect black
  bool all_black = b_l && b_c && b_r;


  // --- START ---
  if (!started) {
    if (any_black) {
      started = true;
    } else {
      motor_stop();
      return;
    }
  }
  if (any_black && !all_black) {
    seen_partial_line = true;
  }

  // --------------------------------------------------
  // END OF TRACK DETECTION
  // --------------------------------------------------
  // If all sensors see black after normal tracking stops immediatiatly and wait
  // for the robot to see white and reset the parameters  for next run
  if (all_black && seen_partial_line) {
    motor_stop();
    while (true) {
      int wl = analogRead(LEFT_SENSOR_PIN);
      int wc = analogRead(CENTER_SENSOR_PIN);
      int wr = analogRead(RIGHT_SENSOR_PIN);
      if (!is_black(wl) && !is_black(wc) && !is_black(wr)) break;
      delay(50);
    }
    reset_all();
    return;
  }

  if (any_black) {
    in_search         = false;
    search_counter    = 0;
    stop_counter      = 0;
    all_white_counter = 0;
    integral          = 0;
    if (b_l) last_seen = "left";
    if (b_r) last_seen = "right";
    if (b_c) last_seen = "center";
  }

  // --- SEARCH MODE ---
  if (in_search) {
    search_counter++;
    if (all_white) stop_counter++;
    else           stop_counter = 0;

    if (search_counter > SEARCH_MAX_FRAMES && stop_counter >= SEARCH_STOP_FRAMES) {
      motor_stop();
      integral   = 0;
      last_error = 0;
      delay(50);
      return;
    }

    if (last_seen == "left") {
      set_speeds(0, SEARCH_SPEED);
    } else if (last_seen == "right") {
      set_speeds(SEARCH_SPEED, 0);
    } else {
      if (last_error <= 0) set_speeds(0, SEARCH_SPEED);
      else                 set_speeds(SEARCH_SPEED, 0);
    }

    delay(5);
    return;
  }

  //If line lost enter search mode and search for the line from the last known location
  if (all_white) {
    all_white_counter++;
    if (all_white_counter >= ALL_WHITE_STOP_FRAMES) {
      in_search      = true;
      search_counter = 0;
      stop_counter   = 0;
    }
    delay(5);
    return;
  } else {
    // Reset counter after detecting line again
    all_white_counter = 0;
  }

  //PID
  float error = get_error(b_l, b_c, b_r);

  integral += error;
  float derivative = error - last_error;
  last_error = error;

  float correction = (Kp * error + Ki * integral + Kd * derivative) * BASE_SPEED;
  // Reduce speed during sharper turns
  float abs_error = abs(error);
  int   speed     = (int)(BASE_SPEED - (BASE_SPEED - TURN_SPEED) * abs_error);

  int left_speed  = speed + (int)correction;
  int right_speed = speed - (int)correction;

  set_speeds(left_speed, right_speed);
  // Small delay for stability
  delay(5);
}
