#include "ASPX.h"

namespace ASPX
{
	namespace Kinematics
	{
		void Euler( twm::Vector &value, const twm::Vector &derivative, float h )
		{
			value += derivative * h;
		}
	}

	namespace Transformation
	{
		twm::Matrix Position( float x, float y, float z )
		{
			return twm::Matrix(	1,	0,	0,	0,
								0,	1,	0,	0,
								0,	0,	1,	0,
								x,	y,	z,	1 );
		}

		twm::Matrix Position( twm::Vector xyz )
		{
			return twm::Matrix(	1,		0,		0,		0,
								0,		1,		0,		0,
								0,		0,		1,		0,
								xyz.x,	xyz.y,	xyz.z,	1 );
		}

		twm::Matrix Rotate( float x, float y, float z )
		{
			x = Conversion::DegreesToRad( x );
			y = Conversion::DegreesToRad( y );
			z = Conversion::DegreesToRad( z );

			twm::Matrix xMatrix(	1,				0,				0,				0,
									0,				cos(x),			-sin(x),		0,
									0,				sin(x),			cos(x),			0,
									0,				0,				0,				1);

			twm::Matrix yMatrix(	cos(y),			0,				sin(y),			0,
									0,				1,				0,				0,
									-sin(y),		0,				cos(y),			0,
									0,				0,				0,				1);

			twm::Matrix zMatrix(	cos(z),			-sin(z),		0,				0,
									sin(z),			cos(z),			0,				0,
									0,				0,				1,				0,
									0,				0,				0,				1);

			return xMatrix * yMatrix * zMatrix;
		}

		twm::Matrix Rotate( twm::Vector xyz )
		{
			float x = Conversion::DegreesToRad( xyz.x );
			float y = Conversion::DegreesToRad( xyz.y );
			float z = Conversion::DegreesToRad( xyz.z );

			twm::Matrix xMatrix(	1,				0,				0,				0,
									0,				cos(x),			-sin(x),		0,
									0,				sin(x),			cos(x),			0,
									0,				0,				0,				1);

			twm::Matrix yMatrix(	cos(y),			0,				sin(y),			0,
									0,				1,				0,				0,
									-sin(y),		0,				cos(y),			0,
									0,				0,				0,				1);

			twm::Matrix zMatrix(	cos(z),			-sin(z),		0,				0,
									sin(z),			cos(z),			0,				0,
									0,				0,				1,				0,
									0,				0,				0,				1);

			return xMatrix * yMatrix * zMatrix;
		}
	}

	namespace Conversion
	{
		float DegreesToRad( float deg )
		{
			return deg * 0.017453f;
		}

		float RadToDegrees( float rad )
		{
			return rad * 57.29578f;
		}
	}

	namespace Interpolation
	{
		float Smoothstep( float min, float max, float x )
		{
			return ( (x) * (x) * ( 3 - 2*(x) ) );
		}

		float Percentage ( float min, float max, float x )
		{
			return ( x - min ) / ( max - min );
		}
	}

	namespace KeyFrame
	{
		CKeyFrame::CKeyFrame(void)
		{
			activeFrame_ = 0;
			animationLength_ = 0.0f;
		}

		CKeyFrame::~CKeyFrame(void)
		{
		}

		void CKeyFrame::addFrame( twm::Vector value, float time )
		{
			SKeyFrame frame;
			frame.value = value;
			frame.time = time;
			frame_.push_back( frame );

			animationLength_ = time;
			intialTime_ = frame_[0].time;
		}

		bool CKeyFrame::update( twm::Vector &vector, float time )
		{
			//Calculate the percentage with respect to time
			float s = Interpolation::Percentage( frame_[activeFrame_].time, frame_[activeFrame_+1].time, time );

			//Work out the new position
			float smooth = Interpolation::Smoothstep( frame_[activeFrame_].time, frame_[activeFrame_+1].time, Interpolation::Clamp( 0.0f, 1.0f, s ) );
			vector = Interpolation::Lerp( frame_[activeFrame_].value, frame_[activeFrame_+1].value, smooth );

			//If current time is the next frames start time, increase the current frame
			if( time >= frame_[activeFrame_+1].time - 0.05f ) activeFrame_++;

			//If the active frame is greater than the frame count
			//Increase each frames time by the complete animation length
			//and set the active frame back to zero
			if( activeFrame_ >= frame_.size()-1 )
			{
				for( unsigned int i = 0; i < frame_.size(); i++ ) frame_[i].time += (animationLength_ - intialTime_);
				activeFrame_ = 0;
			}

			return true;
		}
	}

}