/* ----------------------------------------------
*/
#include "specs.h"
#include "adoublehelper.h"

/* ------------------------- */
STATIC void test373()
{
char *name  = "t373 old file name";
char *name1 = "t373 new file name";
u_int16_t vol = VolID;
int tp,tp1;
int  ofs =  3 * sizeof( u_int16_t );
struct afp_filedir_parms filedir;
DSI *dsi = &Conn->dsi; 
u_int16_t bitmap;
u_int32_t mdate = 0;

	enter_test();
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPCopyFile:test373: copyFile check meta data, file without resource fork\n");

	if (!Mac && !Path) {
		test_skipped(T_MAC_PATH);
		goto test_exit;
	}

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		nottested();
		goto fin;
	}

	if (!Mac && delete_unix_rf(Path, "", name)) {
		nottested();
		goto fin;	
	}

	fprintf(stderr,"sleep(2)\n");  
	sleep(2);
	tp = get_fid(Conn, vol, DIRDID_ROOT, name);
	if (!tp) {
		nottested();
		goto fin;
	}
	bitmap = (1<<DIRPBIT_MDATE);
	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name, bitmap,0)) {
		failed();
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
		mdate = filedir.mdate;
	}
	
	FAIL (FPCopyFile(Conn, vol, DIRDID_ROOT, vol, DIRDID_ROOT, name, "", name1))

	if (FPGetFileDirParams(Conn, vol,  DIRDID_ROOT , name1, bitmap,0)) {
		failed();
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
		if (mdate != filedir.mdate)  {
	        fprintf(stderr,"\tFAILED modification date differ\n");  
	        failed_nomsg();
	        goto fin;
		}
	}

	tp1 = get_fid(Conn, vol, DIRDID_ROOT, name1);
	if (!tp1) {
		nottested();
		goto fin;
	}
	if (tp == tp1) {
	    fprintf(stderr,"\tFAILED both files have same ID\n");  
	    failed_nomsg();
	}

fin:
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name)) 
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT , name1))

test_exit:
	exit_test("test373");
}

/* ----------- */
void FPCopyFile_test()
{
    fprintf(stderr,"===================\n");
    fprintf(stderr,"FPCopyFile page 131\n");
	test373();
}

