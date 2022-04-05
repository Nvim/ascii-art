#include <stdio.h>
#include<stdlib.h>

//affiche les infos du header (14 premiers octets):
void print_header(char *img){

    FILE * p = p = fopen(img,"rb");
    if (p == NULL){
        printf("\nerreur");
        exit(EXIT_FAILURE);
    }
    printf("\nouverture reussie");
    
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

    //octets 14 à 18: taille du header 2
    int size;
    fread(&size,4,1,p);

    //octets 18 à 20: width
    int width;
    fread(&width,2,1,p);
    //octets 20 à 22: height
    int height;
    fread(&height,2,1,p);
    //octets 22 à 24: color planes(must be 1)
    int planes;
    fread(&planes,2,1,p);
    //octets 24 à 26: bits par pixel
    int bits;
    fread(&bits,2,1,p);

    //affichage
    printf("\ntype de l'image: %c%c", octet_1,octet_2);
    printf("\ntaille de l'image: %d octets", taille_image);
    printf("\napplication utilisee: %d", application);
    printf("\n0 normalement: %d", application2);
    printf("\noctet du premier pixel: %d\n~~~~~~~~", debut);
    printf("\ntaille du header 2: %d",size);
    printf("\nlargeur de l'image: %d",width);
    printf("\nhauteur de l'image: %d",height);
    printf("\ncolor planes: %d",planes);
    printf("\nnombre de bits par pixel: %d",bits);
    fclose(p);

}    

int main(void){

    print_header("minecraft.bmp");
    return EXIT_SUCCESS;
}