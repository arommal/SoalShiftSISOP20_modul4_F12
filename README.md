# SoalShiftSISOP20_modul4_F12

Di suatu perusahaan, terdapat pekerja baru yang super jenius, ia bernama Jasir. Jasir baru bekerja selama seminggu di perusahan itu, dalam waktu seminggu tersebut ia selalu terhantui oleh ketidakamanan dan ketidakefisienan file system yang digunakan perusahaan tersebut. Sehingga ia merancang sebuah file system yang sangat aman dan efisien. 

Pada segi keamanan, Jasir telah menemukan 2 buah metode enkripsi file. Pada mode enkripsi pertama, nama file-file pada direktori terenkripsi akan dienkripsi menggunakan metode substitusi. Sedangkan pada metode enkripsi yang kedua, file-file pada direktori terenkripsi akan di-split menjadi file-file kecil sehingga orang-orang yang tidak menggunakan filesystem rancangannya akan kebingungan saat mengakses direktori terenkripsi tersebut.

Untuk segi efisiensi, dikarenakan pada perusahaan tersebut sering dilaksanakan sinkronisasi antara 2 direktori, maka jasir telah merumuskan sebuah metode agar filesystem-nya mampu mengsingkronkan kedua direktori tersebut secara otomatis. 

Agar integritas filesystem tersebut lebih terjamin, maka setiap command yang dilakukan akan dicatat kedalam sebuah file log.
(catatan: filesystem berfungsi normal layaknya linux pada umumnya)
(catatan: mount source (root) filesystem adalah direktori /home/[user]/Documents)

## Soal 1: Enkripsi versi 1
1. Jika sebuah direktori dibuat dengan awalan `encv1_`, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v1.
2. Jika sebuah direktori di-rename dengan awalan `encv1_`, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v1.
3. Apabila sebuah direktori terenkripsi di-rename menjadi tidak terenkripsi, maka isi direktori tersebut akan terdekripsi.
4. Setiap pembuatan direktori terenkripsi baru (`mkdir` ataupun `rename`) akan tercatat ke sebuah database/log berupa file.
5. Semua file yang berada dalam direktori ter enkripsi menggunakan caesar cipher dengan key.
```9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO```

Misalkan ada file bernama “kelincilucu.jpg” dalam directory FOTO_PENTING, dan key yang dipakai adalah 10, maka:
> “encv1_rahasia/FOTO_PENTING/kelincilucu.jpg” => “encv1_rahasia/ULlL@u]AlZA(/g7D.|_.Da_a.jpg

Note : Dalam penamaan file, ‘/’ diabaikan, dan ekstensi tidak perlu di-encrypt.
6. Metode enkripsi pada suatu direktori juga berlaku ke dalam direktori lainnya yang ada di dalamnya.

- ### Penyelesaian
Jika folder atau file terenkripsi di FUSE, maka di mount folder akan terdekripsi, begitu juga sebaliknya. Maka, terdapat fungsi enkripsi dan dekripsi. Di masing-masing fungsi tersebut kita akan melakukan looping untuk mengubah nama folder atau file. Maka kita perlu tahu batas perulangannya (dimulai dari awal nama folder/file dan berakhir sebelum file extension). Maka kita butuh fungsi `ext_id` dan `slash_id` yang akan mengembalikan index file extension dan index slash sebagai penanda awal dan akhir enkripsi dan dekripsi.

- Fungsi ext_id:

    ```C
    int ext_id(char *path)
    {
        int i;

        for (i = strlen(path) - 1; i >= 0; i--){
            if (path[i] == '.')
                return i;
        }

        return strlen(path);
    }
    ```

- Fungsi slash_id:

    ```C
    int slash_id(char *path, int mentok)
    {
        int i;

        for (i = 0; i < strlen(path); i++)
        {
            if (path[i] == '/')
                return i + 1;
        }

        return mentok;
    }
    ```
    Variabel mentok akan dikembalikan oleh fungsi ini jika `path` bukanlah 

- Fungsi enkripsi:
    
    ```C
    void encrypt1(char *path)
    {
        int temp = 0, i, j;

        int endid = ext_id(path);
        int startid = slash_id(path, 0);

        for (i = startid; i < endid; i++){
            if (path[i] != '/'){
                for (j = 0; j < strlen(base); j++){
                    if (path[i] == base[j]){
                        temp = (j + key) % strlen(base); //somehow this reverses?
                        path[i] = base[temp];
                        break;
                    }
                }
            }
        }
    }
    ```

- Fungsi dekripsi:

    ```C
    void decrypt1(char *path)
    {
        int temp = 0, i, j;

        int endid = ext_id(path);
        int startid = slash_id(path, endid);

        for (i = startid; i < endid; i++){
            if (path[i] != '/'){
                for (j = 0; j < strlen(base); j++){
                    if (path[i] == base[j]){
                        temp = (j + (strlen(base) - key)) % strlen(base);
                        path[i] = base[temp];
                        break;
                    }
                }
            }
        }
    }
    ```

Pemanggilan fungsi dekripsi dilakukan di utility functions `getattr`, `mkdir`, `rename`, `rmdir`, `create`, dan fungsi-fungsi lain yang menurut kami esensial dalam proses sinkronisasi FUSE dan mount folder. Fungsi dekripsi dan enkripsi dilakukan di utility function `readdir` karena FUSE akan melakukan dekripsi di mount folder lalu enkripsi di FUSE saat readdir. Pemanggilannya dilakukan dengan pengecekan apakah string `"encv1_"` terdapat di string path di masing-masing utility function dengan menggunakan fungsi `strstr()`. Jika ya, maka fungsi enkripsi dan dekripsi akan dipanggil untuk string tersebut dengan `"encv1_"` sebagai starting point string yang diteruskan.

***

## Soal 2: Enkripsi versi 2
1. Jika sebuah direktori dibuat dengan awalan “encv2_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v2.
2. Jika sebuah direktori di-rename dengan awalan “encv2_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v2.
3. Apabila sebuah direktori terenkripsi di-rename menjadi tidak terenkripsi, maka isi direktori tersebut akan terdekripsi.
4. Setiap pembuatan direktori terenkripsi baru (mkdir ataupun rename) akan tercatat ke sebuah database/log berupa file.
5. Pada enkripsi v2, file-file pada direktori asli akan menjadi bagian-bagian kecil sebesar 1024 bytes dan menjadi normal ketika diakses melalui filesystem rancangan jasir. Sebagai contoh, file File_Contoh.txt berukuran 5 kB pada direktori asli akan menjadi 5 file kecil yakni: File_Contoh.txt.000, File_Contoh.txt.001, File_Contoh.txt.002, File_Contoh.txt.003, dan File_Contoh.txt.004.
6. Metode enkripsi pada suatu direktori juga berlaku kedalam direktori lain yang ada di dalam direktori tersebut (rekursif).

- ### Penyelesaian

Pertama, kita melakukan pengecekkan pada nama folder ketika di rename, sehingga pada fungsi rename kita menggunakan fungsi strstr dengan implementasi
```c
	if (strstr(to, en2) != NULL){
		enkripsi2(topath);
		tulisLog2("ENCRYPT2", from, to);
	}

	if (strstr(from, en2) != NULL && strstr(to, en2) == NULL){
		dekripsi2(topath);
		tulisLog2("DECRYPT2", from, to);
	}
```
Disini kita memanggil fungsi ``enkripsi2`` ketika penamaan folder yang sebelumnya tidak mengandung "encv2_" ke string yang mengandung "encv2_". Ketika yang terjadi adalah sebaliknya, maka kita memanggil fungsi ``dekripsi2``. 

Di fungsi ``enkripsi2`` sendiri, kita mengecek direktori yang telah kita pass sebelumnya (path direktori yang telah direname) dan kita mengecek file didalamnya satu persatu, jika ternyata file yang sedang dicek adalah sebuah direktori, maka kita akan secara rekursi memanggil fungsi enkripsi2 kembali dengan path yang telah diupdate, impleementasi ketika menemukan folder adalah
```c
            else if (S_ISDIR(lol.st_mode)){
				if (strcmp(dir->d_name,".")==0 || strcmp(dir->d_name,"..")==0) continue;
				char ffpath[1000];
				sprintf(ffpath,"%s/%s",fpath, dir->d_name);
				enkripsi2(ffpath);
            }
```
Sedangkan ketika kita menemukan file yang bukan direktori, maka kita melakukan pembagian menjadi beberapa file dengan menggunakan fungsi split, untuk implementasinya, kami menggunakan fork, exec, dan wait. Setelah di split, kita akan meremove filenya. Implementasinya adlaah sebagai berikut
```c
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
					}else {
						while ((wait(&status))>0);
						char *argv[]={"rm",fffpath, NULL};
						execv("/bin/rm",argv);
					}	
				}
```

Berikutnya adlaah fungsi dekripsi2, sama seperti enkripsi2, kita membagi kasus ketika kita menemukan folder dan ketika menemukan file. Ketika menemukan folder, implementasinya adalah sama seperti enkripsi2 yang telah dijelaskan diatas yaitu 
```c
        if (S_ISDIR(lol.st_mode)){
			sprintf(dirPath, "%s/%s", dir, de->d_name);
			dekripsi2(dirPath);
        }
```
Sedangkan ketika menemukan file, kita melakukan pembuatan file yang tanpa ekstensi yang telah kita tambahkan saat mengenkrip, dan kemudian kita menggunakan count untuk mengecek file tersebut dari 000 hingga akhir, jika ada kita copy isi filenya ke file yang telah kita buat sbelumnya dan ketika tidak ada file yang ditemukan, maka kita break. 
```c
        else if (!S_ISDIR(lol.st_mode)){
			sprintf(filePath, "%s/%s", dir, de->d_name);
			filePath[strlen(filePath) - 4] = '\0';
			// decription2(filePath);
			FILE *check = fopen(filePath, "r");
			if (check != NULL)
				return;
			FILE *file = fopen(filePath, "w");
			int count = 0;
			char topath[1000];
			sprintf(topath, "%s.%03d", filePath, count);
			void *buffer = malloc(1024);
			while (1){
				FILE *op = fopen(topath, "rb");
				if (op == NULL)
					break;
				size_t readSize = fread(buffer, 1, 1024, op);
				fwrite(buffer, 1, readSize, file);
				fclose(op);
				remove(topath);
				count++;
				sprintf(topath, "%s.%03d", filePath, count);
			}
```

***

## Soal 3: Sinkronisasi direktori otomatis
Tanpa mengurangi keumuman, misalkan suatu directory bernama dir akan tersinkronisasi dengan directory yang memiliki nama yang sama dengan awalan sync_ yaitu sync_dir. Persyaratan untuk sinkronisasi yaitu:
1. Kedua directory memiliki parent directory yang sama.
2. Kedua directory kosong atau memiliki isi yang sama. Dua directory dapat dikatakan memiliki isi yang sama jika memenuhi:
⋅⋅* Nama dari setiap berkas di dalamnya sama.
⋅⋅* Modified time dari setiap berkas di dalamnya tidak berselisih lebih dari 0.1 detik.
3. Sinkronisasi dilakukan ke seluruh isi dari kedua directory tersebut, tidak hanya di satu child directory saja.
4. Sinkronisasi mencakup pembuatan berkas/directory, penghapusan berkas/directory, dan pengubahan berkas/directory. Jika persyaratan di atas terlanggar, maka kedua directory tersebut tidak akan tersinkronisasi lagi.

Implementasi dilarang menggunakan symbolic links dan thread.

- ### Penyelesaian

Implementasi sinkronisasi sejauh ini yang telah kami lakukan adalah saat terbuatnya folder baru yang namanya berisi awalan "sync_". Jika terdeteksi demikian, maka konten dari folder pasangannya (folder dengan nama yang sama namun tanpa awalan "sync_") akan di-copy ke folder "sync_" tersebut. Pendeteksian apabila nama folder berisi "sync_" atau tidak dilakukan di fungsi `readdir()` dan `mkdir()` dengan fungsi berikut:

	```C
	char *checkSynchronize(char *foldername, char *syncpath)
	{	
		if(strstr(foldername, syncs) == NULL){
			// if foldername is real folder
			sprintf(syncpath, "%s/%s%s", dirPath, syncs, foldername);

			// check if syncpath exists
			DIR *dir;
			dir = opendir(syncpath);

			if(dir == NULL)
				return NULL;
			else
				return syncpath;
			closedir(dir);
		}else{
			// if foldername is sync folder
			char real[1000];
			int i, j = 0;
			for(i=strlen(syncs)-1; i<strlen(foldername); i++){
				real[j] = foldername[i];
				j++;
			}
			real[j] = '\0';

			sprintf(syncpath, "%s/%s", dirPath, real);

			DIR *dir;
			dir = opendir(syncpath);

			if(dir == NULL)
				return NULL;
			else
				return syncpath;
			closedir(dir);
		}
	}
	```
Fungsi di atas akan mengembalikan direktori folder "sync_" jika yang di-passing ke fungsinya adalah folder asli. Jika yang di-passing ke fungsinya adalah folder "sync_", maka yang dikembalikan adalah direktori folder asli. Jika tidak keduanya, maka NULL akan dikembalikan.

Kemudian, jika selain value NULL yang dikembalikan maka akan dijalankan fungsi `syncContents()`:

	```C
	void syncContents(char *syncpath, char *realpath)
	{
		char buffer[4096];

		DIR *dir;
		FILE *realfile, *syncfile;
		struct dirent *dp;

		dir = opendir(realpath);

		while((dp = readdir(dir)) != NULL){
			char srcP[1000], destP[1000];
			sprintf(srcP, "%s/%s", realpath, dp->d_name);
			sprintf(destP, "%s/%s", syncpath, dp->d_name);

			realfile = fopen(srcP, "r");
			syncfile = fopen(destP, "w");

			size_t num;

			while((num = fread(buffer, 1, sizeof(buffer), realfile)) > 0)
				fwrite(buffer, 1, num, syncfile);
		}
		closedir(dir);
	}
	```
Fungsi di atas akan memberi permission read ke file descriptor untuk file asli, dan permission write ke file descriptor untuk file "sync_". Lalu akan dilakukan looping untuk write data file asli ke file "sync_" dengan menggunakan buffer untuk menampung data tersebut.

***


## Soal 4: Log system
1. Sebuah berkas nantinya akan terbentuk bernama "fs.log" di direktori *home* pengguna (/home/[user]/fs.log) yang berguna menyimpan daftar perintah system call yang telah dijalankan.
2. Agar nantinya pencatatan lebih rapi dan terstruktur, log akan dibagi menjadi beberapa level yaitu INFO dan WARNING.

3. Untuk log level WARNING, merupakan pencatatan log untuk syscall rmdir dan unlink.
4. Sisanya, akan dicatat dengan level INFO.
5. Format untuk logging yaitu:
    ```
    [LEVEL]::[yy][mm][dd]-[HH]:[MM]:[SS]::[CMD]::[DESC ...]
    LEVEL : Level logging
    yy : Tahun dua digit
    mm : Bulan dua digit
    dd : Hari dua digit
    HH : Jam dua digit
    MM : Menit dua digit
    SS : Detik dua digit
    CMD : System call yang terpanggil
    DESC : Deskripsi tambahan (bisa lebih dari satu, dipisahkan dengan ::)
    ```
    
- ### Penyelesaian
Kami membuat 2 fungsi tambahan yaitu ``tulislog`` dan ``tulislog2``, dimana fungsi tulislog adalah fungsi yang menghandle ketika perintha yang dilakukan hanya membutuhkan 1 buah deskripsi, seperti rmdir, mkdir, ls. Sedangkan fungsi tulislog2 adalah fungsi yang menghandle ketika perintah yang dilakukan membutuhkan 2 buah deskripsi seperti rename, mv. Implementasinya adlaah menggunakan local time, yang kemudian kita menulis kedalam log sesuai perintah soal. 
```c
void tulisLog(char *nama, char *fpath)
{
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	char haha[1000];

	FILE *file;
	file = fopen("/home/falcon/fs.log", "a");
	if (file == NULL)
		file = fopen("/home/falcon/fs.log", "w");

	if (strcmp(nama, "RMDIR") == 0 || strcmp(nama, "UNLINK") == 0)
		sprintf(haha, "WARNING::%d%.2d%.2d-%.2d:%.2d:%.2d::%s::%s\n", timeinfo->tm_year - 100, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, nama, fpath);
	else
		sprintf(haha, "INFO::%d%.2d%.2d-%.2d:%.2d:%.2d::%s::%s\n", timeinfo->tm_year - 100, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, nama, fpath);

	fputs(haha, file);
	fclose(file);
	return;
}

void tulisLog2(char *nama, const char *from, const char *to)
{
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	char haha[1000];

	FILE *file;
	file = fopen("/home/falcon/fs.log", "a");
	if (file == NULL)
		file = fopen("/home/falcon/fs.log", "w");

	if (strcmp(nama, "RMDIR") == 0 || strcmp(nama, "UNLINK") == 0)
		sprintf(haha, "WARNING::%d%.2d%.2d-%.2d:%.2d:%.2d::%s::%s::%s\n", timeinfo->tm_year - 100, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, nama, from, to);
	else
		sprintf(haha, "INFO::%d%.2d%.2d-%.2d:%.2d:%.2d::%s::%s::%s\n", timeinfo->tm_year - 100, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, nama, from, to);

	fputs(haha, file);
	fclose(file);
	return;
}
```
Kemudian, kita akan memanggil kedua fungsi ini, di setiap fungsi yang ada dalam fuse sesuai dengan perintahnya. Sebagai contoh ini adalah fungsi rename dan fungsi rmdir.
```
static int xmp_rename(const char *from, const char *to)
{
	int res;
	char frompath[1000], topath[1000];

	char *a = strstr(to, en1);

	if (a != NULL)
		decrypt1(a);

	sprintf(frompath, "%s%s", dirPath, from);
	sprintf(topath, "%s%s", dirPath, to);

	res = rename(frompath, topath);
	if (res == -1)
		return -errno;

	tulisLog2("RENAME", frompath, topath);                          // Pemanggilan fungsi tulislog2

	if (strstr(to, en2) != NULL){
		enkripsi2(topath);
		tulisLog2("ENCRYPT2", from, to);
	}

	if (strstr(from, en2) != NULL && strstr(to, en2) == NULL){
		dekripsi2(topath);
		tulisLog2("DECRYPT2", from, to);
	}

	return 0;
}
```

```c
static int xmp_rmdir(const char *path)
{
	int res;
	char fpath[1000];
	char *a = strstr(path, en1);
	if (a != NULL)
		decrypt1(a);

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = rmdir(fpath);
    
	tulisLog("RMDIR", fpath);                               // Pemanggilan fungsi tulislog

	if (res == -1)
		return -errno;

	return 0;
}
```
