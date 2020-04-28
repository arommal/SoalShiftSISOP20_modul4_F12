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
// char base[] = {'9','(','k','u','@','A','W','1','[','L','m','v','g','a','x','6','q','`','5','Y','2','R','y','?','+','s','F','!','^','H','K','Q','i','B','X','C','U','S','e','&','0','M','.','b','%','r','I','\'','7','d',')','o','4','~','V','f','Z','*','{','#',':','}','E','T','t','$','3','J','-','z','p','c',']','l','n','h','8',',','G','w','P','_','N','D','|','j','O','\0'};
char *base = {"9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO"};
const int key = 10;
char en1[10] = "encv1_";
// char *en2 = "encv2";

int ext_id(char* path){
	int i;

	for(i=strlen(path)-1; i >=0; i--){
		if(path[i] == '.'){
			return i;
		}
	}

	return strlen(path);
}

int slash_id(char* path){
	int i;

	for(i=0; i<strlen(path); i++){
		if(path[i] == '/'){
			return i + 1;
		}
	}

	return 0;
}

void encrypt1(char *path)		// ke kiri
{
	if (!strcmp(path, ".") || !strcmp(path, ".."))
		return;

	int temp = 0, i, j;
	// char tempPath[1000];
	// sprintf(tempPath, "%s", path);
	
	int startid = 0;
	int endid = strlen(path);

	for(i=strlen(path)-1; i >=0; i--){
		if(path[i] == '.'){
			endid = i;
			break;
		}
	}

	for(i=0; i<endid; i++){
		if(path[i] == '/'){
			startid = i + 1;
			break;
		}
	}

	for (i = startid; i < endid; i++)
	{
		if(path[i] != '/'){
			for (j = 0; j < strlen(base); j++)
			{
				if (path[i] == base[j])
				{
					temp = (j + key) % 87; //somehow this reverses?
					path[i] = base[temp];
					break;
				}
			}
		}
	}
}

void decrypt1(char *path)			// ke kanan
{
	if(!strcmp(path, ".") || !strcmp(path, ".."))
		return;

	int temp = 0, i, j;
	// char tempPath[1000];
	// sprintf(tempPath, "%s", path);
	
	// int startid = 0;
	int endid = strlen(path);

	for(i=strlen(path)-1; i >=0; i--){
		if(path[i] == '.'){
			endid = i;
			break;
		}
	}

	int startid = endid;

	for(i=0; i<endid; i++){
		if(path[i] == '/'){
			startid = i + 1;
			break;
		}
	}

	for (i = startid; i < endid; i++)
	{
		if(path[i] != '/'){
			for (j = 0; j < strlen(base); j++)
			{
				if (path[i] == base[j])
				{
					temp = (j + (87 - key)) % 87;
					path[i] = base[temp];
					break;
				}
			}
		}
	}
}

// int versi1(char *path, int status){
// 	DIR *dp;
// 	struct dirent *de;

// 	dp = opendir(path);
// 	if (dp == NULL)
// 		return -errno;

// 	while((de = readdir(dp)) != NULL){
// 		struct stat st;
// 		memset(&st, 0, sizeof(st));

// 		char name[1000], fpath[1000];
// 		sprintf(name, "%s", de->d_name);

// 		sprintf(fpath, "%s/%s", path, name);

// 		if(strcmp(path, ".") || strcmp(path, "..")){
// 			char folder[1000],
// 		}
// 	}
// }

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
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rmdir(const char *path)
{
	int res;
	char fpath[1000];
	// char *a = strstr(path, en1);
	// if(a != NULL)
	// 	decrypt1(a);

	if(strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = rmdir(fpath);
	if (res == -1)
		return -errno;

	return 0;
}

// static int xmp_chmod(const char *path, mode_t mode) {
// 	int res;
// 	char fpath[1000], enPath[1000];

// 	encrypt1((char*)path, enPath);

// 	if(strcmp(path, "/") == 0){
// 		path = dirPath;
// 		sprintf(fpath, "%s", path);
// 	}else{
// 		sprintf(fpath, "%s%s", dirPath, enPath);
// 	}

// 	res = chmod(fpath, mode);
// 	if (res == -1)
// 		return -errno;

// 	return 0;
// }

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
	if (res == -1)
		return -errno;

	close(res);
	return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev) {
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

	/* On Linux this could just be 'mknod(path, mode, rdev)' but this
	   is more portable */
	if (S_ISREG(mode)) {
		res = open(path, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(path, mode);
	else
		res = mknod(path, mode, rdev);

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
	.mknod = xmp_mknod,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
