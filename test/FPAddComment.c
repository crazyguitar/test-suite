/* ----------------------------------------------
*/
#include "specs.h"
/* -------------------------- */
STATIC void test55()
{
int fork;
char *name  = "t54 dir no access";
char *name1 = "t54 file.txt";
char *name2 = "t54 ro dir";
char *name3  = "t54 --rwx-- dir";
int pdir;
int rdir = 0;
int dir;
int ret;
u_int16_t vol = VolID;
u_int16_t vol2;
DSI *dsi2;
DSI *dsi = &Conn->dsi;
char *cmt;

    fprintf(stderr,"===================\n");
	fprintf(stderr, "FPAddComment:test55: add comment\n");
	if (!Conn2) {
		test_skipped(T_CONN2);
		return;
	}		

	if (!(pdir = no_access_folder(vol, DIRDID_ROOT, name))) {
		return;
	}
	if (!(rdir = read_only_folder(vol, DIRDID_ROOT, name2) ) ) {
		goto fin;
	}
	dsi2 = &Conn2->dsi;
	vol2  = FPOpenVol(Conn2, Vol);
	if (vol2 == 0xffff) {
		nottested();
		goto fin;
	}
	if (!(dir = FPCreateDir(Conn2,vol2, DIRDID_ROOT , name3))) {
		nottested();
		goto fin;
	}

	FAIL (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name1))

	cmt = "essai";
	ret = FPAddComment(Conn, vol,  DIRDID_ROOT , name, cmt);
	if (not_valid(ret, /* MAC */0, AFPERR_ACCESS)) {
		failed();
	}

	ret = FPAddComment(Conn, vol,  DIRDID_ROOT , name2, cmt);
	if (not_valid(ret, /* MAC */0, AFPERR_ACCESS)) {
		failed();
	}
	if (!ret) {
		ret = FPGetComment(Conn, vol,  DIRDID_ROOT , name2);
		if (ret || strncmp(cmt, dsi->commands +1, strlen(cmt))) {
			failed();
		}
	}

	FAIL (FPAddComment(Conn, vol,  DIRDID_ROOT , name1, "Comment for toto.txt"))

	if (FPAddComment(Conn, vol,  DIRDID_ROOT , name3, "Comment for test folder")) {
		failed();
	}
	if (FPRemoveComment(Conn, vol,  DIRDID_ROOT , name3)) {
		failed();
	}

	fork = FPOpenFork(Conn, vol, OPENFORK_DATA  , 0 ,DIRDID_ROOT, name1,OPENACC_RD );
	if (!fork) {
		failed();
		goto fin;
	}

	FAIL (FPAddComment(Conn, vol,  DIRDID_ROOT , name3, "Comment"))
	FAIL (FPGetComment(Conn, vol,  DIRDID_ROOT , name3)) 
	FAIL (FPRemoveComment(Conn, vol,  DIRDID_ROOT , name3)) 
	FAIL (FPCloseFork(Conn,fork))
#if 0
	if (ntohl(AFPERR_ACCESS) != FPAddComment(Conn, vol,  DIRDID_ROOT , "bogus folder","essai")) {
		fprintf(stderr,"\tFAILED\n");
		return;
	}
#endif	
fin:
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name1)) 
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name3)) 
	delete_folder(vol, DIRDID_ROOT, name);
	if (rdir) {
		delete_folder(vol, DIRDID_ROOT, name2);
	}
}

/* ----------- */
void FPAddComment_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPAddComment page 96\n");
	test55();
}

