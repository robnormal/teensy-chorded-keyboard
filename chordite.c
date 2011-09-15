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
typedef int FingerState;

typedef struct {
  int modifier;
  int key;
} Key;

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
	Key **keys;
  int count;
} Output;

typedef struct {
	SwitchHistory *history;
	Output *outputM;
} ClockReturn;


Key **LAYOUT[LAYOUT_SIZE]; // treat as constant once created

// REMOVE for use in Teensyduino
int digitalRead(int x);
int digitalRead(int x) {
  return 0;
}


void handleOutOfMemory();
void *myalloc(int size);

Key           *newKey       (const int key, const int modifier);
Snapshot      *newSnapshotA (const FingerState i,
                             const FingerState m,
                             const FingerState r,
                             const FingerState p);
SwitchHistory *newHistoryA ();
Output        *newOutputA  ();

void deleteSnapshotD          (Snapshot *sM);
void deleteHistoryD           (SwitchHistory *hM);
void deleteOutputD            (Output *oM);
void discardHistorySnapshotsD (SwitchHistory *hM);


boole historyIsEmpty (const SwitchHistory *h);
boole isRelease      (const Snapshot *s);
boole historyTooLong (const SwitchHistory *h);

int chordId (const Snapshot *s);

Snapshot *chordFromM  (const SwitchHistory *h);
Output   *outputForMA (const Snapshot *sM, const SwitchHistory *h);


SwitchHistory *restartHistoryD (SwitchHistory *h);
SwitchHistory *addToHistory   (Snapshot *sM, SwitchHistory *h);

ClockReturn clockReturn (SwitchHistory *history, Output *output);
ClockReturn clockA      (Snapshot *currentM, SwitchHistory *history);

Snapshot *readInputsAIO ();
void      sendKeyIO     (const Key *k);
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


// Never deleted - these live in LAYOUT for duration of program
Key *newKey(const int key, const int modifier)
{
  Key *k = MALLOC(Key);

  k->modifier = modifier;
  k->key      = key;

  return k;
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

void deleteSnapshotD(Snapshot *sM)
{
  if (NULL != sM) {
    free(sM);
  }
}

SwitchHistory *newHistoryA()
{
	SwitchHistory *h = MALLOC(SwitchHistory);

	h->snapshots = MALLOCS(Snapshot *, MAX_SNAPSHOTS);
	h->place = 0;

	return h;
}

void deleteHistoryD(SwitchHistory *hM)
{
  if (NULL != hM) {
    int i;

    discardHistorySnapshotsD(hM);

    free(hM->snapshots);
    free(hM);
  }
}

// -N Snapshots
void discardHistorySnapshotsD(SwitchHistory *h)
{
  int i;

  for (i = 0; i < MAX_SNAPSHOTS; ++i) {
    free(h->snapshots[i]);
  }
}

Output *newOutputA()
{
	Output *o = MALLOC(Output);
	o->keys   = MALLOCS(Key *,MAX_OUTPUT);

	return o;
}

void deleteOutputD(Output *oM)
{
  if (NULL != oM) {
    int i;

    free(oM->keys); // individual keys live in LAYOUT, so are never freed
    free(oM);
  }
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


// -N Snapshots
// CHANGES h
SwitchHistory *restartHistoryD(SwitchHistory *h)
{
	h->place = 0;
  discardHistorySnapshotsD(h);

	return h;
}

// CHANGES h
SwitchHistory *addToHistory(Snapshot *sM, SwitchHistory *h)
{
	if (sM != NULL) {
		if (historyTooLong(h)) {
			Snapshot *s0M = chordFromM(h);

			restartHistoryD(h);
			addToHistory(s0M, h);
			addToHistory(sM, h);
		} else if (isRelease(sM)) {
			restartHistoryD(h);
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

// +1 Output M
Output *outputForMA(const Snapshot *sM, const SwitchHistory *h)
{
	if (NULL != sM && isRelease(sM)) {
		Output *o;
		Snapshot *chordM = chordFromM(h);

		if (NULL != chordM) {
			o = newOutputA();
			o->keys = LAYOUT[chordId(chordM)];
    } else {
      o = NULL;
    }

    deleteSnapshotD(chordM);

    return o;

	} else {
		return NULL;
	}
}

ClockReturn clockReturn(SwitchHistory *history, Output *outputM)
{
	ClockReturn cr = {history, outputM};

	return cr;
}

// +1 Output M
ClockReturn clockA(Snapshot *currentM, SwitchHistory *history)
{
	SwitchHistory *new_h;
	Output *oM;

	new_h = addToHistory(currentM, history);
	oM    = outputForMA(currentM, new_h);

	return clockReturn(new_h, oM);
}


// +1 Snapshot
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
    int i;

    for (i = 0; i < oM->count; ++i) {
      sendKeyIO(oM->keys[i]);
    }
	}
}

// TODO: uncomment Keyboard lines in Teensyduino
void sendKeyIO(const Key *k)
{
  if (k->modifier != 0) {
    // Keyboard.set_modifier(k->modifier);
  }
  if (k->key != 0) {
    // Keyboard.set_key1(k->key);
  }

  if (k->key != 0 || k->modifier != 0) {
    // press the key
    // Keyboard.send_now();

    // release the key
    // Keyboard.set_key1(0);
    // Keyboard.send_now();
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
  // get function input
  Snapshot *current = readInputsAIO(); // +1 Snapshot - deleted in restartHistoryD
  SwitchHistory * h = history_GLOBAL;

  // call the pure function
	ClockReturn r = clockA(current, h); // + 1 Output

	sendOutputIO(r.outputM);

  // set function output
	history_GLOBAL = r.history;

  // cleanup
  deleteOutputD(r.outputM); // -1 Output
}


void main() {
	setup();
	for(;;) {
		loop();
	}
}

