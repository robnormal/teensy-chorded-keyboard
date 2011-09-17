#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_SNAPSHOTS 1000
#define MAX_OUTPUT 10

#define MALLOCS(t,s) (t *) myalloc(s * sizeof(t))
#define MALLOC(t) (t *) myalloc(sizeof(t))

// add 1 to x's refs and return x
#define ref(x) (x->refs++ ? x : x)

#define INDEX 0
#define MIDDLE 1
#define RING 2
#define PINKY 3
#define THUMB 4

#define TRUE 1
#define FALSE 0

/** KEYBOARD-SPECIFIC CONFIGURATION **/

#define LAYOUT_SIZE 50
#define NUM_FINGERS 4

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

const int SWITCHES[NUM_FINGERS][2] = {
  { INDEX_L,  INDEX_H  }, 
  { MIDDLE_L, MIDDLE_H },
  { RING_L,   RING_H   },
  { PINKY_L,  PINKY_H  }
};

// chordite has 2 switches per finger, plus pressing _both_, which counts separately
const int NUM_SWITCHES[NUM_FINGERS] = {3, 3, 3, 3}; 

/** END **/


/** LAYOUT **/
char *layoutString()
{
  return 
    "0020 space\n"
    "0001 e\n"
  ;
}



typedef short int boole;
typedef int FingerState;
typedef FingerState * Snapshot;

typedef struct {
  int modifier;
  int key;
} Key;

typedef struct {
	Snapshot *snapshots;
	int place;
} SwitchHistory;

typedef struct {
	Key **keys;
  int count;
} Output;

typedef struct {
  int *ids;
  Snapshot *chords;
  Output   **outputs;
  int count;
} Layout;

typedef struct {
	SwitchHistory *history;
	Output *outputM;
} ClockReturn;


Layout *LAYOUT; // treat as constant once created
Snapshot (*stringToSnapshotMA)(const char *str) = NULL;  // only set once

// REMOVE for use in Teensyduino
int digitalRead(int x);
int digitalRead(int x) {
  return 0;
}
#define MODIFIERKEY_CTRL 1
#define MODIFIERKEY_SHIFT 2
#define MODIFIERKEY_ALT 3
#define MODIFIERKEY_GUI 4
#define KEY_ENTER 2
#define KEY_SPACE 3
#define KEY_ESC 4
#define KEY_BKSP 5






int ctoi(const char c);

void handleOutOfMemory();
void *myalloc(int size);

Key           *newKeyA     (const int key, const int modifier);
Snapshot      newSnapshotA ();
SwitchHistory *newHistoryA ();
Output        *newOutputA  ();
Layout        *newLayoutA  ();

void deleteSnapshotD          (Snapshot sM);
void deleteHistoryD           (SwitchHistory *hM);
void deleteOutputD            (Output *oM);
void discardHistorySnapshotsD (SwitchHistory *hM, int starting_at);


boole historyIsEmpty (const SwitchHistory *h);
boole historyTooLong (const SwitchHistory *h);
Snapshot chordFromM  (const SwitchHistory *h);

boole isRelease (const Snapshot s);
int chordId     (const Snapshot s);

SwitchHistory *restartHistoryD (SwitchHistory *h, int starting_at);
SwitchHistory *addToHistory    (Snapshot sM, SwitchHistory *h);

Output *addToOutput (Key *k, Output *o);

boole newSwitchPressed(const Snapshot a, const Snapshot b);
Output *outputForM (const Snapshot sM, const SwitchHistory *h, const Layout *l);
int chordIndex     (const Snapshot s, const Layout *l);

ClockReturn *clockReturn (SwitchHistory *history, Output *output);
ClockReturn *clock       (Snapshot currentM, SwitchHistory *history, Layout *l);

Snapshot readInputsAIO ();
void     sendKeyIO     (const Key *k);
void     sendOutputIO  (const Output *oM);

Layout   *loadLayoutA      (char *str, Snapshot (*toChordMA)(const char *str));
Output   *stringToOutputMA (const char *str);
Layout   *addToLayoutA     (Snapshot s, Output *o, Layout *l);



Snapshot copySnapshotA(const Snapshot s)
{
  Snapshot s_new = newSnapshotA();
  int i;

  for (i = 0; i < NUM_FINGERS; ++i) {
    s_new[i] = s[i];
  }

  return s_new;
}

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
Key *newKeyA(const int key, const int modifier)
{
  Key *k = MALLOC(Key);

  k->modifier = modifier;
  k->key      = key;

  return k;
}

Snapshot newSnapshotA()
{
	Snapshot s = MALLOCS(FingerState, NUM_FINGERS);

	return s;
}

void deleteSnapshotD(Snapshot sM)
{
  if (NULL != sM) {
    free(sM);
  }
}

SwitchHistory *newHistoryA()
{
	SwitchHistory *h = MALLOC(SwitchHistory);

	h->snapshots = MALLOCS(Snapshot, MAX_SNAPSHOTS);
	h->place = 0;

	return h;
}

void deleteHistoryD(SwitchHistory *hM)
{
  if (NULL != hM) {
    discardHistorySnapshotsD(hM, 0);

    free(hM->snapshots);
    free(hM);
  }
}

// -N Snapshots
void discardHistorySnapshotsD(SwitchHistory *h, int starting_at)
{
  int i;

  for (i = starting_at; i < h->place; ++i) {
    free(h->snapshots[i]);
  }
}

Output *newOutputA()
{
	Output *o = MALLOC(Output);
	o->keys   = MALLOCS(Key *,MAX_OUTPUT);
	o->count  = 0;

	return o;
}

void deleteOutputD(Output *oM)
{
  if (NULL != oM) {

    free(oM->keys); // individual keys live in LAYOUT, so are never freed
    free(oM);
  }
}

Layout *newLayoutA()
{
	Layout *l  = MALLOC(Layout);
	l->ids     = MALLOCS(int, LAYOUT_SIZE);
	l->outputs = MALLOCS(Output *, LAYOUT_SIZE);
	l->chords  = MALLOCS(Snapshot, LAYOUT_SIZE);
	l->count   = 0;

	return l;
}



boole historyIsEmpty(const SwitchHistory *h)
{
	return 0 == h->place;
}

// All switches have been released?
boole isRelease(const Snapshot s)
{
  int i;

  for (i = 0; i < NUM_FINGERS; ++i) {
    if (s[i] > 0) {
      return FALSE;
    }
  }
  
  return TRUE;
}

boole historyTooLong(const SwitchHistory *h)
{
	return h->place >= MAX_SNAPSHOTS;
}

int chordId(const Snapshot s)
{
  int i, multiplier = 1, id = 0;

	// number-base style, this forms unique IDS using the smallest possible integers
  for (i = 0; i < NUM_FINGERS; ++i) {
		id += s[i] * multiplier;

		multiplier *= NUM_SWITCHES[i];
	}

  return id;
}


// -N Snapshots
// CHANGES h
SwitchHistory *restartHistoryD(SwitchHistory *h, int starting_at)
{
  discardHistorySnapshotsD(h, starting_at);
	h->place = starting_at;

	return h;
}

Snapshot lastSnapshotM(const SwitchHistory *h) {
	if (historyIsEmpty(h)) {
		return NULL;
	} else {
		return h->snapshots[h->place - 1];
	}
}

/**
 * Append Snapshot to SwitchHistory. 
 *
 * If isRelease(Snapshot), discard history.
 *
 * If new switch is pressed, restart history beginning with the current Snapshot.
 *
 * If SwitchHistory would exceed its maximum length, replace the entire history
 * with just the chord that history would send out were it released now
 * (the chord that history is "working" on), then append the new Snapshot.
 */
// +1 Snapshot
// CHANGES h
SwitchHistory *addToHistory(Snapshot sM, SwitchHistory *h)
{
	if (sM != NULL) {
		if (historyTooLong(h)) {
			Snapshot new_firstM = chordFromM(h);

			if (NULL == new_firstM) {
				restartHistoryD(h, 0);
			} else {
				// copy chord, because it may be free()d when we restartHistoryD
				h->snapshots[0] = copySnapshotA(new_firstM);
				restartHistoryD(h, 1);
				addToHistory(sM, h);
			}
		} else if (isRelease(sM)) {
			restartHistoryD(h, 0);
		} else {
			Snapshot lastM = lastSnapshotM(h);

			if (NULL != lastM && newSwitchPressed(lastM, sM)) {
				restartHistoryD(h, 0);
				addToHistory(sM, h);
			} else {
				h->snapshots[h->place] = copySnapshotA(sM); // +1 Snapshot
				h->place++;
			}
		}
	}

	return h;
}

// Silently does nothing if o is already at max size
Output *addToOutput(Key *k, Output *o)
{
  if (o->count < MAX_OUTPUT) {
    o->keys[o->count] = k;
    o->count++;
  }

  return o;
}

int chordIndex(const Snapshot s, const Layout *l)
{
  const int id = chordId(s);
  int i;

  for (i = 0; i < LAYOUT_SIZE; ++i) {
    if (id == l->ids[i]) {
      return i;
    }
  }

  return -1;
}

Output *outputForM(const Snapshot sM, const SwitchHistory *h, const Layout *l)
{
	if (NULL != sM && isRelease(sM)) {
		Output *o;
		Snapshot chordM = chordFromM(h);

		if (NULL != chordM) {
			o = l->outputs[chordIndex(chordM, l)];
    } else {
      o = NULL;
    }

    deleteSnapshotD(chordM);

    return o;

	} else {
		return NULL;
	}
}



ClockReturn *clockReturn(SwitchHistory *h, Output *oM)
{
	ClockReturn *cr = MALLOC(ClockReturn);

  cr->history = h;
  cr->outputM = oM;

	return cr;
}

ClockReturn *clock(Snapshot currentM, SwitchHistory *history, Layout *l)
{
	SwitchHistory *new_h;
	Output *oM;

	oM    = outputForM(currentM, history, l);
	new_h = addToHistory(currentM, history);

	return clockReturn(new_h, oM);
}


// +1 Snapshot
Snapshot readInputsAIO()
{
  int i, j;
  FingerState state;

  Snapshot s = newSnapshotA();  // +1 Snapshot

  for (i = 0; i < NUM_FINGERS; ++i) {
    state = 0;

    for (j = 0; j < NUM_SWITCHES[j]; ++j) {
      // shift by 1, put new bit at end
      state  = state << 1 + digitalRead(SWITCHES[i][j]);
    }

    s[i] = state;
  }

  return s;
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

// +1 Snapshot M
// +1 Output M
Layout *loadLayoutStringsA(char * chordstrM, char *textM, Snapshot (*toChordMA)(const char *str), Layout *l)
{
  if (NULL != chordstrM && NULL != textM) {
    Snapshot chordM;
    Output *oM;

    // these 2 live in LAYOUT for duration of program
    chordM = (*toChordMA)(chordstrM); // +1 Snapshot M
    oM     = stringToOutputMA(textM); // +1 Output M

    if (NULL != chordM && NULL != oM) {
      l = addToLayoutA(chordM, oM, l);
    }
  }

	return l;
}

// +1 Layout
// +N Snapshot
// +N Output
Layout *loadLayoutA(char *str, Snapshot (*toChordMA)(const char *str))
{
  char *lineM     = MALLOCS(char, 40);
  char *chordstrM = MALLOCS(char, 8);
  char *textM     = MALLOCS(char, 20);
  int count = 0;

  // have to copy the string or strtok seg faults
  char newstr[500];
  strcpy(newstr, str);

	Layout *l = newLayoutA(); // +1 Layout

  lineM = strtok(newstr, "\n");
  while (NULL != lineM) {
    sscanf(lineM, "%s %s", chordstrM, textM);

    l = loadLayoutStringsA(chordstrM, textM, toChordMA, l); // +1 Snapshot M; +1 Output M

    lineM = strtok(NULL, "\n");
    ++count;
  }


  free(lineM); free(chordstrM); free(textM);

	return l;
}


/*** These functions must be altered for different keyboards ***/
Snapshot chordFromM(const SwitchHistory *h)
{
	if (h->place == 0) {
		return NULL;
	} else {
		return h->snapshots[0];
	}
}

boole newSwitchPressed(const Snapshot a, const Snapshot b)
{
	int i;

	// these are the rules for the chordite
	// 3 -> * = no
	// 2 -> {0|2} = no
	// 1 -> {0|1} = no
	// 0 -> {0|0} = no
	// otherwise  = yes
	for (i = 0; i < NUM_FINGERS; i++) {
		if ( b[i] > a[i]
		  || (a[i] == 2 && b[i] == 1)
		) {
			return TRUE;
		}
	}

	return FALSE;
}

/* REQUIREMENT: str must be at least NUM_FINGERS chars long */
Snapshot default_stringToSnapshotMA(const char *str)
{
  int i;
  Snapshot s = newSnapshotA();  // +1 Snapshot

  for (i = 0; i < NUM_FINGERS; ++i) {
    s[i] = ctoi(str[i]);
  }

  return s;
}



int ctoi(const char c)
{
  return c - '0';
}

// +N Keys
// +1 Output
Output *stringToOutputMA(const char *str)
{
  Key *k;

  if (strlen(str) == 1) {
    k = newKeyA(str[0], 0);
  } else if (strcmp(str, "control") == 0) {
    k = newKeyA(0, MODIFIERKEY_CTRL);
  } else if (strcmp(str, "shift") == 0) {
    k = newKeyA(0, MODIFIERKEY_SHIFT);
  } else if (strcmp(str, "alt") == 0) {
    k = newKeyA(0, MODIFIERKEY_ALT);
  } else if (strcmp(str, "win") == 0) {
    k = newKeyA(0, MODIFIERKEY_GUI);
  } else if (strcmp(str, "return") == 0) {
    k = newKeyA(KEY_ENTER, 0);
  } else if (strcmp(str, "space") == 0) {
    k = newKeyA(KEY_SPACE, 0);
  } else if (strcmp(str, "backspace") == 0) {
    k = newKeyA(KEY_BKSP, 0);
  } else if (strcmp(str, "esc") == 0) {
    k = newKeyA(KEY_ESC, 0);
  } else {
		int i, chars = strlen(str);
		Output *o = newOutputA();

		for (i = 0; i < chars; i++) {
			addToOutput(newKeyA(str[i], 0), o);
		}

    return o;
  }

  Output *o = newOutputA();
  addToOutput(k, o);

  return o;
}

Layout *addToLayoutA(Snapshot s, Output *o, Layout *l)
{
  l->ids    [l->count] = chordId(s);
  l->chords [l->count] = s;
  l->outputs[l->count] = o;

  l->count++;

  return l;
}


