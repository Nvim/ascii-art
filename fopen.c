#include <stdio.h>
#include<stdlib.h>

//affiche les infos du header:
void print_header(char *img){

    FILE * p = p = fopen(img,"rb");;
    if (p == NULL){
        printf("\nerreur");
        exit(EXIT_FAILURE);
    }
    printf("ouverture reussie");
    
    //2 premiers octets: char 'B' et 'M'
    char octet_1;
    char octet_2;
    fread(&octet_1,1,1,p);
    fread(&octet_2,1,1,p);

    //octets 2 à 6: taille de l'image
    int taille_image;
    fread(&taille_image,4,1,p);

    //octets 6 à 10: application utilisée(0) 2 fois
    int application;
    int application2;
    fread(&application,2,1,p);
    fread(&application2,2,1,p);

    //octets 10 à 14: adresse de l'octet ou les données de l'image commencent
    int debut;
    fread(&debut,4,1,p);

    //affichage
    printf("\ntype de l'image: %c%c", octet_1,octet_2);
    printf("\ntaille de l'image: %d octets", taille_image);
    printf("\napplication utilisee: %d", application);
    printf("\n0 normalement: %d", application2);
    printf("\noctet du premier pixel: %d", debut);
}


int main(void){

    print_header("minecraft.bmp");
    return EXIT_SUCCESS;
}