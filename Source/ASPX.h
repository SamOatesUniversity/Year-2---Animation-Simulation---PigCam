#include <vector>
#include "twm/util/math.hpp"
using namespace twm::operators;

namespace ASPX
{
	namespace Kinematics
	{
		void Euler( twm::Vector &value, const twm::Vector &derivative, float h );
	}

	namespace Transformation
	{
		twm::Matrix Position( float x, float y, float z );
		twm::Matrix Position( twm::Vector xyz );
		twm::Matrix Rotate( float x, float y, float z );
		twm::Matrix Rotate( twm::Vector xyz );
	}

	namespace Conversion
	{
		float DegreesToRad( float deg );
		float RadToDegrees( float rad );
	}

	namespace Interpolation
	{
		template <class T>
		T Lerp( T p0, T p1, float s ) 
		{ 
			return p0 + (p1 - p0 ) * s; 
		}

		template <class T>
		T Clamp( T min, T max, T x )
		{
			if( x < min ) return min;
			if( x > max ) return min;
			return x;
		}

		float Percentage( float min, float max, float x );
		float Smoothstep( float min, float max, float x );
	}

	namespace KeyFrame
	{
		struct SKeyFrame {
			twm::Vector value;
			float time;
		};

		class CKeyFrame
		{
		private:
			std::vector< SKeyFrame > frame_;
			unsigned int activeFrame_;
			float animationLength_, intialTime_;
		public:
			CKeyFrame(void);
			~CKeyFrame(void);

			void addFrame( twm::Vector value, float time );
			bool update( twm::Vector &vector, float time );
		};
	}

	namespace Path
	{
		twm::Matrix Frenet( const twm::Vector& pos, const twm::Vector& dir, const twm::Vector& up );
	}
}