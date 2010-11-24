std::vector<std::string> split(const std::string& str, const std::string& delimiters)
{
  // Skip delimiters at beginning.
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

  std::vector<std::string> tokens;
  while (std::string::npos != pos || std::string::npos != lastPos)
  {
    // Found a token, add it to the vector.
    tokens.push_back(str.substr(lastPos, pos - lastPos).c_str());
    // Skip delimiters.  Note the "not_of"
    lastPos = str.find_first_not_of(delimiters, pos);
    // Find next "non-delimiter"
    pos = str.find_first_of(delimiters, lastPos);
  }
  return tokens;
}

float s2f(const std::string& str)
{
  float f;
  sscanf(str.c_str(), "%f", &f);
  return f;
}

std::vector<float> s2f(const std::vector<std::string> strs)
{
  std::vector<float> fs;
  for (std::vector<std::string>::const_iterator i = strs.begin(); i != strs.end(); ++i)
  {
    fs.push_back(s2f((*i)));
  }
  return fs;
}
