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

std::ostream& operator<< (std::ostream &os, Vertex &vertex)
{
  os << vertex.x << " " << vertex.y << " " << vertex.z;
  return os;
}

typedef std::vector<Vertex> VertexList;

class Geometry
{
  typedef std::vector<float> FloatList;

public:

  void load_element(TiXmlElement* element)
  {
    std::clog << vertices_.size() << " " << vertices_.empty() << std::endl;

    if (vertices_.empty())
    {
      std::clog << "loading vertices" << std::endl;
      
      TiXmlElement* polylist = element_
        ->FirstChild("mesh")
        ->FirstChild("polylist")
        ->ToElement();

      TiXmlElement* vertex_input = polylist
        ->FirstChild("input")
        ->ToElement();

      int vertex_offset = 0;
      vertex_input->QueryIntAttribute("offset", &vertex_offset);

      std::string indices_raw = polylist
        ->FirstChild("p")
        ->ToElement()
        ->GetText();

      FloatList indices;
      split(indices_raw, " ", indices);

      FloatList vertex_indices;

      int offset = 0;
      for(int i = 0; i < indices.size(); i++)
      {
        if (offset++ > 0)
        {
          offset = 0;
          continue;
        }

        vertex_indices.push_back(indices[i]);
      }

      std::string positions_raw = element_
        ->FirstChild("mesh")
        ->FirstChild("source")
        ->FirstChild("float_array")
        ->ToElement()
        ->GetText();

      FloatList positions;
      split(positions_raw, " ", positions);

      for(FloatList::iterator i = vertex_indices.begin(); i != vertex_indices.end(); ++i)
      {
        int position_index = (*i) * 3;

        Vertex vertex(positions[position_index], 
                      positions[position_index + 1], 
                      positions[position_index + 2],
                      true);

        vertices_.push_back(vertex);
      }

      std::clog << "loaded vertices" << std::endl;
    }
  }

  Geometry(TiXmlElement* element)
    : element_(element)
  {
    load_element(element);
  }

  VertexList vertices()
  {
    std::clog << vertices_.size() << std::endl;
    return vertices_;
  }

private:

  TiXmlElement* element_;
  VertexList vertices_;
};

typedef std::vector<Geometry> GeometryList;

#include "dae_file.hpp"

class Model
{
  typedef std::vector<float> FloatList;

public:

  Model(DAEFile* file)
    : file_(file)
    { };

  GLfloat* array(const VertexList& vec)
  {
    int j = 0;
    GLfloat* float_array = new GLfloat[vec.size() * 3];
    for (VertexList::const_iterator i = vec.begin(); i != vec.end(); ++i)
    {
      float_array[j++] = (*i).x;
      float_array[j++] = (*i).y;
      float_array[j++] = (*i).z;
    }
     
    return float_array;
  }

  void render()
  {
    GeometryList geometries = file_->geometries();

    for (GeometryList::iterator i = geometries.begin(); i != geometries.end(); ++i)
    {
      glPushMatrix();

      glEnableClientState(GL_VERTEX_ARRAY);

      std::clog << (*i).vertices().size() << std::endl;

      GLfloat* vertices = array((*i).vertices());
      glVertexPointer(3, GL_FLOAT, 0, vertices);
      glDrawArrays(GL_TRIANGLES, 0, (*i).vertices().size());

      delete[] vertices;
     
      glDisableClientState(GL_VERTEX_ARRAY);
    
      glPopMatrix();
    }
  }

public:

  DAEFile* file_;
};

DAEFile* dae = new DAEFile();
Model* model = new Model(dae);

void render()
{
  std::clog << "render" << std::endl;
  glClear(GL_COLOR_BUFFER_BIT);

  glRotatef(0.1, 0.0, 1.0, 0.0);
  
  model->render();

  glutSwapBuffers();

  glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{

}

void reshape(int w, int h)
{
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 2.0, 1000000.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt (10, 10, -10.00, 
             0.0, 0.0, 0.0, 
             0.0, 1.0, 0.0);         
}

void init()
{
  dae->load("/Users/nk/Desktop/box.dae");  

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
