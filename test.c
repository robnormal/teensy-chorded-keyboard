#include "chordite_lib.h"

int fake_inputs[100][8];
int fake_index;
int fake_total;


void setupLayout()
{
  LAYOUT = newLayoutA();

  layoutAddChar( "3000", 'c' );
}

int main()
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

	for (i = 15; i < 20; ++i) {
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

	fake_total = 20;
	fake_index = 0;
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
		putchar(k->key);
	}
}

int charToCode(const char c)
{
	return c;
}

