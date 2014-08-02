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
			460, 100, resolution[0], resolution[1],//position and size
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

	LPDIRECTSOUND8 ds_face;
	LPDIRECTSOUNDBUFFER dsb;
	LPDIRECTSOUNDBUFFER8 dsb8;

	{//create sound device
		auto err = DirectSoundCreate8(0, &ds_face, 0);
		if(err != DS_OK){
			return err;
		}

		err = ds_face->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
		if(err != DS_OK){
			return err;
		}

		WAVEFORMATEX wft = {
			WAVE_FORMAT_PCM, //wFormatTag
			1, //nChannels
			44100, //nSamplesPerSec
			88200, //nAvgBytesPerSex = nSamplesPerSec*nBlockAlign = nSamplesPerSec*nChannels*wBitsPerSample/8
			2, //nBlockAlign = nChannels*wBitsPerSample/8
			16, //wBitsPerSample
			0, //cbSize
		};

		DSBUFFERDESC dsb_desc = {
			sizeof(DSBUFFERDESC),
			DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLPOSITIONNOTIFY|DSBCAPS_GETCURRENTPOSITION2|DSBCAPS_GLOBALFOCUS,
			DSBSIZE_MAX,
			0,//reserved
			&wft,
			GUID_NULL,
		};
		
		HRESULT buff_err = ds_face->CreateSoundBuffer(&dsb_desc, &dsb, 0);
		if(buff_err != DS_OK && buff_err != DS_NO_VIRTUALIZATION){
			return buff_err;
		}

		dsb->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*) &dsb8);
		
		LPDWORD size0 = (LPDWORD)53036;
		char * snd0 = new char[53036];
		
		LPDWORD size1 = (LPDWORD)0;
		char * snd1 = new char[53036];

		err = dsb8->Lock(0, 53036, (LPVOID *) snd0, size0, (LPVOID *) snd1, size1, 0);
		if(err != DS_OK){
			return err;
		}
		files::read("sounds/Meaty_Deaths_2.wav", snd0);
		dsb8->Unlock((LPVOID *) snd0, *size0, (LPVOID *) snd1, *size1);

		dsb8->SetCurrentPosition(0);
		dsb8->SetVolume(10000);
		dsb8->Play(0, 0, DSBPLAY_LOOPING);
	}

	SetCursor(0);//disable cursor
	RECT r;
	GetWindowRect( hwnd, &r);
	ClipCursor(&r);
	
	GLuint program;
	GLuint post_program;
	{//graphics
		shaderInfo shaderList[2] = {
			GL_VERTEX_SHADER, files::getString("shaders/Vertex.vert"),
			GL_FRAGMENT_SHADER, files::getString("shaders/Fragment.frag"),
		};

		program = createProgram(shaderList, sizeof(shaderList)/sizeof(shaderList[0]));

		//post proc stuff
		shaderInfo post_shaderList[2] = {
			GL_VERTEX_SHADER, files::getString("shaders/VertexPost.vert"),
			GL_FRAGMENT_SHADER, files::getString("shaders/FragmentPost.frag"),
		};

		post_program = createProgram(post_shaderList, sizeof(post_shaderList)/sizeof(post_shaderList[0]));
		//end post proc stuff

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

		wglSwapIntervalEXT(0);//TODO: temp
		
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_GEQUAL);
		glDepthRange(0.0f, 1.0f);
		glEnable(GL_DEPTH_CLAMP);
		glViewport(0, 0, resolution[0], resolution[1]);
	}

	world w = createWorld();

	GLuint vbo_fbo_vertices;

	GLfloat fbo_vertices[] = {
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		-1.0f,  1.0f,
		 1.0f,  1.0f,
	};
			
	glGenBuffers(1, &vbo_fbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_fbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	//probably should move this to RenderingStructs
	vertexObject vOs[] = {
		createVO(modelData()),
		createVO(files::getVertexData("models/floor.ply")),
		createVO(files::getVertexData("models/wall.ply")),
		createVO(files::getVertexData("models/crosshair.ply")),
		createVO(files::getVertexData("models/corpse.ply")),
		createVO(files::getVertexData("models/person.ply")),
		createVO(files::getVertexData("models/pirate.ply")),
		createVO(files::getVertexData("models/bullet.ply")),
		createVO(files::getVertexData("models/evil_bullet.ply")),
	};

	//fb stuff
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resolution[0]*2, resolution[1]*2, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint depth_buffer;
	glGenRenderbuffers(1, &depth_buffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, resolution[0]*2, resolution[1]*2);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	GLuint frameBuffer;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex, 0);
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_buffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer);

	//glDrawBuffer(0);

	//water buffer
	GLuint water_height;
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &water_height);
	glBindTexture(GL_TEXTURE_2D, water_height);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 100, 100, 0, GL_RED, GL_FLOAT, &w.wh[w.awh][0][0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLenum error = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(error != GL_FRAMEBUFFER_COMPLETE){
		fprintf(stderr, "glCheckFramebufferStatus: error %p", error);
		return -1;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//end fb stuff

	LARGE_INTEGER oldTime;
	LARGE_INTEGER curTime;
	LARGE_INTEGER frq;
	{
		QueryPerformanceFrequency(&frq);
		QueryPerformanceCounter(&curTime);
		oldTime = curTime;
	}

	MSG msg;

	float left_over_time = 0.0;
	float old_dt = 0.0;

	do {
		if(input::pressed('Q')){
			dsb8->Play(0, 0, DSBPLAY_LOOPING);
		}

		if(input::pressed('O')){
			wglSwapIntervalEXT(1);
		}
		if(input::pressed('P')){
			wglSwapIntervalEXT(0);
		}

		const float frametime = 1.0f/125.0f;
		
		QueryPerformanceCounter(&curTime);
		float dt = (float)(curTime.QuadPart - oldTime.QuadPart)/(float)(frq.QuadPart);

		left_over_time += dt;

		if(dt-old_dt > frametime*4.0){
			left_over_time = frametime*4.0f;
		}

		old_dt = dt;

		for(; left_over_time >= 0.0; left_over_time -= frametime){
			worldLoop(&w, frametime, (float)(curTime.QuadPart)/(float)(frq.QuadPart));
		}
		oldTime = curTime;

		{//render stuff
			glViewport(0, 0, resolution[0]*2, resolution[0]*2);
			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
			glClearColor(0.5f, 0.5f, 0.5f, 0.1f);
			glClearDepth(0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUseProgram(program);
			GLuint trans = glGetUniformLocation(program, "t");
			
			//<temp>
			//renderObject(scale(w.cam.r, -1.0), float2(10000.0, 0.0), modId_wall, vOs, trans);
			//<\temp>

			renderWorld(&w, vOs, trans);
			
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			//draw framebuffer
			glViewport(0, 0, resolution[0], resolution[0]);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClearDepth(0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUseProgram(post_program);
			
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex);
			GLuint texture = glGetUniformLocation(post_program, "tex");
			glUniform1i(texture, 0);

			glActiveTexture(GL_TEXTURE1);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 100, 100, 0, GL_RED, GL_FLOAT, &w.wh[w.awh]);
			glBindTexture(GL_TEXTURE_2D, water_height);
			GLuint wh = glGetUniformLocation(post_program, "wh");
			glUniform1i(wh, 1);
			
			GLuint tim = glGetUniformLocation(post_program, "time");
			glUniform1f(tim, (float)(curTime.QuadPart)/(float)(frq.QuadPart));

			GLuint center = glGetUniformLocation(post_program, "center");
			glUniform2fv(center, 1, &w.cam.r[0]);

			float4x4 transform = {
				1.0, 0.0, 0.0, 0.0,
				0.0, 1.0, 0.0, 0.0,
				0.0, 0.0, 1.0, 0.0,
				0.0, 0.0, 0.0, 1.0,
			};

			transform[0][3] = 0.5*sub(w.cam.r, w.m.r)[0];
			transform[1][3] = 0.5*sub(w.cam.r, w.m.r)[1];
				
			float4x4 rotation = {
				1.0, 0.0, 0.0, 0.0,
				0.0, 1.0, 0.0, 0.0,
				0.0, 0.0, 1.0, 0.0,
				0.0, 0.0, 0.0, 1.0,
			};

			float2 dir = float2(-sin(w.m.theta), cos(w.m.theta));
			
			rotation[0][0] =  dir[1];
			rotation[0][1] =  dir[0];
			rotation[1][0] = -dir[0];
			rotation[1][1] =  dir[1];

			transform = multiply(transform, rotation);
			
			GLuint water_trans = glGetUniformLocation(post_program, "trans_w");
			glUniformMatrix4fv(water_trans, 1, false, (float*)&transform[0][0]);

			glBindBuffer(GL_ARRAY_BUFFER, vbo_fbo_vertices);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, 0, 0, 0);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			
			SwapBuffers(dc);
		}

		while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

	} while(msg.message != WM_QUIT);
	
	//glDeleteBuffers(1, &vbo_fbo_vertices);

	return msg.wParam;
}