#include "Paddle.h"
#include <ngl/ShaderLib.h>
#include <ngl/AbstractMesh.h>

Paddle::Paddle( ngl::Vec3 _pos, std::string _fname )
{

  m_pos = _pos;
  m_mesh.reset( new ngl::Obj(_fname) );
  m_mesh->createVAO();
  m_mesh->calcBoundingSphere();
  //  std::cout << ( m_mesh->getSphereRadius() ) << "<---------------------------------SPERE SIZE\n";1
  m_mesh->getSphereCenter();
}

void Paddle::draw(const std::string &_shader,  ngl::Camera *_cam )
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  (*shader)[_shader]->use();

  m_transform.setPosition( m_pos );
  ngl::Mat4 MVP= _cam->getVPMatrix() * m_transform.getMatrix();
  shader->setUniform( "MVP", MVP );

  m_mesh->draw();
  m_mesh->drawBBox();
}

void Paddle::move( float _x, float _y )
{
  float currentX = m_pos.m_x;
  float currentY = m_pos.m_y;
  m_pos.m_x += _x;
  m_pos.m_y += _y;

  if( m_pos.m_x <= - s_xExtents || m_pos.m_x >= s_xExtents )
  {
    m_pos.m_x = currentX;
  }

  if( m_pos.m_y <= -s_yExtents || m_pos.m_y >= s_yExtents )
  {
    m_pos.m_y = currentY;
  }
}


