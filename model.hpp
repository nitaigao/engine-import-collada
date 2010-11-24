class Model
{
  typedef std::vector<float> FloatList;

public:

  Model(DAEFile* file)
    : file_(file)
    { };

  GLfloat* array(const Vertex::VertexList& vec)
  {
    int j = 0;
    GLfloat* float_array = new GLfloat[vec.size() * 3];
    for (Vertex::VertexList::const_iterator i = vec.begin(); i != vec.end(); ++i)
    {
      float_array[j++] = (*i).x;
      float_array[j++] = (*i).y;
      float_array[j++] = (*i).z;
    }
     
    return float_array;
  }

  void render()
  {
    Geometry::GeometryList geometries = file_->geometries();

    for (Geometry::GeometryList::iterator i = geometries.begin(); i != geometries.end(); ++i)
    {
      glPushMatrix();

      glEnableClientState(GL_VERTEX_ARRAY);
      glEnableClientState(GL_NORMAL_ARRAY);

      GLfloat* vertices = array((*i).vertices());
      Vertex::VertexList verts = (*i).vertices();
      glVertexPointer(3, GL_FLOAT, 0, vertices);
      
      GLfloat* normals = array((*i).normals());
      Vertex::VertexList norms = (*i).normals();
      glNormalPointer(GL_FLOAT, 0, normals);

      glDrawArrays(GL_TRIANGLES, 0, (*i).vertices().size());

      delete[] vertices;
      delete[] normals;

      glDisableClientState(GL_NORMAL_ARRAY);     
      glDisableClientState(GL_VERTEX_ARRAY);
    
      glPopMatrix();
    }
  }

public:

  DAEFile* file_;
};

typedef std::vector<Model*> ModelList;
