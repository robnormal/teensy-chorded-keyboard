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

int readPinIO(int pin)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  pinMode(pin, INPUT);

  return digitalRead(pin);
}

int charToCode(const char c)
{
  if ('a' <= c && c <= 'z') {
    return c - ('a' - KEY_A);
  } else if ('0' <= c && c <= '9') {
    return c - ('0' - KEY_0);
  } else {
    switch (c) {
    case ' ':
      return KEY_SPACE;
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





/***** MAIN PROCESSES BELOW HERE *****/

void setupLayout()
{
  LAYOUT = newLayoutA();

  layoutAddChar( "0200", ' ' );
  layoutAddChar( "2000", 'e' );
  layoutAddChar( "1000", 't' );
  layoutAddChar( "0100", 'a' );
  layoutAddChar( "3000", 'i' );
  layoutAddChar( "0300", 'o' );
  layoutAddChar( "0020", 'n' );
  layoutAddChar( "0030", 's' );
  layoutAddChar( "0003", 'h' );
  layoutAddChar( "0010", 'r' );
  layoutAddChar( "0001", 'l' );
  layoutAddChar( "0002", 'd' );
  layoutAddChar( "2200", 'c' );
  layoutAddChar( "3300", 'u' );
  layoutAddChar( "1100", 'm' );
  layoutAddChar( "1110", 'w' );
  layoutAddChar( "0011", 'g' );
  layoutAddChar( "3333", 'f' );
  layoutAddChar( "1111", 'y' );
  layoutAddChar( "0220", 'p' );
  layoutAddChar( "0022", 'b' );
  layoutAddChar( "2220", ',' );
  layoutAddChar( "3330", '.' );
  layoutAddChar( "0111", 'v' );
  layoutAddChar( "0333", 'k' );
  layoutAddChar( "2222", '\n' );
  layoutAddChar( "0222", '"' );
  layoutAddChar( "3003", '\'' );
  layoutAddChar( "1001", '-' );
  layoutAddChar( "3030", 'x' );
	layoutAddChar( "0033", 'j' );
  layoutAddChar( "2002", ';' );
  layoutAddChar( "1010", '(' );
  layoutAddChar( "3100", ')' );
  layoutAddChar( "2001", 'q' );
  layoutAddChar( "3200", '?' );

  layoutAddMod("0330", MODIFIERKEY_SHIFT );
  layoutAddMod("0110", MODIFIERKEY_CTRL );

}

SwitchHistory *history_GLOBAL;

Key *mine;
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

  sendOutputIO(r->outputM);

  // set function output
  // THIS ALREADY HAPPENED - just here for reference
  history_GLOBAL = r->history;

  // cleanup

  // EXPLAIN ME: why do I have to NOT delete this output?
  // deleteOutputD(r->outputM); // -1 Output

  free(r);
}

