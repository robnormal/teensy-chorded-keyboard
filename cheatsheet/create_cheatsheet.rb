require 'erb'
require File.dirname(__FILE__) + '/chord_layout.rb'

include ChordLayout

def output_group str
  if str.length > 1
    if ['Up', 'Down', 'Left', 'Right'].include? str
      0
    elsif ['PgUp', 'PgDn'].include? str
      1
    elsif ['Shift', 'Alt', 'Ctrl', 'Win'].include? str
      2
    else
      3
    end
  else
    case str
    when /[a-i]/ then 4 
    when /[j-r]/ then 5
    when /[s-z]/ then 6
    when /[0-9]/ then 7
    when /["'$()]/ then 7
    else 8
    end
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

