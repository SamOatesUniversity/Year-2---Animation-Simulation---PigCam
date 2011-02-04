/**
  * Original Author: Tyrone Davison, Teesside University
  * Filename: pigcam.hpp
  * Date: January 2011
  * Description: class header for assignment task
  */

#include "twm/core/world.hpp"
#include "twm/core/service.hpp"
#include "twm/util/timer.hpp"
#include <vector>

#include "ASPX.h"

class PigCamService : public twm::IService
{
public:
  PigCamService();
  ~PigCamService();
public:
  void DoStart( twm::IWorldUpdate* world );
  void DoStop( twm::IWorldUpdate* world );
  void DoFrame( twm::IWorldUpdate* world, twm::IMessageIterator* messages );
private:
  twm::Timer _timer;
  twm::Entity _camera, _pig_entity;
  twm::Vector _front, _rear, _driver, _active_view_position;
  twm::Vector _currentPosition, _startPosition, _destPosition; 

  twm::Vector _frontAngle, _rearAngle, _driverAngle, _active_view_angle;
  twm::Vector _currentAngle, _startAngle, _destAngle;

  float _startTranslation, _translationLength;
  bool _isTranslating, _isKeyFraming;

  ASPX::KeyFrame::CKeyFrame *_keyFramePosition, *_keyFrameAngle;

  void DoTranslation( twm::Vector destination, twm::Vector angle, float currentTime );
  void UpdateVector( twm::Vector &vector, twm::Vector start, twm::Vector end, float time );
  twm::Vector getLookAtVec( twm::Vector position, twm::Vector lookat );
};