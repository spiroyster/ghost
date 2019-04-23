# Ghost

Header only UI macro recording and injection facilities. Ghost is designed to be easy to use and lightweight using just STL and standard Windows.h includes.
It requires C++14. This is a port to C++ from C# for a test suite used for black box automated ui testing.

## usage

Ghost is a header only library using just STL and C++14 so to use in your program simply include ghost.hpp in your code.

	#include "ghost.hpp"

Ghost is distrubted under a MIT lic so you can do what you want with it, however any distribution must contain the information from license.txt in a program readme.

## script

In order to replay actions. A script can be used to create macros. Each script must contain as its first action a program::exec action. This will denote the scope of the hooks when injecting.
This will be either the scope of the application, and the application will be invoked/run when recording or replaying, or absolute in which case the current application running will be
assumed to have the handle for the hooks.

Scripts can be constructed from syntax.

	ghost::script macro(scriptSyntax);

The script can then be replayed by calling 'play'.

	macro.play();

Scripts can be constructed manually.

	ghost::script macro;
	macro.add(ghost::action(new ghost::mouse::down(ghost::mouse::button::Left, 10, 10)));
	macro.add(ghost::action(new ghost::mouse::down(ghost::mouse::button::Left, 20, 10)));
	macro.add(ghost::action(new ghost::mouse::down(ghost::mouse::button::Left, 20, 20)));
	macro.add(ghost::action(new ghost::mouse::down(ghost::mouse::button::Left, 10, 20)));

Please remember to insert wait actions inbetween others.

## recording

Scripts can be recorded from this process:

	ghost::sciprt macro = ghost::record::script("");

From an new process which is run/invoked from calling this function. Recording starts immediately and finished when program terminates.

	ghost::script macro = ghost::record::script("command args");















## actions

Ghost can be used to inject keyboard and mouse actions into program message loops. A single event can be injected by creating an instance of the injectable action and then calling inject.

	ghost::keyboard::down('a').inject();		// inject 'a' key press down.
	ghost::mouse::move(10, 10).inject();		// move the mouse cursor position to 10, 10

This will be injected using this programs handle so any mouse coordinates will be relative to the entire screen. 
Alternatively an action instance can be constructed and the inject call delayed and repeated.

	ghost::mouse::move mm(10, 10);
	mm.inject();
	mm.inject();

This will create a invokcable action which is held in the stack and only valid for the scope of this code. To create an invocable action on the heap this needs to be done via the action interface.

	typedef std::shared_ptr<ghost::invocable> ghost::action;

This can be constructed either manually or via a factory.

	ghost::action mouseMoveAction(new ghost::mouse::move(10, 10));
	ghost::action mouseMoveAction = ghost::mouse::factory("mm", "10, 10");






## examples
