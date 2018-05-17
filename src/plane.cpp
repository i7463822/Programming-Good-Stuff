#include "plane.h"
#include <ngl/ShaderLib.h>

void Plane::draw(const std::string &_shader, ngl::Camera *_cam)
{

  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  (*shader)[_shader]->use();
  ngl::Transformation t;
  t.setPosition( m_pos );
  ngl::Mat4 MVP= _cam->getVPMatrix() * t.getMatrix();
  shader->setUniform( "MVP", MVP );

  m_mesh->draw();

}
