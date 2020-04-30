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