#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "WProgram.h"

#define MAX_SNAPSHOTS 100
#define MAX_OUTPUT 3

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

#define LAYOUT_SIZE 25
#define NUM_FINGERS 4

#define PINKY_L 0
#define PINKY_H 1
#define RING_L 2
#define RING_H 3
#define MIDDLE_L 4
#define MIDDLE_H 5
#define INDEX_L 6
#define INDEX_H 7

#define UP LOW
#define DOWN HIGH

const int SWITCHES[NUM_FINGERS][2] = {
  { INDEX_L,  INDEX_H  }, 
  { MIDDLE_L, MIDDLE_H },
  { RING_L,   RING_H   },
  { PINKY_L,  PINKY_H  }
};
const int NUM_SWITCHES[NUM_FINGERS] = {2, 2, 2, 2}; 

// chordite has 2 switches per finger, plus pressing _both_, which counts separately
const int NUM_STATES[NUM_FINGERS] = {3, 3, 3, 3}; 

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

int ctoi(const char c);

void handleOutOfMemory();
void *myalloc(int size);

Key           *newKeyA     (const int key, const int modifier);
Snapshot      newSnapshotA ();
SwitchHistory *newHistoryA ();
Output        *newOutputA  (const int num_keys);
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

Output   *stringToOutputMA (const char *str);
Layout   *addToLayoutA     (Snapshot s, Output *o, Layout *l);
Layout   *layoutAddChar    (const char *chordstr, const char c);
Layout   *layoutAddString  (const char *chordstr, const char *str);
Layout   *layoutAddMod     (const char *chordstr, const char mod);
Layout   *layoutAddCharMod (const char *chordstr, const char c, const char mod);
Layout   *layoutAddOutput  (const char *chordstr, Output *o);

int charToCode(const char c);



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
  Keyboard.println("out of memory");
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

Output *newOutputA(const int num_keys)
{
  Output *o = MALLOC(Output);
  o->keys   = MALLOCS(Key *, num_keys);
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

    multiplier *= NUM_STATES[i];
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

int readPinIO(int pin)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  pinMode(pin, INPUT);

  return HIGH == digitalRead(pin) ? HIGH : LOW;
}

// +1 Snapshot
Snapshot readInputsAIO()
{
  int i, j;
  FingerState state;

  Snapshot s = newSnapshotA();  // +1 Snapshot


  char XX[30];
  int k = 0;
  for (i = 0; i < NUM_FINGERS; ++i) {
    for (j = 0; j < NUM_SWITCHES[i]; ++j) {
      XX[k] = HIGH == readPinIO(SWITCHES[i][j]) ? '0' : '_';
      k++;
    }
  }
  XX[k] = '\0';
  Keyboard.println(XX);

  for (i = 0; i < NUM_FINGERS; ++i) {
    state = 0;

    for (j = 0; j < NUM_SWITCHES[i]; ++j) {
      // shift by 1, put new bit at end
      state  = state << 1 + readPinIO(SWITCHES[i][j]);
    }

    s[i] = state;
  }
  /*
  sscanf(XX,
 "Reads: %i %i %i %i %i %i %i %i %i %i %i %i %i",
 (HIGH == digitalRead(0)) ? 1 : 0,
 (HIGH == digitalRead(1)) ? 1 : 0,
 (HIGH == digitalRead(2)) ? 1 : 0,
 (HIGH == digitalRead(3)) ? 1 : 0,
 (HIGH == digitalRead(4)) ? 1 : 0,
 (HIGH == digitalRead(5)) ? 1 : 0,
 (HIGH == digitalRead(6)) ? 1 : 0,
 (HIGH == digitalRead(7)) ? 1 : 0,
 (HIGH == digitalRead(8)) ? 1 : 0,
 (HIGH == digitalRead(9)) ? 1 : 0,
 (HIGH == digitalRead(10)) ? 1 : 0,
 (HIGH == digitalRead(11)) ? 1 : 0,
 (HIGH == digitalRead(12)) ? 1 : 0);
  Keyboard.println(XX);
  free(XX);
  */

  
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
    Keyboard.set_modifier(k->modifier);
  }
  if (k->key != 0) {
    Keyboard.set_key1(k->key);
  }

  if (k->key != 0 || k->modifier != 0) {
    // press the key
    Keyboard.send_now();

    // release the key
    Keyboard.set_key1(0);
    Keyboard.send_now();
  }
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
Snapshot stringToSnapshotMA(const char *str)
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
  int i, len = strlen(str);
  Output *o = newOutputA(len);

  for (i = 0; i < len; i++) {
    addToOutput(newKeyA(str[i], 0), o);
  }

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

Layout *layoutAddChar(const char *chordstr, const char c)
{
  return layoutAddOutput(chordstr, addToOutput(newKeyA(charToCode(c), 0), newOutputA(1)));
}

Layout *layoutAddString(const char *chordstr, const char *str)
{
  return layoutAddOutput(chordstr, stringToOutputMA(str));
}

Layout *layoutAddMod(const char *chordstr, const char mod)
{
  return layoutAddOutput(chordstr, addToOutput(newKeyA(0, mod), newOutputA(1)));
}

Layout *layoutAddCharMod(const char *chordstr, const char c, const char mod)
{
  return layoutAddOutput(chordstr, addToOutput(newKeyA(charToCode(c), mod), newOutputA(1)));
}

Layout *layoutAddOutput(const char *chordstr, Output *o)
{
  Snapshot s = stringToSnapshotMA(chordstr);

  if (NULL != s) {
    LAYOUT = addToLayoutA(s, o, LAYOUT);
    return LAYOUT;
  }
}

int charToCode(const char c)
{
  if ('a' <= c && c <= 'z') {
    return c - ('a' - KEY_A);
  } else if ('0' <= c && c <= '9') {
    return c - ('0' - KEY_0);
  } else {
    switch (c) {
    case '-':
      return KEY_MINUS;
    case '=':
      return KEY_EQUAL;
    case '[':
      return KEY_LEFT_BRACE;
    case ']':
      return KEY_RIGHT_BRACE;
    case '\\':
      return KEY_BACKSLASH;
    case ';':
      return KEY_SEMICOLON;
    case '\'':
      return KEY_QUOTE;
    case '`':
      return KEY_TILDE;
    case ',':
      return KEY_COMMA;
    case '.':
      return KEY_PERIOD;
    case '/':
      return KEY_SLASH;
    case '!':
      return ASCII_21;
    case '"':
      return ASCII_22;
    case '#':
      return ASCII_23;
    case '$':
      return ASCII_24;
    case '%':
      return ASCII_25;
    case '&':
      return ASCII_26;
    case '(':
      return ASCII_28;
    case ')':
      return ASCII_29;
    case '*':
      return ASCII_2A;
    case '+':
      return ASCII_2B;
    case ':':
      return ASCII_3A;
    case '<':
      return ASCII_3C;
    case '>':
      return ASCII_3E;
    case '?':
      return ASCII_3F;
    case '@':
      return ASCII_40;
    case '^':
      return ASCII_5E;
    case '_':
      return ASCII_5F;
    case '{':
      return ASCII_7B;
    case '|':
      return ASCII_7C;
    case '}':
      return ASCII_7D;
    case '~':
      return ASCII_7E;
    default:
      return c;
    }
  }
}

