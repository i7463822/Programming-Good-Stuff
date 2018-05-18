///  @file Paddle.cpp
///  @brief This module contains the constuctor, draw function and the move function of the paddle.

#include "Paddle.h"
#include <ngl/ShaderLib.h>
#include <ngl/AbstractMesh.h>


// Module Contents
// Constructor of the paddle
Paddle::Paddle( ngl::Vec3 _pos , std::string _fname )
{
  // Contains members of the class 'Paddle'
  m_pos = _pos;
  m_mesh.reset( new ngl::Obj( _fname ) );
  m_mesh->createVAO();
  m_mesh->calcBoundingSphere();
  m_mesh->getSphereCenter();
}

// Function to draw the paddle (assigning shaders)
void Paddle::draw( const std::string &_shader ,  ngl::Camera *_cam )
{
  ngl::ShaderLib *shader = ngl::ShaderLib::instance();
  (*shader)[_shader]->use();

  m_transform.setPosition( m_pos );
  ngl::Mat4 MVP = _cam->getVPMatrix() * m_transform.getMatrix();
  shader->setUniform( "MVP", MVP );
  //draw the paddle mesh and draw the bounding box around the paddle
  m_mesh->draw();
  m_mesh->drawBBox();
}

// Function to caluclate the movement of the paddle
void Paddle::move( float _x, float _y )
{
  float currentX = m_pos.m_x;
  float currentY = m_pos.m_y;
  m_pos.m_x += _x;
  m_pos.m_y += _y;

  if( m_pos.m_x <= -s_xExtents || m_pos.m_x >= s_xExtents )
  {
    m_pos.m_x = currentX;
  }

  if( m_pos.m_y <= -s_yExtents || m_pos.m_y >= s_yExtents )
  {
    m_pos.m_y = currentY;
  }
}


