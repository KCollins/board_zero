/*
Second complete attempt at CCW
  PPS pin begins timer interrupts
  Timer interrupts set the Morse elements
  Started 27 July 2022
  v1: Can use one of two timer ISRs.
      Realized that timer ISR is not called until first interval passes,
      so have PPS ISR call timer ISR to start sequence.
      Software PLL may require adjustment of the timer's microseconds.
  v2: Add keying of transmit MOSFET on pin 4
      This does key the code oscillator at 10 per second but has a
      slight glitch somewhere.  Put on an oscilloscope and check!
  v4: Change how often the interval time gets reset by PPS: trigger_period
  v5: 21 August 2022 Sunday
      Goal: Create String object and clock out send commands
      Achieved: keyer_buf clocks out 1 and 0 string to keyer MOSFET
      Morse string then loops in key_transmitter()
  v6: Get character string translated for sending.
      Should be able to reuse some code from earlier versions.
  v7: 4 September 2022
      Corrected timing errors when using a constant string, repeated:
      The last statement in key_transmitter() re-calls that function
      when a zero-length string is encountered.  Otherwise an extra key-up
      element is added.
  v8: 5 September 2022
      Moved PPS ISR, key_transmitter(), and some unused functions into tabs.
      Cleaned those up, commented global variables.
      NEXT: Add class for generating Morse, adding typed characters to 
            keyer_buf String.
  v9: 5 September 2022
      Added h_morse_generator tab
      Class MorseGenerator does not take any argument.
        Its public methods are 
          void setMorseChar(char morseLetter)
          char getMorseChar()
          String getMorseString()
          String getKeyingString()
        Expected use is
          setMorseChar(char);
          getKeyingString();
          concantenate that result to keyer_buf in main sketch.

     Might really be easier just to use a function.  MorseCharacter should return the appropriate string but it's not being included properly in the compilation.
     I'm not sure why--might just need a prototype.
   v.9.1: 5 September 2022
     Put the functions morseString and elements back into main sketch.
     elements only picks off the first element of the string resulting from
     a character being processed in morseString.  Fix this tomorrow.
       6 September 2022
     Need to add static variable to tell main sketch if interrupt is active.
   v.9.2: 8 September 2022
     Try adding that static variable to the ISR.
     Did that, works.
     Added prototypes to some of the long functions in main sketch.
     TODO: Make String keying_string="" global
       Put its manipulation in while(String of outgoing characters not empty)
       
  DK
*/
