/**
 * Suffix legend:
 *
 * M:  "Maybe" - values that may be NULL, or functions that may return NULL
 * A:  "Allocates" - function that allocates memory it does not free. 
 *        Check comment for how much memory. Refers to the net effect of 
 *        the function. Thus, a function that calls an "A" and does not
 *        free the resulting allocation must also end in "A".
 * D:  "Deallocates" - function that frees memory it does not allocate.
 *        Refers to net effect of function.
 * IO: "I/O" - performs input/output. Refers to net effect of function.
 */
#ifndef CHORDED_LIB
#define CHORDED_LIB

/** KEYBOARD-SPECIFIC CONFIGURATION **/

#define LAYOUT_SIZE 61
#define NUM_FINGERS 5

#define PINKY_L 0
#define PINKY_H 1
#define RING_L 2
#define RING_H 3
#define MIDDLE_L 4
#define MIDDLE_H 5
#define INDEX_L 6
#define INDEX_H 7
#define THUMB_L 8
#define THUMB_H 9

#define LEFT_HANDED TRUE

#define UP HIGH
#define DOWN LOW

/** END **/

#define MAX_SNAPSHOTS 10
#define MAX_OUTPUT 3

#define MALLOCS(t,s) (t *) myalloc(s * sizeof(t))
#define MALLOC(t) (t *) myalloc(sizeof(t))

#define INDEX 0
#define MIDDLE 1
#define RING 2
#define PINKY 3
#define THUMB 4

#define TRUE 1
#define FALSE 0

#define PULLUP 1

typedef unsigned char boole;
typedef unsigned char integer;
typedef unsigned char FingerState;
typedef FingerState * Snapshot;

typedef struct {
  integer modifier;
  int key;
} Key;

typedef struct {
  Snapshot *snapshots;
  integer place;
} SwitchHistory;

typedef struct {
  Key **keys;
  integer count;
} Output;

typedef struct {
  int *ids;
  Snapshot *chords;
  Output   **outputs;
  integer count;
} Layout;

typedef struct {
  SwitchHistory *history;
  Output *outputM;
} ClockReturn;


integer ctoi(const char c);

void handleOutOfMemory();
void *myalloc(int size);

Key           *newKeyA     (const int key, const integer modifier);
Snapshot      newSnapshotA ();
SwitchHistory *newHistoryA ();
Output        *newOutputA  (const integer num_keys);
Layout        *newLayoutA  ();

void deleteSnapshotD          (Snapshot sM);
void deleteHistoryD           (SwitchHistory *hM);
void deleteOutputD            (Output *oM);
void discardHistorySnapshotsD (SwitchHistory *hM, integer starting_at);

Snapshot copySnapshotA(const Snapshot s);

boole historyIsEmpty (const SwitchHistory *h);
boole historyTooLong (const SwitchHistory *h);
Snapshot chordFromM  (const SwitchHistory *h);

boole isRelease (const Snapshot s);
int chordId     (const Snapshot s);

SwitchHistory *restartHistoryD (SwitchHistory *h, integer starting_at);
SwitchHistory *addToHistory    (Snapshot sM, SwitchHistory *h);

Output *addToOutput (Key *k, Output *o);

boole newSwitchPressed(const Snapshot a, const Snapshot b);
Output *outputForM (const Snapshot sM, const SwitchHistory *h, const Layout *l);
integer chordIndex     (const Snapshot s, const Layout *l);

ClockReturn *clockReturn (SwitchHistory *history, Output *output);
ClockReturn *clock       (Snapshot currentM, SwitchHistory *history, Layout *l);

Snapshot readInputsAIO ();
integer  sendOutputIO  (const Output *oM, integer modifier);

Key      *outputKeyA   (const Key *k, const integer modifier);

Output   *stringToOutputMA (const char *str);
Layout   *addToLayoutA     (Snapshot s, Output *o, Layout *l);
Layout   *layoutAddKey     (const char *chordstr, Key *k);
Layout   *layoutAddChar    (const char *chordstr, const char c);
Layout   *layoutAddString  (const char *chordstr, const char *str);
Layout   *layoutAddMod     (const char *chordstr, const char mod);
Layout   *layoutAddCharMod (const char *chordstr, const char c, const char mod);
Layout   *layoutAddOutput  (const char *chordstr, Output *o);

/** GLOBALS **/
Layout        *LAYOUT; // treat as constant once created
SwitchHistory *history_GLOBAL;
integer           modifier_GLOBAL; // context modifier - can be left over from last chord release

/**
 * These are NOT defined in chorded_kbd_lib.c
 * You must include teensy_chorded_keyboard.pde or other
 * source file for these to work
 */
void handleOutOfMemory ();
void iDebugOut          (int msg);
void strDebugOut          (char *msg);
void sendKeyIO         (const Key *k);
integer  readPinIO     (integer pin);
Key  *charToKeyA       (const char c);

void putss(const char *c);
#endif

