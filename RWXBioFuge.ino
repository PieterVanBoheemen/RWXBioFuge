// WARNING: Code is in development, not fully tested!!

#include <Wire.h>
#include <SPI.h>
#include <Ethernet.h>
#include "RWXBioFuge.h"
#include <Servo.h>

// lab of things
byte mac[] = {  0x90, 0xA2, 0xDA, 0x0E, 0xD6, 0xEE }; // Mac address
IPAddress server(192,168,1,10); // Server
// Initialize the Ethernet client library
EthernetClient client;

// set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27,16,2);

// Settings
static int Settings[2] = { 0, 0}; // RPM goal, time

// Buttons
boolean Locked = false; // Lid lock
boolean Short = false; // Short spin button
boolean Start = false; // Start button
boolean Stop = false; // Stop button

// Clock vars
uint32_t lastTick = 0; // Global Clock
uint32_t stateStartTime = 0; // Start state Clock
uint32_t StateDt; // Time within a state
uint32_t PhaseStartTime = 0;
int LCDTime = 0;

// Potentiometer pins
int RPMpotPin = 1;
int TimepotPin = 2;

// RPM calculations
int CurrentRPM = 0; // Current average RPM
int PrevRPM = 0; // Previous RPM
double RPMtime = 0; // RPM time
double RPMnow = 0; // Measured RPM
double Gforce = 0; // Calculated GForce
int InfraPin = 8; // Infrared sensor pin

// set initial state
const char* state = "StateProgramming";

// ESC control
Servo esc;

void setup() {
      	// update clock
	lastTick = millis();

	// init I2C
	Wire.begin();

	// open serial connection
	Serial.begin(9600);
        Serial.println("Start");

	// initialize the LCD
	lcd.init();
	lcd.backlight();
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print(F("RWXBioFuge"));
        delay(1000);
        lcd.clear();

	// initialize buttons
	pinMode(3, INPUT); // Start button
	pinMode(4, INPUT); // Short button
	pinMode(5, INPUT); // Limit switch of the lid
        pinMode(6, INPUT); // Stop button
	
	// initialize output
        esc.attach(7);

/*
        // Start Ethernet connection
        if (Ethernet.begin(mac) == 0) 
        {
                Serial.println("Failed to configure Ethernet using DHCP");
        }
        if (client.connect(server, 80)) 
        {
                Serial.println("connected");
                // Make a HTTP request
                client.println("GET /init HTTP/1.0");
                client.println();
                // Print reply
                if (client.available()) 
                {
                        char c = client.read();
                        Serial.print(c);
                }
        } 
        else {
                // if you didn't get a connection to the server:
                Serial.println("connection failed");
        } 
*/
}

void loop() {
	// update clock
	uint32_t time = millis();
	uint16_t dt = time-lastTick;
	lastTick = time;

	// Button updates
	if(digitalRead(3) == LOW) {
		Start = true;
	}
	if(digitalRead(4) == LOW) {
		Short = true;
	}
	if(digitalRead(5) == LOW) {
		Locked = true;
	}
        if(digitalRead(6) == LOW) {
                Stop = true;
        }

        if(Stop == true)
        {
                stateChange("StateRampdown");
                Serial.println("Stop");
        }

	// Machine logic
	machineUpdate(dt);

	// Button updates
	if(digitalRead(3) == HIGH) {
		Start = false;
	}
	if(digitalRead(4) == HIGH) {
		Short = false;
	}
	if(digitalRead(5) == HIGH) {
		Locked = false;
	}
        if(digitalRead(6) == HIGH) {
                Stop = false;
        }

	// Check for panic
	if(state != "StateProgramming" && state != "StatePanic" && state != "StateUnlock")
	{
		if(Locked == false)
		{
			stateChange("StatePanic");
		}
	}
}

void machineUpdate(uint16_t dt) {

	// fixed state machine logic

		if(state == "StateProgramming") {
                        // Arm ESC
                        esc.write(21);
  
			// Responsive to all interactions

			// Time
			int TimepotVal = analogRead(TimepotPin);
                        if(TimepotVal < 120) Settings[1] = map(TimepotVal, 0, 120, 0, 60);
                        else if(TimepotVal < 360) Settings[1] = map(TimepotVal, 120, 300, 60, 300);
                        else if(TimepotVal < 720) Settings[1] = map(TimepotVal, 300, 720, 300, 1800);
                        else if(TimepotVal < 1000) Settings[1] = map(TimepotVal, 720, 1023, 1800, 3600);
                        else if(TimepotVal > 999) Settings[1] = -2;
                        else Settings[1] = 0;

                        lcd.setCursor(0,0);
                        lcd.print("Time");
			lcd.setCursor(6,0);
                        lcd.print(time(Settings[1]));

			// RPM
			Settings[0] = map(analogRead(RPMpotPin), 0, 1000, 0, 100);
                        lcd.setCursor(0,1);
                        lcd.print("Speed");
			lcd.setCursor(6,1);
                        if(Settings[0] < 10) lcd.print(" ");
                        if(Settings[0] < 100) lcd.print(" ");
			lcd.print(Settings[0]);
                        lcd.print("%");

			// If user presses Start button
			if(Start)
			{
                                // To Do: convert pot values to RPM and Time
                                
				stateChange("StateLock");
			}
			// If user presses Short button
			if(Short)
			{
				// Set short
				Settings[0] = 100; // RPM
                                Settings[1] = -1; // Time
				// Continue to next state
				stateChange("StateLock");
			}

		}

  		if(state == "StateLock") {
                        lcd.clear();
			// Check lid before spin
			if(Locked) 
			{
                                // Remap speed setting
                                Settings[0] = map(Settings[0],0,100,30,165);
                                
				// Continue to next state
				stateChange("StateRampup");
			}
			else 
			{
				// Print error
				printInfo("Close the Lid!"," ");
				delay(1000);

				// Contine to previous state
				stateChange("StateProgramming");
			}
		}
		
		if(state == "StateRampup") {
			// Start spinning the rotor

			// Send pulses to ESC
                        for(int i=30;i<Settings[0];i+=10) {
                                esc.write(i);
                                
                                // update LCD
                                measureSpeed(dt);	
			        printStatus(dt);

                                // wait a little
                                delay(300);       
                        }
                        
                        esc.write(Settings[0]);

			// If RPM reaches target
			stateChange("StateSpinSteady");
                        
                        // Check for end of short
			if(Settings[1] < 0) 
			{
				if(!Short && Settings[1] == -1) 
				{
					stateChange("StateRampdown");
				}
			}
		}

		if(state == "StateSpinSteady") {
			// Maintain constant speed
	                measureSpeed(dt);
			printStatus(dt);
	
			// Send pulse to ESC
                        esc.write(Settings[0]);

			// Time in current state
			StateDt = millis() - PhaseStartTime;

			// Check for end of Short or Eternal spin
			if(Settings[1] < 0) 
			{
				if(!Short && Settings[1] == -1) 
				{
					stateChange("StateRampdown");
				}
			}
			else 
			{
				// Spin down after time runs out
				if(StateDt > Settings[1]*1000) 
				{
					stateChange("StateRampdown");
				}
			}
		}

		if(state == "StateRampdown") {
			// Slowly reduce rotor speed

			// Send pulses to ESC
                        for(int i=Settings[0];i>21;i=i-10) {
                                esc.write(i);
                                
                                // update LCD
                                measureSpeed(dt);	
			        printStatus(dt);

                                // wait a little
                                delay(300);       
                        }
                        
			// Send pulse to ESC
                        esc.write(0);

			// Continue when rotor stops
			stateChange("StateUnlock");
		}
	
		if(state == "StateUnlock") {
			// Unlock the system
			
			// Print finish
			printInfo("Done","");
			delay(1000);
			
			// Return to programming mode
			stateChange("StateProgramming");
		}

		if(state == "StatePanic") {
			// Stop rotor
			esc.write(0);

			// Print info
			printInfo("PANIC: Emergency break","Lid opened!");
                        measureSpeed(dt);

			stateChange("StateUnlock");		
		}
	
}

static void measureSpeed(uint16_t dt) 
{ 
        // Derived from code of Karlin Yeh
	PrevRPM = RPMnow;
	RPMtime = pulseIn(InfraPin,HIGH);
	RPMtime+= pulseIn(InfraPin,LOW);
	RPMtime/= 1000000;
	RPMnow = 1 / RPMtime;

	Gforce = 2 * RPMnow * 3.1415;
	Gforce = pow(Gforce,2);
	Gforce *= 0.065;
	Gforce /= 9.8;

	RPMnow *= 60;

	CurrentRPM = (CurrentRPM + RPMnow + PrevRPM) / 3;
}

static void printStatus(uint16_t dt) 
{
        LCDTime += dt;
        if(LCDTime > 1000) {
                LCDTime = 0;
                // Print to LCD
          	lcd.clear();
          	lcd.setCursor(0,0);
          	lcd.print(state);
          	lcd.setCursor(0,1);
          	//lcd.print(F(""));
          	lcd.print(time(StateDt/1000));
          	lcd.print(F(" "));
          	lcd.print(CurrentRPM);
        }

/*
        // Make a HTTP request
        client.print("GET /status?a=Fuge&t=");
        client.print(dt);
        client.print("&rpm=");
        client.print(CurrentRPM);
        client.println(" HTTP/1.0");
        client.println();
*/
}

static void printInfo(char* line1, char* line2) 
{
        // Print to LCD
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print(line1);
	lcd.setCursor(0,1);
	lcd.print(line2);

/*
        // Make a HTTP request
        client.print("GET /status?a=Fuge&l1=");
        client.print(URLEncode(line1));
        client.print("&l2=");
        client.print(URLEncode(line2));
        client.println(" HTTP/1.0");
        client.println();
*/
}

static void stateChange(const char* newstate) 
{
	state = newstate;
        PhaseStartTime = millis();
}

String URLEncode(const char* msg)
{
        // Derived from http://hardwarefun.com/tutorials/url-encoding-in-arduino
        const char *hex = "0123456789abcdef";
        String encodedMsg = "";
        while (*msg!='\0')
        {
                 if( ('a' <= *msg && *msg <= 'z') 
                           || ('A' <= *msg && *msg <= 'Z')
                           || ('0' <= *msg && *msg <= '9') )
                 {
                        encodedMsg += *msg;
                 }
                 else
                 {
                        encodedMsg += '%';
                        encodedMsg += hex[*msg >> 4];
                        encodedMsg += hex[*msg & 15];
                 }
                 msg++;
        }
        return encodedMsg;
}

String time(long val){  
         if(val < 0) 
         {
              return "infinite";
         }
         else 
         {
             int days = elapsedDays(val);
             int hours = numberOfHours(val);
             int minutes = numberOfMinutes(val);
             int seconds = numberOfSeconds(val);
            
              String returnval = "";
            
              // digital clock display of current time 
              returnval = printDigits(hours) + ":" + printDigits(minutes) + ":" + printDigits(seconds);
              
              return returnval;
         }
    }

String printDigits(byte digits){
          // utility function for digital clock display: prints colon and leading 0
          String returnval = "";
          if(digits < 10)
                    returnval += "0";
          returnval += digits; 
         
          return returnval; 
}
