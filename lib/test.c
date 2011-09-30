#include "../chordite_lib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../chordite_lib.c"

void putss(const char *c)
{
	printf("%s\n", c);
}

void deleteLayoutD(Layout *l)
{
	int i;
	for (i = 0; i < l->count; i++) {
		deleteOutputD(l->outputs[i]);
		deleteSnapshotD(l->chords[i]);
	}
	free(l->outputs);
	free(l->chords);
	free(l->ids);
	free(l);
}


int fake_inputs[1000][8];
int fake_index;
int fake_total;
SwitchHistory *history_GLOBAL;


void setupLayout()
{
  LAYOUT = newLayoutA();

  layoutAddChar( "0003", 'c' );
  layoutAddMod ( "2000", 1 );
}

void setup()
{
	setupLayout();

	int i, unit = 25;

	for (i = 0; i < unit; ++i) {
		fake_inputs[i][0] = 
		fake_inputs[i][1] = 
		fake_inputs[i][2] = 
		fake_inputs[i][3] = 
		fake_inputs[i][4] = 
		fake_inputs[i][5] = 
		fake_inputs[i][6] = 
		fake_inputs[i][7] = 
			0;
	}

	for (i = unit; i < unit * 2; ++i) {
		fake_inputs[i][0] = 1;

		fake_inputs[i][1] = 
		fake_inputs[i][2] = 
		fake_inputs[i][3] = 
		fake_inputs[i][4] = 
		fake_inputs[i][5] = 
		fake_inputs[i][6] = 
		fake_inputs[i][7] = 
			0;
	}

	for (i = 2*unit; i < 3*unit; ++i) {
		fake_inputs[i][0] = 1;
		fake_inputs[i][1] = 1;

		fake_inputs[i][2] = 
		fake_inputs[i][3] = 
		fake_inputs[i][4] = 
		fake_inputs[i][5] = 
		fake_inputs[i][6] = 
		fake_inputs[i][7] = 
			0;
	}

	for (i = 3*unit; i < 4*unit; ++i) {
		fake_inputs[i][1] = 1;

		fake_inputs[i][0] =
		fake_inputs[i][2] = 
		fake_inputs[i][3] = 
		fake_inputs[i][4] = 
		fake_inputs[i][5] = 
		fake_inputs[i][6] = 
		fake_inputs[i][7] = 
			0;
	}

  printf("show 'c':\n");

	for (i = 4*unit; i < 5*unit; ++i) {
		fake_inputs[i][0] =
		fake_inputs[i][1] =
		fake_inputs[i][2] = 
		fake_inputs[i][3] = 
		fake_inputs[i][4] = 
		fake_inputs[i][5] = 
		fake_inputs[i][6] = 
		fake_inputs[i][7] = 
			0;
	}

  // SHIFT
	for (i = 5*unit; i < 6*unit; ++i) {
		fake_inputs[i][6] = 1;

		fake_inputs[i][0] =
		fake_inputs[i][1] =
		fake_inputs[i][2] = 
		fake_inputs[i][3] = 
		fake_inputs[i][4] = 
		fake_inputs[i][5] = 
		fake_inputs[i][7] = 
			0;
	}

	for (i = 6*unit; i < 7*unit; ++i) {
		fake_inputs[i][0] =
		fake_inputs[i][1] =
		fake_inputs[i][2] = 
		fake_inputs[i][3] = 
		fake_inputs[i][4] = 
		fake_inputs[i][5] = 
		fake_inputs[i][6] = 
		fake_inputs[i][7] =
			0;
	}

  // c again - should be capital this time
	for (i = 7*unit; i < 8*unit; ++i) {
		fake_inputs[i][0] = 1;
		fake_inputs[i][1] = 1;

		fake_inputs[i][2] = 
		fake_inputs[i][3] = 
		fake_inputs[i][4] = 
		fake_inputs[i][5] = 
		fake_inputs[i][6] = 
		fake_inputs[i][7] = 
			0;
	}

  printf("show 'C':\n");

	for (i = 8*unit; i < 9*unit; ++i) {
		fake_inputs[i][0] =
		fake_inputs[i][1] =
		fake_inputs[i][2] = 
		fake_inputs[i][3] = 
		fake_inputs[i][4] = 
		fake_inputs[i][5] = 
		fake_inputs[i][6] = 
		fake_inputs[i][7] = 
			0;
	}

  // c one last time - should no longer be capital
	for (i = 9*unit; i < 10*unit; ++i) {
		fake_inputs[i][0] = 1;
		fake_inputs[i][1] = 1;

		fake_inputs[i][2] = 
		fake_inputs[i][3] = 
		fake_inputs[i][4] = 
		fake_inputs[i][5] = 
		fake_inputs[i][6] = 
		fake_inputs[i][7] = 
			0;
	}

  printf("show 'c' again:\n");

	for (i = 10*unit; i < 11*unit; ++i) {
		fake_inputs[i][0] =
		fake_inputs[i][1] =
		fake_inputs[i][2] = 
		fake_inputs[i][3] = 
		fake_inputs[i][4] = 
		fake_inputs[i][5] = 
		fake_inputs[i][6] = 
		fake_inputs[i][7] = 
			0;
	}

	fake_total = 11*unit;
	fake_index = 0;

	history_GLOBAL = newHistoryA();
}

void loop()
{
	// get function input
	Snapshot current = readInputsAIO(); // +1 Snapshot - deleted in restartHistoryD
	SwitchHistory *h = history_GLOBAL;

	// call the pure function
	ClockReturn *r = clock(current, h, LAYOUT); // + 1 Output

	deleteSnapshotD(current);

  modifier_GLOBAL = sendOutputIO(r->outputM, modifier_GLOBAL);

	// set function output
	// THIS ALREADY HAPPENED - just here for reference
	history_GLOBAL = r->history;

	// cleanup

	// EXPLAIN ME: why do I have to NOT delete this output?
	// deleteOutputD(r->outputM); // -1 Output

	free(r);

	fake_index++;
	if (fake_index >= fake_total) {
		// restart cycle
		fake_index = 0;
	}
}

int main()
{
	setup();

	int j;
	for (j = 0; j < fake_total; j++) {
		loop();
	}

	deleteHistoryD(history_GLOBAL);
	deleteLayoutD(LAYOUT);
}

void handleOutOfMemory()
{
	printf("out of memory\n");
}

int readPinIO(int pin)
{
	if (fake_index < fake_total) {
		return fake_inputs[fake_index][pin];
	} else {
		return 0;
	}
}

void sendKeyIO(const Key *k)
{
  if (k->key != 0) {
    if (k->modifier != 0) {
      printf("is modified: ");
    }
		printf("char: %c\n", k->key);
	}
}

int charToCode(const char c)
{
	return c;
}

