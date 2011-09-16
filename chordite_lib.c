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

const int NUM_SWITCHES[NUM_FINGERS] = {2, 2, 2, 2};

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
  int ids[LAYOUT_SIZE];
  Snapshot chords[LAYOUT_SIZE];
  Output   *outputs[LAYOUT_SIZE];
  int count;
} Layout;

typedef struct {
	SwitchHistory *history;
	Output *outputM;
} ClockReturn;


Layout LAYOUT; // treat as constant once created
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

Key           *newKeyA      (const int key, const int modifier);
Snapshot      newSnapshotA  ();
SwitchHistory *newHistoryA  ();
Output        *newOutputA   ();

void deleteSnapshotD          (Snapshot sM);
void deleteHistoryD           (SwitchHistory *hM);
void deleteOutputD            (Output *oM);
void discardHistorySnapshotsD (SwitchHistory *hM, int starting_at);


boole historyIsEmpty (const SwitchHistory *h);
boole isRelease      (const Snapshot s);
boole historyTooLong (const SwitchHistory *h);

int chordId (const Snapshot s);

Snapshot chordFromM (const SwitchHistory *h);
Output   *outputForM (const Snapshot sM, const SwitchHistory *h);


SwitchHistory *restartHistoryD (SwitchHistory *h, int starting_at);
SwitchHistory *addToHistory    (Snapshot sM, SwitchHistory *h);
Output        *addToOutput     (Key *k, Output *o);

ClockReturn *clockReturn (SwitchHistory *history, Output *output);
ClockReturn *clock       (Snapshot currentM, SwitchHistory *history);

Snapshot readInputsAIO ();
void      sendKeyIO     (const Key *k);
void      sendOutputIO  (const Output *oM);

void      loadLayoutA        (char *str, Snapshot (*toChordMA)(const char *str));
Output   *stringToOutputMA   (const char *str);
void      addToLayoutA       (Snapshot s, Output *o);





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

	return o;
}

void deleteOutputD(Output *oM)
{
  if (NULL != oM) {

    free(oM->keys); // individual keys live in LAYOUT, so are never freed
    free(oM);
  }
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

Snapshot chordFromM(const SwitchHistory *h)
{
	if (h->place == 0) {
		return NULL;
	} else {
		return h->snapshots[0];
	}
}


// -N Snapshots
// CHANGES h
SwitchHistory *restartHistoryD(SwitchHistory *h, int starting_at)
{
  discardHistorySnapshotsD(h, starting_at);
	h->place = 0;

	return h;
}

// +1 Snapshot
// CHANGES h
SwitchHistory *addToHistory(Snapshot sM, SwitchHistory *h)
{
	if (sM != NULL) {
		if (historyTooLong(h)) {
			restartHistoryD(h, 1);
			addToHistory(sM, h);
		} else if (isRelease(sM)) {
			restartHistoryD(h, 0);
		} else {
			h->snapshots[h->place] = copySnapshotA(sM); // +1 Snapshot
			h->place++;
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

int chordId(const Snapshot s)
{
  int i, id = 0;

  for (i = 0; i < NUM_FINGERS; ++i) {
    id = id << NUM_SWITCHES[i] + s[i];
  }

  return id;
}

int chordIndex(const Snapshot s)
{
  const int id = chordId(s);
  int i;

  for (i = 0; i < LAYOUT_SIZE; ++i) {
    if (id == LAYOUT.ids[i]) {
      return i;
    }
  }

  return -1;
}

Output *outputForM(const Snapshot sM, const SwitchHistory *h)
{
	if (NULL != sM && isRelease(sM)) {
		Output *o;
		Snapshot chordM = chordFromM(h);

		if (NULL != chordM) {
			o = LAYOUT.outputs[chordIndex(chordM)];
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

ClockReturn *clock(Snapshot currentM, SwitchHistory *history)
{
	SwitchHistory *new_h;
	Output *oM;

	new_h = addToHistory(currentM, history);
	oM    = outputForM(currentM, new_h);

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
void loadLayoutStringsA(char * chordstrM, char *textM, Snapshot (*toChordMA)(const char *str))
{
  if (NULL != chordstrM && NULL != textM) {
    Snapshot chordM;
    Output *oM;

    // these 2 live in LAYOUT for duration of program
    chordM = (*toChordMA)(chordstrM); // +1 Snapshot M
    oM     = stringToOutputMA(textM); // +1 Output M

    if (NULL != chordM && NULL != oM) {
      addToLayoutA(chordM, oM);
    }
  }
}

void loadLayoutA(char *str, Snapshot (*toChordMA)(const char *str))
{
  char *lineM     = MALLOCS(char, 40);
  char *chordstrM = MALLOCS(char, 8);
  char *textM     = MALLOCS(char, 20);
  int count = 0;

  // have to copy the string or strtok seg faults
  char newstr[500];
  strcpy(newstr, str);


  lineM = strtok(newstr, "\n");
  while (NULL != lineM) {
    sscanf(lineM, "%s %s", chordstrM, textM);

    loadLayoutStringsA(chordstrM, textM, toChordMA);

    lineM = strtok(NULL, "\n");
    ++count;
  }


  free(lineM); free(chordstrM); free(textM);
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

// +1 Key
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
    // no output
    return NULL;
  }

  Output *o = newOutputA();
  addToOutput(k, o);

  return o;
}

void addToLayoutA(Snapshot s, Output *o)
{
  LAYOUT.chords[LAYOUT.count]  = s;
  LAYOUT.ids[LAYOUT.count]     = chordId(s);
  LAYOUT.outputs[LAYOUT.count] = o;
  LAYOUT.count++;
}


