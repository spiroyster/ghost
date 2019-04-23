
#include "..\include\ghost.hpp"

int main(int argc, char** argv)
{
	// example 1
	{
		ghost::program::exec cmd("cmd");
		cmd.run();

		ghost::inject(ghost::program::wait(1000));

		ghost::inject(ghost::keyboard::down('e'));
		ghost::inject(ghost::keyboard::up('e'));
		ghost::inject(ghost::program::wait(1000));

		ghost::inject(ghost::keyboard::down('x'));
		ghost::inject(ghost::keyboard::up('x'));
		ghost::inject(ghost::program::wait(1000));

		ghost::inject(ghost::keyboard::down('i'));
		ghost::inject(ghost::keyboard::up('i'));
		ghost::inject(ghost::program::wait(1000));

		ghost::inject(ghost::keyboard::down('t'));
		ghost::inject(ghost::keyboard::up('t'));

		ghost::inject(ghost::program::wait(1000));
		ghost::inject(ghost::keyboard::down(ghost::keyboard::key::Enter));
		ghost::inject(ghost::keyboard::up(ghost::keyboard::key::Enter));

		cmd.wait();
	}

	{
		ghost::program::exec cmd("cmd");
		cmd.run();

		ghost::inject(ghost::program::wait(1000));

		ghost::inject(ghost::keyboard::press('e'));
		ghost::inject(ghost::program::wait(100));
		ghost::inject(ghost::keyboard::press('x'));
		ghost::inject(ghost::program::wait(100));
		ghost::inject(ghost::keyboard::press('i'));
		ghost::inject(ghost::program::wait(100));
		ghost::inject(ghost::keyboard::press('t'));
		ghost::inject(ghost::program::wait(1000));
		ghost::inject(ghost::keyboard::press(ghost::keyboard::key::Enter));

		cmd.wait();
	}

	{
		ghost::program::exec cmd("cmd");
		cmd.run();

		ghost::inject(ghost::program::wait(1000));

		ghost::inject(ghost::keyboard::type("exit"));
		ghost::inject(ghost::program::wait(1000));
		ghost::inject(ghost::keyboard::press(ghost::keyboard::key::Enter));

		cmd.wait();
	}

	{
		ghost::script macro;

		macro.add(ghost::action(new ghost::program::wait(1000))); 
		macro.add(ghost::action(new ghost::keyboard::press('e')));
		macro.add(ghost::action(new ghost::program::wait(100)));
		macro.add(ghost::action(new ghost::keyboard::press('x')));
		macro.add(ghost::action(new ghost::program::wait(100)));
		macro.add(ghost::action(new ghost::keyboard::press('i')));
		macro.add(ghost::action(new ghost::program::wait(100)));
		macro.add(ghost::action(new ghost::keyboard::press('t')));
		macro.add(ghost::action(new ghost::program::wait(1000)));
		macro.add(ghost::action(new ghost::keyboard::press(ghost::keyboard::key::Enter)));

		ghost::program::exec cmd("cmd");
		cmd.run();
		macro.play();
		cmd.wait();

	}

	return 0;

}