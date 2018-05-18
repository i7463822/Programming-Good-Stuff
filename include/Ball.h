/// \@file Ball.h
/// \@brief this class contains the ball parameters, getters and setters
/// \@author Dominique Claire Matthew
/// \@version 7
/// \@date 18/05/18

#ifndef BALL_H__
#define BALL_H__




#include <ngl/Camera.h>
#include <ngl/Vec3.h>
#include <ngl/Obj.h>
#include <ngl/Transformation.h>

class Ball
{
  public :
    /// @param _pos the initial position
    /// @param _fname the name of mesh to load

    ///Ball constructor
    Ball(ngl::Vec3 _pos ,  ngl::Vec3 _dir ,	GLfloat _rad , std::string _fname );
    ///Ball default constructor
    Ball();
    /// Draw method
    void draw( const std::string &_shaderName , const ngl::Mat4 &_globalMat , ngl::Camera *_cam )const ;
    /// Loading the matrices to the shader
    void loadMatricesToShader( ngl::Transformation &_tx , const ngl::Mat4 &_globalMat , ngl::Camera *_cam )const;
    /// Access function for when the ball hits either a paddle or the wall- checks if the collision is true
    inline void setHit(){ m_hit = true ;}
    /// Access functions for the position of the ball
    inline ngl::Vec3 getPos() const { return m_pos ;}
    inline ngl::Vec3 getNextPos() const { return m_nextPos ;}
    /// Access function for the radius
    inline GLfloat getRadius() const { return m_radius ; }
    /// Access functions (getter and setter) for the direction of the ball
    inline ngl::Vec3 getDirection() const { return m_dir ;}
    inline void setDirection(ngl::Vec3 _d ){ m_dir = _d ;}
    /// Update the position
    void updatePos();

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
    /// @brief the ball's mesh
    std::unique_ptr<ngl::Obj>m_mesh;
    /// @brief a transform stack
    ngl::Transformation m_transform;
  };

#endif
