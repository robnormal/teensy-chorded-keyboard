#include "chordite_lib.h"

int fake_inputs[100][8];
int fake_index;
int fake_total;
SwitchHistory *history_GLOBAL;


void setupLayout()
{
  LAYOUT = newLayoutA();

  layoutAddChar( "0003", 'c' );
}

void setup()
{
	setupLayout();

	int i;

	for (i = 0; i < 5; ++i) {
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

	for (i = 5; i < 10; ++i) {
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

	for (i = 10; i < 15; ++i) {
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

	for (i = 15; i < 25; ++i) {
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

	fake_total = 25;
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

	sendOutputIO(r->outputM);

	// set function output
	// THIS ALREADY HAPPENED - just here for reference
	history_GLOBAL = r->history;

	// cleanup

	// EXPLAIN ME: why do I have to NOT delete this output?
	// deleteOutputD(r->outputM); // -1 Output

	free(r);

	fake_index++;
}

int main()
{
	setup();

	int j;
	for (j = 0; j <= 25; j++) {
		loop();
	}
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
  if (k->modifier != 0) {
		printf("is modified\n");
	}
  if (k->key != 0) {
		printf("char: %c\n", k->key);
	}
}

int charToCode(const char c)
{
	return c;
}

