#include <stdio.h>
#include<stdlib.h>

struct bmp_header{ //correspond aux 14 premiers octets
    unsigned char name[2];//BM
    unsigned int size;//taille de l'image au total (headers inclus)
    unsigned short int app1;//on s'en fout mais c'est 0
    unsigned short int app2;//pareil
    unsigned int offset; //octet du premier pixel: 54 pour les 24bits(16 777 216 couleurs), 1078 pour les 8bits(256 couleurs) 118 pour les 4bits(16 couleurs)
};

struct dib_header{ //correspond aux 40 octets suivants
    unsigned int header_size;//taille du header dib (40 octets)
    unsigned int width;
    unsigned int height;
    unsigned short int colorplanes;//pas compris mais c'est 1
    unsigned short int bpp;//bits par pixel, 8 = noir et blanc 24 = RGB
    unsigned int compression;//méthode de compression utilisée
    unsigned int pixelarray_size;//taille de l'image (headers exclus) = pixelarraysize
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
    printf("\nTaille du header DIB: %d\nLargeur: %dpx\nHauteur: %dpx\nColor Planes: %d\nBits par pixel: %d\nMethode de compression: %d\nTaille de l'image sans header(PixelArraySize): %d", dib_header.header_size, dib_header.width,dib_header.height,dib_header.colorplanes,dib_header.bpp,dib_header.compression,dib_header.pixelarray_size);

    //on place le curseur (le pointeur fp) a l'octet du premier pixel (offset) et on lit les valeurs de rgb sur 3 char car on a 24 bits/pixel donc 3 octets/pixel
    fseek(fp, bmp_header.offset, SEEK_SET);
    unsigned char r;
    unsigned char g;
    unsigned char b;
    fread(&r, 1, 1, fp);
    fread(&g, 1, 1, fp);
    fread(&b, 1, 1, fp);
    printf("\n\nPremier pixel: %d %d %d", r,g,b);

    unsigned char r2;
    unsigned char g2;
    unsigned char b2;
    fread(&r2, 1, 1, fp);
    fread(&g2, 1, 1, fp);
    fread(&b2, 1, 1, fp);
    printf("\nDeuxieme pixel: %d %d %d", r2,g2,b2);

    unsigned char r3;
    unsigned char g3;
    unsigned char b3;
    fread(&r3, 1, 1, fp);
    fread(&g3, 1, 1, fp);
    fread(&b3, 1, 1, fp);
    printf("\nTroisieme pixel: %d %d %d", r3,g3,b3);

    fseek(fp, 4, SEEK_CUR);//padding: PixelArraySize = 16 mais on a 4 pixels de 3 octets, 3*4 = 12 donc on a 4 octets fantomes
    unsigned char r4;
    unsigned char g4;
    unsigned char b4;
    fread(&r4, 1, 1, fp);
    fread(&g4, 1, 1, fp);
    fread(&b4, 1, 1, fp);
    printf("\nQuatrieme pixel: %d %d %d", r4,g4,b4);
}


int main(void){

    if(sizeof(char) == 1 && sizeof(short int) == 2 && sizeof(int) == 4){
        print_header_structs("bmpsimple.bmp");
        return EXIT_SUCCESS;
    }
    else{
        printf("\nTaille des variables pas adaptée");
        return EXIT_FAILURE;
    }
}