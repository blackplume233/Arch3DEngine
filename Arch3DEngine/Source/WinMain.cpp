#define WIN32_LEAN_AND_MEAN

#pragma  warning(disable:4305)


#define INITGUID // make sure all the COM interfaces are available
                 // instead of this you can include the .LIB file
                 // DXGUID.LIB

#include <windows.h>  // include important windows stuff
#include <windowsx.h>
#include <mmsyscom.h>
#include <iostream>   // include important C/C++ stuff
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <io.h>
#include <fcntl.h>


#include <ddraw.h> // directX includes

#include <dmksctrl.h>


#include <dinput.h>
#include "T3D/t3dlib1.h" // game library includes
#include "T3D/t3dlib2.h"
#include "T3D/t3dlib4.h"
#include "t3d/t3dlib5.h"
#include "t3d/t3dlib6.h"

#pragma comment (lib,"DDRAW.LIB")
#pragma comment(lib,"DINPUT8.LIB")

#pragma warning(disable: 4244 4101)



// defines for windows interface
#define WINDOW_CLASS_NAME L"WIN3DCLASS" // class name
#define WINDOW_TITLE      L"T3D Graphics console Ver 2.0"
#define WINDOW_WIDTH      800  // size of window
#define WINDOW_HEIGHT     600

#define WINDOW_BPP        16   //bit depth of window(8,16,24 etc.)
                               // note: if windowed and not
                               // full_screen then bit depth must
                               // be same as system bit depth
                               // also if 8_bit the a pallet
                               // is created and attached

#define WINDOWS_APP        1   // 0 not windowed,1 windowed

#define NUM_OBJECTS        16   // number of objects on a row


// define for the game universe
#define UNIVERSE_RADIUS 4000
#define POINT_SIZE      200
#define NUM_POINTS_X     (2*UNIVERSE_RADIUS/POINT_SIZE)
#define NUM_POINTS_Z     (2*UNIVERSE_RADIUS/POINT_SIZE)
#define NUM_POINTS       (NUM_POINTS_X*NUM_POINTS_Z)

// define for objects
#define NUM_TOWERS   96
#define NUM_TANKS    24
#define TANK_SPEED   15

// create some constants for ease of access
#define AMBIENT_LIGHT_INDEX      0 // ambient light index
#define INFINITE_LIGHT_INDEX     1 // infinite light index
#define POINT_LIGHT_INDEX        2 // point light index
#define SPOT_LIGHT_INDEX         3 // spot light index



// PROTOTYPES //////////////////////////////////////////////////////////////////////////

// game consoles
int Game_Init(void* params = NULL);
int Game_Main(void* params = NULL);
int Game_Shutdown(void* params = NULL);

// GLOBALS //////////////////////////////////////////////////////////////////////////

HWND main_window_handle = NULL;  // save the window handle
HINSTANCE main_instance  = NULL;  // save the instance
char buffer[256];                 // use to print text

// initialize camera position and direction
POINT4D cam_pos = { 0.0f,40.f,0.0f,1.0f };
VECTOR4D cam_dir = { 0.0f,0.0f,0.0f,1.0f };
POINT4D cam_target = { 0,0,0,1.f };

// all your initialization code goes here...
VECTOR4D vscale = { 1.0f,1.0f,1.0f,1.0f },
         vpos = { 0.0f,0.0f,0.0f,1.0f },
         vrot = { 0.0f,0.0f,0.0f,1.0f };

CAM4DV1        cam;                            // the single camera
RENDERLIST4DV1 rend_list;                      // the single render_list

OBJECT4DV1 FirstCube;                         // the first cube model
OBJECT4DV1 TankObj;                           // Tank obj
OBJECT4DV1 TowerObj;                          // used to hold the master tank
OBJECT4DV1 MarkerObj;                         // the ground marker
OBJECT4DV1 PlayerObj;                         // the player object

POINT4D towers[NUM_TOWERS],                     
        tanks[NUM_TANKS];





// FUNCTIONS //////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK WindowProc(HWND hwnd,
	                        UINT msg,
	                        WPARAM wparam,
	                        LPARAM lparam)
{
	// this is the main message handler of the system
	PAINTSTRUCT PaintStruct;       // used in WM_PAINT
	HDC         hdc;               // handle to a device context

	// what is the message
	switch (msg)
	{
	case WM_CREATE:
	{
		// do initialization stuff here
		return(0);
	}break;

	case WM_PRINT:
	{
		// starting painting
		hdc = BeginPaint(hwnd,&PaintStruct);

		// end painting
		EndPaint(hwnd, &PaintStruct);
		return (0);
	}break;

	case WM_DESTROY:
	{
		// kill the application
		PostQuitMessage(0);
		return(0);
	}break;

	default:
		break;
	} // end switch

	// process any message that we didn't take care of
	return (DefWindowProc(hwnd, msg, wparam, lparam));
} // end WinProc

// WINMAIN //////////////////////////////////////////////////////////////////////////

int WINAPI WinMain(HINSTANCE hinstance,
	               HINSTANCE hprevinstance,
	               LPSTR lpcmdline,
	               int ncmdshow)
{
	// this is the win_main function

	WNDCLASS winclass; // this will hold the class we create
	HWND     hwnd;     // generic window handle
	MSG      msg;      // generic messAge
	HDC      hdc;      // generic device context
	PAINTSTRUCT ps;    // generic paint_struct

	// first fill in the window class structure
	winclass.style         = CS_DBLCLKS | CS_OWNDC |
		                     CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc   = WindowProc;
	winclass.cbClsExtra    = 0;
	winclass.cbWndExtra    = 0;
	winclass.hInstance     = hinstance;
	winclass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	winclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName  = NULL;
	winclass.lpszClassName = WINDOW_CLASS_NAME;

	// register the window class
	if (!RegisterClass(&winclass))
	{
		return(0);
	}

	// create the window, note the test to see if WINDOW_APP is
	// true to select the appropriate window style
	if (!(hwnd=CreateWindow(WINDOW_CLASS_NAME, //class
		WINDOW_TITLE, // title
		(WINDOWS_APP?(WS_OVERLAPPED|WS_SYSMENU|WS_CAPTION):(WS_POPUP|WS_VISIBLE)),
		0,0,           // x,y
		WINDOW_WIDTH,  // width
		WINDOW_HEIGHT, // height
		NULL,          // handle to parent
		NULL,          // handle to menu
		hinstance,     // instance
		NULL)))        // creation params
	{
		return(0);
	}

	// save the window handle and instance in global
	main_window_handle = hwnd;
	main_instance       = hinstance;

	// resize the window so that client is really width x height
	if (WINDOWS_APP)
	{
		// now resize the window, so the client area is the actual size requested
		// since there may be borders and controls if this is going to be a windowed app
		// if the app is not windowed then it won't matter
		RECT window_rect = { 0,0,WINDOW_WIDTH - 1,WINDOW_HEIGHT - 1 };

		// make the call to adjust window_rect
		AdjustWindowRectEx(&window_rect,
			GetWindowStyle(main_window_handle),
			GetMenu(main_window_handle) != NULL,
			GetWindowExStyle(main_window_handle));

		// save the global client offsets, they are needed in DDraw_Flip()
		window_client_x0 = -window_rect.left;
		window_client_y0 = -window_rect.top;

		// now resize the window with a call to MoveWindow()
		MoveWindow(main_window_handle,
			0, // x position
			0, // y position
			window_rect.right - window_rect.left, // width
			window_rect.bottom - window_rect.top, // height
			FALSE);

		// show the window, so there's no garbage on first render
		ShowWindow(main_window_handle, SW_SHOW);
	} // end if windowed

	// perform all game console specific initialization
	Game_Init();

	// disable CTRL+ALT+DEL,ALT+TAB,comment this line out
	// if it causes your system to crash
	SystemParametersInfo(SPI_SCREENSAVERRUNNING, TRUE, NULL, 0);

	// enter main event loop
	while (1)
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			// test if this is a quit
			if (msg.message == WM_QUIT)
				break;
			
			// translate any accelerator keys
			TranslateMessage(&msg);

			// send the message to the window proc
			DispatchMessage(&msg);
		} // end if

		// main game processing goes here
		Game_Main();

	} // end while 

	// shutdown game and release all resources
	Game_Shutdown();

	// enable CTRL+ALT+DEL,ALT+TAB,comment this line out
	// if it causes your system to crash
	SystemParametersInfo(SPI_SCREENSAVERRUNNING, FALSE, NULL, 0);

	// return to Windows like this
	return(msg.wParam);

} // end WinMain


// T3D TI GAME PROGRAMMING CONSOLE FUNCTIONS //////////////////////////////////////////////////////////////////////////

int Game_Init(void* params /* = NULL */)
{
	// this function is where you do all the initialization
	// for your game

	int index;  // looping var

	// start up DirectDraw (replace the params as your desire)
	DDraw_Init(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_BPP, WINDOWS_APP);

	// initialize directInput
	DInput_Init();

	// acquire the keyboard
	DInput_Init_Keyboard();

	// add calls to acquire other directInput devices here...



	// hide the mouse
	if (!WINDOWS_APP)
		ShowCursor(false);

	// seed random number generator
	srand(Start_Clock());

	Open_Error_File("ERROR.TXT");

	// initialize math engine
	Build_Sin_Cos_Tables();

	// LOAD ALL MODEL OBJ
	// load the master tank obj
	VECTOR4D_INITXYZ(&vscale, 0.75f, 0.75f, 0.75f);
	Load_OBJECT4DV1_PLG(&TankObj, "Model/tank2.plg", &vscale, &vpos, &vrot);

	// load player object for 3rd person view
	VECTOR4D_INITXYZ(&vscale, 0.75f, 0.75f, 0.75f);
	Load_OBJECT4DV1_PLG(&PlayerObj, "Model/tank3.plg", &vscale, &vpos, &vrot);

	// load the master tower object
	VECTOR4D_INITXYZ(&vscale, 1.f, 2.f, 1.f);
	Load_OBJECT4DV1_PLG(&TowerObj, "Model/tower1.plg", &vscale, &vpos, &vrot);

	// load the master ground marker
	VECTOR4D_INITXYZ(&vscale, 3.f, 3.f, 3.f);
	Load_OBJECT4DV1_PLG(&MarkerObj, "Model/marker1.plg", &vscale, &vpos, &vrot);

	// position the tanks
	for (index = 0; index < NUM_TANKS; index++)
	{
		// randomly position the tanks
		tanks[index].x = RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
		tanks[index].y = 0;
		tanks[index].z = RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
		tanks[index].w = RAND_RANGE(0, 360);
	} // end for

	// position the towers
	for (index = 0; index < NUM_TOWERS; index++)
	{
		// randomly position the tower
		towers[index].x = RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
		towers[index].y = 0;
		towers[index].z = RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
	} // end for 

	// set up lights
	Reset_Lights_LIGHTV1();

	// create some working colors
	RGBAV1 white, gray, black, red, green, blue;

	white.rgba = _RGBA32BIT(255, 255, 255, 0);
	gray.rgba = _RGBA32BIT(100, 100, 100, 0);
	black.rgba = _RGBA32BIT(0, 0, 0, 0);
	red.rgba = _RGBA32BIT(255, 0, 0, 0);
	green.rgba = _RGBA32BIT(0, 255, 0, 0);
	blue.rgba = _RGBA32BIT(0, 0, 255, 0);

	// ambient light
	Init_Light_LIGHTV1(AMBIENT_LIGHT_INDEX,  //  
		LIGHTV1_STATE_ON,                    // turn the light on
		LIGHTV1_ATTR_AMBIENT,                // ambient light  type
		gray, blue, black,                   // color for ambient term only
		NULL, NULL,                          // no need for pos and dir
		0, 0, 0,                             // spot light info NA
		0, 0, 0);

	VECTOR4D dlight_dir = { -1,0,-1,0 };

	// directional light
	Init_Light_LIGHTV1(INFINITE_LIGHT_INDEX,
		LIGHTV1_STATE_ON,                          // turn on the light
		LIGHTV1_ATTR_INFINITE,                     // ambient light type
		black, gray, black,                        // color for ambient term only
		NULL, &dlight_dir,                         // need direction only
		0, 0, 0,                                   // no need for attenuation
		0, 0, 0);

	VECTOR4D plight_pos = { 0,200,0,0 };

	// point light
	Init_Light_LIGHTV1(POINT_LIGHT_INDEX,
		LIGHTV1_STATE_ON,                   // turn on the light
		LIGHTV1_ATTR_POINT,                 // pointlight type
		black, green, black,                // color for diffuse term only
		&plight_pos, NULL,                  // need pos only
		0, 0.001, 0,                          // linear attenuation only
		0, 0, 1);      // spotlight info NA

	VECTOR4D slight_pos = { 0,200,0,0 };
	VECTOR4D slight_dir = { -1,0,-1,0 };

	// spot light
	Init_Light_LIGHTV1(SPOT_LIGHT_INDEX,  
		LIGHTV1_STATE_ON,                 // turn the light on
		LIGHTV1_ATTR_SPOTLIGHT2,          // spot light type 2
		black, red, black,                // color for diffuse term only
		&slight_pos, &slight_dir,         // need pos and direciton
		0, 0.001, 0,                      // linear attenuation only
		0, 0, 1);                         // spotlight power-factor only

	// create lookup table for lightinng engine
	RGB_16_8_IndexedRGB_Table_Builder(DD_PIXEL_FORMAT565,
	                                  palette,
		                              rgblookup);



	// initialize the camera with 90 FOV, normalized coordinates
	Init_CAM4DV1(&cam,            // the camera object
		         CAM_MODEL_EULER, // Euler camera model
		         &cam_pos,        // initial camera position
		         &cam_dir,
		         &cam_target,// initial camera angles
		         200.0,            // near and far clipping planes
		         12000.0,          
		         120.0,            // field of view in degrees
		         WINDOW_WIDTH,
		         WINDOW_HEIGHT);

	// return success
	return(1);

} // end Game_Init

//////////////////////////////////////////////////////////////////////////

int Game_Shutdown(void* params /* = NULL */)
{
	// this function is where you shut shown your game and
	// release all resources that you allocated

	// shut everything down

	// release all your resources created for the game here....


	DInput_Shutdown();

	// shutdown direct_draw last
	DDraw_Shutdown();

	Close_Error_File();

	// return success
	return(1);
} // end Game_Shutdown

//////////////////////////////////////////////////////////////////////////

int Game_Main(void* params /* = NULL */)
{
	// this is the workhorse of your game it will be called
	// continuously in real-time this is like main() in c
	// all the calls for your game go here!

	static MATRIX4X4 mrot;                          // general rotation matrix
	static float ang_x = 0,
		         ang_y = 5,
		         ang_z = 0;   //rotation angle

	// these are used to creating a circling camera
	static float view_angle=0.f;
	static float camera_distance = 6000.f;
	static VECTOR4D pos = { 0,0,0,0 };
	static float tank_speed;
	static float turning = 0;

	// states variables for different rendering modes and help
	static int wireframe_mode = -1;
	static int backface_mode = 1;
	static int lighting_mode = 1;
	static int help_mode = -1;
	static int sort_mode = 0;

	char work_string[256];

	int index; //looping var

	// start the timing clock
	Start_Clock();

	// clear the drawing surface
	DDraw_Fill_Surface(lpddsback, 0);

	// draw the sky
	//Draw_Rectangle(0, 0, WINDOW_WIDTH - 1, WINDOW_HEIGHT / 2, RGB16Bit(0, 140, 192), lpddsback);

	// draw teh ground
	//Draw_Rectangle(0, WINDOW_HEIGHT / 2, WINDOW_WIDTH - 1, WINDOW_HEIGHT - 1, RGB16Bit(103, 62, 3), lpddsback);

	// read keyboard and other devices here
	DInput_Read_Keyboard();

	// game logic here....

	// initialize the render-list
	Reset_RENDERLIST4DV1(&rend_list);
	//Reset_OBJECT4DV1(&FirstCube);

	// allow user to move camera
	// turbo
	if (keyboard_state[DIK_SPACE])
	{
		tank_speed = 5 * TANK_SPEED;
	}
	else
		tank_speed = TANK_SPEED;

	// forward/backward
	if (keyboard_state[DIK_UP])
	{
		// move forward
		cam.pos.x += tank_speed*Fast_Sin(cam.dir.y);
		cam.pos.z += tank_speed*Fast_Cos(cam.dir.y);
	} // end if 

	if (keyboard_state[DIK_DOWN])
	{
		// move backward
		cam.pos.x -= tank_speed*Fast_Sin(cam.dir.y);
		cam.pos.z -= tank_speed*Fast_Cos(cam.dir.y);
	} // end if

	// rotate 
	if (keyboard_state[DIK_RIGHT])
	{
		cam.dir.y += 3;

		// add a little turn to object
		if ((turning+=2) > 15)
		{
			turning = 15;
		}
	} // end if

	if (keyboard_state[DIK_LEFT])
	{
		cam.dir.y -= 3;

		// add a little turn to object
		if ((turning-=2) < -15)
		{
			turning = -15;
		}
	} // end if 
	else // center heading again
	{
		if (turning > 0)
		{
			turning -= 1;
		}
		else if(turning < 0)
		{
			turning += 1;
		}
	} // end else

	// modes and lights

	// wireframe mode
	if (keyboard_state[DIK_W])
	{
		//toggle wireframe mode
		wireframe_mode = -wireframe_mode;
		Wait_Clock(100);  // wait,so keyboard doesn't bounce
	} // end if

	// backface removal
	if (keyboard_state[DIK_B])
	{
		// toggle backface mode
		backface_mode = -backface_mode;
		Wait_Clock(100); // wait,so keyboard doesn't bounce
	} // end if

	// lighting
	if (keyboard_state[DIK_L])
	{
		// toggle lighting mode
		lighting_mode = -lighting_mode;
		Wait_Clock(100);// wait,so keyboard doesn't bounce
	} // end if

	// toggle ambient light
	if (keyboard_state[DIK_A])
	{
		// toggle ambient light
		if (lights[AMBIENT_LIGHT_INDEX].state == LIGHTV1_STATE_ON)
			lights[AMBIENT_LIGHT_INDEX].state = LIGHTV1_STATE_OFF;
		else
			lights[AMBIENT_LIGHT_INDEX].state = LIGHTV1_STATE_ON;

		Wait_Clock(100); // wait, so keyboard doesn't bounce
	}

	// toggle point light
	if (keyboard_state[DIK_P])
	{
		// toggle point light
		if (lights[POINT_LIGHT_INDEX].state == LIGHTV1_STATE_ON)
			lights[POINT_LIGHT_INDEX].state = LIGHTV1_STATE_OFF;
		else
			lights[POINT_LIGHT_INDEX].state = LIGHTV1_STATE_ON;

		Wait_Clock(100);
	} // end if 

	// toggle infiite light
	if (keyboard_state[DIK_I])
	{
		// toggle infinite light
		if (lights[INFINITE_LIGHT_INDEX].state == LIGHTV1_STATE_ON)
			lights[INFINITE_LIGHT_INDEX].state = LIGHTV1_STATE_OFF;
		else
			lights[INFINITE_LIGHT_INDEX].state = LIGHTV1_STATE_ON;

		Wait_Clock(100);
	} // end if


	// toggle spot light
	if (keyboard_state[DIK_S])
	{
		// toggle spot light
		if (lights[SPOT_LIGHT_INDEX].state == LIGHTV1_STATE_ON)
			lights[SPOT_LIGHT_INDEX].state = LIGHTV1_STATE_OFF;
		else
			lights[SPOT_LIGHT_INDEX].state = LIGHTV1_STATE_ON;

		Wait_Clock(100);
	} // end if

	// help menu
	if (keyboard_state[DIK_H])
	{
		// toggle help menu
		help_mode = -help_mode;
		Wait_Clock(100);
	} // end if

	// sort mode
	if (keyboard_state[DIK_Z])
	{
		++sort_mode;
		if (sort_mode >2)
		{
			sort_mode = 0;
		}
	}

	static float plight_ang = 0, slight_ang = 0;// angles for light motion

	// move point light source in ellipse around game world
	lights[POINT_LIGHT_INDEX].pos.x = 4000 * Fast_Cos(plight_ang);
	lights[POINT_LIGHT_INDEX].pos.y = 200;
	lights[POINT_LIGHT_INDEX].pos.z = 4000 * Fast_Sin(plight_ang);

	if ((plight_ang += 3) > 360)
		plight_ang = 0;

	// move spot light source in ellipse around game world
	lights[SPOT_LIGHT_INDEX].pos.x = 2000 * Fast_Cos(slight_ang);
	lights[SPOT_LIGHT_INDEX].pos.y = 2000;
	lights[SPOT_LIGHT_INDEX].pos.z = 2000 * Fast_Sin(slight_ang);

	if ((slight_ang -= 5) < 5)
		slight_ang = 360;



	// insert polygon into the render-list
	//Insert_POLYF4DV1_RENDERLIST4DV1(&rend_list, &poly1);

	// generate rotation matrix around y axis
	//Build_XYZ_Rotation_MATRIX4X4(ang_x, ang_y, ang_z, &mrot);

	// rotate the local coordinations of single polygon in render-list
	//Transform_RENDERLIST4DV1(&rend_list, &mrot, TRANSFORM_LOCAL_ONLY);
	//Transform_OBJECT4DV1(&TankObj, &mrot, TRANSFORM_LOCAL_ONLY,1);
	
	strcpy(buffer, "Objects Culled: ");

	// compute the camera's new position,camera move around the circle
	//cam.pos.x = 2*camera_distance*Fast_Cos(view_angle);
	//cam.pos.y = 2 * camera_distance*Fast_Sin(view_angle);
	//cam.pos.z = 2*camera_distance*Fast_Sin(view_angle);

	

	// generate camera matrix
	Build_CAM4DV1_Matrix_Euler(&cam, CAM_ROT_SEQ_ZYX);

	// insert the tanks in the world
	for (index =0; index < NUM_TANKS; index++)
	{
		// reset the object (this is only matters for back-face and object removal)
		Reset_OBJECT4DV1(&TankObj);

		// generate rotation matrix around y axis
		Build_XYZ_Rotation_MATRIX4X4(0, tanks[index].w, 0, &mrot);

		// rotate the local coordinations of the object
		Transform_OBJECT4DV1(&TankObj, &mrot, TRANSFORM_LOCAL_TO_TRANS, 1);

		// set the position of tank
		TankObj.world_pos.x = tanks[index].x;
		TankObj.world_pos.y = tanks[index].y;
		TankObj.world_pos.z = tanks[index].z;

		// attempt to cull object
		if (!Cull_OBJECT4DV1(&TankObj,&cam,CULL_OBJECT_XYZ_PLANES))
		{
			// if we get here then the object is visible at this world position
			// so we can insert it into the rendering list
			// perform local/model to world transform
			Model_To_World_OBJECT4DV1(&TankObj, TRANSFORM_TRANS_ONLY);

			// perform lighting
			if (lighting_mode==1)
			{
				Light_OBJECT4DV1_World16(&TankObj, &cam,lights,4);
			}

			// insert the object into render list
			Insert_OBJECT4DV1_RENDERLIST4DV12(&rend_list, &TankObj, 0, lighting_mode);
		} // end if 

	} // end for

	// insert the player into the world
	// reset the object(this only matters for back-face and object removal)
	Reset_OBJECT4DV1(&PlayerObj);

	// set position of tank
	PlayerObj.world_pos.x = cam.pos.x + 300 * Fast_Sin(cam.dir.y);
	PlayerObj.world_pos.y = cam.pos.y - 70;
	PlayerObj.world_pos.z = cam.pos.z + 300 * Fast_Cos(cam.dir.y);
	
	// generate rotation matrix around y axis
	Build_XYZ_Rotation_MATRIX4X4(0, cam.dir.y + turning, 0, &mrot);

	// rotate the local coordinations of the object
	Transform_OBJECT4DV1(&PlayerObj, &mrot, TRANSFORM_LOCAL_TO_TRANS, 1);

	// perform world transform
	Model_To_World_OBJECT4DV1(&PlayerObj, TRANSFORM_TRANS_ONLY);

	// perform lighting
	if (lighting_mode == 1)
		Light_OBJECT4DV1_World16(&PlayerObj, &cam, lights, 4);

	// insert the object into render list
	Insert_OBJECT4DV1_RENDERLIST4DV12(&rend_list, &PlayerObj,0,lighting_mode);

	// insert the towers in the world
	for (index = 0; index < NUM_TOWERS; index++)
	{
		// reset the object (this only matters for back-face and object removal)
		Reset_OBJECT4DV1(&TowerObj);

		// set position of tower
		TowerObj.world_pos.x = towers[index].x;
		TowerObj.world_pos.y = towers[index].y;
		TowerObj.world_pos.z = towers[index].z;

		// attempt to cull object
		if (!Cull_OBJECT4DV1(&TowerObj,&cam,CULL_OBJECT_XYZ_PLANES))
		{
			// if we get here then the object is visible at this world position
			// so we can insert it into the rendering list
			// perform local/model to world transform
			Model_To_World_OBJECT4DV1(&TowerObj);

			// perform lighting
			if (lighting_mode==1)
			{
				Light_OBJECT4DV1_World16(&TowerObj, &cam, lights, 4);
			}

			// insert the object into rend list
			Insert_OBJECT4DV1_RENDERLIST4DV12(&rend_list, &TowerObj,0,lighting_mode);
		} // end if 

	} // end for

	// seed number generator so that modulation of markers is always the same
	srand(13);

	// insert the ground markers into the world
	for (int index_x = 0;index_x < NUM_POINTS_X; index_x++)
	{
		for (int index_z=0; index_z < NUM_POINTS_Z; index_z++)
		{
			// reset the object(this is only matters for backface and object removal)
			Reset_OBJECT4DV1(&MarkerObj);

			// set position of tower
			MarkerObj.world_pos.x = RAND_RANGE(-100, 100) - UNIVERSE_RADIUS + index_x*POINT_SIZE;
			MarkerObj.world_pos.y = MarkerObj.max_radius;
			MarkerObj.world_pos.z = RAND_RANGE(-100, 100) - UNIVERSE_RADIUS + index_z*POINT_SIZE;

			// attempt to cull object
			if (!Cull_OBJECT4DV1(&MarkerObj,&cam,CULL_OBJECT_XYZ_PLANES))
			{
				// if we get here then the object is visible at this world position
				// so we can insert it into the rendering list
				// perform local/model to world transform
				Model_To_World_OBJECT4DV1(&MarkerObj);

				// perform lighting
				if (lighting_mode==1)
				{
					Light_OBJECT4DV1_World16(&MarkerObj, &cam, lights, 4);
				}

				// insert the object into render list
				Insert_OBJECT4DV1_RENDERLIST4DV2(&rend_list, &MarkerObj,0,lighting_mode);
			} // end if 
		}
	}

	

	// perform local/model to world transform
	//Model_To_World_RENDERLIST4DV1(&rend_list, &poly1_pos);
	//Model_To_World_OBJECT4DV1(&FirstCube);

	

	// remove the back face
	//Remove_Backfaces_OBJECT4DV1(&FirstCube, &cam);
	if (backface_mode==1)
		Remove_Backfaces_RENDERLIST4DV1(&rend_list, &cam);

	// apply world to camera transform
	World_To_Camera_RENDERLIST4DV1(&rend_list, &cam);
	//World_To_Camera_OBJECT4DV1(&FirstCube, &cam);

	// sort rend-list ploy
	Sort_RENDERLIST4DV1(&rend_list, sort_mode);

	// apply camera to perspective transformation
	Camera_To_Perspective_RENDERLIST4DV1(&rend_list, &cam);
	//Camera_To_Perspective_OBJECT4DV1(&FirstCube, &cam);


	// apply screen transform
	Perspective_To_Screen_RENDERLIST4DV1(&rend_list, &cam);
	//Perspective_To_Screen_OBJECT4DV1(&FirstCube, &cam);

	sprintf(work_string, "pos:[%f %f %f] heading:[%f] elev:[%f]", cam.pos.x, cam.pos.y,cam.pos.z,cam.dir.y,cam.dir.x);

	// draw instructions
	Draw_Text_GDI(work_string, 0, WINDOW_HEIGHT-20, RGB(0, 255, 0), lpddsback);

	sprintf(work_string, "Lighting [%s]: Ambient=%d, Infinite=%d, Point=%d, Spot=%d | BackFaceRM [%s] | SortMode [ %d ]",
		((lighting_mode == 1) ? "ON" : "OFF"),
		lights[AMBIENT_LIGHT_INDEX].state,
		lights[INFINITE_LIGHT_INDEX].state,
		lights[POINT_LIGHT_INDEX].state,
		lights[SPOT_LIGHT_INDEX].state,
		((backface_mode == 1) ? "ON" : "OFF"),
		sort_mode);

	Draw_Text_GDI(work_string, 0, WINDOW_HEIGHT - 34, RGB(0, 255, 0), lpddsback);
	Draw_Text_GDI("Press ESC to exit,Press <H> for help.",0, 0, RGB(255, 255, 0), lpddsback);

	// should we display help
	int text_y = 16;
	if (help_mode==1)
	{
		// draw help menu
		Draw_Text_GDI("<A>..............Toggle ambient light source.", 0, text_y += 12, RGB(255, 255, 255), lpddsback);
		Draw_Text_GDI("<I>..............Toggle infinite light source.", 0, text_y += 12, RGB(255, 255, 255), lpddsback);
		Draw_Text_GDI("<P>..............Toggle point light source.", 0, text_y += 12, RGB(255, 255, 255), lpddsback);
		Draw_Text_GDI("<S>..............Toggle spot light source.", 0, text_y += 12, RGB(255, 255, 255), lpddsback);
		Draw_Text_GDI("<W>..............Toggle wire frame/solid mode.", 0, text_y += 12, RGB(255, 255, 255), lpddsback);
		Draw_Text_GDI("<B>..............Toggle backface removal.", 0, text_y += 12, RGB(255, 255, 255), lpddsback);
		Draw_Text_GDI("<RIGHT ARROW>....Rotate player right.", 0, text_y += 12, RGB(255, 255, 255), lpddsback);
		Draw_Text_GDI("<LEFT ARROW>.....Rotate player left.", 0, text_y += 12, RGB(255, 255, 255), lpddsback);
		Draw_Text_GDI("<UP ARROW>.......Move player forward.", 0, text_y += 12, RGB(255, 255, 255), lpddsback);
		Draw_Text_GDI("<DOWN ARROW>.....Move player backward.", 0, text_y += 12, RGB(255, 255, 255), lpddsback);
		Draw_Text_GDI("<SPACE BAR>......Turbo.", 0, text_y += 12, RGB(255, 255, 255), lpddsback);
		Draw_Text_GDI("<H>..............Toggle Help.", 0, text_y += 12, RGB(255, 255, 255), lpddsback);
		Draw_Text_GDI("<ESC>............Exit demo.", 0, text_y += 12, RGB(255, 255, 255), lpddsback);

	}


	// lock the back buff
	DDraw_Lock_Back_Surface();
	
	// reset number of polys rendered
	debug_polys_rendered_per_frame = 0;

	// render the polygon list
	if (wireframe_mode==1)
	{
		Draw_RENDERLIST4DV1_Wire16(&rend_list, back_buffer, back_lpitch);
	}
	else
	{
		Draw_RENDERLIST4DV1_Solid16(&rend_list, back_buffer, back_lpitch);
	}
	//Draw_OBJECT4DV1_Wire16(&FirstCube, back_buffer, back_lpitch);

	// unlock the back buffer
	DDraw_Unlock_Back_Surface();

	// flip the surfaces
	DDraw_Flip();

	// sync to 30ish fps
	Wait_Clock(30);
	
	// check of users is trying to exit
	if (KEY_DOWN(VK_ESCAPE)|| keyboard_state[DIK_ESCAPE])
	{
		PostMessage(main_window_handle, WM_DESTROY, 0, 0);
	}// end if 

	// return success
	return(1);
	
	
} // end Game_Main()