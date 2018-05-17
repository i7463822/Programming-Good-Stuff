#ifndef PLANE_H
#define PLANE_H

#include <ngl/Camera.h>
#include <ngl/Vec3.h>
#include <ngl/Obj.h>
#include <ngl/Transformation.h>

class Plane
{
  public :
    /// @brief ctor
    /// @param _pos the initial position
    /// @param _fname the name of mesh to load
    Plane(ngl::Vec3 _pos, std::string _fname);
    Plane();
    ~Plane();
   ///@brief draw method
   void draw(const std::string &_shader, ngl::Camera *_cam );

  private:
     /// @brief the position of the plane
     ngl::Vec3 m_pos;
     /// @brief the Plane's mesh
     std::unique_ptr<ngl::Obj>m_mesh;
     void loadMatricesToShader(ngl::Camera *_cam ) const;

};

#endif // PLANE_H
