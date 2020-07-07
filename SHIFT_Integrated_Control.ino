
/*********

About this program

This Arduino program is the host code for SHIFT, team A.I. Tek's mobile furniture-moving system. SHIFT
is a mechanized system consisting of several 6" by 6" by 3" square motorized platforms, called Pods.
A minimum of three Pods are used to move pieces of furniture, which are manually rested atop tbe Pods 
prior to moving. Held in place by thick, high-friction rubber, the furniture is fastened securely to 
the Pods while they move it anywhere, anytime as per a user's request. Upon completion, users can 
either leave their furniture atop the Pods indefinitely, or remove it as they wish. This program is
responsible for making the motorized Pods carry out their movement requests, as well as simultaneously
lookout for unplanned obstacles with their in-house optical sensors, to stop all motion in the case
of unexpected complications. 


In order to control SHIFT, this program, when uploaded to an ESP32 Microcontroller chip, first takes 
in strings of data sent to it via a WiFi GET request from an iOS mobile chatbot application, which 
intakes user commands either in the form of spoken words or written text. Once the mobile chabot has
used its host AI program to process the commands, translate them into strings of data, and sent them 
here through a GET request made when the user submits his or her command, this program further sifts
through the data, processing it and translating it into a form usable by the hardware in the SHIFT 
Pods. It then commands the motors, actuators, and encoders in the physical SHIFT Pods to move them 
according to the custom commands by the user. For instance, the user could say something like, "Move
my couch 3 feet to the right," and the app will translate this into a recognized command, send it 
here, and this program will transform the request into physical motion of the Pods such that they 
move the attached furniture, e.g. couch, 3 feet to its right.

This program is meant to be uploaded to SHIFT Pods prior to placement of furniture on top of the Pods.
When executing, this code instantaneously instructs the Pods to move in their requested manner 
immediately. It can process any number of requests at a time; it simply carries them out one at a 
time. After it has processed the first request, say turn to the right 90 degrees, and is carrying it 
out, it processes the next and puts it next in line - to be immediately executed upon completion of 
the first task. A similar procedure is used for any successive tasks after this. For the sake of our 
ME 310 course demo, only requests with one or two motion tasks are being demonstrated.



Instructions for Use

before running this program, make sure all appropriate drivers, boards, and libraries are installed on your
machine. Once your computer detects your ESP-32S appropriately, open and run the example program 
SimpleWiFiServer to get the IP Address for you ESP32-WiFi duo. Take note of this IP Address. Next, open the
mobile SHIFT app, and be prepared to enter this IP Address in the mobile application.

To run this program, leave the ESP-32S plugged into your laptop or desktop. Then open the serial monitor. 
Then, enter your WiFi name and password into the entry sections indicated below. Once this is complete, 
upload as normal. Wait until the program has finished uploading. You should see a constant output of 
variables and identifiers in the serial monitor window. 

Now go to your mobile iOS application. Open it, follow through the pages, and enter your IP Address when it 
asks for it. Follow the onscreen instructions, and you are good to go! Enjoy controlling the Pods with your
voice or your fingertips. Enjoy!


Important Things to know in order to run this program fully and properly:
  - Update WiFi SSID and password as necessary.
  - Flash the sketch to the ESP8266 board
  - Install Bonjour host software:
    - For Linux, install Avahi (http://avahi.org/).
    - For Windows, install Bonjour (http://www.apple.com/support/bonjour/).
    - For Mac OSX and iOS support is built in through Bonjour already.

*********/

// Code modified for SHIFT usage 
// Beginning of program




////////////////////////////////////// INCLUDE LIBRARIES ////////////////////////////////////////////


#include <Arduino.h>

// If using ESP32
#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
  #include <WiFiClient.h>

#else // Otherwise ESP8266-12e
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
  #include <ESP8266mDNS.h>
  #include <WiFiClient.h>
#endif // End the if statement


// For all cases include
#include <ESPAsyncWebServer.h>
#include <HttpClient.h> 






////////////////////////////////////// VARIABLE DEFINITIONS /////////////////////////////////////////


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ENCODER VARIABLES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 

// Defining GPIO pins for the right and left Pod encoders
const int R_encoder1       = 33;     const int R_encoder2       = 25;
const int L_encoder1       = 34;     const int L_encoder2       = 35;

// Defining variables to help check encoder state
int R_encoder1_curr_analog = 0;  int R_encoder1_prev_analog = 0;  int R_encoder1_diff_analog = 0;
int R_encoder2_curr_analog = 0;  int R_encoder2_prev_analog = 0;  int R_encoder2_diff_analog = 0;

int L_encoder1_curr_analog = 0;  int L_encoder1_prev_analog = 0;  int L_encoder1_diff_analog = 0;
int L_encoder2_curr_analog = 0;  int L_encoder2_prev_analog = 0;  int L_encoder2_diff_analog = 0;

// Defining threshold of the magnitude of jump between white and black lines/surfaces
int R_ENCODER1_THRESHOLD   = 800;  int R_ENCODER2_THRESHOLD   = 950;
int L_ENCODER1_THRESHOLD   = 710;  int L_ENCODER2_THRESHOLD   = 590;

// Defining counter for velocity
int R_encoder1_vel_count   = 0;  int R_encoder2_vel_count   = 0;
int L_encoder1_vel_count   = 0;  int L_encoder2_vel_count   = 0;

// Defining counter for distance
int R_encoder1_dist_count   = 0;  int R_encoder2_dist_count   = 0;
int L_encoder1_dist_count   = 0;  int L_encoder2_dist_count   = 0;




// Defining and initializing variables for our timer
hw_timer_t * timer = NULL;
int COUNTER_UPDATE_PERIOD = 10000; // [10000 us = 10 ms]

// Defining variables to keep track of time that resets every function cycle
unsigned long t = 0;
unsigned long turn_t = 0;


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ MOTOR VARIABLES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Defining GPIO pins for the motor driver
const int R_motor_IN1      = 12;  const int R_motor_IN2      = 13;
const int L_motor_IN1      = 14;  const int L_motor_IN2      = 27;

// Setting PWM properties
const int R_motor_freq1       = 5000;    const int R_motor_freq2       = 5000;
const int R_motor_Channel1    = 0;       const int R_motor_Channel2    = 1;
const int R_motor_resolution1 = 8;       const int R_motor_resolution2 = 8;

const int L_motor_freq1       = 5000;    const int L_motor_freq2       = 5000;
const int L_motor_Channel1    = 2;       const int L_motor_Channel2    = 3;
const int L_motor_resolution1 = 8;       const int L_motor_resolution2 = 8;

// Defining variables used to update the state of both motors
unsigned long current_time = 0;
unsigned long prev_time    = 0;

// Defining current motor write states between (0 - 255 scale)
float MOTOR_INT_STATE       = 120;  // Amount of input to overcome static friction
float R_motor_state1        = MOTOR_INT_STATE;
float R_motor_state2        = 0;

float L_motor_state1        = MOTOR_INT_STATE;
float L_motor_state2        = 0;

// Parameters to acheive straight line motion
// Forward
float R_motor_state_for   = 153;
float L_motor_state_for   = 158;

// Backwards
float R_motor_state_back   = 151;
float L_motor_state_back   = 180;

// Defining variable for distance traveled
float dist_traveled = 0;
float avg_dist_count = 0;


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ WHEEL VARIABLES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Defining wheel parameters
float WHEEL_DIAMETER = 90;                  // [mm] 
float TICKS_PER_REV  = 30;                  // [number of ticks] 
float DIST_MULTIPLIER = 840.0 / 1000.0 ;    // scalar multiplier
float WAIT_TIME       = 4000;               // milliseconds

float dist_to_travel = 0;                   // milliseconds
float ang_to_travel  = 0;                   // milliseconds
int time_to_stop     = 0;                   // milliseconds
float R_turn_time    = 0;                   // milliseconds
float L_turn_time    = 0;                   // milliseconds






// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ APP VARIABLES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Variables that come from the mobile app that define the type of action SHIFT will undergo 

// Initialize these variables here.
// Actionmode set to 1 for forward, 2 for backward, 3 - left, 4 - right, 5 - stop, 6 - STANDBY.
// Initially it is set to 0 for stop (default action mode).
int action_mode = 0; 
String app_command  = "";
int new_command = 1;

// Command length/number/time all set to 0 initially.
int length_of_command = 0;
int number_of_commands = 0;
int current_command = 0;


// Dstance/degrees number (the latter 4 numbers of each 5-number string segment) 
// These are also initialized at 0 for now.
String app_distance = "0";
String app_angle    = "0";


// next create a buffer where we can store the incoming command string
char char_command[64];


// The other variable defines how far the vehicle has to travel or turn
// Create the distance/degrees variables in numbers that the vehicle can process/work with.
// E.g. degrees might not be "90" for a right turn in the vehicle's internal language.
float dist_magnitude = 0;
float angle_magnitude = 0;

// Defining a variable to reset the counter variables
// This is to reset the actions when a task (5-char string) has been fully executed
int reset_count = 0;
int action_start = 0;










/////////////////////////////////// FUNCTION DEFINITIONS ///////////////////////////////////////////

// This timer updates the counter variable for all encoders every 10 milliseconds
void IRAM_ATTR counter_update() {

  // Get current analog value from the encoders
  R_encoder1_curr_analog = analogRead(R_encoder1);
  R_encoder2_curr_analog = analogRead(R_encoder2);

  L_encoder1_curr_analog = analogRead(L_encoder1);
  L_encoder2_curr_analog = analogRead(L_encoder2);




  // Calculate difference between current and previous analog value
  R_encoder1_diff_analog = R_encoder1_curr_analog - R_encoder1_prev_analog;
  R_encoder2_diff_analog = R_encoder2_curr_analog - R_encoder2_prev_analog;

  L_encoder1_diff_analog = L_encoder1_curr_analog - L_encoder1_prev_analog;
  L_encoder2_diff_analog = L_encoder2_curr_analog - L_encoder2_prev_analog;

  // If the difference is greater than the set threshold, then the wheel has rotated by a tick
  if (abs(R_encoder1_diff_analog) >= R_ENCODER1_THRESHOLD) {
    R_encoder1_vel_count += 1;
    R_encoder1_dist_count += 1;
  }

  // If the difference is greater than the set threshold, then the wheel has rotated by a tick 
  if (abs(R_encoder2_diff_analog) >= R_ENCODER2_THRESHOLD) {
    R_encoder2_vel_count += 1;
    R_encoder2_dist_count += 1;
  }

  // If the difference is greater than the set threshold, then the wheel has rotated by a tick
  if (abs(L_encoder1_diff_analog) >= L_ENCODER1_THRESHOLD) {
    L_encoder1_vel_count += 1;
    L_encoder1_dist_count += 1;
  }

  // If the difference is greater than the set threshold, then the wheel has rotated by a tick
  if (abs(L_encoder2_diff_analog) >= L_ENCODER2_THRESHOLD) {
    L_encoder2_vel_count += 1;
    L_encoder2_dist_count += 1;
  }



  // If so, now set the previous reading equal to the current reading
  R_encoder1_prev_analog = R_encoder1_curr_analog;
  R_encoder2_prev_analog = R_encoder2_curr_analog;

  L_encoder1_prev_analog = L_encoder1_curr_analog;
  L_encoder2_prev_analog = L_encoder2_curr_analog;

  // Update time variable
  t += 10;                                  // [milliseconds]
  turn_t += 10;                             // [milliseconds]
}




// This function calculates the total distance travelled in the current motion order
// It calculates the distance based on the current number of ticks counted 
// It uses the language of the motors and encoders to calculate distances
void calc_dist() {

  // Averaging counts from all four encoders
  avg_dist_count = (R_encoder1_dist_count + R_encoder2_dist_count +
                    L_encoder1_dist_count + L_encoder2_dist_count) / 4.0;
  
  // Calculate distance traveled [mm]
  dist_traveled = PI * WHEEL_DIAMETER * avg_dist_count / TICKS_PER_REV;   // [mm]

}





// This function sets the motor parameters to achieve a STOP action
void action_stop() {
  
  // Set all motor states to zero
  // Update speed of right motor
  ledcWrite(R_motor_Channel1, 0);
  ledcWrite(R_motor_Channel2, 0);

  // Update speed of left motor 
  ledcWrite(L_motor_Channel1, 0);
  ledcWrite(L_motor_Channel2, 0);
}





// This function sets the motor parameters to achieve a FORWARD action
void action_forward() {
  
  // Update speed of right motor
  ledcWrite(R_motor_Channel1, 0);
  ledcWrite(R_motor_Channel2, R_motor_state_for);

  // Update speed of left motor
  ledcWrite(L_motor_Channel1, 0);
  ledcWrite(L_motor_Channel2, L_motor_state_for);
}





// This function sets the motor parameters to achieve a BACKWARD action
void action_backward() {
  
  // Update speed of right motor
  ledcWrite(R_motor_Channel1, R_motor_state_back);
  ledcWrite(R_motor_Channel2, 0);

  // Update speed of left motor
  ledcWrite(L_motor_Channel1, L_motor_state_back);
  ledcWrite(L_motor_Channel2, 0);
}





// This function sets the motor parameters to achieve a TURN LEFT action
void action_turn_left() {
  
  // Update speed of right motor
  ledcWrite(R_motor_Channel1, 0);
  ledcWrite(R_motor_Channel2, R_motor_state_for);

  // Update speed of left motor
  ledcWrite(L_motor_Channel1, L_motor_state_back);
  ledcWrite(L_motor_Channel2, 0);
}





// This function sets the motor parameters to achieve a TURN RIGHT action
void action_turn_right() {
  
  // Update speed of right motor
  ledcWrite(R_motor_Channel1, R_motor_state_back);
  ledcWrite(R_motor_Channel2, 0);

  // Update speed of left motor
  ledcWrite(L_motor_Channel1, 0);
  ledcWrite(L_motor_Channel2, L_motor_state_for);
}






// This function resets all the states
void reset_states() {
  
  // Reset all distance counters
  dist_traveled = 0;
  R_encoder1_dist_count = 0;
  R_encoder2_dist_count = 0;
  L_encoder1_dist_count = 0;
  L_encoder2_dist_count = 0;

  // Reset all time counters
  turn_t = 0;
  t = 0;
}






// This function prints out information about the current states to the serial monitor
void print_state() {
  
  // Old code for testing/debugging purposes
  //Serial.print("| AvgCt: " + String(avg_dist_count));
  //Serial.print(" | D: " + String(dist_traveled));
  //Serial.print(" | t: " + String(t));
  //Serial.print(" | A: " + String(4920.0*turn_t/90.0));

  // Current active printing
  Serial.print(" | Ddes: " + String(dist_to_travel));
  Serial.print(" | Ades: " + String(ang_to_travel));
  Serial.print(" | t: " + String(t));

  // Old code for testing/debugging purposes
  // Serial.print(" | R: " + String(R_turn_time));
  // Serial.print(" | L: " + String(L_turn_time));

  // Current active printing
  Serial.print(" | tt: " + String(turn_t));
  Serial.print(" | D: " + String(dist_traveled));

  // Old code for testing/debugging purposes
  //  Serial.print( " | CC:" + String(current_command) );
  //  Serial.print( " | NC:" + String(new_command) );

  // Current active printing
  Serial.println(" | AM: "  + String(action_mode) );

}







// This function sifts through the incoming string commands 
// It decides which part of the command is used when
void interpret_command() {


  // only execute read if current_command is less than number_of_commands
  // Essentially telling the hardware to only read the next-in-line command
  // Also only read new command once previous command has been fully exectued and the hardware
  //    has been set to standby mode.
  if (current_command < number_of_commands && new_command == 0) {

    // The location of the current command will be updated after every motion request finishes
    // Depending on the command location, we define various values

    // The first value of every command is the action_mode of the system
    // Stop, F, B, L, R, STANDBY
    String first = (String)char_command[5 * current_command];
    action_mode = first.toInt();

    // The next four values correspond to the magnitude of distance or angle depending on case
    String second = (String)char_command[5 * current_command + 1] +
    (String)char_command[5 * current_command + 2] +
    (String)char_command[5 * current_command + 3] +
    (String)char_command[5 * current_command + 4];

    // If the first value of the current command is 1 or 2 we save to distance
    if (action_mode == 1 || action_mode == 2) {
      
      // save magnitude to distance
      dist_to_travel = second.toFloat();
    }
  

    // If the first value of the current command is a 3 or 4 we save to angle
    else if (action_mode == 3 || action_mode == 4) {
      // save magnitude to angle
      ang_to_travel = second.toFloat();

      // converting to corresponding signals
      // Convert to language of device
      R_turn_time = 4920.0 * ang_to_travel / 90.0; // 4920 [milliseconds] equals 90 degrees right
      L_turn_time = 4942.0 * ang_to_travel / 90.0; // 4920 [milliseconds] equals 90 degrees left
    }

    // If the first value of the current command is 5, then we save as stop.
    else if (action_mode == 5){
      
    // save magnitude to time 
    time_to_stop = second.toInt();
    
    }
  } // End of inner if-statement

  // After linked command is finished we set the system to STANDBY
  // System is now ready to intake a new command
  else{
    // Reset command variables
    new_command = 1;
    current_command = 0;
    action_mode = 6;
  }

} // End of interpret_command function














//////////////////////////////////// WIFI REQUEST SECTION ///////////////////////////////////////////

// Start webserver for incoming url requests from app
AsyncWebServer server(80);



// REPLACE WITH YOUR NETWORK CREDENTIALS
// Constants that will not change
const char* ssid = "spiritlakeiowa1914"; // WiFi Network Name
const char* password = "fig4five6seven"; // WiFi Network Password


// Set up input parameters for the intermediate web app
//    We do not see this anymore because of the iOS app
// input 1 is for the strings of data to be sent to the ESP8266-12e
//    but IT IS NOT THE ACTUAL VALUE
const char* PARAM_INPUT_1 = "input1";

// Other parameters that set up the stage for location services and
//    other stuf down the road that we have not done yet.
const char* PARAM_INPUT_2 = "input2";
const char* PARAM_INPUT_3 = "input3";
const char* PARAM_INPUT_4 = "input4";


// Initialize string and const char array for IP Address
String IPAddressString = "";
const char* IPAddressChar = "";

// Initialize the variable to store in incoming app command string as a global variable
String inputVariable1 = "0";






// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ HTML CODE SECTION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Create an HTML web page to handle 4 input fields 
//    (input1, input2, input3, input4)
const char index_html[] PROGMEM = R"rawliteral(

// HTML webserver file
// This file creates a virtual webserver at the IP Address created by the ESP32 when hooked up to 
//    the user's WiFi network. 

// This webserver contains four bins for four different types of requests to be received in this code
//    via WiFi, and they correspond to data as follows:
//      1. Bin 1 - SHIFT motion commands from mobile iOS application
//          (the incoming app command strings that consist of 5, 10, 15, etc. numbers)
//      2. Bin 2 - SHIFT location services coordinate 1 (currently unused)
//      3. Bin 3 - SHIFT location services coordinate 2 (currently unused)
//      4. Bin 4 - SHIFT location services coordinate 3 (currently unused)

// Create this HTML Webserver file
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>

  // GET request bin 1 - incoming app command string
  <form action="/get">
    input1: <input type="text" name="input1">
    <input type="submit" value="Submit">
  </form><br>

  // GET request bin 2 - location coordinate 1 (unused yet)
  <form action="/get">
    input2: <input type="text" name="input2">
    <input type="submit" value="Submit">
  </form><br>

  // GET request bin 3 - location coordinate 2 (unused yet)
  <form action="/get">
    input3: <input type="text" name="input3">
    <input type="submit" value="Submit">
  </form><br>

  // GET request bin 4 - location coordinate 3 (unused yet)
  <form action="/get">
    input4: <input type="text" name="input4">
    <input type="submit" value="Submit">
  </form>
  
</body></html>)rawliteral";
// End of the HTML page code




// Just in case if the request sent to the HTML webpage from the application does not match anything
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}






//////////////////////////////////// SETUP REQUEST SECTION //////////////////////////////////////////

// Setup section of the code
void setup() {
  
  // Begin loggin in the serial output baud rate
  Serial.begin(115200);


  // Initialize encoder analog inputs
  R_encoder1_curr_analog = analogRead(R_encoder1);
  R_encoder1_prev_analog = R_encoder1_curr_analog;
  R_encoder2_curr_analog = analogRead(R_encoder2);
  R_encoder2_prev_analog = R_encoder2_curr_analog;

  L_encoder1_curr_analog = analogRead(L_encoder1);
  L_encoder1_prev_analog = L_encoder1_curr_analog;
  L_encoder2_curr_analog = analogRead(L_encoder2);
  L_encoder2_prev_analog = L_encoder2_curr_analog;


  // Setting up the timer loop to update counter
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &counter_update, true);
  timerAlarmWrite(timer, COUNTER_UPDATE_PERIOD, true);
  timerAlarmEnable(timer);


  // Configure PWM functionalitites 
  ledcSetup(R_motor_Channel1, R_motor_freq1, R_motor_resolution1);
  ledcSetup(R_motor_Channel2, R_motor_freq2, R_motor_resolution2);
  ledcSetup(L_motor_Channel1, L_motor_freq1, L_motor_resolution1);
  ledcSetup(L_motor_Channel2, L_motor_freq2, L_motor_resolution2);

  // Attach the motor channels to the GPIO so it can control them
  ledcAttachPin(R_motor_IN1, R_motor_Channel1);
  ledcAttachPin(R_motor_IN2, R_motor_Channel2);
  ledcAttachPin(L_motor_IN1, L_motor_Channel1);
  ledcAttachPin(L_motor_IN2, L_motor_Channel2);



  // Reset time variable
  t = 0;
  turn_t = 0;






  ///////////////////////////////////// WIFI CONNECTION STUFF ///////////////////////////////////////

  // Connect to WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // If fails to connect to WiFi, print that in the logger
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }

  // If otherwise (it works), print out the IP Address
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());





  /////////////////////////////////// GET REQUEST CALLING SECTION ///////////////////////////////////

  // Make the web page with input fields accessible to the client
  //    In other words, deploy it
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;

    // Initialize the unused input variables to be stored as placeholders in the unused location bins
    String inputVariable2 = "0";
    String inputVariable3 = "0";
    String inputVariable4 = "0";






    /////////////////////////////////// GET REQUEST BINS SECTION ////////////////////////////////////



    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ BIN 1 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
    
    // If GET request to the first bin (input 1, app command)
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;

      // This is the variable that stores the incoming 5-, 10-, 15-, etc.- long app command string
      inputVariable1 = inputMessage;

      // Print the app command string to verify it was received by the code
      Serial.println(inputVariable1);



      // If a new command has just been requested
      if (new_command == 1) {

        // Assign the app command the 5-, 10-, 15-, etc.- long string to a new variable
        app_command = inputVariable1;

        // converting String into a character array to allow indexing 
        // Convert the 5-character string to char
        app_command.toCharArray(char_command, 100);

        // Print to console
        Serial.print(" | " + String(char_command) + " | ");

        // storing length of character array 
        // 5 or 10 in example demonstrated iOS app case
        length_of_command = strlen(char_command);

        // We know that the linked command always comes in a multiple of 5,
        // therefore the number of commands that need to be executed are: 
        // Will be 1 or 2 in iOS app case
        number_of_commands = length_of_command / 5;

        // set recieving new command to 0 so that the value of app_command
        // does not change until the command has finished executing        
        new_command = 0;
      
      }
      

    }



    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ BIN 2 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
    // Retrieve request from user (client) through the second bin
    //    (location stuff, unused currently)
    // GET input2 value on <ESP_IP>/get?input2=<inputMessage>
    else if (request->hasParam(PARAM_INPUT_2)) {
      inputMessage = request->getParam(PARAM_INPUT_2)->value();
      inputParam = PARAM_INPUT_2;

      // Location-string-storing variable made
      inputVariable2 = inputMessage;
      Serial.println(inputVariable2);
    }



    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ BIN 3 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
    
    // Retrieve request from user (client) through the third bin
    //    (location stuff, unused currently)
    // GET input3 value on <ESP_IP>/get?input3=<inputMessage>
    else if (request->hasParam(PARAM_INPUT_3)) {
      inputMessage = request->getParam(PARAM_INPUT_3)->value();
      inputParam = PARAM_INPUT_3;

      // Location-string-storing variable made
      inputVariable3 = inputMessage;
      Serial.println(inputVariable3);
    }



    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ BIN 4 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
    
    // Retrieve request from user (client) through the fourth bin
    //    (location stuff, unused currently)
    // GET input4 value on <ESP_IP>/get?input4=<inputMessage>
    else if (request->hasParam(PARAM_INPUT_4)) {
      inputMessage = request->getParam(PARAM_INPUT_4)->value();
      inputParam = PARAM_INPUT_4;

      // Location-string-storing variable made
      inputVariable4 = inputMessage;
      Serial.println(inputVariable4);
    }



    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ IF NO BIN ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
    
    // If no message (request) sent from the user at the given time
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }






    // Actually send GET request to the created HTML webserver
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
                                     + inputParam + ") with value: " + inputMessage +
                                     "<br><a href=\"/\">Return to Home Page</a>");
  });
  server.onNotFound(notFound);
  server.begin();
}









//////////////////////////////////// MAIN LOOP - ALWAYS RUNNING /////////////////////////////////////


// Code for actually carrying out the ordered Pod commands goes here
// Also location services code would go here if later implemented
void loop() {


  // Old code used to test reception of incoming app command request in real time
  // Serial.println(inputVariable1);




  
  // interpret command to get desired action and distance values
  interpret_command();
  

  // After interpreting commands, execute command based each segment's action mode parameter





  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FORWARD ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
  
  // If moving forward, and distance traveled is less than the full requested distance
  if (dist_traveled <= dist_to_travel * DIST_MULTIPLIER && action_mode == 1) {

    // Print information about current states
    Serial.print(" FOR | ");
    print_state();

    // Calculate current value of dist_traveled
    calc_dist();

    // Move forward 
    action_forward();

    // Once the Pod has traveled the desired distance update current_command, reset all the states
    if (dist_traveled >= dist_to_travel * DIST_MULTIPLIER - 50) {
      current_command++;
      reset_states();
    }

  } 
  // End of FORWARD if-statement





  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ BACKWARD ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
  
  // If moving backward and distance traveled is less than the full requested distance
  else if (dist_traveled <= dist_to_travel * DIST_MULTIPLIER && action_mode == 2) {

    // Print information about current states
    Serial.print(" BACK | ");
    print_state();

    // Calculate current value of dist_traveled
    calc_dist();

    // Move backward
    action_backward();

    // Once the Pod has traveled the desired distance update current_command, reset all the states
    if (dist_traveled >= dist_to_travel * DIST_MULTIPLIER - 50) {
      current_command++;
      reset_states();
    }

  } 
  // End of BACKWARD if else-statement





  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ TURN LEFT ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
  
  // If turning left and turn has not been completed in time
  else if (turn_t <= L_turn_time && action_mode == 3) {

    // Print information about current states
    Serial.print(" LEFT | ");
    print_state();

    // Turn left
    action_turn_left();

    // Once the Pod has traveled the desired distance update current_command, reset all the states 
    if (turn_t <= L_turn_time && turn_t >= L_turn_time - 10) {
      current_command++;
      reset_states();
    }
  } 
  // End of TURN LEFT if else-statement





  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ TURN RIGHT ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
  
  // If turning right and turn has not yet been completed in time
  else if (turn_t <= R_turn_time && action_mode == 4) {

    // Print information about current states 
    Serial.print(" RIGHT | ");
    print_state();

    // Turn left
    action_turn_right();

    // Once the Pod has traveled the desired distance update current_command, reset all the states
    if (turn_t <= R_turn_time && turn_t >= R_turn_time - 10)  {
      current_command++;
      reset_states();
    }
  } 
  // End of TURN RIGHT if else-statement



  

  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ STOP ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
  else if (t <= time_to_stop && action_mode == 5) {

    // Print information about current states 
    Serial.print(" STOP | ");
    print_state();

    // Undergo stop algorithm 
    action_stop();

    if (t <= time_to_stop && t >= time_to_stop - 5) {
      current_command++;
      reset_states();
    }
  } 
  // End of STOP if else-statement 





  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ STANDBY ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
  
  // If not in any mode - in STANDBY waiting for command
  else {
    Serial.print(" STANDBY | ");
    print_state();

    // Undergo stop algorithm
    action_stop();
    reset_states();

 
  }
  // End of STANDY else-statement 




  // Location stuff starts here

  // Not attempted yet

  // Location stuff ends here
  
}

///////////////////////////////////////// END OF MAIN LOOP //////////////////////////////////////////


////////////////////////////////////////// END OF PROGRAM ///////////////////////////////////////////
