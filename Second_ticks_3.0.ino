/*
 * Version 2.0
 * 22 January 2022
 * Rewrite after discussion with project group
 * Use the interrupt service routine to
 *  start data collection,
 *  end data collection,
 *  send serial data to host.
 *  loop() is empty.
 *  --> Is there a way to avoid the while loop in the PPS interrupt routine?
 *  
 *  23 January 2022
 *  Version 3.0
 *  Moved the data activation to the loop() function.
 *  Still have two "competing" ISRs, but time between samples
 *  is available in the loop() if wanted for something.
 */

/*
 * In this version, blink the light after the top of second
 * for ten blinks.
 * 24 October 2021 1821 EDT: Works.
 * I don't think that any interrupts need to be enabled/disabled.
 * The A/D sampling version will be similar:
 *  Sample 0 will be immediately after firing the PPS interrupt,
 *  samples will then continue to 799 and stop.
 * loop() can watch for the end of the sample time and write a file, 
 * do filtering, etc.
 * May need to set timer counter to zero to avoid jitter of up to
 * 125 microseconds (one sample time).
 * 26 October 2021 Samples 0 to 3.3 V on input A0, displays on
 * serial monitor.
 * 
 * Starting v.1.2
 * Plan: Sample at 125 microseconds (8000 s/s)
 * Store an 800 vector of samples.
 * Stop.
 * 27 October 2021:
 * That works, if not very elegantly.
 * Now try connecting to transceiver.
 * Use new version, 1.3
 * 27 October 2021 12:36 EDT:
 * Read data off headphone jack of TS-450
 * 
 * 17 November 2021:
 *   Version 1.4 receive on data tick from headphone jack
 *   Displayed to Arduino serial monitor.
 *   Having communications trouble,
 * 6 December 2021 13:15 EST added delay(1) after serial send. Might need more.
 *   TODO: Delay data collection to no-skip delay time from WWV to station, or a little less,
 *         to save storage and improve timing.
 * 09 January 2022
 *   Cleaned up extraneous comments
 */
 
// --------------------------------------------------------------------------------------
// v. 3.0 23 January 2022
// Set up globals
// NB: Adjust the sample rate and sample size together:
// need about 13 milliseconds minimum sampling for Cleveland
// Fort Collins to Cleveland great-circle delay is about 7.5 ms (~2000 km)

// define pins: GPS_PPS input, analog input.
const int GPS_PPS_PIN=2; // attachment pin for GPS PPS
const int ANALOG_IN=0; // Input A1 is Teensy 4.1 pin 15.
// Create IntervalTimer object, and set its sampling rate
IntervalTimer ADtimer; // interrupt timer for A/D conversions
const int AD_Period = 50; // 50 microseconds (20,000 samples/second)
// set up data array for samples
#define sampleSize 512 // Adjust if sampling rate is changed.
volatile unsigned int data[sampleSize];
// Global boolean to mark top of second from GPS PPS interrupt:
volatile bool TOS=false;
// Need a global volatile index for the data array:
volatile unsigned int sampleCount=0; // ISR will manipulate this


void setup() {
  // set up the GPS digital input, A/D analog input, LED digital output, and the serial port.
  // define the GPS input pin and GPS function for the interrupt
  attachInterrupt(digitalPinToInterrupt(GPS_PPS_PIN), GPS_interrupt, RISING);
  pinMode(LED_BUILTIN, OUTPUT); // for blinking the onboard LED
  Serial.begin(57600); // rate here doesn't matter--Teensy sets fastest the USB port will accept
}

void GPS_interrupt()  { // communicates top of second to the rest of the code.
  TOS=true;
} // GPS_interrupt

void getAD() { // the timer's ISR.  Checked timing, it works at this speed
  int analogVal; // A/D converter output
  analogVal = analogRead(ANALOG_IN); // input A0, pin 14
  // zero volts in returns 511
  // range -1.5 to + 1.5 is inside 0 to 1023 A/D range
  // sampleCount is global, initialized at top of second in GPS-interrupt()
  data[sampleCount] = analogVal; // store data. Yes, this is an extra step...
  sampleCount++; // incrementing after A/D, so sampleCount==sampleSize means last one was just taken.
} // getAD() 

void loop() {
  while (!TOS)  {} // idle until GPS top of second
  
  // Fell through while(), so TOS now true. Get to work.
  // Start collecting data
  // (NB: sampleCount is already initialized to zero; it's incremented in ISR getAD())
  ADtimer.begin(getAD, AD_Period); // getAD is the interrupt service routine for the timer/ AD_Period is time between samples.
  digitalWrite(LED_BUILTIN, HIGH); // light on-board LED to indicate PPS tasks being done
  while (sampleCount<sampleSize)  {} // idle while samples being collected. Could do stuff here.
  ADtimer.end(); // Samples collected: stop collecting data
  
  // data array collected, send it via serial port to host
  // Better way to do this? Is there some library for sending whole arrays?
  for (unsigned int i=0; i<sampleCount; i++) {
    Serial.print(data[i]); // send data without EOL
    if(i<sampleCount-1) {Serial.print(",");} // omit trailing comma.
    } // for.  Now finish the line with EOL characters:
  Serial.println(); // send EOL characters to finish CSV line
  digitalWrite(LED_BUILTIN, LOW); // extinguish on-board LED: this second's data line is written.
  sampleCount=0; // get ready for next second
  TOS=false;     // "" ""
  
} // loop()
