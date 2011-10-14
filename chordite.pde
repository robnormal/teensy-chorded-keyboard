#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "chordite_lib.h"
#include "chordite_lib.c"

void putss(const char *c)
{
  Keyboard.println(c);
}

void handleOutOfMemory()
{
  Keyboard.println("out of memory");
}

void sendKeyIO(const Key *k)
{
  if (k->key != 0) {
    // press modifier
    if (k->modifier != 0) {
      Keyboard.set_modifier(k->modifier);
      Keyboard.send_now();
    }

    // press the key
    Keyboard.set_key1(k->key);
    Keyboard.send_now();

    // release the key
    Keyboard.set_modifier(0);
    Keyboard.set_key1(0);
    Keyboard.send_now();
  }

}

int readPinIO(int pin)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  pinMode(pin, INPUT);

  return digitalRead(pin);
}

Key *charToKeyA(const char c)
{
  // default modifier is 0
  int key, mod = 0;

  if ('a' <= c && c <= 'z') {
     key = c - ('a' - KEY_A);
  } else if ('0' <= c && c <= '9') {
    key = c - ('0' - KEY_0);
  } else {
    switch (c) {
    case ' ':
      key = KEY_SPACE;
      break;
    case '-':
      key = KEY_MINUS;
      break;
    case '=':
      key = KEY_EQUAL;
      break;
    case '\\':
      key = KEY_BACKSLASH;
      break;
    case ';':
      key = KEY_SEMICOLON;
      break;
    case '\'':
      key = KEY_QUOTE;
      break;
    case '\n':
      key = KEY_ENTER;
      break;
    case ',':
      key = KEY_COMMA;
      break;
    case '.':
      key = KEY_PERIOD;
      break;
    case '/':
      key = KEY_SLASH;
      break;
    case '[':
      key = KEY_LEFT_BRACE;
      break;
    case ']':
      key = KEY_RIGHT_BRACE;
      break;
    case '?':
      key = KEY_SLASH;
      mod = MODIFIERKEY_SHIFT;
      break;
    case '"':
      key = KEY_QUOTE;
      mod = MODIFIERKEY_SHIFT;
      break;
    case '`':
      key = KEY_TILDE;
      break;
    case '~':
      key = KEY_TILDE;
      mod = MODIFIERKEY_SHIFT;
      break;
    case '!':
      key = KEY_1;
      mod = MODIFIERKEY_SHIFT;
      break;
    case '@':
      key = KEY_2;
      mod = MODIFIERKEY_SHIFT;
      break;
    case '#':
      key = KEY_3;
      mod = MODIFIERKEY_SHIFT;
      break;
    case '$':
      key = KEY_4;
      mod = MODIFIERKEY_SHIFT;
      break;
    case '%':
      key = KEY_5;
      mod = MODIFIERKEY_SHIFT;
      break;
    case '^':
      key = KEY_6;
      mod = MODIFIERKEY_SHIFT;
      break;
    case '&':
      key = KEY_7;
      mod = MODIFIERKEY_SHIFT;
      break;
    case '*':
      key = KEY_8;
      mod = MODIFIERKEY_SHIFT;
      break;
    case '(':
      key = KEY_9;
      mod = MODIFIERKEY_SHIFT;
      break;
    case ')':
      key = KEY_0;
      mod = MODIFIERKEY_SHIFT;
      break;
    case '_':
      key = KEY_MINUS;
      mod = MODIFIERKEY_SHIFT;
      break;
    case '+':
      key = KEY_EQUAL;
      mod = MODIFIERKEY_SHIFT;
      break;
    case ':':
      key = KEY_SEMICOLON;
      mod = MODIFIERKEY_SHIFT;
      break;
    case '<':
      key = KEY_COMMA;
      mod = MODIFIERKEY_SHIFT;
      break;
    case '>':
      key = KEY_PERIOD;
      mod = MODIFIERKEY_SHIFT;
      break;
    case '{':
      key = KEY_LEFT_BRACE;
      mod = MODIFIERKEY_SHIFT;
      break;
    case '|':
      key = KEY_BACKSLASH;
      mod = MODIFIERKEY_SHIFT;
      break;
    case '}':
      key = KEY_RIGHT_BRACE;
      mod = MODIFIERKEY_SHIFT;
      break;
    default:
      key = c;
    }
  }

  return newKeyA(colemak(key), mod);
}

// converts output for computers using the Colemak layout instead of qwerty
int colemak(const int key) {
  switch (key) {
    // a-c are the same
    case KEY_D: return KEY_G;
    case KEY_E: return KEY_K;
    case KEY_F: return KEY_E;
    case KEY_G: return KEY_T;
    // h is the same
    case KEY_I: return KEY_L;
    case KEY_J: return KEY_Y;
    case KEY_K: return KEY_N;
    case KEY_L: return KEY_U;
    // m is the same
    case KEY_N: return KEY_J;
    case KEY_O: return KEY_SEMICOLON;
    case KEY_P: return KEY_R;
    // q is the same
    case KEY_R: return KEY_S;
    case KEY_S: return KEY_D;
    case KEY_T: return KEY_F;
    case KEY_U: return KEY_I;
    // v & w are the same
    case KEY_Y: return KEY_O;
    // z is the same

    case KEY_SEMICOLON: return KEY_P;

    default:  return key;
  }
}






/***** MAIN PROCESSES BELOW HERE *****/

void setupLayout()
{
  LAYOUT = newLayoutA();

  layoutAddMod("0220", MODIFIERKEY_SHIFT );
  layoutAddMod("0110", MODIFIERKEY_CTRL );
  layoutAddKeyCode("0022", KEY_BACKSPACE );

  layoutAddChar( "0100", ' ' );
  layoutAddChar( "1000", 'e' );
  layoutAddChar( "2000", 't' );
  layoutAddChar( "0200", 'a' );
  layoutAddChar( "3000", 'i' );
  layoutAddChar( "0300", 'o' );
  layoutAddChar( "0010", 'n' );
  layoutAddChar( "0020", 's' );
  layoutAddChar( "0002", 'h' );
  layoutAddChar( "0030", 'r' );
  layoutAddChar( "0001", 'l' );
  layoutAddChar( "0003", 'd' );
  layoutAddChar( "1100", 'c' );
  layoutAddChar( "3300", 'u' );
  layoutAddChar( "2200", 'm' );
  layoutAddChar( "2220", 'w' );
  layoutAddChar( "0330", 'g' );
  layoutAddChar( "0011", 'f' );
  layoutAddChar( "2222", 'y' );
  layoutAddChar( "3333", 'p' );
  layoutAddChar( "1110", 'b' );
  layoutAddChar( "3330", ',' );
  layoutAddChar( "0222", '.' );
  layoutAddChar( "0333", 'v' );
  layoutAddChar( "1111", 'k' );
  layoutAddChar( "0111", '\n' );
  layoutAddChar( "3003", '"' );
  layoutAddChar( "2002", '\'' );
  layoutAddChar( "3030", '-' );
  layoutAddChar( "0033", 'x' );
  layoutAddChar( "1001", 'j' );
  layoutAddChar( "2020", ';' );
  layoutAddChar( "3200", '(' );
  layoutAddChar( "1002", ')' );
  layoutAddChar( "3100", 'q' );
  layoutAddChar( "1011", '?' );
  layoutAddChar( "1010", 'z' );
  layoutAddChar( "3002", ':' );
}


void setup() {
  Serial.begin(9600);
  delay(1000);

  for (int i = 0; i < 13; i++) {
    pinMode(i, INPUT);
  }

/*
	for (int i = 44; i <= 56; i++) {
		Keyboard.print("Number: ");
		Keyboard.println(i);
		sendKeyIO(newKeyA(i, 0));
		Keyboard.println("");
	}
	*/

  // this allocation is never freed
  history_GLOBAL = newHistoryA();

  setupLayout();
}


void loop() {
  // get function input
  Snapshot current = readInputsAIO(); // +1 Snapshot - deleted in restartHistoryD
  SwitchHistory *h = history_GLOBAL;

  // call the pure function
  ClockReturn *r = clock(current, h, LAYOUT); // + 1 Output

  deleteSnapshotD(current);

  modifier_GLOBAL = sendOutputIO(r->outputM, modifier_GLOBAL);

  // set function output
  // THIS ALREADY HAPPENED - just here for reference
  history_GLOBAL = r->history;

  // cleanup

  // EXPLAIN ME: why do I have to NOT delete this output?
  // deleteOutputD(r->outputM); // -1 Output

  free(r);
}

