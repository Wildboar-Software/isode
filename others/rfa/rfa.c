/*
 * RFA - Remote File Access
 *
 * Access and Management for a partial file system tree that exists
 * at two sites either as master files or slave files
 *
 * rfa.c : initiator for RFA commands
 *
 * Contributed by Oliver Wenzel, GMD Berlin, 1990
 *
 * $Header: /xtel/isode/isode/others/rfa/RCS/rfa.c,v 9.0 1992/06/16 12:47:25 isode Rel $
 *
 * $Log: rfa.c,v $
 * Revision 9.0  1992/06/16  12:47:25  isode
 * Release 8.0
 *
 */

#ifndef       lint
static char *rcsid = "$Header: /xtel/isode/isode/others/rfa/RCS/rfa.c,v 9.0 1992/06/16 12:47:25 isode Rel $";
#endif

/*
 *                              NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

#include <ctype.h>
#include "general.h"
#include <stdio.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "logger.h"
#include "RFA-ops.h"
#include "RFA-types.h"
#include "rfa.h"
#include "rfainfo.h"

#define START		retcode=OK
#define RETURN		return retcode
#define CONT(r)		{retcode=(r); continue;}

extern char *fsBase;
extern char *isodesbinpath;
extern char *isodetcpath;
extern char *strtok();
extern char *getRfaContext();

char *myname;
int connected = 0;
FILE *err, *out;
char cwd_remote[512];
int interactive = 1;
extern int commandMode;
int retcode;
int quit_command = 0;


/*--------------------------------------------------------------*/
/*  getLocalFileRfaInfo						*/
/*--------------------------------------------------------------*/
int
getLocalRfaInfo (char **fn, struct RfaInfo **rfap, struct RfaInfo **rfalp, int reg) {
	int rc;


	/*--- expand symbolic links in fn ---*/
	if((*fn = getRfaContext(cwd_remote, *fn)) == NULL) {
		fprintf(err,
				"*** local file access error : not within RFA subtree %s ***\n"
				, fsBase);
		return NOTOK_OUTOFSUBTREE;
	}
	if ((*fn = expandSymLinks(*fn)) == NULL) {
		fprintf(err, "*** local file access error : %s ***\n", rfaErrStr);
		return NOTOK_OUTOFSUBTREE;
	}

	/*--- get file Info ---*/
	if ((rc = getRfaInfoList(dirname(*fn), rfalp, basename(*fn), 1)) != OK) {
		fprintf(err,
				"*** local file access error : %s ***\n", errMsg(rc));
		return NOTOK_FILEACCESS;
	}

	if ((*rfap = findRfaInfo(basename(*fn), *rfalp)) == NULL) {
		releaseRfaInfoList(*fn,*rfalp);
		*rfalp = NULL;
		fprintf(err,"*** local file access error : %s does not exist ***\n",
				*fn);
		return NOTOK_FILEACCESS;
	}

	/*--- check if regular file ---*/
	if (reg)
		if (((*rfap)->ri_mode & S_IFMT) != S_IFREG) {
			releaseRfaInfoList(*fn,*rfalp);
			fprintf(err,"*** status error : not a regular file ***\n");
			*rfalp = NULL;
			return NOTOK_NOTREGULAR;
		}

	return OK;
}


/*--------------------------------------------------------------*/
/*  Local List Dir						*/
/*--------------------------------------------------------------*/
int
do_localListDir (char **av) {
	struct RfaInfo *rfa, *rfalist;
	char *fn;
	int rc;

	if (*av == NULL) {
		*av = "";
		*(av+1) = NULL;
	}

	START;
	for(; *av; av++) {
		if((fn = getRfaContext(cwd_remote, *av)) == NULL) {
			fprintf(err,
					"*** local file access error : not within RFA subtree %s ***\n",
					fsBase);
			CONT(NOTOK_OUTOFSUBTREE);
		}

		if ((rc = getRfaInfoList(fn, &rfalist, NULLCP, 0)) != OK) {
			fprintf(err, "*** local file access error : %s ***\n", errMsg(rc));
			CONT(NOTOK_FILEACCESS);
		}
		sortRfaInfoList(&rfalist);
		for (rfa = rfalist; rfa; rfa = rfa->ri_next)
			fprintf(out, "%s\n", (char *)rfa2ls(rfa));
		freeRfaInfoList(rfalist);
	}
	RETURN;
}

/*--------------------------------------------------------------*/
/*  List Dir							*/
/*--------------------------------------------------------------*/
int
do_listDir (char **av) {
	struct RfaInfo *rfa, *rfalist;
	int rc;
	char *fn;

	if (*av == NULL) {
		*av = "";
		*(av+1) = NULL;
	}

	START;
	for(; *av; av++) {
		if((fn = getRfaContext(cwd_remote, *av)) == NULL) {
			fprintf(err,
					"*** local file access error : not within RFA subtree %s ***\n",
					fsBase);
			CONT(NOTOK_OUTOFSUBTREE);
		}

		if ((rc= getRemoteRfaInfoList(fn, &rfalist)) != OK)
			CONT(rc);

		sortRfaInfoList(&rfalist);
		for (rfa = rfalist; rfa; rfa = rfa->ri_next)
			fprintf(out, "%s\n", (char *)rfa2ls(rfa));
		freeRfaInfoList(rfa);
	}
	RETURN;
}


/*--------------------------------------------------------------*/
/*  getRemoteRfaInfoList
/*--------------------------------------------------------------*/
int
getRemoteRfaInfoList (char *fn, struct RfaInfo **rfap) {
	struct type_RFA_FileName *arg;
	struct type_RFA_FileInfoList *fil;
	int res, rc;


	if (getConnection() != OK)
		return NOTOK_REMOTE_ERROR;

	arg = str2qb(fn, strlen(fn), 1);
	if (invoke(operation_RFA_listDir, (caddr_t)arg, (caddr_t *)&fil, &res) == NOTOK) {
		fprintf(err, "*** remote operation invocation failed ***\n");
		qb_free(arg);
		return NOTOK_REMOTE_ERROR;
	}
	qb_free(arg);

	if (res != RY_RESULT) {
		printError(res, (caddr_t)fil, &rc);
		return rc;
	}
	if ((*rfap = fi2rfa(fil)) == NULL) {
		fprintf(err,"*** local error : no memory ***\n");
		return NOTOK_LOCAL_ERROR;
	}
	return OK;
}


/*--------------------------------------------------------------*/
/*  Get File							*/
/*--------------------------------------------------------------*/
int
do_getFile (char **av) {
	int rc, new = 0;
	struct RfaInfo *rfalist, *rfa;
	char *fn, buf[512];
	int rmode = 0;

	START;
	for(; *av; av++) {

		if((fn = getRfaContext(cwd_remote, *av)) == NULL) {
			fprintf(err,
					"*** local file access error : not within RFA subtree %s ***\n",
					fsBase);
			CONT(NOTOK_OUTOFSUBTREE);
		}
		/*--- expand symbolic links in fn ---*/
		if ((fn = expandSymLinks(fn)) == NULL) {
			fprintf(err, "*** local file access error : %s ***", rfaErrStr);
			CONT(NOTOK_OUTOFSUBTREE);
		}

		/*--- get file Info ---*/
		if ((rc=getRfaInfoList(dirname(fn),&rfalist,basename(fn), 1)) != OK) {
			fprintf(err,
					"*** local file access error : can't read rfainfo (%s) ***\n",
					errMsg(rc));
			CONT(NOTOK_FILEACCESS);
		}

		if ((rfa = findRfaInfo(basename(fn), rfalist)) == NULL) {

			/*--- file does locally not exist, get it from master  ---*/
			if ((rfa = mallocRfaInfo(strdup(basename(fn)))) == NULL) {
				releaseRfaInfoList(dirname(fn), rfalist);
				fprintf(err, "*** local error : %s ***\n", rfaErrStr);
				CONT(NOTOK_LOCAL_ERROR);
			}
			SET_STATUS(rfa->ri_status, RI_SLAVE);
			SET_LOCKINFO(rfa->ri_status, RI_UNLOCKED);
			SET_TRANSFER(rfa->ri_status, default_transfer);
			time(&(rfa->ri_lastChange));
			rfa->ri_modTime = 0L;
			rfa->ri_next = rfalist;
			rfalist = rfa;
			new++;
		}

		/*--- check if file is master ---*/
		if (IS_MASTER(rfa->ri_status)) {
			releaseRfaInfoList(dirname(fn), rfalist);
			fprintf(err,"*** local file %s is master version ***\n", fn );
			CONT(NOTOK_GETMASTER);
		}

		/*--- check if file is unregistered ---*/
		if (IS_UNREGISTERED(rfa->ri_status)) {
			if(interactive) {
				fprintf(err,
						"unregistered local version of %s exists, overwrite ? (y/n) : ",
						fn);
				gets(buf);
				if ((*buf != 'y') && (*buf != 'Y')) {
					releaseRfaInfoList(dirname(fn), rfalist);
					CONT(NOTOK_UNREG_LOCAL_FILE);
				} else {
					SET_LOCKINFO(rfa->ri_status, RI_UNLOCKED);

					/*--- will be set in getfile_aux acc. to remote state ---*/
					SET_STATUS(rfa->ri_status, RI_SLAVE);
					SET_TRANSFER(rfa->ri_status, default_transfer);

					time(&(rfa->ri_lastChange));
					rfa->ri_modTime = 0L;
					new++;
				}
			} else {
				fprintf(err,
						"*** unregistered local version of %s exists ***\n", fn);
				releaseRfaInfoList(dirname(fn), rfalist);
				CONT(NOTOK_UNREG_LOCAL_FILE);
			}
		}

		/*--- otherwise we are slave or unregistered file, get from master ---*/
		if ((rc = getfile_aux(fn, rfa, &rmode)) != OK) {
			releaseRfaInfoList(dirname(fn), rfalist);
			CONT(rc);
		}

		if(new)
			if ((rc = putRfaInfoList(dirname(fn), rfalist)) != OK) {
				fprintf(err, "*** local file access error : %s ***\n",
						errMsg(rc));
				releaseRfaInfoList(dirname(fn), rfalist);
				CONT(NOTOK_FILEACCESS);
			}

		releaseRfaInfoList(dirname(fn), rfalist);

	}
	RETURN;
}


/*--------------------------------------------------------------*/
/*  unlockFile							*/
/*--------------------------------------------------------------*/
int
do_unlockFile (char **av) {
	int rc;
	struct RfaInfo *rfalist, *rfa;
	char *fn;

	START;
	for(; *av; av++) {

		/*--- get file Info ---*/
		fn = *av;
		if ((rc = getLocalRfaInfo(&fn, &rfa, &rfalist, 0)) != OK)
			CONT(rc);

		/*--- check if we are master ---*/
		if (IS_SLAVE(rfa->ri_status)) {
			releaseRfaInfoList(dirname(fn), rfalist);
			fprintf(err,"*** local file %s is slave version ***\n", fn);
			CONT(NOTOK_IS_SLAVE);
		}
		if (!IS_LOCKED(rfa->ri_status)) {
			releaseRfaInfoList(dirname(fn), rfalist);
			fprintf(err,"*** local file %s not locked ***\n", fn);
			CONT(NOTOK_NOTLOCKED);
		}
		SET_LOCKINFO(rfa->ri_status, RI_UNLOCKED);
		rfa->ri_lcksince = NULL;
		free(rfa->ri_lckname);
		rfa->ri_lckname = "NONE";

		if ((rc = putRfaInfoList(dirname(fn), rfalist)) != OK) {
			releaseRfaInfoList(dirname(fn), rfalist);
			fprintf(err, "*** local file access error : %s ***\n", errMsg(rc));
			CONT(NOTOK_FILEACCESS);
		}
		releaseRfaInfoList(dirname(fn), rfalist);
		if (interactive)
			fprintf(out,"unlocked file %s\n", fn);
	}
	RETURN;
}


/*--------------------------------------------------------------*/
/*  lockFile							*/
/*--------------------------------------------------------------*/
int
do_lockFile (char **av) {
	struct type_RFA_RequestMasterRes *rmr;
	struct type_RFA_RequestMasterArg *rma;
	int res, rc;
	struct RfaInfo *rfalist, *rfa;
	char *fn, *shortTime();
	int rmode = 0;

	START;
	for(; *av; av++) {

		/*--- get file Info ---*/
		fn = *av;
		if ((rc = getLocalRfaInfo(&fn, &rfa, &rfalist, 0)) != OK)
			CONT(rc);

		/*--- check if we are master ---*/
		if (IS_MASTER(rfa->ri_status) || IS_UNREGISTERED(rfa->ri_status)) {
			if (IS_LOCKED(rfa->ri_status)) {
				releaseRfaInfoList(dirname(fn), rfalist);
				fprintf(err,"*** file already locked by %s since %s ***\n",
						rfa->ri_lckname, shortTime(&(rfa->ri_lcksince)));
				CONT(NOTOK_LOCKED);
			}
			SET_LOCKINFO(rfa->ri_status, RI_LOCKED);
			rfa->ri_lckname = strdup(getenv("USER"));
			time(&(rfa->ri_lcksince));
			if ((rc = putRfaInfoList(dirname(fn), rfalist)) != OK) {
				releaseRfaInfoList(dirname(fn), rfalist);
				fprintf(err, "*** local file access error : %s ***\n",
						errMsg(rc));
				CONT(NOTOK_FILEACCESS);
			}
			releaseRfaInfoList(dirname(fn), rfalist);
			fprintf(out, "locked file %s\n",fn);
			continue;
		}

		/*--- otherwise we are slave, get from master ---*/
		if (getConnection() != OK)
			CONT(NOTOK_REMOTE_ERROR);

		if (rfa->ri_mode & S_IFMT & S_IFREG)
			if ((rc = getfile_aux(fn, rfa, &rmode)) != OK)  {
				releaseRfaInfoList(dirname(fn), rfalist);
				CONT(NOTOK_REMOTE_ERROR);
			}

		if ((rma = (struct type_RFA_RequestMasterArg *)
				   malloc(sizeof(struct type_RFA_RequestMasterArg))) == NULL) {
			releaseRfaInfoList(dirname(fn), rfalist);
			fprintf(err,"*** local error : no memory ***\n");
			CONT(NOTOK_LOCAL_ERROR);
		}

		rma->filename = str2qb(fn, strlen(fn), 1);
		rma->slaveVersion = rfa->ri_modTime;

		if (invoke(operation_RFA_requestMaster, (caddr_t)rma, (caddr_t *)&rmr, &res)
				== NOTOK) {
			releaseRfaInfoList(dirname(fn), rfalist);
			free_RFA_RequestMasterArg(rma);
			fprintf(err, "*** remote operation invocation failed ***\n");
			CONT(NOTOK_REMOTE_ERROR);
		}
		if (res != RY_RESULT) {
			releaseRfaInfoList(dirname(fn), rfalist);
			free_RFA_RequestMasterArg(rma);
			printError(res, (caddr_t)rmr, &rc);
			CONT(rc);
		}

		free_RFA_RequestMasterRes(rmr);
		free_RFA_RequestMasterArg(rma);
		SET_LOCKINFO(rfa->ri_status, RI_LOCKED);
		SET_STATUS(rfa->ri_status, RI_MASTER);
		time(&(rfa->ri_lastChange));
		rfa->ri_lckname = strdup(getenv("USER"));
		time(&(rfa->ri_lcksince));
		if ((rc = putRfaInfoList(dirname(fn), rfalist)) != OK) {
			releaseRfaInfoList(dirname(fn), rfalist);
			fprintf(err, "*** local file access error : %s ***\n", errMsg(rc));
			fprintf(err,"*** PANIC : can't set local Master status of %s ***\n",
					fn);
			CONT(NOTOK_FILEACCESS);
		}

		/*--- set owner and group mode to writable ---*/
		if (rmode && (rfa->ri_mode & S_IFMT & S_IFREG))
			if (changeFileMode(fn, rmode, "set write permissions") != OK)
				fprintf(err, "*** %s ***\n", rfaErrStr);

		releaseRfaInfoList(dirname(fn), rfalist);
		fprintf(out, "locked file %s\n",fn);
	}
	RETURN;
}

/*--------------------------------------------------------------*/
/*  master							*/
/*--------------------------------------------------------------*/
int
do_master (char **av) {
	int rc, res;
	struct RfaInfo *rfalist, *rfa, *remoteRfaList, *rrfa;
	char *fn, *shortTime();
	char  buf[512];
	struct type_RFA_RequestMasterRes *rmr;
	struct type_RFA_RequestMasterArg *rma;
	int rmode = 0;

	START;
	for(; *av; av++) {

		/*--- get file Info ---*/
		fn = *av;
		if ((rc = getLocalRfaInfo(&fn, &rfa, &rfalist, 0)) != OK)
			CONT(rc);

		/*--- check if not a .rfaexec file ---*/
		if (strcmp(rfa->ri_filename, ".rfaexec") == 0) {
			releaseRfaInfoList(dirname(fn), rfalist);
			fprintf(err," *** not allowed on file '%s' ***\n",rfa->ri_filename);
			CONT(NOTOK_NOT_ALLOWED);
		}

		/*--- check if unregistered ---*/
		if (IS_MASTER(rfa->ri_status)) {
			releaseRfaInfoList(dirname(fn), rfalist);
			fprintf(err," *** file %s is already master version ***\n", fn);
			CONT(NOTOK_ALREADY_MASTER);
		}

		/*--- check if remote is already master ---*/
		if ((rc = getRemoteRfaInfoList(dirname(fn), &remoteRfaList)) != OK) {
			if (interactive) {
				fprintf(err, "can't determine remote status, become master");
				fprintf(err, " anyway ? (y/n): ");
				gets(buf);
				if ((*buf != 'y') && (*buf != 'Y')) {
					releaseRfaInfoList(dirname(fn), rfalist);
					fprintf(err,"...aborted\n");
					CONT(NOTOK_REMOTE_ERROR);
				}
			} else {
				releaseRfaInfoList(dirname(fn), rfalist);
				fprintf(err,"*** can't determine remote status of %s ***\n",fn);
				CONT(NOTOK_REMOTE_ERROR);
			}
		} else {

			/*--- consistency checks ---*/
			if (rrfa = findRfaInfo(basename(fn), remoteRfaList)) {
				if (IS_MASTER(rrfa->ri_status)) {

					/*--- try to become master ---*/
					if (rfa->ri_mode & S_IFMT & S_IFREG)
						if ((rc = getfile_aux(fn, rfa, &rmode)) != OK)  {
							releaseRfaInfoList(dirname(fn), rfalist);
							freeRfaInfoList(remoteRfaList);
							CONT(NOTOK_REMOTE_ERROR);
						}

					if ((rma = (struct type_RFA_RequestMasterArg *)
							   malloc(sizeof(struct type_RFA_RequestMasterArg))) == NULL) {
						releaseRfaInfoList(dirname(fn), rfalist);
						freeRfaInfoList(remoteRfaList);
						fprintf(err,"*** local error : no memory ***\n");
						CONT(NOTOK_LOCAL_ERROR);
					}

					rma->filename = str2qb(fn, strlen(fn), 1);
					rma->slaveVersion = rfa->ri_modTime;

					if (invoke(operation_RFA_requestMaster, (caddr_t) rma,
							   (caddr_t *)&rmr, &res) ==NOTOK) {
						releaseRfaInfoList(dirname(fn), rfalist);
						free_RFA_RequestMasterArg(rma);
						freeRfaInfoList(remoteRfaList);
						fprintf(err,
								"*** remote operation invocation failed ***\n");
						CONT(NOTOK_REMOTE_ERROR);
					}
					if (res != RY_RESULT) {
						releaseRfaInfoList(dirname(fn), rfalist);
						free_RFA_RequestMasterArg(rma);
						printError(res, (caddr_t)rmr, &rc);
						CONT(rc);
					}
					free_RFA_RequestMasterArg(rma);
					free_RFA_RequestMasterRes(rmr);
				}
				if (IS_SLAVE(rrfa->ri_status)) {
					if (interactive) {
						fprintf(err, "remote version is already slave, become");
						fprintf(err, " master anyway ? (y/n): ");
						gets(buf);
						if ((*buf != 'y') && (*buf != 'Y')) {
							releaseRfaInfoList(dirname(fn), rfalist);
							freeRfaInfoList(remoteRfaList);
							fprintf(err,"...aborted\n");
							CONT(NOTOK_REMOTE_ERROR);
						}
					} else {
						releaseRfaInfoList(dirname(fn), rfalist);
						freeRfaInfoList(remoteRfaList);
						fprintf(err,
								"*** remote version of %s is already slave ***\n",fn);
						CONT(NOTOK_REMOTE_ERROR);
					}
				}
			}
			freeRfaInfoList(remoteRfaList);
		}

		if (IS_UNREGISTERED(rfa->ri_status))
			SET_TRANSFER(rfa->ri_status, default_transfer);
		SET_STATUS(rfa->ri_status, RI_MASTER);
		time(&(rfa->ri_lastChange));
		if ((rc = putRfaInfoList(dirname(fn), rfalist)) != OK) {
			releaseRfaInfoList(dirname(fn), rfalist);
			fprintf(err, "*** local file access error : %s ***\n", errMsg(rc));
			CONT(NOTOK_FILEACCESS);
		}

		/*--- set owner and group mode to writable ---*/
		if (rmode && (rrfa->ri_mode & S_IFMT & S_IFREG))
			if (changeFileMode(fn, rmode, "set write permissions") != OK)
				fprintf(err, "*** %s ***\n", rfaErrStr);

		releaseRfaInfoList(dirname(fn), rfalist);
		fprintf(out, "changed local version of %s to MASTER\n",fn);
	}
	RETURN;
}

/*--------------------------------------------------------------*/
/* slave							*/
/*--------------------------------------------------------------*/
int
do_slave (char **av) {
	int rc;
	struct RfaInfo *rfalist, *rfa, *remoteRfaList, *rrfa;
	char *fn, *shortTime();
	char  buf[512];

	START;
	for(; *av; av++) {

		/*--- get file Info ---*/
		fn = *av;
		if ((rc = getLocalRfaInfo(&fn, &rfa, &rfalist, 0)) != OK)
			CONT(rc);

		/*--- check if not a .rfaexec file ---*/
		if (strcmp(rfa->ri_filename, ".rfaexec") == 0) {
			releaseRfaInfoList(dirname(fn), rfalist);
			fprintf(err," *** not allowed on file '%s' ***\n",rfa->ri_filename);
			CONT(NOTOK_NOT_ALLOWED);
		}

		/*--- check if unregistered ---*/
		if (IS_SLAVE(rfa->ri_status)) {
			releaseRfaInfoList(dirname(fn), rfalist);
			fprintf(err," *** file %s already is slave version ***\n", fn);
			CONT(NOTOK_ALREADY_SLAVE);
		}

		/*--- check if unregistered ---*/
		if (IS_LOCKED(rfa->ri_status)) {
			releaseRfaInfoList(dirname(fn), rfalist);
			fprintf(err," *** file %s is currently locked ***\n", fn);
			CONT(NOTOK_LOCKED);
		}

		/*--- check if remote is already master ---*/
		if ((rc = getRemoteRfaInfoList(dirname(fn), &remoteRfaList)) != OK) {
			if (interactive) {
				fprintf(err, "can't determine remote status, become slave");
				fprintf(err, " anyway ? (y/n): ");
				gets(buf);
				if ((*buf != 'y') && (*buf != 'Y')) {
					releaseRfaInfoList(dirname(fn), rfalist);
					fprintf(err,"...aborted\n");
					CONT(NOTOK_REMOTE_ERROR);
				}
			} else {
				releaseRfaInfoList(dirname(fn), rfalist);
				fprintf(err,"*** can't determine remote status of %s ***\n",fn);
				CONT(NOTOK_REMOTE_ERROR);
			}
		} else {

			/*--- consistency checks ---*/
			if (rrfa = findRfaInfo(basename(fn), remoteRfaList)) {
				if (!IS_MASTER(rrfa->ri_status)) {
					releaseRfaInfoList(dirname(fn), rfalist);
					freeRfaInfoList(remoteRfaList);
					fprintf(err,
							"*** remote site is not master for file %s ***\n", fn);
					CONT(NOTOK_REMOTE_NOT_MASTER);
				}
				if (((rfa->ri_mode & S_IFMT & S_IFDIR) == 0)
						&& (rrfa->ri_modTime < rfa->ri_modTime)) {
					releaseRfaInfoList(dirname(fn), rfalist);
					freeRfaInfoList(remoteRfaList);
					fprintf(err,"*** remote master version of %s would be", fn);
					fprintf(err, " older than local slave version ***\n");
					CONT(NOTOK_REMOTE_MASTER_OLDER);
				}
			} else {
				releaseRfaInfoList(dirname(fn), rfalist);
				freeRfaInfoList(remoteRfaList);
				fprintf(err,
						"*** remote site is not master for file %s ***\n", fn);
				CONT(NOTOK_REMOTE_NOT_MASTER);
			}
			freeRfaInfoList(remoteRfaList);
		}

		if ((rfa->ri_mode & S_IFMT & S_IFDIR) == 0)
			if (makeFileReadOnly(fn, rfa) != OK)
				fprintf(err, "*** %s ***\n", rfaErrStr);

		SET_LOCKINFO(rfa->ri_status, RI_UNLOCKED);
		if (IS_UNREGISTERED(rfa->ri_status))
			SET_TRANSFER(rfa->ri_status, default_transfer);
		SET_STATUS(rfa->ri_status, RI_SLAVE);
		time(&(rfa->ri_lastChange));

		if ((rc = putRfaInfoList(dirname(fn), rfalist)) != OK) {
			releaseRfaInfoList(dirname(fn), rfalist);
			fprintf(err, "*** local file access error : %s ***\n", errMsg(rc));
			CONT(NOTOK_FILEACCESS);
		}
		releaseRfaInfoList(dirname(fn), rfalist);
		fprintf(out, "changed local version of %s to SLAVE\n",fn);
	}
	RETURN;
}

/*--------------------------------------------------------------*/
/* unregister							*/
/*--------------------------------------------------------------*/
int
do_unregister(char **av) {
	int rc;
	struct RfaInfo *rfalist, *rfa;
	char *fn;

	START;
	for(; *av; av++) {

		/*--- get file Info ---*/
		fn = *av;
		if ((rc = getLocalRfaInfo(&fn, &rfa, &rfalist, 0)) != OK)
			CONT(rc);

		/*--- check if unregistered ---*/
		if (IS_UNREGISTERED(rfa->ri_status)) {
			releaseRfaInfoList(dirname(fn), rfalist);
			fprintf(err," *** file %s already is unregistered ***\n", fn);
			CONT(NOTOK_ALREADY_UNREG);
		}

		/*--- check if locked ---*/
		if (IS_LOCKED(rfa->ri_status)) {
			releaseRfaInfoList(dirname(fn), rfalist);
			fprintf(err," *** file %s is currently locked ***\n", fn);
			CONT(NOTOK_LOCKED);
		}

		if ((rfa->ri_mode & S_IFMT & S_IFDIR) == 0)
			if (makeFileReadWrite(fn, rfa) != OK)
				fprintf(err, "*** %s ***\n", rfaErrStr);

		SET_LOCKINFO(rfa->ri_status, RI_UNLOCKED);
		SET_TRANSFER(rfa->ri_status, RI_TR_REQ);
		SET_STATUS(rfa->ri_status, RI_UNREGISTERED);
		time(&(rfa->ri_lastChange));

		if ((rc = putRfaInfoList(dirname(fn), rfalist)) != OK) {
			releaseRfaInfoList(dirname(fn), rfalist);
			fprintf(err, "*** local file access error : %s ***\n", errMsg(rc));
			CONT(NOTOK_FILEACCESS);
		}
		releaseRfaInfoList(dirname(fn), rfalist);
		fprintf(out, "changed local version of %s to UNREGISTERED\n",fn);
	}
	RETURN;
}


/*--------------------------------------------------------------*/
/*  rsyncdir							*/
/*--------------------------------------------------------------*/
int
do_rsyncdir (char **av) {
	char *fn;
	int rc;

	if (*av == NULL) {
		*av = "";
		*(av+1) = NULL;
	}
	START;
	for(; *av; av++) {
		if((fn = getRfaContext(cwd_remote, *av)) == NULL) {
			fprintf(err,
					"*** local file access error : not within RFA subtree %s ***\n",
					fsBase);
			CONT(NOTOK_OUTOFSUBTREE);
		}
		if ((fn = expandSymLinks(fn)) == NULL) {
			fprintf(err, "*** local file access error : %s ***\n", rfaErrStr);
			CONT(NOTOK_OUTOFSUBTREE);
		}
		if((rc = syncDir(fn, 1)) != OK)
			CONT(rc);
	}
	RETURN;
}


/*--------------------------------------------------------------*/
/*  syncdir							*/
/*--------------------------------------------------------------*/
int
do_syncdir (char **av) {
	int rc;
	char *fn;

	if (*av == NULL) {
		*av = "";
		*(av+1) = NULL;
	}

	START;
	for(; *av; av++) {
		if((fn = getRfaContext(cwd_remote, *av)) == NULL) {
			fprintf(err,
					"*** local file access error : not within RFA subtree %s ***\n",
					fsBase);
			CONT(NOTOK_OUTOFSUBTREE);
		}
		if ((fn = expandSymLinks(fn)) == NULL) {
			fprintf(err, "*** local file access error : %s ***\n", rfaErrStr);
			CONT(NOTOK_OUTOFSUBTREE);
		}

		if((rc = syncDir(fn, 0)) != OK)
			CONT(rc);
	}
	RETURN;
}


/*--------------------------------------------------------------*/
/*  settransfer							*/
/*--------------------------------------------------------------*/
int
do_setreq (char **av) {
	do_settransfer(av, RI_TR_REQ);
}

int
do_setauto (char **av) {
	do_settransfer(av, RI_TR_AUTO);
}

int
do_settransfer (char **av, int mode) {
	int rc;
	struct RfaInfo *rfalist, *rfa;
	char *fn, *shortTime();

	START;
	for(; *av; av++) {

		/*--- get file Info ---*/
		fn = *av;
		if ((rc = getLocalRfaInfo(&fn, &rfa, &rfalist, 0)) != OK)
			CONT(rc);

		SET_TRANSFER(rfa->ri_status, mode);

		if ((rc = putRfaInfoList(dirname(fn), rfalist)) != OK) {
			releaseRfaInfoList(dirname(fn), rfalist);
			fprintf(err, "*** local file access error : %s ***\n", errMsg(rc));
			CONT(NOTOK_FILEACCESS);
		}
		releaseRfaInfoList(dirname(fn), rfalist);
		fprintf(out, "set transfer mode to %s for file %s\n",
				mode == RI_TR_AUTO ? "AUTOMATIC" : "REQUEST", fn);
	}
	RETURN;
}


/*--------------------------------------------------------------*/
/*  Print WD							*/
/*--------------------------------------------------------------*/
int
do_pwd (char **av) {
	fprintf(out,"working directory: %s\n",*cwd_remote? cwd_remote : "/");
}

/*--------------------------------------------------------------*/
/*  Change Dir							*/
/*--------------------------------------------------------------*/
int
do_changeDir (char **av) {
	char *s, *r, *fn = *av;

	if (fn == NULL) {
		*cwd_remote = '\0';
		return OK;
	}
	if (*fn == '@')  {
		*cwd_remote = '\0';
		fn++;
	}

	for (s = strtok(fn, "/"); s; s = strtok(NULL, "/")) {
		if (strcmp(s, ".") == 0)
			continue;
		if (strcmp(s, "..") == 0) {
			if (*cwd_remote)
				if ( r = rindex(cwd_remote, '/'))
					*r = '\0';
				else
					*cwd_remote = '\0';
			continue;
		}
		strcat(cwd_remote, "/");
		strcat(cwd_remote, s);
	}
	return OK;
}

/*--------------------------------------------------------------*/
/*  timesync							*/
/*--------------------------------------------------------------*/
int
do_timesync (char **av) {
	struct type_RFA_SyncTimeArg sta;
	struct type_RFA_SyncTimeRes *str;
	int res, rc;
	time_t rt, lt, dt;
	char buf[BUFSIZ];

	if (getConnection() != OK)  {
		return NOTOK_REMOTE_ERROR;
	}

	if (timeSlave) {
		sta.role = int_RFA_role_slave;
		sta.time = 0;
	} else {
		sta.role = int_RFA_role_master;
		time(&(sta.time));
		sta.time += SENDTIME_DELAY;
	}
	if (invoke(operation_RFA_syncTime, (caddr_t)&sta, (caddr_t *)&str, &res)
			==NOTOK) {
		fprintf(err, "*** remote operation invocation failed ***\n");
		return (NOTOK_REMOTE_ERROR);
	}
	time(&lt);

	if (res != RY_RESULT) {
		printError(res, (caddr_t)str, &rc);
		return rc;
	}
	rt = str->parm;
	free_RFA_SyncTimeRes(str);
	if (timeSlave) {
		if (dt = rt - lt) {
			if (changeTime(dt) != OK) {
				fprintf(err, "*** %s ***\n", rfaErrStr);
				sprintf(buf, "%s/rfatime %ld", isodesbinpath, dt);
				if (system(buf) != OK)  {
					fprintf(err, "*** %s ***\n", rfaErrStr);
					return NOTOK_LOCAL_ERROR;
				}
			}
			if(dt > 0)
				fprintf(out, "advanced local time by %ld sec\n", dt);
			else
				fprintf(out, "retarding local time by %ld sec\n", dt);
		}
	} else {
		if(rt > 0)
			fprintf(out, "remote site advanced local time by %ld sec\n", rt);
		else if (rt < 0)
			fprintf(out,"remote site is retarding local time by %ld sec\n",rt);
	}
	return OK;
}

/*--------------------------------------------------------------*/
/*  quit							*/
/*--------------------------------------------------------------*/
int
do_quit (char **av) {
	quit_command++;
}


/*--------------------------------------------------------------*/
/*  Execute Command						*/
/*--------------------------------------------------------------*/
int
executeCommand (char *cmd) {
	char **ap, *aps[BUFSIZ];
	static struct cmd {
		char	*n;
		int	(*f)();
		char 	*h;
	} cmds[] = {
		{
			"get",	do_getFile,
			"update a local SLAVE file according to the remote MASTER"
		},
		{
			"lock",	do_lockFile,
			"request lock for a local file"
		},
		{
			"unlock",	do_unlockFile,
			"release lock for a local file"
		},
		{
			"rlist",	do_listDir,
			"list files in the remote directory"
		},
		{
			"list",		do_localListDir,
			"list files in a local directory"
		},
		{
			"pwd",	do_pwd,
			"print the current directory path"
		},
		{
			"cd",		do_changeDir,
			"change the current directory path"
		},
		{
			"master",	do_master,
			"make a local file a MASTER version"
		},
		{
			"slave",	do_slave,
			"make a local file a SLAVE version"
		},
		{
			"unregister",	do_unregister,
			"unregister a previously MASTER or SLAVE file"
		},
		{
			"setreq",	do_setreq,
			"set file transfer mode to 'request'"
		},
		{
			"setauto",	do_setauto,
			"set file transfer mode to 'automatic'"
		},
		{
			"syncdir",	do_syncdir,
			"synchronize files in a directory with the remote site"
		},
		{
			"rsyncdir",	do_rsyncdir,
			"recursively synchronize files in a directory with the remote site"
		},
		{
			"timesync",	do_timesync,
			"syncronize local time with masters clock"
		},
		{
			"quit",	do_quit,
			"terminate RFA session"
		},
		{
			"exit",	do_quit,
			"terminate RFA session"
		},
		{ NULL,		NULL,		NULL			}
	}, *cmdp, *fc;


	aps[0] = strtok (cmd, " ");
	for (ap = aps+1; *ap = strtok(NULL," "); ap++)
		;

	if (aps[0]) {
		if ((strncmp(*aps, "h", 1) == 0) || (strncmp(*aps, "?", 1) == 0)) {
			for (cmdp = cmds; cmdp->n; cmdp++)
				fprintf(out,"%8.8s - %s\n", cmdp->n, cmdp->h);
			return NOTOK;
		}

		fc = NULL;
		for (cmdp = cmds; cmdp->n; cmdp++)
			if (strncmp(cmdp->n, *aps, strlen(*aps)) == 0)  {
				if (fc) {
					fprintf(stderr,"command '%s' ambiguous, use one of\n",*aps);
					for (cmdp = cmds; cmdp->n; cmdp++)
						if (strncmp(cmdp->n, *aps, strlen(*aps)) == 0)
							fprintf(out,"    %8.8s - %s\n", cmdp->n, cmdp->h);
					return NOTOK;
				}
				fc = cmdp;
			}
		if (fc == NULL) {
			fprintf(err, "unknown command: %s\n",*aps);
			return NOTOK;
		}

		if (fc->f)
			return (*(fc->f))(aps+1);
		else
			return NOTOK;

	}
	return OK;
}

int
cleanup () {
}


int
getConnection () {
	if (connected)
		return OK;
	if (makeconn(host, passwd, user) == NOTOK)  {
		fprintf(err,"*** provider error: can't establish connection to %s ***\n"
		, host);
		return NOTOK;
	}
	connected = 1;
	return OK;
}


int
main (int ac, char **av) {
	char c, buf[BUFSIZ];
	extern char *optarg;
	extern int optind;
	char *cmd=NULL;
	int rc;

	host = "localhost";
	myname = av[0];

	out = stdout;
	err = stderr;

	isodetailor (myname, 1);

	/*-- create log file --*/
	initLog(myname);

	/*--- rfa tailoring ---*/
	sprintf(buf, "%s/rfatailor", isodetcpath);
	if (tailor(buf) != OK) {
		fprintf(stderr,"*** tailoring of file '%s' failed:%s\n",buf, rfaErrStr);
		exit(NOTOK_LOCAL_ERROR);
	}
	sprintf(buf,"%s/.rfarc", getenv("HOME"));
	if (tailor(buf) != OK) {
		fprintf(stderr,"*** tailoring of file '%s' failed:%s\n",buf, rfaErrStr);
		exit(NOTOK_LOCAL_ERROR);
	}

	while ((c = getopt(ac, av, "qu:p:c:h:")) != -1)
		switch (c) {
		case 'u':
			user = optarg;
			break;
		case 'p':
			passwd = optarg;
			break;
		case 'c':
			cmd = optarg;
			break;
		case 'h':
			host = optarg;
			break;
		case 'q':
			out = fopen("/dev/null", "w");
			interactive = 0;
			break;
		case '?':
			fprintf(stderr, "USAGE: %s [-h  hostname] [-u user] [-p password] [-c command] [ -q ]\n", basename(myname));
			exit(NOTOK_LOCAL_ERROR);
		}

	/*-- set uid, gid --*/
	if (initUserId(getuid(), getgid(), "") != OK)
		fprintf(err, "*** %s ***\n", rfaErrStr);

	/*--- init cwd ---*/
	getwd(buf);
	if (strncmp(buf, fsBase, strlen(fsBase)) == 0)
		strcpy(cwd_remote, buf + strlen(fsBase));

	if (cmd) {
		commandMode = 1;
		rc = executeCommand(cmd);
		if (connected)
			closeconn();
		exit(rc);
	}

	while (! quit_command) {
		printf("rfa-%s@%s> ", user, host);
		gets(buf);
		executeCommand(buf);
	}
	if (connected)
		closeconn();
	exit (0);
}



