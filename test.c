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



TEST(ctoi)
  insist(ctoi('5') == 5, "ctoi converts char to int");
END_TEST

TEST(newKey)
	Key *k = newKeyA(35, 1);

	insist(k->key == 35, "");
	insist(k->modifier == 1, "");

	free(k);
END_TEST

TEST(history)
	SwitchHistory *h = newHistoryA();

	insist(historyIsEmpty(h), "");

	deleteHistoryD(h);
END_TEST

TEST(snapshot)
	Snapshot s  = newSnapshotA();

	int i;
	for (i = 0; i < NUM_FINGERS; i++) {
		s[i] = 0;
	}

	insist(isRelease(s), "");
		
	s[0] = 1;

	refuse(isRelease(s), "");

	deleteSnapshotD(s);
END_TEST

int main(void) {
  test_ctoi();
  test_newKey();
	test_history();
	test_snapshot();

  printf("%i passed, %i failed\n", __total_asserts - __failures, __failures);
}

