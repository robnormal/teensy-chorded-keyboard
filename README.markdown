# Teensy Chorded Keyboard

teensy-chorded-keyboard provides all the necessary code to use a 
[Teensy board](http://www.pjrc.com/teensy/) as a chorded keyboard 
via USB. This code project was inspired by the [Chordite](http://chordite.com/)
keyboard, though it is not necessarily specifc to it.

## Installation

* Install the [Arduino IDE](http://arduino.cc/en/Main/Software), if you have not already.
* Install the [Teensyduino](http://www.pjrc.com/teensy/td_download.html) add-on.
* Open the included sketch:
  arduino teensy\_chorded\_keyboard/teensy\_chorded\_keyboard.pde
* Then just compile and upload the program to your Teensy board.

## Configuration

For the moment, the keyboard can only be configured by editting the source
files directly. 

### Configuring for a Chordite keyboard
* The constants PINKY\_L, PINKY\_H, etc. define the pin numbers for the low pinky switch,
  the high pinky switch, and so on. You will need to adjust these to match your wiring.
*	If LEFT\_HANDED is set to true, chord definitions will be expected to start with the 
  pinky finger on the left, up to the index finger. If LEFT\_HANDED is false, the order is
  reversed.
* To change the layout itself, find the function setupLayout() in teensy\_chorded\_keyboard.pde.
  Chord strings for each finger are defined as follows:
    - Underscore(\_) = no switch
    - Caret(^) = high switch
    - v = low switch
    - Percent(%) = both switches
* The available functions are
    - layoutAddChar(chord, char) - for an ASCII character
    - layoutAddMod(chord, modifier) - for a modifier (MODIFIERKEY\_CTRL, MODIFIERKEY\_SHIFT, etc)
    - layoutAddCharMod(chord, char, modifier) - for an ASCII character with a modifier 
    - layoutAddKeyCode(chord, keycode) - for use with USB keycodes. See the list in 
      <arduino>/hardware/teensy/cores/teensy/usb\_common.h
* LAYOUT\_SIZE will need to be adjusted to be at least equal to the total number of chords in your layout.

### Configuring other keyboards
If you are using a different design, you may also need to configure the following:

* In chorded\_kbd\_lib.c, SWITCHES, NUM\_SWITCHES, and NUM\_STATES configure,
  respectively, the IDs of the switches on each finger, the number of switches
	on each finger, and the number of possible states for each finger. For example, a
	Chordite would have 2 switches for each finger, and 4 states (no switch pressed,
	switch A, switch B, and both).
* The pins for each switch will also need to be defined at the top of chorded\_kbd\_lib.h

## Attribution

Maintainer: [Rob Rosenbaum](http://robrosenbaum.com)

