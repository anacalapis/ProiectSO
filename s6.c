#include<stdio.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<unistd.h>
#include<stdint.h>
#include<math.h>
#include<time.h>
#define BUF_SIZE 512
#define BUF_READ 4

int ridicare_la_putere (int baza, int exp)
{
    int rezultat=1;
    while(exp>0)
    {
        rezultat=rezultat*baza;
        exp--;
    }
    return rezultat;
}

int calculare_format_zecimal(int nr_octeti, uint8_t buffer_read[])
{
    int k_bit=0;   //tine minte la ce putere trebuie sa inmultim cand facem conversia
    int nr=0; 
    for(int i=0; i<nr_octeti; i++)
    {
        int copie=buffer_read[i];
        for(int j=0; j<8; j++)      //trecem prin fiecare bit din octet
        {
            if(copie%2==1)
            {
                nr=nr+ridicare_la_putere(2,k_bit);
            }
            copie=copie/2;
            k_bit++;
        }
    }
    return nr;
}
char sir[5];
char* acces_user(struct stat var)
{
    strcpy(sir, "\0");
    if((var.st_mode & S_IRUSR) !=0)
        strcat(sir, "r");
    else
        strcat(sir, "-");
    if((var.st_mode & S_IWUSR) !=0)
        strcat(sir, "w");
    else
        strcat(sir, "-");
    if((var.st_mode & S_IXUSR) !=0)
        strcat(sir, "x");
    else
        strcat(sir, "-");
    return sir;
}
char* acces_grup(struct stat var)
{
    strcpy(sir, "\0");
    if((var.st_mode & S_IRGRP) !=0)
        strcat(sir, "r");
    else
        strcat(sir, "-");
    if((var.st_mode & S_IWGRP) !=0)
        strcat(sir, "w");
    else
        strcat(sir, "-");
    if((var.st_mode & S_IXGRP) !=0)
        strcat(sir, "x");
    else
        strcat(sir, "-");
    return sir;
}
char* acces_others(struct stat var)
{
    strcpy(sir, "\0");
    if((var.st_mode & S_IROTH) !=0)
        strcat(sir, "r");
    else
        strcat(sir, "-");
    if((var.st_mode & S_IWOTH) !=0)
        strcat(sir, "w");
    else
        strcat(sir, "-");
    if((var.st_mode & S_IXOTH) !=0)
        strcat(sir, "x");
    else
        strcat(sir, "-");
    return sir;
}
int main(int argc, char* argv[])
{
    struct stat var;
    int fout, fin, fclose, rd;
    uint8_t buffer_read[BUF_READ], buffer_read_nefolosit[BUF_READ];
    uint8_t buffer[BUF_SIZE];
    int finbmp;
    if(argc!=2)        //daca avem 2 argumente
    {
        perror("Numarul parametrilor nu este corespunzator");
        exit(7);
    }

    char mesaj_eroare[100]="Usage ";
    strcat(mesaj_eroare, argv[0]);
    strcat(mesaj_eroare, " ");
    strcat(mesaj_eroare, argv[1]);
 
    if(finbmp=((stat(argv[1], &var))!=0) ) 
    {
    	perror(mesaj_eroare);
        exit(7);
    }
    char sir_arg[100];
    strcpy(sir_arg, argv[1]);
    strcpy(sir_arg, sir_arg+strlen(sir_arg)-4); 
    if ((S_ISREG(var.st_mode) !=0 ) && (strcmp(sir_arg, ".bmp")!=0))
    {
    	perror(mesaj_eroare);
        exit(7);
    }
   if((fin=open(argv[1], O_RDONLY))<0)
   {
        perror("Nu s-a putut deschide fisierul de citire");
   }
   if((fout=open("statistica.txt", O_WRONLY|O_TRUNC|O_CREAT, S_IRWXU))<0)
    {
        perror("Nu s-a putut deschide fisierul de scriere");
    }

    sprintf(buffer, "nume fisier : %s\n", argv[1]);
    write(fout, buffer, strlen(buffer));

    read(fin, buffer_read_nefolosit, 2);
    read(fin, buffer_read, BUF_READ);
    sprintf(buffer, "dimensiune in octeti : %u\n", calculare_format_zecimal(BUF_READ, buffer_read));
    write(fout, buffer, strlen(buffer));

    read(fin, buffer_read_nefolosit,BUF_READ);
    read(fin, buffer_read_nefolosit,BUF_READ);
    read(fin, buffer_read_nefolosit,BUF_READ);
    read(fin, buffer_read, BUF_READ);
    sprintf(buffer, "inaltime : %u\n", calculare_format_zecimal(BUF_READ, buffer_read));
    write(fout, buffer, strlen(buffer));

    read(fin, buffer_read, BUF_READ);
    sprintf(buffer, "lungime : %u\n", calculare_format_zecimal(BUF_READ, buffer_read));
    write(fout, buffer, strlen(buffer));

    sprintf(buffer, "identificatorul utilizatorului: %d\n", var.st_uid);
    write(fout, buffer, strlen(buffer));

    char buffer_data[50];
    time_t timp=var.st_mtime;
    struct tm data_si_timp;
    localtime_r(&timp, &data_si_timp);
    strftime(buffer_data, 50, "timpul ultimei modificari %d.%m.%Y \n", &data_si_timp);
    write(fout, buffer_data, strlen(buffer_data));

    sprintf(buffer, "contorul de legaturi : %ld\n", var.st_nlink);
    write(fout, buffer, strlen(buffer));

  
    sprintf(buffer, "dreptul de acces user : %s\n", acces_user(var));
    write(fout, buffer, strlen(buffer));
    sprintf(buffer, "dreptul de acces grup : %s\n", acces_grup(var));
    write(fout, buffer, strlen(buffer));
    sprintf(buffer, "dreptul de acces pt ceilalti : %s\n", acces_others(var));
    write(fout, buffer, strlen(buffer));

    

    if((fclose=close(fin))!=0)
    {
        perror("Nu s-a putut inchide fisierul de intrare");
    }
    if((fclose=close(fout))!=0)
    {
        perror("Nu s-a putut inchide fisierul de scriere");
    }
    return 0;
}

