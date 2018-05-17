#ifndef PADDLE_H__
#define PADDLE_H__

#include <ngl/Camera.h>
#include <ngl/Vec3.h>
#include <ngl/Vec2.h>

#include <ngl/Obj.h>
#include <ngl/Transformation.h>
#include <ngl/AbstractMesh.h>

const static float s_xExtents = 45;
const static float s_yExtents = 30;

class Paddle
{
  public :
    /// @brief ctor
    /// @param _pos the initial position
    /// @param _fname the name of mesh to load
    Paddle(ngl::Vec3 _pos, std::string _fname);
    ///@brief draw method
    void draw(const std::string &_shader, ngl::Camera *_cam );
    void move(float _x,	float _y	);
    void rotate(float _rot);
    inline GLfloat getRadius(){ return m_radius; }
    inline ngl::Vec3 getPos(){ return m_pos; }
    ngl::Vec2 getBBox() const { return ngl::Vec2(  m_mesh->getBBox().width(), m_mesh->getBBox().height() ); }

    private :
    /// @brief the position of the paddle
    ngl::Vec3 m_pos;
    /// @brief the paddle's mesh
    std::unique_ptr<ngl::Obj>m_mesh;
    /*! the radius of the Bounding Sphere */
    GLfloat m_radius;
    /// @brief paddle movement increment
    const static float s_increment;
    /// @brief a transform stack
    ngl::Transformation m_transform;
    /// @brief the rotation of the paddle
    float m_rotation;
  };

#endif
