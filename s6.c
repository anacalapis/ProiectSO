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

void validare_imagine_bmp(int argc, char* argv[], struct stat var)
{
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
}

void validare_fisiere_deschidere_si_scriere(int *fin, int *fout, char* argv[])
{
    if((*fin=open(argv[1], O_RDONLY))<0)
        {
        perror("Nu s-a putut deschide fisierul de citire");
        }
   if((*fout=open("statistica.txt", O_WRONLY|O_TRUNC|O_CREAT, S_IRWXU))<0)
        {   
        perror("Nu s-a putut deschide fisierul de scriere");
        }
}

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
char* acces_user(struct stat var, char* argv[])
{
    stat(argv[1], &var);
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
char* acces_grup(struct stat var, char* argv[])
{
    stat(argv[1], &var);
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
char* acces_others(struct stat var, char* argv[])
{
    stat(argv[1], &var);
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

void acces_drepturi_pentru_toate_tipurile_de_utilizatori(struct stat var, int *fout, char* argv[])
{
    stat(argv[1], &var);
    uint8_t buffer[BUF_SIZE];
    sprintf(buffer, "dreptul de acces user : %s\n", acces_user(var, argv));
    if((write(*fout, buffer, strlen(buffer)))<0)
    {
        perror("Nu s-a putut scrie in fisier!");
        exit(7);
    }
    sprintf(buffer, "dreptul de acces grup : %s\n", acces_grup(var, argv));
    if((write(*fout, buffer, strlen(buffer)))<0)
    {
        perror("Nu s-a putut scrie in fisier!");
        exit(7);
    }
    sprintf(buffer, "dreptul de acces pt ceilalti : %s\n", acces_others(var,argv));
    if((write(*fout, buffer, strlen(buffer)))<0)
    {
        perror("Nu s-a putut scrie in fisier!");
        exit(7);
    }

}
void afisare_timpul_ultimei_modificari(int *fout, struct stat var, char* argv[])
{
    stat(argv[1], &var);
    char buffer_data[50];
    time_t timp=var.st_mtime;
    struct tm data_si_timp;
    localtime_r(&timp, &data_si_timp);
    strftime(buffer_data, 50, "timpul ultimei modificari %d.%m.%Y \n", &data_si_timp);
    if((write(*fout, buffer_data, strlen(buffer_data)))<0)
    {
        perror("Nu s-a putut scrie in fisier!");
        exit(7);
    }
}
void contor_de_legaturi(int *fout, struct stat var, char* argv[])
{
    uint8_t buffer[BUF_SIZE];
    stat(argv[1], &var);
    sprintf(buffer, "contorul de legaturi : %ld\n", var.st_nlink);
    if((write(*fout, buffer, strlen(buffer)))<0)
    {
        perror("Nu s-a putut scrie in fisier!");
        exit(7);
    }
}

void dimensiune_imagine_bmp(int *fin, int *fout)
{
    uint8_t buffer_read[BUF_READ];
    uint8_t buffer[BUF_SIZE];
    if((read(*fin, buffer_read, BUF_READ))== -1)
    {
        perror("Nu s-a putut citi din fisier!");
        exit(7);
    }
    sprintf(buffer, "inaltime : %u\n", calculare_format_zecimal(BUF_READ, buffer_read));
    if((write(*fout, buffer, strlen(buffer)))<0)
    {
        perror("Nu s-a putut scrie in fisier!");
        exit(7);
    }

    if((read(*fin, buffer_read, BUF_READ))== -1)
    {
        perror("Nu s-a putut citi din fisier!");
        exit(7);
    }
    sprintf(buffer, "lungime : %u\n", calculare_format_zecimal(BUF_READ, buffer_read));
    if((write(*fout, buffer, strlen(buffer)))<0)
    {
        perror("Nu s-a putut scrie in fisier!");
        exit(7);
    }
}

void citire_inutila_din_fisier_pentru_a_sari_peste_info_inutila(int *fin, int nr_bytes)
{
    uint8_t buffer_read_nefolosit[BUF_READ];
    if((read(*fin, buffer_read_nefolosit, nr_bytes))== -1)
    {
        perror("Nu s-a putut citi din fisier!");
        exit(7);
    }
}

void afisare_nume_fisier(int *fout, char* argv[])
{
    uint8_t buffer[BUF_SIZE];
    sprintf(buffer, "nume fisier : %s\n", argv[1]);
    if((write(*fout, buffer, strlen(buffer)))<0)
    {
        perror("Nu s-a putut scrie in fisier!");
        exit(7);
    } 
}
void afisare_dimensiuni_in_octeti(int *fin, int *fout)
{
    uint8_t buffer[BUF_SIZE];
    uint8_t buffer_read[BUF_READ];
    if((read(*fin, buffer_read, BUF_READ))== -1)
    {
        perror("Nu s-a putut citi din fisier!");
        exit(7);
    }
    sprintf(buffer, "dimensiune in octeti : %u\n", calculare_format_zecimal(BUF_READ, buffer_read));
    if((write(*fout, buffer, strlen(buffer)))<0)
    {
        perror("Nu s-a putut scrie in fisier!");
        exit(7);
    }
    
}

void afisare_identificator_utilizator(int *fout, struct stat var, char* argv[])
{
    uint8_t buffer[BUF_SIZE];
    stat(argv[1], &var);
    sprintf(buffer, "identificatorul utilizatorului: %d\n", var.st_uid);
    if((write(*fout, buffer, strlen(buffer)))<0)
    {
        perror("Nu s-a putut scrie in fisier!");
        exit(7);
    }

}

void inchidere_fisiere(int *fin, int *fout)
{
    int fclose;
    if((fclose=close(*fin))!=0)
    {
        perror("Nu s-a putut inchide fisierul de intrare");
    }
    if((fclose=close(*fout))!=0)
    {
        perror("Nu s-a putut inchide fisierul de scriere");
    }
}

int main(int argc, char* argv[])
{
    struct stat var;
    int fout, fin, fclose, rd, finbmp;
   
    if(finbmp=((stat(argv[1], &var))!=0) ) 
    {
    	perror("mesaj_eroare");
        exit(7);
    }

    validare_imagine_bmp(argc, argv, var);
    validare_fisiere_deschidere_si_scriere(&fin, &fout, argv);

    afisare_nume_fisier(&fout, argv);

    citire_inutila_din_fisier_pentru_a_sari_peste_info_inutila(&fin, 2);
    afisare_dimensiuni_in_octeti(&fin, &fout);

    citire_inutila_din_fisier_pentru_a_sari_peste_info_inutila(&fin, BUF_READ);
    citire_inutila_din_fisier_pentru_a_sari_peste_info_inutila(&fin, BUF_READ);
    citire_inutila_din_fisier_pentru_a_sari_peste_info_inutila(&fin, BUF_READ);
    
    dimensiune_imagine_bmp(&fin, &fout);

    afisare_identificator_utilizator(&fout,var,argv);
    contor_de_legaturi(&fout, var, argv);
    afisare_timpul_ultimei_modificari(&fout, var, argv);
    acces_drepturi_pentru_toate_tipurile_de_utilizatori(var, &fout, argv);    
    inchidere_fisiere(&fin, &fout);
    return 0;
}

