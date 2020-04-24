#define FUSE_USE_VERSION 28

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <wait.h>

static const char *dirPath = "/home/falcon/Documents";

//ada 87 isi base
char *base = {"9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO"};
const int key = 10;
char *en1 = "encv1_";
char *en2 = "encv2_";
int x=0;

void tulisLog(char *nama, char *fpath){
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	char haha[1000];

	FILE *file;
	file = fopen("/home/falcon/fs.log","a");
	if (file == NULL) file = fopen("/home/falcon/fs.log","w");

	if (strcmp(nama,"RMDIR")==0 || strcmp(nama,"UNLINK")==0) 
		sprintf(haha,"WARNING::%d%.2d%.2d-%.2d:%.2d:%.2d::%s::%s\n",timeinfo->tm_year-100,timeinfo->tm_mon+1, timeinfo->tm_mday,timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, nama, fpath);
	else sprintf(haha,"INFO::%d%.2d%.2d-%.2d:%.2d:%.2d::%s::%s\n",timeinfo->tm_year-100,timeinfo->tm_mon+1, timeinfo->tm_mday,timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, nama, fpath);

	fputs(haha,file);
	fclose(file);
	return;
}

void tulisLog2(char *nama, const char *from, const char *to){
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	char haha[1000];

	FILE *file;
	file = fopen("/home/falcon/fs.log","a");
	if (file == NULL) file = fopen("/home/falcon/fs.log","w");
	
	if (strcmp(nama,"RMDIR")==0 || strcmp(nama,"UNLINK")==0) 
		sprintf(haha,"WARNING::%d%.2d%.2d-%.2d:%.2d:%.2d::%s::%s::%s\n",timeinfo->tm_year-100,timeinfo->tm_mon+1, timeinfo->tm_mday,timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, nama,from,to);
	else sprintf(haha,"INFO::%d%.2d%.2d-%.2d:%.2d:%.2d::%s::%s::%s\n",timeinfo->tm_year-100,timeinfo->tm_mon+1, timeinfo->tm_mday,timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, nama,from,to);

	fputs(haha,file);
	fclose(file);
	return;
}

int ext_id(char* path){
	int i;

	for(i=strlen(path)-1; i >=0; i--){
		if(path[i] == '.'){
			return i;
		}
	}

	return strlen(path);
}

int slash_id(char* path, int mentok){
	int i;

	for(i=0; i<strlen(path); i++){
		if(path[i] == '/'){
			return i + 1;
		}
	}

	return mentok;
}

void encrypt1(char *path)		// ke kanan
{
	if (!strcmp(path, ".") || !strcmp(path, ".."))
		return;

	printf("encrypt path: %s\n", path);

	int temp = 0, i, j;

	int endid = ext_id(path);
	int startid = slash_id(path, 0);

	for (i = startid; i < endid; i++)
	{
		if(path[i] != '/'){
			for (j = 0; j < strlen(base); j++)
			{
				if (path[i] == base[j])
				{
					temp = (j + key) % strlen(base); //somehow this reverses?
					path[i] = base[temp];
					break;
				}
			}
		}
	}
}

void decrypt1(char *path)			// ke kiri
{
	if(!strcmp(path, ".") || !strcmp(path, ".."))
		return;

	printf("decrypt path: %s\n", path);

	int temp = 0, i, j;

	int endid = ext_id(path);
	int startid = slash_id(path, endid);

	for (i = startid; i < endid; i++)
	{
		if(path[i] != '/'){
			for (j = 0; j < strlen(base); j++)
			{
				if (path[i] == base[j])
				{
					temp = (j + (strlen(base) - key)) % strlen(base);
					path[i] = base[temp];
					break;
				}
			}
		}
	}
}

void enkripsi2(char *fpath){
	// FILE *file;
	// file = fopen("/home/insanity/fs.log","a");
	// if (file == NULL) file = fopen("/home/insanity/fs.log","w");
	// fputs("masuk1\n",file);

	chdir(fpath);
	DIR *d;
    struct dirent *dir;
	struct stat lol;
    d = opendir(".");
    if (d){
        while ((dir = readdir(d)) != NULL){
			if (stat(dir->d_name, &lol) < 0);
			else if (S_ISDIR(lol.st_mode)){
				if (strcmp(dir->d_name,".")==0 || strcmp(dir->d_name,"..")==0) continue;
				char ffpath[1000];
				sprintf(ffpath,"%s/%s",fpath, dir->d_name);
				enkripsi2(ffpath);
			}
			else{
				char fffpath[1000];
				sprintf(fffpath,"%s/%s",fpath, dir->d_name);
				char ffpath[1000];
				sprintf(ffpath,"%s.",fffpath);
				pid_t child_id;
				child_id = fork();
				int status;
				if (child_id == 0){
					pid_t lmao = fork();
					if (lmao ==0){
						char* argv[] = {"split","-b","1024","-a","3","-d",fffpath,ffpath, NULL};
						execv("/usr/bin/split",argv);
					}
					else {
						while ((wait(&status))>0);
						char *argv[]={"rm",fffpath, NULL};
						execv("/bin/rm",argv);
					}	
				}
				// enkripsi22(fffpath);
			}
            // printf("%s\n", dir->d_name);
		}
        // closedir(d);
    }
	return;
}

void tulis(char *src, char *dst){
	FILE *fp1, *fp2;
    char ch;
    int pos;
    if ((fp1 = fopen(src,"r")) == NULL){    
        printf("\nFile cannot be opened");
        return;
	}
    else{
        printf("\nFile opened for copy...\n ");    
    }
    fp2 = fopen(dst, "a");  
    fseek(fp1, 0L, SEEK_END); // file pointer at end of file
    pos = ftell(fp1);
    fseek(fp1, 0L, SEEK_SET); // file pointer set at start
    while (pos--){
        ch = fgetc(fp1);  // copying file character by character
        fputc(ch, fp2);
    }    
    fclose(fp1);
	fclose(fp2);
	remove(src);
}

void dekripsi2(char *fpath){
	// FILE *file;
	// file = fopen("/home/insanity/fs.log","a");
	// if (file == NULL) file = fopen("/home/insanity/fs.log","w");

	chdir(fpath);
	DIR *d;
    struct dirent *dir;
	struct stat lol;
    d = opendir(".");
    if (d){
        while ((dir = readdir(d)) != NULL){
			if (stat(dir->d_name, &lol) < 0);
			else if (S_ISDIR(lol.st_mode)){
				if (strcmp(dir->d_name,".")==0 || strcmp(dir->d_name,"..")==0) continue;
				char ffpath[1000];
				sprintf(ffpath,"%s/%s",fpath, dir->d_name);
				dekripsi2(ffpath);
			}
			else{
				// pid_t child_id = fork();

				char fffpath[1000];
				sprintf(fffpath,"%s/%s",fpath, dir->d_name);
				if (strstr(fffpath,".") != NULL){
					int len = strlen(fffpath);
					fffpath[len-4] = '\0';
					int x=0;
					FILE *file;
					file = fopen(fffpath,"w");
					fclose (file);

					// file = fopen(ffpath,"a");
					while (1){
						char ffpath[1000];
						sprintf(ffpath, "%s.%.03d", fffpath,x);

						struct stat tes;
						if (stat(ffpath, &tes)!=0) break;
						tulis(fffpath,ffpath);
						// pid_t child_id = fork();
						// if (child_id == 0){
						// 	char *argv[]={"rm",ffpath,}
						// }
						// fclose(file);
						x++;
					}
				}
				
				// int x=0;
				// FILE *file2;
				// file2 = fopen(fffpath,"w");
				// fclose (file2);
				// sprintf(ffpath,"%s.%.03d",fffpath,x);		
				// FILE *file;
				// file = fopen(ffpath,"a");
				// while (file!=NULL){
				// 	tulis(fffpath,ffpath);
				// 	fclose(file);
				// 	x++;
				// 	sprintf(ffpath,"%s.%.03d",fffpath,x);
				// 	file = fopen(ffpath,"a");
				// }
			}
		}
    }
	return;
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
	int res;
	char fpath[1000];

	char *a = strstr(path, en1);
	if(a != NULL)
		decrypt1(a);

	if(strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	// printf("getattr fpath: %s\n", fpath);

	res = lstat(fpath, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
					   off_t offset, struct fuse_file_info *fi)
{
	char fpath[1000];

	char *a = strstr(path, en1);
	if(a != NULL)
		decrypt1(a);

	if(strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	if (x!=24)
		x++;
	else
		tulisLog("READDIR",fpath);

	// printf("readdir path: %s\n", path);

	int res = 0;
	DIR *dp;
	struct dirent *de;

	(void)offset;
	(void)fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	// if path contains encv1_ or encv2_, encrypt content

	while ((de = readdir(dp)) != NULL) //read whats inside dirpath (level 1 only)
	{								   // de->d_name is file name + ext
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;

		printf("before: %s\n", de->d_name);
		printf("a: %s\n", a);
		if(a != NULL)
			encrypt1(de->d_name);
		printf("after: %s\n", de->d_name);

		res = (filler(buf, de->d_name, &st, 0));

		if (res != 0)
			break;
	}

	closedir(dp);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
					struct fuse_file_info *fi)
{
	char fpath[1000];
	char *a = strstr(path, en1);
	if(a != NULL)
		decrypt1(a);

	if(strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	// printf here doesn't do anything

	int res = 0;
	int fd = 0;

	(void)fi;

	fd = open(fpath, O_RDONLY);

	tulisLog("READ",fpath);

	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
	int res;
	char fpath[1000];

	char *a = strstr(path, en1);

	if(a != NULL){
		decrypt1(a);
	}

	if(strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = mkdir(fpath, mode);

	tulisLog("MKDIR",fpath);

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rmdir(const char *path)
{
	int res;
	char fpath[1000];
	char *a = strstr(path, en1);
	if(a != NULL)
		decrypt1(a);

	if(strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = rmdir(fpath);

	tulisLog("RMDIR",fpath);

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rename(const char *from, const char *to)
{
	int res;
	char frompath[1000], topath[1000];

	char *a = strstr(to, en1);

	if(a != NULL)
		decrypt1(a);

	sprintf(frompath, "%s%s", dirPath, from);
	sprintf(topath, "%s%s", dirPath, to);

	res = rename(frompath, topath);
	if (res == -1)
		return -errno;

	tulisLog2("RENAME",frompath, topath);

	if (strstr(to,en2) != NULL){
		enkripsi2(topath);
		tulisLog2("ENCRYPT2",from,to);
	}
	if (strstr (from, en2)!= NULL && strstr(to, en2)==NULL){
		dekripsi2(topath);
		tulisLog2("DECRYPT2",from,to);
	}

	return 0;
}

static int xmp_truncate(const char *path, off_t size)
{
	int res;
	char fpath[1000];
	char *a = strstr(path, en1);
	if(a != NULL)
		decrypt1(a);

	if(strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	tulisLog("TRUNC",fpath);

	res = truncate(fpath, size);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_write(const char *path, const char *buf, size_t size,
					 off_t offset, struct fuse_file_info *fi)
{
	int fd;
	int res;
	char fpath[1000];
	char *a = strstr(path, en1);
	if(a != NULL)
		decrypt1(a);

	if(strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	(void)fi;

	// sprintf(fpath, "/home/falcon/Documents/modul4/fuseex/encv1_a");

	fd = open(fpath, O_WRONLY);

	tulisLog("WRITE",fpath);

	if (fd == -1)
		return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);

	return res;
}

static int xmp_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
	(void)fi;

	int res;
	char fpath[1000];

	char *a = strstr(path, en1);
	if(a != NULL)
		decrypt1(a);

	if(strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = creat(fpath, mode);

	tulisLog("CREATE",fpath);

	if (res == -1)
		return -errno;

	close(res);

	return 0;
}

static int xmp_utimens(const char *path, const struct timespec ts[2])
{
	int res;
	char fpath[1000];

	char *a = strstr(path, en1);
	if(a != NULL)
		decrypt1(a);

	if(strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	struct timeval tv[2];

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;

	res = utimes(fpath, tv);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_access(const char *path, int mask)
{
	int res;
	char fpath[1000];

	char *a = strstr(path, en1);
	if(a != NULL)
		decrypt1(a);

	if(strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = access(fpath, mask);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi)
{
	int res;
	char fpath[1000];
	char *a = strstr(path, en1);
	if(a != NULL)
		decrypt1(a);

	if(strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = open(fpath, fi->flags);

	tulisLog("OPEN",fpath);

	if (res == -1)
		return -errno;

	close(res);
	return 0;
}

static int xmp_unlink(const char *path) {
	int res;
	char fpath[1000];
	char *a = strstr(path, en1);
	if(a != NULL)
		decrypt1(a);

	if(strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = unlink(fpath);

	tulisLog("UNLINK",fpath);

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readlink(const char *path, char *buf, size_t size)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, en1);
	if(a != NULL)
		decrypt1(a);

	if(strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = readlink(fpath, buf, size - 1);
	tulisLog("READLINK",fpath);
	if (res == -1)
		return -errno;

	buf[res] = '\0';
	return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
	int res;
	char fpath[1000];
	char *a = strstr(path, en1);
	if(a != NULL)
		decrypt1(a);

	if(strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	tulisLog("MKNOD",fpath);
	/* On Linux this could just be 'mknod(path, mode, rdev)' but this
	   is more portable */
	if (S_ISREG(mode)) {
		res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(fpath, mode);
	else
		res = mknod(fpath, mode, rdev);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_symlink(const char *from, const char *to)
{
	int res;
	char frompath[1000], topath[1000];

	char *a = strstr(to, en1);

	if(a != NULL)
		decrypt1(a);

	sprintf(frompath, "%s%s", dirPath, from);
	sprintf(topath, "%s%s", dirPath, to);
	
	res = symlink(frompath, topath);
	tulisLog2("SYMLINK",frompath, topath);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_link(const char *from, const char *to)
{
	int res;
	char frompath[1000], topath[1000];

	char *a = strstr(to, en1);

	if(a != NULL)
		decrypt1(a);

	sprintf(frompath, "%s%s", dirPath, from);
	sprintf(topath, "%s%s", dirPath, to);

	res = link(frompath, topath);

	tulisLog2("LINK",frompath, topath);

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chmod(const char *path, mode_t mode)
{
	int res;
	char fpath[1000];
	char *a = strstr(path, en1);
	if(a != NULL)
		decrypt1(a);

	if(strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = chmod(fpath, mode);

	tulisLog("CHMOD",fpath);

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid)
{
	int res;
	char fpath[1000];
	char *a = strstr(path, en1);
	if(a != NULL)
		decrypt1(a);

	if(strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = lchown(fpath, uid, gid);
	tulisLog("CHOWN",fpath);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_statfs(const char *path, struct statvfs *stbuf)
{
	int res;
	char fpath[1000];
	char *a = strstr(path, en1);
	if(a != NULL)
		decrypt1(a);

	if(strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = statvfs(fpath, stbuf);
	tulisLog("STATFS",fpath);
	if (res == -1)
		return -errno;

	return 0;
}

static struct fuse_operations xmp_oper = {
	.getattr = xmp_getattr,
	.readdir = xmp_readdir,
	.read = xmp_read,
	.mkdir = xmp_mkdir,
	.rmdir = xmp_rmdir,
	.rename = xmp_rename,
	.truncate = xmp_truncate,
	.write = xmp_write,
	.create = xmp_create,
	.utimens = xmp_utimens,
	.access = xmp_access,
	.open = xmp_open,
	.unlink = xmp_unlink,
	.readlink = xmp_readlink,
	.mknod = xmp_mknod,
	.symlink = xmp_symlink,
	.link = xmp_link,
	.chmod = xmp_chmod,
	.chown = xmp_chown,
	.statfs = xmp_statfs,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
