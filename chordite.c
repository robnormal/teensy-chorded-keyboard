#include <stdlib.h>

#define MAX_SNAPSHOTS 1000
#define MAX_OUTPUT 10

#define MALLOCS(t,s) (t *) myalloc(s * sizeof(t))
#define MALLOC(t) (t *) myalloc(sizeof(t))


/** KEYBOARD-SPECIFIC CONFIGURATION **/

#define LAYOUT_SIZE 50

#define PINKY_L 0
#define PINKY_H 1
#define RING_L 2
#define RING_H 3
#define MIDDLE_L 4
#define MIDDLE_H 5
#define INDEX_L 7
#define INDEX_H 8

#define UP LOW
#define DOWN HIGH

#define NUM_SWITCHES_I 2
#define NUM_SWITCHES_M 2
#define NUM_SWITCHES_R 2
#define NUM_SWITCHES_P 2

const int SWITCHES_I[NUM_SWITCHES_I] = {
  INDEX_L,
  INDEX_H
};
const int SWITCHES_M[NUM_SWITCHES_M] = {
  MIDDLE_L,
  MIDDLE_H
};

const int SWITCHES_R[NUM_SWITCHES_R] = {
  RING_L,
  RING_H
};
const int SWITCHES_P[NUM_SWITCHES_P] = {
  PINKY_L,
  PINKY_H
};

/** END **/



typedef short int boole;
typedef int Key;
typedef int FingerState;

typedef struct {
	FingerState index;
	FingerState middle;
	FingerState ring;
	FingerState pinky;
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


Key *LAYOUT[LAYOUT_SIZE]; // treat as constant once created

// REMOVE for use in Teensyduino
int digitalRead(int x);
int digitalRead(int x) {
  return 0;
}


void handleOutOfMemory();
void *myalloc(int size);

Snapshot      *newSnapshotA (const FingerState i,
                             const FingerState m,
                             const FingerState r,
                             const FingerState p);
SwitchHistory *newHistoryA ();
Output        *newOutputA ();

void deleteSnapshotD (Snapshot *s);
void deleteHistoryD (SwitchHistory *h);
void deleteOutputD (Output *o);


boole historyIsEmpty (const SwitchHistory *h);
boole isRelease      (const Snapshot *s);
boole historyTooLong (const SwitchHistory *h);

int chordId (const Snapshot *s);

Snapshot *chordFromM  (const SwitchHistory *h);
Output   *outputForMA (const Snapshot *sM, const SwitchHistory *h);


SwitchHistory *restartHistory (SwitchHistory *h);
SwitchHistory *addToHistory   (Snapshot *sM, SwitchHistory *h);

ClockReturn clockReturn (SwitchHistory *history, Output *output);
ClockReturn clockA      (Snapshot *currentM, SwitchHistory *history);

Snapshot *readInputsAIO ();
void      sendOutputIO  (const Output *oM);



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

Snapshot *newSnapshotA(const FingerState i, const FingerState m, const FingerState r, const FingerState p)
{
	Snapshot *s = MALLOC(Snapshot);

	s->index = i;
	s->middle = m;
	s->ring = r;
	s->pinky = p;

	return s;
}

void deleteSnapshotD(Snapshot *s)
{
  free(s);
}

SwitchHistory *newHistoryA()
{
	SwitchHistory *h = MALLOC(SwitchHistory);

	h->snapshots = MALLOCS(Snapshot *, MAX_SNAPSHOTS);
	h->place = 0;

	return h;
}

void deleteHistoryD(SwitchHistory *h)
{
  int i;

  for (i = 0; i < MAX_SNAPSHOTS; ++i) {
    free(h->snapshots[i]);
  }
  free(h->snapshots);
  free(h);
}

Output *newOutputA()
{
	Output *o = MALLOC(Output);
	o->keys   = MALLOCS(Key,MAX_OUTPUT); //(Key *) malloc(MAX_OUTPUT * sizeof(Key));

	return o;
}

void deleteOutputD(Output *o)
{
  free(o->keys);
  free(o);
}



boole historyIsEmpty(const SwitchHistory *h)
{
	return 0 == h->place;
}

boole isRelease(const Snapshot *s)
{
	return s->pinky == 0 && s->ring == 0 && s->middle == 0 && s->index == 0;
}

boole historyTooLong(const SwitchHistory *h)
{
	return h->place == MAX_SNAPSHOTS;
}

Snapshot *chordFromM(const SwitchHistory *h)
{
	if (h->place == 0) {
		return NULL;
	} else {
		return h->snapshots[0];
	}
}


// CHANGES h
SwitchHistory *restartHistory(SwitchHistory *h)
{
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

int chordId(const Snapshot *s)
{
	return s->index + 2*s->middle + 4*s->ring + 8*s->pinky;
}

Output *outputForMA(const Snapshot *sM, const SwitchHistory *h)
{
	if (NULL != sM && isRelease(sM)) {
		Snapshot *chordM = chordFromM(h);

		if (NULL != chordM) {
			Output *o = newOutputA();
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

ClockReturn clockA(Snapshot *currentM, SwitchHistory *history)
{
	SwitchHistory *new_h;
	Output *oM;

	new_h = addToHistory(currentM, history);
	oM    = outputForMA(currentM, new_h);

	return clockReturn(new_h, oM);
}


Snapshot *readInputsAIO()
{
  int i, multiplier;
  FingerState index = 0, middle = 0, ring = 0, pinky = 0;

  for (i = 0; i < NUM_SWITCHES_I; ++i) {
    index  = index  << 1 + digitalRead(SWITCHES_I[i]);
  }
  for (i = 0; i < NUM_SWITCHES_M; ++i) {
    middle = middle << 1 + digitalRead(SWITCHES_M[i]);
  }
  for (i = 0; i < NUM_SWITCHES_R; ++i) {
    ring   = ring   << 1 + digitalRead(SWITCHES_R[i]);
  }
  for (i = 0; i < NUM_SWITCHES_P; ++i) {
    pinky  = pinky  << 1 + digitalRead(SWITCHES_P[i]);
  }

  return newSnapshotA(index, middle, ring, pinky);
}

void sendOutputIO(const Output *oM)
{
	if (NULL != oM) {
		// TODO
	}
}





/***** MAIN PROCESSES BELOW HERE *****/

SwitchHistory *history_GLOBAL;

void setup() {
  // this allocation is never freed
  history_GLOBAL = newHistoryA();

	// TODO: create LAYOUT from config
}

void loop() {
	ClockReturn r = clockA(readInputsAIO(), history_GLOBAL);

	sendOutputIO(r.outputM);

  // set for next loop
	history_GLOBAL = r.history;

  // cleanup memory
  deleteHistoryD(r.history);
}


void main() {
	setup();
	for(;;) {
		loop();
	}
}

