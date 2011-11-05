require File.dirname(__FILE__) + '/chord_layout.rb'
include ChordLayout

layout = read_layout('chordlist.txt')
lines = layout.map do |output, chord|
  output_str = case output
    when 'Shift';     'MODIFIERKEY_SHIFT'
    when 'Ctrl';      'MODIFIERKEY_CTRL'
    when 'Win';       'MODIFIERKEY_GUI'
    when 'Alt';       'MODIFIERKEY_ALT'
    when 'Backspace'; 'KEY_BACKSPACE'
    when 'Esc';       'KEY_ESC'
    when 'Tab';       'KEY_TAB'
    when 'Up';        'KEY_UP'
    when 'Down';      'KEY_DOWN'
    when 'Left';      'KEY_LEFT'
    when 'Right';     'KEY_RIGHT'
    when 'PgUp';    'KEY_PAGE_UP'
    when 'PgDn';  'KEY_PAGE_DOWN'
    when 'Space';     "' '"
    when 'Return';    "'\\n'"
    when 'Enter';     "'\\n'"
    when "'";         "'\\''"
    when /^KEY/;      output
    else "'" + output + "'"
    end

  function_str = case output
    when *['Shift', 'Alt', 'Ctrl', 'Win']
      'layoutAddMod'
    when *['Backspace', 'Tab', 'Esc', /^KEY_/]
      'layoutAddKeyCode'
    else
      'layoutAddChar'
    end

  "#{function_str}( \"#{chord}\", #{output_str} );"

end

puts lines.join("\n")
puts "Layout size: #{lines.count}"

