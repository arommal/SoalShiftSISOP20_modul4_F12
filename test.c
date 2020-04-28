#include <string.h>
#include <stdio.h>

int main () {
   char str[80] = "/home/falcon/Documents/modul4/abc.c";
   const char s[10] = "/";
   char *token;
   char sliced[1000] = {'\0'};
   
   /* get the first token */
   token = strtok(str, s);
   
   /* walk through other tokens */
   while( (token) != NULL ) {
      printf( "%s\n", token );
      sprintf(sliced, "%s/%s", sliced, token);
      // printf("%d\n", (int)strlen(token));
      printf("%s\n", sliced);
    
      token = strtok(NULL, "/");
   }
   
   return(0);
}
// 
// void checkSynchronize(char *fpath){
	// char temp[1000], syncdir[1000], sliced[1000] = {'\0'};
	// char *slicedDirs[1000];
	// char *token;
	// int i = 0;
// 
	// sprintf(temp, "%s", fpath);
// 
	// token = strtok(temp, "/");
// 
	// while(token != NULL){
		// sprintf(sliced, "%s/%s", sliced, token);
		// slicedDirs[i++] = sliced;
		// token = strtok(NULL, "/");
	// }
// 
	// if(i < 3)
		// return;
// 
	// while(i>2){
		// DIR *dir;
		// struct dirent *de;
		// struct dirent *dp;
// 
		// dir = opendir(slicedDirs[i]);
// 
		// if (dir != NULL){
			// while((de = readdir(dir)) != NULL){
				// if(de->d_type == DT_DIR){
					// sprintf(syncdir, "%s%s", syncs, de->d_name);
// 
					// while((dp = readdir(dir)) != NULL){
						// if(strcmp(dp->d_name, syncdir) && dp->d_type == DT_DIR){
							// char dir1[1000], dir2[1000];
							// sprintf(dir1, "%s/%s", slicedDirs[i], de->d_name);
							// sprintf(dir2, "%s/%s", slicedDirs[i], dp->d_name);
// 
							// autoSynchronize(dir1, dir2);
						// }
					// }
				// }
			// }
		// }
		// i--;
	// }
// 
// }