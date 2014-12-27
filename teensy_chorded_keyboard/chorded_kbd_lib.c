#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "chorded_kbd_lib.h"

const integer SWITCHES[NUM_FINGERS][2] = {
  { THUMB_L, THUMB_H   },
  { INDEX_L,  INDEX_H  }, 
  { MIDDLE_L, MIDDLE_H },
  { RING_L,   RING_H   },
  { PINKY_L,  PINKY_H  }
};
const integer NUM_SWITCHES[NUM_FINGERS] = {2, 2, 2, 2, 2}; 

// chordite has 2 switches per finger, plus pressing _both_, which counts separately, as does pressing _nothing_
const integer NUM_STATES[NUM_FINGERS] = {4, 4, 4, 4, 4}; 

/** END **/

void *myalloc(int size)
{
  void *x = malloc(size);

  if (NULL == x) {
    handleOutOfMemory();
  }

  return x;
}

// Never deleted - these live in LAYOUT for duration of program
Key *newKeyA(const int key, const integer modifier)
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

Snapshot copySnapshotA(const Snapshot s)
{
  Snapshot s_new = newSnapshotA();
  integer i;

  for (i = 0; i < NUM_FINGERS; ++i) {
    s_new[i] = s[i];
  }

  return s_new;
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
void discardHistorySnapshotsD(SwitchHistory *h, integer starting_at)
{
  integer i;

  for (i = starting_at; i < h->place; ++i) {
    deleteSnapshotD(h->snapshots[i]);
  }
}

Output *newOutputA(const integer num_keys)
{
  Output *o = MALLOC(Output);
  o->keys   = MALLOCS(Key *, num_keys);
  o->count  = 0;

  return o;
}

void deleteOutputD(Output *oM)
{
  if (NULL != oM) {
		integer i;
		for (i = 0; i < oM->count; i++) {
			free(oM->keys[i]);
		}

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
  integer i;
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
  integer i;
  int id = 0, multiplier = 1;

  // number-base style, this forms unique IDS using the smallest possible integers
  for (i = 0; i < NUM_FINGERS; ++i) {
    id += s[i] * multiplier;

    multiplier *= NUM_STATES[i];
  }

  return id;
}


// -N Snapshots
// CHANGES h
SwitchHistory *restartHistoryD(SwitchHistory *h, integer starting_at)
{
  // EXPLAIN ME: why do I have to NOT delete these snapshots?
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
        restartHistoryD(h, 1);
        addToHistory(sM, h);
      }
    } else if (isRelease(sM)) {
      if (h->place) {
        restartHistoryD(h, 0);
      }
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

integer chordIndex(const Snapshot s, const Layout *l)
{
  const int id = chordId(s);
  integer i;

  for (i = 0; i < l->count; ++i) {
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
      integer index = chordIndex(chordM, l);
      if (index != -1) {
        o = l->outputs[index];
      } else {
        o = NULL;
      }
    } else {
      o = NULL;
    }

    return o;

  } else {
    return NULL;
  }
}

ClockReturn *clockReturn(SwitchHistory *h, Output *oM)
{
  ClockReturn *cr = MALLOC(ClockReturn);

  cr->history  = h;
  cr->outputM  = oM;

  return cr;
}

boole justModifier(Key *k)
{
  return k->modifier && !k->key;
}

ClockReturn *clock(Snapshot currentM, SwitchHistory *history, Layout *l)
{
  int n;
/*  for (n = 0; n < NUM_FINGERS; ++n) {
    iDebugOut(currentM[n]);
  } strDebugOut("");*/
  
  Output        *oM    = outputForM(currentM, history, l);
  SwitchHistory *new_h = addToHistory(currentM, history);

  return clockReturn(new_h, oM);
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
  integer i;

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

boole chordStringIsShifted(const char *str)
{
  return str[0] == 'S' && str[1] == '-';
}

/* REQUIREMENT: str must be at least NUM_FINGERS chars long */
Snapshot stringToSnapshotMA(const char *str)
{
  integer i, j;
  Snapshot s = newSnapshotA();  // +1 Snapshot

  for (i = 0; i < NUM_FINGERS; ++i) {
    // reverse order of list if left-handed
    j = LEFT_HANDED ? NUM_FINGERS - 1 - i : i;

    s[j] = ctoi(str[i]);
  }

  return s;
}



integer ctoi(const char c)
{
  switch (c) {
  case '_':
    return 0;
    break;
  case '^':
    return 1;
    break;
  case 'v':
    return 2;
    break;
  case '%':
    return 3;
    break;
  default:
    return -1;
    break;
  }
}

// +N Keys
// +1 Output
Output *stringToOutputMA(const char *str)
{
  integer i, len = strlen(str);
  Output *o = newOutputA(len);

  for (i = 0; i < len; i++) {
    addToOutput(newKeyA(str[i], 0), o);
  }

  return o;
}

Layout *addToLayoutA(Snapshot s, Output *o, Layout *l)
{
	if (l->count == LAYOUT_SIZE) {
		// putss("Layout exceeds maximum size");
	} else {
		l->ids    [l->count] = chordId(s);
		l->chords [l->count] = s;
		l->outputs[l->count] = o;

		l->count++;
	}

  return l;
}

Layout *layoutAddKeyCode(const char *chordstr, const int k)
{
  return layoutAddKey(chordstr, newKeyA(k, 0));
}

Layout *layoutAddKey(const char *chordstr, Key *k)
{
  return layoutAddOutput(chordstr, addToOutput(k, newOutputA(1)));
}

Layout *layoutAddChar(const char *chordstr, const char c)
{
  return layoutAddKey(chordstr, charToKeyA(c));
}

Layout *layoutAddString(const char *chordstr, const char *str)
{
  return layoutAddOutput(chordstr, stringToOutputMA(str));
}

Layout *layoutAddMod(const char *chordstr, const char mod)
{
  return layoutAddKey(chordstr, newKeyA(0, mod));
}

Layout *layoutAddCharMod(const char *chordstr, const char c, const char mod)
{
  Key *k = charToKeyA(c);
  k->modifier = k->modifier | mod;

  return layoutAddKey(chordstr, k);
}

Layout *layoutAddOutput(const char *chordstr, Output *o)
{
  Snapshot s = stringToSnapshotMA(chordstr);

  if (NULL != s) {
    LAYOUT = addToLayoutA(s, o, LAYOUT);
    return LAYOUT;
  }
}

// +1 Snapshot
Snapshot readInputsAIO()
{
  integer i, j;
  FingerState state;

  Snapshot s = newSnapshotA();  // +1 Snapshot


  for (i = 0; i < NUM_FINGERS; ++i) {
    state = 0;

    for (j = 0; j < NUM_SWITCHES[i]; ++j) {
      // shift by 1, put new bit at end
      state = state*2 + readPinIO(SWITCHES[i][j]);      
    }

    s[i] = state;
  }

  return s;
}

integer sendOutputIO(const Output *oM, integer modifier)
{
  char *msg = "in sendOutputIO";
    
  if (oM && oM->count) {
//sstrDebugOut(msg);
    integer i, end = oM->count;
    Key *kM;

    for (i = 0; i < end; ++i) {
      if (justModifier(oM->keys[i])) {

        return modifier | oM->keys[i]->modifier;

      } else {
        kM = outputKeyA(oM->keys[i], modifier); // +1 Key

        if (kM) {
          sendKeyIO(kM);
          free(kM); // -1 Key

          return 0;
        } else {
          return modifier;
        }
      }
    }
  } else {
    return modifier;
  }
}

