class Vertex
{

public:

  typedef std::vector<Vertex> VertexList;

  Vertex(float x_, float y_, float z_, bool z_up)
    : x(x_)
    , y(z_up ? z_ : y_)
    , z(z_up ? y_ : z_)
    { 
    }


  float x, y, z;
};

std::ostream& operator<< (std::ostream &os, Vertex &vertex)
{
  os << vertex.x << " " << vertex.y << " " << vertex.z;
  return os;
}
