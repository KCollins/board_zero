/* This is the interrupt service routine for the timer.
 *  It will need to include the receive-side signal processing.
 * 
 * Using tabs.  This is b_key_transmitter(), containing the
 * function for determining if transmitter key should be up or down
 * in the current timing interval.
 *  Function processes global String keyer_buf.
 *  If keyer_buf is empty, it keeps the key up.
 *  ELSE:
 *    Pop the first character.
 *      If 0, keep key up.
 *      If 1, put  key down.
 *      
 *  Hard-coded for now:
 *    When keyer_buf depleted, reset it and start over.
 */
#include <Arduino.h>
void key_transmitter()  { // put the MOSFET to key-down as necessary
  inside_key_transmitter=true; // Mark to sketch that this ISR is executing
  //String keyer_buf_init=keyer_buf;
  int key_state=LOW; // default state for each keying interval
  
  if (keyer_buf.length()>0) { // something left to buffer out
    char char0=keyer_buf.charAt(0); // store zeroth character temporarily
    keyer_buf.remove(0,1);          // and pop it
    if (char0=='1') {
      key_state=HIGH;  // key down if needed for this element
    }
  digitalWrite(XMIT_PIN, key_state); // key transmitter as needed
  } // if(keyer_buf)
  else  { // buffer is empty, refill it for this demo. Should run as a loop.
    // TODO: Might have a flag here indicating if buffer is to refill
    keyer_buf=HELLO;
    key_transmitter(); // for this demo, need to restart keying.
                       // otherwise can idle until buffer has content.
  }
  inside_key_transmitter=false; // Mark to sketch that this ISR is exiting
} // key_transmitter()
