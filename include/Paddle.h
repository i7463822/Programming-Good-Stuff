/// \@file Paddle.h
/// \@brief this class contains the paddle parameters, getters and setters
/// \@author Dominique Claire Matthew
/// \@version 7
/// \@date 18/05/18

#ifndef PADDLE_H__
#define PADDLE_H__

#include <ngl/Camera.h>
#include <ngl/Vec3.h>
#include <ngl/Vec2.h>

#include <ngl/Obj.h>
#include <ngl/Transformation.h>
#include <ngl/AbstractMesh.h>

//These const static floats determines the limits of which the paddle can move along the screen from it's
//initialising position i.e. 30 in both directions of the x and y axis.
const static float s_xExtents = 30;
const static float s_yExtents = 30;

class Paddle
{
  public :
    /// @brief ctor
    /// @param _pos the initial position
    /// @param _fname the name of mesh to load
    Paddle( ngl::Vec3 _pos , std::string _fname );
    ///@brief Draw method
    void draw( const std::string &_shader, ngl::Camera *_cam );
    /// Move method
    void move( float _x ,	float _y	);
    /// Access functions for the position of the position of the paddle
    inline ngl::Vec3 getPos(){ return m_pos; }
    /// Bounding box created around the paddle
    ngl::Vec2 getBBox() const { return ngl::Vec2(  m_mesh->getBBox().width() , m_mesh->getBBox().height() ); }

    private :
    /// @brief the position of the paddle
    ngl::Vec3 m_pos;
    /// @brief the paddle's mesh
    std::unique_ptr<ngl::Obj>m_mesh;
    /// @brief a transform stack
    ngl::Transformation m_transform;
    /// @brief the score chart
  };

#endif
