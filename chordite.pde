#include "chordite_lib.h"

/***** MAIN PROCESSES BELOW HERE *****/

void setupLayout()
{
  layoutAddChar( "0200", KEY_SPACE );
  layoutAddChar( "0100", KEY_E );
  layoutAddChar( "1000", KEY_T );
  layoutAddMod(  "1100", MODIFIERKEY_CTRL );
  layoutAddChar( "0201", KEY_S );
  layoutAddChar( "0101", KEY_R );
  layoutAddChar( "1001", KEY_N );
  layoutAddMod(  "1101", MODIFIERKEY_SHIFT );
  layoutAddChar( "0231", KEY_S );
  layoutAddChar( "0131", KEY_R );
  layoutAddChar( "1031", KEY_N );
  layoutAddChar( "0231", KEY_S );
  layoutAddChar( "0131", KEY_R );
  layoutAddChar( "1031", KEY_N );
  layoutAddChar( "0231", KEY_S );
  layoutAddChar( "0131", KEY_R );
  layoutAddChar( "1031", KEY_N );
  layoutAddChar( "0231", KEY_S );
  layoutAddChar( "0131", KEY_R );
  layoutAddChar( "1031", KEY_N );
}

SwitchHistory *history_GLOBAL;

Key *mine;
void setup() {
  Serial.begin(9600);
  delay(1000);

  // read pins
  /*
  for (int i = 0; i < SWITCHES; i++) {
    pinMode(SWITCH_ORDER[i], INPUT);
  }
  */

  for (int i = 0; i < 13; i++) {
    pinMode(i, INPUT);
  }


  // this allocation is never freed
  history_GLOBAL = newHistoryA();

  LAYOUT = newLayoutA();
  setupLayout();
}


void loop() {
  // get function input
  Snapshot current = readInputsAIO(); // +1 Snapshot - deleted in restartHistoryD
  SwitchHistory *h = history_GLOBAL;

  // call the pure function
  ClockReturn *r = clock(current, h, LAYOUT); // + 1 Output

  deleteSnapshotD(current);

  sendOutputIO(r->outputM);

  // set function output
  // THIS ALREADY HAPPENED - just here for reference
  history_GLOBAL = r->history;

  // cleanup

  // EXPLAIN ME: why do I have to NOT delete this output?
  // deleteOutputD(r->outputM); // -1 Output

  free(r);

  // Keyboard.println("u;;r");
  delay(100);
}

