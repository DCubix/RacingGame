#ifndef WINDOW_H
#define WINDOW_H

#include <Windows.h>
#include <string>
#include <deque>
#include <unordered_map>

#pragma comment(lib, "opengl32.lib")

using i32 = int;
using u32 = unsigned int;

struct ContextAttribs {
	struct PixelFormat {
		u32 depthBits, stencilBits;
		u32 redBits, greenBits, blueBits, alphaBits;
	} pixelFormat;

	struct Multisample {
		u32 samples, buffers;
	} multisample;

	struct OpenGL {
		u32 majorVersion, minorVersion;
	} openGL;

	bool doublebuffer;
};
const ContextAttribs DEFAULT_ATTRIBS = { { 24, 8, 8, 8, 8, 8 }, { 0, 0 }, { 3, 3 }, true };

enum EventType {
	NullType = 0,
	MouseButtonDown,
	MouseButtonUp,
	MouseMove,
	MouseWheel,
	KeyDowm,
	KeyUp,
	Quit
};

enum MouseButton {
	NullButton = 0,
	LeftButton,
	RightButton,
	MiddleButton
};

#pragma region Key Codes
enum Key {
	NullKey = 0,
	Cancel = VK_CANCEL,
	Back = VK_BACK,
	Tab = VK_TAB,
	Clear = VK_CLEAR,
	Enter = VK_RETURN,
	LShift = VK_LSHIFT,
	RShift = VK_RSHIFT,
	LControl = VK_LCONTROL,
	RControl = VK_RCONTROL,
	LAlt = VK_LMENU,
	RAlt = VK_RMENU,
	Pause = VK_PAUSE,
	CapsLock = VK_CAPITAL,
	Esc = VK_ESCAPE,
	Space = VK_SPACE,
	PgUp = VK_PRIOR,
	PgDown = VK_NEXT,
	End = VK_END,
	Home = VK_HOME,
	Left = VK_LEFT,
	Right = VK_RIGHT,
	Up = VK_UP,
	Down = VK_DOWN,
	Select = VK_SELECT,
	PrtScr = VK_SNAPSHOT,
	Insert = VK_INSERT,
	Delete = VK_DELETE,
	Help = VK_HELP,
	LWin = VK_LWIN,
	RWin = VK_RWIN,
	Suspend = VK_SLEEP,
	Multiply = VK_MULTIPLY,
	Add = VK_ADD,
	Subtract = VK_SUBTRACT,
	Divide = VK_DIVIDE,
	Separator = VK_SEPARATOR,
	Decimal = VK_DECIMAL,
	NumLock = VK_NUMLOCK,
	ScrollLock = VK_SCROLL,
	Key0 = 0x30,
	Key1 = 0x31,
	Key2 = 0x32,
	Key3 = 0x33,
	Key4 = 0x34,
	Key5 = 0x35,
	Key6 = 0x36,
	Key7 = 0x37,
	Key8 = 0x38,
	Key9 = 0x39,
	KeyA = 0x41,
	KeyB = 0x42,
	KeyC = 0x43,
	KeyD = 0x44,
	KeyE = 0x45,
	KeyF = 0x46,
	KeyG = 0x47,
	KeyH = 0x48,
	KeyI = 0x49,
	KeyJ = 0x4A,
	KeyK = 0x4B,
	KeyL = 0x4C,
	KeyM = 0x4D,
	KeyN = 0x4E,
	KeyO = 0x4F,
	KeyP = 0x50,
	KeyQ = 0x51,
	KeyR = 0x52,
	KeyS = 0x53,
	KeyT = 0x54,
	KeyU = 0x55,
	KeyV = 0x56,
	KeyW = 0x57,
	KeyX = 0x58,
	KeyY = 0x59,
	KeyZ = 0x5A,
	Numpad0 = 0x60,
	Numpad1 = 0x61,
	Numpad2 = 0x62,
	Numpad3 = 0x63,
	Numpad4 = 0x64,
	Numpad5 = 0x65,
	Numpad6 = 0x66,
	Numpad7 = 0x67,
	Numpad8 = 0x68,
	Numpad9 = 0x69,
	F1 = 0x70,
	F2 = 0x71,
	F3 = 0x72,
	F4 = 0x73,
	F5 = 0x74,
	F6 = 0x75,
	F7 = 0x76,
	F8 = 0x77,
	F9 = 0x78,
	F10 = 0x79,
	F11 = 0x7A,
	F12 = 0x7B,
	F13 = 0x7C,
	F14 = 0x7D,
	F15 = 0x7E,
	F16 = 0x7F,
	F17 = 0x80,
	F18 = 0x81,
	F19 = 0x82,
	F20 = 0x83,
	F21 = 0x84,
	F22 = 0x85,
	F23 = 0x86,
	F24 = 0x87
};
#pragma endregion

struct Event {
	EventType type;
	MouseButton mouseButton;
	Key key;
	int mouseX, mouseY;
	int mouseWheel;
};

class Window {
	friend class Direct3D;
public:
	enum WindowState {
		Shown = SW_SHOW,
		Hidden = SW_HIDE,
		Minimized = SW_MINIMIZE,
		Maximized = SW_MAXIMIZE,
		Restore = SW_RESTORE
	};

	Window() = default;
	Window(const std::string& title, u32 width, u32 height, ContextAttribs attribs = DEFAULT_ATTRIBS);
	~Window();

	u32 width() const { return m_width; }
	u32 height() const { return m_height; }

	bool shouldClose();
	bool popEvent(Event& evt);
	void setState(WindowState state);
	void swapBuffers();

	void title(std::string title);
	std::string title();

	void resize(u32 newWidth, u32 newHeight);

protected:
	HDC m_hdc;
	HGLRC m_glContext;
	HWND m_handle;
	std::string m_className;

	void update();
	void processEvents();

private:
	u32 m_width, m_height;
	bool m_shouldClose;
	std::deque<Event> m_eventQueue;

	static LRESULT CALLBACK WndProc(HWND hWnd,
									UINT msg,
									WPARAM wParam,
									LPARAM lParam);

	static u32 WID;
};

namespace intern {
	using FuncPtr = void*;
	FuncPtr loadFunction(const char* name);
}

#endif // WINDOW_H