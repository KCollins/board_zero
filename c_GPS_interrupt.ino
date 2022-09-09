// using tabs.  This is a_GPS_interrupt, containing the
// interrupt service routine (ISR) for the GPS pulse-per-second.

/*
   This ISR uses global variables trigger_period and secs_since_trigger
      trigger_period is the number of seconds between timing clock resets
      secs_since_trigger is number of seconds since last such reset.
   The ISR starts the timer interrupt if necessary and returns.      
*/

// 
// Clock accuracy measured at about 5 parts per million.
// May eventually want this ISR to define a phase-locked loop; maybe not.

void GPS_interrupt()  {
    static int secs_since_trigger  = 0;   // how many seconds since last clock reset
    Serial.println(secs_since_trigger);
    inside_GPS_interrupt=true; // mark to the sketch that this interrupt is executing
    if (secs_since_trigger==0)  { // start new timing synchronization
      key_transmitter();          // need this for first Morse period
      elementTimer.begin(key_transmitter, timer_rate);  // ticks run every 0.1 seconds
    } // if (restart elementTime)
    secs_since_trigger++; // otherwise increment seconds since last reset
    if (secs_since_trigger == trigger_period) { // roll over reset interval if needed
      secs_since_trigger=0; // count modulo secs_since_trigger, usually 10
    } // if (need to roll over setting period)
    inside_GPS_interrupt=false; // mark to the sketch that this interrupt is returning
} // GPS_interrupt()
