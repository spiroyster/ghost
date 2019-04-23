
#include "..\include\ghost.hpp"

int main(int argc, char** argv)
{
	const float pi = 3.141592654f;
	// example 2
	{
		unsigned int orbits = 5;
		unsigned int segments = 128;
		ghost::mouse::screen scn;
		unsigned int fps = 2000 / segments;
		unsigned int midX = scn.width() / 2;
		unsigned int midY = scn.height() / 2;
		
		float thetaDiff = (2.0f * pi) / static_cast<float>(segments);
		float orbitRadius = (scn.height() / 2) / orbits;
		
		float currentRadius = 0;
		float radiusDiff = orbitRadius / segments;

		while (static_cast<unsigned int>(currentRadius) < scn.height())
		{
			for (unsigned int n = 0; n < segments; ++n)
			{
				float theta = static_cast<float>(n) * thetaDiff;
				float r = currentRadius;

				float x = r * cos(theta);
				float y = r * sin(theta);

				ghost::mouse::move(static_cast<unsigned int>(x) + midX, static_cast<unsigned int>(y) + midY).inject();
				ghost::program::wait(fps).inject();

				currentRadius += radiusDiff;
			}
		}
		
	}


	return 0;

}