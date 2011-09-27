#include "WProgram.h"

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

  return HIGH == digitalRead(pin) ? HIGH : LOW;
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

