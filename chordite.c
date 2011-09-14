#include <stdlib.h>

#define MAX_SNAPSHOTS 1000
#define MAX_OUTPUT 10
#define LAYOUT_SIZE 50

#define MALLOCS(t,s) (t *) myalloc(s * sizeof(t))
#define MALLOC(t) (t *) myalloc(sizeof(t))

typedef short int boole;
typedef int Key;

typedef struct {
	boole index;
	boole middle;
	boole ring;
	boole pinky;
} Snapshot;

typedef struct {
	Snapshot **snapshots;
	int place;
} SwitchHistory;

typedef struct {
	Key *keys;
} Output;

typedef struct {
	SwitchHistory *history;
	Output *outputM;
} ClockReturn;

void handleOutOfMemory()
{
	exit(1);
}

void *myalloc(int size)
{
	void *x = malloc(size);

	if (NULL == x) {
		handleOutOfMemory();
	}

	return x;
}

Snapshot *newSnapshot(const boole i, const boole m, const boole r, const boole p)
{
	Snapshot *s = MALLOC(Snapshot);

	s->index = i;
	s->middle = m;
	s->ring = r;
	s->pinky = p;

	return s;
}

SwitchHistory *newHistory() {
	SwitchHistory *h = MALLOC(SwitchHistory);

	h->snapshots = MALLOCS(Snapshot *, MAX_SNAPSHOTS);
	h->place = 0;

	return h;
}

Output *newOutput()
{
	Output *o = MALLOC(Output);
	o->keys   = MALLOCS(Key,MAX_OUTPUT); //(Key *) malloc(MAX_OUTPUT * sizeof(Key));

	return o;
}

boole historyIsEmpty(SwitchHistory *h) {
	return 0 == h->place;
}

boole isRelease(const Snapshot *s) {
	return s->pinky == 0 && s->ring == 0 && s->middle == 0 && s->index == 0;
}

boole historyTooLong(const SwitchHistory *h) {
	return h->place == MAX_SNAPSHOTS;
}

Snapshot *chordFromM(SwitchHistory *h) {
	if (h->place == 0) {
		return NULL;
	} else {
		return h->snapshots[0];
	}
}


SwitchHistory *restartHistory(SwitchHistory *h) {
	h->place = 0;
	return h;
}

// CHANGES h
SwitchHistory *addToHistory(Snapshot *sM, SwitchHistory *h)
{
	if (sM != NULL) {
		if (historyTooLong(h)) {
			Snapshot *s0M = chordFromM(h);

			restartHistory(h);
			addToHistory(s0M, h);
			addToHistory(sM, h);
		} else if (isRelease(sM)) {
			restartHistory(h);
		} else {
			h->snapshots[h->place] = sM;
			h->place++;
		}
	}

	return h;
}

int chordId(Snapshot *s)
{
	return s->index + 2*s->middle + 4*s->ring + 8*s->pinky;
}

Output *outputForM(Snapshot *sM, SwitchHistory *h)
{
	if (NULL != sM && isRelease(sM)) {
		Snapshot *chordM = chordFromM(h);

		if (NULL != chordM) {
			Output *o = newOutput();
			o->keys = LAYOUT[chordId(chordM)];

			return o;
		} else {
			return NULL;
		}
	} else {
		return NULL;
	}
}

ClockReturn clockReturn(SwitchHistory *history, Output *output)
{
	ClockReturn cr = {history, output};
	return cr;
}

ClockReturn clock(Snapshot *currentM, SwitchHistory *history) {
	Output  *oM;
	SwitchHistory *new_h;

	new_h = addToHistory(currentM, history);
	oM    = outputForM(currentM, new_h);

	return clockReturn(new_h, oM);
}


Snapshot *readInputsIO() {
	// TODO
}

void sendOutputIO(Output *oM)
{
	if (NULL != oM) {
		// TODO
	}
}





/***** MAIN PROCESSES BELOW HERE *****/

SwitchHistory *history_GLOBAL;
Key LAYOUT[LAYOUT_SIZE]; // treat as constant once created

void setup() {
	// TODO: create LAYOUT from config
}

void loop() {
	ClockReturn r = clock(readInputsIO(), history_GLOBAL);

	sendOutputIO(r.outputM);

	history_GLOBAL = r.history;
}


void main() {
	setup();
	for(;;) {
		loop();
	}
}

