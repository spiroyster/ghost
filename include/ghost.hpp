/*
BSD 3 - Clause License

Copyright(c) 2019, spiroyster
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met :

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
	SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
	CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
	OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef GHOST_HPP
#define GHOST_HPP

#ifdef _WIN32
#define GHOST_WINDOWS
#include <Windows.h>
#endif



#include <string>
#include <sstream>
#include <thread>
#include <chrono>
#include <vector>
#include <memory>
#include <list>
#include <algorithm>

namespace ghost
{

#ifdef GHOST_ENABLE_MESSAGES
	static std::function<void(const std::string& msg)> messageCallback;
#endif
	

	
	// ui action
	class injectable
	{
	public:
		virtual void inject() const = 0;
		virtual std::string args() const = 0;
		virtual std::string op() const = 0;

		std::string syntax() const
		{
			std::ostringstream oss;
			oss << op() << " {" << args() << "}";
			std::string msg = oss.str();

			return msg;
		}

	};

	typedef std::shared_ptr<injectable> action;

	namespace program
	{
		class exec : public injectable
		{
			std::string program_;
			PROCESS_INFORMATION processInfo_;
		public:
			exec(const std::string& program)
				: program_(program)
			{
			}

			//std::string op() const { return ID::str[ID::Exec]; }
			std::string op() const { return "exec"; }
			std::string args() const { return program_; }

			void run()
			{
				if (program_.empty())
				{
					// we do not invoke programs...
				}
				else
				{
					STARTUPINFO info = { sizeof(info) };
					LPSTR s = const_cast<char *>(program_.c_str());
					if (!CreateProcess(NULL, s, NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo_))
						std::exception("Unable to start program...");
				}
				
			}
			void inject() const
			{

			}
			HANDLE handle() 
			{
				if (program_.empty())
				{
					return NULL;
				}
				else
					return processInfo_.hProcess; 
			}
			DWORD threadID()
			{
				if (program_.empty())
				{
					return 0;
				}
				else
					return processInfo_.dwThreadId;
			}

			void wait()
			{
				if (program_.empty())
				{

				}
				else if (processInfo_.hProcess != NULL)
					WaitForSingleObject(processInfo_.hProcess, INFINITE);
			}
			void terminate()
			{
				if (program_.empty())
				{

				}
				else if (processInfo_.hProcess != NULL)
				{
					CloseHandle(processInfo_.hProcess);
					CloseHandle(processInfo_.hThread);
				}
			}
		};

		// actions
		class wait : public injectable
		{
			unsigned int millisecs_;
		public:
			wait(const std::string& args) { std::istringstream iss; iss >> millisecs_; }
			wait(unsigned int millisecs) : millisecs_(millisecs) {}

			//std::string op() const { return ID::str[ID::Wait]; }
			std::string op() const { return "w"; }
			std::string args() const { return std::to_string(millisecs_); }

			void inject() const
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(millisecs_));
			}
		};

		static action factory(const std::string& op, const std::string& args)
		{
			action newAction;

			if (op == "w")
				newAction.reset(new wait(args));
			else if (op == "exec")
				newAction.reset(new exec(args));

			return newAction;
		}

	}
	

	// mouse actions
	namespace mouse
	{
		enum button
		{
			None = 0,
			Left,
			Right,
			Middle,
			X,
			CairoElephant
		};

		static const std::vector<std::string> buttonStr({
			"m",
			"ml",
			"mr",
			"mm",
			"mx",
			"ce"
			});

		struct screen
		{
			screen()
			{
				RECT desktop;
				// Get a handle to the desktop window
				const HWND hDesktop = GetDesktopWindow();
				// Get the size of screen to the variable desktop
				GetWindowRect(hDesktop, &desktop);

				left_ = desktop.left;
				right_ = desktop.right;
				top_ = desktop.top;
				bottom_ = desktop.bottom;
			}

			int width() const { return abs(right_ - left_); }
			int height() const { return abs(top_ - bottom_); }

			int left_, right_, top_, bottom_;
		};

		class base : public injectable
		{
		public:
			base()
				: button_(button::None) {}
			base(const button& button)
				: button_(button) {}
			base(const button& button, int x, int y)
				: button_(button), x_(x), y_(y) 
			{
				int g = 0;
				++g;
			}

			std::string args() const
			{
				std::ostringstream oss;
				oss << buttonStr[static_cast<unsigned int>(button_)] << " " << x_ << " " << y_;
				return oss.str();
			}

		protected:
			void setButton(const std::string& bStr)
			{
				for (unsigned int id = button::None; id <= button::CairoElephant; ++id)
					if (buttonStr[id] == bStr)
					{
						button_ = static_cast<button>(id);
						return;
					}
			}

			button button_;
			int x_, y_;
		};


		class down : public base
		{
		public:
			down(const button& button, int x, int y)
				: base(button, x, y) {}
			down(const std::string& args)
			{
				std::string bArgs;
				std::istringstream iss(args);
				iss >> bArgs >> std::ws >> x_ >> std::ws >> y_;
				setButton(bArgs);
			}

			//std::string op() const { return ID::str[ID::MouseDown]; }
			std::string op() const { return "md"; }

			void inject() const
			{
				std::vector<INPUT> input(2);
				input[0].type = INPUT_MOUSE;
				input[0].mi.dx = x_ * (65536 / GetSystemMetrics(SM_CXSCREEN));
				input[0].mi.dy = y_ * (65536 / GetSystemMetrics(SM_CYSCREEN));
				input[0].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;

				input[1].type = INPUT_MOUSE;
				switch (button_)
				{
				case button::Right:
					input[1].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
					break;
				case button::Left:
					input[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
					break;
				case button::Middle:
					input[1].mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
					break;
				case button::X:
					input[1].mi.dwFlags = MOUSEEVENTF_XDOWN;
					break;
				}

				SendInput(static_cast<UINT>(input.size()), &input.front(), sizeof(INPUT));
			}
		};

		class up : public base
		{
		public:
			up(const button& button, int x, int y)
				: base(button, x, y) {}
			up(const std::string& args)
			{
				std::string bArgs;
				std::istringstream iss(args);
				iss >> bArgs >> std::ws >> x_ >> std::ws >> y_;
				setButton(bArgs);
			}

			//std::string op() const { return ID::str[ID::MouseUp]; }
			std::string op() const { return "mu"; }

			void inject() const
			{
				std::vector<INPUT> input(2);
				input[0].type = INPUT_MOUSE;
				input[0].mi.dx = x_ * (65536 / GetSystemMetrics(SM_CXSCREEN));
				input[0].mi.dy = y_ * (65536 / GetSystemMetrics(SM_CYSCREEN));
				input[0].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;

				input[1].type = INPUT_MOUSE;
				switch (button_)
				{
				case button::Right:
					input[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
					break;
				case button::Left:
					input[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
					break;
				case button::Middle:
					input[1].mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
					break;
				case button::X:
					input[1].mi.dwFlags = MOUSEEVENTF_XUP;
					break;
				}

				SendInput(static_cast<UINT>(input.size()), &input.front(), sizeof(INPUT));
			}
		};

		class wheel : public base
		{
			int value_;
		public:
			wheel(int value, int x, int y)
				: base(button::None, x, y), value_(value) {}
			wheel(const std::string& args)
			{
				std::istringstream iss(args);
				iss >> value_ >> std::ws >> x_ >> std::ws >> y_;
			}

			//std::string op() const { return ID::str[ID::MouseWheel]; }
			std::string op() const { return "mw"; }

			void inject() const
			{
				std::vector<INPUT> input(2);
				input[0].type = INPUT_MOUSE;
				input[0].mi.dx = x_ * (65536 / GetSystemMetrics(SM_CXSCREEN));
				input[0].mi.dy = y_ * (65536 / GetSystemMetrics(SM_CYSCREEN));
				input[0].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;

				input[1].type = INPUT_MOUSE;
				input[1].mi.dwFlags = MOUSEEVENTF_WHEEL;
				input[1].mi.mouseData = value_;

				SendInput(static_cast<UINT>(input.size()), &input.front(), sizeof(INPUT));

			}
		};

		class move : public base
		{
		public:
			move(int x, int y)
				: base(button::None, x, y) {}
			move(const std::string& args)
			{
				std::istringstream iss(args);
				iss >> x_ >> std::ws >> y_;
			}

			//std::string op() const { return ID::str[ID::MouseMove]; }
			std::string op() const { return "mm"; }

			void inject() const
			{
				std::vector<INPUT> input(1);
				input[0].type = INPUT_MOUSE;
				input[0].mi.dx = x_ * (65536 / GetSystemMetrics(SM_CXSCREEN));
				input[0].mi.dy = y_ * (65536 / GetSystemMetrics(SM_CYSCREEN));
				input[0].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;

				SendInput(static_cast<UINT>(input.size()), &input.front(), sizeof(INPUT));
			}
		};

		static action factory(const std::string& op, const std::string& args)
		{
			action newAction;

			if (op == "mm")
				newAction.reset(new move(args));
			else if (op == "md")
				newAction.reset(new down(args));
			else if (op == "mu")
				newAction.reset(new up(args));
			else if (op == "mw")
				newAction.reset(new wheel(args));
			
			return newAction;
		}

	}	// namespace mouse


	namespace keyboard
	{
		// these are the special keys
		enum key
		{
			None = 0,
			Enter,
			Cancel,
			Backspace,
			Tab,
			Shift,
			Control,
			Alt,
			Pause,
			Capslock,
			Escape,
			Space,
			PageUp,
			PageDown,
			End,
			Home,
			Left,
			Right,
			Up,
			Down,
			Select,
			Print,
			Insert,
			Delete,
			Help,
			

			CairoElephant

		};

		struct keyCode
		{
			keyCode(const std::string& str, unsigned char key)
				: str_(str), key_(key)
			{
			}

			std::string str_;
			unsigned char key_;
		};

		static const std::vector<keyCode> special({
			keyCode("", 0),
			keyCode("Enter", VK_RETURN),
			
			keyCode("CairoElephant", 0)
			});

		class base : public injectable
		{
		public:
			base(unsigned char k)
				: key_(k) {}
			base(const key& k)
				: key_(special[k].key_) {}

			std::string args() const
			{
				return std::to_string(key_);
			}

		protected:
			unsigned char getKey(const std::string& keySyntax)
			{
				unsigned char result;
				std::istringstream iss(keySyntax); iss >> result;
				return result;
			}
			unsigned char key_;
		};


		class down : public base
		{
		public:
			down(unsigned char k) : base(k) {}
			down(const key& k) : base(k) {}
			down(const std::string& args)
				: base(getKey(args))
			{
			}

			//std::string op() const { return ID::str[ID::KeyDown]; }
			std::string op() const { return "kd"; }

			void inject() const
			{
				byte vk = VkKeyScan((char)key_);
				unsigned short scanCode = MapVirtualKey(vk, 0);

				INPUT input;
				input.type = INPUT_KEYBOARD;
				input.ki.wScan = static_cast<unsigned short>(scanCode & 0xff);
				input.ki.dwFlags = KEYEVENTF_SCANCODE;

				SendInput(1, &input, sizeof(INPUT));
			}
		};

		class up : public base
		{
		public:
			up(unsigned char k) : base(k) {}
			up(const key& k) : base(k) {}
			up(const std::string& args)
				: base(getKey(args))
			{
			}

			//std::string op() const { return ID::str[ID::KeyUp]; }
			std::string op() const { return "ku"; }

			void inject() const
			{
				byte vk = VkKeyScan((char)key_);
				unsigned short scanCode = MapVirtualKey(vk, 0);

				INPUT input;
				input.type = INPUT_KEYBOARD;
				input.ki.wScan = static_cast<unsigned short>(scanCode & 0xff); 
				input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;

				SendInput(1, &input, sizeof(INPUT));
			}
		};

		class press : public base
		{
		public:
			press(unsigned char k) : base(k) {}
			press(const key& k) : base(k) {}
			press(const std::string& args)
				: base(getKey(args))
			{
			}
			std::string op() const { return "kp"; }

			void inject() const
			{
				keyboard::down(key_).inject();
				program::wait(10).inject();
				keyboard::up(key_).inject();
			}

		};

		class type : public base
		{
			std::string str_;
		public:
			type(const std::string& str) : base(key::None), str_(str) {}
			
			std::string op() const { return "kt"; }

			void inject() const
			{
				std::for_each(str_.begin(), str_.end(), [](char c) 
				{
					keyboard::press(c).inject();
					program::wait w(20);
				});
			}

		};

		static action factory(const std::string& op, const std::string& args)
		{
			action newAction;

			if (op == "ku")
				newAction.reset(new up(args));
			else if (op == "kd")
				newAction.reset(new down(args));
			else if (op == "kt")
				newAction.reset(new press(args));
			else if (op == "type")
				newAction.reset(new type(args));
			
			return newAction;
		}

	}	// keyboard

	
	class script
	{

		std::string rtrim(const std::string& str)
		{
			std::size_t itr = str.find_last_not_of(' ');
			return itr != std::string::npos ? str.substr(0, itr + 1) : str;
		}
		std::string ltrim(const std::string& str)
		{
			std::size_t itr = str.find_first_not_of(' ');
			return itr != std::string::npos ? str.substr(itr) : str;
		}
		std::string trim(const std::string& str)
		{
			return rtrim(ltrim(str));
		}
		std::string removeLineEnds(const std::string& str)
		{
			std::string syntax = str;
			syntax.erase(std::remove(syntax.begin(), syntax.end(), '\n'), syntax.end());
			return syntax;
		}

		action parseActionSyntax(const std::string& actionSyntax)
		{
			std::size_t argsStart = actionSyntax.find("{");
			std::size_t argsEnd = actionSyntax.find("}");

			if (argsStart == std::string::npos || argsEnd == std::string::npos || argsStart > argsEnd)
				throw std::exception("args syntax error.");
			++argsStart;
			std::string args = trim(actionSyntax.substr(argsStart, argsEnd - argsStart));
			std::string op = trim(actionSyntax.substr(0, argsStart - 1));

			action result = program::factory(op, args);
			if (!result)
				result = keyboard::factory(op, args);
			if (!result)
				result = mouse::factory(op, args);
			if (!result)
				throw std::exception("Unknown action syntax.");

			return result;
		}

		std::list<action> actions_;
		
	public:

		script(const std::string& scriptSyntax)
		{
			std::string str = removeLineEnds(scriptSyntax);

			std::size_t start = 0;
			std::size_t end = str.find(",");

			while (end != std::string::npos)
			{
				action newAction = parseActionSyntax(trim(str.substr(start, end - start)));
				if (newAction)
					actions_.push_back(newAction);

				start = end + 1;
				end = str.find(",", start);
			}

			// parse the last command token...
			action lastAction = parseActionSyntax(trim(str.substr(start, end - (start))));
			if (lastAction)
				actions_.push_back(lastAction);
		}

		script()
		{
		}

		void play()
		{
			// check first action is program::exec...
			//if ( )

			std::for_each(actions_.begin(), actions_.end(), [](action a) { a->inject(); });

			// return when finished...
		}

		void wait()
		{

		}

		std::string syntax()
		{
			std::ostringstream oss;
			for (std::list<action>::const_iterator itr = actions_.begin(); itr != actions_.end(); ++itr)
				oss << (*itr)->syntax() << ",";
			return oss.str();
		}

		void add(action a)
		{
			actions_.push_back(a);
		}
	
	};

	namespace record
	{
		namespace impl
		{
			static HHOOK mouseHookID_;
			static HHOOK keyboardHookID_;
			static std::list<ghost::script*> listeners_;

			static LRESULT MouseHookCallback(int nCode, WPARAM wParam, LPARAM lParam)
			{
				// If we support this event, 
				if (nCode >= 0)
				{
					action thisAction;
					POINT pt;
					GetCursorPos(&pt);
					if (wParam == WM_MOUSEMOVE)
						thisAction.reset(new mouse::move(static_cast<unsigned int>(pt.x), static_cast<unsigned int>(pt.y)));
					else if (wParam == WM_RBUTTONDOWN)
						thisAction.reset(new mouse::down(mouse::button::Right, static_cast<unsigned int>(pt.x), static_cast<unsigned int>(pt.y)));
					else if (wParam == WM_LBUTTONDOWN)
						thisAction.reset(new mouse::down(mouse::button::Left, static_cast<unsigned int>(pt.x), static_cast<unsigned int>(pt.y)));
					else if (wParam == WM_RBUTTONUP)
						thisAction.reset(new mouse::up(mouse::button::Right, static_cast<unsigned int>(pt.x), static_cast<unsigned int>(pt.y)));
					else if (wParam == WM_LBUTTONUP)
						thisAction.reset(new mouse::up(mouse::button::Left, static_cast<unsigned int>(pt.x), static_cast<unsigned int>(pt.y)));
					else if (wParam == WM_MBUTTONDOWN)
						thisAction.reset(new mouse::down(mouse::button::Middle, static_cast<unsigned int>(pt.x), static_cast<unsigned int>(pt.y)));
					else if (wParam == WM_MBUTTONUP)
						thisAction.reset(new mouse::up(mouse::button::Middle, static_cast<unsigned int>(pt.x), static_cast<unsigned int>(pt.y)));
					else if (wParam == WM_MOUSEWHEEL)
						thisAction.reset(new mouse::wheel(0, static_cast<unsigned int>(pt.x), static_cast<unsigned int>(pt.y)));

#ifdef GHOST_ENABLE_MESSAGES
					if (ghost::messageCallback)
						ghost::messageCallback("ghost hook: " + thisAction->syntax());
#endif

					if (thisAction)
						std::for_each(listeners_.begin(), listeners_.end(), [&thisAction](ghost::script* s) { s->add(thisAction); });
				}

				return CallNextHookEx(mouseHookID_, nCode, wParam, lParam);
			}
			static LRESULT KeyboardHookCallback(int nCode, WPARAM wParam, LPARAM lParam)
			{
				if (nCode >= 0)
				{
					action thisAction;
					if (wParam == WM_KEYDOWN)
						thisAction.reset(new keyboard::down(static_cast<unsigned char>(lParam)));
					else if (wParam == WM_KEYUP)
						thisAction.reset(new keyboard::up(static_cast<unsigned char>(lParam)));

#ifdef GHOST_ENABLE_MESSAGES
					if (ghost::messageCallback)
						ghost::messageCallback("ghost hook: " + thisAction->syntax());
#endif

					if (thisAction)
						std::for_each(listeners_.begin(), listeners_.end(), [&thisAction](ghost::script* s) { s->add(thisAction); });
				}
				
				return CallNextHookEx(keyboardHookID_, nCode, wParam, lParam);
			}
		}

		static ghost::script script(const program::exec& cmd)
		{
			ghost::script result;
			action thisProgram(new program::exec(cmd.args()));
			program::exec& prog = static_cast<program::exec&>(*thisProgram);

			result.add(thisProgram);

			prog.run();

			impl::listeners_.push_back(&result);

			impl::mouseHookID_ = SetWindowsHookExW(WH_MOUSE, (HOOKPROC)impl::MouseHookCallback, (HINSTANCE)prog.handle(), prog.threadID());
			impl::keyboardHookID_ = SetWindowsHookExW(WH_KEYBOARD, (HOOKPROC)impl::KeyboardHookCallback, (HINSTANCE)prog.handle(), prog.threadID());

			prog.wait();
			
			std::list<ghost::script*>::iterator itr = std::find(impl::listeners_.begin(), impl::listeners_.end(), &result);
			if (itr != impl::listeners_.end())
				impl::listeners_.erase(itr);
			
			// unhook the callbacks...
			UnhookWindowsHookEx(impl::mouseHookID_);
			UnhookWindowsHookEx(impl::keyboardHookID_);

			return result;
		}



	}

	static void inject(const ghost::injectable& action)
	{
#ifdef GHOST_ENABLE_MESSAGES
		if (ghost::messageCallback)
			ghost::messageCallback("ghost inject: " + action.syntax());
#endif
		action.inject();
	}
}

#endif // GHOST_HPP