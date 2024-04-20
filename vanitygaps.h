/* Key binding functions */
static void defaultgaps(const Arg *arg);
static void incrgaps(const Arg *arg);
static void incrigaps(const Arg *arg);
static void incrogaps(const Arg *arg);
static void incrohgaps(const Arg *arg);
static void incrovgaps(const Arg *arg);
static void incrihgaps(const Arg *arg);
static void incrivgaps(const Arg *arg);
static void togglegaps(const Arg *arg);
/* Layouts */
static void flextile(Monitor *m);
/* Internals */
static void getgaps(Monitor *m, int *oh, int *ov, int *ih, int *iv, unsigned int *nc, float *mf, float *sf);
static void setgaps(int oh, int ov, int ih, int iv);
static void setcfact(const Arg *arg);
static void setflexsymbols(Monitor *m, unsigned int n);
static void setflexlayout(const Arg *arg);
static void rotatelayoutaxis(const Arg *arg);
static void mirrorlayout(const Arg *arg);

/* Settings */
static int enablegaps = 1;

/* Named flextile constants */
#define LAYOUT 0
#define MASTER 1
#define STACK 2
#define SPLIT_VERTICAL 1   // master stack vertical split
#define SPLIT_HORIZONTAL 2 // master stack horizontal split
#define SPLIT_CENTERED_V 3 // centered master vertical split
#define SPLIT_CENTERED_H 4 // centered master horizontal split
#define LEFT_TO_RIGHT 1    // clients are stacked horizontally
#define TOP_TO_BOTTOM 2    // clients are stacked vertically
#define MONOCLE 3          // clients are stacked in deck / monocle mode
#define GRID 4             // clients are stacked in grid mode
