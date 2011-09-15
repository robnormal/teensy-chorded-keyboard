#include "chordite_lib.c"

/***** MAIN PROCESSES BELOW HERE *****/

SwitchHistory *history_GLOBAL;

void setup() {
  // this allocation is never freed
  history_GLOBAL = newHistoryA();

	// TODO: create LAYOUT from config
  if (NULL == stringToSnapshotMA) {
    stringToSnapshotMA = &default_stringToSnapshotMA;
  }

  loadLayoutA(layoutString(), stringToSnapshotMA);
}

void loop() {
  // get function input
  Snapshot current = readInputsAIO(); // +1 Snapshot - deleted in restartHistoryD
  SwitchHistory *h = history_GLOBAL;

  // call the pure function
	ClockReturn *r = clock(current, h); // + 1 Output

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

