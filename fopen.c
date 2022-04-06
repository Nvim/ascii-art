#include <stdio.h>
#include<stdlib.h>

struct bmp_header{ //correspond aux 14 premiers octets
    unsigned char name[2];//BM
    unsigned int size;//taille de l'image au total (headers inclus)
    unsigned short int app1;//on s'en fout mais c'est 0
    unsigned short int app2;//pareil
    unsigned int offset; //octet du premier pixel: 54 pour les 24bits(16 777 216 couleurs), 1078 pour les 8bits(256 couleurs) 118 pour les 4bits(16 couleurs)
};

struct dib_header{ //correspond aux 40 premiers octets noramlement
    unsigned int header_size;//taille du header dib (40 octets)
    unsigned int width;
    unsigned int height;
    unsigned short int colorplanes;//pas compris mais c'est 1
    unsigned short int bpp;//bits par pixel, 8 = noir et blanc 24 = RGB
    unsigned int compression;//méthode de compression utilisée
    unsigned int img_size;//taille de l'image (headers exclus)
};


//affiche les informations utiles des headers BMP et DIB
void print_header_structs(char *img){
    FILE * fp = fopen(img,"rb");
    if (fp == NULL){
        printf("\nerreur");
        exit(EXIT_FAILURE);
    }
    printf("\nOuverture reussie: \n");
    struct bmp_header bmp_header;
    struct dib_header dib_header;
    
    //on lit pas tout le struct d'un coup a cause du nom en char
    fread(bmp_header.name,2,1,fp);
    printf("\nType d'image: %c%c", bmp_header.name[0],bmp_header.name[1]);

    //on devrait lire tout ça d'un coup avec 2*sizeof(int)+2*sizeof(short int) = 3*sizeof(int) ?
    fread(&bmp_header.size,4,1,fp);
    fread(&bmp_header.app1,2,1,fp);
    fread(&bmp_header.app2,2,1,fp);
    fread(&bmp_header.offset,4,1,fp);
    printf("\nTaille de l'image (header inclus): %d\nOffset (nombre d'octets avant le premier pixel): %d\n",bmp_header.size,bmp_header.offset);


    //méthode lire tout le struct d'un coup
    fread(&dib_header,sizeof(struct dib_header),1,fp);
    printf("\nTaille du header DIB: %d\nLargeur: %dpx\nHauteur: %dpx\nColor Planes: %d\nBits par pixel: %d\nMethode de compression: %d\nTaille de l'image (sans headers): %d", dib_header.header_size, dib_header.width,dib_header.height,dib_header.colorplanes,dib_header.bpp,dib_header.compression,dib_header.img_size);
}


int main(void){

    if(sizeof(char) == 1 && sizeof(short int) == 2 && sizeof(int) == 4){
        print_header_structs("onepiece256.bmp");
        return EXIT_SUCCESS;
    }
    else{
        printf("\nTaille des variables pas adaptée");
        return EXIT_FAILURE;
    }
}