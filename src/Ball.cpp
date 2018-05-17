#include "Ball.h"
#include <ngl/ShaderLib.h>

Ball::Ball(ngl::Vec3 _pos, ngl::Vec3 _dir,  GLfloat _rad, std::string _fname)
 {

   m_mesh.reset( new ngl::Obj(_fname));
   m_mesh->createVAO();
   m_pos=_pos;
   m_dir=_dir;
   m_radius=_rad;
   m_hit=false;

 }

Ball::Ball()
{
  m_hit=false;
}

void Ball::draw(const std::string &_shader, const ngl::Mat4 &_globalMat,  ngl::Camera *_cam )const
{
  if(m_hit)
  {
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  }
  else
  {
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  }

  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  (*shader)[_shader]->use();
  ngl::Transformation t;
  t.setPosition( m_pos );
  ngl::Mat4 MVP= _cam->getVPMatrix() * t.getMatrix();
  shader->setUniform( "MVP", MVP );

  m_mesh->draw();

}

void Ball::set(ngl::Vec3 _pos, ngl::Vec3 _dir, GLfloat _rad)
{
  m_pos = _pos;
  m_dir = _dir;
  m_radius = _rad;
}


void Ball::updatePos()
{

    // store the last position
    m_lastPos = m_pos;
    // update the current position
    m_pos += m_dir;
    // get the next position
    m_nextPos = m_pos + m_dir;
    m_hit=false;

}







