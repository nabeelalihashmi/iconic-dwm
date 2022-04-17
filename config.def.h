/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  				    = 2;        /* border pixel of windows */
static const unsigned int dmenulines  				    = 20;        /* border pixel of windows */
static const unsigned int dmenug      				    = 4;        /* border pixel of windows */
static const unsigned int snap      				    = 8;
static const int swallowfloating    					= 0;        /* 1 means swallow floating windows by default */
static const unsigned int gappih    				    = 4;       /* horiz inner gap between windows */
static const unsigned int gappiv    				    = 4;       /* vert inner gap between windows */
static const unsigned int gappoh    				    = 4;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    				    = 4;       /* vert outer gap between windows and screen edge */
static       		  int smartgaps             	    = 0;       /* snap pixel */
static const unsigned int systraypinning 			    = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft 			    = 1;   	/* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing 			    = 2;   /* systray spacing */
static const 		  int systraypinningfailfirst 		= 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const 		  int showsystray        			= 1;     /* 0 means no systray */
static const 		  int showbar            			= 1;     /* 0 means no bar */
static const 		  int topbar             			= 1;     /* 0 means bottom bar */
static const 		  char *fonts[]          			= { "FontAwesome:size:11;Ubuntu:size=12" };
static const 		  char dmenufont[]       			= "Ubuntu:size=11";
static const 		  char col_gray1[]       			= "#222222";
static const 		  char col_gray2[]       			= "#444444";
static const 		  char col_gray3[]       			= "#bbbbbb";
static const 		  char col_gray4[]       			= "#eeeeee";
static const 		  char col_cyan[]        			= "#c3002e";
static const 		  char col_red[]        			= "#ff0000";
static const 		  char col_yellow[]        			= "#FFDE00";
static const 		  char *colors[][3]      			= {
	/*               			fg         bg          border   */
	[SchemeNorm] 		    = { col_gray3, col_gray1,  col_gray2  },
	[SchemeSel]  		    = { col_gray4, col_cyan,   col_red    },
	[SchemeStatus]  		= { col_gray3, col_gray1,  "#000000"  }, // Statusbar righrmt {text,background,not used but cannot be empty}
	[SchemeTagsSel]  		= { col_gray4, col_cyan,   "#000000"  }, // Tagbar left selected {text,background,not used but cannot be empty}
    [SchemeTagsNorm]  		= { col_gray3, col_gray1,  "#000000"  }, // Tagbar left unselected {text,background,not used but cannot be empty}
    [SchemeInfoSel]  		= { col_gray4, col_gray1,  "#000000"  }, // infobar middle  selected {text,background,not used but cannot be empty}
    [SchemeInfoNorm]  		= { col_gray3, col_gray1,  "#000000"  }, // infobar middle  unselected {text,background,not used but cannot be empty}
};

/* tagging */
static const char *tags[]   = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[]   = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class     				   instance   title        tags mask  isfloating  isterminal      noswallow     monitor */

	{ "Pavucontrol",               NULL,       NULL,	       	0,            1,          0,		   0           -1 },
	{ "Pcmanfm",                   NULL,       NULL,	       	0,            1,          0,		   0           -1 },
	{ "SimpleScreenRecorder",      NULL,       NULL,	       	0,            1,          0,		   0           -1 },
	{ "Nitrogen",                  NULL,       NULL,	       	0,            1,          0,		   0           -1 },
	{ "Lxappearance",              NULL,       NULL,	       	0,            1,          0,		   0           -1 },
	{ "Lxtask",                    NULL,       NULL,	       	0,            1,          0,		   0           -1 },
	{ "Synaptic",                  NULL,       NULL,	       	0,            1,          0,		   0           -1 },
	{ "Viewnior",                  NULL,       NULL,	       	0,            1,          0,		   0           -1 },
	{ "Audacious",                 NULL,       NULL,	       	0,            1,          0,		   0           -1 },
	{ "st",     				   NULL,       NULL,	        0,         	  0,          1,           0,          -1 },
	{ "kitty",     				   NULL,       NULL,	        0,         	  0,          1,           0,          -1 },
	{ "X-terminal-emulator",	   NULL,       NULL,	        0,         	  0,          1,           0,          -1 },
	{ NULL,      				   NULL,       "Event Tester", 	0,            0,          0,           1,          -1 },


};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[T]",      tile },    /* first entry is default */
	{ "[F]",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "[C]",      col },
	{ "[G]",      grid },
	{ "[B]",      bstack },
	{ "[BH]",     bstackhoriz },
	{ "[D]",      deck },
};

/* key definitions */
#define MODKEY Mod4Mask   // Window Key ? Super key
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
// ❯ dmenu_run -c -bw 2 -l 20 -g 4 -sb "#ff0000"
// static const char *dmenucmd[] = {"dmenu_run -c -bw  2 -l  20 -g  4" , "-fn", dmenufont, "-sb", col_cyan };
// static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
// static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, "-bw", "3" , "-l", "20", "-g", "3", "-c" ,NULL};
static const char *dmenucmd[] 				= { "dmenu_run", "-m", dmenumon, NULL};
static const char *termcmd[]  				= { "st", NULL };
static const char *alaccmd[]  				= { "kitty", NULL };
static const char *browsercmd[]  			= { "firefox", NULL };
static const char *filemanagercmd[]			= { "pcmanfm", NULL };
static const char *volctrlcmd[]  			= { "pavucontrol", NULL };
static const char *scrshotcmd[]				= { "xfce4-screenshooter", NULL };
static const char *scrshotfscmd[]  			= { "xfce4-screenshooter", "-f", NULL };
static const char *scrshotselcmd[]			= { "xfce4-screenshooter", "-r", NULL };
static const char *scrshotwincmd[]			= { "xfce4-screenshooter", "-w", NULL };
static const char *xkillcmd[]  				= { "xkill", NULL};
static const char *xtrlockcmd[]				= { "xtrlock", NULL};
static const char *slockcmd[]  				= { "slock", NULL};
static const char *wpccmd[]  				= { "nitrogen", "--random" , "--set-zoom-fill", NULL};
static const char *volupcmd[] 				= {"pactl", "set-sink-volume", "@DEFAULT_SINK@", "+5%", NULL};
static const char *voldowncmd[] 			= {"pactl", "set-sink-volume", "@DEFAULT_SINK@", "-5%", NULL};
static const char *voltoggcmd[]				= {"pactl", "set-sink-mute", "@DEFAULT_SINK@", "toggle", NULL};

static const char *brightness_up[] 			= {"sudo", "brightnessctl", "set", "+10%",  "-n", "2", NULL};
static const char *brightness_down[] 		= {"sudo", "brightnessctl", "set", "10%-","-n", "2", NULL};
static const char *powermenu[] 				= {"sessionmenu", NULL};
static const char *socialmenu[] 			= {"socialmenu", NULL};

#include "movestack.c"
static Key keys[] = {
	/* modifier                     		key             function        argument */

	{MODKEY|ShiftMask, 						XK_F3, 			spawn,          {.v = volupcmd }},
	{MODKEY|ShiftMask, 						XK_F2, 			spawn,          {.v = voldowncmd }},
	{MODKEY|ShiftMask, 						XK_F1,        	spawn,          {.v = voltoggcmd }},

	{MODKEY|ShiftMask, 						XK_F11,        	spawn,   		{.v = brightness_down }},
	{MODKEY|ShiftMask, 						XK_F12,        	spawn,   		{.v = brightness_up }},


	{ 0,          						  	XK_Print,  		spawn,          {.v = scrshotcmd } },
	{ ShiftMask,						  	XK_Print,  		spawn,          {.v = scrshotfscmd } },
	{ MODKEY|ControlMask,           		XK_Print,  		spawn,          {.v = scrshotselcmd } },
	{ MODKEY|ShiftMask,				  		XK_Print,  		spawn,          {.v = scrshotwincmd } },
	{ MODKEY|ShiftMask,			     		XK_v,      		spawn,          {.v = volctrlcmd } },
	{ MODKEY|ShiftMask,			     		XK_s,      		spawn,          {.v = socialmenu } },
	{ MODKEY|ShiftMask,			     		XK_w,      		spawn,          {.v = wpccmd } },
	{ MODKEY,                       		XK_p,      		spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             		XK_p,      		spawn,          {.v = powermenu } },
	{ MODKEY,                       		XK_space,  		spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             		XK_Return, 		spawn,          {.v = termcmd } },
	{ MODKEY|ShiftMask,             		XK_t, 	   		spawn,          {.v = alaccmd } },
	{ MODKEY,                       		XK_b,      		togglebar,      {0} },
	{ MODKEY|ShiftMask,             		XK_b,      		spawn,          {.v = browsercmd} },
	{ MODKEY|ShiftMask,             		XK_e,      		spawn,          {.v = filemanagercmd} },
	{ MODKEY|ShiftMask,             		XK_x,      		spawn,          {.v = xkillcmd} },
	{ Mod1Mask|ControlMask,			  		XK_l,      		spawn,          {.v = slockcmd} },
	{ Mod1Mask|ControlMask|ShiftMask,		XK_l,      		spawn,          {.v = xtrlockcmd} },
	{ MODKEY,                       		XK_j,		    focusstack,     {.i = +1 } },
	{ MODKEY,                       		XK_k,		    focusstack,     {.i = -1 } },
	{ MODKEY,                       		XK_i,		    incnmaster,     {.i = +1 } },
	{ MODKEY,                       		XK_d,		    incnmaster,     {.i = -1 } },
	{ MODKEY,                       		XK_h,		    setmfact,       {.f = -0.05} },
	{ MODKEY,                       		XK_l,		    setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,             		XK_h,		    setcfact,       {.f = +0.25} },
	{ MODKEY|ShiftMask,             		XK_l,		    setcfact,       {.f = -0.25} },
	{ MODKEY|ShiftMask,             		XK_o,		    setcfact,       {.f =  0.00} },
	{ MODKEY|ShiftMask,             		XK_j,		    movestack,      {.i = +1 } },
	{ MODKEY|ShiftMask,             		XK_k,		    movestack,      {.i = -1 } },
	{ MODKEY,                       		XK_Return,	    zoom,           {0} },
	{ MODKEY|ControlMask,              		XK_0,		    togglegaps,     {0} },
	{ MODKEY|ControlMask|ShiftMask,    		XK_0,		    defaultgaps,    {0} },
	{ MODKEY,                       		XK_Tab,		    view,           {0} },
	{ MODKEY|ShiftMask,             		XK_c,		    killclient,     {0} },
	{ Mod1Mask,                     		XK_F4,		    killclient,     {0} },
	{ MODKEY,                       		XK_t,		    setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       		XK_f,		    setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       		XK_m,		    setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       		XK_c,		    setlayout,      {.v = &layouts[3]} },
	{ MODKEY,                       		XK_g,		    setlayout,      {.v = &layouts[4]} },
	{ MODKEY,                       		XK_u,	        setlayout,      {.v = &layouts[5]} },
	{ MODKEY,                       		XK_o,   	    setlayout,      {.v = &layouts[6]} },
	{ MODKEY,                       		XK_e,   	    setlayout,      {.v = &layouts[7]} },
	{ MODKEY|ShiftMask,             		XK_space,		togglefloating, {0} },
	{ MODKEY|ShiftMask,             		XK_f,		    togglefullscr,  {0} },
	{ MODKEY,                       		XK_s,		    togglesticky,   {0} },
	{ MODKEY,                       		XK_0,		    view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             		XK_0,		    tag,            {.ui = ~0 } },
	{ MODKEY,                       		XK_comma,		focusmon,       {.i = -1 } },
	{ MODKEY,                       		XK_period,	    focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             		XK_comma,		tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             		XK_period,	    tagmon,         {.i = +1 } },
    { Mod1Mask|ControlMask,         		XK_BackSpace,   quit,           {0} },
	TAGKEYS(                        		XK_1,			0)
	TAGKEYS(                        		XK_2,			1)
	TAGKEYS(                        		XK_3,			2)
	TAGKEYS(                        		XK_4,			3)
	TAGKEYS(                        		XK_5,			4)
	TAGKEYS(                        		XK_6,			5)
	TAGKEYS(                        		XK_7,			6)
	TAGKEYS(                        		XK_8,			7)
	TAGKEYS(                        		XK_9,			8)
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

