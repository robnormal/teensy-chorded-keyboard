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

  layoutAddChar( "0200", KEY_SPACE );
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

  layoutAddMod("0002", MODIFIERKEY_CTRL );
                              
  layoutAddChar( "2200", 'd' );
  layoutAddChar( "3300", 'c' );
  layoutAddChar( "1100", 'u' );
  layoutAddChar( "0110", 'm' );

  layoutAddMod("0330", MODIFIERKEY_SHIFT );

  layoutAddChar( "1110", 'w' );

}

SwitchHistory *history_GLOBAL;

Key *mine;
void setup() {
  Serial.begin(9600);
  delay(1000);

  Keyboard.println("setup");
  for (int i = 0; i < 13; i++) {
    pinMode(i, INPUT);
  }

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

  delay(100);
}

