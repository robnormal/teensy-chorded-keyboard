#ifndef CHORDED_LIB
#define CHORDED_LIB

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

/** KEYBOARD-SPECIFIC CONFIGURATION **/

#define LAYOUT_SIZE 60
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

#define LEFT_HANDED TRUE

/** END **/



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

Snapshot copySnapshotA(const Snapshot s);

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
int      sendOutputIO  (const Output *oM, int modifier);

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
int           modifier_GLOBAL; // context modifier - can be left over from last chord release

/**
 * These are NOT defined in chorded_kbd_lib.c
 * You must include teensy_chorded_keyboard.pde or other
 * source file for these to work
 */
void handleOutOfMemory ();
void sendKeyIO         (const Key *k);
int  readPinIO         (int pin);
Key  *charToKeyA       (const char c);

void putss(const char *c);
#endif

