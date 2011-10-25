require File.dirname(__FILE__) + '/chord_layout.rb'
include ChordLayout

layout = read_layout('chordlist.txt')
str = (layout.map do |output, chord|
  output_str = case output
    when 'Shift';     'MODIFIERKEY_SHIFT'
    when 'Ctrl';      'MODIFIERKEY_CTRL'
    when 'Win';       'MODIFIERKEY_GUI'
    when 'Alt';       'MODIFIERKEY_ALT'
    when 'Backspace'; 'KEY_BACKSPACE'
    when 'Escape';    'KEY_ESC'
    when 'Tab';       'KEY_TAB'
    when 'Space';     "' '"
    when 'Return';    "'\\n'"
    when 'Enter';     "'\\n'"
    when "'";         "'\\''"
    else "'" + output + "'"
    end

  function_str = case output
    when *['Shift', 'Alt', 'Ctrl', 'Win']
      'layoutAddMod'
    when *['Backspace', 'Tab', 'Esc']
      'layoutAddKeyCode'
    else
      'layoutAddChar'
    end

  "#{function_str}( \"#{chord}\", #{output_str} );"
end).join "\n"

puts str

