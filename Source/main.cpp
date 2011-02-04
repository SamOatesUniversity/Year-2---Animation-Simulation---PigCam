/**
  * Original Author: Tyrone Davison, Teesside University
  * Filename: main.cpp
  * Date: January 2011
  * Description: application main for assignment task 2
  * Notes: you shouldn't need to modify this file
  */

#include "twm/core/world.hpp"
#include "twm/core/service.hpp"

#include "twm/windows/window.hpp"
#include "twm/graphics/components.hpp"
#include "twm/graphics/basic_renderer.hpp"

#include "prototype.hpp"
#include "pigcam.hpp"

#include <iostream>
#include <crtdbg.h>

//** you should not need to modify main for the assignment
int main( int argc, char* argv[] )
{
// debug check for memory leaks on exit
  _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

  std::cout << "Animation and Simulation Programming: ICA Task 2" << std::endl;
  std::cout << "Usage instructions:" << std::endl;
  std::cout << "  '1' - Camera mode \"front\"" << std::endl;
  std::cout << "  '2' - Camera mode \"rear\"" << std::endl;
  std::cout << "  '3' - Camera mode \"driver\"" << std::endl;
  std::cout << "  '4' - Toggle active WartPig" << std::endl;

  // create an instance of a virtual world
  twm::World* world = new twm::World();

  // create a window and attach it to the world
  twm::Window* window = new twm::Window( "win1" );
  world->AttachService( window );

  // create and assign a renderer for the window
  twm::BasicRenderer* renderer = new twm::BasicRenderer();
  window->SetRenderer( renderer );

  // register component types for the renderer
  twm::RegisterGraphicsComponentTypes( world );

  // create the testing service and attach it to the world
  PrototypeService* prototype = new PrototypeService();
  world->AttachService( prototype );

  // register component types for the testing service
  RegisterPrototypeComponentTypes( world );

  // create our service to host the assignment code and attach it to the world
  PigCamService* pigcam = new PigCamService();
  world->AttachService( pigcam );

  // signal services to start
  world->Start();

  // advance to the next frame at approximately 60fps
  const unsigned int ideal_frame_duration = 17;
  twm::Timer timer;
  unsigned int frame_time = timer.GetTime() + ideal_frame_duration;
  while ( window->IsOpen() )
  {
    world->AdvanceFrame();

    unsigned int current_time = timer.GetTime();
    int delta_time = frame_time - current_time;
    if ( delta_time > 0 )
    {
      timer.Sleep( delta_time );
      frame_time += ideal_frame_duration;
    }
    else
    {
      // dropped time
      frame_time = current_time + ideal_frame_duration;
    }
  }

  // signal services to stop
  world->Stop();

  // cleanup
  delete pigcam;
  delete prototype;
  delete window;
  delete renderer;
  delete world;

  // wait for a key press before exit (so you can see console output)
  system( "PAUSE" );
  return 0;
}
