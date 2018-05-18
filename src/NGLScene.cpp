///  @file NGLScene.cpp
///  @brief This module

#include <QMouseEvent>
#include <QGuiApplication>
#include <QApplication>
#include <QLabel>

#include "NGLScene.h"
#include <ngl/Camera.h>
#include <ngl/Light.h>
#include <ngl/Transformation.h>
#include <ngl/Material.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <ngl/AbstractMesh.h>
#include <QImage>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <thread>
#include <chrono>

//Module Contents
constexpr static float s_paddleUpdate = 0.2;


NGLScene::NGLScene()
{
  // Where the title of the window is set, as well as the starting scores of Player 1 and 2
  setTitle( "P O N G" );
  m_scoreA = 0;
  m_scoreB = 0;
}


NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}


void NGLScene::timer()
{

  /// The following section is from : Cplusplus.com
  /// @cplusplus.com (2000-2017). SLEEP_FOR - C++ REFERENCE [online]. [Accessed 2018].
  /// Available from: "http://www.cplusplus.com/reference/thread/this_thread/sleep_for/".

  for(int i = 3; i > 0; --i)
  {
    std::cout<< i << "\n";
    std::this_thread::sleep_for (std::chrono::seconds( 1 ));
  }
  /// end of Citation

}


void NGLScene::reset()
{
    // Reset the paddles and ball to the original positions
    // Timer is called in between resets to create a delay before the next round is started.
    // The delay is of 3 seconds
    timer();

    m_paddleA.reset( new Paddle( ngl::Vec3( 0, 19, 0 ), "models/paddle3.obj"));
    m_paddleB.reset( new Paddle( ngl::Vec3( 0, -19, 0 ), "models/paddle3.obj"));
    m_ball.reset( new Ball( ngl::Vec3( 0, 0, 0 ), ngl::Vec3( 0.04f, 0.04f, 0.0f), 2.0f, "models/ball.obj"));
}


void NGLScene::resizeGL( int _w, int _h )
{
  m_cam.setShape( 45.0f, static_cast<float>( _w ) / _h, 0.05f, 350.0f );
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );
}


void NGLScene::initializeGL()
{
  // we must call this first before any other GL commands to load and link the
  // gl commands from the lib, if this is not done program will crash
  ngl::NGLInit::instance();

  /// This function was originally written by Jon Macey

  glClearColor( 0.6f , 1.0f , 1.0f , 1.0f );
  // Enable depth testing for drawing
  glEnable( GL_DEPTH_TEST );
  // Enable multisampling for smoother drawing
  glEnable( GL_MULTISAMPLE );
  // Now to load the shader and set the values
  // Grab an instance of shader manager
  ngl::ShaderLib *shader = ngl::ShaderLib::instance();
  // Create a shader called Phong
  shader->createShaderProgram( "Phong" );
  // Create empty shaders for Frag and Vert
  shader->attachShader( "PhongVertex" ,ngl::ShaderType::VERTEX);
  shader->attachShader( "PhongFragment" ,ngl::ShaderType::FRAGMENT);
  // Attach the source
  shader->loadShaderSource( "PhongVertex" , "shaders/PhongVertex.glsl" );
  shader->loadShaderSource( "PhongFragment" , "shaders/PhongFragment.glsl" );
  // Compile the shaders
  shader->compileShader( "PhongVertex" );
  shader->compileShader( "PhongFragment" );
  // Add them to the program
  shader->attachShaderToProgram( "Phong" , "PhongVertex" );
  shader->attachShaderToProgram( "Phong" , "PhongFragment" );

  // Now we have associated this data we can link the shader
  shader->linkProgramObject( "Phong" );
  // and make it active ready to load values
  (*shader)[ "Phong" ]->use();

  // Now we will create a basic Camera from the graphics library
  // This is a static camera so it only needs to be set once
  // First create Values for the camera position
  ngl::Vec3 from( 0, 0, -80 );
  ngl::Vec3 to( 0, 0, 0 );
  ngl::Vec3 up( 0, 1, 0 );
  // now load to our new camera
  m_cam.set( from, to, up );
  // set the shape using FOV 45 Aspect Ratio based on Width and Height
  // The final two are near and far clipping planes of 0.5 and 10
  m_cam.setShape( 45 , (float)720.0 / 576.0 , 0.05 , 350 );
  shader->setUniform( "viewerPos",m_cam.getEye().toVec3());
  // now create our light this is done after the camera so we can pass the
  // transpose of the projection matrix to the light to do correct eye space
  // transformations
  ngl::Mat4 iv = m_cam.getViewMatrix();
  iv.transpose();

  ///end of Citation

  ngl::Light light1(ngl::Vec3( 5, 10, 2 ),ngl::Colour( 1, 0, 1, 1 ),ngl::Colour( 1, 0, 1, 1 ),ngl::LightModes::SPOTLIGHT);
  light1.setTransform(iv);

  // Load these values to the shader as well
  light1.loadToShader( "light" );

  ngl::Material m(ngl::STDMAT::GOLD);
  m.loadToShader( "material" );

  // Create our bounding box
  m_bbox.reset( new ngl::BBox(ngl::Vec3(), 50.0f , 50.0f , 50.0f ));
  m_bbox->setDrawMode(GL_LINE);

  //Create our paddles
  m_paddleA.reset( new Paddle(ngl::Vec3( 0, 19 , 0) , "models/paddle3.obj" ));
  m_paddleB.reset( new Paddle(ngl::Vec3( 0, -19 , 0 ), "models/paddle3.obj" ));

  // Create our ball
  m_ball.reset( new Ball(ngl::Vec3( 0, 0, 0 ), ngl::Vec3( 0.04f, 0.04f , 0.0f ), 2.0f , "models/ball.obj" ));

  // Now we start a timer and assign it to the m_updatePaddleTimer and the m_ballUpdateTimer variable
  // This will be triggered at an interval of every 2ms
  m_animate = true;
  m_updatePaddleTimer = startTimer( 2 );
  m_redrawTimer = startTimer( 20 );
  m_ballUpdateTimer = startTimer( 2 );
  // Now clear the key set
  m_keysPressed.clear();

  // Set the font parameters before they are drawn on the screen
  m_text.reset( new ngl::Text(QFont( "Arial" , 15 )));
  m_text->setScreenSize(width(),height());
}


void NGLScene::loadMatricesToShader()
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  ngl::Mat4 MVP = m_cam.getVPMatrix();
  shader->setUniform( "MVP" , MVP );
}


void NGLScene::paintGL()
{
  /// This function was originally written by Jon Macey
  // Clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // now load these values to the shader


  glViewport( 0 , 0 , m_win.width , m_win.height );
  // Rotation based on the mouse position for our global
  // transform
  ngl::Mat4 rotX;
  ngl::Mat4 rotY;
  // create the rotation matrices
  rotX.rotateX( m_win.spinXFace );
  rotY.rotateY( m_win.spinYFace );

  // multiply the rotations
  m_mouseGlobalTX = rotY * rotX;

  // add the translations
  m_mouseGlobalTX.m_m[3][0] = m_modelPos.m_x;
  m_mouseGlobalTX.m_m[3][1] = m_modelPos.m_y;
  m_mouseGlobalTX.m_m[3][2] = m_modelPos.m_z;

  ///end of Citation

  // Set text colour as well as what the string will render on screen
  m_text->setColour( 0 , 0 , 0 );
  QString text = QString("                                                SCORE: P1 %1 : P2 %2" ).arg(m_scoreA).arg(m_scoreB);
  m_text->renderText( 10 , 20 , text );
  QString text1 = QString("Buttons: P1 Left Arrow Key = Left, Right Arrow Key = Right. P2 A = Left, D = Right. ");
  m_text->renderText( 10 , 690 , text1 );

  // Paint both paddles and the ball to the screen with the shader assigned to them
  m_paddleA->draw( "Phong" , &m_cam );
  m_paddleB->draw( "Phong" , &m_cam );

  m_ball->draw( "Phong" , m_mouseGlobalTX , &m_cam );

  // m_bbox has to be loaded last as it attaches to either the paddle or the ball and moves with it
  loadMatricesToShader();
  m_bbox->draw();
}


void NGLScene::checkCollisions()
{
  // Check collisions with the bounding box initially
  BBoxCollision();

  // Collide is set to false as there are no collisions
  bool collide = false;

  // Setting the positions of the paddles and ball by using a pointer to the getter
  ngl::Vec3 paddlePosB = m_paddleB->getPos();
  ngl::Vec3 paddlePosA = m_paddleA->getPos();
  ngl::Vec3 ballPos = m_ball->getPos();

  // Range represents the amount of space the ball has before it is reflected by the paddle
  float range = 4.0f;

  // If conditional statement checks if the absolute value of the distance between the paddle(A) and the ball is less than the range set
  if ( abs ( paddlePosA.m_x - ballPos.m_x ) < range )
  {
    if ( abs ( paddlePosA.m_y - ballPos.m_y ) < range )
    {
      // If the distance is smaller than the range, then the paddle(A) and ball are touching
      collide = true;
      ngl::Vec3 newDirX = m_ball->getDirection();
      ngl::Vec3 newDirY = m_ball->getDirection();

      // Therefore, if they are touching, I reflect the direction using the reflection matrix (taking x and y values)
      m_ball->setDirection( newDirX.reflect(ngl::Vec3( 1 , 0 , 0 )));
      m_ball->setDirection( newDirY.reflect(ngl::Vec3( 0 , -1 , 0 )));
      // setHit means that both the paddle(A) and ball have collided
      m_ball->setHit();

    }
  }
  // If conditional statement checks if the absolute value of the distance between the paddle(B) and the ball is less than the range set
  if ( abs ( paddlePosB.m_x - ballPos.m_x ) < range )
  {
    if ( abs ( paddlePosB.m_y - ballPos.m_y ) < range )
    {
      // If the distance is smaller than the range, then the paddle(B) and ball are touching
      collide = true;
      ngl::Vec3 newDirX = m_ball->getDirection();
      ngl::Vec3 newDirY = m_ball->getDirection();

      // Therefore, if they are touching, I reflect the direction using the reflection matrix (taking x and y values)S
      m_ball->setDirection( newDirX.reflect(ngl::Vec3( 1 , 0 , 0 )));
      m_ball->setDirection( newDirY.reflect(ngl::Vec3( 0 , -1 , 0 )));
      // setHit means that both the paddle(A) and ball have collided
      m_ball->setHit();
    }
  }
}


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseMoveEvent( QMouseEvent* _event )
{
  // note the method buttons() is the button state when event was called
  // that is different from button() which is used to check which button was
  // pressed when the mousePress/Release event is generated
  if ( m_win.rotate && _event->buttons() == Qt::LeftButton )
  {
    int diffx = _event->x() - m_win.origX;
    int diffy = _event->y() - m_win.origY;
    m_win.spinXFace += static_cast<int>( 0.5f * diffy );
    m_win.spinYFace += static_cast<int>( 0.5f * diffx );
    m_win.origX = _event->x();
    m_win.origY = _event->y();
    update();
  }
  // right mouse translate code
  else if ( m_win.translate && _event->buttons() == Qt::RightButton )
  {
    int diffX      = static_cast<int>( _event->x() - m_win.origXPos );
    int diffY      = static_cast<int>( _event->y() - m_win.origYPos );
    m_win.origXPos = _event->x();
    m_win.origYPos = _event->y();
    m_modelPos.m_x += INCREMENT * diffX;
    m_modelPos.m_y -= INCREMENT * diffY;
    update();
  }
}


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent( QMouseEvent* _event )
{
  // that method is called when the mouse button is pressed in this case we
  // store the value where the maouse was clicked (x,y) and set the Rotate flag to true
  if ( _event->button() == Qt::LeftButton )
  {
    m_win.origX  = _event->x();
    m_win.origY  = _event->y();
    m_win.rotate = true;
  }
  // right mouse translate mode
  else if ( _event->button() == Qt::RightButton )
  {
    m_win.origXPos  = _event->x();
    m_win.origYPos  = _event->y();
    m_win.translate = true;
  }
}


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent( QMouseEvent* _event )
{
  // that event is called when the mouse button is released
  // we then set Rotate to false
  if ( _event->button() == Qt::LeftButton )
  {
    m_win.rotate = false;
  }
  // right mouse translate mode
  if ( _event->button() == Qt::RightButton )
  {
    m_win.translate = false;
  }
}


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::wheelEvent( QWheelEvent* _event )
{
  // check the diff of the wheel position (0 means no change)
  if ( _event->delta() > 0 )
  {
    m_modelPos.m_z += ZOOM;
  }
  else if ( _event->delta() < 0 )
  {
    m_modelPos.m_z -= ZOOM;
  }
  update();
}


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::timerEvent( QTimerEvent *_event)
{
  //the usual process is to check the event timerID and compare it to
  // any timers we have started with startTimer
  if ( _event->timerId() == m_updatePaddleTimer )
  {
    movePaddleA();
    movePaddleB();
  }
  if ( _event->timerId() == m_ballUpdateTimer )
  {
    moveBall();
  }

  checkCollisions();

  if ( _event->timerId() == m_redrawTimer )
  {
    update();
  }
}

// This function moves Player 1's paddle
void NGLScene::movePaddleA()
{
  /// This moves the paddle that is positioned at the top of the screen
  /// first we reset the movement values
  float xDirection = 0.0;
  float yDirection = 0.0;

  // now we loop for each of the pressed keys in the the set
  // and see which ones have been pressed. If they have been pressed
  // we set the movement value to be an incremental value
  foreach(Qt::Key key, m_keysPressed)
  {
    switch (key)
    {
      case Qt::Key_Left :  { xDirection = s_paddleUpdate; break;}
      case Qt::Key_Right : { xDirection = -s_paddleUpdate; break;}
      default : break;
    }
  }
  // if the set is non zero size we can update the paddle's movement
  // then tell openGL to re-draw
  if(m_keysPressed.size() != 0)
  {
    m_paddleA->move(xDirection,yDirection);
  }
}


// This function moves Player 2's paddle
void NGLScene::movePaddleB()
{
  /// first we reset the movement values
  float xDirection = 0.0;
  float yDirection = 0.0;

  // now we loop for each of the pressed keys in the the set
  // and see which ones have been pressed. If they have been pressed
  // we set the movement value to be an incremental value
  foreach(Qt::Key key, m_keysPressed)
  {
    switch (key)
    {
      case Qt::Key_A : { xDirection = s_paddleUpdate; break;}
      case Qt::Key_D : { xDirection = -s_paddleUpdate; break;}
      default : break;
    }
  }
  // if the set is non zero size we can update the ship movement
  // then tell openGL to re-draw
  if(m_keysPressed.size() != 0)
  {
    m_paddleB->move( xDirection, yDirection);
  }
}


// This function moves the ball by constantly updating the position
void NGLScene::moveBall()
{
  m_ball->updatePos();
}


//----------------------------------------------------------------------------------------------------------------------
// in this case this method is called from the MainWindow class and passed
// the key event from that, if we didn't need to process the events in main
// window we could just overload this as the keyPressEvent and only process here.
//----------------------------------------------------------------------------------------------------------------------
void NGLScene::keyPressEvent( QKeyEvent *_event)
{
  // add to our keypress set the values of any keys pressed
  m_keysPressed += (Qt::Key)_event->key();

  switch (_event->key())
  {
    case Qt::Key_Escape : QApplication::exit(EXIT_SUCCESS); break;
  }
}


//----------------------------------------------------------------------------------------------------------------------
// in this case this method is called from the MainWindow class and passed
// the key event from that, if we didn't need to process the events in main
// window we could just overload this as the keyPressEvent and only process here.
//----------------------------------------------------------------------------------------------------------------------
void NGLScene::keyReleaseEvent( QKeyEvent *_event	)
{
  // remove from our key set any keys that have been released
  m_keysPressed -= (Qt::Key)_event->key();
}


void NGLScene::BBoxCollision()
{
  m_ball->getPos();

  /// This function was originally written by Jon Macey
  //create an array of the extents of the bounding box
  float ext[ 4 ];
  ext[ 0 ] = ( m_bbox->height()/ 2.0f );
  ext[ 1 ] = ( m_bbox->height()/ 2.0f );
  ext[ 2 ] = ( m_bbox->width()/ 2.0f );
  ext[ 3 ] = ( m_bbox->width()/ 2.0f);

  //Dot product needs a Vector so we convert The Point Temp into a Vector so we can
  // do a dot product on it
  ngl::Vec3 p = m_ball->getPos();
  // D is the distance of the Agent from the Plane. If it is less than ext[i] then there is
  // no collision
  GLfloat D;
  //Now we need to check the Sphere agains all 6 planes of the BBOx
  //If a collision is found we change the dir of the Sphere then Break
  for(int i = 0; i < 4; ++i)
  {
    //to calculate the distance we take the dotporduct of the Plane Normal
    //with the new point P
    D = m_bbox->getNormalArray()[ i ].dot( p );

    //Now Add the Radius of the sphere to the offsett
    D += m_ball->getRadius();

    // If this is greater or equal to the BBox extent /2 then there is a collision
    //So we calculate the Spheres new direction
    ///end of Citation

    if(D >= ext[ i ])
    {
      //We use the same calculation as in raytracing to determine the
      // the new direction
      GLfloat x = 2 *( m_ball->getDirection().dot((m_bbox->getNormalArray()[ i ])));
      ngl::Vec3 d = m_bbox->getNormalArray()[ i ] * x;
      m_ball->setDirection(m_ball->getDirection() - d);
      m_ball->setHit();

      //This is for the top wall
      if(i == 0)
      {
        // Increment the score of Player 2
        ++m_scoreB;
        // Reset the paddles and ball to their original position
        reset();
        //If the player's score equals 3, the game ends by displaying the winner in the terminal
        if (m_scoreB == 3)
        {
          std::cout<<"Player 2 wins!\n Closing Application in 3 seconds \n";
          //Delays the exit process for three seconds
          timer();
          QApplication::exit(EXIT_SUCCESS);
         }
      }
      //This is for the bottom wall
      else if ( i == 1 )
      {
        // Increment the score of Player 1
        ++m_scoreA;
        // Reset the paddles and ball to their original position
        reset();
        //If the player's score equals 3, the game ends by displaying the winner in the terminal
        if ( m_scoreA == 3 )
        {
          std::cout<<" Player 1 wins!\n Closing Application in 3 seconds \n ";
          //Delays the exit process for three seconds
          timer();
          QApplication::exit( EXIT_SUCCESS );
        }
      }
    }//end of hit test
  }//end of for loop
}


/// This function was originally written by Jon Macey
/// This checks the sphere collisions based on the bounding box of the paddle and the sphere of the ball
bool NGLScene::sphereSphereCollision( ngl::Vec3 _pos1, GLfloat _radius1 , ngl::Vec3 _pos2 , GLfloat _radius2 )
{
  // the relative position of the spheres
  ngl::Vec3 relPos;
  //min an max distances of the spheres
  GLfloat dist;
  GLfloat minDist;
  GLfloat len;
  relPos = _pos1 - _pos2;
  // and the distance
  len = relPos.length();
  dist = len * len;
  minDist = _radius1 + _radius2;
  // if it is a hit
  if( dist <= ( minDist * minDist ) )
  {
    return true;
  }
  else
  {
    return false;
  }
}/// end of function




