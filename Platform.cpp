#include "Platform.h"

vertexObject createVO(modelData data){
	vertexObject vO;
	vO.data = data;

	glGenBuffers(1, &vO.vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vO.vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, vO.data.vertexBytes, vO.data.vertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vO.indexBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vO.indexBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,vO. data.indexSize*sizeof(vO.data.indexData[0]), vO.data.indexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &vO.vertexArrayObject);

	vO = bindVO(vO);
	glBindVertexArray(0);

	return vO;
}

GLuint createShader(const GLenum eShaderType, const char * shaderSource){
    GLuint shader = glCreateShader(eShaderType);

    glShaderSource(shader, 1, &shaderSource, NULL);

    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar *infoLog = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog);
        const char *shaderType = NULL;
        switch(eShaderType){
			case GL_VERTEX_SHADER: shaderType = "vertex"; break;
			case GL_GEOMETRY_SHADER: shaderType = "geometry"; break;
			case GL_FRAGMENT_SHADER: shaderType = "fragment"; break;
        }
        fprintf(stderr, "Compile failure in %s shader:\n%s\n", shaderType, infoLog);
        delete[] infoLog;
    }

    return shader;
}

GLuint createProgram(shaderInfo * shaderList, unsigned nShaders){
	GLuint * shaders = new GLuint[nShaders];
	for(unsigned s = 0; s < nShaders; s++){
		shaders[s] = createShader(shaderList[s].type, shaderList[s].source);
	}

    GLuint program;

	program = glCreateProgram();

	for(unsigned i = 0; i < nShaders; i++){
		glAttachShader(program, shaders[i]);
	}

	glLinkProgram(program);

	GLint status;
	glGetProgramiv (program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *infoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, infoLog);
		fprintf(stderr, "Linker failure: %s\n", infoLog);
		delete[] infoLog;
	}

	for(unsigned i = 0; i < nShaders; i++){
		glDetachShader(program, shaders[i]);
		glDeleteShader(shaders[i]);
	}
	return program;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg) { 
		case WM_INPUT: {
			RAWINPUT * raw;
			{//initialize raw
				BYTE buffer[sizeof(RAWINPUT)] = {};
				size_t size = sizeof(RAWINPUT);

				GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buffer, &size, sizeof(RAWINPUTHEADER));

				raw = (RAWINPUT*) buffer;
			}

			if (raw->header.dwType == RIM_TYPEKEYBOARD) {
				const RAWKEYBOARD kb = raw->data.keyboard;
					
				if(kb.Flags == RI_KEY_MAKE){
					input::dopress(kb.VKey);
				}
				else if(kb.Flags == RI_KEY_BREAK){
					input::unpress(kb.VKey);
				}

				if(input::pressed(VK_ESCAPE)){
					DestroyWindow(hwnd);
				}
			}
			if (raw->header.dwType == RIM_TYPEMOUSE) {
				const RAWMOUSE ms = raw->data.mouse;
					
				if(ms.usFlags == MOUSE_MOVE_RELATIVE){
					input::moveMouse(ms.lLastX, ms.lLastY);
				}

				if(ms.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN){//usButtonFlags can have more than one input at a time
					input::dopress(VK_LBUTTON);
				}
				else if(ms.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP){
					input::unpress(VK_LBUTTON);
				}

				if(ms.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN){//usButtonFlags can have more than one input at a time
					input::dopress(VK_RBUTTON);
				}
				else if(ms.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP){
					input::unpress(VK_RBUTTON);
				}

				raw->header.dwType = 1;
			}
		}
		break;
		case WM_CLOSE:
			DestroyWindow(hwnd);
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
		break;
		case WM_SIZE:
			glViewport(0, 0, LOWORD(lParam),HIWORD(lParam));
			break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

//from http://stackoverflow.com/questions/589064/how-to-enable-vertical-sync-in-opengl
bool WGLExtensionSupported(const char *extension_name) {
	// this is pointer to function which returns pointer to string with list of all wgl extensions
	PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = NULL;

	// determine pointer to wglGetExtensionsStringEXT function
	_wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC) wglGetProcAddress("wglGetExtensionsStringEXT");

	if (strstr(_wglGetExtensionsStringEXT(), extension_name) == NULL)
	{
		// string was not found
		return false;
	}

	// extension is supported
	return true;
}
//endfrom

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	char className[] = "class name";

	WNDCLASSEX wc;
	{//init the window class
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_OWNDC;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = LoadIcon(0, IDI_APPLICATION);
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
		wc.lpszMenuName = 0;
		wc.lpszClassName = className;
		wc.hIconSm = LoadIcon(0, IDI_APPLICATION);

		if(!RegisterClassEx(&wc)) {
			MessageBox(0, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
			return 0;
		}
	}

	if(fullscreen){
		DEVMODE dmScreenSettings;	
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));       // Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);       // Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth    = resolution[0];            // Selected Screen Width
		dmScreenSettings.dmPelsHeight   = resolution[1];           // Selected Screen Height
		dmScreenSettings.dmBitsPerPel   = 32;             // Selected Bits Per Pixel
		dmScreenSettings.dmFields = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		if(ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL){
			return -1;
		}
	}

	HWND hwnd;
	{//init hwnd
		hwnd = CreateWindowEx(
			WS_EX_APPWINDOW,
			className,//the class name
			"The... TITLE!",//The title
			(fullscreen ? WS_POPUP : WS_OVERLAPPEDWINDOW) | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			0, 0, resolution[0], resolution[1],//position and size
			0, 0, hInstance, 0
		);
		if(hwnd == 0) {
			MessageBox(0, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
			return 0;
		}
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	{//GLEW
		HDC dummyDc;
		{
			dummyDc = GetDC(hwnd);

			PIXELFORMATDESCRIPTOR pfd =
			{
				sizeof(PIXELFORMATDESCRIPTOR),
				1,
				PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
				PFD_TYPE_RGBA,            //The kind of framebuffer. RGBA or palette.
				32,                        //Colordepth of the framebuffer.
				0, 0, 0, 0, 0, 0,
				0,
				0,
				0,
				0, 0, 0, 0,
				32,                        //Number of bits for the depthbuffer
				0,                        //Number of bits for the stencilbuffer
				0,                        //Number of Aux buffers in the framebuffer.
				PFD_MAIN_PLANE,
				0,
				0, 0, 0
			};

			const int pf = ChoosePixelFormat(dummyDc, &pfd);
			SetPixelFormat(dummyDc, pf, &pfd);
		}
		HGLRC glrc = wglCreateContext(dummyDc);

		wglMakeCurrent(dummyDc, glrc);

		glewExperimental = true;
		GLenum err=glewInit();
		if(err!=GLEW_OK) {
			DestroyWindow(hwnd);
		}

		wglMakeCurrent(0, 0);
		wglDeleteContext(glrc);
	}

	HDC dc;
	{
		dc = GetDC(hwnd);

		const int attribList[] = {
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB, 32,
			WGL_DEPTH_BITS_ARB, 32,
			WGL_STENCIL_BITS_ARB, 0,
			0,        //End
		};

		int pixFormat;
		UINT numFormats;

		wglChoosePixelFormatARB(dc, attribList, 0, 1, &pixFormat, &numFormats);

		PIXELFORMATDESCRIPTOR pfd;
		SetPixelFormat(dc, pixFormat, &pfd);
        
		int attribs[] = {
                WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
                WGL_CONTEXT_MINOR_VERSION_ARB, 1,
                WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
                0
        };

		HGLRC glrc = wglCreateContextAttribsARB(dc, 0, attribs);

		wglMakeCurrent(dc, glrc);
	}

	{//create raw input device
		RAWINPUTDEVICE rid[2];

		//mouse
		rid[0].usUsagePage = 0x01; 
		rid[0].usUsage = 0x02;
		rid[0].dwFlags = RIDEV_NOLEGACY;
		rid[0].hwndTarget = 0;

		//keyboard
		rid[1].usUsagePage = 0x01; 
		rid[1].usUsage = 0x06; 
		rid[1].dwFlags = RIDEV_NOLEGACY;
		rid[1].hwndTarget = 0;
		
		if(RegisterRawInputDevices(rid, sizeof(rid)/sizeof(rid[0]), sizeof(rid[0])) == 0){
			return -1;
		}
	}

	SetCursor(0);//disable cursor
	RECT r;
	GetWindowRect( hwnd, &r);
	ClipCursor(&r);
	
	GLuint program;
	{//graphics
		shaderInfo shaderList[2] = {
			GL_VERTEX_SHADER, files::getString("shaders/Vertex.vert"),
			GL_FRAGMENT_SHADER, files::getString("shaders/Fragment.frag"),
		};

		program = createProgram(shaderList, sizeof(shaderList)/sizeof(shaderList[0]));

		//from http://stackoverflow.com/questions/589064/how-to-enable-vertical-sync-in-opengl
		PFNWGLSWAPINTERVALEXTPROC       wglSwapIntervalEXT = NULL;
		PFNWGLGETSWAPINTERVALEXTPROC    wglGetSwapIntervalEXT = NULL;

		if (WGLExtensionSupported("WGL_EXT_swap_control"))
		{
			// Extension is supported, init pointers.
			wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress("wglSwapIntervalEXT");

			// this is another function from WGL_EXT_swap_control extension
			wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC) wglGetProcAddress("wglGetSwapIntervalEXT");
		}
		//endfrom

		wglSwapIntervalEXT(1);//TODO: temp
		
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LEQUAL);
		glDepthRange(0.0f, 1.0f);
		glEnable(GL_DEPTH_CLAMP);
		glViewport(0, 0, resolution[0], resolution[1]);
	}

	vertexObject vOs[] = {
		createVO(modelData()),
		createVO(files::getVertexData("models/crosshair.ply")),
		createVO(files::getVertexData("models/person.ply")),
		createVO(files::getVertexData("models/bullet.ply")),
	};

	LARGE_INTEGER oldTime;
	LARGE_INTEGER curTime;
	LARGE_INTEGER frq;
	{
		QueryPerformanceFrequency(&frq);
		QueryPerformanceCounter(&curTime);
		oldTime = curTime;
	}

	world w = createWorld();

	MSG msg;
	
	do {
		if(input::pressed('O')){
			wglSwapIntervalEXT(1);
		}
		if(input::pressed('P')){
			wglSwapIntervalEXT(0);
		}

		QueryPerformanceCounter(&curTime);
		unsigned loops = 1;
		
		float dt = (float)(curTime.QuadPart - oldTime.QuadPart)/(float)(frq.QuadPart*loops);
		for(unsigned i = 0; i < loops; i++){
			worldLoop(w, dt);
		}
		oldTime = curTime;

		{//render stuff
			glClearColor(0.7f, 0.8f, 1.0f, 1.0f);
			glClearDepth(1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUseProgram(program);
			GLuint trans = glGetUniformLocation(program, "t");

			//<temp>
			renderObject(scale(w.cam.r, -1.0), float2(10000.0, 0.0), modId_crosshair, vOs, trans);
			//<\temp>

			renderWorld(w, vOs, trans);

			SwapBuffers(dc);
		}

		while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

	} while(msg.message != WM_QUIT);

	return msg.wParam;
}