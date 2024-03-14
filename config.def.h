/* See LICENSE file for copyright and license details. */

/* appearance */
static unsigned int borderpx            = 2;        /* border pixel of windows */
static const unsigned int snap          = 32;       /* snap pixel */
static const int swallowfloating        = 0;        /* 1 means swallow floating windows by default */
static int showbar                      = 1;        /* 0 means no bar */
static int topbar                       = 1;        /* 0 means bottom bar */
static char centerfirst[]               = "1";
static const Bool viewontag             = True;     /* Switch view on tag switch */
static char font[]                      = "Fira Code:size=12" ;
static char dmenufont[]                 = "Fira Code:size=12";
static const char *fonts[]              = { font };
static char normbgcolor[]               = "#222222";
static char normbordercolor[]           = "#444444";
static char normfgcolor[]               = "#bbbbbb";
static char selfgcolor[]                = "#eeeeee";
static char selbordercolor[]            = "#005577";
static char selbgcolor[]                = "#005577";
static char *colors[][3] = {
    /*               fg           bg           border   */
    [SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
    [SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class            instance    title           tags-mask   isfloating  isterminal  noswallow   centerfirst     monitor */
	{ "Firefox-esr",    NULL,       NULL,           0,          0,          0,          0,          centerfirst,    -1 },
	{ "st-256color",    NULL,       NULL,           0,          0,          1,          0,          centerfirst,    -1 },
	{ NULL,             NULL,       "Event Tester", 0,          0,          0,          1,          0,              -1 }, /* xev */
};

/* layout(s) */
static float mfact     = 0.333; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static int defaultlayout = 0;
static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "|M|",      centeredmaster },
	{ "[]=",      tile },
	{ "><>",      NULL },
	{ "[M]",      monocle },
	{ ">M>",      centeredfloatingmaster },
	{ NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbordercolor, "-sf", selfgcolor, NULL };
static const char *termcmd[]  = { "st", NULL };
static const char scratchpadname[] = "scratchpad";
static const char *scratchpadcmd[] = { "st", "-t", scratchpadname, "-g", "120x34", NULL };

#include <X11/XF86keysym.h>

/* If you use pipewire add somewhere in your constants definition section. Use "wpctl status" to
   find out the real sink ID, 0 is a placeholder here. */
static const char *upvol[]      = { "/usr/bin/wpctl",	"set-volume",	"@DEFAULT_AUDIO_SINK@",	"5%+",	  "--limit", "1", NULL };
static const char *downvol[]    = { "/usr/bin/wpctl",	"set-volume",	"@DEFAULT_AUDIO_SINK@",	"5%-",	  NULL };
static const char *mutevol[]    = { "/usr/bin/wpctl",	"set-mute",	"@DEFAULT_AUDIO_SINK@",	"toggle", NULL };

/* To use light add this to the constant definition section. Thanks Hritik14. */
static const char *light_up[]   = { "/usr/bin/light",   "-A", "5", NULL };
static const char *light_down[] = { "/usr/bin/light",   "-U", "5", NULL };

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
    NULL
};

#include "exitdwm.c"
#include "movestack.c"
static const Key keys[] = {
    /* modifier                     key         function                argument */
	{ MODKEY,                       XK_b,       togglebar,              {0} },
	{ MODKEY,                       XK_d,       incnmaster,             {.i = -1 } },
    { MODKEY,                       XK_f,       spawn,                  SHCMD ("firefox") },
	{ MODKEY,                       XK_h,       setmfact,               {.f = -0.05} },
	{ MODKEY,                       XK_i,       incnmaster,             {.i = +1 } },
	{ MODKEY,                       XK_j,       focusstack,             {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_j,       movestack,              {.i = +1 } },
	{ MODKEY,                       XK_k,       focusstack,             {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_k,       movestack,              {.i = -1 } },
	{ MODKEY,                       XK_l,       setmfact,               {.f = +0.05} },
	{ MODKEY,                       XK_p,       spawn,                  {.v = dmenucmd } },
	{ MODKEY,                       XK_q,       killclient,             {0} },
	{ MODKEY|ShiftMask,             XK_q,       exitdwm,                {0} },
	{ MODKEY,                       XK_s,       togglescratch,          {.v = scratchpadcmd } },
	{ MODKEY,                       XK_Return,  zoom,                   {0} },
	{ MODKEY|ShiftMask,             XK_Return,  spawn,                  {.v = termcmd } },
	{ MODKEY,                       XK_Tab,     view,                   {0} },
	{ MODKEY,                       XK_space,   setlayout,              {0} },
	{ MODKEY|ShiftMask,             XK_space,   togglefloating,         {0} },
	{ MODKEY,                       XK_0,       view,                   {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,       tag,                    {.ui = ~0 } },
	{ MODKEY,                       XK_comma,   focusmon,               {.i = -1 } },
	{ MODKEY,                       XK_period,  focusmon,               {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,   tagmon,                 {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period,  tagmon,                 {.i = +1 } },
	{ MODKEY|ControlMask,           XK_comma,   cyclelayout,            {.i = -1 } },
	{ MODKEY|ControlMask,           XK_period,  cyclelayout,            {.i = +1 } },
    { 0,                            XF86XK_AudioLowerVolume,    spawn,  {.v = downvol } },
    { 0,                            XF86XK_AudioMute,           spawn,  {.v = mutevol } },
    { 0,                            XF86XK_AudioRaiseVolume,    spawn,  {.v = upvol   } },
    { 0,                            XF86XK_MonBrightnessUp,     spawn,  {.v = light_up} },
    { 0,                            XF86XK_MonBrightnessDown,   spawn,  {.v = light_down} },
	TAGKEYS(                        XK_1,                               0)
	TAGKEYS(                        XK_2,                               1)
	TAGKEYS(                        XK_3,                               2)
	TAGKEYS(                        XK_4,                               3)
	TAGKEYS(                        XK_5,                               4)
	TAGKEYS(                        XK_6,                               5)
	TAGKEYS(                        XK_7,                               6)
	TAGKEYS(                        XK_8,                               7)
	TAGKEYS(                        XK_9,                               8)
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
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

