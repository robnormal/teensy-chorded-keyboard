require 'erb'

def valid_chord? c_str
  c_str.match /^[_v^%]{4}$/
end

def add_chord c_str, str, l
  if valid_chord? c_str
    if str == 'Backspace' then str = 'Bksp' end
    l[str] = c_str
  end
end

def read_layout file
  layout = {}

  File.foreach file do |line|
    chord, output = line.split ' '
    if chord and output
      add_chord chord, output, layout
    end
  end

  layout
end

def output_group str
  if str.length > 1
    if ['Shift', 'Alt', 'Ctrl', 'Win'].include? str
      0
    else
      1
    end
  elsif str.match /[a-i]/ then 2
  elsif str.match /[j-r]/ then 3
  elsif str.match /[s-z]/ then 4
  elsif str.match /[0-9]/ then 5
  else 6
  end
end

layout = read_layout('chordlist.txt')
outputs = layout.keys.sort do |a, b|
  group_a = output_group a
  group_b = output_group b

  if group_a != group_b
    group_a <=> group_b
  else
    a <=> b
  end
end


cheatsheet = ERB.new(File.open('cheat.erb', 'rb').read)
cheatsheet.run

