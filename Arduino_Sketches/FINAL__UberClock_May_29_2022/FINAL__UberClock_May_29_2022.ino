// #############################################################################
// # Uber Clock                                                                #
// # A MIDI Precision Clock using:                                             #
// # - DFROBOT FireBeetle Board ESP32-E                       (P/N: DRF0654-F) # 
// # - DFROBOT Capacitive Touch Screen 3.5' 480x320 TFT LCD   (P/N: DFR0669)   #
// # - DFROBOT EC11 Rotary Encoder Module                     (P/N: SEN0235)   #
// # Erik Oostveen www.erikoostveen.co.uk                                      #
// # May 2022                                                                  #
// #############################################################################

#include "ESP32TimerInterrupt.h"
#include "MIDI.h"
#include "Button.h"
#include "AiEsp32RotaryEncoder.h"
#include "AiEsp32RotaryEncoderNumberSelector.h"
#include "DFRobot_UI.h"
#include "Arduino.h"
#include "DFRobot_GDL.h"
#include "DFRobot_Touch.h"


  String word_1[150] = { "resonantly","wildly","quintessentially","inadvertently","nervously","coaxingly","dutifully","regretfully","bashfully","ironically","restlessly","terribly","hourly","soundlessly","mystically","affectionately","frequently","briskly","fervently","positively","successfully","frenetically","openly","weakly","wearily","seriously","solidly","gradually","mysteriously","constrictingly","heartily","passionately","darkly","casually","perpetually","silently","sympathetically","inwardly","dangerously","continuously","disgustingly","meekly","reluctantly","triumphantly","badly","mordently","unwillingly","lovingly","deceivingly","accusingly","fortunately","rapturously","beautifully","hungrily","cleverly","increasingly","willfully","arrogantly","nightly","roughly","perfectly","foolishly","knowingly","sadly","pleasingly","playfully","awkwardly","suspiciously","unhappily","violently","restfully","conspicuously","solemnly","resoundingly","selfishly","deeply","generally","breathlessly","messily","fleetingly","superstitiously","tenderly","photographically","concurrently","happily","smoothly","spiritually","gracefully","telekinetically","sedately","faintly","lustfully","astonishingly","gently","speedily","slyly","gleefully","futuristically","unexpectedly","quaveringly","accidentally","reassuringly","shakily","expeditiously","later","rudely","sleepily","strictly","viciously","genetically","writhingly","carefully","searchingly","crisply","softly","anciently","intensely","enviously","sternly","quietly","spitefully","shrilly","tightly","ravenously","mockingly","mournfully","innocently","jokingly","ear-splittingly","impulsively","anxiously","claustrophobically","amazingly","grumpily","thunderously","loudly","thoughtfully","diligently","sheepishly","patiently","understandably","honestly","mathematically","politely","purposefully","fully","tremendously","never","determinedly","enticingly" };
  String word_2[150] = { "cold","divine","planetary","smelly","mesmeric","digital","coastal","accurate","fair","nice","shallow","nautical","icy","spectral","astral","interstellar","oceanic","wet","blue","luminescent","aquatic","handy","honest","rare","fluvial","heady","big","cabalistic","incognito","polar","ugly","chubby","breathtaking","tall","sweaty","dull","abyssal","strange","sparkly","bold","occult","sore","bizarre","scary","faint","bewitching","transcendental","manifesting","paranormal","astonishing","seductive","ghostly","alien","meteoric","celestial","open","intriguing","dense","delicate","shivering","otherworldly","true","esoteric","angry","dizzying","sweet","easy","clumsy","bland","busy","fundamental","flippant","bad","nonsensical","close","wonderous","cryptic","fancy","beautiful","unprecedented ","idyllic","astronomical","thermal","hairy","astounding","classy","cloudy","happy","intergalactic","sincere","tough","inscrutable","stellar","ululating","bitter","pure","dark","incomparable","weeping","mindful","spellbinding","warm","noisy","dreamy","secret","weak","crafty","allegorical","hydrating","smart","deadly","legitimate","solar","natural","clever","galactic","metaphysical","granular","black","vast","quivering","odd","basic","unfathomable","throbbing","heavenly","supernatural","martian","delightful","little","deep","tiny","wild","hip","nonstandard","ineffable","infinite","pleasurable","unrefined","barbaric","quiet","obscure","fake","electric","necromantic","small","external","chewy","logical","bloody" };
  String word_3[150] = { "being","gem","nebula","supernova","computer","random","retrogame","zookeeper","sandbox","speedrun","vaporware","beginning","flaps","fractal","solar wind","radio","moon","pilot  ","curmudgeon","crystal","phenomenon","cleric","dream","stone","earthwork","ocean","peril","kaleidoscope","time travel","marathon","blossom","reptile","dolmen","universe","sponge","asteroid belt","science","binary star","existence","dreamcatcher","shard","sun","plain","forest","whale","quasar","application","orbit","big bang","starfish","fairy","corona","milky way","consequence","mind","cave","conscience","pisces","algorithm","cradle","elixir","gyroscope","action","planet","cactus","church","star","power","passage","photon","rem sleep","caldera","path","castle","stardust","comet","destiny","courage","grassland","plasma","cherry","dimension","station","omen","yurt","flowers","broadband","champagne","galaxy","gravity","dark matter","bulkhead","piggyback","essence","osiris","individuality","thor","voicemail","allure","ghostwriter","alchemy","water","hypnosis","avatar","cosmos","satellite","attention","megalith","celebration","echo","energy","black hole","density","gold","magnet","entropy","chiptune","atom","proton","boson","gelatin","astronaut","data","dungeon","idea","arras","ghost","grave","orb","sphere","cube","surrealism","pyramid","clone","existentialism","individualism","mode","humanity","species","deism","egalitarianism","illusion","materialism","plural","romanticism","poem","synergism","syzygy","transcendentalism","egg" };
  
  int midiClockCounter = 0;  // Used for clock division purposes.
  int ClearFlag = 0;         // Used to catch 1st MIDI count (downbeat)
  int ReStartFlag = 0;       // Used to indicate that RE/START button has been tapped.
  int StopFlag = 1;          // Used to indicate that STOP button has been tapped.
  int qbb = 0;               // Used to set colour of the quarter beats Box (qbb).
    
  // Timer --------------------------- 
  unsigned long previousMillis = 0;
  const long interval = 1005;
  int millisec;
  int tseconds;
  int dhours;
  int dminutes;
  int dseconds;
  unsigned long timeOffset = 0;
  // Timer ---------------------------
  
  // Additional colours ---------------------------------------
  #define COLOR_RGB565_DARKGREY     0x0861   //  Dark Grey 
  #define COLOR_RGB565_DARKGREEN    0x0260   //  Dark Green
  #define COLOR_RGB565_BRIGHTBLUE   0x001f   //  Bright Blue
  // Additional colours ---------------------------------------

  // Screen ---------------------------------------
  #define TFT_DC  D2
  #define TFT_CS  D6
  #define TFT_RST D3
  // Screen ---------------------------------------

  /** Touch Screen **/
  DFRobot_ILI9488_320x480_HW_SPI screen(TFT_DC,TFT_CS,TFT_RST);
  DFRobot_Touch_GT911 touch;
  DFRobot_UI ui(&screen, &touch);
    
  // Rotary Encoder ---------------------------------------
  #define ROTARY_ENCODER_A_PIN 16
  #define ROTARY_ENCODER_B_PIN 17
  #define ROTARY_ENCODER_BUTTON_PIN -1 // Disable encoder's switch pin
  #define ROTARY_ENCODER_STEPS 4
  AiEsp32RotaryEncoder *rotaryEncoder = new AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, -1, ROTARY_ENCODER_STEPS);
  AiEsp32RotaryEncoderNumberSelector numberSelector = AiEsp32RotaryEncoderNumberSelector();
  
  void IRAM_ATTR readEncoderISR()
  {
      rotaryEncoder->readEncoder_ISR();
  }
  int displayBPMvalue;
  int bpm = 120;
  Button encoderSwitch(4);
  // Rotary Encoder ---------------------------------------

  // Create MIDI Instance
  MIDI_CREATE_DEFAULT_INSTANCE();
  
  //Callback function for two buttons
  void btnCallback(DFRobot_UI::sButton_t &btn,DFRobot_UI::sTextBox_t &obj) {
     
     String text((char *)btn.text);
     
     if(text == "Re/Start") { // Re/Start MIDI Gear       
       ReStartFlag = 1;
       StopFlag = 0;
     } else if(text == "Stop") { // Stop MIDI Gear       
       StopFlag = 1;
       qbb = 0;
       MIDI.sendStop(); // Send MIDI Stop Byte
       clockStatus(2); // Change Clock Status to "Stopped"
     }
     
  }

  void screenBPM() { // Dynamic BPM Idicator
    screen.setFont(&FreeSans12pt7b); 
    screen.fillRect(5, 192, 125, 75, COLOR_RGB565_BLACK);        
    screen.setTextColor(COLOR_RGB565_WHITE);
    screen.setTextSize(3);
    screen.setCursor(10,255);
    if ( bpm < 100 ) { 
       screen.print(String("0") + bpm);
      } else {
       screen.print(bpm);     
      }                              
    displayBPMvalue = 0;    
  }

    void staticGraphs() { // Static graphs placement 
    // UberClock Box
    screen.fillRoundRect(176, 190, 134, 80, 25, COLOR_RGB565_YELLOW);
    screen.fillCircle(284,211,11,COLOR_RGB565_BLACK);

    // UberClock
    screen.setFont(&FreeSansBold12pt7b);
    screen.setTextColor(COLOR_RGB565_BLACK);
    screen.setTextSize(1);
    screen.setCursor(202,225);
    screen.print("Uber"); 
    screen.setCursor(211,248);
    screen.print("Clock"); 
    
    // Horizontal Ruler I
    screen.fillRoundRect(10, 280, 300, 2, 0, COLOR_RGB565_ORANGE);
        
    // Draw box around beat Box & lines inside
    screen.drawRoundRect(14, 288, 292, 120, 2, COLOR_RGB565_WHITE);
    screen.drawFastVLine(133, 293, 110, COLOR_RGB565_WHITE);   
    screen.drawFastHLine(138, 318, 162, COLOR_RGB565_WHITE);   
    
    // Horizontal Ruler II
    screen.fillRoundRect(10, 415, 300, 2, 0, COLOR_RGB565_ORANGE);
    // Bottom Timer Bar
    screen.fillRoundRect(10, 425, 300, 50, 4, COLOR_RGB565_BLUE);      
    // Line to Elapsed Time
    screen.drawFastVLine(26, 437, 12, COLOR_RGB565_WHITE);
    screen.drawFastHLine(26, 437, 10, COLOR_RGB565_WHITE);
    // Line to Status
    screen.drawFastVLine(181, 437, 12, COLOR_RGB565_WHITE);
    screen.drawFastHLine(181, 437, 10, COLOR_RGB565_WHITE);       
    }

    void staticText() { // Static text placement
    // BPM
    screen.setFont(&FreeSans9pt7b);
    screen.setTextColor(COLOR_RGB565_WHITE);
    screen.setTextSize(1);
    screen.setCursor(130,255);
    screen.print("BPM");

    screen.setCursor(145,308);
    screen.print("Word Jam");
    
    // Elapsed Time
    screen.setCursor(40,442);
    screen.print("Elapsed Time");

    // Clock Status
    screen.setCursor(195,442);
    screen.print("Clock Status");
        
  }


// Init ESP32 timer 0
  ESP32Timer ITimer0(0);

  // Tx MIDI Clock Bytes 
  bool IRAM_ATTR TimerHandler0(void * timerNo)
  { 

    if ( midiClockCounter == 1 ) {
      ClearFlag = 1; 
    }
    
    midiClockCounter++;
     if ( midiClockCounter > 96 )       
      {        
        midiClockCounter = 1;        
      }
    MIDI.sendClock();
    return true;
  }

  // Set timer period
  void setTempo(int bpm) {
  volatile int TIMER0_INTERVAL_US = 2500000 / bpm;  // Original formula: 60000000 / ( bpm * 24 ) 
  ITimer0.attachInterruptInterval(TIMER0_INTERVAL_US, TimerHandler0);
  }

void setup()
{

  // Initial MIDI
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();

  // Set Initial Tempo at 120 Beats Per Minute (BPM)
  setTempo(120);

  // Rotary Encoder ---------------------------------------
  rotaryEncoder->begin();
  rotaryEncoder->setup(readEncoderISR);
  numberSelector.attachEncoder(rotaryEncoder);
  numberSelector.setRange(30, 300, 1, false, 0); // Min Val., Max Val., Step, Cycles step (min-max), # Decimals after the comma
  numberSelector.setValue(120);
  encoderSwitch.begin();
  // Rotary Encoder ---------------------------------------

 //Initialize UI 
  ui.begin();
  //Set the UI theme, there are two themes to choose from: CLASSIC and MODERN.
  ui.setTheme(DFRobot_UI::MODERN);
  //Set screen orientation (0 = No rotation, horizontal, 1 = Rotate 90 degrees clockwise, 2 = Rotate 180 degrees clockwise, 3 = Rotate 270 degrees clockwise) 
  screen.setRotation(0);

  //Create buttons
  // Red Start Button
  DFRobot_UI::sButton_t & btn1 = ui.creatButton();
  btn1.setText((char *)"Re/Start"); 
  btn1.fontSize = 2;
  btn1.bgColor = COLOR_RGB565_DARKGREEN;
  btn1.setCallback(btnCallback);
  ui.draw(&btn1,10,10,300,80); /* x-coord, y-coord, width, height */

  // Blue Stop Button
  DFRobot_UI::sButton_t & btn2 = ui.creatButton();
  btn2.setText((char *)"Stop");
  btn2.fontSize = 2;
  btn2.bgColor = COLOR_RGB565_MAROON;
  btn2.setCallback(btnCallback);
  ui.draw(&btn2,10,100,300,80); /* x-coord, y-coord, width, height */

  MIDI.sendStop(); // Send MIDI Stop Byte
  clockStatus(2); // Change Clock Status to "Stopped"

  staticGraphs();
  screenBPM();
  staticText();
  wordJam();
  elapsedTime();
  clockStatus(0); 
 
}

void loop()
{

        // Blinking rounded beat square ------------------------------------------------------
        if ( ClearFlag == 1 && bpm > 199 ) {
          screen.fillRoundRect(48, 322, 50, 50, 15, COLOR_RGB565_WHITE); // At higher tempos blnking is hard            
        }
        else {
             if ( ClearFlag == 1 ) {    
              screen.fillRoundRect(48, 322, 50, 50, 15, COLOR_RGB565_BRIGHTBLUE); // 1/4 Beat     
              ClearFlag = 0;   
            }
          
                if ( midiClockCounter == 12  ) {  
                  DrawDarkBox(); // Clear 
                }
    
            if ( midiClockCounter == 25 ) {  
              DrawBeatBox(); // 2/4 Beat 
            }
          
                if ( midiClockCounter == 36  ) {  
                  DrawDarkBox(); // Clear 
                }
          
            if ( midiClockCounter == 49 ) { 
              DrawBeatBox(); // 3/4 Beat 
            }
          
                if ( midiClockCounter == 60  ) {  
                  DrawDarkBox(); // Clear 
                }
          
            if ( midiClockCounter == 73 ) {
              DrawBeatBox(); // 4/4 Beat 
            }
          
                if ( midiClockCounter == 84  ) {  
                  DrawDarkBox(); // Clear 
                }          
        }       
        // Blinking rounded beat square ------------------------------------------------------


  if ( ReStartFlag == 1 ) {   

      if ( ReStartFlag == 1 && StopFlag == 0 ) {
        qbb = 2;
      }


      if ( midiClockCounter == 1  && StopFlag == 1 ) {                   
            ReStartFlag = 0;
            StopFlag = 0;    
      } else if ( midiClockCounter == 1  && StopFlag == 0 ) {               
            MIDI.sendStart(); // Send MIDI Start Byte
            clockStatus(1); // Change Clock Status to "(Re)Started"
            delay(1);
            MIDI.sendNoteOn(62, 1, 16);
            delay(1);
            MIDI.sendNoteOn(62, 0, 16);                
            ReStartFlag = 0;
            qbb = 1;                    
      }   
        
  }



  if (rotaryEncoder->encoderChanged())
  {
    bpm = numberSelector.getValue();
    setTempo(bpm);
    screenBPM(); 
  }


  ui.refresh();

  if (encoderSwitch.released() ) {
    timeOffset = millis();

    // Print Random Words (Word Jam)
    wordJam();
        
  }
 

  unsigned long currentMillis = millis() - timeOffset;

  if ( currentMillis - previousMillis >= interval ) {
   
    previousMillis = currentMillis;
      
      tseconds = currentMillis / 1000;
      dseconds = tseconds % 60;
      dminutes = (tseconds / 60) % 60;
      dhours = tseconds / 3600;

      if ( ( dseconds % 5) == 0 ) { // Update elapsed time every 5 seconds
        elapsedTime();
      }
    
  }   
 
}

// Draw Beat Boxes in Various Colours depending on clock state
void DrawBeatBox() {

  switch (qbb) {
    case 0:
    screen.fillRoundRect(48, 322, 50, 50, 15, COLOR_RGB565_MAROON);     // Clock State: Stopped      
  break;
    case 1:
    screen.fillRoundRect(48, 322, 50, 50, 15, COLOR_RGB565_DARKGREEN);  // Clock State: Running    
  break;
    case 2:
    screen.fillRoundRect(48, 322, 50, 50, 15, COLOR_RGB565_PURPLE);     // Clock State: Waiting for Down Beat (then running)    
  break;
  }

}

// Draw Dark Box
void DrawDarkBox() {

  screen.fillRoundRect(48, 322, 50, 50, 15, COLOR_RGB565_DARKGREY);
 
}

// Word Jam
void wordJam() {

    screen.setTextColor(COLOR_RGB565_WHITE);
    screen.setFont(&FreeSans9pt7b);
    screen.setTextSize(1);
    screen.fillRect(135, 325, 170, 76, COLOR_RGB565_BLACK);
    screen.setCursor(147,342);
    screen.print( word_1[random(150)] );
    screen.setCursor(147,367);
    screen.print( word_2[random(150)] );
    screen.setCursor(147,392);
    screen.print( word_3[random(150)] );
  
}

// Display elapsed time
void elapsedTime() {

    screen.fillRect(15, 447, 100, 25, COLOR_RGB565_BLUE); 
    screen.setTextColor(COLOR_RGB565_WHITE);        
    screen.setFont(&FreeSans12pt7b); 
    screen.setTextSize(1);   
    screen.setCursor(20,467);      
    screen.print(dhours + String(":") + dminutes + String(":") + dseconds);
  
}

// Display status
void clockStatus(int clkStatus) {

    screen.fillRect(170, 447, 130, 26, COLOR_RGB565_BLUE); 
    screen.setTextColor(COLOR_RGB565_WHITE);        
    screen.setFont(&FreeSans12pt7b); 
    screen.setTextSize(1);   
    screen.setCursor(175,467);

    if ( clkStatus == 0 ) {
      screen.print("Power ON");
    }
    
    if ( clkStatus == 1 ) {
      screen.print("(Re)Started");
    }

    if ( clkStatus == 2 ) {
      screen.print("Stopped");
    }
    
}
