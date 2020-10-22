#ifndef lint
static char *sccsid ="action.c	(CWI)	1.1	85/03/01";
#endif
#include "ideal.h"
#include "y.tab.h"

LINEPTR rbuild (noadtree, linelist)
NOADPTR noadtree;
LINEPTR linelist;
{
	STMTPTR slist[2];
	STMTPTR curstmt, opqstmt;
	NOADPTR nextnoad;
	int i;
	slist[0] = noadtree->defnode->parm->stmtlist;
	slist[1] = findbox (noadtree->defnode->parm->name,FALSE)->stmtlist;
	nextnoad = noadtree->son;
	if ((opqstmt = nextstmt (OPAQUE, slist[0])) || (opqstmt = nextstmt (OPAQUE, slist[1])))
		linelist = opqact (opqstmt, noadtree, linelist);
	if (noadtree->defnode->parm->name == lookup ("circle"))
		linelist = circact (noadtree, linelist);
	else if (noadtree->defnode->parm->name == lookup ("arc"))
		linelist = arcact (noadtree, linelist);
	for (i = 0; i < 2; i ++)
	for (curstmt = slist[i]; curstmt; curstmt = curstmt->next) {
	switch (curstmt->kind) {
		case '=':
			break;
		case CONN:
			linelist = connact (
				(EXPRPTR) curstmt->stmt,
				noadtree,
				linelist
			);
			break;
		case USING:
			linelist = penact (
				(PENPTR) curstmt->stmt,
				noadtree,
				linelist
			);
			break;
		case PUT:
			if (((PUTPTR)curstmt->stmt)->p_or_c == PUT)
				linelist = rbuild (nextnoad, linelist);
			else if (((PUTPTR)curstmt->stmt)->p_or_c == CONSTRUCT)
				nextnoad->linelist = rbuild (nextnoad, nextnoad->linelist);
			else impossible ("rbuild (PUT)");
			nextnoad = nextnoad->brother;
			break;
                        case DRAW:
			linelist = drawact (
				(MISCPTR) curstmt->stmt,
				noadtree,
				linelist
			);
			break;
		case STRING:
			linelist = stract (
				(STRPTR) curstmt->stmt,
				noadtree,
				linelist
			);
			break;
		case SPLINE:
			linelist = splact (
				(EXPRPTR) curstmt->stmt,
				noadtree,
				linelist
			);
			break;
		case OPAQUE:
			break;
		case BDLIST:
			break;
		case VAR:
			break;
		default:
			impossible ("rbuild");
		}
	}
	return (linelist);
}

LINEPTR build (noadtree, linelist)
NOADPTR noadtree;
LINEPTR linelist;
{
	LINEPTR retval;
	if (when_bug & 040) bug_on;
	retval = rbuild (noadtree, linelist);
	bug_off;
	return (retval);
}

LINEPTR connact (connstmt, noadtree, linelist)
EXPRPTR connstmt;
NOADPTR noadtree;
LINEPTR linelist;
{
	INTLPTR frompt, topt;
	LINEPTR newline;
	EXPRPTR linwalk;
	newline = NULL;
	frompt = expreval (connstmt->expr, noadtree);
	linwalk = connstmt->next;
	while (linwalk) {
		topt = expreval (linwalk->expr, noadtree);
		if (!known(frompt) || !known(topt)) {
			fprintf (stderr, "ideal: indeterminate endpoints\n   >>>conn ignored\n");
		} else {
			newline = linegen (
				Re(frompt), Im(frompt),
				Re(topt), Im(topt)
			);
			dprintf "Adding line (%f,%f) to (%f,%f)\n",
				Re(frompt), Im(frompt),
				Re(topt), Im(topt)
			);
			newline->next = linelist;
			linelist = newline;
		}
		intlfree (frompt);
		frompt = topt;
		linwalk = linwalk->next;
	}
	intlfree (topt);
	return (newline);
}

LINEPTR penact (penstmt, noadtree, linelist)
PENPTR penstmt;
NOADPTR noadtree;
LINEPTR linelist;
{
	INTLPTR frompt, topt, copies;
	LINEPTR newline;
	frompt = expreval (penstmt->from, noadtree);
	topt = expreval (penstmt->to, noadtree);
	copies = expreval (penstmt->copies, noadtree);
	newline = linelist;
	if (!known(frompt) || !known(topt) || !known(copies)) {
		fprintf (stderr, "ideal: indeterminate pen\n   >>>pen ignored\n");
	} else {
		PUTNODE dummyput;
		NOADPTR pennoad;
		STMTPTR ostmthead;
		int i;

		dprintf "Drawing pen from (%f,%f) to (%f,%f)\n",
			Re(frompt),
			Im(frompt),
			Re(topt),
			Im(topt)
		);

		/* add key statements to beginning of parameter section */
		dummyput.name = 0;
		dummypeqnwalk = nextstmt ('=', eqnwalk->next)) {
			INTLPTR junk;
			nl_fail = FALSE;
			junk = expreval ((EXPR) eqnwalk->stmt, noadtree);
			intlfree (junk);
			if (nl_fail) {
				EQNPTR nueqn;
				nueqn = eqngen (
					(EXPR) eqnwalk->stmt,
					noadtree
				);
				nueqn->next = nl_eqns;
				nl_eqns = nueqn;
				nl_fail = FALSE;
			}
			depvarclean ();
			incon_warn = TRUE;
		}
	reqneval (noadtree->son);
	reqneval (noadtree->brother);
}

void eqneval (noadtree)
NOADPTR noadtree;
{
	if (when_bug & 04) bug_on;
	reqneval (noadtree);
	bug_off;
}

void nl_eval ()
{
	static boolean nl_succ;
	INTLPTR junk;
	{
	EQNPTR nl_prev, nl_curr, nl_temp;
	if (when_bug & 010) bug_on;
	nl_prev = nl_curr = nl_eqns;
	nl_temp = NULL;
	while (nl_curr) {
		nl_curr = nl_prev->next;
		nl_prev->next = nl_temp;
		nl_temp = nl_prev;
		nl_prev = nl_curr;
	}
	nl_eqns = nl_temp;
	nl_succ = TRUE;
	}
	while (nl_eqns && nl_succ) {
		EQNPTR prev_eqn, nl_walk;
		EQNNODE dummy_eqn;
		dprintf "Retrying nonlinear equations\n");
		prev_eqn = &dummy_eqn;
		prev_eqn->next = nl_walk = nl_eqns;
		nl_succ = FALSE;
		while (nl_walk) {
			nl_fail = FALSE;
			junk = expreval (nl_walk->eqn, nl_walk->noad);
			intlfree (junk);
			depvarclean ();
			if (!nl_fail) {
				prev_eqn->next = nl_walk->next;
				tryfree(nl_walk);
				nl_walk = prev_eqn->next;
				nl_succ = TRUE;
			} else {
				prev_eqn = nl_walk;
				nl_walk = nl_walk->next;
			}
		}
		nl_eqns = dummy_eqn.next;
	}
	if (nl_eqns) {
		EQNPTR nl_walk, nl_next;
		dprintf "Nonlinear failure\n");
		nl_warn = TRUE;
		for (nl_walk = nl_eqns;
			nl_walk;
			nl_walk = nl_next) {
			junk = expreval (nl_walk->eqn, nl_walk->noad);
			intlfree (junk);
			depvarclean ();
			nl_next = nl_walk->next;
			tryfree(nl_walk);
		}
	}
	bug_off;
}

void depvarkill ()
{
	/* remove all unknown variables from depvarlist ...
	   no chance for them to be determined now  */
	if (!depvarlist)
		return;
	if (when_bug & 020)
		fprintf (stderr, "killing depvarlist\n");
	depfree (depvarlist);
	depvarlist = NULL;
}
TR depvarwalk;
		lefttemp = expstuff/ditroff/newideal/ideal.1                                                                         444    2570       0        25665  4045343317  11656                                                                                                                                                                                                                                                                                                                                                                      .\"	ideal.1	(CWI)	1.2	85/03/03
.EQ
.nr 99 \n(.s
.nr 98 \n(.f
.ps 10
.ft 2
.ps \n(99
.ft \n(98
.EN
.TH IDEAL 1 
.SH NAME
ideal \- troff preprocessor for drawing pictures
.SH SYNOPSIS
.B ideal
[
.BI \-p
[
.BI \-v
]
[
.BI \-4
]
[
.BI \-n
]
[ files ]
.SH DESCRIPTION
.I Ideal
is yet another
.IR troff (1)
preprocessor for drawing figures on a typesetter.
A line beginning `.IS' marks the start of an
.I ideal
program.
An
.I ideal
program ends with `.IE' or `.IF';
`.IE' leaves you below the bottom of the picture,
while `.IF' (flyback)
leaves you at the same place you were when you said `.IS'.
.PP
When invoked
with the
.I \-p
option,
.I ideal
produces 
.IR plot (1)
instructions.
The erases come at every `.IS', and they come fast and
furious, so you might prefer using the
.I \-4
option, which produces instructions for a Tektronix
4014, and waits at each `.IE' for an input character
before erasing and starting the next picture.
The
.I \-n
option produces raw
.IR ideal
output, which passes unharmed through
.IR nroff (1).
The
.I \-v
option produces
.IR ditroff (1)
input meant for the Versatec V-80, of course ditroff should know about
the Versatec by using the flag -Tver.
.PP
The building block for
.I ideal
programs is a ``box'';
boxes look like C functions,
in that they are named, and delimited by braces.
They may include the following kinds of statements,
each terminated by a semicolon:
.TP
.I var
declares one or more complex variables local to the box.
Variable names are made up of letters and digits, and
start with a letter; do not use any of the following
keywords as variable names:
at,
bdlist,
boundary,
box,
conn,
construct,
draw,
exterior,
interior,
left,
opaque,
put,
right,
spline,
text,
to,
using,
var
.TP
.I equation
declares relative positions of significant points of the box
.TP
.I conn
asks for a straight-line path through named points
.TP
.I pen
asks for a box to be replicated along a line between two points
.TP
.I left
left-justifies text with respect to a point
.TP
.I text
centers text with respect to a point
.TP
.I right
right-justifies text with respect to a point
.TP
.I spline
draws a spline guided by the named points
.TP
.I put
asks for an instance of a box to be drawn
.TP
.I opaque
asks for a box to erase lines already in the picture that
are covered by its bounding polygon
.TP
.I boundary
specifies the bounding polygon for an opaque box
.TP
.I construct
builds a partial picture on a separate ``sheet of paper''
.TP
.I draw
adds the contents of the named constructed box to the current picture
.PP
.I Ideal
expects all components of a picture to be specified as boxes;
instructions to draw the entire picture should comprise a box called ``main.''
Boxes are remembered across .IS-.IE boundaries;
if you won't need a box again, you can reclaim the
space it requires by including the command
`...forget boxname' on a line between any .IS-.IE
pair after the last use of boxname.
Box
.I main
is an exception to this rule:
it is always forgotten when the .IE is processed.
.PP
During its first pass,
.I ideal
solves all the equations to determine the locations of all points
it needs to know.
These equations must be linear equ