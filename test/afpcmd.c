/*
 * $Id: afpcmd.c,v 1.4 2003-05-12 09:40:59 didg Exp $
 *
 */
#include "afpclient.h"
#include "test.h"

#include <pwd.h> 

/* -------------------------------------------*/
static char *dsi_command2str[] =
{
"???",
"DSIFUNC_CLOSE",   // 1       /* DSICloseSession */
"DSIFUNC_CMD",     // 2       /* DSICommand */
"DSIFUNC_STAT",    // 3       /* DSIGetStatus */
"DSIFUNC_OPEN",    // 4       /* DSIOpenSession */
"DSIFUNC_TICKLE",  // 5       /* DSITickle */
"DSIFUNC_WRITE",   // 6       /* DSIWrite */
"DSI???",		   // 7
"DSIFUNC_ATTN",    // 8       /* DSIAttention */
};

char *afp_error(int error)
{
char *s;
	switch( ntohl(error)) {
	case AFPERR_ACCESS  :  s = "AFPERR_ACCESS  ";break; /* -5000   permission denied */
	case AFPERR_AUTHCONT:  s = "AFPERR_AUTHCONT";break; /* -5001   logincont */
	case AFPERR_BADUAM  :  s = "AFPERR_BADUAM  ";break; /* -5002   uam doesn't exist */
	case AFPERR_BADVERS :  s = "AFPERR_BADVERS ";break; /* -5003   bad afp version number */
	case AFPERR_BITMAP  :  s = "AFPERR_BITMAP  ";break; /* -5004   invalid bitmap */
	case AFPERR_CANTMOVE:  s = "AFPERR_CANTMOVE";break; /* -5005   can't move file */
	case AFPERR_DENYCONF:  s = "AFPERR_DENYCONF";break; /* -5006   file synchronization locks conflict */
	case AFPERR_DIRNEMPT:  s = "AFPERR_DIRNEMPT";break; /* -5007   directory not empty */
	case AFPERR_DFULL   :  s = "AFPERR_DFULL   ";break; /* -5008   disk full */
	case AFPERR_EOF     :  s = "AFPERR_EOF     ";break; /* -5009   end of file -- catsearch and afp_read */
	case AFPERR_BUSY    :  s = "AFPERR_BUSY    ";break; /* -5010   FileBusy */
	case AFPERR_FLATVOL :  s = "AFPERR_FLATVOL ";break; /* -5011   volume doesn't support directories */
	case AFPERR_NOITEM  :  s = "AFPERR_NOITEM  ";break; /* -5012   ItemNotFound */
	case AFPERR_LOCK    :  s = "AFPERR_LOCK    ";break; /* -5013   LockErr */
	case AFPERR_MISC    :  s = "AFPERR_MISC    ";break; /* -5014   misc. err */
	case AFPERR_NLOCK   :  s = "AFPERR_NLOCK   ";break; /* -5015   no more locks */
	case AFPERR_NOSRVR  :  s = "AFPERR_NOSRVR  ";break; /* -5016   no response by server at that address */
	case AFPERR_EXIST   :  s = "AFPERR_EXIST   ";break; /* -5017   object already exists */
	case AFPERR_NOOBJ   :  s = "AFPERR_NOOBJ   ";break; /* -5018   object not found */
	case AFPERR_PARAM   :  s = "AFPERR_PARAM   ";break; /* -5019   parameter error */
	case AFPERR_NORANGE :  s = "AFPERR_NORANGE ";break; /* -5020   no range lock */
	case AFPERR_RANGEOVR:  s = "AFPERR_RANGEOVR";break; /* -5021   range overlap */
	case AFPERR_SESSCLOS:  s = "AFPERR_SESSCLOS";break; /* -5022   session closed */
	case AFPERR_NOTAUTH :  s = "AFPERR_NOTAUTH ";break; /* -5023   user not authenticated */
	case AFPERR_NOOP    :  s = "AFPERR_NOOP    ";break; /* -5024   command not supported */
	case AFPERR_BADTYPE :  s = "AFPERR_BADTYPE ";break; /* -5025   object is the wrong type */
	case AFPERR_NFILE   :  s = "AFPERR_NFILE   ";break; /* -5026   too many files open */
	case AFPERR_SHUTDOWN:  s = "AFPERR_SHUTDOWN";break; /* -5027   server is going down */
	case AFPERR_NORENAME:  s = "AFPERR_NORENAME";break; /* -5028   can't rename */
	case AFPERR_NODIR   :  s = "AFPERR_NODIR   ";break; /* -5029   couldn't find directory */
	case AFPERR_ITYPE   :  s = "AFPERR_ITYPE   ";break; /* -5030   wrong icon type */
	case AFPERR_VLOCK   :  s = "AFPERR_VLOCK   ";break; /* -5031   volume locked */
	case AFPERR_OLOCK   :  s = "AFPERR_OLOCK   ";break; /* -5032   object locked */
	case AFPERR_CTNSHRD :  s = "AFPERR_CTNSHRD ";break; /* -5033   share point contains a share point */
	case AFPERR_NOID    :  s = "AFPERR_NOID    ";break; /* -5034   file thread not found */
	case AFPERR_EXISTID :  s = "AFPERR_EXISTID ";break; /* -5035   file already has an id */
	case AFPERR_DIFFVOL :  s = "AFPERR_DIFFVOL ";break; /* -5036   different volume */
	case AFPERR_CATCHNG :  s = "AFPERR_CATCHNG ";break; /* -5037   catalog has changed */
	case AFPERR_SAMEOBJ :  s = "AFPERR_SAMEOBJ ";break; /* -5038   source file == destination file */
	case AFPERR_BADID   :  s = "AFPERR_BADID   ";break; /* -5039   non-existent file id */
	case AFPERR_PWDSAME :  s = "AFPERR_PWDSAME ";break; /* -5040   same password/can't change password */
	case AFPERR_PWDSHORT:  s = "AFPERR_PWDSHORT";break; /* -5041   password too short */
	case AFPERR_PWDEXPR :  s = "AFPERR_PWDEXPR ";break; /* -5042   password expired */
	case AFPERR_INSHRD  :  s = "AFPERR_INSHRD  ";break; /* -5043   folder being shared is inside a */
	case AFPERR_INTRASH :  s = "AFPERR_INTRASH ";break; /* -5044   shared folder in trash. */
	case AFPERR_PWDCHNG :  s = "AFPERR_PWDCHNG ";break; /* -5045   password needs to be changed */
	case AFPERR_PWDPOLCY:  s = "AFPERR_PWDPOLCY";break; /* -5046   password fails policy check */
	case AFPERR_USRLOGIN:  s = "AFPERR_USRLOGIN";break; /* -5047   user already logged on */
	case 0				:  s = "";break;
	default				:  s = "unknow";break;
	}
	return s;
}

/* --------------------------------------------------- */
const char *AfpNum2name(int num)
{
	switch(num) {
    case AFP_BYTELOCK      : return "AFP_BYTELOCK      ";   /*   1 */
	case AFP_CLOSEVOL      : return "AFP_CLOSEVOL      ";  	/*   2 */
	case AFP_CLOSEDIR      : return "AFP_CLOSEDIR      ";  	/*   3 */
	case AFP_CLOSEFORK     : return "AFP_CLOSEFORK     ";  	/*   4 */
	case AFP_COPYFILE      : return "AFP_COPYFILE      ";  	/*   5 */
	case AFP_CREATEDIR     : return "AFP_CREATEDIR     ";  	/*   6 */
	case AFP_CREATEFILE    : return "AFP_CREATEFILE    ";  	/*   7 */
	case AFP_DELETE        : return "AFP_DELETE        "; 	/*   8 */
	case AFP_ENUMERATE     : return "AFP_ENUMERATE     "; 	/*   9 */
	case AFP_FLUSH         : return "AFP_FLUSH         ";	/*	10 */
	case AFP_FLUSHFORK     : return "AFP_FLUSHFORK     ";	/*	11 */

	case AFP_GETFORKPARAM  : return "AFP_GETFORKPARAM  ";   /*	14 */
	case AFP_GETSRVINFO    : return "AFP_GETSRVINFO    "; 	/* 	15 */
	case AFP_GETSRVPARAM   : return "AFP_GETSRVPARAM   ";   /*	16 */
	case AFP_GETVOLPARAM   : return "AFP_GETVOLPARAM   ";   /*	17 */
	case AFP_LOGIN         : return "AFP_LOGIN         ";  	/*  18 */
	case AFP_LOGOUT        : return "AFP_LOGOUT        ";  	/*  20 */
    case AFP_MAPID         : return "AFP_MAPID         ";   /*  21 */
    case AFP_MAPNAME       : return "AFP_MAPNAME       ";   /*  22 */
    case AFP_MOVE          : return "AFP_MOVE          ";   /*  23 */
	case AFP_OPENVOL       : return "AFP_OPENVOL       ";  	/*  24 */
	case AFP_OPENDIR       : return "AFP_OPENDIR       ";  	/*  25 */
	case AFP_OPENFORK      : return "AFP_OPENFORK      ";  	/*  26 */
	case AFP_READ		   : return "AFP_READ          ";	/*  27 */
    case AFP_RENAME        : return "AFP_RENAME        ";   /*  28 */
    case AFP_SETDIRPARAM   : return "AFP_SETDIRPARAM   ";   /*  29 */
    case AFP_SETFILEPARAM  : return "AFP_SETFILEPARAM  ";   /*  30 */
    case AFP_SETFORKPARAM  : return "AFP_SETFORKPARAM  ";   /*  31 */
    case AFP_SETVOLPARAM   : return "AFP_SETVOLPARAM   ";   /*  32 */
	case AFP_WRITE 		   : return "AFP_WRITE         ";	/*  33 */
	case AFP_GETFLDRPARAM  : return "AFP_GETFLDRPARAM  ";	/*  34 */	
	case AFP_SETFLDRPARAM  : return "AFP_SETFLDRPARAM  ";	/*  35 */	
	case AFP_CHANGEPW      : return "AFP_CHANGEPW      ";	/*  36 */	

    case AFP_GETSRVRMSG    : return "AFP_GETSRVRMSG    ";   /*  38 */
    case AFP_CREATEID      : return "AFP_CREATEID      ";   /*  39 */
    case AFP_DELETEID      : return "AFP_DELETEID      ";   /*  40 */
    case AFP_RESOLVEID     : return "AFP_RESOLVEID     ";   /*  41 */
    case AFP_EXCHANGEFILE  : return "AFP_EXCHANGEFILE  ";   /*  42 */
    case AFP_CATSEARCH     : return "AFP_CATSEARCH     ";   /*  43 */

	case AFP_OPENDT        : return "AFP_OPENDT        ";	/*  48 */
	case AFP_CLOSEDT       : return "AFP_CLOSEDT       ";	/*	49 */
    case AFP_GETICON       : return "AFP_GETICON       ";	/*	51 */
    case AFP_GTICNINFO     : return "AFP_GTICNINFO     ";	/*	52 */
    case AFP_ADDAPPL       : return "AFP_ADDAPPL       ";  	/*	53 */
    case AFP_RMVAPPL       : return "AFP_RMVAPPL       ";  	/*	54 */
    case AFP_GETAPPL       : return "AFP_GETAPPL       ";   /*	55 */
    case AFP_ADDCMT        : return "AFP_ADDCMT        ";   /*	56 */
	case AFP_RMVCMT        : return "AFP_RMVCMT        ";   /*	57 */
	case AFP_GETCMT        : return "AFP_GETCMT        ";   /*	58 */
    /* version 3.0 */
    case AFP_BYTELOCK_EXT  : return "AFP_BYTELOCK_EXT  ";   /*  59 */
    case AFP_READ_EXT      : return "AFP_READ_EXT      ";   /*  60 */
    case AFP_WRITE_EXT     : return "AFP_WRITE_EXT     ";   /*  61 */
    case AFP_GETSESSTOKEN  : return "AFP_GETSESSTOKEN  ";   /*  64 */ 
    case AFP_LOGIN_EXT     : return "AFP_LOGIN_EXT     ";   /*  63 */
    case AFP_DISCTOLDSESS  : return "AFP_DISCTOLDSESS  ";   /*  65 */
    case AFP_ENUMERATE_EXT : return "AFP_ENUMERATE_EXT ";   /*  66 */
    case AFP_CATSEARCH_EXT : return "AFP_CATSEARCH_EXT ";   /*  67 */
    /* version 3.1 */
    case AFP_ENUMERATE_EXT2: return "AFP_ENUMERATE_EXT2";   /*  68 */
    case AFP_ZZZ           : return "AFP_ZZZ           ";	/* 122 */
															  
	case AFP_ADDICON       : return "AFP_ADDICON";       	/* 192 */
	}                    									  
	return "not yet defined";								  
}

/* --------------------------------------------------- */
void dump_header(DSI *dsi)
{
char *s;
	if (Verbose) {
		fprintf(stderr, "clientID              %d\n",dsi->clientID);
		fprintf(stderr, "cmdlen                %d\n\n",dsi->cmdlen);
		fprintf(stderr, "header.dsi_flags      %d %s\n",dsi->header.dsi_flags, dsi->header.dsi_flags?"REPLY":"REQUEST");
		fprintf(stderr, "header.dsi_command    %d %s\n",dsi->header.dsi_command, dsi_command2str[dsi->header.dsi_command]);
		fprintf(stderr, "header.dsi_requestID  %d\n",ntohs(dsi->header.dsi_requestID));
	}
	if (0 != dsi->header.dsi_code || Verbose) {
		fprintf(stderr, "header.dsi_code       %d",ntohl(dsi->header.dsi_code));
    	if (dsi->header.dsi_flags) {
    		s = afp_error(dsi->header.dsi_code);
		}
		else 
			s = "";
		fprintf(stderr,"\t%s\n", s);
	}
	if (Verbose) {
		fprintf(stderr, "header.dsi_len        %d\n\n",ntohl(dsi->header.dsi_len));
	}

	if (Verbose && dsi->header.dsi_command == DSIFUNC_ATTN) {
	u_int16_t i;

		memcpy(&i, dsi->commands, sizeof(i));
		fprintf(stderr,"\t %x\n",  ntohs(i));
	}
}

/* --------------------------------------------------- */
void dump_open(DSI *dsi)
{
u_int32_t i = 0; 

	/* parse options */
  	while (i < dsi->cmdlen) {
    	switch (dsi->commands[i++]) {
	    case DSIOPT_ATTNQUANT:
      		memcpy(&dsi->attn_quantum, dsi->commands + i + 1, dsi->commands[i]);
      		dsi->attn_quantum = ntohl(dsi->attn_quantum);
	  		fprintf(stderr,"\tDSIOPT_ATTNQUANT %x\n", dsi->attn_quantum);
			break; 
    	case DSIOPT_SERVQUANT:
      		memcpy(&dsi->server_quantum, dsi->commands + i + 1, dsi->commands[i]);
      		dsi->server_quantum = ntohl(dsi->server_quantum);
	  		fprintf(stderr,"\tDSIOPT_SERVQUANT %x\n", dsi->server_quantum);
			break;
    	default:
	  		fprintf(stderr,"\tunknow %d\n", dsi->commands[i -1]);
      		break;
    	}
		i += dsi->commands[i] + 1; /* forward past length tag + length */
  	}
	fprintf(stderr,"=========================\n");
}

/* -----------------------------
	spec violation in netatalk 
	FPlogout ==> dsiclose
	Page 209
    Specs                C variable
	string AFPVersion   vers
	string UAM          uam
	
*/
unsigned int FPopenLogin(CONN *conn, char *vers, char *uam, char *usr, char *pwd)
{
int ret;
struct passwd *p = NULL;
DSI *dsi;

	dsi = &conn->dsi;

	if (!usr) {
		p = getpwuid(getuid());
		if (!p)
			return -1;
		usr = p->pw_name;
	}
	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Login Version: \"%s\" uam: \"%s\" user: \"%s\"\n\n", vers, uam, usr);

	ret = AFPopenLogin(conn, vers, uam, usr, pwd);
	dump_header(dsi);
	return ret;
}

/* -------------
*/
unsigned int FPopenLoginExt(CONN *conn, char *vers, char *uam, char *usr, char *pwd)
{
int ret;
struct passwd *p = NULL;
DSI *dsi;

	dsi = &conn->dsi;

	if (!usr) {
		p = getpwuid(getuid());
		if (!p)
			return -1;
		usr = p->pw_name;
	}
	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"LoginExt Version: \"%s\" uam: \"%s\" user: \"%s\"\n\n", vers, uam, usr);
	ret = AFPopenLoginExt(conn,vers, uam, usr, pwd);
	dump_header(dsi);
	return ret;
}

/* ------------------------------- */
unsigned int FPLogOut(CONN *conn)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;
	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Logout\n\n");
	ret = AFPLogOut(conn);
	dump_header(dsi);
	return ret;
}

/* ------------------------------- */
unsigned int FPzzz(CONN *conn)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;
	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"FPzzz enter sleep mode\n\n");
	ret = AFPzzz(conn);
	dump_header(dsi);
	return ret;
}

/* ------------------------------- */
unsigned int FPGetSessionToken(CONN *conn, int type, u_int32_t time, int len, char *token)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;
	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Get Session Token type %d\n\n", type);
	ret = AFPGetSessionToken(conn,type, time, len, token);
	dump_header(dsi);
	return ret;
}

/* ------------------------------- */
unsigned int FPDisconnectOldSession(CONN *conn, u_int16_t type, int len, char *token)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;
	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Disconnect old session type %d token len %d\n\n", type, len);
	ret = AFPDisconnectOldSession(conn, type, len, token);
	dump_header(dsi);
	return ret;
}

/* ------------------------------- */
unsigned int FPMapID(CONN *conn, char fn, int id)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;
	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Map ID fn %d, id %d\n\n", fn, id);
	ret = AFPMapID(conn,fn,id);
	dump_header(dsi);
	return ret;
}

/* ------------------------------- */
unsigned int FPMapName(CONN *conn, char fn, char *name )
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;
	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Map Name fn %d, name  <%s>\n\n", fn, name);
	ret = AFPMapName(conn,fn,name);
	dump_header(dsi);
	return ret;
}

/* ------------------------------- */
unsigned int FPBadPacket(CONN *conn, char fn, char *name )
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;
	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Bad packet fn %d, name  <%s>\n\n", fn, name);
	ret = AFPBadPacket(conn, fn,name);
	dump_header(dsi);
	return ret;
}

/* ------------------------------- */
unsigned int FPGetUserInfo(CONN *conn, char flag, int id, u_int16_t bitmap)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;
	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Get %s User Info id %d (get %s ID)\n\n", flag?"Default":"", 
					id, (bitmap & 1)?"User":"Group");
	ret = AFPGetUserInfo(conn,flag,id, bitmap);
	dump_header(dsi);
	return ret;
}

/* ------------------------------- */
unsigned int FPGetSrvrInfo(CONN *conn)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;
	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"get Server info\n\n");
	ret = AFPGetSrvrInfo(conn);
	dump_header(dsi);
	return ret;
}

/* ------------------------------- */
unsigned int FPGetSrvrParms(CONN *conn)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;
	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"get Server param\n\n");
	ret = AFPGetSrvrParms(conn);
	dump_header(dsi);
	return ret;
}

/* ------------------------------- */
unsigned int FPGetSrvrMsg(CONN *conn, u_int16_t type, u_int16_t bitmap)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;
	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"get Server message type %d bitmap %d\n\n", type, bitmap);
	ret = AFPGetSrvrMsg(conn, type, bitmap);
	dump_header(dsi);
	return ret;
}

/* ------------------------------- 
	pas bon sur le code d'erreur
	return -1 if error (O is a valid volume ID).
	page 235	
*/
u_int16_t FPOpenVolFull(CONN *conn, char *vol, u_int16_t  bitmap)
{
u_int16_t  volID = 0;
DSI *dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Open Vol %s bitmap %x\n\n", vol, bitmap);

	volID = AFPOpenVol(conn,vol, bitmap);
	dump_header(dsi);
	return(!dsi->header.dsi_code?volID:0xffff);
}

/* ------------------ */
u_int16_t FPOpenVol(CONN *conn, char *vol)
{
	return FPOpenVolFull(conn, vol, (1<<VOLPBIT_VID)| (1<<VOLPBIT_ATTR));
}

/* ------------------------------- */
unsigned int FPCloseVol(CONN *conn, u_int16_t vol)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;
	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Close Vol %d\n\n", vol);

	ret  = AFPCloseVol(conn,vol);
	dump_header(dsi);
	return(dsi->header.dsi_code);
}

/* ------------------------------- */
u_int16_t  FPOpenDT(CONN *conn, u_int16_t vol)
{
int ofs;
u_int16_t  id = 0xffff,result;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Open Desktop Vol %d\n\n", vol);

	memset(dsi->commands, 0, DSI_CMDSIZ);
	dsi->header.dsi_flags = DSIFL_REQUEST;     
	dsi->header.dsi_command = DSIFUNC_CMD;
	dsi->header.dsi_requestID = dsi_clientID(dsi);
	ofs = 0;
	dsi->commands[ofs++] = AFP_OPENDT;
	dsi->commands[ofs++] = 0;
	memcpy(dsi->commands +ofs, &vol, sizeof(vol));
	ofs += sizeof(vol);

	dsi->datalen = ofs;
	dsi->header.dsi_len = htonl(dsi->datalen);
	dsi->header.dsi_code = 0; 
 
   	my_dsi_stream_send(dsi, dsi->commands, dsi->datalen);
	/* ------------------ */
	my_dsi_receive(dsi);
	dump_header(dsi);
	if (!dsi->header.dsi_code) {
		ofs = 0;
		memcpy(&result, dsi->commands, sizeof(result));
		ofs += sizeof(result);
		memcpy(&result, dsi->commands +ofs, sizeof(result));
		id = result;
		fprintf(stderr,"Desktop ID %d\n", id);
	}
	return(id);
}

/* ------------------------------- */
unsigned int FPGetIcon(CONN *conn, u_int16_t dt, char *creator, char *type, char itype, u_int16_t size )
{
int ofs;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Get Icon Desktop %d creator %s type %s size %d\n\n", dt, creator, type, size);

	memset(dsi->commands, 0, DSI_CMDSIZ);
	dsi->header.dsi_flags = DSIFL_REQUEST;     
	dsi->header.dsi_command = DSIFUNC_CMD;
	dsi->header.dsi_requestID = dsi_clientID(dsi);
	ofs = 0;
	dsi->commands[ofs++] =  AFP_GETICON;
	dsi->commands[ofs++] = 0;

	memcpy(dsi->commands +ofs, &dt, sizeof(dt));
	ofs += sizeof(dt);

	memcpy(dsi->commands +ofs, creator, 4);
	ofs += 4;
	
	memcpy(dsi->commands +ofs, type, 4);
	ofs += 4;

	dsi->commands[ofs++] = itype;
	dsi->commands[ofs++] = 0;

	size = htons(size);
	memcpy(dsi->commands +ofs, &size, sizeof(size));
	ofs += sizeof(size);

	dsi->datalen = ofs;
	dsi->header.dsi_len = htonl(dsi->datalen);
	dsi->header.dsi_code = 0; 
 
   	my_dsi_stream_send(dsi, dsi->commands, dsi->datalen);
	/* ------------------ */
	my_dsi_receive(dsi);
	dump_header(dsi);
	return dsi->header.dsi_code;
}

/* ------------------------------- */
unsigned int FPAddIcon(CONN *conn, u_int16_t dt, char *creator, char *type, char itype, u_int32_t tag, u_int16_t size, char *data )
{
int ofs;
DSI *dsi;
u_int16_t temp;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Add Icon Desktop %d creator %s type %s size %d\n\n", dt, creator, type, size);

	memset(dsi->commands, 0, DSI_CMDSIZ);
	dsi->header.dsi_flags = DSIFL_REQUEST;     
	dsi->header.dsi_command = DSIFUNC_WRITE;
	dsi->header.dsi_requestID = dsi_clientID(dsi);
	ofs = 0;
	dsi->commands[ofs++] =  AFP_ADDICON;
	dsi->commands[ofs++] = 0;

	memcpy(dsi->commands +ofs, &dt, sizeof(dt));
	ofs += sizeof(dt);

	memcpy(dsi->commands +ofs, creator, 4);
	ofs += 4;
	
	memcpy(dsi->commands +ofs, type, 4);
	ofs += 4;

	dsi->commands[ofs++] = itype;
	dsi->commands[ofs++] = 0;

	memcpy(dsi->commands +ofs, &tag, sizeof(tag));
	ofs += sizeof(tag);

	temp = htons(size);
	memcpy(dsi->commands +ofs, &temp, sizeof(temp));
	ofs += sizeof(temp);

	dsi->datalen = ofs;
	dsi->header.dsi_len = htonl(dsi->datalen +size);
	dsi->header.dsi_code = htonl(ofs);  
 
   	my_dsi_stream_send(dsi, dsi->commands, ofs);
	my_dsi_stream_write(dsi, data, size);
	/* ------------------ */
	my_dsi_receive(dsi);
	dump_header(dsi);
	return dsi->header.dsi_code;
}

/* ------------------------------- */
unsigned int FPGetIconInfo(CONN *conn, u_int16_t dt, char *creator, u_int16_t itype )
{
int ofs;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Get Icon Desktop %d creator %s itype %d\n\n", dt, creator, itype);

	memset(dsi->commands, 0, DSI_CMDSIZ);
	dsi->header.dsi_flags = DSIFL_REQUEST;     
	dsi->header.dsi_command = DSIFUNC_CMD;
	dsi->header.dsi_requestID = dsi_clientID(dsi);
	ofs = 0;
	dsi->commands[ofs++] =  AFP_GTICNINFO;
	dsi->commands[ofs++] = 0;

	memcpy(dsi->commands +ofs, &dt, sizeof(dt));
	ofs += sizeof(dt);

	memcpy(dsi->commands +ofs, creator, 4);
	ofs += 4;
	
	itype = htons(itype);
	memcpy(dsi->commands +ofs, &itype, sizeof(itype));
	ofs += sizeof(itype);

	dsi->datalen = ofs;
	dsi->header.dsi_len = htonl(dsi->datalen);
	dsi->header.dsi_code = 0; 
 
   	my_dsi_stream_send(dsi, dsi->commands, dsi->datalen);
	/* ------------------ */
	my_dsi_receive(dsi);
	dump_header(dsi);
	return dsi->header.dsi_code;
}

/* ------------------------------- */
unsigned int FPGetVolParam(CONN *conn, u_int16_t vol, u_int16_t bitmap)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Get Volume Parameters Vol %d  bitmap %x \n\n", vol, bitmap);
	ret = AFPGetVolParam(conn,vol, bitmap);
	dump_header(dsi);
	return ret;
}

/* ------------------------------- */
unsigned int FPSetVolParam(CONN *conn, u_int16_t vol, u_int16_t bitmap, struct afp_volume_parms *parms)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Set Volume Parameters Vol %d  bitmap %x \n\n", vol, bitmap);
	ret = AFPSetVolParam(conn,vol, bitmap, parms);
	dump_header(dsi);
	return ret;
}

/* ------------------------------- */
unsigned int FPCloseDT(CONN *conn, u_int16_t vol)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;
	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Close Desktop %d\n\n", vol);
	ret = AFPCloseDT(conn,vol);
	dump_header(dsi);
	return ret;
}

/* ------------------------------- */
unsigned int FPCloseFork(CONN *conn, u_int16_t vol)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;
	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Close Fork %d\n\n", vol);
	ret = AFPCloseFork(conn,vol);
	dump_header(dsi);
	return ret;
}

/* ------------------------------- */
unsigned int FPFlush(CONN *conn, u_int16_t vol)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;
	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Flush volume %d\n\n", vol);
	ret = AFPFlush(conn,vol);
	dump_header(dsi);
	return ret;
}

/* ------------------------------- */
unsigned int FPByteLock(CONN *conn, u_int16_t fork, int end, int mode, int offset, int size )
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Byte lock fork %d end %d %s offset %d size %d\n\n", 
	                         fork, end, mode?"unlock":"lock", offset, size);
	ret = AFPByteLock(conn,fork, end, mode, offset, size);
	dump_header(dsi);
	return ret;
}

/* ------------------------------- */
unsigned int FPByteLock_ext(CONN *conn, u_int16_t fork, int end, int mode, off_t offset, off_t size )
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Byte lock ext fork %d end %d %s offset %lld size %lld\n\n", 
	                         fork, end, mode?"unlock":"lock", offset, size);
	ret = AFPByteLock_ext(conn,fork, end, mode, offset, size);
	dump_header(dsi);
	return ret;
}

/* ------------------------------- */
unsigned int FPFlushFork(CONN *conn, u_int16_t vol)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;
	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Flush fork %d\n\n", vol);
	ret = AFPFlushFork(conn,vol);
	dump_header(dsi);
	return ret;
}

/* ------------------------------- 
*/
unsigned int FPCloseDir(CONN *conn, u_int16_t vol, int did)
{
int ofs;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Close dir vol %d dir 0x%d\n\n", vol, htonl(did));
	memset(dsi->commands, 0, DSI_CMDSIZ);
	dsi->header.dsi_flags = DSIFL_REQUEST;     
	dsi->header.dsi_command = DSIFUNC_CMD;
	dsi->header.dsi_requestID = dsi_clientID(dsi);
	ofs = 0;
	dsi->commands[ofs++] = AFP_CLOSEDIR;
	dsi->commands[ofs++] = 0;
	memcpy(dsi->commands +ofs, &vol, sizeof(vol));
	ofs += sizeof(vol);

	memcpy(dsi->commands +ofs, &did, sizeof(did));
	ofs += sizeof(did);

	dsi->datalen = ofs;
	dsi->header.dsi_len = htonl(dsi->datalen);
	dsi->header.dsi_code = 0; 
 
   	my_dsi_stream_send(dsi, dsi->commands, dsi->datalen);
	/* ------------------ */
	my_dsi_receive(dsi);
	dump_header(dsi);
	return(dsi->header.dsi_code);
}

/* ------------------------------- 
*/
unsigned int FPEnumerate(CONN *conn, u_int16_t vol, int did , char *name, u_int16_t f_bitmap, u_int16_t d_bitmap)
{
int ofs;
u_int16_t bitmap;
int len;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Enumerate Vol %d did : 0x%x <%s>\n\n", vol, ntohl(did), name);
	memset(dsi->commands, 0, DSI_CMDSIZ);
	dsi->header.dsi_flags = DSIFL_REQUEST;     
	dsi->header.dsi_command = DSIFUNC_CMD;
	dsi->header.dsi_requestID = htons(dsi_clientID(dsi));
	ofs = 0;
	dsi->commands[ofs++] = AFP_ENUMERATE;
	dsi->commands[ofs++] = 0;

	memcpy(dsi->commands +ofs, &vol, sizeof(vol));
	ofs += sizeof(vol);
	
	memcpy(dsi->commands +ofs, &did, sizeof(did));
	ofs += sizeof(did);

	bitmap = htons(f_bitmap);
	memcpy(dsi->commands +ofs, &bitmap, sizeof(bitmap));
	ofs += sizeof(bitmap);
	
	bitmap = htons(d_bitmap);;
	memcpy(dsi->commands +ofs, &bitmap, sizeof(bitmap));
	ofs += sizeof(bitmap);

	bitmap = htons(1024);		/* reqcnt */
	memcpy(dsi->commands +ofs, &bitmap, sizeof(bitmap));
	ofs += sizeof(bitmap);
	
	bitmap = htons(1);		/* sindex  */
	memcpy(dsi->commands +ofs, &bitmap, sizeof(bitmap));
	ofs += sizeof(bitmap);

	bitmap = htons(DSI_DATASIZ);		/* maxsize  */
	memcpy(dsi->commands +ofs, &bitmap, sizeof(bitmap));
	ofs += sizeof(bitmap);

	dsi->commands[ofs++] = 2;		/* long name */
	len = strlen(name);
	dsi->commands[ofs++] = len;
	u2mac(&dsi->commands[ofs], name, len);
	ofs += len;
		
	dsi->datalen = ofs;
	dsi->header.dsi_len = htonl(dsi->datalen);
	dsi->header.dsi_code = 0; 
 
   	my_dsi_stream_send(dsi, dsi->commands, dsi->datalen);
	/* ------------------ */
	my_dsi_data_receive(dsi);

	dump_header(dsi);
	return(dsi->header.dsi_code);
}


/* ------------------------------- 
*/
unsigned int FPEnumerateFull(CONN *conn, u_int16_t vol, u_int16_t sindex, u_int16_t reqcnt, u_int16_t size, int did , char *name, u_int16_t f_bitmap, u_int16_t d_bitmap)
{
int ofs;
u_int16_t bitmap;
int len;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Enumerate Vol %d did : 0x%x <%s>\n\n", vol, ntohl(did), name);
	memset(dsi->commands, 0, DSI_CMDSIZ);
	dsi->header.dsi_flags = DSIFL_REQUEST;     
	dsi->header.dsi_command = DSIFUNC_CMD;
	dsi->header.dsi_requestID = htons(dsi_clientID(dsi));
	ofs = 0;
	dsi->commands[ofs++] = AFP_ENUMERATE;
	dsi->commands[ofs++] = 0;

	memcpy(dsi->commands +ofs, &vol, sizeof(vol));
	ofs += sizeof(vol);
	
	memcpy(dsi->commands +ofs, &did, sizeof(did));
	ofs += sizeof(did);

	bitmap = htons(f_bitmap);
	memcpy(dsi->commands +ofs, &bitmap, sizeof(bitmap));
	ofs += sizeof(bitmap);
	
	bitmap = htons(d_bitmap);;
	memcpy(dsi->commands +ofs, &bitmap, sizeof(bitmap));
	ofs += sizeof(bitmap);

	bitmap = htons(reqcnt);		/* reqcnt */
	memcpy(dsi->commands +ofs, &bitmap, sizeof(bitmap));
	ofs += sizeof(bitmap);
	
	bitmap = htons(sindex);		/* sindex  */
	memcpy(dsi->commands +ofs, &bitmap, sizeof(bitmap));
	ofs += sizeof(bitmap);

	bitmap = htons(size); // DSI_DATASIZ);		/* maxsize  */
	memcpy(dsi->commands +ofs, &bitmap, sizeof(bitmap));
	ofs += sizeof(bitmap);

	dsi->commands[ofs++] = 2;		/* long name */
	len = strlen(name);
	dsi->commands[ofs++] = len;
	u2mac(&dsi->commands[ofs], name, len);
	ofs += len;
		
	dsi->datalen = ofs;
	dsi->header.dsi_len = htonl(dsi->datalen);
	dsi->header.dsi_code = 0; 
 
   	my_dsi_stream_send(dsi, dsi->commands, dsi->datalen);
	/* ------------------ */
	my_dsi_data_receive(dsi);

	dump_header(dsi);
	return(dsi->header.dsi_code);
}

/* ------------------------------- 
*/
unsigned int FPGetFileDirParams(CONN *conn, u_int16_t vol, int did , char *name, u_int16_t f_bitmap, u_int16_t d_bitmap)
{
int ofs;
u_int16_t bitmap;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"GetFileDirParams Vol %d did : 0x%x <%s>\n\n", vol, ntohl(did), name);
	memset(dsi->commands, 0, DSI_CMDSIZ);
	dsi->header.dsi_flags = DSIFL_REQUEST;     
	dsi->header.dsi_command = DSIFUNC_CMD;
	dsi->header.dsi_requestID = htons(dsi_clientID(dsi));
	ofs = 0;
	dsi->commands[ofs++] = AFP_GETFLDRPARAM;
	dsi->commands[ofs++] = 0;

	memcpy(dsi->commands +ofs, &vol, sizeof(vol));
	ofs += sizeof(vol);
	
	memcpy(dsi->commands +ofs, &did, sizeof(did));
	ofs += sizeof(did);

	bitmap = htons(f_bitmap);
	memcpy(dsi->commands +ofs, &bitmap, sizeof(bitmap));
	ofs += sizeof(bitmap);
	
	bitmap = htons(d_bitmap);;
	memcpy(dsi->commands +ofs, &bitmap, sizeof(bitmap));
	ofs += sizeof(bitmap);

	ofs = FPset_name(conn, ofs, name);
		
	dsi->datalen = ofs;
	dsi->header.dsi_len = htonl(dsi->datalen);
	dsi->header.dsi_code = 0; 
 
   	my_dsi_stream_send(dsi, dsi->commands, dsi->datalen);
	/* ------------------ */
	my_dsi_data_receive(dsi);

	dump_header(dsi);
	return(dsi->header.dsi_code);
}

/* ------------------------------- 
*/
unsigned int FPCreateID(CONN *conn, u_int16_t vol, int did , char *name)
{
int ofs;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Create ID Vol: %d did: 0x%x <%s>\n\n", vol, ntohl(did), name);
	memset(dsi->commands, 0, DSI_CMDSIZ);
	dsi->header.dsi_flags = DSIFL_REQUEST;     
	dsi->header.dsi_command = DSIFUNC_CMD;
	dsi->header.dsi_requestID = htons(dsi_clientID(dsi));
	ofs = 0;
	dsi->commands[ofs++] = AFP_CREATEID;
	dsi->commands[ofs++] = 0;

	memcpy(dsi->commands +ofs, &vol, sizeof(vol));
	ofs += sizeof(vol);
	
	memcpy(dsi->commands +ofs, &did, sizeof(did));
	ofs += sizeof(did);

	ofs = FPset_name(conn, ofs, name);
		
	dsi->datalen = ofs;
	dsi->header.dsi_len = htonl(dsi->datalen);
	dsi->header.dsi_code = 0; 
 
   	my_dsi_stream_send(dsi, dsi->commands, dsi->datalen);
	/* ------------------ */
	my_dsi_data_receive(dsi);

	dump_header(dsi);
	return(dsi->header.dsi_code);
}

/* ------------------------------- 
*/
unsigned int FPDeleteID(CONN *conn, u_int16_t vol, int did )
{
int ofs;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Delete ID Vol: %d id: 0x%x \n\n", vol, ntohl(did));
	memset(dsi->commands, 0, DSI_CMDSIZ);
	dsi->header.dsi_flags = DSIFL_REQUEST;     
	dsi->header.dsi_command = DSIFUNC_CMD;
	dsi->header.dsi_requestID = htons(dsi_clientID(dsi));
	ofs = 0;
	dsi->commands[ofs++] = AFP_DELETEID;
	dsi->commands[ofs++] = 0;

	memcpy(dsi->commands +ofs, &vol, sizeof(vol));
	ofs += sizeof(vol);
	
	memcpy(dsi->commands +ofs, &did, sizeof(did));
	ofs += sizeof(did);

	dsi->datalen = ofs;
	dsi->header.dsi_len = htonl(dsi->datalen);
	dsi->header.dsi_code = 0; 
 
   	my_dsi_stream_send(dsi, dsi->commands, dsi->datalen);
	/* ------------------ */
	my_dsi_data_receive(dsi);

	dump_header(dsi);
	return(dsi->header.dsi_code);
}

/* ------------------------------- 
*/
unsigned int FPResolveID(CONN *conn, u_int16_t vol, int did, u_int16_t bitmap )
{
int ofs;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Resolve ID Vol: %d id: 0x%x %x\n\n", vol, ntohl(did), bitmap);
	memset(dsi->commands, 0, DSI_CMDSIZ);
	dsi->header.dsi_flags = DSIFL_REQUEST;     
	dsi->header.dsi_command = DSIFUNC_CMD;
	dsi->header.dsi_requestID = htons(dsi_clientID(dsi));
	ofs = 0;
	dsi->commands[ofs++] = AFP_RESOLVEID;
	dsi->commands[ofs++] = 0;

	memcpy(dsi->commands +ofs, &vol, sizeof(vol));
	ofs += sizeof(vol);
	
	memcpy(dsi->commands +ofs, &did, sizeof(did));
	ofs += sizeof(did);

	bitmap = htons(bitmap);;
	memcpy(dsi->commands +ofs, &bitmap, sizeof(bitmap));
	ofs += sizeof(bitmap);

	dsi->datalen = ofs;
	dsi->header.dsi_len = htonl(dsi->datalen);
	dsi->header.dsi_code = 0; 
 
   	my_dsi_stream_send(dsi, dsi->commands, dsi->datalen);
	/* ------------------ */
	my_dsi_data_receive(dsi);

	dump_header(dsi);
	return(dsi->header.dsi_code);
}
/* ------------------------------- 
*/
unsigned int FPEnumerate_ext(CONN *conn, u_int16_t vol, int did , char *name, u_int16_t f_bitmap, u_int16_t d_bitmap)
{
int ofs;
u_int16_t bitmap;
u_int16_t len;
u_int16_t tp;
u_int32_t hint;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Enumerate_ext Vol %d did : 0x%x <%s>\n\n", vol, ntohl(did), name);
	memset(dsi->commands, 0, DSI_CMDSIZ);
	dsi->header.dsi_flags = DSIFL_REQUEST;     
	dsi->header.dsi_command = DSIFUNC_CMD;
	dsi->header.dsi_requestID = htons(dsi_clientID(dsi));
	ofs = 0;
	dsi->commands[ofs++] = AFP_ENUMERATE_EXT;
	dsi->commands[ofs++] = 0;

	memcpy(dsi->commands +ofs, &vol, sizeof(vol));
	ofs += sizeof(vol);
	
	memcpy(dsi->commands +ofs, &did, sizeof(did));
	ofs += sizeof(did);

	bitmap = htons(f_bitmap);
	memcpy(dsi->commands +ofs, &bitmap, sizeof(bitmap));
	ofs += sizeof(bitmap);
	
	bitmap = htons(d_bitmap);;
	memcpy(dsi->commands +ofs, &bitmap, sizeof(bitmap));
	ofs += sizeof(bitmap);

	bitmap = htons(1024);		/* reqcnt */
	memcpy(dsi->commands +ofs, &bitmap, sizeof(bitmap));
	ofs += sizeof(bitmap);
	
	bitmap = htons(1);		/* sindex  */
	memcpy(dsi->commands +ofs, &bitmap, sizeof(bitmap));
	ofs += sizeof(bitmap);

	bitmap = htons(DSI_DATASIZ);		/* maxsize  */
	memcpy(dsi->commands +ofs, &bitmap, sizeof(bitmap));
	ofs += sizeof(bitmap);

	dsi->commands[ofs++] = 3;		/* UTF8 name */
	hint = htonl(0x08000103);
	memcpy(dsi->commands +ofs, &hint, sizeof(hint));
	ofs += sizeof(hint);

	len = strlen(name);
	tp = htons(len);
	memcpy(dsi->commands +ofs, &tp, sizeof(tp));
	ofs += sizeof(tp);

	strncpy(&dsi->commands[ofs], name, len);
	ofs += len;
		
	dsi->datalen = ofs;
	dsi->header.dsi_len = htonl(dsi->datalen);
	dsi->header.dsi_code = 0; 
 
   	my_dsi_stream_send(dsi, dsi->commands, dsi->datalen);
	/* ------------------ */
	my_dsi_data_receive(dsi);

	dump_header(dsi);
	return(dsi->header.dsi_code);
}

/* ------------------------------- 
*/
unsigned int FPEnumerate_ext2(CONN *conn, u_int16_t vol, int did , char *name, u_int16_t f_bitmap, u_int16_t d_bitmap)
{
int ofs;
u_int16_t bitmap;
u_int16_t len;
u_int16_t tp;
u_int32_t hint;
u_int32_t int32;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Enumerate_ext2 Vol %d did : 0x%x <%s>\n\n", vol, ntohl(did), name);
	memset(dsi->commands, 0, DSI_CMDSIZ);
	dsi->header.dsi_flags = DSIFL_REQUEST;     
	dsi->header.dsi_command = DSIFUNC_CMD;
	dsi->header.dsi_requestID = htons(dsi_clientID(dsi));
	ofs = 0;
	dsi->commands[ofs++] = AFP_ENUMERATE_EXT2;
	dsi->commands[ofs++] = 0;

	memcpy(dsi->commands +ofs, &vol, sizeof(vol));
	ofs += sizeof(vol);
	
	memcpy(dsi->commands +ofs, &did, sizeof(did));
	ofs += sizeof(did);

	bitmap = htons(f_bitmap);
	memcpy(dsi->commands +ofs, &bitmap, sizeof(bitmap));
	ofs += sizeof(bitmap);
	
	bitmap = htons(d_bitmap);
	memcpy(dsi->commands +ofs, &bitmap, sizeof(bitmap));
	ofs += sizeof(bitmap);

	bitmap = htons(1024);		/* reqcnt */
	memcpy(dsi->commands +ofs, &bitmap, sizeof(bitmap));
	ofs += sizeof(bitmap);
	
	int32 = htonl(1);		/* sindex  */
	memcpy(dsi->commands +ofs, &int32, sizeof(int32));
	ofs += sizeof(int32);

	int32 = htonl(DSI_DATASIZ);		/* maxsize  */
	memcpy(dsi->commands +ofs, &int32, sizeof(int32));
	ofs += sizeof(int32);

	dsi->commands[ofs++] = 3;		/* UTF8 name */
	hint = htonl(0x08000103);
	memcpy(dsi->commands +ofs, &hint, sizeof(hint));
	ofs += sizeof(hint);

	len = strlen(name);
	tp = htons(len);
	memcpy(dsi->commands +ofs, &tp, sizeof(tp));
	ofs += sizeof(tp);

	strncpy(&dsi->commands[ofs], name, len);
	ofs += len;
		
	dsi->datalen = ofs;
	dsi->header.dsi_len = htonl(dsi->datalen);
	dsi->header.dsi_code = 0; 
 
   	my_dsi_stream_send(dsi, dsi->commands, dsi->datalen);
	/* ------------------ */
	my_dsi_data_receive(dsi);

	dump_header(dsi);
	return(dsi->header.dsi_code);
}


/* ------------------------------- 
*/
unsigned int FPDelete(CONN *conn, u_int16_t vol, int did , char *name)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Delete Vol %d did : 0x%x <%s>\n\n", vol, ntohl(did), name);
	ret = AFPDelete(conn,vol, did , name);

	dump_header(dsi);
	return ret;
}

/* ------------------------------- 
*/
unsigned int FPGetComment(CONN *conn, u_int16_t vol, int did , char *name)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Get Comment Vol %d did : %d <%s>\n\n", vol, did, name);
	ret = AFPGetComment(conn,vol, did , name);

	dump_header(dsi);
	return ret;
}

/* ------------------------------- 
*/
unsigned int FPRemoveComment(CONN *conn, u_int16_t vol, int did , char *name)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Remove Comment Vol %d did : %d <%s>\n\n", vol, did, name);
	ret = AFPRemoveComment(conn,vol, did , name);

	dump_header(dsi);
	return ret;
}

/* ------------------------------- 
*/
unsigned int FPAddComment(CONN *conn, u_int16_t vol, int did , char *name, char *cmt)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Add Comment Vol %d did : %d <%s> comment <%s>\n\n", vol, did, name, cmt);
	ret = AFPAddComment(conn,vol, did , name, cmt);

	dump_header(dsi);
	return ret;
}

/* ------------------------------- 
	depreciate (for variable dir volume)
*/
unsigned int FPOpenDir(CONN *conn, u_int16_t vol, int did , char *name)
{
int ofs;
int dir = 0;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Open Directory Vol %d did : 0x%x <%s>\n\n", vol, htonl(did), name);
	memset(dsi->commands, 0, DSI_CMDSIZ);
	dsi->header.dsi_flags = DSIFL_REQUEST;     
	dsi->header.dsi_command = DSIFUNC_CMD;
	dsi->header.dsi_requestID = htons(dsi_clientID(dsi));
	ofs = 0;
	dsi->commands[ofs++] = AFP_OPENDIR;
	dsi->commands[ofs++] = 0;

	memcpy(dsi->commands +ofs, &vol, sizeof(vol));
	ofs += sizeof(vol);
	
	memcpy(dsi->commands +ofs, &did, sizeof(did));
	ofs += sizeof(did);

    ofs = FPset_name(conn, ofs, name);
		
	dsi->datalen = ofs;
	dsi->header.dsi_len = htonl(dsi->datalen);
	dsi->header.dsi_code = 0; 
 
   	my_dsi_stream_send(dsi, dsi->commands, dsi->datalen);
	/* ------------------ */
	my_dsi_data_receive(dsi);

	dump_header(dsi);
	if (!dsi->header.dsi_code) {
		ofs = 0;
		memcpy(&dir, dsi->data, sizeof(dir));			/* did */
		ofs += sizeof(dir);
		fprintf(stderr,"directory ID 0x%x\n", ntohl(dir));
	}
	return(dir);
}

/* -------------------------------- */
unsigned int FPCreateDir(CONN *conn, u_int16_t vol, int did , char *name)
{
int dir = 0;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Create Directory Vol %d did : 0x%x <%s>\n\n", vol, ntohl(did), name);

	dir = AFPCreateDir(conn,vol, did , name);

	dump_header(dsi);
	return(dir);
}

/* ------------------------------- 
	type : resource or data
*/
u_int16_t  FPOpenFork(CONN *conn, u_int16_t vol, char type, u_int16_t bitmap, int did , char *name, int access)
{
u_int16_t ofork;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Open Fork %s Vol %d did : 0x%x <%s> access %x\n\n", (type == OPENFORK_DATA)?"data":"resource", 
						vol, ntohl(did), name, access);

	ofork = AFPOpenFork(conn,vol, type, bitmap, did , name, access);

	dump_header(dsi);
	if (!dsi->header.dsi_code) {
		fprintf(stderr,"fork refnum ID %d\n", ofork);
	}

	return ofork;
}

/* ------------------------- */
unsigned int FPCreateFile(CONN *conn, u_int16_t vol, char type, int did , char *name)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Create File %s Vol %d did : 0x%x <%s>\n\n", (type )?"HARD":"SOFT", vol, ntohl(did), name);
	ret = AFPCreateFile(conn,vol, type, did , name);
	dump_header(dsi);
	return ret;
}

/* ------------------------------- */
unsigned int FPGetForkParam(CONN *conn, u_int16_t fork, u_int16_t bitmap)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Get Fork param fork %d bitmap 0x%x\n\n", fork, bitmap);

	ret = AFPGetForkParam(conn,fork, bitmap);
	dump_header(dsi);
	return ret;	
}

/* ------------------------------- */
unsigned int FPGetAppl(CONN *conn, u_int16_t dt, char *name, u_int16_t index, u_int16_t bitmap)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"GetAppl DT %d <%s>\n\n", dt, name);
	ret = AFPGetAPPL(conn,dt, name, index, bitmap);
	dump_header(dsi);
	return ret;	
}

/* ------------------------------- */
unsigned int FPAddAPPL(CONN *conn, u_int16_t dt, int did, char *creator, u_int32_t tag, char *name)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"AddAPPL DT %d creator: %s tag %x did: 0x%x <%s>\n\n", dt, creator, tag, htonl(did), name);
	ret = AFPAddAPPL(conn, dt, did, creator, tag, name);
	dump_header(dsi);
	return ret;	
}

/* ------------------------------- */
unsigned int FPRemoveAPPL(CONN *conn, u_int16_t dt, int did, char *creator, char *name)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"RemoveAPPL DT %d creator: %s did: 0x%x <%s>\n\n", dt, creator, htonl(did), name);
	ret = AFPRemoveAPPL(conn, dt, did, creator, name);
	dump_header(dsi);
	return ret;	
}

/* ------------------------------- */
unsigned int FPSetForkParam(CONN *conn, u_int16_t fork,  u_int16_t bitmap, off_t size)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Set Fork param fork %d bitmap 0x%x size %lld\n\n", fork, bitmap,size);
	ret = AFPSetForkParam(conn, fork,  bitmap, size);
	dump_header(dsi);
	return ret;
}

/* ------------------------------- */
unsigned int FPSetDirParms(CONN *conn, u_int16_t vol, int did, char *name, u_int16_t bitmap, 
                  struct afp_filedir_parms *dir )
{
int ofs;
u_int16_t rbitmap;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Set DirParam 0x%x %s bitmap 0x%x\n\n", ntohl(did), name, bitmap);

	memset(dsi->commands, 0, DSI_CMDSIZ);
	dsi->header.dsi_flags = DSIFL_REQUEST;     
	dsi->header.dsi_command = DSIFUNC_CMD;
	dsi->header.dsi_requestID = htons(dsi_clientID(dsi));
	ofs = 0;
	dsi->commands[ofs++] = AFP_SETDIRPARAM;
	dsi->commands[ofs++] = 0;

	memcpy(dsi->commands +ofs, &vol, sizeof(vol));	/* volume */
	ofs += sizeof(vol);

	memcpy(dsi->commands +ofs, &did, sizeof(did));  /* directory did */
	ofs += sizeof(did);

	rbitmap = htons(bitmap);
	memcpy(dsi->commands +ofs, &rbitmap, sizeof(bitmap));  /* bitmap */
	ofs += sizeof(bitmap);

	ofs = FPset_name(conn, ofs, name);
	if ((ofs & 1)) {
		ofs++;
	}
	ofs +=afp_filedir_pack(dsi->commands +ofs, dir, 0, bitmap);

	dsi->datalen = ofs;
	dsi->header.dsi_len = htonl(dsi->datalen);
	dsi->header.dsi_code = 0; 
 
   	my_dsi_stream_send(dsi, dsi->commands, dsi->datalen);
	/* ------------------ */
	my_dsi_receive(dsi);
	dump_header(dsi);
	return dsi->header.dsi_code;
}

/* ------------------------------- */
unsigned int FPSetFileParams(CONN *conn, u_int16_t vol, int did, char *name, u_int16_t bitmap, 
                  struct afp_filedir_parms *fil )
{
int ofs;
u_int16_t rbitmap;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Set FilParam 0x%x %s bitmap 0x%x\n\n", ntohl(did), name, bitmap);

	memset(dsi->commands, 0, DSI_CMDSIZ);
	dsi->header.dsi_flags = DSIFL_REQUEST;     
	dsi->header.dsi_command = DSIFUNC_CMD;
	dsi->header.dsi_requestID = htons(dsi_clientID(dsi));
	ofs = 0;
	dsi->commands[ofs++] = AFP_SETFILEPARAM;
	dsi->commands[ofs++] = 0;

	memcpy(dsi->commands +ofs, &vol, sizeof(vol));	/* volume */
	ofs += sizeof(vol);

	memcpy(dsi->commands +ofs, &did, sizeof(did));  /* directory did */
	ofs += sizeof(did);

	rbitmap = htons(bitmap);
	memcpy(dsi->commands +ofs, &rbitmap, sizeof(bitmap));  /* bitmap */
	ofs += sizeof(bitmap);

	ofs = FPset_name(conn, ofs, name);
	if ((ofs & 1)) {
		ofs++;
	}
	ofs +=afp_filedir_pack(dsi->commands +ofs, fil, bitmap,0);

	dsi->datalen = ofs;
	dsi->header.dsi_len = htonl(dsi->datalen);
	dsi->header.dsi_code = 0; 
 
   	my_dsi_stream_send(dsi, dsi->commands, dsi->datalen);
	/* ------------------ */
	my_dsi_receive(dsi);
	dump_header(dsi);
	return dsi->header.dsi_code;
	
}

/* ------------------------------- */
unsigned int FPCatSearch(CONN *conn, u_int16_t vol, u_int32_t  nbe, char *pos, u_int16_t f_bitmap, u_int16_t d_bitmap,
                                u_int32_t rbitmap, struct afp_filedir_parms *filedir, struct afp_filedir_parms *filedir2)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"FPCatSearch vol: %d bitmap 0x%x\n\n", vol, rbitmap);
	ret = AFPCatSearch(conn, vol, nbe, pos, f_bitmap, d_bitmap, rbitmap, filedir, filedir2);
	dump_header(dsi);
	return ret;
}

/* ------------------------------- */
unsigned int FPCatSearchExt(CONN *conn, u_int16_t vol, u_int32_t  nbe, char *pos, u_int16_t f_bitmap, u_int16_t d_bitmap,
                                u_int32_t rbitmap, struct afp_filedir_parms *filedir, struct afp_filedir_parms *filedir2)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"FPCatSearchExt vol: %d bitmap 0x%x\n\n", vol, rbitmap);
	ret = AFPCatSearchExt(conn, vol, nbe, pos, f_bitmap, d_bitmap, rbitmap, filedir, filedir2);
	dump_header(dsi);
	return ret;
}

/* ------------------------------- */
unsigned int FPSetFilDirParam(CONN *conn, u_int16_t vol, int did, char *name, u_int16_t bitmap,
                  struct afp_filedir_parms *fil )
{
int ofs;
u_int16_t rbitmap;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Set FilDirParam 0x%x <%s> bitmap 0x%x\n\n", ntohl(did), name, bitmap);

	memset(dsi->commands, 0, DSI_CMDSIZ);
	dsi->header.dsi_flags = DSIFL_REQUEST;     
	dsi->header.dsi_command = DSIFUNC_CMD;
	dsi->header.dsi_requestID = htons(dsi_clientID(dsi));
	ofs = 0;
	dsi->commands[ofs++] = AFP_SETFLDRPARAM;
	dsi->commands[ofs++] = 0;

	memcpy(dsi->commands +ofs, &vol, sizeof(vol));	/* volume */
	ofs += sizeof(vol);

	memcpy(dsi->commands +ofs, &did, sizeof(did));  /* directory did */
	ofs += sizeof(did);

	rbitmap = htons(bitmap);
	memcpy(dsi->commands +ofs, &rbitmap, sizeof(bitmap));  /* bitmap */
	ofs += sizeof(bitmap);

	ofs = FPset_name(conn, ofs, name);
	if ((ofs & 1)) {
		ofs++;
	}
	ofs +=afp_filedir_pack(dsi->commands +ofs, fil, bitmap,bitmap);

	dsi->datalen = ofs;
	dsi->header.dsi_len = htonl(dsi->datalen);
	dsi->header.dsi_code = 0; 
 
   	my_dsi_stream_send(dsi, dsi->commands, dsi->datalen);
	/* ------------------ */
	my_dsi_receive(dsi);
	dump_header(dsi);
	return dsi->header.dsi_code;
	
}

/* ------------------------------- */
unsigned int FPCopyFile(CONN *conn, u_int16_t svol, int sdid, u_int16_t dvol, int ddid, char *src, char *dst)
{
int ofs;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Copy File Vol %d did : %d <%s> ==> %s\n\n", svol, ntohl(sdid), src, dst);
	memset(dsi->commands, 0, DSI_CMDSIZ);
	dsi->header.dsi_flags = DSIFL_REQUEST;     
	dsi->header.dsi_command = DSIFUNC_CMD;
	dsi->header.dsi_requestID = htons(dsi_clientID(dsi));
	ofs = 0;
	dsi->commands[ofs++] = AFP_COPYFILE;
	dsi->commands[ofs++] = 0;

	memcpy(dsi->commands +ofs, &svol, sizeof(svol));	/* volume */
	ofs += sizeof(svol);

	memcpy(dsi->commands +ofs, &sdid, sizeof(sdid));  /* directory did */
	ofs += sizeof(sdid);

	memcpy(dsi->commands +ofs, &dvol, sizeof(dvol));	/* volume */
	ofs += sizeof(dvol);

	memcpy(dsi->commands +ofs, &ddid, sizeof(ddid));  /* directory did */
	ofs += sizeof(ddid);

	ofs = FPset_name(conn, ofs, src);

	ofs = FPset_name(conn, ofs, "");

	ofs = FPset_name(conn, ofs, dst);

	dsi->datalen = ofs;
	dsi->header.dsi_len = htonl(dsi->datalen);
	dsi->header.dsi_code = 0; 
 
   	my_dsi_stream_send(dsi, dsi->commands, dsi->datalen);
	/* ------------------ */
	my_dsi_data_receive(dsi);

	dump_header(dsi);
	return(dsi->header.dsi_code);
}

/* ------------------------------- */
unsigned int FPExchangeFile(CONN *conn, u_int16_t vol, int sdid, int ddid, char *src, char *dst)
{
int ofs;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Exchange File Vol: %d did: 0x%x \"%s\" ==> 0x%x \"%s\"\n\n", vol, ntohl(sdid), src, 
				ntohl(ddid), dst);
	memset(dsi->commands, 0, DSI_CMDSIZ);
	dsi->header.dsi_flags = DSIFL_REQUEST;     
	dsi->header.dsi_command = DSIFUNC_CMD;
	dsi->header.dsi_requestID = htons(dsi_clientID(dsi));
	ofs = 0;
	dsi->commands[ofs++] = AFP_EXCHANGEFILE;
	dsi->commands[ofs++] = 0;

	memcpy(dsi->commands +ofs, &vol, sizeof(vol));	/* volume */
	ofs += sizeof(vol);

	memcpy(dsi->commands +ofs, &sdid, sizeof(sdid));  /* directory did */
	ofs += sizeof(sdid);

	memcpy(dsi->commands +ofs, &ddid, sizeof(ddid));  /* directory did */
	ofs += sizeof(ddid);

	ofs = FPset_name(conn, ofs, src);

	ofs = FPset_name(conn, ofs, dst);

	dsi->datalen = ofs;
	dsi->header.dsi_len = htonl(dsi->datalen);
	dsi->header.dsi_code = 0; 
 
   	my_dsi_stream_send(dsi, dsi->commands, dsi->datalen);
	/* ------------------ */
	my_dsi_data_receive(dsi);

	dump_header(dsi);
	return(dsi->header.dsi_code);
}

/* ------------------------------- */
unsigned int FPMoveAndRename(CONN *conn, u_int16_t svol, int sdid, int ddid, char *src, char *dst)
{
int ofs;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Move and rename Vol: %d did: 0x%x <%s> ==> 0x%x <%s>\n\n", svol, ntohl(sdid), src, ntohl(ddid), dst);
	memset(dsi->commands, 0, DSI_CMDSIZ);
	dsi->header.dsi_flags = DSIFL_REQUEST;     
	dsi->header.dsi_command = DSIFUNC_CMD;
	dsi->header.dsi_requestID = htons(dsi_clientID(dsi));
	ofs = 0;
	dsi->commands[ofs++] = AFP_MOVE;
	dsi->commands[ofs++] = 0;

	memcpy(dsi->commands +ofs, &svol, sizeof(svol));	/* volume */
	ofs += sizeof(svol);

	memcpy(dsi->commands +ofs, &sdid, sizeof(sdid));  /* source directory  did */
	ofs += sizeof(sdid);

	memcpy(dsi->commands +ofs, &ddid, sizeof(ddid));  /* directory did */
	ofs += sizeof(ddid);

	ofs = FPset_name(conn, ofs, src);

	ofs = FPset_name(conn, ofs, "");

	ofs = FPset_name(conn, ofs, dst);

	dsi->datalen = ofs;
	dsi->header.dsi_len = htonl(dsi->datalen);
	dsi->header.dsi_code = 0; 
 
   	my_dsi_stream_send(dsi, dsi->commands, dsi->datalen);
	/* ------------------ */
	my_dsi_data_receive(dsi);

	dump_header(dsi);
	return(dsi->header.dsi_code);
}
/* ------------------------------- */
unsigned int FPRename(CONN *conn, u_int16_t svol, int sdid, char *src, char *dst)
{
int ofs;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"Rename  File Vol %d did : %d <%s> ==> %s\n\n", svol, ntohl(sdid), src, dst);
	memset(dsi->commands, 0, DSI_CMDSIZ);
	dsi->header.dsi_flags = DSIFL_REQUEST;     
	dsi->header.dsi_command = DSIFUNC_CMD;
	dsi->header.dsi_requestID = htons(dsi_clientID(dsi));
	ofs = 0;
	dsi->commands[ofs++] = AFP_RENAME;
	dsi->commands[ofs++] = 0;

	memcpy(dsi->commands +ofs, &svol, sizeof(svol));	/* volume */
	ofs += sizeof(svol);

	memcpy(dsi->commands +ofs, &sdid, sizeof(sdid));  /* directory did */
	ofs += sizeof(sdid);

	ofs = FPset_name(conn, ofs, src);

	ofs = FPset_name(conn, ofs, dst);

	dsi->datalen = ofs;
	dsi->header.dsi_len = htonl(dsi->datalen);
	dsi->header.dsi_code = 0; 
 
   	my_dsi_stream_send(dsi, dsi->commands, dsi->datalen);
	/* ------------------ */
	my_dsi_data_receive(dsi);

	dump_header(dsi);
	return(dsi->header.dsi_code);
}

/* ------------------------------- */
unsigned int FPRead(CONN *conn, u_int16_t fork, int offset, int size, char *data)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"read fork %d  offset %d size %d\n\n", fork , offset, size);

	ret = AFPRead(conn,fork, offset, size, data);

	dump_header(dsi);
	return ret;
}

/* ------------------------------- */
unsigned int FPRead_ext (CONN *conn, u_int16_t fork, off_t offset, off_t size, char *data)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"read ext fork %d  offset %lld size %lld\n\n", fork , offset, size);

	ret = AFPRead_ext(conn,fork, offset, size, data);

	dump_header(dsi);
	return ret;
}

/* ------------------------------- */
unsigned int FPWrite(CONN *conn, u_int16_t fork, int offset, int size, char *data, char whence)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"write fork %d  offset %d size %d from 0x%x\n\n", fork , offset, size, (unsigned)whence);

	ret = AFPWrite(conn,fork, offset, size, data, whence);
	dump_header(dsi);
	return ret;
}

/* ------------------------------- */
unsigned int FPWrite_ext(CONN *conn, u_int16_t fork, off_t  offset, off_t size, char *data, char whence)
{
int ret;
DSI *dsi;

	dsi = &conn->dsi;

	fprintf(stderr,"---------------------\n");
	fprintf(stderr,"write_ext fork %d  offset %lld size %lld\n\n", fork , offset, size);

	ret = AFPWrite_ext(conn,fork, offset, size, data, whence);
	dump_header(dsi);
	return ret;
}

