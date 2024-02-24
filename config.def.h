/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx      = 1;        /* border pixel of windows */
static const unsigned int snap          = 32;       /* snap pixel */
static const int swallowfloating        = 0;        /* 1 means swallow floating windows by default */
static int showbar                      = 0;        /* 0 means no bar */
static int topbar                       = 1;        /* 0 means bottom bar */
static char centerfirst[]               = "1";
static char font[]                      = "monospace:size=10" ;
static char dmenufont[]                 = "monospace:size=10";
static const char *fonts[]              = { font };
static const unsigned int baralpha      = 0xb4;
static const unsigned int borderalpha   = 0xb4;
static const double activeopacity       = 1.0f;     /* Window opacity when it's focused (0 <= opacity <= 1) */
static const double inactiveopacity     = 0.8f;     /* Window opacity when it's inactive (0 <= opacity <= 1) */
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
static const unsigned int alphas[][3] = {
    /*               fg      bg        border*/
    [SchemeNorm] = { OPAQUE, baralpha, borderalpha },
	[SchemeSel]  = { OPAQUE, baralpha, borderalpha },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class            instance    title           tags-mask   isfloating  isterminal  noswallow   focusopacity    unfocusopacity      centerfirst     monitor */
	{ "Gimp",           NULL,       NULL,           0,          0,          0,          0,          activeopacity,  inactiveopacity,    0,              -1 },
	{ "Firefox-esr",    NULL,       NULL,           0,          0,          0,          0,          activeopacity,  inactiveopacity,    centerfirst,    -1 },
	{ "st",             NULL,       NULL,           0,          0,          1,          0,          0.9,            inactiveopacity,    centerfirst,    -1 },
	{ NULL,             NULL,       "Event Tester", 0,          0,          0,          1,          1.0,            1.0,                0,              -1 }, /* xev */
};

/* layout(s) */
static float mfact     = 0.5; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 0; /* 1 will force focus on the fullscreen window */

static int defaultlayout = 0;
static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
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

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
	{ "font",               STRING,     &font },
    { "font",               STRING,     &dmenufont },
    { "background",         STRING,     &normbgcolor },
    { "background",         STRING,     &normbordercolor },
    { "foreground",         STRING,     &normfgcolor },
    { "color1",             STRING,     &selbgcolor },
    { "color1",             STRING,     &selbordercolor },
    { "foreground",         STRING,     &selfgcolor },
    { "showbar",          	INTEGER,    &showbar },
    { "topbar",          	INTEGER,    &topbar },
    { "nmaster",          	INTEGER,    &nmaster },
    { "mfact",      	 	FLOAT,      &mfact },
    { "defaultlayout",      INTEGER,    &defaultlayout },
    { "centerfirst",        STRING,    &centerfirst },
};

#include "exitdwm.c"
static const Key keys[] = {
    /* modifier                     key         function                argument */
    { MODKEY|ShiftMask,             XK_a,       changefocusopacity,     {.f = +0.025}},
	{ MODKEY,                       XK_b,       togglebar,              {0} },
	{ MODKEY,                       XK_d,       incnmaster,             {.i = -1 } },
    { MODKEY,                       XK_f,       spawn,                  SHCMD ("firefox") },
	{ MODKEY|ShiftMask,             XK_f,       setlayout,              {.v = &layouts[1]} },
	{ MODKEY,                       XK_h,       setmfact,               {.f = -0.05} },
	{ MODKEY,                       XK_i,       incnmaster,             {.i = +1 } },
	{ MODKEY,                       XK_j,       focusstack,             {.i = +1 } },
	{ MODKEY,                       XK_k,       focusstack,             {.i = -1 } },
	{ MODKEY,                       XK_l,       setmfact,               {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_m,       setlayout,              {.v = &layouts[2]} },
	{ MODKEY,                       XK_o,       setlayout,              {.v = &layouts[4]} },
	{ MODKEY,                       XK_p,       spawn,                  {.v = dmenucmd } },
	{ MODKEY,                       XK_q,       killclient,             {0} },
	{ MODKEY|ShiftMask,             XK_q,       exitdwm,                {0} },
    { MODKEY|ShiftMask,             XK_s,       changefocusopacity,     {.f = -0.025}},
    { MODKEY|ShiftMask,             XK_t,       setlayout,              {.v = &layouts[0]} },
	{ MODKEY,                       XK_u,       setlayout,              {.v = &layouts[3]} },
    { MODKEY|ShiftMask,             XK_x,       changeunfocusopacity,   {.f = -0.025}},
	{ MODKEY|ShiftMask,             XK_z,       changeunfocusopacity,   {.f = +0.025}},
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

