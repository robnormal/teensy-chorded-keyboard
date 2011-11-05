module ChordLayout
  def valid_chord? c_str
    c_str.match /^(S-)?[_v^%]{4}$/
  end

  def add_chord c_str, str, l
    if valid_chord? c_str
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
end

