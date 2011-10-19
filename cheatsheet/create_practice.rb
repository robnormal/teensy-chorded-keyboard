def read_outputs file
  outputs = []

  File.foreach file do |line|
    chord, output = line.split ' '
    if chord and output
      outputs.push output
    end
  end

  outputs
end

outputs = read_outputs 'chordlist.txt'

puts outputs.sample(outputs.length).join '|'

