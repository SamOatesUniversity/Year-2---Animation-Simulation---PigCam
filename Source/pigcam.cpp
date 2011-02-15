/**
* Original Author: Tyrone Davison, Teesside University
* Filename: pigcam.cpp
* Date: January 2011
* Description: class source for assignment task 2
* Notes: most of your work will be in here
*/

#include "prototype.hpp"
#include "pigcam.hpp"

#include "twm/graphics/components.hpp"
#include "twm/util/math.hpp"

#include <iostream>

using namespace twm::operators;

PigCamService::PigCamService()
{
}

PigCamService::~PigCamService()
{
}

void PigCamService::DoStart( twm::IWorldUpdate* world )
{
	// create a camera
	_camera = world->CreateEntity();
	twm::PerspectiveCamera perspective = _camera.CreateComponent( twm::kPerspectiveCamera );
	perspective.SetFieldOfView( 50 );

	_front = twm::Vector( 0.0f, 5.0f, 10.0f );
	_rear = twm::Vector( 0.0f, 4.0f, -15.0f );
	_driver = twm::Vector( 0.5f, 2.4f, 0.1f );

	_currentPosition = _rear;
	_startPosition = _currentPosition;
	_destPosition = _currentPosition;

	_frontAngle = twm::Vector( ASPX::Conversion::RadToDegrees( atan( _front.y / _front.z ) ), 0.0f, 0.0f );
	_rearAngle = twm::Vector( ASPX::Conversion::RadToDegrees( atan( -_rear.y / _rear.z ) ), 180.0f, 0.0f );
	_driverAngle = twm::Vector( 0.0f, 180.0f, 0.0f );
	
	_currentAngle = _rearAngle;
	_startPosition = _currentAngle;
	_destPosition = _currentAngle;
	
	_camera.SetTransformation(  ASPX::Transformation::Rotate( 0.0f, 180.0f, 0.0f ) * 
								ASPX::Transformation::Position( _currentPosition ) );

	_translationLength = 1.8f;
	_isTranslating = false;
	_isKeyFraming = false;

	_keyFramePosition = new ASPX::KeyFrame::CKeyFrame();
	_keyFrameAngle = new ASPX::KeyFrame::CKeyFrame();

	_timer.Reset();
}

void PigCamService::DoStop( twm::IWorldUpdate* world )
{
	delete _keyFramePosition;
	delete _keyFrameAngle;
}

void PigCamService::DoFrame( twm::IWorldUpdate* world, twm::IMessageIterator* messages )
{
	// compute the elapsed time in seconds since the service started
	const float time = 0.001f * _timer.GetTime();

	// iterate through the messages to find camera directives
	while ( messages->HasNext() )
	{
		const twm::Message& msg = messages->Next();

		switch ( msg.type )
		{
		case kSetActiveWartPig: // you must smoothly move the camera to the WartPig indicated in msg.entity1
			{
				std::cout << "debug: change pig trigger received" << std::endl; // for debugging purposes only
				_pig_entity = world->GetEntity( msg.entity1 );
				_camera.SetParent( _pig_entity );
			}
			break;
		case kSetCameraFrontView: // you must smoothly move the camera to the front view of the active WartPig
			{
				_active_view_position = _front;
				_active_view_angle = _frontAngle;
				DoTranslation( _active_view_position, _active_view_angle, time );
				std::cout << "debug: front view trigger received" << std::endl; // for debugging purposes only
			}
			break;
		case kSetCameraBackView: // you must smoothly move the camera to the back view of the active WartPig
			{
				_active_view_position = _rear;
				_active_view_angle = _rearAngle;
				DoTranslation( _active_view_position, _active_view_angle, time );
				std::cout << "debug: rear view trigger received" << std::endl; // for debugging purposes only
			}
			break;
		case kSetCameraDriverView: // you must smoothly move the camera to the driver's view of the active WartPig
			{
				_active_view_position = _driver;
				_active_view_angle = _driverAngle;
				DoTranslation( _active_view_position, _active_view_angle, time );
				std::cout << "debug: driver view trigger received" << std::endl; // for debugging purposes only
			}
			break;
		case kStartCameraAnimation: // you must smoothly transit to and start the repair station's camera sequence
			{
				std::cout << "debug: start keyframe animation trigger received" << std::endl; // for debugging purposes only
				//** example code for finding the repair station entity and component (only valid in this case statement)
				twm::Entity repair_station_entity = world->GetEntity( msg.entity1 );
				RepairStationComponent repair_station_data = repair_station_entity.GetComponent( kRepairStation );
				//** example code for reading out the keyframes you must use for the camera
				std::vector< RepairStationComponent::CameraKeyframe > keys;
				keys.resize( repair_station_data.GetCameraKeyframes(0,0) );
				repair_station_data.GetCameraKeyframes( &keys[0], keys.size() );

				_camera.SetParent( repair_station_entity );
				_isKeyFraming = true;

				for( unsigned int i = 0; i < keys.size(); i++ )
					_keyFramePosition->addFrame( keys[i].position, keys[i].time + time );
				_keyFramePosition->addFrame( keys[0].position, keys[keys.size()-1].time  + 2.0f + time );

				for( unsigned int i = 0; i < keys.size(); i++ )
					_keyFrameAngle->addFrame( keys[i].interest, keys[i].time + time );
				_keyFrameAngle->addFrame( keys[0].interest, keys[keys.size()-1].time  + 2.0f + time );
				
			}
			break;
		case kStopCameraAnimation: // you must stop the camera animation and smoothly move to the previous view
			{
				std::cout << "debug: abort keyframe animation trigger received" << std::endl; // for debugging purposes only
				_isKeyFraming = false;
				_camera.SetParent( _pig_entity );
				DoTranslation( _active_view_position, _active_view_angle, time );
			}
			break;
		}
	}

	if( _isTranslating && !_isKeyFraming )
	{
		UpdateVector( _currentPosition, _startPosition, _destPosition, time );
		UpdateVector( _currentAngle, _startAngle, _destAngle, time );
		if( time > _startTranslation + _translationLength - 0.02f ) _isTranslating = false;
	}
	else if( _isKeyFraming )
	{
		_keyFramePosition->update( _currentPosition, time );
		twm::Vector lookat;
		_keyFrameAngle->update( lookat, time );
		_currentAngle = getLookAtVec( _currentPosition, lookat );
	}

	twm::Matrix camerPosition = ASPX::Transformation::Rotate( _currentAngle ) * 
								ASPX::Transformation::Position( _currentPosition );

	_camera.SetTransformation( camerPosition );
}

void PigCamService::UpdateVector( twm::Vector &vector, twm::Vector start, twm::Vector end, float time )
{
	float s = ASPX::Interpolation::Percentage( _startTranslation, _startTranslation + _translationLength, time );
	float smooth = ASPX::Interpolation::Smoothstep( _startTranslation, _startTranslation + _translationLength,
																ASPX::Interpolation::Clamp( 0.0f, 1.0f, s ) );
	vector = ASPX::Interpolation::Lerp( start, end, smooth );
}

void PigCamService::DoTranslation( twm::Vector destination, twm::Vector angle, float currentTime )
{
	_startPosition = _currentPosition;
	_destPosition = destination;
	
	_startAngle = _currentAngle;
	_destAngle = angle;
	
	_startTranslation = currentTime;
	_isTranslating = true;
}


//BEZIER... POINT ONE AT KEY FRAME POS, POINT TWO AND INTREST...
//TANGENT OF BEZIER IS = TO LOOK AT.

twm::Vector PigCamService::getLookAtVec( twm::Vector position, twm::Vector lookat )
{
	twm::Vector Vector;
	Vector.x = lookat.x - position.x;
	Vector.y = lookat.y - position.y;
	Vector.z = lookat.z - position.z;

	double yangle = atan2(Vector.x, Vector.z);
	if( yangle < 0.0 ) yangle += ASPX::Conversion::DegreesToRad( 360.0f );
	if( yangle >= ASPX::Conversion::DegreesToRad( 360.0f ) ) yangle -= ASPX::Conversion::DegreesToRad( 360.0f );

	twm::HVector HVector = twm::HVector( Vector, 1.0f );
	HVector = HVector * ASPX::Transformation::Rotate( 0.0f, (float)-yangle, 0.0f );
	Vector = twm::Vector( HVector.x, HVector.y, HVector.z );

	double xangle = -atan2( Vector.y, Vector.z );
	if( xangle < 0.0 ) xangle += ASPX::Conversion::DegreesToRad( 360.0 );
	if( xangle >= ASPX::Conversion::DegreesToRad( 360.0f ) ) xangle -= ASPX::Conversion::DegreesToRad( 360.0f );

	return twm::Vector( ASPX::Conversion::RadToDegrees( (float)xangle ), ASPX::Conversion::RadToDegrees( (float)yangle ), 0.0f );
}