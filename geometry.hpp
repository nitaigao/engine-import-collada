class Geometry
{
  typedef std::vector<std::string> StringList;
  typedef std::vector<float> FloatList;

public:
  
  typedef std::vector<Geometry> GeometryList;

  Geometry(TiXmlElement* element)
    : element_(element)
  {
  }

  Vertex::VertexList normals()
  {
    if (normals_.empty())
    {
      load_element(element_);
    }

    return normals_;
  }

  Vertex::VertexList vertices()
  {
    if (vertices_.empty())
    {
      load_element(element_);
    }

    return vertices_;
  }

private:

  FloatList parse_indices()
  {
    int vertex_offset = 0;

    TiXmlNode* node = element_
      ->FirstChild("mesh")
      ->FirstChild("polylist");

    node
      ->FirstChild("input")
      ->ToElement()
      ->QueryIntAttribute("offset", &vertex_offset);

    std::string indices_text = node
      ->FirstChild("p")
      ->ToElement()
      ->GetText();
      
    StringList indices_raw = split(indices_text, " ");
    return s2f(indices_raw);      
  }

  FloatList parse_vertex_positions()
  {
    std::string positions_text = element_
      ->FirstChild("mesh")
      ->FirstChild("source")
      ->FirstChild("float_array")
      ->ToElement()
      ->GetText();

    StringList positions_raw = split(positions_text, " ");
    return s2f(positions_raw); 
  }

  FloatList parse_vertex_normals()
  {
    std::string positions_text = element_
      ->FirstChild("mesh")
      ->FirstChild("source")
      ->NextSibling("source")
      ->FirstChild("float_array")
      ->ToElement()
      ->GetText();

    StringList positions_raw = split(positions_text, " ");
    return s2f(positions_raw); 
  }

  void load_element(TiXmlElement* element)
  {
    FloatList indices = parse_indices();
    
    FloatList vertex_indices;
    FloatList normal_indices;

    for(int i = 0, offset = 0; i < indices.size(); i++)
    {
      if (offset == 0)
      {
        vertex_indices.push_back(indices[i]);
      }
      else if (offset == 1)
      {
        normal_indices.push_back(indices[i]);
        offset = 0; continue;
      }
      offset++;
    }

    FloatList vertex_positions = parse_vertex_positions();      
      
    for(FloatList::iterator i = vertex_indices.begin(); i != vertex_indices.end(); ++i)
    {
      int position_index = (*i) * 3;

      Vertex vertex(vertex_positions[position_index], 
                    vertex_positions[position_index + 1], 
                    vertex_positions[position_index + 2],
                    true);

      vertices_.push_back(vertex);
    }

    FloatList vertex_normals = parse_vertex_normals();     

    for(FloatList::iterator i = normal_indices.begin(); i != normal_indices.end(); ++i)
    {
      int normal_index = (*i) * 3;

      Vertex normal(vertex_normals[normal_index], 
                    vertex_normals[normal_index + 1], 
                    vertex_normals[normal_index + 2],
                    true);

      normals_.push_back(normal);
    }

  }

private:

  TiXmlElement* element_;
  Vertex::VertexList vertices_;
  Vertex::VertexList normals_;
};
