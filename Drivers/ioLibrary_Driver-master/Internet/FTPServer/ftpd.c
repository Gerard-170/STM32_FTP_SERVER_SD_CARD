/*
* Wiznet.
* (c) Copyright 2002, Wiznet.
*
* Filename	: ftpd.c
* Version	: 1.0
* Programmer(s)	: 
* Created	: 2003/01/28
* Description   : FTP daemon. (AVR-GCC Compiler)
*/


#include <stdio.h> 
#include <ctype.h> 
#include <string.h>
#include <limits.h>
#include <stdarg.h>
#include "stdio_private.h"
#include "socket.h"
#include "ftpd.h"

/* Command table */
static char *commands[] = {
	"user",
	"acct",
	"pass",
	"type",
	"list",
	"cwd",
	"dele",
	"name",
	"quit",
	"retr",
	"stor",
	"port",
	"nlst",
	"pwd",
	"xpwd",
	"mkd",
	"xmkd",
	"xrmd",
	"rmd ",
	"stru",
	"mode",
	"syst",
	"xmd5",
	"xcwd",
	"feat",
	"pasv",
	"size",
	"mlsd",
	"appe",
	NULL
};

#if 0
/* Response messages */
char banner[] = "220 %s FTP version %s ready.\r\n";
char badcmd[] = "500 Unknown command '%s'\r\n";
char binwarn[] = "100 Warning: type is ASCII and %s appears to be binary\r\n";
char unsupp[] = "500 Unsupported command or option\r\n";
char givepass[] = "331 Enter PASS command\r\n";
char logged[] = "230 Logged in\r\n";
char typeok[] = "200 Type %s OK\r\n";
char only8[] = "501 Only logical bytesize 8 supported\r\n";
char deleok[] = "250 File deleted\r\n";
char mkdok[] = "200 MKD ok\r\n";
char delefail[] = "550 Delete failed: %s\r\n";
char pwdmsg[] = "257 \"%s\" is current directory\r\n";
char badtype[] = "501 Unknown type \"%s\"\r\n";
char badport[] = "501 Bad port syntax\r\n";
char unimp[] = "502 Command does not implemented yet.\r\n";
char bye[] = "221 Goodbye!\r\n";
char nodir[] = "553 Can't read directory \"%s\": %s\r\n";
char cantopen[] = "550 Can't read file \"%s\": %s\r\n";
char sending[] = "150 Opening data connection for %s (%d.%d.%d.%d,%d)\r\n";
char cantmake[] = "553 Can't create \"%s\": %s\r\n";
char writerr[] = "552 Write error: %s\r\n";
char portok[] = "200 PORT command successful.\r\n";
char rxok[] = "226 Transfer complete.\r\n";
char txok[] = "226 Transfer complete.\r\n";
char noperm[] = "550 Permission denied\r\n";
char noconn[] = "425 Data connection reset\r\n";
char lowmem[] = "421 System overloaded, try again later\r\n";
char notlog[] = "530 Please log in with USER and PASS\r\n";
char userfirst[] = "503 Login with USER first.\r\n";
char okay[] = "200 Ok\r\n";
char syst[] = "215 %s Type: L%d Version: %s\r\n";
char sizefail[] = "550 File not found\r\n";
#endif

uint8_t remote_ip[4];
uint16_t  remote_port;
uint8_t local_ip[4];
uint16_t  local_port;
uint8_t connect_state_control = 0;
uint8_t connect_state_data = 0;

struct ftpd ftp;

int ftpd_dsock_ready();
int ftpd_listcmd(char* arg);
int ftpd_retrcmd(char* arg);
int ftpd_storcmd(char* arg);
int proc_ftpd(char * buf);
char ftplogin(char * pass);
int pport(char * arg);

int current_year = 2014;
int current_month = 12;
int current_day = 31;
int current_hour = 10;
int current_min = 10;
int current_sec = 30;

//PV Funciones para FATFS Usuario
FRESULT flag_FF;

//Prototipos de funciones implementadas
//scan_files(ftp.workingdir, dbuf, (int *)&size);
FRESULT scan_files (char* path, char* dbuf, int* size); /* Start node to be scanned (***also used as work area***) */

// Funciones modificadas para el FTP:
/*La siguiente funcion escanea los archivos en la SD y devuelve el buffer completo*/
FRESULT scan_files (char* path, char* dbuf, int* size)/* Start node to be scanned (***also used as work area***) */
{
    FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;
    //char temp[10];
    //strcpy(temp, "/");


    res = f_opendir(&dir, path); /* Open the directory */
    if(res != FR_OK)
    	Error_Handler();
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) {                    /* It is a directory, si es un directorio */
                i = strlen(path);
                sprintf(&path[i], "/%s", fno.fname);
                res = scan_files(path, dbuf, (int *)&size);                    /* Enter the directory */
                if (res != FR_OK) break;
                path[i] = 0;
            } else {                                       /* It is a file. */
                //printf("%s/%s\n", path, fno.fname);
                i = strlen((char*)dbuf);
                *size = sprintf(&dbuf[i], "%s/%s\n", path, fno.fname);
            }
        }
        f_closedir(&dir);
    }

    return res;
}


/* La siguiente Funcion devuelve el tamaño en bytes de los archivos*/
int get_filesize(char* path, char* filename)
{
	FIL file_s;
	FSIZE_t	o_size;

	if(f_open(&file_s, filename, FA_READ) != FR_OK)
	    		Error_Handler();
	o_size = file_s.obj.objsize;
	if(f_close(&file_s) != FR_OK)
	    		Error_Handler();
	return (int) o_size;

}


int fsprintf(uint8_t s, const char *format, ...)
{
	int i;
/*
	char buf[_FTP_FILENAME_LEN_];
	FILE f;
	va_list ap;

	f.flags = __SWR | __SSTR;
	f.buf = buf;
	f.size = INT_MAX;
	va_start(ap, format);
	i = vfprintf(&f, format, ap);
	va_end(ap);
	buf[f.len] = 0;

	send(s, (uint8_t *)buf, strlen(buf));
*/
	return i;
}

void ftpd_init(char* user, char* pass)
{
	uint8_t i;
	uint8_t status;
	wiz_NetInfo gWIZNETINFO;
	ctlnetwork(CN_GET_NETINFO, (void*) &gWIZNETINFO);

	ftp.is_login = 0;	//ftp.state = FTPS_NOT_LOGIN;
	ftp.is_active = 1;	//ftp.dsock_mode = ACTIVE_MODE;
	//ftp.current_cmd = NO_CMD;
	strcpy(ftp.user, user);
	strcpy(ftp.pass, pass);

	printf("Save_user0: %p\r\n", &ftp);
	local_ip[0] = gWIZNETINFO.ip[0];
	local_ip[1] = gWIZNETINFO.ip[1];
	local_ip[2] = gWIZNETINFO.ip[2];
	local_ip[3] = gWIZNETINFO.ip[3];
	local_port = 35000;
	strcpy(ftp.workingdir, "/");
	
	for(i = 0; i < SOCK_MAX_NUM; i++)
	{
		getsockopt(i, SO_STATUS, &status);
		if(status == SOCK_CLOSED)
		{
			ftp.ctrl_sock = i++;
			break;
		}
	}
	for(; i < SOCK_MAX_NUM; i++)
	{
		getsockopt(i, SO_STATUS, &status);
		if(status == SOCK_CLOSED)
		{
			ftp.data_sock = i;
			break;;
		}
	}


}

uint8_t ftpd_run(uint8_t * dbuf)
{
	uint16_t size = 0;
	long ret = 0;
	uint32_t recv_byte;
#if defined(F_FILESYSTEM)
	//FILINFO fno;
#endif

	//memset(dbuf, 0, sizeof(_MAX_SS ));
	
    switch(getSn_SR(ftp.ctrl_sock))
    {
    	case SOCK_ESTABLISHED :
    		if(!connect_state_control)
    		{
#if defined(_FTP_DEBUG_)
    			printf("%d:FTP Connected\r\n", ftp.ctrl_sock);
#endif
    			//fsprintf(ftp.ctrl_sock, banner, HOSTNAME, VERSION);
    			strcpy(ftp.workingdir, "/");
    			sprintf((char *)dbuf, "220 %s FTP version %s ready.\r\n", HOSTNAME, VERSION);
    			ret = send(ftp.ctrl_sock, (uint8_t *)dbuf, strlen((const char *)dbuf));
    			if(ret < 0)
    			{
#if defined(_FTP_DEBUG_)
    				printf("%d:send() error:%ld\r\n",ftp.ctrl_sock,ret);
#endif
    				close(ftp.ctrl_sock);
    				return ret;
    			}
    			connect_state_control = 1;
    		}
	
#if defined(_FTP_DEBUG_)
    		//printf("ftp socket %d\r\n", ftp.ctrl_sock);
#endif
			 
    		if((size = getSn_RX_RSR(ftp.ctrl_sock)) > 0) // Don't need to check SOCKERR_BUSY because it doesn't not occur.
    		{
#if defined(_FTP_DEBUG_)
    			printf("size: %d\r\n", size);
    			printf("Save_user1: %p\r\n", &dbuf);
#endif

    			memset(dbuf, 0, _MAX_SS );

    			//memset(dbuf, 0, 16);                      //------------------------------------------------------------>>  Workaround temporal

    			if(size > _MAX_SS ) size = _MAX_SS  - 1;

    			ret = recv(ftp.ctrl_sock,dbuf,size);
    			dbuf[ret] = '\0';
    			if(ret != size)
    			{
    				if(ret==SOCK_BUSY) return 0;
    				if(ret < 0)
    				{
#if defined(_FTP_DEBUG_)
    					printf("%d:recv() error:%ld\r\n",ftp.ctrl_sock,ret);
#endif
    					close(ftp.ctrl_sock);
    					return ret;
    				}
    			}
#if defined(_FTP_DEBUG_)
    			printf("Rcvd Command: %s", dbuf);
    			printf("Save_user2: %s\r\n", ftp.user);
#endif
    			proc_ftpd((char *)dbuf);
    		}
    		break;

    	case SOCK_CLOSE_WAIT :
#if defined(_FTP_DEBUG_)
    		printf("%d:CloseWait\r\n",ftp.ctrl_sock);
#endif
    		if((ret=disconnect(ftp.ctrl_sock)) != SOCK_OK) return ret;
#if defined(_FTP_DEBUG_)
    		printf("%d:Closed\r\n",ftp.ctrl_sock);
#endif
    		break;

    	case SOCK_CLOSED :
#if defined(_FTP_DEBUG_)
    		printf("%d:FTPStart\r\n",ftp.ctrl_sock);
#endif
    		if((ret=socket(ftp.ctrl_sock, Sn_MR_TCP, IPPORT_FTP, 0x0)) != ftp.ctrl_sock)
    		{
#if defined(_FTP_DEBUG_)
    			printf("%d:socket() error:%ld\r\n", ftp.ctrl_sock, ret);
#endif
    			close(ftp.ctrl_sock);
    			return ret;
    		}
    		break;

    	case SOCK_INIT :
#if defined(_FTP_DEBUG_)
    		printf("%d:Opened\r\n",ftp.ctrl_sock);
#endif
    		//strcpy(ftp.workingdir, "/");
    		if( (ret = listen(ftp.ctrl_sock)) != SOCK_OK)
    		{
#if defined(_FTP_DEBUG_)
    			printf("%d:Listen error\r\n",ftp.ctrl_sock);
#endif
    			return ret;
    		}
			connect_state_control = 0;

#if defined(_FTP_DEBUG_)
			printf("%d:Listen ok\r\n",ftp.ctrl_sock);
#endif
			break;

    	default :
    		break;
    }
    return 0;
}

int proc_ftpd(char * buf)
{
	char **cmdp, *cp, *arg, *tmpstr;
	char sendbuf[200];
	int slen;

	printf("\r\nSave_user3: %s\r\n", ftp.user);

	/* Translate first word to lower case */
	for (cp = buf; *cp != ' ' && *cp != '\0'; cp++)
		*cp = tolower(*cp);

	/* Find command in table; if not present, return syntax error */
	for (cmdp = commands; *cmdp != NULL; cmdp++)
		if (strncmp(*cmdp, buf, strlen(*cmdp)) == 0)
			break;

	if (*cmdp == NULL)
	{
		//fsprintf(ftp.ctrl_sock, badcmd, buf);
		slen = sprintf(sendbuf, "500 Unknown command '%s'\r\n", buf);
		send(ftp.ctrl_sock, (uint8_t *)sendbuf, slen);
		return 0;
	}
	/* Allow only USER, PASS and QUIT before logging in */
	if (ftp.is_login == 0)
	{
		switch(cmdp - commands)
		{
			case USER_CMD:
			case PASS_CMD:
			case QUIT_CMD:
				break;
			default:
				//fsprintf(ftp.ctrl_sock, notlog);
				slen = sprintf(sendbuf, "530 Please log in with USER and PASS\r\n");
				send(ftp.ctrl_sock, (uint8_t *)sendbuf, slen);
				return 0;
		}
	}
	
	arg = &buf[strlen(*cmdp)];
	while(*arg == ' ') arg++;
	slen = strlen(arg);
	arg[slen - 1] = 0x00;
	arg[slen - 2] = 0x00;

	/* Execute specific command */
	switch (cmdp - commands)
	{
		case USER_CMD :
#if defined(_FTP_DEBUG_)
			printf("USER_CMD : %s\r\n", arg);
			printf("Save_user3 %s\r\n", ftp.user);
#endif

			if(strcmp(arg,ftp.user)==0)
			{
				slen = sprintf(sendbuf, "331 Enter PASS command\r\n");
				send(ftp.ctrl_sock, (uint8_t *)sendbuf, slen);
			}
			else
			{
				slen = sprintf(sendbuf, "530 Not logged in\r\n");
				send(ftp.ctrl_sock, (uint8_t *)sendbuf, slen);
			}
			break;


		case PASS_CMD :
#if defined(_FTP_DEBUG_)
			printf("PASS_CMD : %s", arg);
#endif
			if(strcmp(arg,ftp.pass)==0)
			{
			   printf("%s logged in\r\n", ftp.username);
			   slen = sprintf(sendbuf, "230 Logged on\r\n");
			   send(ftp.ctrl_sock, (uint8_t *)sendbuf, slen);
			   ftp.is_login = 1;
			}
			else
			{
			   printf("%s Not logged in\r\n", ftp.username);
			   slen = sprintf(sendbuf, "530 Not logged in\r\n");
			   send(ftp.ctrl_sock, (uint8_t *)sendbuf, slen);
			}
			break;

		case TYPE_CMD :
			switch(arg[0])
			{
				case 'A':
				case 'a':	/* Ascii */
					ftp.is_ascii = 1;
					//fsprintf(ftp.ctrl_sock, typeok, arg);
					slen = sprintf(sendbuf, "200 Type set to %s\r\n", arg);
					send(ftp.ctrl_sock, (uint8_t *)sendbuf, slen);
					break;

				case 'B':
				case 'b':	/* Binary */
				case 'I':
				case 'i':	/* Image */
					ftp.is_ascii = 0;
					//fsprintf(ftp.ctrl_sock, typeok, arg);
					slen = sprintf(sendbuf, "200 Type set to %s\r\n", arg);
					send(ftp.ctrl_sock, (uint8_t *)sendbuf, slen);
					break;

				default:	/* Invalid */
					//fsprintf(ftp.ctrl_sock, badtype, arg);
					slen = sprintf(sendbuf, "501 Unknown type \"%s\"\r\n", arg);
					send(ftp.ctrl_sock, (uint8_t *)sendbuf, slen);
					break;
			}
			break;

		case FEAT_CMD :
			slen = sprintf(sendbuf, "211-Features:\r\n MDTM\r\n REST STREAM\r\n SIZE\r\n MLST size*;type*;create*;modify*;\r\n MLSD\r\n UTF8\r\n CLNT\r\n MFMT\r\n211 END\r\n");
			send(ftp.ctrl_sock, (uint8_t *)sendbuf, slen);
			break;

		case QUIT_CMD :
#if defined(_FTP_DEBUG_)
			printf("QUIT_CMD\r\n");
#endif
			//fsprintf(ftp.ctrl_sock, bye);
			slen = sprintf(sendbuf, "221 Goodbye!\r\n");
			send(ftp.ctrl_sock, (uint8_t *)sendbuf, slen);
			disconnect(ftp.ctrl_sock);
			break;

		case RETR_CMD :
#if defined(_FTP_DEBUG_)
			printf("RETR_CMD\r\n");
#endif
			ftpd_retrcmd(arg);
			break;

		case APPE_CMD :
		case STOR_CMD:
#if defined(_FTP_DEBUG_)
			printf("STOR_CMD\r\n");
#endif
			ftpd_storcmd(arg);
			break;

		case PORT_CMD:
#if defined(_FTP_DEBUG_)
			printf("PORT_CMD\r\n");
#endif
			if (pport(arg) == -1){
				//fsprintf(ftp.ctrl_sock, badport);
				slen = sprintf(sendbuf, "501 Bad port syntax\r\n");
				send(ftp.ctrl_sock, (uint8_t *)sendbuf, slen);
			} else{
				//fsprintf(ftp.ctrl_sock, portok);
				ftp.is_active = 1; //ftp.dsock_mode = ACTIVE_MODE;
				//ftp.dsock_state = DATASOCK_READY;
				slen = sprintf(sendbuf, "200 PORT command successful.\r\n");
				send(ftp.ctrl_sock, (uint8_t *)sendbuf, slen);
			}
			ftpd_dsock_ready();

			break;

		case MLSD_CMD:
#if defined(_FTP_DEBUG_)
			printf("MLSD_CMD\r\n");
#endif
			ftpd_listcmd(arg);
			break;

		case LIST_CMD:
#if defined(_FTP_DEBUG_)
			printf("LIST_CMD\r\n");
#endif
			ftpd_listcmd(arg);
			break;

		case NLST_CMD:
#if defined(_FTP_DEBUG_)
			printf("NLST_CMD\r\n");
#endif
			break;

		case SYST_CMD:
			slen = sprintf(sendbuf, "215 UNIX emulated by WIZnet\r\n");
			send(ftp.ctrl_sock, (uint8_t *)sendbuf, slen);
			break;

		case PWD_CMD:
		case XPWD_CMD:
			slen = sprintf(sendbuf, "257 \"%s\" is current directory.\r\n", ftp.workingdir);
			send(ftp.ctrl_sock, (uint8_t *)sendbuf, slen);
			break;

		case PASV_CMD:
			slen = sprintf(sendbuf, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)\r\n", local_ip[0], local_ip[1], local_ip[2], local_ip[3], local_port >> 8, local_port & 0x00ff);
			send(ftp.ctrl_sock, (uint8_t *)sendbuf, slen);
			ftp.is_active = 0;
			ftpd_dsock_ready();
#if defined(_FTP_DEBUG_)
			printf("PASV port: %d\r\n", local_port);
#endif
		break;

		case SIZE_CMD:
			if(slen > 3)
			{
				tmpstr = strrchr(arg, '/');
				*tmpstr = 0;
#if defined(F_FILESYSTEM)
				slen = get_filesize(arg, tmpstr + 1);
#else
				slen = _MAX_SS ;
#endif
				if(slen > 0)
					slen = sprintf(sendbuf, "213 %d\r\n", slen);
				else
					slen = sprintf(sendbuf, "550 File not Found\r\n");
			}
			else
			{
				slen = sprintf(sendbuf, "550 File not Found\r\n");
			}
			send(ftp.ctrl_sock, (uint8_t *)sendbuf, slen);
			break;

		case CWD_CMD:
			if(slen > 3)
			{
				arg[slen - 3] = 0x00;
				tmpstr = strrchr(arg, '/');
				*tmpstr = 0;
#if defined(F_FILESYSTEM)
				slen = get_filesize(arg, tmpstr + 1);
#else
				slen = 0;
#endif
				*tmpstr = '/';
				if(slen == 0){
					slen = sprintf(sendbuf, "213 %d\r\n", slen);
					strcpy(ftp.workingdir, arg);
					slen = sprintf(sendbuf, "250 CWD successful. \"%s\" is current directory.\r\n", ftp.workingdir);
				}
				else
				{
					slen = sprintf(sendbuf, "550 CWD failed. \"%s\"\r\n", arg);
				}
			}
			else
			{
				strcpy(ftp.workingdir, arg);
				slen = sprintf(sendbuf, "250 CWD successful. \"%s\" is current directory.\r\n", ftp.workingdir);
			}
			send(ftp.ctrl_sock, (uint8_t *)sendbuf, slen);
			break;

		case MKD_CMD:
		case XMKD_CMD:
#if defined(F_FILESYSTEM)
			if (f_mkdir(arg) != 0)
			{
				slen = sprintf(sendbuf, "550 Can't create directory. \"%s\"\r\n", arg);
			}
			else
			{
				slen = sprintf(sendbuf, "257 MKD command successful. \"%s\"\r\n", arg);
				//strcpy(ftp.workingdir, arg);
			}
#else
			slen = sprintf(sendbuf, "550 Can't create directory. Permission denied\r\n");
#endif
			send(ftp.ctrl_sock, (uint8_t *)sendbuf, slen);
			break;

		case DELE_CMD:

#if defined(F_FILESYSTEM)
			if (f_unlink(arg) != 0)
			{
				slen = sprintf(sendbuf, "550 Could not delete. \"%s\"\r\n", arg);
			}
			else
			{
				slen = sprintf(sendbuf, "250 Deleted. \"%s\"\r\n", arg);
			}
#else
			slen = sprintf(sendbuf, "550 Could not delete. Permission denied\r\n");
#endif
			send(ftp.ctrl_sock, (uint8_t *)sendbuf, slen);
			break;

		case XCWD_CMD:
		case ACCT_CMD:
		case XRMD_CMD:
		case RMD_CMD:
		case STRU_CMD:
		case MODE_CMD:
		case XMD5_CMD:
			//fsprintf(ftp.ctrl_sock, unimp);
			slen = sprintf(sendbuf, "502 Command does not implemented yet.\r\n");
			send(ftp.ctrl_sock, (uint8_t *)sendbuf, slen);
			break;

		default:	/* Invalid */
			//fsprintf(ftp.ctrl_sock, badcmd, arg);
			slen = sprintf(sendbuf, "500 Unknown command \'%s\'\r\n", arg);
			send(ftp.ctrl_sock, (uint8_t *)sendbuf, slen);
			break;
	}
	
	return 1;
}


int pport(char * arg)
{
	int i;
	char* tok=0;

	for (i = 0; i < 4; i++)
	{
		if(i==0) tok = strtok(arg,",\r\n");
		else	 tok = strtok(NULL,",");
		remote_ip[i] = (uint8_t)atoi(tok);
		if (!tok)
		{
#if defined(_FTP_DEBUG_)
			printf("bad pport : %s\r\n", arg);
#endif
			return -1;
		}
	}
	remote_port = 0;
	for (i = 0; i < 2; i++)
	{
		tok = strtok(NULL,",\r\n");
		remote_port <<= 8;
		remote_port += atoi(tok);
		if (!tok)
		{
#if defined(_FTP_DEBUG_)
			printf("bad pport : %s\r\n", arg);
#endif
			return -1;
		}
	}
#if defined(_FTP_DEBUG_)
	printf("ip : %d.%d.%d.%d, port : %d\r\n", remote_ip[0], remote_ip[1], remote_ip[2], remote_ip[3], remote_port);
#endif

	return 0;
}




int ftpd_dsock_ready(){
    int ret;
    if(getSn_SR(ftp.data_sock)!= SOCK_CLOSED)
    {
        close(ftp.data_sock);
    }
    if(ftp.is_active == 1){
        socket(ftp.data_sock, Sn_MR_TCP, IPPORT_FTPD, 0x0);
        ret=connect(ftp.data_sock, remote_ip, remote_port);
        if(ret  != SOCK_OK){
#if defined(_FTP_DEBUG_)
            printf("%d:Connect error\r\n", ftp.data_sock);
#endif
            return ret;
        }

    }
    else
    {
        socket(ftp.data_sock, Sn_MR_TCP, local_port, 0x0);
        listen(ftp.data_sock);
    }
    return 0;
}

int ftpd_listcmd(char* arg){
    char sendbuf[200], dbuf[_MAX_SS];
    uint8_t tmp;
    int size;
    int16_t slen;

    slen = sprintf(sendbuf, "150 Opening data channel for directory listing of \"%s\"\r\n", ftp.workingdir);
    send(ftp.ctrl_sock, (uint8_t *)sendbuf, slen);
    getsockopt(ftp.data_sock, SO_STATUS, &tmp);
    if(tmp!=SOCK_ESTABLISHED)
    {
        ftpd_dsock_ready();
    }

#if defined(F_FILESYSTEM)
    				scan_files(ftp.workingdir, dbuf, (int *)&size);
#endif
#if defined(_FTP_DEBUG_)
    				printf("returned size: %d\r\n", size);
    				printf("%s\r\n", dbuf);
#endif
#if !defined(F_FILESYSTEM)
    if (strncmp(ftp.workingdir, "/$Recycle.Bin", sizeof("/$Recycle.Bin")) != 0)
        size = sprintf((char*)dbuf, "drwxr-xr-x 1 ftp ftp 0 Dec 31 2014 $Recycle.Bin\r\n-rwxr-xr-x 1 ftp ftp 512 Dec 31 2014 test.txt\r\n");
#endif
    size = strlen((char*)dbuf);
    send(ftp.data_sock, (uint8_t*)dbuf, size);
    disconnect(ftp.data_sock);
    size = sprintf((char*)dbuf, "226 Successfully transferred \"%s\"\r\n", ftp.workingdir);
    send(ftp.ctrl_sock, (uint8_t*)dbuf, size);
    return 0;
}

int ftpd_retrcmd(char* arg){
    uint16_t slen;
    size_t remain_filesize;
    uint8_t tmp;
    uint32_t blocklen, send_byte;
    char sendbuf[200], dbuf[_MAX_SS];

    if(strlen(ftp.workingdir) == 1)
        sprintf(ftp.filename, "/%s", arg);
    else
        sprintf(ftp.filename, "%s/%s", ftp.workingdir, arg);
    slen = sprintf(sendbuf, "150 Opening data channel for file download from server of \"%s\"\r\n", ftp.filename);
    send(ftp.ctrl_sock, (uint8_t *)sendbuf, slen);
    getsockopt(ftp.data_sock, SO_STATUS, &tmp);
    if(tmp!=SOCK_ESTABLISHED)
    {
        ftpd_dsock_ready();
    }
#if defined(_FTP_DEBUG_)
    printf("filename to retrieve : %s %d\r\n", ftp.filename, strlen(ftp.filename));
#endif
#if defined(F_FILESYSTEM)
	ftp.fr = f_open(&(ftp.fil), (const char *)ftp.filename, FA_READ);
	//print_filedsc(&(ftp.fil));
	if(ftp.fr == FR_OK){
		remain_filesize = ftp.fil.obj.objsize;
#if defined(_FTP_DEBUG_)
		printf("f_open return FR_OK\r\n");
#endif
		do{
#if defined(_FTP_DEBUG_)
			//printf("remained file size: %d\r\n", ftp.fil.fsize);
#endif
			memset(dbuf, 0, _MAX_SS);

			if(remain_filesize > _MAX_SS)
				send_byte = _MAX_SS;
			else
				send_byte = remain_filesize;

			ftp.fr = f_read(&(ftp.fil), dbuf, send_byte , &blocklen);
			if(ftp.fr != FR_OK)
				break;
#if defined(_FTP_DEBUG_)
			printf("#");
			//printf("----->fsize:%d recv:%d len:%d \r\n", remain_filesize, send_byte, blocklen);
			//printf("----->fn:%s data:%s \r\n", ftp.filename, dbuf);
#endif
			send(ftp.data_sock, dbuf, blocklen);
			remain_filesize -= blocklen;
		}while(remain_filesize != 0);
#if defined(_FTP_DEBUG_)
		printf("\r\nFile read finished\r\n");
#endif
		ftp.fr = f_close(&(ftp.fil));
	}else{
#if defined(_FTP_DEBUG_)
		printf("File Open Error: %d\r\n", ftp.fr);
#endif
	}
#else
    remain_filesize = strlen(ftp.filename);
    do{
        //memset(dbuf, 0, _MAX_SS);
        blocklen = sprintf(dbuf, "%s", ftp.filename);
        printf("########## dbuf:%s\r\n", dbuf);
        send(ftp.data_sock, (uint8_t*)dbuf, blocklen);
        remain_filesize -= blocklen;
    }while(remain_filesize != 0);
#endif
    disconnect(ftp.data_sock);
    slen = sprintf(sendbuf, "226 Successfully transferred \"%s\"\r\n", ftp.filename);
    send(ftp.ctrl_sock, (uint8_t*)sendbuf, slen);
    return 0;

}

int ftpd_storcmd(char* arg){
    char sendbuf[200], dbuf[_MAX_SS];
    uint16_t slen;
    uint16_t remain_datasize;
    uint8_t tmp;
    uint16_t recv_byte;
    uint16_t ret;
    uint32_t blocklen;

    if(strlen(ftp.workingdir) == 1)
       sprintf(ftp.filename, "/%s", arg);
    else
       sprintf(ftp.filename, "%s/%s", ftp.workingdir, arg);
    slen = sprintf(sendbuf, "150 Opening data channel for file upload to server of \"%s\"\r\n", ftp.filename);
    send(ftp.ctrl_sock, (uint8_t *)sendbuf, slen);

    getsockopt(ftp.data_sock, SO_STATUS, &tmp);
	if(tmp!=SOCK_ESTABLISHED)
	{
		ftpd_dsock_ready();
	}

#if defined(_FTP_DEBUG_)
    printf("filename to store : %s %d\r\n", ftp.filename, strlen(ftp.filename));
#endif
#if defined(F_FILESYSTEM)
	ftp.fr = f_open(&(ftp.fil), (const char *)ftp.filename, FA_CREATE_ALWAYS | FA_WRITE);
	//print_filedsc(&(ftp.fil));
	if(ftp.fr == FR_OK){
#if defined(_FTP_DEBUG_)
		printf("f_open return FR_OK\r\n");
#endif
		while(1){
			if((remain_datasize = getSn_RX_RSR(ftp.data_sock)) > 0){
				while(1){
					memset(dbuf, 0, _MAX_SS);

					if(remain_datasize > _MAX_SS)
						recv_byte = _MAX_SS;
					else
						recv_byte = remain_datasize;

					ret = recv(ftp.data_sock, dbuf, recv_byte);
#if defined(_FTP_DEBUG_)
					//printf("----->fn:%s data:%s \r\n", ftp.filename, dbuf);
#endif

					ftp.fr = f_write(&(ftp.fil), dbuf, (UINT)ret, &blocklen);
#if defined(_FTP_DEBUG_)
					//printf("----->dsize:%d recv:%d len:%d \r\n", remain_datasize, ret, blocklen);
#endif
					remain_datasize -= blocklen;

					if(ftp.fr != FR_OK){
#if defined(_FTP_DEBUG_)
						printf("f_write failed\r\n");
#endif
						break;
					}

					if(remain_datasize <= 0)
						break;
				}

				if(ftp.fr != FR_OK){
#if defined(_FTP_DEBUG_)
					printf("f_write failed\r\n");
#endif
					break;
				}

#if defined(_FTP_DEBUG_)
				printf("#");
#endif
			}else{
				if(getSn_SR(ftp.data_sock) != SOCK_ESTABLISHED)
					break;
			}
		}
#if defined(_FTP_DEBUG_)
		printf("\r\nFile write finished\r\n");
#endif
		ftp.fr = f_close(&(ftp.fil));
		}else{
#if defined(_FTP_DEBUG_)
			printf("File Open Error: %d\r\n", ftp.fr);
#endif
		}

    				//fno.fdate = (WORD)(((current_year - 1980) << 9) | (current_month << 5) | current_day);
    				//fno.ftime = (WORD)((current_hour << 11) | (current_min << 5) | (current_sec >> 1));
    				//f_utime((const char *)ftp.filename, &fno);
#else
    while(1){
        getsockopt(ftp.data_sock, SO_RECVBUF, &remain_datasize);
       if((remain_datasize) > 0){
           while(1){
               if(remain_datasize > _MAX_SS)
                   recv_byte = _MAX_SS;
               else
                   recv_byte = remain_datasize;
               ret = recv(ftp.data_sock, (uint8_t*)dbuf, recv_byte);
               dbuf[_MAX_SS]=0;
               printf("########## dbuf:%s\r\n", dbuf);
               remain_datasize -= ret;
               if(remain_datasize <= 0)
                   break;
           }
       }else{
           getsockopt(ftp.data_sock, SO_STATUS, &tmp);
           if(tmp != SOCK_ESTABLISHED)
               break;
       }
    }
#endif
    disconnect(ftp.data_sock);

    slen = sprintf(sendbuf, "226 Successfully transferred \"%s\"\r\n", ftp.filename);
    send(ftp.ctrl_sock, (uint8_t*)sendbuf, slen);
    return 0;
}

#if defined(F_FILESYSTEM)
void print_filedsc(FIL *fil)
{
#if defined(_FTP_DEBUG_)
	printf("File System pointer : %08X\r\n", fil->obj.fs);
	printf("File System mount ID : %d\r\n", fil->obj.id);
	printf("File status flag : %08X\r\n", fil->flag);
	printf("File System pads : %08X\r\n", fil->err);
	printf("File read write pointer : %08X\r\n", fil->fptr);
	printf("File size : %08X\r\n", fil->obj.objsize);
	printf("File start cluster : %08X\r\n", fil->obj.sclust);
	printf("current cluster : %08X\r\n", fil->clust);
	printf("current data sector : %08X\r\n", fil->sect);
	printf("dir entry sector : %08X\r\n", fil->dir_sect);
	printf("dir entry pointer : %08X\r\n", fil->dir_ptr);
#endif
}
#endif
