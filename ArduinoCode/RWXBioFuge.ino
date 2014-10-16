// WARNING: Code is in development, not fully tested!!

#include <Wire.h>
#include <SPI.h>
#include <Ethernet.h>
#include "avr/pgmspace.h" // new include
#include "RWXBioFuge.h"
#include <Servo.h>
#include "WebServer.h"

// rotary counter
#include <ByteBuffer.h>
#include <ooPinChangeInt.h>
//#define DEBUG
//#ifdef DEBUG
//ByteBuffer printBuffer(200);
//#endif
#include <AdaEncoder.h>
#define ENCA_a 8
#define ENCA_b 9
#define ENCB_a A0
#define ENCB_b A1
AdaEncoder encoderA = AdaEncoder('a', ENCA_a, ENCA_b);
AdaEncoder encoderB = AdaEncoder('b', ENCB_a, ENCB_b);
int8_t clicks=0;
char id=0;
int time_counter = 0;
int rpm_counter = 0;

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

/* LEGACY CODE for potmeters 
// Potentiometer pins
// int RPMpotPin = 1;
// int TimepotPin = 2; */

// RPM calculations
int CurrentRPM = 0; // Current average RPM
int PrevRPM = 0; // Previous RPM
double RPMtime = 0; // RPM time
double RPMnow = 0; // Measured RPM
double Gforce = 0; // Calculated GForce
int InfraPin = 2; // Infrared sensor pin

// set initial state
const char* state = "StateProgramming";

// ESC control
Servo esc;

// Panic settings
boolean breakopp = false;

// network configuration.  gateway and subnet are optional.
/* Mac Address */
static uint8_t mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0xDE, 0xDE };
/* IP Address */
static uint8_t ip[] = { 192, 168, 1, 210 };
// ROM-based messages used by the application, saves RAM
P(Page_start) = "<html><head><title>RWXBioFuge</title></head><body><h1>RWXBioFuge Webcontrol</h1><hr /><h2>Commands</h2><table><tbody><tr><th>1 = Time</th><td>set length of centrifugation</td></tr><tr><th>2 = RPM</th><td>Speed in %</td></tr><tr><th>3 = Start</th><td>1 to start</td></tr><tr><th>4 = Stop</th><td>1 to stop</td></tr></tbody></table><br /><hr /><h2>Send commands to RWXBioFuge</h2><form action='index.html' method='get'>Time (seconds): <input type='text' name='1' value='0' /><br />Speed (0-100%): <input type='text' name='2' value='0' /><br /><button name='3' type='submit' value='1'>Start</button> <button name='4' type='submit' value='1'>Stop</button></form><hr /><iframe src='status.html' height='180'></iframe><hr /> \n";
P(Status_start) = "<html><head><meta http-equiv='refresh' content='2'></head><body><h2>Status</h2><table><tbody><tr><td>Set Time</td><td>";
P(Status_2) = "</td></tr><tr><td>Set Speed</td><td>";
P(Status_3) = "</td></tr><tr><td>Current Speed</td><td>";
P(Status_4) = "</td></tr><tr><td>Time remaining</td><td>";
P(Status_5) = "</td></tr></tbody></table></body></html>";
P(Page_end) = "</body></html>";
P(Get_head) = "<h3>GET from ";
P(Post_head) = "<h3>POST to ";
P(Unknown_head) = "<h3>UNKNOWN request for ";
P(Default_head) = "unidentified URL requested.</h3>\n";
P(Parsed_head) = "index.html requested.</h3>\n";
P(Good_tail_begin) = "Commands received via URL tail = '";
P(Bad_tail_begin) = "INCOMPLETE URL tail = '";
P(Tail_end) = "'<br>\n";
P(Parsed_tail_begin) = "Parameters:<br>\n";
P(Parsed_item_separator) = " = '";
P(Params_end) = "End of parameters<br>\n";
P(Post_params_begin) = "Parameters sent by POST:<br>\n";
P(Line_break) = "<br>\n";
/* This creates an instance of the webserver.  By specifying a prefix
 * of "", all pages will be at the root of the server. */
#define PREFIX ""
WebServer webserver(PREFIX, 80);
#define NAMELEN 2
#define VALUELEN 32
#define WEBDUINO_FAIL_MESSAGE "<h1>Request Failed</h1>"
int webstart = 0;
int webstop = 0;

void parsedCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  URLPARAM_RESULT rc;
  char name[NAMELEN];
  char value[VALUELEN];

  /* this line sends the standard "we're all OK" headers back to the
     browser */
  server.httpSuccess();

  /* if we're handling a GET or POST, we can output our data here.
     For a HEAD request, we just stop after outputting headers. */
  if (type == WebServer::HEAD)
    return;

  server.printP(Page_start);
 
  switch (type)
    {
    case WebServer::GET:
        server.printP(Get_head);
        break;
    case WebServer::POST:
        server.printP(Post_head);
        break;
    default:
        server.printP(Unknown_head);
    }

    server.printP(Parsed_head);
    server.printP(tail_complete ? Good_tail_begin : Bad_tail_begin);
    server.print(url_tail);
    server.printP(Tail_end);

  if (strlen(url_tail))
    {
    server.printP(Parsed_tail_begin);
    while (strlen(url_tail))
      {
      rc = server.nextURLparam(&url_tail, name, NAMELEN, value, VALUELEN);
      if (rc == URLPARAM_EOS)
        server.printP(Params_end);
       else
        {
        /* if(atoi(name) == 1) { server.print(F("Time")); }
        else if(atoi(name) == 2){ server.print(F("RPM")); }
        else if(atoi(name) == 3){ server.print(F("Webstart")); }
        else if(atoi(name) == 4){ server.print(F("Webstop")); }
        else { server.print(name); }
        server.printP(Parsed_item_separator);
        server.print(value);
        server.printP(Tail_end); */
        parseVars(server, name, value);
        
        //if(atoi(name) == 1) Settings[1] = atoi(value);
        //if(atoi(name) == 2) Settings[0] = atoi(value);
        //if(atoi(name) == 3) webstart = atoi(value);
        //if(atoi(name) == 4) webstop = atoi(value);
        storeValue(name, value); 
        }
      }
    }
  if (type == WebServer::POST)
  {
    server.printP(Post_params_begin);
    while (server.readPOSTparam(name, NAMELEN, value, VALUELEN))
    {
      /*if(atoi(name) == 1) { server.print(F("Time")); }
      else if(atoi(name) == 2){ server.print(F("RPM")); }
      else if(atoi(name) == 3){ server.print(F("Webstart")); }
      else if(atoi(name) == 4){ server.print(F("Webstop")); }
      else { server.print(name); }
      server.printP(Parsed_item_separator);
      server.print(value);
      server.printP(Tail_end); */
      parseVars(server, name, value);
  
      /* if(atoi(name) == 1) Settings[1] = atoi(value);
      if(atoi(name) == 2) Settings[0] = atoi(value);
      if(atoi(name) == 3) webstart = atoi(value); 
      if(atoi(name) == 4) webstop = atoi(value); */ 
      storeValue(name, value); 
    }
  }
    
  server.printP(Page_end);

}

void storeValue(char name[32], char value[32]) {
      if(atoi(name) == 1) Settings[1] = atoi(value);
      if(atoi(name) == 2) Settings[0] = atoi(value);
      if(atoi(name) == 3) webstart = atoi(value); 
      if(atoi(name) == 4) webstop = atoi(value); 
}

void parseVars(WebServer &server, char name[32], char value[32]) {
      if(atoi(name) == 1) { server.print(F("Time")); }
      else if(atoi(name) == 2){ server.print(F("RPM")); }
      else if(atoi(name) == 3){ server.print(F("Start")); }
      else if(atoi(name) == 4){ server.print(F("Stop")); }
      else { server.print(name); }
      server.printP(Parsed_item_separator);
      server.print(value);
      server.printP(Tail_end); 
}

void statusCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  server.printP(Status_start);
  server.print(Settings[1]);
  server.printP(Status_2);
  server.print(Settings[0]);
  server.printP(Status_3); 
  server.print(CurrentRPM);
  server.printP(Status_4); 
  if(state != "StateProgramming" && state != "StatePanic" && state != "StateUnlock") {
    server.print(time(Settings[1] - StateDt/1000));
  }
  else {
    server.print(0);
  }
  server.printP(Status_5); 
}

void my_failCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  /* this line sends the "HTTP 400 - Bad Request" headers back to the
     browser */
  server.httpFail();

  /* if we're handling a GET or POST, we can output our data here.
     For a HEAD request, we just stop after outputting headers. */
  if (type == WebServer::HEAD)
    return;

  server.printP(Page_start); 
/*  switch (type)
    {
    case WebServer::GET:
        server.printP(Get_head);
        break;
    case WebServer::POST:
        server.printP(Post_head);
        break;
    default:
        server.printP(Unknown_head);
    }

    server.printP(Default_head);
    server.printP(tail_complete ? Good_tail_begin : Bad_tail_begin);
    server.print(url_tail);
    server.printP(Tail_end); */
    server.printP(Page_end);

}

void setup() {
      	// update clock
	lastTick = millis();

	// init I2C
	Wire.begin();

	// open serial connection
	Serial.begin(9600);
        Serial.println(F("Start"));

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
       
        /* initialize the Ethernet adapter */
        Ethernet.begin(mac, ip);
        /* setup our default command that will be run when the user accesses
         * the root page on the server */
        webserver.setDefaultCommand(&parsedCmd);
        /* setup our default command that will be run when the user accesses
         * a page NOT on the server */
        webserver.setFailureCommand(&my_failCmd);
        /* run the same command if you try to load /index.html, a common
         * default page name */
        webserver.addCommand("index.html", &parsedCmd);
        webserver.addCommand("status.html", &statusCmd);
        /* start the webserver */
        webserver.begin();
        
        // initialize output
        esc.attach(7);

}

void loop() {
	// update clock
	uint32_t time = millis();
	uint16_t dt = time-lastTick;
	lastTick = time;   

        // Rotary encoders
        //char outChar;
        //while ((outChar=(char)printBuffer.get()) != 0) Serial.print(outChar);
        AdaEncoder *thisEncoder=NULL;
        thisEncoder=AdaEncoder::genie();
        if (thisEncoder != NULL) {
          //Serial.print(thisEncoder->getID()); Serial.print(':');
          clicks=thisEncoder->query();
          if (clicks > 0) {
            //Serial.println(" CW");
            if(thisEncoder->getID() == 'a') time_counter += 1;
            if(thisEncoder->getID() == 'b') rpm_counter += 1;
            //Serial.println(acounter);
            //Serial.println(bcounter);
          }
          if (clicks < 0) {
            // Serial.println(" CCW");
            if(thisEncoder->getID() == 'a') time_counter -= 1;
            if(thisEncoder->getID() == 'b') rpm_counter -= 1;
            //Serial.println(acounter);
            //Serial.println(bcounter);       
          }
          if(time_counter < 0) time_counter = 0;
          if(rpm_counter < 0) rpm_counter = 0;
          if(rpm_counter > 100) rpm_counter = 100; 
        }

        // Ethernet
        char buff[64];
        int len = 64;
        /* process incoming connections one at a time forever */
        webserver.processConnection(buff, &len);
                    
	// Button updates
	if(digitalRead(3) == HIGH) {
		Start = true;
	}
        if(webstart == 1) {
                Start = true;
        }
	if(digitalRead(4) == LOW) {
		Short = true;
	}
	if(digitalRead(5) == HIGH) {
		Locked = true;
	}
        if(digitalRead(6) == HIGH) {
                Stop = true;
        }
        if(webstop == 1) {
                Stop = true;
        }

        if(Stop == true)
        {
                stateChange("StateRampdown");
        }

	// Machine logic
	machineUpdate(dt);

	// Button updates
	if(digitalRead(3) == LOW) {
		Start = false;
	}
        if(webstart == 1) {
                Start = false;
                webstart = 0;
        }
	if(digitalRead(4) == HIGH) {
		Short = false;
	}
	if(digitalRead(5) == LOW) {
		Locked = false;
	}
        if(digitalRead(6) == LOW) {
                Stop = false;
        }
        if(webstop == 1) {
                Stop = false;
                webstop = 0;
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

                        // TIME
                        //int timec = (int) time_counter;
                        if(time_counter < 30) Settings[1] = map(time_counter, 0, 30, 0, 30);
                        else if(time_counter < 36) Settings[1] = map(time_counter, 30, 36, 30, 60);
                        else if(time_counter < 40) Settings[1] = map(time_counter, 36, 40, 60, 120);
                        else if(time_counter < 48) Settings[1] = map(time_counter, 40, 48, 120, 600);
                        else if(time_counter < 52) Settings[1] = map(time_counter, 48, 52, 600, 3000);
                        else if(time_counter > 52) Settings[1] = -2;
                        else Settings[1] = 0; 
                        /* Legacy Potmeter code
			int TimepotVal = analogRead(TimepotPin);
                        if(TimepotVal < 120) Settings[1] = map(TimepotVal, 0, 120, 0, 60);
                        else if(TimepotVal < 360) Settings[1] = map(TimepotVal, 120, 300, 60, 300);
                        else if(TimepotVal < 720) Settings[1] = map(TimepotVal, 300, 720, 300, 1800);
                        else if(TimepotVal < 1000) Settings[1] = map(TimepotVal, 720, 1023, 1800, 3600);
                        else if(TimepotVal > 999) Settings[1] = -2;
                        else Settings[1] = 0;*/

                        lcd.setCursor(0,0);
                        lcd.print(F("Time"));
			lcd.setCursor(6,0);
                        lcd.print(time(Settings[1]));

  
			// RPM
                        Settings[0] = map((int) rpm_counter, 0, 100, 0, 100);
                        /* Legacy pot meter code
			Settings[0] = map(analogRead(RPMpotPin), 0, 1000, 0, 100);
                        */
                        lcd.setCursor(0,1);
                        lcd.print(F("Speed"));
			lcd.setCursor(6,1);
                        if(Settings[0] < 10) lcd.print(F("  "));
                        if(Settings[0] < 100) lcd.print(F(" "));
			lcd.print(Settings[0]);
                        lcd.print(F("%"));

			// If user presses Start button
			if(Start)
			{
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

                        // update LCD
                        printInfo("Speeding up","");

			// Send pulses to ESC
                        for(int i=30;i<Settings[0];i+=10) {
                                esc.write(i);
                                
                                // update LCD
                                //measureSpeed(dt);	
			        //printStatus(dt);

                                // wait a little
                                delay(500); 
                                
                                // check if short button is released
                                if(digitalRead(4) == HIGH) 
                                {
		                        Short = false;
                                } 
                                // check if we need to stop spinning
                                if(!Short && Settings[1] == -1) 
                                {
                                        breakopp = true;
                                }
                                // check lid
                                if(digitalRead(5) == LOW) 
                                {
		                        Locked = false;
                                        breakopp = true;
	                        }
                                if(digitalRead(6) == HIGH) 
                                {
                                        Stop = true;
                                        breakopp = true;
                                }
                                
                                // check if we need to stop
                                if(breakopp) 
                                {
                                        Settings[0] = 0;
                                        esc.write(0);
                                        breakopp = false;
                                        break; // break for loop
                                }
                        }
                        // continue full speed if no breaks appeared
                        if(!breakopp) {
                                esc.write(Settings[0]);
                        }

			// When RPM reaches target
			stateChange("StateSpinSteady");
		}

		if(state == "StateSpinSteady") {
			// Maintain constant speed
	                measureSpeed(dt);
			printStatus(dt);
	
			// Send pulse to ESC
                        // esc.write(Settings[0]);

			// Time in current state
			StateDt = millis() - PhaseStartTime;

			// Check for end of Short or Eternal spin
			if(Settings[1] < 0) 
			{
  	                        if(digitalRead(4) == HIGH) 
                                {
		                        Short = false;
                                }  
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

                        // update LCD
                        printInfo("Slowing down","");

			// Send pulses to ESC
                        for(int i=Settings[0];i>21;i=i-10) {
                                esc.write(i);
                                
                                // update LCD
                                //measureSpeed(dt);	
			        //printStatus(dt);

                                // wait a little
                                delay(300);
                                
                                // check lid
                                if(digitalRead(5) == LOW) 
                                {
		                       Locked = false;
                                       esc.write(0);
                                       break;
                                }
                        }
                        
			// Send pulse to ESC
                        esc.write(0);

			// Continue when rotor stops
			stateChange("StateUnlock");
		}
	
		if(state == "StateUnlock") {
			// Unlock the system
			
			// Print finish
			printInfo("Unlocking machine","");
			delay(1000);
			
			// Return to programming mode
			stateChange("StateProgramming");
		}

		if(state == "StatePanic") {
			// Stop rotor
			esc.write(0);
                        Settings[0] = 0;

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
          	//lcd.print(state);
                lcd.print(F("Spinning @ "));
                lcd.print(Gforce);
                lcd.print(F("g"));
          	lcd.setCursor(0,1);
          	//lcd.print(F(""));
          	lcd.print(time(Settings[1] - StateDt/1000));
          	lcd.print(F(" "));
          	lcd.print(CurrentRPM);
        }
}

static void printInfo(char* line1, char* line2) 
{
        // Print to LCD
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print(line1);
	lcd.setCursor(0,1);
	lcd.print(line2);
}

static void stateChange(const char* newstate) 
{
	state = newstate;
        Serial.println(newstate);
        PhaseStartTime = millis();

	// Button updates
	if(digitalRead(4) == HIGH) {
		Short = false;
	}
	if(digitalRead(5) == LOW) {
		Locked = false;
	}
        if(digitalRead(6) == HIGH) {
                Stop = true;
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

/*
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
} */

String time(int val){  
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
             
             //Serial.println(minutes);
            
             // digital clock display of current time 
             returnval = printDigits(minutes) + ":" + printDigits(seconds) + "   ";
             //returnval = String(hours) + ":" + String(minutes) + ":" + String(seconds); 
             
             return returnval;
         }
    }

String printDigits(int digits){
          // utility function for digital clock display: prints colon and leading 0
          String returnval = "";
          if(digits < 10)
                    returnval += "0";
          returnval += digits; 
         
          return returnval; 
}
