/* See LICENSE file for copyright and license details. */

#include "exitdwm.c"
#include "movestack.c"
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const Bool viewontag         = True;     /* Switch view on tag switch */
static const char *fonts[]          = { "FiraCode Nerd Font:size=12" };
static const char *colors[][3]      = {
	/*               fg         bg         border   */
  [SchemeNorm] = { "#d8dee9", "#2e3440", "#4c566a" },
  [SchemeSel]  = { "#eceff4", "#4c566a", "#88c0d0" },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class          instance  title           tags mask   isfloating  CenterThisWindow? isterminal    noswallow  monitor */
	{ "Firefox",      NULL,     NULL,           0,          0,          1,                0,            0,        -1 },
  { "qutebrowser",  NULL,     NULL,           0,          0,          1,                0,            0,        -1 },
	{ "st",           NULL,     NULL,           0,          0,          1,                1,            0,        -1 },
  { NULL,           NULL,     "scratchpad",   0,          1,          0,                1,            1,        -1 },
	{ NULL,           NULL,     "Event Tester", 0,          0,          0,                0,            1,        -1 }, /* xev */
};

/* layout(s) */
static const float mfact     = 0.667; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 2;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
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
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, NULL };
static const char *termcmd[]  = { "st", NULL };
static const char scratchpadname[] = "scratchpad";
static const char *scratchpadcmd[] = { "st", "-t", scratchpadname, "-g", "120x34", NULL };

static const char *upvol[]      = { "/run/current-system/sw/bin/wpctl",	"set-volume",	"@DEFAULT_AUDIO_SINK@",	"5%+",	  "--limit", "1", NULL };
static const char *downvol[]    = { "/run/current-system/sw/bin/wpctl",	"set-volume",	"@DEFAULT_AUDIO_SINK@",	"5%-",	  NULL };
static const char *mutevol[]    = { "/run/current-system/sw/bin/wpctl",	"set-mute",	"@DEFAULT_AUDIO_SINK@",	"toggle", NULL };
static const char *media_prev[] = { "/run/current-system/sw/bin/playerctl", "previous", NULL };
static const char *media_next[] = { "/run/current-system/sw/bin/playerctl", "next", NULL };
static const char *media_pause[]= { "/run/current-system/sw/bin/playerctl", "pause", NULL };
static const char *media_play[] = { "/run/current-system/sw/bin/playerctl", "play-pause", NULL };
static const char *media_stop[] = { "/run/current-system/sw/bin/playerctl", "stop", NULL };
static const char *light_up[]   = { "/run/current-system/sw/bin/light",   "-A", "5", NULL };
static const char *light_down[] = { "/run/current-system/sw/bin/light",   "-U", "5", NULL };

static const Key keys[] = {
	/* modifier                     key         function        argument */
	{ MODKEY,                       XK_p,       spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_Return,  spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_s,       togglescratch,  {.v = scratchpadcmd } },
  { MODKEY,                       XK_b,       spawn,          SHCMD ("bwm") },
  { MODKEY,                       XK_w,       spawn,          SHCMD ("qutebrowser") },

	{ MODKEY,                       XK_j,       focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,       focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,       incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,       incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,       setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,       setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_j,       movestack,      {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,       movestack,      {.i = -1 } },

	{ MODKEY|ShiftMask,             XK_b,       togglebar,      {0} },
	{ MODKEY|ShiftMask,             XK_space,   togglefloating, {0} },
	{ MODKEY|ShiftMask,             XK_Return,  zoom,           {0} },
	{ MODKEY,                       XK_Tab,     view,           {0} },
	{ MODKEY,                       XK_q,       killclient,     {0} },
	{ MODKEY|ShiftMask,             XK_q,       exitdwm,       {0} },

	{ MODKEY,                       XK_t,       setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,       setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,       setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,   setlayout,      {0} },

	{ MODKEY,                       XK_0,       view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,       tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,   focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period,  focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,   tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period,  tagmon,         {.i = +1 } },

  { 0,                            XF86XK_AudioLowerVolume,    spawn,  {.v = downvol } },
  { 0,                            XF86XK_AudioMute,           spawn,  {.v = mutevol } },
  { 0,                            XF86XK_AudioRaiseVolume,    spawn,  {.v = upvol } },
  { 0,                            XF86XK_MonBrightnessUp,     spawn,  {.v = light_up } },
  { 0,                            XF86XK_MonBrightnessDown,   spawn,  {.v = light_down } },
  { 0,                            XF86XK_AudioPrev,           spawn,  {.v = media_prev } },
  { 0,                            XF86XK_AudioNext,           spawn,  {.v = media_next } },
  { 0,                            XF86XK_AudioPause,          spawn,  {.v = media_pause } },
  { 0,                            XF86XK_AudioPlay,           spawn,  {.v = media_play } },
  { 0,                            XF86XK_AudioStop,           spawn,  {.v = media_stop } },

	TAGKEYS(                        XK_1,                       0)
	TAGKEYS(                        XK_2,                       1)
	TAGKEYS(                        XK_3,                       2)
	TAGKEYS(                        XK_4,                       3)
	TAGKEYS(                        XK_5,                       4)
	TAGKEYS(                        XK_6,                       5)
	TAGKEYS(                        XK_7,                       6)
	TAGKEYS(                        XK_8,                       7)
	TAGKEYS(                        XK_9,                       8)
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

