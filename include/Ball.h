#ifndef BALL_H__
#define BALL_H__

#include <ngl/Camera.h>
#include <ngl/Vec3.h>
#include <ngl/Obj.h>
#include <ngl/Transformation.h>

class Ball
{
  public :
    /// @brief ctor
    /// @param _pos the initial position
    /// @param _fname the name of mesh to load


  Ball(ngl::Vec3 _pos,  ngl::Vec3 _dir,	GLfloat _rad, std::string _fname);
  Ball();
  /// draw method
  void draw(const std::string &_shaderName,const ngl::Mat4 &_globalMat, ngl::Camera *_cam )const ;
  void loadMatricesToShader(ngl::Transformation &_tx, const ngl::Mat4 &_globalMat, ngl::Camera *_cam )const;
  inline void reverse(){m_dir=m_dir*-1.0f;}
  //void reflect( const ngl::Vec3 _normal ); //{ m_dir = m_dir.reflect( _border ); }
  inline void setHit(){m_hit=true;}
  inline void setNotHit(){m_hit=false;}
  inline bool isHit()const {return m_hit;}
  inline ngl::Vec3 getPos() const {return m_pos;}
  inline ngl::Vec3 getNextPos() const {return m_nextPos;}
  inline GLfloat getRadius() const { return m_radius; }
  inline void setDirection(ngl::Vec3 _d){m_dir=_d;}
  inline ngl::Vec3 getDirection() const { return m_dir;}


  void updatePos();
  /// set the sphere values
  /// @param[in] _pos the position to set
  /// @param[in] _dir the direction of the sphere
  /// @param[in] _rad the radius of the sphere

  void set(ngl::Vec3 _pos, ngl::Vec3 _dir, GLfloat _rad );

  private :
    /// @brief the position of the ball
    ngl::Vec3 m_pos;

    /*! the radius of the sphere */
    GLfloat m_radius;
    /*! flag to indicate if the sphere has been hit by ray */
    bool m_hit;
    // the direction of the sphere
    ngl::Vec3 m_dir;
    // the last position of the sphere
    ngl::Vec3 m_lastPos;
    // the next position of the sphere
    ngl::Vec3 m_nextPos;
    ngl::Vec3 m_colour;
    //ngl::Vec3 reflect( const ngl::Vec3 _border );

    /// @brief the ball's mesh
    std::unique_ptr<ngl::Obj>m_mesh;
    /// @brief ball movement increment
    const static float s_increment;
    /// @brief a transform stack
    ngl::Transformation m_transform;
  };

#endif
