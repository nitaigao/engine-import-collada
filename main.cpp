#include <iostream>
#include <tinyxml.h>
#include <vector>
#include <string>

#include <gl.h>
#include <glu.h>
#include <glut.h>

void split(const std::string& str, const std::string& delimiters , std::vector<float>& tokens)
{
  // Skip delimiters at beginning.
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

  while (std::string::npos != pos || std::string::npos != lastPos)
  {
    // Found a token, add it to the vector.
    float value = 0;
    sscanf(str.substr(lastPos, pos - lastPos).c_str(), "%f", &value);
    tokens.push_back(value);
    // Skip delimiters.  Note the "not_of"
    lastPos = str.find_first_not_of(delimiters, pos);
    // Find next "non-delimiter"
    pos = str.find_first_of(delimiters, lastPos);
  }
}

class Vertex
{

public:

  Vertex(float x_, float y_, float z_, bool z_up)
    : x(x_)
    , y(z_up ? z_ : y_)
    , z(z_up ? y_ : z_)
    { }

  float x, y, z;
};

typedef std::vector<Vertex> VertexList;

class Geometry
{

public:

  Geometry(TiXmlElement* element)
    : element_(element)
  {
  }

  std::vector<float> vertices()
  {
    TiXmlElement* polylist = element_
      ->FirstChild("mesh")
      ->FirstChild("polylist")
      ->ToElement();


    TiXmlElement* vertex_input = polylist
      ->FirstChild("input")
      ->ToElement();

    int vertex_offset = 0;
    vertex_input->QueryIntAttribute("offset", &vertex_offset);
    
    TiXmlElement* normal_input = vertex_input
      ->NextSiblingElement("input")
      ->ToElement();
    
    int normal_offset = 0;
    normal_input->QueryIntAttribute("offset", &normal_offset);

    std::string indices_raw = polylist
      ->FirstChild("p")
      ->ToElement()
      ->GetText();

    std::vector<float> indices;
    split(indices_raw, " ", indices);

    std::vector<float> vertex_indices;

    int offset = 0;
    for(int i = 0; i < indices.size(); i++)
    {
      if (offset == normal_offset)
      {
        offset = 0;
        continue;
      }

      offset += 1;
      vertex_indices.push_back(indices[i]);
    }

    std::string positions_raw = element_
      ->FirstChild("mesh")
      ->FirstChild("source")
      ->FirstChild("float_array")
      ->ToElement()
      ->GetText();

    std::vector<float> positions;
    split(positions_raw, " ", positions);

    std::vector<float> vertices;

    for(int i = 0; i < vertex_indices.size(); ++i)
    {
      vertices.push_back(positions[i]);
      vertices.push_back(positions[i+1]);
      vertices.push_back(positions[i+2]);
    }

    return vertices;
  }

private:

  TiXmlElement* element_;

};

class DAEFile
{

public:

  DAEFile() { };

  void load(const std::string& path)
  {
    bool loadOkay = doc_.LoadFile(path.c_str());

    if (loadOkay)
    {
      deserialize(doc_.RootElement()->FirstChild());
    }
    else
    {
      std::clog << "Failed to load file" << std::endl;
    }    
  }

  VertexList vertices()
  {
    VertexList vertices;
    std::vector<float> points = deserialize(doc_.RootElement()->FirstChild());

    for(int i = 0; i < points.size();)
    {
      Vertex vertex(points[i++], points[i++], points[i++], true);
      vertices.push_back(vertex);
    }

    return vertices;
  }

  std::vector<float> deserialize(TiXmlNode* element)
  {
    if (strcmp(element->Value(), "library_geometries") == 0)
    {
      return deserialize(element->FirstChild());
    }

    if (strcmp(element->Value(), "geometry") == 0)
    {
      return Geometry(element->ToElement()).vertices();
    }

    TiXmlNode* sibling = element->NextSibling();

    if (sibling)
    {
      return deserialize(sibling);
    }

    return std::vector<float>();
  }


private:
  
  TiXmlDocument doc_;

};

class Model
{

public:

  Model(DAEFile& file)
    : vertices_(file.vertices())
    { }

  float* vertice_array()
  {
    float* vertices = new float[vertices_.size()*3];

    int j = 0;

    for(VertexList::iterator i = vertices_.begin(); i != vertices_.end(); ++i)
    {
      vertices[j++] = (*i).x;
      vertices[j++] = (*i).y;
      vertices[j++] = (*i).z;
    }

    return vertices;
  }

  void render()
  {
    glPushMatrix();

    glEnableClientState(GL_VERTEX_ARRAY);

    GLfloat* vertices = vertice_array();

    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawArrays(GL_POLYGON, 0, vertices_.size());

    glDisableClientState(GL_VERTEX_ARRAY);

    delete[] vertices;

    glPopMatrix();
  }

public:

  VertexList vertices_;
  
};

DAEFile dae;
Model* model = 0;

void render()
{
  glClear(GL_COLOR_BUFFER_BIT);

  model->render();

  glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{

}

void reshape(int w, int h)
{
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 1000000.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt (10, 10, -10.00, 
             0.0, 0.0, 0.0, 
             0.0, 1.0, 0.0);         
}

void init()
{
  dae.load("/Users/nk/Desktop/box.dae");  
  model = new Model(dae);

  glClearColor(0.00, 0.0, 0.0, 1.0);
  glShadeModel(GL_SMOOTH);
}

int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_MULTISAMPLE | GLUT_DEPTH);
  glutInitWindowSize(800, 600);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Import");

  init();

  glutDisplayFunc(render);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);

  glutMainLoop();

  return 0;
}
