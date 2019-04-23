
#include "..\include\ghost.hpp"

int main(int argc, char** argv)
{
	// example 3
	{
		// macro recording and playback...

		ghost::script recording = ghost::record::script(ghost::program::exec("cmd"));

		ghost::program::wait(1000).inject();

		// then playback the recording...
		recording.play();
	}


	return 0;

}