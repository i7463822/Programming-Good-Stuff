///  @file Ball.cpp
///  @brief This module contains the constuctor, defualt constructor, draw function and the update position function of the ball.

#include "Ball.h"
#include <ngl/ShaderLib.h>

// Module Contents

// Constructor of the ball
Ball::Ball( ngl::Vec3 _pos , ngl::Vec3 _dir ,  GLfloat _rad , std::string _fname )
{
  m_mesh.reset( new ngl::Obj( _fname ));
  m_mesh->createVAO();
  m_pos = _pos;
  m_dir = _dir;
  m_radius =_rad;
  m_hit = false;
}

// Default constructor of the ball, collisions set to false
Ball::Ball()
{
  m_hit = false;
}


// Function for the ball to be draw (assigning shaders)
void Ball::draw( const std::string &_shader, const ngl::Mat4 &_globalMat,  ngl::Camera *_cam )const
{
  //It is clear when the ball collides with either the bounding box or the paddle as the ball changes into wireframe mode.
  if( m_hit )
  {
    glPolygonMode( GL_FRONT_AND_BACK , GL_LINE );
  }
  else
  {
    glPolygonMode( GL_FRONT_AND_BACK , GL_FILL );
  }

  ngl::ShaderLib *shader = ngl::ShaderLib::instance();
  (*shader)[_shader]->use();
  ngl::Transformation t;
  t.setPosition( m_pos );
  ngl::Mat4 MVP = _cam->getVPMatrix() * t.getMatrix();
  shader->setUniform( "MVP", MVP );
  //draw the ball mesh
  m_mesh->draw();
}


// Function for the ball update
void Ball::updatePos()
{
  // Store the last position
  m_lastPos = m_pos;
  // Update the current position
  m_pos += m_dir;
  // Get the next position
  m_nextPos = m_pos + m_dir;
  // Bool of the ball- it is set to false to allow the ball to move
  m_hit = false;
}







