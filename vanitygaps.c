static void
setgaps(int oh, int ov, int ih, int iv)
{
	if (oh < 0) oh = 0;
	if (ov < 0) ov = 0;
	if (ih < 0) ih = 0;
	if (iv < 0) iv = 0;

	selmon->gappoh = oh;
	selmon->gappov = ov;
	selmon->gappih = ih;
	selmon->gappiv = iv;
	arrange(selmon);
}

static void
togglegaps(const Arg *arg)
{
	enablegaps = !enablegaps;
	arrange(NULL);
}

static void
defaultgaps(const Arg *arg)
{
	setgaps(gappoh, gappov, gappih, gappiv);
}

static void
incrgaps(const Arg *arg)
{
	setgaps(
		selmon->gappoh + arg->i,
		selmon->gappov + arg->i,
		selmon->gappih + arg->i,
		selmon->gappiv + arg->i
	);
}

static void
incrigaps(const Arg *arg)
{
	setgaps(
		selmon->gappoh,
		selmon->gappov,
		selmon->gappih + arg->i,
		selmon->gappiv + arg->i
	);
}

static void
incrogaps(const Arg *arg)
{
	setgaps(
		selmon->gappoh + arg->i,
		selmon->gappov + arg->i,
		selmon->gappih,
		selmon->gappiv
	);
}

static void
incrohgaps(const Arg *arg)
{
	setgaps(
		selmon->gappoh + arg->i,
		selmon->gappov,
		selmon->gappih,
		selmon->gappiv
	);
}

static void
incrovgaps(const Arg *arg)
{
	setgaps(
		selmon->gappoh,
		selmon->gappov + arg->i,
		selmon->gappih,
		selmon->gappiv
	);
}

static void
incrihgaps(const Arg *arg)
{
	setgaps(
		selmon->gappoh,
		selmon->gappov,
		selmon->gappih + arg->i,
		selmon->gappiv
	);
}

static void
incrivgaps(const Arg *arg)
{
	setgaps(
		selmon->gappoh,
		selmon->gappov,
		selmon->gappih,
		selmon->gappiv + arg->i
	);
}

static void
getgaps(Monitor *m, int *oh, int *ov, int *ih, int *iv, unsigned int *nc, float *mf, float *sf)
{
	unsigned int n, oe = enablegaps, ie = enablegaps;
	float mfacts = 0, sfacts = 0;
	Client *c;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++) {
		if (m->nmaster && n < m->nmaster)
			mfacts += c->cfact;
		else
			sfacts += c->cfact;
	}
	if (smartgaps && n == 1) {
		oe = 0; // outer gaps disabled when only one client
	}

	*oh = m->gappoh*oe; // outer horizontal gap
	*ov = m->gappov*oe; // outer vertical gap
	*ih = m->gappih*ie; // inner horizontal gap
	*iv = m->gappiv*ie; // inner vertical gap
	*nc = n;            // number of clients
	*mf = mfacts;       // total factor of master area
	*sf = sfacts;       // total factor of stack area
}

/* Mirror layout axis for flextile */
void
mirrorlayout(const Arg *arg) {
	if (!selmon->lt[selmon->sellt]->arrange)
		return;
	selmon->ltaxis[0] *= -1;
	selmon->pertag->ltaxes[selmon->pertag->curtag][0] = selmon->ltaxis[0];
	arrange(selmon);
}

/* Rotate layout axis for flextile */
void
rotatelayoutaxis(const Arg *arg) {
	if (!selmon->lt[selmon->sellt]->arrange)
		return;
	if (arg->i == 0) {
		if (selmon->ltaxis[0] > 0)
			selmon->ltaxis[0] = selmon->ltaxis[0] + 1 > 4 ? 1 : selmon->ltaxis[0] + 1;
		else
			selmon->ltaxis[0] = selmon->ltaxis[0] - 1 < -4 ? -1 : selmon->ltaxis[0] - 1;
	} else
		selmon->ltaxis[arg->i] = selmon->ltaxis[arg->i] + 1 > 4 ? 1 : selmon->ltaxis[arg->i] + 1;
	selmon->pertag->ltaxes[selmon->pertag->curtag][arg->i] = selmon->ltaxis[arg->i];
	arrange(selmon);
}

/*
 * Set predefined flextile layout.
 *
 * The arg int value is a binary representation of the setup where certain bits have different
 * meanings, similar to how Linux permissions work.
 *
 * The first two bits represents the stack axis, bits 3 and 4 the master axis. Bits 5 and 6
 * are used to control the layout while bit 7 indicates whether or not the layout is mirrored.
 * The 8th bit is reserved while bit 9 through 12 control nmaster with up to 15 clients in the
 * master stack.
 *
 * Bitwise layout:
 *
 *    0000          (nmaster: 0-15 = clients in master stack)
 *        0         (reserved)
 *         0        (orientation: 0 = normal, 1 = mirror)
 *          00      (layout: 00 = vertical, 01 = horizontal, 10 = centered (vert), 11 = centered (horz))
 *            00    (master axis: 00 = left to right, 01 = top to bottom, 10 = monocle, 11 = grid)
 *              00  (stack axis:  00 = left to right, 01 = top to bottom, 10 = monocle, 11 = grid)
 *
 * Examples:
 *    binary         int  layout
 *    --------------------------
 *    000000000110     6  monocle
 *    000100000110   262  deck layout
 *    000100010000   272  bstack layout
 *    000100010001   273  bstackhoriz layout
 *    000000000111     7  grid layout
 *    000100000101   261  default tile layout
 *    000100100101   293  centered master
 *    000100000111   263  default tile layout with grid stack
 */
void
setflexlayout(const Arg *arg)
{
	int i;

	/* Find flextile layout */
	for (i = 0; i < LENGTH(layouts); i++)
		if (layouts[i].arrange == flextile)
			break;

	selmon->nmaster = ((arg->i & 0x0F00) >> 8);
	selmon->ltaxis[0] = (1 + ((arg->i & 0x30) >> 4)) * (arg->i & 0x40 ? -1 : 1);
	selmon->ltaxis[1] = 1 + ((arg->i & 0xC) >> 2);
	selmon->ltaxis[2] = 1 + (arg->i & 0x3);

	selmon->pertag->nmasters[selmon->pertag->curtag] = selmon->nmaster;
	selmon->pertag->ltaxes[selmon->pertag->curtag][0] = selmon->ltaxis[0];
	selmon->pertag->ltaxes[selmon->pertag->curtag][1] = selmon->ltaxis[1];
	selmon->pertag->ltaxes[selmon->pertag->curtag][2] = selmon->ltaxis[2];

	setlayout(&((Arg) { .v = &layouts[i] }));
}

void
setcfact(const Arg *arg) {
	float f;
	Client *c;

	c = selmon->sel;

	if(!arg || !c || !selmon->lt[selmon->sellt]->arrange)
		return;
	f = arg->f + c->cfact;
	if(arg->f == 0.0)
		f = 1.0;
	else if(f < 0.25 || f > 4.0)
		return;
	c->cfact = f;
	arrange(selmon);
}

/* Flextile layout + gaps + cfacts + grid
 * https://dwm.suckless.org/patches/flextile/
 */
static void
flextile(Monitor *m) {
	unsigned int i, n, nc = 0, sc = 0, lt, cn = 0, rn = 0, cc = 0; // counters
	int cols = 1, rows = 1;
	int x, y, h, w;     // master x, y, height, width
	int sx, sy, sh, sw; // stack x, y, height, width
	int ox, oy;         // other stack x, y (centered layout)
	int oh, ov, ih, iv; // gaps outer/inner horizontal/vertical

	float facts, sfacts, ofacts;
	Client *c;

	getgaps(m, &oh, &ov, &ih, &iv, &n, &facts, &sfacts);
	setflexsymbols(m, n);

	if (n == 0)
		return;

	/* No outer gap if full screen monocle */
	if ((!m->nmaster && m->ltaxis[STACK] == MONOCLE) || (n <= m->nmaster && m->ltaxis[MASTER] == MONOCLE)) {
		ox = sx = x = m->wx;
		oy = sy = y = m->wy;
		sh = h = m->wh;
		sw = w = m->ww;
	} else {
		ox = sx = x = m->wx + ov;
		oy = sy = y = m->wy + oh;
		sh = h = m->wh - 2*oh;
		sw = w = m->ww - 2*ov;
	}
	ofacts = sfacts;
	sc = n - m->nmaster;


	/* Split master into master + stack if we have enough clients */
	if (m->nmaster && n > m->nmaster) {
		if (abs(m->ltaxis[LAYOUT]) == SPLIT_VERTICAL
				|| (abs(m->ltaxis[LAYOUT]) == SPLIT_CENTERED_V && n == m->nmaster + 1)) {
			sw = (w - iv) * (1 - m->mfact);
			w = (w - iv) * m->mfact;
			if (m->ltaxis[LAYOUT] < 0) // mirror
				x = sx + sw + iv;
			else
				sx = x + w + iv;
		} else if (abs(m->ltaxis[LAYOUT]) == SPLIT_HORIZONTAL
				|| (abs(m->ltaxis[LAYOUT]) == SPLIT_CENTERED_H && n == m->nmaster + 1)) {
			sh = (h - ih) * (1 - m->mfact);
			h = (h - ih) * m->mfact;
			if (m->ltaxis[LAYOUT] < 0) // mirror
				y = sy + sh + ih;
			else
				sy = y + h + ih;
		} else if (abs(m->ltaxis[LAYOUT]) == SPLIT_CENTERED_V) {
			sw = (w - 2*iv) * (1 - m->mfact) / 2;
			w = (w - 2*iv) * m->mfact;
			x = sx + sw + iv;
			if (m->ltaxis[LAYOUT] < 0) // mirror
				ox = x + w + iv;
			else
				sx = x + w + iv;
		} else if (abs(m->ltaxis[LAYOUT]) == SPLIT_CENTERED_H) {
			sh = (h - 2*ih) * (1 - m->mfact) / 2;
			h = (h - 2*ih) * m->mfact;
			y = sy + sh + ih;
			if (m->ltaxis[LAYOUT] < 0) // mirror
				oy = y + h + ih;
			else
				sy = y + h + ih;
		}

		if (abs(m->ltaxis[LAYOUT]) == SPLIT_CENTERED_V || abs(m->ltaxis[LAYOUT]) == SPLIT_CENTERED_H) {
			sc = (n - m->nmaster) / 2 + ((n - m->nmaster) % 2 > 0 ? 1 : 0);
			facts = sfacts = ofacts = 0;
			for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++) {
				if (i < m->nmaster)
					facts += c->cfact; // total factor of master area
				else if (sc && i < m->nmaster + sc)
					sfacts += c->cfact; // total factor of first stack area
				else
					ofacts += c->cfact; // total factor of second stack area
			}
		}
	}

	for (i = 0, lt = MASTER, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++) {
		if (i == 0 || (m->nmaster && i == m->nmaster) || i == (m->nmaster + sc)) {
			nc = MIN(n, m->nmaster);
			if (!m->nmaster || i == m->nmaster) { // switch to stack area
				x = sx, y = sy, h = sh, w = sw, facts = sfacts, lt = STACK;
				nc = sc;
			} else if (i > 0 && i == (m->nmaster + sc)) { // switch to second stack area
				x = ox, y = oy, h = sh, w = sw, nc = n - i, facts = ofacts;
			}

			if (m->ltaxis[lt] == LEFT_TO_RIGHT)
				w -= iv * (nc - 1);
			else if (m->ltaxis[lt] == TOP_TO_BOTTOM)
				h -= ih * (nc - 1);
			else if (m->ltaxis[lt] == GRID) {
				/* grid dimensions */
				for (cols = 1; cols <= nc/2; cols++)
					if (cols*cols >= nc)
						break;
				if (nc == 5) /* set layout against the general calculation: not 1:2:2, but 2:3 */
					cols = 2;
				rows = nc/cols;
				cn = rn = cc = 0; // reset cell no, row no, client count
			}
		}

		if (m->ltaxis[lt] == LEFT_TO_RIGHT) {
			resize(c, x, y, w * (c->cfact / facts) - (2*c->bw), h - (2*c->bw), 0);
			x = x + WIDTH(c) + iv;
		} else if (m->ltaxis[lt] == TOP_TO_BOTTOM) {
			resize(c, x, y, w - (2*c->bw), h * (c->cfact / facts) - (2*c->bw), 0);
			y = y + HEIGHT(c) + ih;
		} else if (m->ltaxis[lt] == MONOCLE) {
			resize(c, x, y, w - (2*c->bw), h - (2*c->bw), 0);
		} else if (m->ltaxis[lt] == GRID) {
			if (cc/rows + 1 > cols - nc%cols)
				rows = nc/cols + 1;
			resize(c,
				x + cn*((w - iv*(cols - 1)) / cols + iv),
				y + rn*((h - ih*(rows - 1)) / rows + ih),
				(w - iv*(cols - 1)) / cols,
				(h - ih*(rows - 1)) / rows,
				0);
			rn++;
			cc++;
			if (rn >= rows) {
				rn = 0;
				cn++;
			}
		}
	}

    if (n == 1 && selmon->sel->CenterThisWindow)
        resizeclient(selmon->sel,
                     m->wx + oh + (selmon->mw - selmon->mw * 0.5) / 2,
                     m->wy + ov + (selmon->mh - selmon->mh * 1) / 2,
                     selmon->mw * 0.5 - m->wx - 2*oh,
                     selmon->mh * 1 - m->wy - 2*ov);
}

static void
setflexsymbols(Monitor *m, unsigned int n) {
	char sym1 = 61, sym2 = 93, sym3 = 61, sym = 0;

	/* Predefined layouts */
	/* bstack */
	if (abs(m->ltaxis[LAYOUT]) == SPLIT_HORIZONTAL && m->ltaxis[MASTER] == LEFT_TO_RIGHT && m->ltaxis[STACK] == LEFT_TO_RIGHT) {
		snprintf(m->ltsymbol, sizeof m->ltsymbol, (m->ltaxis[LAYOUT] < 0 ? "⚍⚍⚍" : "⚎⚎⚎"));
		return;
	}

	/* bstackhoriz */
	if (abs(m->ltaxis[LAYOUT]) == SPLIT_HORIZONTAL && m->ltaxis[MASTER] == LEFT_TO_RIGHT && m->ltaxis[STACK] == TOP_TO_BOTTOM) {
		snprintf(m->ltsymbol, sizeof m->ltsymbol, (m->ltaxis[LAYOUT] < 0 ? "☳☳☳" : "☶☶☶"));
		return;
	}

	/* centered master horizontal split */
	if (abs(m->ltaxis[LAYOUT]) == SPLIT_CENTERED_H && m->ltaxis[MASTER] == TOP_TO_BOTTOM && m->ltaxis[STACK] == TOP_TO_BOTTOM) {
		snprintf(m->ltsymbol, sizeof m->ltsymbol, "☰☰☰");
		return;
	}

	if (abs(m->ltaxis[LAYOUT]) == SPLIT_CENTERED_H && m->ltaxis[MASTER] == LEFT_TO_RIGHT && m->ltaxis[STACK] == LEFT_TO_RIGHT) {
		snprintf(m->ltsymbol, sizeof m->ltsymbol, "☵☵☵");
		return;
	}

	/* monocle */
	if (n <= 1 && ((!m->nmaster && m->ltaxis[STACK] == MONOCLE) || (n <= m->nmaster && m->ltaxis[MASTER] == MONOCLE))) {
		snprintf(m->ltsymbol, sizeof m->ltsymbol, "[M]");
		return;
	}

	/* Layout symbols */
	if (abs(m->ltaxis[LAYOUT]) == SPLIT_VERTICAL) {
		if (m->nmaster > 1 || m->ltaxis[MASTER] == MONOCLE)
			sym2 = 124; // |
		else if (m->ltaxis[LAYOUT] < 0)
			sym2 = 91; // [
		else
			sym2 = 93; // ]
	} else if (abs(m->ltaxis[LAYOUT]) == SPLIT_HORIZONTAL) {
		if (m->nmaster > 1 || m->ltaxis[MASTER] == MONOCLE)
			sym2 = 58; // :
		else if (m->ltaxis[LAYOUT] < 0)
			sym2 = 91; // [
		else
			sym2 = 93; // ]
	} else if (abs(m->ltaxis[LAYOUT]) == SPLIT_CENTERED_V) {
		if (m->ltaxis[LAYOUT] < 0)
			sym2 = 87; // W
		else
			sym2 = 77; // M
	} else if (abs(m->ltaxis[LAYOUT]) == SPLIT_CENTERED_H) {
		if (m->ltaxis[LAYOUT] < 0)
			sym2 = 87; // W
		else
			sym2 = 77; // M
	}

	if (m->ltaxis[MASTER] == LEFT_TO_RIGHT)
		sym1 = 124; // | ⏸
	else if (m->ltaxis[MASTER] == TOP_TO_BOTTOM)
		sym1 = 61; // =
	else if (m->ltaxis[MASTER] == MONOCLE)
		sym1 = MIN(n, m->nmaster);
	else if (m->ltaxis[MASTER] == GRID)
		sym1 = 35; // #

	if (m->ltaxis[STACK] == LEFT_TO_RIGHT)
		sym3 = 124; // |
	else if (m->ltaxis[STACK] == TOP_TO_BOTTOM)
		sym3 = 61; // =
	else if (m->ltaxis[STACK] == MONOCLE)
		sym3 = n - m->nmaster;
	else if (m->ltaxis[STACK] == GRID)
		sym3 = 35; // #


	/* Generic symbols */
	if (!m->nmaster) {
		if (m->ltaxis[STACK] == MONOCLE) {
			snprintf(m->ltsymbol, sizeof m->ltsymbol, "%c%d%c", 91, sym3, 93);
		} else {
			snprintf(m->ltsymbol, sizeof m->ltsymbol, "%c%c%c", sym3, sym3, sym3);
		}
		return;
	}

	if (n <= m->nmaster) {
		if (m->ltaxis[MASTER] == MONOCLE) {
			snprintf(m->ltsymbol, sizeof m->ltsymbol, "%c%d%c", 91, sym1, 93);
		} else {
			snprintf(m->ltsymbol, sizeof m->ltsymbol, "%c%c%c", 91, sym1, 93);
		}
	} else {
		if (m->ltaxis[LAYOUT] < 0) {
			sym = sym1;
			sym1 = sym3;
			sym3 = sym;
		}
		if (m->nmaster == 1 && abs(m->ltaxis[LAYOUT]) <= SPLIT_HORIZONTAL && m->ltaxis[MASTER] != MONOCLE) {
			if (m->ltaxis[LAYOUT] > 0)
				sym1 = 91;
			else
				sym3 = 93;
		}
		if (m->ltaxis[MASTER] == MONOCLE && m->ltaxis[STACK] == MONOCLE)
			snprintf(m->ltsymbol, sizeof m->ltsymbol, "%d%c%d", sym1, sym2, sym3);
		else if ((m->nmaster && m->ltaxis[MASTER] == MONOCLE && m->ltaxis[LAYOUT] > 0) || (m->ltaxis[STACK] == MONOCLE && m->ltaxis[LAYOUT] < 0))
			snprintf(m->ltsymbol, sizeof m->ltsymbol, "%d%c%c", sym1, sym2, sym3);
		else if ((m->ltaxis[STACK] == MONOCLE && m->ltaxis[LAYOUT] > 0) || (m->nmaster && m->ltaxis[MASTER] == MONOCLE && m->ltaxis[LAYOUT] < 0))
			snprintf(m->ltsymbol, sizeof m->ltsymbol, "%c%c%d", sym1, sym2, n - m->nmaster);
		else
			snprintf(m->ltsymbol, sizeof m->ltsymbol, "%c%c%c", sym1, sym2, sym3);
	}
}
