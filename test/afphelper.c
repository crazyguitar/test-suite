/* ----------------------------------------------
*/
#include "specs.h"

/* -------------------------- */
#if 0
static char afp_cmd_with_fork[] = {
AFP_BYTELOCK,
AFP_CLOSEFORK,
AFP_GETFORKPARAM,
AFP_SETFORKPARAM,
AFP_READ,
AFP_FLUSHFORK,
AFP_WRITE,
};
#endif

void illegal_fork(DSI * dsi, char cmd, char *name)
{
u_int16_t vol = VolID;
int ofs;
int fork;
u_int16_t param;
u_int16_t bitmap = 0;

	if (FPCreateFile(Conn, vol,  0, DIRDID_ROOT , name)) {
		failed();
		return;
	}
	/* get an illegal fork descriptor */
	fork = FPOpenFork(Conn, vol, OPENFORK_DATA , bitmap ,DIRDID_ROOT, name, OPENACC_WR | OPENACC_RD);

	if (!fork) {
		failed();		
		FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name))
		return;
	}

	FAIL (FPCloseFork(Conn,fork))
	FAIL (FPDelete(Conn, vol,  DIRDID_ROOT, name))
	/* send the cmd with it */
	param  = fork;

	memset(dsi->commands, 0, DSI_CMDSIZ);
	dsi->header.dsi_flags = DSIFL_REQUEST;     
	dsi->header.dsi_command = DSIFUNC_CMD;
	dsi->header.dsi_requestID = htons(dsi_clientID(dsi));

	ofs = 0;
	dsi->commands[ofs++] = cmd;
	dsi->commands[ofs++] = 0;

	memcpy(dsi->commands +ofs, &param, sizeof(param));
	ofs += sizeof(param);
		
	dsi->datalen = ofs;
	dsi->header.dsi_len = htonl(dsi->datalen);
	dsi->header.dsi_code = 0; 
 
	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"AFP call  %d\n\n", cmd);
   	my_dsi_stream_send(dsi, dsi->commands, dsi->datalen);
	my_dsi_cmd_receive(dsi);
	dump_header(dsi);
		
    if (ntohl(AFPERR_PARAM) != dsi->header.dsi_code) {
		fprintf(stderr,"\tFAILED command %i\n", cmd);
		failed();
    }
}

/* ---------------------- */
int get_did(CONN *conn, u_int16_t vol, int dir, char *name)
{
int  ofs =  3 * sizeof( u_int16_t );
u_int16_t bitmap = (1<<DIRPBIT_DID );
struct afp_filedir_parms filedir;
DSI *dsi;

	dsi = &conn->dsi;

	filedir.did = 0;
	if (FPGetFileDirParams(conn, vol,  dir , name, 0, bitmap)) {
		nottested();
		return 0;
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi->data +ofs, 0, bitmap);
	if (!filedir.did) {
		nottested();
	}
	return filedir.did;
}

/* ---------------------- */
int get_fid(CONN *conn, u_int16_t vol, int dir, char *name)
{
int  ofs =  3 * sizeof( u_int16_t );
u_int16_t bitmap = (1<<FILPBIT_FNUM );
struct afp_filedir_parms filedir;
DSI *dsi = &conn->dsi;

	filedir.did = 0;
	if (FPGetFileDirParams(conn, vol,  dir , name, bitmap,0)) {
		failed();
	}
	else {
		filedir.isdir = 0;
		afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
		if (!filedir.did) {
			failed();
		}
	}
	return filedir.did;
}

/* ---------------------- */
u_int32_t get_forklen(DSI *dsi, int type)
{
u_int16_t bitmap = 0;
int len = (type == OPENFORK_RSCS)?(1<<FILPBIT_RFLEN):(1<<FILPBIT_DFLEN);
int  ofs =  sizeof( u_int16_t );
struct afp_filedir_parms filedir;
u_int32_t flen;

	filedir.isdir = 0;
	bitmap = len;
	afp_filedir_unpack(&filedir, dsi->data +ofs, bitmap, 0);
	flen = (type == OPENFORK_RSCS)?filedir.rflen:filedir.dflen;
	return flen;
}

/* ------------------------- */
void write_fork(CONN *conn, u_int16_t vol,int dir, char *name, char *txt)
{
int fork;
u_int16_t bitmap = 0;

	fork = FPOpenFork(conn, vol, OPENFORK_DATA , bitmap ,dir, name,OPENACC_WR | OPENACC_RD);
	if (!fork) {
		failed();
		return;
	}
	if (FPWrite(conn, fork, 0, strlen(txt), txt, 0 )) {
		failed();
	}	
	FAIL (FPCloseFork(conn,fork))
}

/* ------------------------- */
void read_fork(CONN *conn, u_int16_t vol,int dir, char *name,int len)
{
int fork;
u_int16_t bitmap = 0;

	fork = FPOpenFork(conn, vol, OPENFORK_DATA , bitmap ,dir, name,OPENACC_WR | OPENACC_RD);
	if (!fork) {
		failed();
		return;
	}
	memset(Data, 0, len +1);
	if (FPRead(conn, fork, 0, len, Data)) {
		failed();
	}
	FAIL (FPCloseFork(conn,fork))
}

/* ---------------------- 
 * Use the second user for creating a folder with no access right
 * assume did are the same for != user
*/
int no_access_folder(u_int16_t vol, int did, char *name)
{
int ret = 0;
int dir = 0;
u_int16_t vol2;
int  ofs =  3 * sizeof( u_int16_t );
u_int16_t bitmap =  (1 << DIRPBIT_ACCESS) | (1<<DIRPBIT_UID) | (1 << DIRPBIT_GID);
struct afp_filedir_parms filedir;
DSI *dsi, *dsi2;
u_int32_t uid;

    if (!Conn2) {
    	return 0;
    }
	fprintf(stderr,"\t>>>>>>>> Create no access folder <<<<<<<<<< \n");
	dsi2 = &Conn2->dsi;
	dsi = &Conn->dsi;
	vol2  = FPOpenVol(Conn2, Vol);
	if (vol2 == 0xffff) {
		nottested();
		return 0;
	}
	ret = FPGetUserInfo(Conn, 1, 0, 1); /* who I am */
	if (ret) {
		nottested();
		goto fin;
	}
	ret = FPGetUserInfo(Conn2, 1, 0, 1); /* who I am */
	if (ret) {
		nottested();
		goto fin;
	}
	memcpy(&uid, dsi2->commands + sizeof(u_int16_t), sizeof(u_int32_t));
	uid = ntohl(uid);
	
	if (!(dir = FPCreateDir(Conn2,vol2, did , name))) {
		nottested();
		if (!(dir = get_did(Conn2, vol2, did, name))) {
			goto fin;
		}
	}

	if (FPGetFileDirParams(Conn2, vol2,  dir , "", 0,bitmap )) {
		nottested();
		goto fin;
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi2->data +ofs, 0, bitmap);

    bitmap =  (1 << DIRPBIT_ACCESS);    
    filedir.access[0] = 0; 
    filedir.access[1] = 0; 
    filedir.access[2] = 0; 
    filedir.access[3] = 3;  /* was 7 */
 	if (FPSetDirParms(Conn2, vol2, dir , "", bitmap, &filedir)) {
		nottested();
		goto fin;
	}
	sleep(1);
	/* double check the first user can't create a dir in it */
	ret = get_did(Conn, vol, did, name);
	if (!ret) {
		if (ntohl(AFPERR_ACCESS) != dsi->header.dsi_code) {   
			goto fin;
		}
		/* 1.6.x fails here, so cheat a little, it doesn't work with did=last though */
		ret = dir;
	}
	if (FPCreateDir(Conn, vol, ret , name)) {
	    /* Mac OSX here does strange things 
	     * for when things go wrong */
		nottested();

	    FPEnumerate(Conn2, vol2,  DIRDID_ROOT , "", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) ,
		     (1<< DIRPBIT_ATTR) |  (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		     | (1<<DIRPBIT_UID) | (1 << DIRPBIT_GID));

	    FPEnumerate(Conn, vol,  ret , "", 
	         (1<<FILPBIT_LNAME) | (1<<FILPBIT_FNUM ) | (1<<FILPBIT_ATTR) ,
		     (1<< DIRPBIT_ATTR) |  (1<< DIRPBIT_LNAME) | (1<< DIRPBIT_PDID) | (1<< DIRPBIT_DID)|(1<< DIRPBIT_ACCESS)
		     | (1<<DIRPBIT_UID) | (1 << DIRPBIT_GID));
		
		FPDelete(Conn, vol,  ret, name);
    	bitmap =  (1 << DIRPBIT_ACCESS);    
    	filedir.access[0] = 0; 
    	filedir.access[1] = 0; 
    	filedir.access[2] = 0; 
    	filedir.access[3] = 7;  /* was 7 */
 		FPSetDirParms(Conn2, vol2, dir , "", bitmap, &filedir);
		FPDelete(Conn2, vol2,  dir, name); /* dir and ret should be the same */
		ret = 0;
	}
fin:
	if (!ret && dir) {
		if (FPDelete(Conn2, vol2,  did, name)) {
			nottested();
		}
	}
	FPCloseVol(Conn2,vol2);
	fprintf(stderr,"\t>>>>>>>> done <<<<<<<<<< \n");
	return ret;
}

/* ---------------------- */
int group_folder(u_int16_t vol, int did, char *name)
{
int ret = 0;
int dir = 0;
u_int16_t vol2;
int  ofs =  3 * sizeof( u_int16_t );
u_int16_t bitmap =  (1 << DIRPBIT_ACCESS);
struct afp_filedir_parms filedir;
DSI *dsi, *dsi2;

    if (!Conn2) {
    	return 0;
    }
	fprintf(stderr,"\t>>>>>>>> Create ---rwx--- folder <<<<<<<<<< \n");
	dsi2 = &Conn2->dsi;
	dsi = &Conn->dsi;
	vol2  = FPOpenVol(Conn2, Vol);
	if (vol2 == 0xffff) {
		nottested();
		return 0;
	}
	if (!(dir = FPCreateDir(Conn2,vol2, did , name))) {
		nottested();
		goto fin;
	}

	if (FPGetFileDirParams(Conn2, vol2,  dir , "", 0,bitmap )) {
		nottested();
		goto fin;
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi2->data +ofs, 0, bitmap);
    filedir.access[0] = 0; 
    filedir.access[1] = 0; 
    filedir.access[2] = 7; 
    filedir.access[3] = 0; 
 	if (FPSetDirParms(Conn2, vol2, dir , "", bitmap, &filedir)) {
		nottested();
		goto fin;
	}
	/* double check the first user can't create a dir in it */
	ret = get_did(Conn, vol, did, name);
	if (!ret) {
		if (ntohl(AFPERR_ACCESS) != dsi->header.dsi_code) {   
			goto fin;
		}
		/* 1.6.x fails here, so cheat a little, it doesn't work with did=last though */
		ret = dir;
	}
	if (FPCreateDir(Conn, vol, ret , name)) {
		nottested();
		FPDelete(Conn, vol,  ret, name);
		FPDelete(Conn, vol,  did, name);
		ret = 0;
	}
fin:
	if (!ret && dir) {
		if (FPDelete(Conn2, vol2,  did, name)) {
			nottested();
		}
	}
	FPCloseVol(Conn2,vol2);
	fprintf(stderr,"\t>>>>>>>> done <<<<<<<<<< \n");
	return ret;
}

/* ---------------------- 
 * Use the second user for creating a folder with read only access right
 * assume did are the same for != user
*/
int read_only_folder(u_int16_t vol, int did, char *name)
{
int ret = 0;
int dir = 0;
u_int16_t vol2;
int  ofs =  3 * sizeof( u_int16_t );
u_int16_t bitmap =  (1 << DIRPBIT_ACCESS);
struct afp_filedir_parms filedir;
DSI *dsi2;

    if (!Conn2) {
    	return 0;
    }
	fprintf(stderr,"\t>>>>>>>> Create read only folder <<<<<<<<<< \n");
	dsi2 = &Conn2->dsi;
	vol2  = FPOpenVol(Conn2, Vol);
	if (vol2 == 0xffff) {
		nottested();
		return 0;
	}
	if (!(dir = FPCreateDir(Conn2,vol2, did , name))) {
		nottested();
		goto fin;
	}

	if (FPGetFileDirParams(Conn2, vol2,  dir , "", 0,bitmap )) {
		nottested();
		goto fin;
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi2->data +ofs, 0, bitmap);
    filedir.access[0] = 0; 
    filedir.access[1] = 3; 
    filedir.access[2] = 3; 
    filedir.access[3] = 3; 
 	if (FPSetDirParms(Conn2, vol2, dir , "", bitmap, &filedir)) {
		nottested();
		goto fin;
	}
	/* double check the first user can't create a dir in it */
	ret = get_did(Conn, vol, did, name);
	if (FPCreateDir(Conn, vol, ret , name)) {
		nottested();
		FPDelete(Conn2, vol2,  ret, name);
		FPDelete(Conn2, vol2,  did, name);
		ret = 0;
	}
fin:
	if (!ret && dir) {
		if (FPDelete(Conn2, vol2,  did, name)) {
			nottested();
		}
	}
	FPCloseVol(Conn2,vol2);
	fprintf(stderr,"\t>>>>>>>> done <<<<<<<<<< \n");
	return ret;
}

/* ---------------------- 
 * Use the second user for creating a folder with read only access right
 * assume did are the same for != user
*/
int read_only_folder_with_file(u_int16_t vol, int did, char *name, char *file)
{
int ret = 0;
int dir = 0;
u_int16_t vol2;
int  ofs =  3 * sizeof( u_int16_t );
u_int16_t bitmap =  (1 << DIRPBIT_ACCESS);
struct afp_filedir_parms filedir;
DSI *dsi2;

    if (!Conn2) {
    	return 0;
    }
	fprintf(stderr,"\t>>>>>>>> Create folder <<<<<<<<<< \n");
	dsi2 = &Conn2->dsi;
	vol2  = FPOpenVol(Conn2, Vol);
	if (vol2 == 0xffff) {
		nottested();
		return 0;
	}
	if (!(dir = FPCreateDir(Conn2,vol2, did , name))) {
		nottested();
		goto fin;
	}

	if (FPGetFileDirParams(Conn2, vol2,  dir , "", 0,bitmap )) {
		nottested();
		goto fin;
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi2->data +ofs, 0, bitmap);

	if (FPCreateFile(Conn2, vol2,  0, dir , file)) {
		nottested();
		goto fin;
	}

    filedir.access[0] = 0; 
    filedir.access[1] = 3; 
    filedir.access[2] = 3; 
    filedir.access[3] = 3; 
 	if (FPSetDirParms(Conn2, vol2, dir , "", bitmap, &filedir)) {
		nottested();
		goto fin;
	}
	/* double check the first user can't create a dir in it */
	ret = get_did(Conn, vol, did, name);
	if (FPCreateDir(Conn, vol, ret , name)) {
		nottested();
		FPDelete(Conn, vol,  ret, name);
		FPDelete(Conn, vol,  did, name);
		ret = 0;
	}
fin:
	if (!ret && dir) {
		if (FPDelete(Conn2, vol2,  did, name)) {
			nottested();
		}
	}
	FPCloseVol(Conn2,vol2);
	fprintf(stderr,"\t>>>>>>>> done <<<<<<<<<< \n");
	return ret;
}

/* ------------------------ 
 * We need to set rw perm first for .AppleDouble
*/
int delete_folder(u_int16_t vol, int did, char *name)
{
u_int16_t vol2;
int  ofs =  3 * sizeof( u_int16_t );
u_int16_t bitmap =  (1 << DIRPBIT_ACCESS);
struct afp_filedir_parms filedir;
DSI *dsi2;

    if (!Conn2) {
    	return 0;
    }
	fprintf(stderr,"\t>>>>>>>> Delete folder <<<<<<<<<< \n");
	dsi2 = &Conn2->dsi;
	vol2  = FPOpenVol(Conn2, Vol);
	if (vol2 == 0xffff) {
		nottested();
		return 0;
	}
	if (FPGetFileDirParams(Conn2, vol2,  did , name, 0,bitmap )) {
		nottested();
		FPCloseVol(Conn2,vol2);
		return 0;
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi2->data +ofs, 0, bitmap);
    filedir.access[0] = 0; 
    filedir.access[1] = 7; 
    filedir.access[2] = 7; 
    filedir.access[3] = 7; 
 	if (FPSetDirParms(Conn2, vol2, did , name, bitmap, &filedir)) {
		nottested();
		FPCloseVol(Conn2,vol2);
		return 0;
	}
	if (FPDelete(Conn2, vol2,  did, name)) {
		nottested();
		FPCloseVol(Conn2,vol2);
		return 0;
	}
	FPCloseVol(Conn2,vol2);
	fprintf(stderr,"\t>>>>>>>> done <<<<<<<<<< \n");
	return 1;
}

/* ------------------------ 
 * We need to set rw perm first for .AppleDouble
*/
int delete_folder_with_file(u_int16_t vol, int did, char *name, char *file)
{
u_int16_t vol2;
int  ofs =  3 * sizeof( u_int16_t );
u_int16_t bitmap =  (1 << DIRPBIT_ACCESS)|(1<<DIRPBIT_DID );
struct afp_filedir_parms filedir;
DSI *dsi2;

    if (!Conn2) {
    	return 0;
    }
	fprintf(stderr,"\t>>>>>>>> Delete folder <<<<<<<<<< \n");
	dsi2 = &Conn2->dsi;
	vol2  = FPOpenVol(Conn2, Vol);
	if (vol2 == 0xffff) {
		nottested();
		return 0;
	}
	if (FPGetFileDirParams(Conn2, vol2,  did , name, 0,bitmap )) {
		nottested();
		FPCloseVol(Conn2,vol2);
		return 0;
	}
	filedir.isdir = 1;
	afp_filedir_unpack(&filedir, dsi2->data +ofs, 0, bitmap);
    filedir.access[0] = 0; 
    filedir.access[1] = 7; 
    filedir.access[2] = 7; 
    filedir.access[3] = 7; 
    bitmap =  (1 << DIRPBIT_ACCESS);
 	if (FPSetDirParms(Conn2, vol2, did , name, bitmap, &filedir)) {
		nottested();
		FPCloseVol(Conn2,vol2);
		return 0;
	}
	if (FPDelete(Conn2, vol2, filedir.did, file)) {
		nottested();
	}
	if (FPDelete(Conn2, vol2,  did, name)) {
		nottested();
		FPCloseVol(Conn2,vol2);
		return 0;
	}
	FPCloseVol(Conn2,vol2);
	fprintf(stderr,"\t>>>>>>>> done <<<<<<<<<< \n");
	return 1;
}

/* ---------------------- */
int get_vol_attrib(u_int16_t vol) 
{
struct afp_volume_parms parms;
DSI *dsi;

	dsi = &Conn->dsi;

 	if (FPGetVolParam(Conn, vol, (1 << VOLPBIT_ATTR))) {
		nottested();
		return 0;
 	}
	afp_volume_unpack(&parms, dsi->commands +sizeof( u_int16_t ), (1 << VOLPBIT_ATTR));
	return parms.attr;
}

/* ---------------------- */
int not_valid(unsigned int ret, int mac_error, int netatalk_error)
{
	if (htonl(mac_error) != ret) {
		if (!Mac) {
    		fprintf(stderr,"MAC RESULT: %d %s\n", mac_error, afp_error(htonl(mac_error)));
			if (htonl(netatalk_error) != ret) {
				return 1;
			}
    	}
    	else
    		return 1;
	}
	else if (!Mac) {
    	fprintf(stderr,"Warning MAC and Netatalk now same RESULT!\n");
	}
	return 0;
}

/* ---------------------- */
static int error_in_list(unsigned int bitmap, unsigned int error)
{
	if ((BITERR_NOOBJ & bitmap) && htonl(error) == AFPERR_NOOBJ)
		return 1;
	if ((BITERR_NODIR & bitmap) && htonl(error) == AFPERR_NODIR)
		return 1;
	if ((BITERR_PARAM & bitmap) && htonl(error) == AFPERR_PARAM)
		return 1;
	if ((BITERR_BUSY & bitmap) && htonl(error) == AFPERR_BUSY)
		return 1;
	if ((BITERR_BADTYPE & bitmap) && htonl(error) == AFPERR_BADTYPE)
		return 1;
	if ((BITERR_NOITEM & bitmap) && htonl(error) == AFPERR_NOITEM)
		return 1;
	if ((BITERR_DENYCONF & bitmap) && htonl(error) == AFPERR_DENYCONF)
		return 1;
	if ((BITERR_NFILE & bitmap) && htonl(error) == AFPERR_NFILE)
		return 1;
	if ((BITERR_ACCESS & bitmap) && htonl(error) == AFPERR_ACCESS)
		return 1;
	if ((BITERR_NOID & bitmap) && htonl(error) == AFPERR_NOID)
		return 1;
	if ((BITERR_BITMAP & bitmap) && htonl(error) == AFPERR_BITMAP)
		return 1;

	return 0;
}

/* ---------------------- */
static char *bitmap2text(unsigned int bitmap)
{
static char temp[4096];
static char temp1[4096];

	temp[0] = 0;
	if ((BITERR_NOOBJ & bitmap)) {
	    sprintf(temp, "%d %s ", AFPERR_NOOBJ, afp_error(htonl(AFPERR_NOOBJ)));
	}
	if ((BITERR_NODIR & bitmap)) {
	    sprintf(temp1, "%d %s ", AFPERR_NODIR, afp_error(htonl(AFPERR_NODIR)));
		strcat(temp, temp1);
	}

	if ((BITERR_PARAM & bitmap)) {
	    sprintf(temp1, "%d %s ", AFPERR_PARAM, afp_error(htonl(AFPERR_PARAM)));
		strcat(temp, temp1);
	}

	if ((BITERR_BUSY & bitmap)) {
	    sprintf(temp1, "%d %s ", AFPERR_BUSY, afp_error(htonl(AFPERR_BUSY)));
		strcat(temp, temp1);
	}
	if ((BITERR_BADTYPE & bitmap)) {
	    sprintf(temp1, "%d %s ", AFPERR_BADTYPE, afp_error(htonl(AFPERR_BADTYPE)));
		strcat(temp, temp1);
	}
	if ((BITERR_NOITEM & bitmap)) {
	    sprintf(temp1, "%d %s ", AFPERR_NOITEM, afp_error(htonl(AFPERR_NOITEM)));
		strcat(temp, temp1);
	}
	if ((BITERR_DENYCONF & bitmap)) {
	    sprintf(temp1, "%d %s ", AFPERR_DENYCONF, afp_error(htonl(AFPERR_DENYCONF)));
		strcat(temp, temp1);
	}
	if ((BITERR_NFILE & bitmap)) {
	    sprintf(temp1, "%d %s ", AFPERR_NFILE, afp_error(htonl(AFPERR_NFILE)));
		strcat(temp, temp1);
	}
	if ((BITERR_ACCESS & bitmap)) {
	    sprintf(temp1, "%d %s ", AFPERR_ACCESS, afp_error(htonl(AFPERR_ACCESS)));
		strcat(temp, temp1);
	}
	if ((BITERR_NOID & bitmap)) {
	    sprintf(temp1, "%d %s ", AFPERR_NOID, afp_error(htonl(AFPERR_NOID)));
		strcat(temp, temp1);
	}
	if ((BITERR_BITMAP & bitmap)) {
	    sprintf(temp1, "%d %s ", AFPERR_BITMAP, afp_error(htonl(AFPERR_BITMAP)));
		strcat(temp, temp1);
	}
	return temp;
}

/* ---------------------- */
int not_valid_bitmap(unsigned int ret, unsigned int bitmap, int netatalk_error)
{
	if (!Mac) {
    	fprintf(stderr,"MAC RESULT: %s\n", bitmap2text(bitmap));
    }
	if (!error_in_list(bitmap,ret)) {
		if (htonl(netatalk_error) == ret) {
		    return 0;
		}
    	return 1;
	}
	return 0;
}

/* ------------------------- */
void test_skipped(int why) 
{
char *s;
	switch(why) {
	case T_CONN2:
		s = "second user";
		break;
	case T_PATH:
		s = "volume path";
		break;
	case T_AFP3:
		s = "AFP 3.x";
		break;
	case T_AFP3_CONN2:
		s = "AFP 3.x and no second user";
		break;
	case T_MAC_PATH:
		s = "-m (Mac server) or the volume path";
		break;
	case T_UNIX_PREV:
		s =" Volume with unix privilege";
		break;
	case T_UTF8:
		s = "Volume with UTF8 encoding";
		break;
	}
	fprintf(stderr,"\tSKIPPED (need %s)\n",s);
}

/* ------------------------- */
void failed_nomsg(void)
{
	if (!ExitCode)
		ExitCode = 1;
}
/* ------------------------- */
void failed(void)
{
	fprintf(stderr,"\tFAILED\n");
	failed_nomsg();
}

/* ------------------------- */
void nottested(void)
{
	fprintf(stderr,"\tNOT TESTED\n");
	if (!ExitCode)
		ExitCode = 2;
}

