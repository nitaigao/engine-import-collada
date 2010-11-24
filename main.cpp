#include <iostream>
#include <tinyxml.h>
#include <vector>
#include <string>

#include <gl.h>
#include <glu.h>
#include <glut.h>

#include "string_utils.hpp"

#include "vertex.hpp"
#include "geometry.hpp"

#include "dae_file.hpp"
#include "model.hpp"

ModelList models;

void render()
{
  glClear(GL_COLOR_BUFFER_BIT);

  glPushMatrix();

  static int angle = 0;
  angle++;

  glRotatef(angle, 0.0, 1.0, 0.0);

  for (ModelList::iterator i = models.begin(); i != models.end(); ++i)
  {
    (*i)->render();
  }

  glPopMatrix();

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
  gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 0.1, 1000000.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt (0, 0, -10.00, 
             0.0, 0.0, 0.0, 
             0.0, 1.0, 0.0);         
}

void init()
{
  DAEFile* face_dae = new DAEFile();
  face_dae->load("geometry/planet.dae");  
  Model* face = new Model(face_dae);
  models.push_back(face);

  glClearColor(1.00, 0.0, 0.0, 1.0);
  glShadeModel(GL_SMOOTH);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
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
