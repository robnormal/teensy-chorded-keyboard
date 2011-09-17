#include "chordite_lib.c"
#define TEST(x) int test_##x () { __current_test = #x; __assert_count = 0;
#define END_TEST return 0;}; 

char *__current_test;


int __assert_count = 0;
int __total_asserts = 0;
int __failures = 0;

void insist(int cond, char *msg)
{
  __assert_count++;
  __total_asserts++;

  if (!cond) {
    __failures++;
    printf("%s, assertion %i: %s\n", __current_test, __assert_count, msg);
  }
}

void refuse(int cond, char *msg)
{
	insist(! cond, msg);
}



Snapshot example0A()
{
  int i;

  Snapshot example = newSnapshotA();
	for (i = 0; i < NUM_FINGERS; i++) {
		example[i] = 0;
	}
  
  return example;
}


Snapshot example1A()
{
  int i;

  Snapshot example = newSnapshotA();
	for (i = 0; i < NUM_FINGERS; i++) {
		example[i] = i % 2; // just for variety, kinda
	}
  
  return example;
}

TEST(ctoi)
  insist(ctoi('5') == 5, "ctoi converts char to int");
END_TEST

TEST(newKey)
	Key *k = newKeyA(35, 1);

	insist(k->key == 35, "");
	insist(k->modifier == 1, "");

	free(k);
END_TEST

TEST(snapshot)
	Snapshot s = example0A();

	insist(isRelease(s), "");
		
	s[0] = 1;

	refuse(isRelease(s), "");

	s[0] = 0;
  insist(chordId(s) == 0, "");
  insist(chordId(example1A()) == 1*1 + 0*3 + 1*9 + 0*27, "");

	deleteSnapshotD(s);
END_TEST

TEST(history)
  int i;
	SwitchHistory *h = newHistoryA();

	insist(historyIsEmpty(h), "");
  insist(chordFromM(h) == NULL, "Empty history returns NULL for chordFromM");

  Snapshot s0 = example0A();
  s0[PINKY] = 1; // otherwise, s0 would be a "release"

  Snapshot s1 = example1A();

  h = addToHistory(s0, h);
  insist(h->place == 1, "addToHistory increments history.place");

  for (i = 1; i < MAX_SNAPSHOTS - 1; i++) {
    h = addToHistory(s1, h);
  }

  refuse(historyTooLong(h), "");
    
  h = addToHistory(s1, h);
  insist(historyTooLong(h), "");

  insist(chordId(chordFromM(h)) == chordId(s0), "chordFromM returns first chord in history");

  restartHistoryD(h, 2);
  insist(h->place == 2, "");

  restartHistoryD(h, 0);
  insist(h->place == 0, "");
  
  s0[PINKY] = 0;
  h = addToHistory(s1, h);
  h = addToHistory(s1, h);
  h = addToHistory(s0, h);

  insist(h->place == 0, "adding a 0 chord restarts history");

	deleteHistoryD(h);
	deleteSnapshotD(s0);
	deleteSnapshotD(s1);
END_TEST

TEST(output)
  Output *o = newOutputA();

  insist(o->count == 0, "");

  Key *k1 = newKeyA('Q', 1);
  o = addToOutput(k1, o);

  insist(o->count == 1, "addToOutput increments Output.count");
  insist(o->keys[0] != NULL && o->keys[0]->key == 'Q', "");

  int i;
  for (i = o->count; i < MAX_OUTPUT; i++) {
    o = addToOutput(k1, o);
  }

  int max_count = o->count;
  addToOutput(k1, o);

  insist(o->count == max_count, "Adding Keys beyond MAX_OUTPUT has no effect on Output");

  free(k1);
  deleteOutputD(o);
END_TEST

TEST(string2out)
	Output *o = stringToOutputMA("o");
	insist(o->count == 1, "");
	insist(o->keys[0]->key == 'o', "");
	insist(o->keys[0]->modifier == 0, "");
	deleteOutputD(o);

	Output *oo = stringToOutputMA("return");
	insist(oo->count == 1, "");
	insist(oo->keys[0]->key == KEY_ENTER, "");
	deleteOutputD(oo);

	Output *x = stringToOutputMA("hello");
	insist(x->count == 5, "");
	insist(x->keys[4]->key == 'o', "");
	deleteOutputD(x);
END_TEST

TEST(strToSnap)
	Snapshot s = default_stringToSnapshotMA("3010");
	insist(s[0] == 3, "");
	insist(s[1] == 0, "");
	insist(s[2] == 1, "");
	deleteSnapshotD(s);
END_TEST

TEST(layout)
	Layout *l = newLayoutA();
  l = loadLayoutStringsA("3012", "e", default_stringToSnapshotMA, l);
	insist(l->count == 1, "");
	insist(l->chords[0][0] == 3 && l->chords[0][3] == 2, "");
	insist(l->ids[0] == chordId(l->chords[0]), "Layout ids match chords");
	insist(l->outputs[0]->keys[0]->key == 'e', "Layout output matches config string");

  l = loadLayoutStringsA("1110", "return", default_stringToSnapshotMA, l);
	insist(l->outputs[1]->keys[0]->key == KEY_ENTER, "Special-named outputs are loaded into layout");

  l = loadLayoutStringsA("1010", "hello", default_stringToSnapshotMA, l);
	insist(l->outputs[2]->keys[4]->key == 'o', "Multi-character string outputs are loaded into layout");
	insist(l->count == 3, "");

	free(l);

	Layout *ll = loadLayoutA("3012 e\n1110 return\n1010 helLo\n", default_stringToSnapshotMA);
	insist(ll->count == 3, "");
	insist(ll->outputs[1]->keys[0]->key == KEY_ENTER, "");

	insist(ll->ids[0] == chordId(ll->chords[0]), "");
	insist(ll->ids[1] == chordId(ll->chords[1]), "");
	insist(ll->outputs[2]->keys[3]->key == 'L', "");
	free(ll);

END_TEST

int main(void) {
  test_ctoi();
  test_newKey();
	test_history();
	test_snapshot();
	test_output();
	test_string2out();
	test_strToSnap();
	test_layout();

  printf("%i passed, %i failed\n", __total_asserts - __failures, __failures);
}

