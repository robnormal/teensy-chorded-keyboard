#include "chordite_lib.c"

/***** MAIN PROCESSES BELOW HERE *****/

void setupLayout()
{
	layoutAddChar( "0200", KEY_SPACE );
	layoutAddChar( "0100", 'e' );
	layoutAddChar( "1000", 't' );
	layoutAddMod(  "1100", MODIFIERKEY_CTRL );
}

SwitchHistory *history_GLOBAL;

void setup() {
  // this allocation is never freed
  history_GLOBAL = newHistoryA();

  // LAYOUT = loadLayoutA(layoutString(), stringToSnapshotMA);
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
	// history_GLOBAL = r->history;

  // cleanup
  deleteOutputD(r->outputM); // -1 Output
  free(r);
}

void main() {
	setup();
	for(;;) {
		loop();
	}

}

