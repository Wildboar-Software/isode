/* ftam-glob.c - interactive initiator FTAM -- globbing */

/*
 * C-shell glob for random programs.
 *
 * Modified for FTAM (and linted)
 *
 * Used by permission.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include "ftamuser.h"

#define	QUOTE 0200
#define	TRIM 0177
#define	eq(a,b)		(strcmp(a, b)==0)

#ifndef NCARGS		/* Not defined in S5R4 */
#define NCARGS          1000
#endif

#define	GAVSIZ		(NCARGS/6)
#define	isdir(d)	((d.st_mode & S_IFMT) == S_IFDIR)

static void rscan (char **t, int (*f)(char));
static void sort (void);
static void acollect (char *as);
static void collect (char *as);
static void expand (char *as);
static int execbrc (const char *p, const char *s);
static int match (const char *s, char *p);
static int amatch (const char *s, char *p);
static void Gcat (const char *s1, const char *s2);
static void addpath (const char c);
static int letter (const char c);
static int digit (const char c);
static int any (const int c, const char *s);
static int chkrdir (char *path, struct stat *st);
static int getrdir (char *hdir);
static void fatal (const char *s);

static char ** glob (char *v);
static void ginit (char **agargv);
static	char **gargv;		/* Pointer to the (stack) arglist */
static	int    gargc;		/* Number args in gargv */
static	int    gnleft;
static	int    gflag;
static	int tglob(char c);
char	*globerr;
static char *home;
static	char *strspl (const char *cp, const char *dp), **copyblk (char **v), *strend (char *cp);

static	int globcnt;

static char	*globchars = "`{[*?";

static	char *gpath, *gpathp, *lastgpathp;
static	int globbed;
static	char *entp;
static	char **sortbas;

static int chkldir (char *path, struct stat *st);
static int (*chkdir) (char *path, struct stat *st) = chkldir;

static int getldir (char *hdir);
static int (*gethdir) (char *hdir) = getldir;

static void matchldir (char *pattern);
static void matchrdir (char *pattern);
static void (*matchdir) (char *pattern) = matchldir;

static char ** glob (char *v) {
	char agpath[BUFSIZ];
	char *agargv[GAVSIZ];
	char *vv[2];
	vv[0] = malloc_plus_int (strlen (v), 1);
	if (vv[0] == (char *)0)
		fatal("out of memory");
	strcpy (vv[0], v);
	v = vv[0];
	vv[1] = 0;
	gflag = 0;
	rscan(vv, tglob);
	if (gflag == 0)
		return (copyblk(vv));

	globerr = 0;
	gpath = agpath;
	gpathp = gpath;
	*gpathp = 0;
	lastgpathp = &gpath[sizeof agpath - 2];
	ginit(agargv);
	globcnt = 0;
	collect(v);
	free(v);
	if (globcnt == 0 && (gflag&1)) {
		blkfree(gargv), gargv = 0;
		return (0);
	} else
		return (gargv = copyblk(gargv));
}

static void ginit (char **agargv) {
	agargv[0] = 0;
	gargv = agargv;
	sortbas = agargv;
	gargc = 0;
	gnleft = NCARGS - 4;
}

static void collect (char *as) {
	if (eq(as, "{") || eq(as, "{}")) {
		Gcat(as, "");
		sort();
	} else
		acollect(as);
}

static void acollect (char *as) {
	const int ogargc = gargc;

	gpathp = gpath;
	*gpathp = 0;
	globbed = 0;
	expand(as);
	if (gargc != ogargc)
		sort();
}

static void sort (void) {
	char **p1, **p2, *c;
	char **Gvp = &gargv[gargc];

	p1 = sortbas;
	while (p1 < Gvp-1) {
		p2 = p1;
		while (++p2 < Gvp)
			if (strcmp(*p1, *p2) > 0)
				c = *p1, *p1 = *p2, *p2 = c;
		p1++;
	}
	sortbas = Gvp;
}

static void expand (char *as) {
	char *cs,
		 *sgpathp,
		 *oldcs;
	char   *csstr;
	struct stat stb;

	sgpathp = gpathp;
	cs = csstr = strdup (as);
	if (*cs == '~' && gpathp == gpath) {
		addpath('~');
		for (cs++; letter(*cs) || digit(*cs) || *cs == '-';)
			addpath(*cs++);
		if (!*cs || *cs == '/') {
			if (gpathp != gpath + 1) {
				*gpathp = 0;
				if ((*gethdir) (gpath + 1))
					globerr = "Unknown user name after ~";
				strcpy(gpath, gpath + 1);
			} else
				strcpy(gpath, home);
			gpathp = strend(gpath);
		}
	}
	while (!any(*cs, globchars)) {
		if (*cs == 0) {
			if (!globbed)
				Gcat(gpath, "");
			else if (stat(gpath, &stb) >= 0) {
				Gcat(gpath, "");
				globcnt++;
			}
			goto endit;
		}
		addpath(*cs++);
	}
	oldcs = cs;
	while (cs > as && *cs != '/')
		cs--, gpathp--;
	if (*cs == '/')
		cs++, gpathp++;
	*gpathp = 0;
	if (*oldcs == '{') {
		execbrc(cs, ((char *)0));
		return;
	}
	(*matchdir) (cs);
endit:
	gpathp = sgpathp;
	*gpathp = 0;

	if (csstr)
		free (csstr);
}

static void matchldir (char *pattern) {
	char	pat[MAXPATHLEN];
	struct stat stb;
	struct dirent *dp;
	DIR *dirp;

	strcpy (pat, pattern);

	dirp = opendir(*gpath ? gpath : ".");
	if (dirp == NULL) {
		if (globbed)
			return;
		goto patherr2;
	}
	if (fstat(dirfd(dirp), &stb) < 0)
		goto patherr1;
	if (!isdir(stb)) {
		errno = ENOTDIR;
		goto patherr1;
	}
	for (errno = 0; dp = readdir(dirp); errno = 0) {
		if (dp->d_ino == 0)
			continue;
		if (match(dp->d_name, pat)) {
			Gcat(gpath, dp->d_name);
			globcnt++;
		}
	}
	if (errno)
		globerr = "corrupted directory";
	closedir(dirp);
	return;

patherr1:
	closedir(dirp);
patherr2:
	globerr = "Bad directory components";
}

static int execbrc (const char *p, const char *s) {
	char restbuf[BUFSIZ + 2];
	char *pe, *pm, *pl;
	int brclev = 0;
	char *lm, savec, *sgpathp;

	for (lm = restbuf; *p != '{'; *lm++ = *p++)
		continue;
	for (pe = ++p; *pe; pe++)
		switch (*pe) {

		case '{':
			brclev++;
			continue;

		case '}':
			if (brclev == 0)
				goto pend;
			brclev--;
			continue;

		case '[':
			for (pe++; *pe && *pe != ']'; pe++)
				continue;
			continue;
		}
pend:
	brclev = 0;
	for (pl = pm = p; pm <= pe; pm++)
		switch (*pm & (QUOTE|TRIM)) {

		case '{':
			brclev++;
			continue;

		case '}':
			if (brclev) {
				brclev--;
				continue;
			}
			goto doit;

		case ','|QUOTE:
		case ',':
			if (brclev)
				continue;
doit:
			savec = *pm;
			*pm = 0;
			strcpy(lm, pl);
			strcat(restbuf, pe + 1);
			*pm = savec;
			if (s == 0) {
				sgpathp = gpathp;
				expand(restbuf);
				gpathp = sgpathp;
				*gpathp = 0;
			} else if (amatch(s, restbuf))
				return (1);
			sort();
			pl = pm + 1;
			if (brclev)
				return (0);
			continue;

		case '[':
			for (pm++; *pm && *pm != ']'; pm++)
				continue;
			if (!*pm)
				pm--;
			continue;
		}
	if (brclev)
		goto doit;
	return (0);
}

static int match (const char *s, char *p) {
	int c;
	char *sentp;
	char sglobbed;

	if (int2char (globbed, &sglobbed) != 0)
		return (0);

	if (*s == '.' && *p != '.')
		return (0);
	sentp = entp;
	entp = s;
	c = amatch(s, p);
	entp = sentp;
	globbed = sglobbed;
	return (c);
}

static int amatch (const char *s, char *p) {
	int scc;
	int ok, lc;
	char *sgpathp;
	struct stat stb;
	int c, cc;

	globbed = 1;
	for (;;) {
		scc = *s++ & TRIM;
		switch (c = *p++) {

		case '{':
			return (execbrc(p - 1, s - 1));

		case '[':
			ok = 0;
			lc = 077777;
			while (cc = *p++) {
				if (cc == ']') {
					if (ok)
						break;
					return (0);
				}
				if (cc == '-') {
					if (lc <= scc && scc <= *p++)
						ok++;
				} else if (scc == (lc = cc))
					ok++;
			}
			if (cc == 0)
				if (ok)
					p--;
				else
					return 0;
			continue;

		case '*':
			if (!*p)
				return (1);
			if (*p == '/') {
				p++;
				goto slash;
			}
			s--;
			do {
				if (amatch(s, p))
					return (1);
			} while (*s++);
			return (0);

		case 0:
			return (scc == 0);

		default:
			if (c != scc)
				return (0);
			continue;

		case '?':
			if (scc == 0)
				return (0);
			continue;

		case '/':
			if (scc)
				return (0);
slash:
			s = entp;
			sgpathp = gpathp;
			while (*s)
				addpath(*s++);
			addpath('/');
			if ((*chkdir) (gpath, &stb))
				if (*p == 0) {
					Gcat(gpath, "");
					globcnt++;
				} else
					expand(p);
			gpathp = sgpathp;
			*gpathp = 0;
			return (0);
		}
	}
}

static int chkldir ( char   *path, struct stat *st) {
	return (stat (path, st) == 0 && (st -> st_mode & S_IFMT) == S_IFDIR);
}

static int Gmatch (const char *s, const char *p) {
	int scc;
	int ok, lc;
	int c, cc;

	for (;;) {
		scc = *s++ & TRIM;
		switch (c = *p++) {

		case '[':
			ok = 0;
			lc = 077777;
			while (cc = *p++) {
				if (cc == ']') {
					if (ok)
						break;
					return (0);
				}
				if (cc == '-') {
					if (lc <= scc && scc <= *p++)
						ok++;
				} else if (scc == (lc = cc))
					ok++;
			}
			if (cc == 0)
				if (ok)
					p--;
				else
					return 0;
			continue;

		case '*':
			if (!*p)
				return (1);
			for (s--; *s; s++)
				if (Gmatch(s, p))
					return (1);
			return (0);

		case 0:
			return (scc == 0);

		default:
			if ((c & TRIM) != scc)
				return (0);
			continue;

		case '?':
			if (scc == 0)
				return (0);
			continue;

		}
	}
}

static void Gcat (const char *s1, const char *s2) {
	int len = 0;

	if (add_sizet_to_int (&len, strlen (s1)) != 0
			|| add_sizet_to_int (&len, strlen (s2)) != 0
			|| add_int_to_int (&len, 1) != 0) {
		globerr = "Arguments too long";
		return;
	}

	if (len >= gnleft || gargc >= GAVSIZ - 1)
		globerr = "Arguments too long";
	else {
		gargc++;
		gnleft -= len;
		gargv[gargc] = 0;
		gargv[gargc - 1] = strspl(s1, s2);
	}
}

static void addpath (const char c) {
	if (gpathp >= lastgpathp)
		globerr = "Pathname too long";
	else {
		*gpathp++ = c;
		*gpathp = 0;
	}
}

static void rscan (char **t, int (*f)(char)) {
	char *p, c;

	while (p = *t++) {
		if (f == tglob)
			if (*p == '~')
				gflag |= 2;
			else if (eq(p, "{") || eq(p, "{}"))
				continue;
		while (c = *p++)
			(*f)(c);
	}
}

#ifdef	notdef
static scan (char **t, int (*f)(char)) {
	char *p, c;

	while (p = *t++)
		while (c = *p)
			*p++ = (*f)(c);
}
#endif

static int tglob (char c) {
	if (any(c, globchars))
		gflag |= c == '{' ? 2 : 1;
	return (c);
}

#ifdef	notdef
static trim (char c) {

	return (c & TRIM);
}
#endif

static int letter (const char c) {
	return (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_');
}

static int digit (const char c) {
	return (c >= '0' && c <= '9');
}

static int any (const int c, const char *s) {
	while (*s)
		if (*s++ == c)
			return(1);
	return(0);
}

int blklen (char **av) {
	int i = 0;
	while (*av++)
		i++;
	return (i);
}

char **blkcpy (char **oav, char **bv) {
	char **av = oav;
	while (*av++ = *bv++)
		continue;
	return (oav);
}

int blkfree (char **av0) {
	char **av = av0;
	while (*av)
		free(*av++);
	free((char *)av0);
}

static char *strspl (const char *cp, const char *dp) {
	int extra = 1;
	char *ep;

	if (add_sizet_to_int (&extra, strlen (dp)) != 0
			|| (ep = malloc_plus_int (strlen (cp), extra)) == (char *)0)
		fatal("out of memory");
	strcpy(ep, cp);
	strcat(ep, dp);
	return (ep);
}

static char **copyblk (char **v) {
	int n = blklen(v);
	char **nv;

	if (add_int_to_int (&n, 1) != 0
			|| (nv = (char **) malloc_nmemb (n, sizeof (char **))) == (char **)0)
		fatal("out of memory");

	return (blkcpy(nv, v));
}

static char *strend (char *cp) {
	while (*cp)
		cp++;
	return (cp);
}
/*
 * Extract a home directory from the password file
 * The argument points to a buffer where the name of the
 * user whose home directory is sought is currently.
 * We write the home directory of the user back there.
 */
static int getldir (char *hdir) {
	struct passwd *pp = getpwnam(hdir);
	if (pp == 0)
		return (1);
	strcpy(hdir, pp->pw_dir);
	return (0);
}

#undef	isdir

int	xglobbed;

static OID   matchoid;

char *xglob1val (char *v, const int remote) {
	char **gp;
	char   *cp, *gb[2];

	gb[0] = v;
	gb[1] = NULLCP;

	if ((gp = xglob (gb, remote)) == NULLVP)
		return NULLCP;

	if (gp[1]) {
		advise (NULLCP, "%s: ambiguous", v);
		blkfree (gp);
		return NULLCP;
	}

	cp = *gp;
	free ((char *) gp);

	return cp;
}

char **xglob (char **v, const int remote) {
	int    i;
	char  *cp,
		  **gp,
		  **vp;
	char   *loses;

	xglobbed = 0;

	if (!globbing) {
		char *dp;

		for (gp = vp = copyblk (v); *gp; gp++) {
			cp = remote ? str2file (*gp) : *gp;
			if ((dp = malloc_plus_int (strlen (cp), 1)) == NULLCP)
				fatal ("out of memory");
			strcpy (dp, cp);
			*gp = dp;
		}

		return vp;
	}

	if (remote) {
		switch (realstore) {
		case RFS_UNKNOWN:
			advise (NULLCP, "%s", rs_unknown);
			return NULLVP;

		case RFS_UNIX:
			home = "~";
			chkdir = chkrdir;
			gethdir = getrdir;
			matchdir = matchrdir;
			break;

		default:
			advise (NULLCP, "%s", rs_support);
			return NULLVP;
		}
	} else {
		home = myhome;
		chkdir = chkldir;
		gethdir = getldir;
		matchdir = matchldir;
	}

	for (i = 0, loses = NULL, vp = NULLVP; cp = *v++; ) {
		if ((gp = glob (remote ? str2file (cp) : cp)) == NULLVP) {
			if (!loses && globerr)
				loses = globerr;
			continue;
		}

		if (vp) {
			int    j;
			char **xp,
				 **yp;

			if ((j = blklen (gp)) > 1)
				xglobbed++;

			{
				int n = i;

				if (add_int_to_int (&n, j) != 0
						|| add_int_to_int (&n, 1) != 0
						|| (vp = (char **) realloc_nmemb (vp, n, sizeof *vp))
						== NULLVP)
					fatal ("out of memory");
			}

			for (xp = vp + i, yp = gp; *xp = *yp; xp++, yp++)
				continue;
			i += j;

			free ((char *) gp);
		} else if ((i = blklen (vp = gp)) > 1)
			xglobbed++;
	}

	if (vp == NULLVP || *vp == NULLCP) {
		if (!loses)
			loses = "no files match specification";
		advise (NULLCP, "%s", loses);

		if (vp) {
			blkfree (vp);
			vp = NULLVP;
		}
	}

	if (vp && debug)
		for (gp = vp; *gp; gp++)
			printf ("%d: \"%s\"\n", gp - vp, *gp);

	return vp;
}

static void matchrdir (char *pattern) {
	char  *cp;
	char    cwd[MAXPATHLEN],
			pat[MAXPATHLEN];
	struct FADUidentity faduids;
	struct FADUidentity *faduid = &faduids;
	struct filent *fi, *gi;

	strcpy (pat, pattern);

	switch (isdir (gpath, cwd, 1)) {
	case NOTOK:
	case OK:
		if (!globbed)
			globerr = "Bad directory components";
		return;

	default:
		if (cwd[0] == NULL)
			strcpy (cwd, gpath);
		cp = cwd + strlen (cwd) - 1;
		if (*cp == '/')
			*cp = 0;
		else {
			*++cp = '/';
			*++cp = 0;
		}
		cp = cwd;
		break;
	}

	faduid -> fa_type = FA_FIRSTLAST;
	faduid -> fa_firstlast = FA_FIRST;

	fdffnx (NOTOK, (struct PSAPdata *) 0, 1);
	getvf (cp, NULLCP, faduid, &vfs[VFS_FDF], fdffnx);

	fi = gi = filents, filents = NULL;
	fdffnx (NOTOK, (struct PSAPdata *) 0, 0);

	{
		const size_t len = strlen (cp);

		for (fi = gi; fi; fi = fi -> fi_next)
			if (strncmp (fi -> fi_name, cp, len) == 0)
				fi -> fi_entry = fi -> fi_name + len;
	}

	for (fi = gi; fi; fi = fi -> fi_next) {
		matchoid = fi -> fi_oid;
		if (match (fi -> fi_entry, pat)) {
			Gcat (gpath, fi -> fi_entry);
			globcnt++;
		}
	}

	filents = gi;
	fdffnx (NOTOK, (struct PSAPdata *) 0, 0);
}

static int chkrdir ( char   *path, struct stat *st) {
	return (oid_cmp (vfs[VFS_FDF].vf_oid, matchoid) == 0);
}

static int getrdir (char *hdir) {
	char    buffer[BUFSIZ];
	sprintf (buffer, "~%s", hdir);
	return (isdir (buffer, hdir, 1) != DONE);
}

static void fatal (const char *s) {
	adios (NULLCP, "%s", s);
}

int f_echo (char **vec) {
	char  **gb, **gp, *gs;

	if (*++vec && (gp = gb = xglob (vec, 1))) {
		char   *cp;

		for (cp = ""; *gp; gp++, cp = " ") {
			gs = rindex (*gp, '/');
			if (gs == NULL)
				printf ("%s%s", cp, *gp);
			else
				printf ("%s%s", cp, ++gs);
		}
		printf ("\n");

		blkfree (gb);
	}

	return OK;
}
