#include <Bounce2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "chorded_kbd_lib.h"
#include "chorded_kbd_lib.c"

Bounce buttons[NUM_FINGERS * 2];

void handleOutOfMemory()
{
  Keyboard.println("out of memory");
}

void iDebugOut(int out)
{
  Keyboard.print(out);
  Serial.print(out);
}

void strDebugOut(char *out)
{
  Keyboard.println(out);
}

void debugout(String out)
{
    Keyboard.println(out);
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

integer readPinIO(integer pin)
{
  buttons[pin].update();
  
  integer s = buttons[pin].read();

  //Toggle if PULLUP
  return s!=PULLUP;
}

Key *charToKeyA(const char c)
{
  // default modifier is 0
  int key;
  integer mod = 0;

  if ('a' <= c && c <= 'z') {
     key = c - ('a' - KEY_A);
  } else {
    switch (c) {
    case '0':
      key = KEY_0;
      break;
    case '1':
      key = KEY_1;
      break;
    case '2':
      key = KEY_2;
      break;
    case '3':
      key = KEY_3;
      break;
    case '4':
      key = KEY_4;
      break;
    case '5':
      key = KEY_5;
      break;
    case '6':
      key = KEY_6;
      break;
    case '7':
      key = KEY_7;
      break;
    case '8':
      key = KEY_8;
      break;
    case '9':
      key = KEY_9;
      break;
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

  // return newKeyA(colemak(key), mod);
  return newKeyA(key, mod);
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

// +1 Key
Key *outputKeyA(const Key *k, const integer modifier)
{
  int key = k->key, outkey = key;
  integer outmod = k->modifier | modifier;

  if (modifier == MODIFIERKEY_SHIFT) {
    if (k->modifier == 0) {
      switch (key) {
      case KEY_0:
        outkey = KEY_5;
        outmod = 0;
        break;
      case KEY_1:
        outkey = KEY_6;
        outmod = 0;
        break;
      case KEY_2:
        outkey = KEY_7;
        outmod = 0;
        break;
      case KEY_3:
        outkey = KEY_8;
        outmod = 0;
        break;
      case KEY_4:
        outkey = KEY_9;
        outmod = 0;
        break;
      case KEY_COMMA:
        // ?
        outkey = KEY_SLASH;
        outmod = MODIFIERKEY_SHIFT;
        break;
      case KEY_PERIOD:
        // !
        outkey = KEY_1;
        outmod = MODIFIERKEY_SHIFT;
        break;
      case KEY_QUOTE:
        // `
        outkey = KEY_TILDE;
        outmod = 0;
        break;
      case KEY_SEMICOLON:
        // @
        outkey = KEY_2;
        outmod = MODIFIERKEY_SHIFT;
        break;
      case KEY_SLASH:
        outkey = KEY_BACKSLASH;
        outmod = 0;
        break;
      case KEY_BACKSPACE:
        outkey = KEY_DELETE;
        outmod = 0;
        break;
      case KEY_PAGE_UP:
        outkey = KEY_HOME;
        outmod = 0;
        break;
      case KEY_PAGE_DOWN:
        outkey = KEY_END;
        outmod = 0;
        break;
      }
    } else if (k->modifier == MODIFIERKEY_SHIFT) {
      switch (key) {

      // Shift + ( -> [
      case KEY_9:
        outkey = KEY_LEFT_BRACE;
        outmod = 0;
        break;

      // Shift + ) -> ]
      case KEY_0:
        outkey = KEY_RIGHT_BRACE;
        outmod = 0;
        break;

      // Shift + { -> <
      case KEY_LEFT_BRACE:
        outkey = KEY_COMMA;
        outmod = MODIFIERKEY_SHIFT;
        break;

      // Shift + } -> >
      case KEY_RIGHT_BRACE:
        outkey = KEY_PERIOD;
        outmod = MODIFIERKEY_SHIFT;
        break;

      // Shift + " -> &
      case KEY_QUOTE:
        outkey = KEY_7;
        outmod = MODIFIERKEY_SHIFT;
        break;

      // Shift + : -> |
      case KEY_SEMICOLON:
        outkey = KEY_BACKSLASH;
        outmod = MODIFIERKEY_SHIFT;
        break;

      // Shift + $ -> ~
      case KEY_4:
        outkey = KEY_TILDE;
        outmod = MODIFIERKEY_SHIFT;
        break;

      // Shift + * -> ^
      case KEY_8:
        outkey = KEY_6;
        outmod = MODIFIERKEY_SHIFT;
        break;
      }
    }
  }

  //return newKeyA(colemak(outkey), outmod);
  return newKeyA(outkey, outmod);
}





/***** MAIN PROCESSES BELOW HERE *****/

void setupLayoutUEB()
{
  LAYOUT = newLayoutA();

//  layoutAddKeyCode( "_^_%", KEY_UP );
//  layoutAddKeyCode( "_v_%", KEY_DOWN );
//  layoutAddKeyCode( "^__%", KEY_LEFT );
//  layoutAddKeyCode( "v__%", KEY_RIGHT );
//  layoutAddKeyCode( "^^_^", KEY_PAGE_UP );
//  layoutAddKeyCode( "vv_v", KEY_PAGE_DOWN );
//  layoutAddChar( "__^_", ' ' );
//  layoutAddKeyCode( "_^__", KEY_BACKSPACE );
//  layoutAddMod( "_vv_", MODIFIERKEY_SHIFT );
//  layoutAddMod( "_^^_", MODIFIERKEY_CTRL );
//  layoutAddMod( "_%%_", MODIFIERKEY_GUI );
//  layoutAddMod( "%%__", MODIFIERKEY_ALT );
//  layoutAddKeyCode( "_^_^", KEY_TAB );
//  layoutAddChar( "_%_%", '\n' );
//  layoutAddKeyCode( "vvvv", KEY_ESC );
//  layoutAddChar( "^__^", ',' );
//  layoutAddChar( "_^^^", '.' );
//  layoutAddChar( "%__%", '\'' );
//  layoutAddChar( "%__v", '"' );
//  layoutAddChar( "vvv_", '-' );
//  layoutAddChar( "_%_v", ';' );
//  layoutAddChar( "^^^_", '(' );
//  layoutAddChar( "^_^_", ')' );
//  layoutAddChar( "_^^v", '/' );
//  layoutAddChar( "^^_v", ':' );
//  layoutAddChar( "_vv%", '=' );
//  layoutAddChar( "%^__", '$' );
//  layoutAddChar( "^^_%", '*' );
//  layoutAddChar( "^_%_", '{' );
//  layoutAddChar( "v_%_", '}' );
//  layoutAddChar( "_^%_", '0' );
//  layoutAddChar( "v_v_", '1' );
//  layoutAddChar( "%_%_", '2' );
//  layoutAddChar( "%%%_", '3' );
//  layoutAddChar( "^^^%", '4' );

/*
  This assumes ten keys in a row corresponding to each finger.
    Layout  1v 1^ 2v 2^ 3v 3^ 4v 4^ 5v 5^
            _  _  _  _  _  _  _  _  _  _
    Finger  LP RT LR RI LM RM LI RR LT RP
    EUB     X  X  3  4  2  5  1  6  X  X
    button  1  2  3  4  5  6  7  8  9  10
*/
  layoutAddChar( "___v_", 'a' );
  layoutAddChar( "__vv_", 'b' );
  layoutAddChar( "_^_v_", 'c' );
  layoutAddChar( "_^^v_", 'd' );
  layoutAddChar( "__^v_", 'e' );
  layoutAddChar( "_^vv_", 'f' );
  layoutAddChar( "_^%v_", 'g' );
  layoutAddChar( "__%v_", 'h' );
  layoutAddChar( "_^v__", 'i' );
  layoutAddChar( "_^%__", 'j' );
  layoutAddChar( "_v_v_", 'k' );
  layoutAddChar( "_vvv_", 'l' );
  layoutAddChar( "_%_v_", 'm' );
  layoutAddChar( "_%^v_", 'n' );
  layoutAddChar( "_v^v_", 'o' );
  layoutAddChar( "_%vv_", 'p' );
  layoutAddChar( "_%%v_", 'q' );
  layoutAddChar( "_v%v_", 'r' );
  layoutAddChar( "_%v__", 's' );
  layoutAddChar( "_%%__", 't' );
  layoutAddChar( "_v_%_", 'u' );
  layoutAddChar( "_vv%_", 'v' );
  layoutAddChar( "_^%^_", 'w' );
  layoutAddChar( "_%_%_", 'x' );
  layoutAddChar( "_%^%_", 'y' );
  layoutAddChar( "_v^%_", 'z' );

}

void setupLayout()
{
  LAYOUT = newLayoutA();

  layoutAddKeyCode( "_^_%", KEY_UP );
  layoutAddKeyCode( "_v_%", KEY_DOWN );
  layoutAddKeyCode( "^__%", KEY_LEFT );
  layoutAddKeyCode( "v__%", KEY_RIGHT );
  layoutAddKeyCode( "^^_^", KEY_PAGE_UP );
  layoutAddKeyCode( "vv_v", KEY_PAGE_DOWN );
  layoutAddChar( "__^_", ' ' );
  layoutAddKeyCode( "_^__", KEY_BACKSPACE );
  layoutAddChar( "___^", 'e' );
  layoutAddChar( "___v", 't' );
  layoutAddChar( "__v_", 'a' );
  layoutAddChar( "___%", 'i' );
  layoutAddChar( "__%_", 'o' );
  layoutAddChar( "_v__", 'n' );
  layoutAddChar( "^___", 's' );
  layoutAddChar( "_%__", 'h' );
  layoutAddChar( "v___", 'r' );
  layoutAddChar( "%___", 'l' );
  layoutAddChar( "__^^", 'd' );
  layoutAddChar( "__vv", 'c' );
  layoutAddChar( "__^v", 'u' );
  layoutAddChar( "^^__", 'm' );
  layoutAddMod( "_vv_", MODIFIERKEY_SHIFT );
  layoutAddMod( "_^^_", MODIFIERKEY_CTRL );
  layoutAddMod( "_%%_", MODIFIERKEY_GUI );
  layoutAddMod( "%%__", MODIFIERKEY_ALT );
  layoutAddKeyCode( "_^_^", KEY_TAB );
  layoutAddChar( "__^%", 'w' );
  layoutAddChar( "_^_v", 'g' );
  layoutAddChar( "__%v", 'f' );
  layoutAddChar( "__%%", 'y' );
  layoutAddChar( "_v_v", 'p' );
  layoutAddChar( "v__v", 'b' );
  layoutAddChar( "^__^", ',' );
  layoutAddChar( "_^^^", '.' );
  layoutAddChar( "_vvv", 'v' );
  layoutAddChar( "_%_%", '\n' );
  layoutAddKeyCode( "vvvv", KEY_ESC );
  layoutAddChar( "^__v", 'k' );
  layoutAddChar( "%__%", '\'' );
  layoutAddChar( "%__v", '"' );
  layoutAddChar( "vvv_", '-' );
  layoutAddChar( "__v%", 'x' );
  layoutAddChar( "_%%%", 'j' );
  layoutAddChar( "_%_v", ';' );
  layoutAddChar( "^^^_", '(' );
  layoutAddChar( "^_^_", ')' );
  layoutAddChar( "^^^^", 'q' );
  layoutAddChar( "_^^v", '/' );
  layoutAddChar( "_^^%", 'z' );
  layoutAddChar( "^^_v", ':' );
  layoutAddChar( "_^%_", '0' );
  layoutAddChar( "v_v_", '1' );
  layoutAddChar( "%_%_", '2' );
  layoutAddChar( "%%%_", '3' );
  layoutAddChar( "^^^%", '4' );
  layoutAddChar( "_vv%", '=' );
  layoutAddChar( "%^__", '$' );
  layoutAddChar( "^^_%", '*' );
  layoutAddChar( "^_%_", '{' );
  layoutAddChar( "v_%_", '}' );
}

void setup() {
  Serial.begin(9600);
  delay(1000);


  //We assume Pin 6 (teensy++ LED) is available via a 1K pullup
  for (int i = 0; i < NUM_FINGERS * 2; i++) {
    buttons[i] = Bounce();
    buttons[i].attach(i);
    buttons[i].interval(5);
    pinMode(i, INPUT_PULLUP);
  }

  // this allocation is never freed
  history_GLOBAL = newHistoryA();

  setupLayoutUEB();
}


void loop() {
  // get function input
  Snapshot current = readInputsAIO(); // +1 Snapshot - deleted in restartHistoryD
  SwitchHistory *h = history_GLOBAL;

  // call the pure function
  ClockReturn *r = clock(current, h, LAYOUT); // + 1 Output

  deleteSnapshotD(current);

  if(r->outputM->count > 0)
  {
   // iDebugOut(r->outputM->count);
  }

  modifier_GLOBAL = sendOutputIO(r->outputM, modifier_GLOBAL);

  // set function output
  // THIS ALREADY HAPPENED - just here for reference
  history_GLOBAL = r->history;

  // cleanup

  // EXPLAIN ME: why do I have to NOT delete this output?
  // deleteOutputD(r->outputM); // -1 Output

  free(r);
}

