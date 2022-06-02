// #############################################################################
// # Uber Box                                                                  #
// # Driven by the MIDI Precision Clock called Uber Clock. Using:              #
// # - DFROBOT LED Button RED                                 (P/N: DFR0785-R) # 
// # - DFROBOT LED Button GREEN                               (P/N: DFR0785-G) #
// # - Arduino Nano board v3                                                   #
// # Erik Oostveen www.erikoostveen.co.uk                                      #
// # March 2022                                                                #
// #############################################################################

#include "MIDI.h"
#include "Button.h"

// Create MIDI Instance
MIDI_CREATE_DEFAULT_INSTANCE();

// Beat LEDs
int Beat_1_PIN = 9;  // 1/4 Downbeat LED
int Beat_2_PIN = 10; // 2/4 LED
int Beat_3_PIN = 11; // 3/4 LED
int Beat_4_PIN = 12; // 4/4 LED

// Stop LED
int Stop_PIN = 5;             // Stop LED

// Stop Button
Button Stop_Button_PIN(7);    // Stop Button

// ReStart LED
int ReStart_PIN = 6;          // ReStart LED 

// ReStart Button
Button ReStart_Button_PIN(8); // ReStart Button

int midiClockCounter = 0;     // Used for clock division purposes.

//int reStartStopFlag = 0;
//int stopFlag = 0;

// Button Status
int StopButton = 1;
int ReStartButton = 0;

// Running Status
int runningStatus = 0;

// Alternate LED (On/Off)
int alternateRed = 1;
int alternateGreen = 1;

void setup() {

  // Init MIDI
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOn();

  // Init Beat LEDSs
  pinMode(Beat_1_PIN, OUTPUT);  // 1/4 Downbeat Blue
  digitalWrite(Beat_1_PIN, LOW);  
  pinMode(Beat_2_PIN, OUTPUT); // 2/4 LED
  digitalWrite(Beat_2_PIN, LOW); 
  pinMode(Beat_3_PIN, OUTPUT); // 3/4 LED
  digitalWrite(Beat_3_PIN, LOW); 
  pinMode(Beat_4_PIN, OUTPUT); // 4/4 LED
  digitalWrite(Beat_4_PIN, LOW); 

  // Init Stop LED
  pinMode(Stop_PIN, OUTPUT);     // Stop LED
  digitalWrite(Stop_PIN, HIGH); 

  // Init Stop Button
  Stop_Button_PIN.begin();
   
  // Init ReStart LED
  pinMode(ReStart_PIN, OUTPUT);  // ReStart LED
  digitalWrite(ReStart_PIN, HIGH); 

  // Init ReStart Button
  ReStart_Button_PIN.begin();
  
}

void loop() { 

if (ReStart_Button_PIN.released()) {  
  ReStartButton = 1; 
  StopButton = 0; 
  runningStatus = 1;
  }

if (Stop_Button_PIN.released()) { 
    MIDI.sendStop(); // Send MIDI Start Byte  
    StopButton = 1; 
    ReStartButton = 0;
    runningStatus = 0; 
  }

  if (MIDI.read()) {       
             
     switch(MIDI.getType()) // MIDI Message Type
      {
      
        case midi::Start:        
        // Start Byte Detected
         allBeatLEDsOff();
         midiClockCounter = 1; // Starting at 1 to keep boxes in sync with main clock
         ReStartButton = 0;
         runningStatus = 1;                     
        break;
        
        case midi::Stop:        
        // Start Byte Detected   
        StopButton = 1; 
        ReStartButton = 0;
        runningStatus = 0;                              
        break;
         
        case midi::Clock:        
        // Clock Byte Detected
         midiClockCounter++;

         if ( StopButton == 1 && runningStatus == 0 ) {
          analogWrite(ReStart_PIN, map(midiClockCounter,1,96,100,255)); 
          analogWrite(Stop_PIN, map(midiClockCounter,1,96,100,255));          
         } 


         switch(midiClockCounter) {
          case 1:

            if ( ReStartButton == 1 && StopButton != 1 ) {
              MIDI.sendStart(); // Send MIDI Start Byte
              delay(1);
              MIDI.sendNoteOn(62, 1, 16);
              ReStartButton = 0;  
            }
            
            digitalWrite(Beat_1_PIN, HIGH); digitalWrite(Beat_4_PIN, LOW);
            
                    if ( runningStatus == 1 ) { analogWrite(ReStart_PIN, 100); analogWrite(Stop_PIN, 100); }
                    
          break;

          // Running LED blinks -----------------------------------------------------                    
          case 3:
                  if ( runningStatus == 1 && ReStartButton == 0 ) { analogWrite(ReStart_PIN, 255); analogWrite(Stop_PIN, 255); }
          break;

          case 5:
                  if ( runningStatus == 1 && ReStartButton == 0 ) { analogWrite(ReStart_PIN, 0); analogWrite(Stop_PIN, 0); }
          break;          
          
          case 7:
                    if ( runningStatus == 1 ) { analogWrite(ReStart_PIN, 255); analogWrite(Stop_PIN, 255); }
          break;
          
          case 13:
                    if ( runningStatus == 1 && ReStartButton == 1 ) { analogWrite(ReStart_PIN, 100); analogWrite(Stop_PIN, 100); }
          break;

          case 19:
                    if ( runningStatus == 1 && ReStartButton == 1 ) { analogWrite(ReStart_PIN, 255); analogWrite(Stop_PIN, 255); }
          break;          
          // Running LED blinks -----------------------------------------------------          

          case 25:
           digitalWrite(Beat_2_PIN, HIGH); digitalWrite(Beat_1_PIN, LOW);

                    if ( runningStatus == 1 ) { analogWrite(ReStart_PIN, 100); analogWrite(Stop_PIN, 100); }
           
          break;

          // Running LED blinks -----------------------------------------------------
          case 31:
                    if ( runningStatus == 1 ) { analogWrite(ReStart_PIN, 255); analogWrite(Stop_PIN, 255); }
          break;

          case 37:
                    if ( runningStatus == 1 && ReStartButton == 1 ) { analogWrite(ReStart_PIN, 100); analogWrite(Stop_PIN, 100); }
          break;

          case 43:
                    if ( runningStatus == 1 && ReStartButton == 1 ) { analogWrite(ReStart_PIN, 255); analogWrite(Stop_PIN, 255); }
          break;           
          // Running LED blinks ----------------------------------------------------- 


          case 49:
           digitalWrite(Beat_3_PIN, HIGH); digitalWrite(Beat_2_PIN, LOW);

                    if ( runningStatus == 1 ) { analogWrite(ReStart_PIN, 100); analogWrite(Stop_PIN, 100); }
           
          break;

          // Running LED blinks -----------------------------------------------------
          case 55:
                    if ( runningStatus == 1 ) { analogWrite(ReStart_PIN, 255); analogWrite(Stop_PIN, 255); }
          break;

          case 61:
                    if ( runningStatus == 1 && ReStartButton == 1 ) { analogWrite(ReStart_PIN, 100); analogWrite(Stop_PIN, 100); }
          break;

          case 67:
                    if ( runningStatus == 1 && ReStartButton == 1 ) { analogWrite(ReStart_PIN, 255); analogWrite(Stop_PIN, 255); }
          break;           
          // Running LED blinks ----------------------------------------------------- 

          case 73:
           digitalWrite(Beat_4_PIN, HIGH); digitalWrite(Beat_3_PIN, LOW);

                    if ( runningStatus == 1 ) { analogWrite(ReStart_PIN, 100); analogWrite(Stop_PIN, 100); }
           
          break;

          // Running LED blinks -----------------------------------------------------        
         case 85:
                    if ( runningStatus == 1 && ReStartButton == 0  ) { analogWrite(ReStart_PIN, 255); analogWrite(Stop_PIN, 255); }
                    if ( runningStatus == 1 && ReStartButton == 1 ) { analogWrite(ReStart_PIN, 100); analogWrite(Stop_PIN, 100); }
          break;

          case 91:
                    if ( runningStatus == 1 && ReStartButton == 1 ) { analogWrite(ReStart_PIN, 255); analogWrite(Stop_PIN, 255); }
          break;           
          // Running LED blinks ----------------------------------------------------- 
       
          case 96:
           midiClockCounter = 0;
          break;
      
         }

      }

  }

}


void allBeatLEDsOff() {

  digitalWrite(Beat_1_PIN, LOW);
  digitalWrite(Beat_2_PIN, LOW);
  digitalWrite(Beat_3_PIN, LOW);
  digitalWrite(Beat_4_PIN, LOW);
 
}
