#pragma once

#ifndef BINDINGS_H
#define BINDINGS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

namespace sdl
{
	using Point         = SDL_Point;
	using FPoint        = SDL_FPoint;
	using Rect          = SDL_Rect;
	using FRect         = SDL_FRect;
	using Color         = SDL_Color;
	using Window        = SDL_Window;
	using Renderer      = SDL_Renderer;
	using Surface       = SDL_Surface;
	using Texture       = SDL_Texture;
	using Event         = SDL_Event;
	using EventType     = SDL_EventType;
	using WindowEventID = SDL_WindowEventID;

	namespace env
	{
		constexpr Color black = {/*.r =*/ 0,   /*.g =*/ 0,   /*.b =*/ 0,   /*.a =*/ 255};
		constexpr Color white = {/*.r =*/ 255, /*.g =*/ 255, /*.b =*/ 255, /*.a =*/ 255};
	}
}

namespace sdl::init
{
	enum : Uint32
	{
		TIMER          = SDL_INIT_TIMER         ,
		AUDIO          = SDL_INIT_AUDIO         ,
		VIDEO          = SDL_INIT_VIDEO         , // VIDEO implies EVENTS
		JOYSTICK       = SDL_INIT_JOYSTICK      , // JOYSTICK implies EVENTS
		HAPTIC         = SDL_INIT_HAPTIC        ,
		GAMECONTROLLER = SDL_INIT_GAMECONTROLLER, // GAMECONTROLLER implies JOYSTICK
		EVENTS         = SDL_INIT_EVENTS        ,
		SENSOR         = SDL_INIT_SENSOR        ,
		NOPARACHUTE    = SDL_INIT_NOPARACHUTE   , // compatibility; this flag is ignored.
		EVERYTHING     = SDL_INIT_EVERYTHING    ,
	};
}

namespace sdl::win::init
{
	enum
	{
		FULLSCREEN         = SDL_WINDOW_FULLSCREEN        , // fullscreen window
		OPENGL             = SDL_WINDOW_OPENGL            , // window usable with OpenGL context
		SHOWN              = SDL_WINDOW_SHOWN             , // window is visible
		HIDDEN             = SDL_WINDOW_HIDDEN            , // window is not visible
		BORDERLESS         = SDL_WINDOW_BORDERLESS        , // no window decoration
		RESIZABLE          = SDL_WINDOW_RESIZABLE         , // window can be resized
		MINIMIZED          = SDL_WINDOW_MINIMIZED         , // window is minimized
		MAXIMIZED          = SDL_WINDOW_MAXIMIZED         , // window is maximized
		MOUSE_GRABBED      = SDL_WINDOW_MOUSE_GRABBED     , // window has grabbed mouse input
		INPUT_FOCUS        = SDL_WINDOW_INPUT_FOCUS       , // window has input focus
		MOUSE_FOCUS        = SDL_WINDOW_MOUSE_FOCUS       , // window has mouse focus
		FULLSCREEN_DESKTOP = SDL_WINDOW_FULLSCREEN_DESKTOP,
		FOREIGN            = SDL_WINDOW_FOREIGN           , // window not created by SDL
		ALLOW_HIGHDPI      = SDL_WINDOW_ALLOW_HIGHDPI     , // window should be created in high-DPI mode if supported.
														/* On macOS NSHighResolutionCapable must be set true in the
															application's Info.plist for this to have any effect. */
		MOUSE_CAPTURE    = SDL_WINDOW_MOUSE_CAPTURE   , // window has mouse captured (unrelated to MOUSE_GRABBED)
		ALWAYS_ON_TOP    = SDL_WINDOW_ALWAYS_ON_TOP   , // window should always be above others
		SKIP_TASKBAR     = SDL_WINDOW_SKIP_TASKBAR    , // window should not be added to the taskbar
		UTILITY          = SDL_WINDOW_UTILITY         , // window should be treated as a utility window
		TOOLTIP          = SDL_WINDOW_TOOLTIP         , // window should be treated as a tooltip
		POPUP_MENU       = SDL_WINDOW_POPUP_MENU      , // window should be treated as a popup menu
		KEYBOARD_GRABBED = SDL_WINDOW_KEYBOARD_GRABBED, // window has grabbed keyboard input
		VULKAN           = SDL_WINDOW_VULKAN          , // window usable for Vulkan surface
		METAL            = SDL_WINDOW_METAL           , // window usable for Metal view

		INPUT_GRABBED    = SDL_WINDOW_INPUT_GRABBED   , // equivalent to MOUSE_GRABBED for compatibility
	};
}

namespace sdl::win::event
{
	enum
	{
		NONE            = SDL_WINDOWEVENT_NONE,           /**< Never used */
		SHOWN           = SDL_WINDOWEVENT_SHOWN,          /**< Window has been shown */
		HIDDEN          = SDL_WINDOWEVENT_HIDDEN,         /**< Window has been hidden */
		EXPOSED         = SDL_WINDOWEVENT_EXPOSED,        /**< Window has been exposed and should be redrawn */
		MOVED           = SDL_WINDOWEVENT_MOVED,          /**< Window has been moved to data1, data2 */
		RESIZED         = SDL_WINDOWEVENT_RESIZED,        /**< Window has been resized to data1xdata2 */
		SIZE_CHANGED    = SDL_WINDOWEVENT_SIZE_CHANGED,   /**< The window size has changed, either as
																a result of an API call or through the
																system or user changing the window size. */
		MINIMIZED       = SDL_WINDOWEVENT_MINIMIZED,      /**< Window has been minimized */
		MAXIMIZED       = SDL_WINDOWEVENT_MAXIMIZED,      /**< Window has been maximized */
		RESTORED        = SDL_WINDOWEVENT_RESTORED,       /**< Window has been restored to normal size
																and position */
		ENTER           = SDL_WINDOWEVENT_ENTER,          /**< Window has gained mouse focus */
		LEAVE           = SDL_WINDOWEVENT_LEAVE,          /**< Window has lost mouse focus */
		FOCUS_GAINED    = SDL_WINDOWEVENT_FOCUS_GAINED,   /**< Window has gained keyboard focus */
		FOCUS_LOST      = SDL_WINDOWEVENT_FOCUS_LOST,     /**< Window has lost keyboard focus */
		CLOSE           = SDL_WINDOWEVENT_CLOSE,          /**< The window manager requests that the window be closed */
		TAKE_FOCUS      = SDL_WINDOWEVENT_TAKE_FOCUS,     /**< Window is being offered a focus (should SetWindowInputFocus() on itself or a subwindow, or ignore) */
		HIT_TEST        = SDL_WINDOWEVENT_HIT_TEST,       /**< Window had a hit test that wasn't SDL_HITTEST_NORMAL. */
		ICCPROF_CHANGED = SDL_WINDOWEVENT_ICCPROF_CHANGED,/**< The ICC profile of the window's display has changed. */
		DISPLAY_CHANGED = SDL_WINDOWEVENT_DISPLAY_CHANGED /**< Window has been moved to display data1. */
	};
}

namespace sdl::renderer
{
	enum
	{
		SOFTWARE      =  SDL_RENDERER_SOFTWARE     , /**< The renderer is a software fallback */
		ACCELERATED   =  SDL_RENDERER_ACCELERATED  , /**< The renderer uses hardware acceleration */
		PRESENTVSYNC  =  SDL_RENDERER_PRESENTVSYNC , /**< Present is synchronized with the refresh rate */
		TARGETTEXTURE =  SDL_RENDERER_TARGETTEXTURE, /**< The renderer supports rendering to texture */
	};
}

namespace img::init
{
	enum
	{
		JPG  = IMG_INIT_JPG,
		PNG  = IMG_INIT_PNG,
		TIF  = IMG_INIT_TIF,
		WEBP = IMG_INIT_WEBP,
		JXL  = IMG_INIT_JXL,
		AVIF = IMG_INIT_AVIF
	};
}

#endif