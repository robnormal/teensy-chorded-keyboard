require 'erb'
require File.dirname(__FILE__) + '/chord_layout.rb'
include ChordLayout

def output_group str
  if str.length > 1
    if ['Shift', 'Alt', 'Ctrl', 'Win'].include? str
      0
    else
      1
    end
  else
    case str
    when /[a-i]/ then 2
    when /[j-r]/ then 3
    when /[s-z]/ then 4
    when /[0-9]/ then 5
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

