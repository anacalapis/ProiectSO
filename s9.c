#include<stdio.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<dirent.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<unistd.h>
#include<stdint.h>
#include<math.h>
#include<time.h>
#include<sys/wait.h>

#define BUF_SIZE 512
#define BUF_READ 4

//verifica daca s-a primit exact 1 paramentru
void numar_corect_de_parametrii(int argc, int i)
{
    if(argc!=i)       
    {
        perror("Numarul parametrilor nu este corespunzator");
        exit(7);
    }
}
//se verifica daca argumentul dat in linia de comanda este director sau nu
void validare_director(char* argv[], struct stat dir, int i)
{
    char mesaj_eroare[100]="Usage ";
    strcat(mesaj_eroare, argv[0]);
    strcat(mesaj_eroare, " ");
    strcat(mesaj_eroare, argv[i]);
    if((stat(argv[i], &dir))<0)
    {
    	perror(mesaj_eroare);
        exit(7);
    }
    if(!S_ISDIR(dir.st_mode))
    {
        perror(mesaj_eroare);
        exit(7);
    }
}
//se verifica daca fisierul primit este o imagine .bmp
int validare_daca_este_sau_nu_imagine_bmp(char *denumire)
{
    char sir[50];
    strcpy(sir, denumire);
    strcpy(sir, sir+strlen(sir)-4); 
    if  (strcmp(sir, ".bmp")!=0)
    {
    	return 0;
    }
    return 1;
}
//verifica daca paramnetul primit in linia de comanda este de tip .bmp(saptamana 6)
void validare_imagine_bmp(char* argv[], struct stat var)
{
    int finbmp;
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
    if ((S_ISREG(var.st_mode) !=0 ) || (strcmp(sir_arg, ".bmp")!=0))
    {
    	perror(mesaj_eroare);
        exit(7);
    }
}
//se deschide fisierul primit ca si paramentru
void validare_fisiere_deschidere(int *fin, char* argv[])
{
    if((*fin=open(argv[1], O_RDONLY))<0)
        {
        perror("Nu s-a putut deschide fisierul de citire");
        }
}
//returneaza fisierul deschis daca totul este ok
DIR* validare_director_deschidere( char* argv[], int i)
{
    DIR* director;
    director=opendir(argv[i]);
    if((opendir(argv[i]))==NULL)
    {
        perror("Nu s-a putut deschide directorul");
    }
    return director;
}
//deschiderea fisierului de scriere
void validare_fisier_de_scriere(int *fout)
{
    if((*fout=open("statistica.txt", O_WRONLY|O_TRUNC|O_CREAT, S_IRWXU))<0)
        {   
        perror("Nu s-a putut deschide fisierul de scriere");
        }
}

void validare_fisier_de_scriere_in_director(int *fout, char nume[50], char* argv[])
{
    char nume_fisier[100];
    strcpy(nume_fisier, argv[2]);
    strcat(nume_fisier, "/");
    strcat(nume_fisier, nume);
    strcat(nume_fisier, "_statistica.txt");
    if((*fout=open(nume_fisier, O_WRONLY|O_TRUNC|O_CREAT, S_IRWXU))<0)
        {   
        perror("Nu s-a putut deschide fisierul de scriere");
        }
}
//am facut o functie de ridicare la putere deoarece fct pow nu poate primii ca si argumente variabile
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
//transformarea din binar in zecimal pentru un nr de octeti dat ca si parametru
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
//returneaza stringul care contine drepturile de acces pentru user
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
//returneaza stringul care contine drepturile de acces pentru grup
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
//returneaza stringul care contine drepturile de acces pentru others
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
//scrie in fisierul de iesire drepturile de acces pentru toate categoriile
void acces_drepturi_pentru_toate_tipurile_de_utilizatori(struct stat var, int *fout)
{
    uint8_t buffer[BUF_SIZE];
    sprintf(buffer, "dreptul de acces user : %s\n", acces_user(var));
    if((write(*fout, buffer, strlen(buffer)))<0)
    {
        perror("Nu s-a putut scrie in fisier!");
        exit(7);
    }
    sprintf(buffer, "dreptul de acces grup : %s\n", acces_grup(var));
    if((write(*fout, buffer, strlen(buffer)))<0)
    {
        perror("Nu s-a putut scrie in fisier!");
        exit(7);
    }
    sprintf(buffer, "dreptul de acces pt ceilalti : %s\n\n\n", acces_others(var));
    if((write(*fout, buffer, strlen(buffer)))<0)
    {
        perror("Nu s-a putut scrie in fisier!");
        exit(7);
    }

}
//se va afisa in formatul dorit (zi, luna, an) data ultimei modificari
void afisare_timpul_ultimei_modificari(int *fout, struct stat var)
{
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
//numara legaturile unui fisier
void contor_de_legaturi(int *fout, struct stat var)
{
    uint8_t buffer[BUF_SIZE];
    sprintf(buffer, "contorul de legaturi : %ld\n", var.st_nlink);
    if((write(*fout, buffer, strlen(buffer)))<0)
    {
        perror("Nu s-a putut scrie in fisier!");
        exit(7);
    }
}
//pentru a putea sari peste anumita informatie dintr-un fisier
//este folosita pentru a ne ajuta sa ajunge la bytes pe care ni dorim
void citire_inutila_din_fisier_pentru_a_sari_peste_info_inutila(int *fin, int nr_bytes)
{
    //uint8_t buffer_read_nefolosit[BUF_READ];
    char buffer_read_nefolosit[BUF_SIZE];
    if((read(*fin, buffer_read_nefolosit, nr_bytes))== -1)
    {
        perror("Nu s-a putut citi din fisier!");
        exit(7);
    }
}
//afiseaza in fisierul de scriere dimensiunile unei imagini .bmp
void dimensiune_imagine_bmp(int *fin, int *fout)
{
    uint8_t buffer_read[BUF_READ];
    uint8_t buffer[BUF_SIZE];
    int n=3;
    while(n>0)
    {
        uint8_t buffer_read_nefolosit[BUF_READ];
        if((read(*fin, buffer_read_nefolosit, 4))== -1)
        {
            perror("Nu s-a putut citi din fisier!");
            exit(7);
        }
        n--;
    }
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
//se afiseaza in fisierul de sciere numele fisierului primit ca si parametru (saptamana 6 )
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
//se afiseaza in fisierul de sciere dimensiune unui fisier in bytes
void afisare_dimensiune_in_octeti(int *fout, struct stat var)
{
    char buffer[BUF_SIZE];
    sprintf(buffer, "dimensiunea in octeti : %ld\n", var.st_size);
    if((write(*fout, buffer, strlen(buffer)))<0)
    {
        perror("Nu s-a putut scrie in fisier!");
        exit(7);
    } 
}
//afisarea fisierului target in cazul legaturilor simbolice
void afisare_dimensiune_in_octeti_a_fisierului_target(int *fout, struct stat var)
{
    char buffer[BUF_SIZE];
    sprintf(buffer, "dimensiunea in octeti a fisierului target : %ld\n", var.st_size);
    if((write(*fout, buffer, strlen(buffer)))<0)
    {
        perror("Nu s-a putut scrie in fisier!");
        exit(7);
    } 
}
//afisarea in fisierul de scriere a dimensiunii in bytes a unei imagini .bmp
void afisare_dimensiuni_in_octeti_a_imaginii_bmp(int *fin, int *fout)
{
    uint8_t buffer[BUF_SIZE];
    uint8_t buffer_read[BUF_READ];
    uint8_t buffer_read_nefolosit[BUF_READ];
    if((read(*fin, buffer_read_nefolosit, 2))== -1)
    {
        perror("Nu s-a putut citi din fisier!");
        exit(7);
    }
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
void afisare_identificator_utilizator(int *fout, struct stat var)
{
    uint8_t buffer[BUF_SIZE];
    sprintf(buffer, "identificatorul utilizatorului: %d\n", var.st_uid);
    if((write(*fout, buffer, strlen(buffer)))<0)
    {
        perror("Nu s-a putut scrie in fisier!");
        exit(7);
    }
}
//afisarea numelui unui fisier ce se afla intr-un director
void afisare_nume_fisier_din_director(int *fout, struct dirent *dir)
{
    uint8_t buffer[BUF_SIZE];
    sprintf(buffer, "nume fisier: %s\n", dir->d_name);
    if((write(*fout, buffer, strlen(buffer)))<0)
    {
        perror("Nu s-a putut scrie in fisier!");
        exit(7);
    }   
}
//inchiderea fisierelor de sciere si citire
void inchidere_fisier(int *fis)
{
    int fclose;
    if((fclose=close(*fis))!=0)
    {
        perror("Nu s-a putut inchide fisierul");
        exit(7);
    }
}
//inchiderea directorului
void inchiderea_directorului(DIR *dir)
{
    int fclose;
    if((fclose=closedir(dir))!=0)
    {
        perror("Nu s-a putut inchide directorul");
        exit(7);
    }
}
void validare_fork(int pid)
{
    if(pid<0)
    {
        perror("Eroare la fork");
        exit(7);
    }
}

void afisare_detaltii_proces(int nr_de_intrari)
{
    int status;
     for(int i=0; i<nr_de_intrari; i++)
    {
        int fiu=wait(&status);
        if(fiu<0)
        {
            perror("nu se asteapta");
            exit(7);
        }
        if(WIFEXITED(status))
        {
            printf("procesul %d s-a incheiat cu statusul %d si pid parinte %d\n", fiu, WEXITSTATUS(status), getppid());
        }
    }
}

void deschidere_fisier_pentru_citire(int *fin, char *nume)
{
    if((*fin=open(nume, O_RDONLY))<0)
    {
        perror("Eroare la deschidere!");
        exit(7);
    }
}
void deschidere_fisier_pentru_citire_si_scriere(int *fin, char *nume)
{
    if((*fin=open(nume, O_RDWR))<0)
    {
        perror("Eroare la deschidere!");
        exit(7);
    }
}

void validare_parametru_alfanumeric(char *argv[])
{   
    char param;
    if(argv[3][1]=='\0')
    {
        param=argv[3][0];
    }
    if(!isalnum(param))
    {
        perror("Ultimul argument nu e caracter alfanumeric!");
        exit(7);
    }
}

int main(int argc, char* argv[])
{
    int FIU1_FIU2[2], FIU2_PARINTE[2];




    int contor_pt_propozitii=0;
    int fout, fin;
    struct stat var;
    DIR* director_citire;
    DIR* director_scriere;
    numar_corect_de_parametrii(argc,4);
    validare_director(argv, var,2);
    validare_parametru_alfanumeric(argv);
    
    director_citire=validare_director_deschidere(argv,1);
    director_scriere=validare_director_deschidere(argv, 2);
    int nr_de_intrari=0;
    struct dirent *dir;
    while((dir=readdir(director_citire))!=NULL)
    {   
            struct stat dir_actual; 
            char nume[100];
            strcpy(nume, argv[1]);
            strcat(nume, "/");
            strcat(nume, dir->d_name);
            validare_fisier_de_scriere_in_director(&fout, dir->d_name, argv);
            if((lstat(nume, &dir_actual))!=0) 
            {
                perror("Eroare la parcurgerea directorului");
                exit(7);
            }
            if(S_ISLNK(dir_actual.st_mode) !=0 )
            {   
                nr_de_intrari++;
                int pid=fork();
                validare_fork(pid);
                if(pid==0)
                {
                    deschidere_fisier_pentru_citire(&fin, nume);
                    struct stat leg;
                    if((stat(nume, &leg))==0)
                    {
                        afisare_nume_fisier_din_director(&fout, dir);
                        afisare_dimensiune_in_octeti(&fout, dir_actual);
                        afisare_dimensiune_in_octeti_a_fisierului_target(&fout, leg);
                        acces_drepturi_pentru_toate_tipurile_de_utilizatori(dir_actual, &fout);
                    }
                    inchidere_fisier(&fout);
                    inchidere_fisier(&fin);
                    exit(nr_de_intrari);
                    
                }
                
                
            }
            if(S_ISREG(dir_actual.st_mode) !=0 ) 
            { 
                if(validare_daca_este_sau_nu_imagine_bmp(dir->d_name)==1)
                {
                    nr_de_intrari++;
                    int pid=fork();
                    validare_fork(pid);
                    if(pid==0)
                    {
                        deschidere_fisier_pentru_citire(&fin, nume);
                        afisare_nume_fisier_din_director(&fout, dir);
                        afisare_dimensiuni_in_octeti_a_imaginii_bmp(&fin, &fout);
                        dimensiune_imagine_bmp(&fin, &fout);
                        //sar peste 28 de bytes
                        citire_inutila_din_fisier_pentru_a_sari_peste_info_inutila(&fin, 28);
                        //prelucrare de bytes
                        afisare_identificator_utilizator(&fout, dir_actual);
                        afisare_timpul_ultimei_modificari(&fout, dir_actual);
                        contor_de_legaturi(&fout, dir_actual);
                        acces_drepturi_pentru_toate_tipurile_de_utilizatori(dir_actual, &fout);
                        inchidere_fisier(&fin);
                        inchidere_fisier(&fout);
                        exit(nr_de_intrari);
                    }
                    nr_de_intrari++;
                    int pid2=fork();
                    validare_fork(pid2);
                    if(pid2==0)
                    {
                        deschidere_fisier_pentru_citire_si_scriere(&fin, nume);
                        citire_inutila_din_fisier_pentru_a_sari_peste_info_inutila(&fin, 18);
                        __uint32_t width, height; //pentru a nu ma complica sa iau 4*_uint8 si sa am vectori
                        __uint8_t buffer_read[4];
                        int r1,r2;
                        if((r1=read(fin, buffer_read,4))<0)
                        {
                            perror("Nu s-a putut citi din fisier!");
                            exit(7);
                        }
                        width=calculare_format_zecimal(4, buffer_read);
                        if((r1=read(fin,buffer_read,4))<0)
                        {
                            perror("Nu s-a putut citi din fisier!");
                            exit(7);
                        }
                        height=calculare_format_zecimal(4, buffer_read);
                        //printf("%d %d", width, height);
                        citire_inutila_din_fisier_pentru_a_sari_peste_info_inutila(&fin, 28);
                        for(int i=0; i<(height*width); i++)
                        {
                            __uint8_t buffer_color[3];
                            if(read(fin,buffer_color,3)<0)
                            {
                                perror("Nu s-a putut citi din fisier culoare!");
                                exit(7);
                            }
                            __uint8_t culoare_gri;
                            culoare_gri= 0.299*buffer_color[0]+0.587*buffer_color[1]+0.114*buffer_color[2];
                            buffer_color[0]=culoare_gri;
                            buffer_color[1]=culoare_gri;
                            buffer_color[2]=culoare_gri;
                            lseek(fin, -3, SEEK_CUR);   //ma deplasez inapoi pt a putea scrie la locurile bune
                            if((write(fin, buffer_color, 3))<0)
                            {
                                perror("Nu s-a putut scrie in fisier culoare!");
                                exit(7);
                            }
                        }
                        inchidere_fisier(&fin);
                        exit(nr_de_intrari);                              
                    }       
                
                }
                else
                {
                    if(pipe(FIU1_FIU2)<0)
                    {
                        perror("eroare de la fiu la fiu");
                        exit(7);
                    }
                    if(pipe(FIU2_PARINTE)<0)
                    {
                        perror("eroare de la fiu2 la parinte");
                        exit(7);
                    }
                    nr_de_intrari++;
                    int pid1=fork();
                    validare_fork(pid1);
                    if(pid1==0)
                    {
                        
                        deschidere_fisier_pentru_citire(&fin, nume);
                        afisare_nume_fisier_din_director(&fout, dir);
                        afisare_dimensiune_in_octeti(&fout, dir_actual);
                        afisare_identificator_utilizator(&fout, dir_actual);
                        afisare_timpul_ultimei_modificari(&fout, dir_actual);
                        contor_de_legaturi(&fout, dir_actual);
                        acces_drepturi_pentru_toate_tipurile_de_utilizatori(dir_actual, &fout);

                        lseek(fin,0, SEEK_SET);    //sunt inapoi la inceputul fisierului

                        //cat fin in fiul celalat
                        //0-citire, 1-scriere
                        if (close(FIU1_FIU2[0]) < 0)
                        {
                            perror("Nu s-a inchis capatul de citire al procesului 1");
                            exit(7);
                        }
                        if (close(FIU2_PARINTE[0]) < 0)
                        {
                            perror("Nu s-a inchis capatul de citire al procesului 2");
                            exit(7);
                        } 
                        if (close(FIU2_PARINTE[1]) < 0)
                        {
                            perror("Nu s-a inchis capatul de citire al procesului 2");
                            exit(7);
                        } 

                        dup2(FIU1_FIU2[1], 1);

                        if (close(FIU1_FIU2[1]) < 0)
                        {
                            perror("Nu s-a inchis capatul de scriere al procesului 1");
                            exit(7);
                        }
                        

                        execlp("cat", "cat", nume ,NULL);
            
                        inchidere_fisier(&fin); 
                        inchidere_fisier(&fout); 
                        exit(nr_de_intrari);
                        
                    } 
                    
                    nr_de_intrari++;
                    int pid2=fork();
                    validare_fork(pid2);
                    if(pid2==0)
                    {
                        
                        if (close(FIU1_FIU2[1]) < 0)
                        {
                            perror("Nu s-a inchis capatul de scriere al procesului 1");
                            exit(7);
                        }
                        if (close(FIU2_PARINTE[0]) < 0)
                        {
                            perror("Nu s-a inchis capatul de citire al procesului 2");
                            exit(7);
                        } 

                        dup2(FIU1_FIU2[0], 0);         //pt ca scriptul sa citeasca de la "STDIN"
                        dup2(FIU2_PARINTE[1], 1);     //ca iesire de la STDOUT sa nu se faca pe ecran ci in parinte


                        if (close(FIU1_FIU2[0]) < 0)
                        {
                            perror("Nu s-a inchis capatul de citire al procesului 1");
                            exit(7);
                        }
                        if (close(FIU2_PARINTE[1]) < 0)
                        {
                            perror("Nu s-a inchis capatul de citire al procesului 2");
                            exit(7);
                        } 

                        execlp("bash", "bash", "bash_script.sh", argv[3] , NULL);
                        
                        inchidere_fisier(&fin); 
                        exit(nr_de_intrari);
                    }
                     
                    if (close(FIU1_FIU2[0]) < 0)
                    {
                        perror("Nu s-a inchis capatul de citire al procesului 1");
                        exit(7);
                    }
                    if (close(FIU1_FIU2[1]) < 0)
                    {
                        perror("Nu s-a inchis capatul de scriere al procesului 1");
                        exit(7);
                    }
                    if (close(FIU2_PARINTE[1]) < 0)
                    {
                        perror("Nu s-a inchis capatul de citire al procesului 2");
                        exit(7);
                    } 
                    //read pt a citii din pipe

                    int citire_pipe, contor_citire_pipe;

                    char buffer_citire_pipe[10];
                    if((citire_pipe=read(FIU2_PARINTE[0], buffer_citire_pipe, 10 ))>=0)
                    {
                        contor_citire_pipe=atoi(buffer_citire_pipe);
                        contor_pt_propozitii=contor_pt_propozitii+contor_citire_pipe;
                    }
                    if (close(FIU2_PARINTE[0]) < 0)
                    {
                        perror("Nu s-a inchis capatul de citire al procesului 2");
                        exit(7);
                    }
                    
                }
            }
            if(S_ISDIR(dir_actual.st_mode) !=0 )
            {
                nr_de_intrari++;
                int pid=fork();
                validare_fork(pid);
                if(pid==0)
                {
                    deschidere_fisier_pentru_citire(&fin, nume);
                    afisare_nume_fisier_din_director(&fout, dir);
                    afisare_identificator_utilizator(&fout, dir_actual);
                    acces_drepturi_pentru_toate_tipurile_de_utilizatori(dir_actual, &fout);
                    inchidere_fisier(&fin); 
                    inchidere_fisier(&fout);
                    exit(nr_de_intrari);
                    
                }
            }
        //sleep(1);
    }
    afisare_detaltii_proces(nr_de_intrari);
    printf("Au fost identificate in total %d propozitii care contin caracterul %s. \n", contor_pt_propozitii, argv[3]);
    inchiderea_directorului(director_citire);
    inchiderea_directorului(director_scriere);
    return 0;
}

