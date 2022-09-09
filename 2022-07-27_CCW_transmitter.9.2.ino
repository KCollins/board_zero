/*
  CCW_transmitter
  Uses Teensy 4.0 and Adafruit Ultimate GPS Module.
  Using tabs. This is the base sketch.

  See a_history for version notes.

*/

// Global declarations
// Written for Teensy 4.0, pin assignments from N8OBJ Board Zero.
#include <Arduino.h>
String MorseCharacter(char); // takes character, returns string of '.' and '-'
String elements(String);     // adds keying to string of '0' and '1' key up/dn


// statics to mark that program is inside interrupts
volatile bool inside_GPS_interrupt = false;
volatile bool inside_key_transmitter=false;

// pin assignments:
const int GPS_PPS_PIN   = 2;  // input from GPS, physical (phys. pin 2)
const int XMIT_PIN      = 4;  // output to key line (phys. pin 4)
const int onboardLED    = 13; // usual onboard LED (phys. pin 13)

// Parameters for interrupt timing:
const int trigger_period= 5;  // seconds between clock resets; less than one minute (60 sec) is probably fine
const int timer_rate=100000;   // microseconds for timer. 100000 give 12 WPM Morse.

// Globals for keying state:
volatile bool LED_state = false; // might not be needed
volatile bool XMIT_state= false; // start with transmitter off

// Object for timer interrupt, which will run the keying and receiving sequences:
IntervalTimer elementTimer;   // oscillator of software PLL

/////////////////////////////////////////////////////////////////////////////////////////////////////
// BIG FUNCTIONS: Get these into a class or at least separate file when they're working correctly.
/////////////////////////////////////////////////////////////////////////////////////////////////////
   String MorseCharacter(char morseLetter) { // method takes in character, generates Morse string.
      // notes: TODO finish FCC prosign list
      //        Uses dot-and-dash string for readability; many using this won't know Morse.
      //        Between-word space may have to be corrected.  Will likely use trailing space element
      //        after characters, so need 6 instead of 7 space elements for interword timing.
    morseLetter=toupper(morseLetter); // no Morse for lower case letters
    switch (morseLetter)  {
    case 'A':
      return(".-");
      break;
    case 'B':
      return("-...");
      break;
    case 'C':
      return("-.-.");
      break;
    case 'D':
      return("-..");
      break;
    case 'E':
      return(".");
      break;  
    case 'F':
      return("..-.");
      break;
    case 'G':
      return("--.");
      break; 
    case 'H':
      return("....");
      break;
    case 'I':
      return("..");
      break;
    case 'J':
      return(".---");
      break;
    case 'K':
      return("-.-");
      break;
    case 'L':
      return(".-..");
      break;
    case 'M':
      return("--");
      break;
    case 'N':
      return("-.");
      break;
    case 'O':
      return("---");
      break;
    case 'P':
      return(".--.");
      break;
    case 'Q':
      return("--.-");
      break;
    case 'R':
      return(".-.");
      break;
    case 'S':
      return("...");
      break;
    case 'T':
      return("-");
      break;
    case 'U':
      return("..-");
      break;  
    case 'V':
      return("...-");
      break;
    case 'W':
      return(".--");
      break;
    case 'X':
      return("-..-");
      break;
    case 'Y':
      return("-.--");
      break;      
    case 'Z':
      return("--..");
      break;

    case '1':
      return(".----");
      break;
    case '2':
      return("..---");
      break;
    case '3':
      return("...--");
      break;
    case '4':
      return("....-");
      break;
    case '5':
      return(".....");
      break;
    case '6':
      return("-....");
      break;
    case '7':
      return("--...");
      break;
    case '8':
      return("---..");
      break;      
    case '9':
      return("----.");
      break;
    case '0':
      return("-----");
      break;

    case '.':
      return(".-.-.-");
      break;
    case ',':
      return("--..--");
      break;
    case '?':
      return("..--..");
      break;
    case '/':
      return("-..-.");
      break;
    case '@':
      return(".--.-.");
      break;
    case ' ': // Space.  Accounted for in elements switch/case: Special case of no dots or dashes
      return(" ");
      break;
    default:  // Any other character, return null string.
      return("");
      break;
   } // switch
  }; // MorseCharacter(char morseLetter)


  String elements(String morseString) {
    if (morseString.length()==0) return ""; // error trap: null input gets null ouput.
    
    String keying_string=""; // morseString has content if got here
    char char0=morseString.charAt(0); // store zeroth character temporarily
    morseString.remove(0,1);     // and pop it

    switch (char0)  {
      case ' ': // space.  Character before has three keyups for intercharacter space, and two will be added later.
        keying_string.concat("00"); // add two keyups here so that result will be seven keyups.
      case '.': // dit
        keying_string.concat("10");
        break;
      case '-': // dah
        keying_string.concat("1110");
        break;
      default: // error here! Fill in an error trap.
        break;
    } // switch
    keying_string.concat("00"); // finish intercharacter space.        
    return keying_string; // number of timing intervals left, including three trailing key up intervals
  }; // elements
// END OF BIG FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////


// some constant strings for testing.  Might be useful to make these settable
// in the GUI for beacons.
const String HELLO="10101010001000101110101000101110101000111011101110000000"; //hello 
const String DITS="10"; // one dit followed by one interelement spacing
const String TEST="W8EDU"; // needs to be made into a keying String
String W8EDU="101110111000";
String W=MorseCharacter('W');
String W10=elements(W);
String keyer_buf=W10;  // for typed characters to be Morsed out


/*////////////////
setup and loop
////////////////*/
void setup() {
  Serial.begin(9600); // dummy speed
  Serial.println(W);
  Serial.print("Morse string: ");
  Serial.println(W10);
  pinMode(onboardLED, OUTPUT);
  pinMode(XMIT_PIN,   OUTPUT);
  digitalWrite(XMIT_PIN, LOW); // Make sure transmitter is off at start
  // Declare the interrupt service routine for the GPS 1 PPS:
  attachInterrupt(digitalPinToInterrupt(GPS_PPS_PIN), GPS_interrupt, RISING);
  // Clear timer interrupt:
  elementTimer.end();
  //elementTimer.priority(1); // not sure where to put this to minimize glitch
}

// Accept characters from host computer
void loop() {
  char incomingByte;
  if (Serial.available()) {
    incomingByte = Serial.read();  // will not be -1
    // actually do something with incomingByte
    // eventually will add this to end of keyer_buf
    Serial.println(incomingByte);
  }
}
