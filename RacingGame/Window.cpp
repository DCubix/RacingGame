#include "Window.h"

#include <windowsx.h>

#define WGL_CONTEXT_DEBUG_BIT_ARB 0x00000001
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_FLAGS_ARB 0x2094
#define ERROR_INVALID_VERSION_ARB 0x2095
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define ERROR_INVALID_PROFILE_ARB 0x2096
#define WGL_SAMPLE_BUFFERS_ARB 0x2041
#define WGL_SAMPLES_ARB 0x2042
#define WGL_DOUBLE_BUFFER_ARB 0x2011
#define WGL_COLOR_BITS_ARB 0x2014
#define WGL_RED_BITS_ARB 0x2015
#define WGL_GREEN_BITS_ARB 0x2017
#define WGL_BLUE_BITS_ARB 0x2019
#define WGL_ALPHA_BITS_ARB 0x201B
#define WGL_DEPTH_BITS_ARB 0x2022
#define WGL_STENCIL_BITS_ARB 0x2023

#ifndef APIENTRY
#define APIENTRY
#endif

typedef HGLRC(APIENTRY* PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC hDC, HGLRC hShareContext, const int *attribList);
static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;

u32 Window::WID = 0;

Window::Window(const std::string& title, u32 width, u32 height, ContextAttribs attribs) {
	const std::string className = std::string("GLTW_") + std::to_string(WID++);
	HINSTANCE i = GetModuleHandle(nullptr);

	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = &Window::WndProc;
	wc.hInstance = i;
	wc.hCursor = LoadCursor(i, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOWTEXT);
	wc.lpszClassName = className.c_str();

	if (!RegisterClassEx(&wc)) {
		return;
	}

	m_handle = CreateWindowEx(
		WS_EX_ACCEPTFILES,
		className.c_str(),
		title.c_str(),
		(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX),
		CW_USEDEFAULT, CW_USEDEFAULT,
		width, height,
		nullptr,
		nullptr,
		i,
		nullptr
	);

	if (!m_handle) {
		return;
	}

	setState(WindowState::Shown);

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = attribs.pixelFormat.depthBits;
	pfd.cStencilBits = attribs.pixelFormat.stencilBits;
	pfd.iLayerType = PFD_MAIN_PLANE;

	m_hdc = GetDC(m_handle);
	int cpfm = ChoosePixelFormat(m_hdc, &pfd);
	SetPixelFormat(m_hdc, cpfm, &pfd);

	HGLRC tempContext = wglCreateContext(m_hdc);
	wglMakeCurrent(m_hdc, tempContext);

	const int gl_attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, attribs.openGL.majorVersion,
		WGL_CONTEXT_MINOR_VERSION_ARB, attribs.openGL.minorVersion,
		WGL_COLOR_BITS_ARB, 32,
		WGL_DEPTH_BITS_ARB, attribs.pixelFormat.depthBits,
		WGL_STENCIL_BITS_ARB, attribs.pixelFormat.stencilBits,
		WGL_RED_BITS_ARB, attribs.pixelFormat.redBits,
		WGL_GREEN_BITS_ARB, attribs.pixelFormat.greenBits,
		WGL_BLUE_BITS_ARB, attribs.pixelFormat.blueBits,
		WGL_ALPHA_BITS_ARB, attribs.pixelFormat.alphaBits,
		WGL_SAMPLES_ARB, attribs.multisample.samples,
		WGL_SAMPLE_BUFFERS_ARB, attribs.multisample.buffers,
		WGL_DOUBLE_BUFFER_ARB, attribs.doublebuffer ? 1 : 0,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		WGL_CONTEXT_FLAGS_ARB, 0,
		0
	};
	if (wglCreateContextAttribsARB == nullptr)
		wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) intern::loadFunction("wglCreateContextAttribsARB");
	m_glContext = wglCreateContextAttribsARB(m_hdc, nullptr, gl_attribs);
	wglMakeCurrent(nullptr, nullptr);
	wglDeleteContext(tempContext);
	wglMakeCurrent(m_hdc, m_glContext);

	//
	m_className = className;
	m_width = width;
	m_height = height;
	m_shouldClose = false;
}

Window::~Window() {
	wglMakeCurrent(nullptr, nullptr);
	wglDeleteContext(m_glContext);
	DestroyWindow(m_handle);
}

bool Window::shouldClose() {
	processEvents();
	return m_shouldClose;
}

void Window::processEvents() {
	MSG msg = { 0 };
	m_eventQueue.clear();
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		Event evt{};
		switch (msg.message) {
			case WM_MOUSEMOVE:
			{
				evt.type = EventType::MouseMove;
				evt.mouseX = GET_X_LPARAM(msg.lParam);
				evt.mouseY = GET_Y_LPARAM(msg.lParam);
			} break;
			case WM_LBUTTONDOWN:
			{
				evt.type = EventType::MouseButtonDown;
				evt.mouseButton = MouseButton::LeftButton;
				evt.mouseX = GET_X_LPARAM(msg.lParam);
				evt.mouseY = GET_Y_LPARAM(msg.lParam);
			} break;
			case WM_LBUTTONUP:
			{
				evt.type = EventType::MouseButtonUp;
				evt.mouseButton = MouseButton::LeftButton;
				evt.mouseX = GET_X_LPARAM(msg.lParam);
				evt.mouseY = GET_Y_LPARAM(msg.lParam);
			} break;
			case WM_RBUTTONDOWN:
			{
				evt.type = EventType::MouseButtonDown;
				evt.mouseButton = MouseButton::RightButton;
				evt.mouseX = GET_X_LPARAM(msg.lParam);
				evt.mouseY = GET_Y_LPARAM(msg.lParam);
			} break;
			case WM_RBUTTONUP:
			{
				evt.type = EventType::MouseButtonUp;
				evt.mouseButton = MouseButton::RightButton;
				evt.mouseX = GET_X_LPARAM(msg.lParam);
				evt.mouseY = GET_Y_LPARAM(msg.lParam);
			} break;
			case WM_MBUTTONDOWN:
			{
				evt.type = EventType::MouseButtonDown;
				evt.mouseButton = MouseButton::MiddleButton;
				evt.mouseX = GET_X_LPARAM(msg.lParam);
				evt.mouseY = GET_Y_LPARAM(msg.lParam);
			} break;
			case WM_MBUTTONUP:
			{
				evt.type = EventType::MouseButtonUp;
				evt.mouseButton = MouseButton::MiddleButton;
				evt.mouseX = GET_X_LPARAM(msg.lParam);
				evt.mouseY = GET_Y_LPARAM(msg.lParam);
			} break;
			case WM_MOUSEWHEEL:
			{
				evt.type = EventType::MouseWheel;
				evt.mouseWheel = GET_WHEEL_DELTA_WPARAM(msg.wParam) / WHEEL_DELTA;
			} break;
			case WM_KEYDOWN:
			{
				evt.type = EventType::KeyDowm;
				evt.key = (Key) msg.wParam;
			} break;
			case WM_KEYUP:
			{
				evt.type = EventType::KeyUp;
				evt.key = (Key) msg.wParam;
			} break;
			case WM_QUIT:
			{
				evt.type = EventType::Quit;
				m_shouldClose = true;
			} break;
		}

		m_eventQueue.push_back(evt);
	}
}

bool Window::popEvent(Event& evt) {
	if (m_eventQueue.empty())
		return false;

	Event ef = m_eventQueue.front();
	evt.key = ef.key;
	evt.mouseButton = ef.mouseButton;
	evt.mouseWheel = ef.mouseWheel;
	evt.mouseX = ef.mouseX;
	evt.mouseY = ef.mouseY;
	evt.type = ef.type;
		
	m_eventQueue.pop_front();
	return true;
}

void Window::update() {
	UpdateWindow(m_handle);
}

void Window::setState(WindowState state) {
	ShowWindow(m_handle, int(state));
	update();
}

void Window::swapBuffers() {
	SwapBuffers(m_hdc);
}

void Window::title(std::string title) {
	SetWindowText(m_handle, title.c_str());
}

std::string Window::title() {
	char _txt[256] = { 0 };
	GetWindowText(m_handle, _txt, 256);
	return std::string(_txt);
}

void Window::resize(u32 newWidth, u32 newHeight) {
	SetWindowPos(m_handle, nullptr, 0, 0, newWidth, newHeight, SWP_NOMOVE);
	m_width = newWidth;
	m_height = newHeight;
}

LRESULT Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_CLOSE:
		case WM_DESTROY: PostQuitMessage(0); break;
		default: return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

namespace intern {
	static HMODULE libGL = nullptr;

	FuncPtr loadFunction(const char* name) {
		FuncPtr p = (FuncPtr) wglGetProcAddress(name);
		if (p == nullptr ||
			(p == (void*) 0x1) || (p == (void*) 0x2) || (p == (void*) 0x3) ||
			(p == (void*) -1))
		{
			if (libGL == nullptr) {
				libGL = LoadLibraryA("opengl32.dll");
			}
			p = (FuncPtr) GetProcAddress(libGL, name);
		}
		return p;
	}
}
