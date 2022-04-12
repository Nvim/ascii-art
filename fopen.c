#include <stdio.h>
#include<stdlib.h>

struct bmp_header{ //correspond aux 14 premiers octets
    unsigned char name[2];//BM
    unsigned int size;//taille de l'image au total (headers inclus)
    unsigned short int app1;//on s'en fout mais c'est 0
    unsigned short int app2;//pareil
    unsigned int offset; //octet du premier pixel= 54 pour les 24bits(16 777 216 couleurs), 1078 pour les 8bits(256 couleurs) 118 pour les 4bits(16 couleurs)
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

struct pixel{
    unsigned char b, g, r;
};

//affiche les informations utiles des headers BMP et DIB
void print_info(char *img){
    FILE * fp = fopen(img,"rb");
    if (fp == NULL){
        printf("\nerreur");
        exit(EXIT_FAILURE);
    }
    printf("\nOuverture reussie: \n");
    struct bmp_header bmp_header;
    struct dib_header dib_header;
    struct pixel pixel;

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
    
    //infos utiles sur le pixel array et le padding: 
    int rowsize = ((dib_header.bpp*dib_header.width+31)/32)*4; //taille en octets de chaque row
    int pixels_nb = dib_header.width * dib_header.height;  //nombre total de pixels sur l'image
    int padding_total = dib_header.pixelarray_size - (pixels_nb*3); //nombre total d'octets de padding: pixelarray_size - le nombre d'octets alloués a des pixels
    int padding_row = padding_total / dib_header.height; //nombre d'octets de padding par row: nombre d'octets de padding total / nombre de rows
    int pixels_row = (rowsize - padding_row)/3;
    printf("\nTaille de chaque row: %d\nNombre total de pixels: %d\nNombre d'octets de padding: %d\nPadding par row: %d\nPixels par row: %d\n", rowsize,pixels_nb,padding_total,padding_row,pixels_row);

    //on place le curseur (le pointeur fp) a l'octet du premier pixel (offset) et on lit les valeurs de rgb sur 3 char car on a 24 bits/pixel donc 3 octets/pixel
    fseek(fp, bmp_header.offset, SEEK_SET);

    printf("\nPixel Array: \n"); //affichage des octets 1 par 1
    for(int i = 1; i<=dib_header.height; i++){  //ligne i
        for (int j = 1; j<= pixels_row; j++){  //pixel j
            fread(&pixel, 3, 1, fp);
            printf("%d | %d | %d | ", pixel.b,pixel.g,pixel.r);
        }
        fseek(fp, padding_row, SEEK_CUR);//padding en fin de ligne
        for(int k = 0; k<2; k++){
            printf("* | ");
        }
    }

    fseek(fp, bmp_header.offset, SEEK_SET);
    printf("\n\nAffichage par pixel:\n"); //affichage valeurs pixels par pixel (plus lisible)
    int p = 1;
    for(int i = 1; i<=dib_header.height; i++){
        for (int j = 1; j<= pixels_row; j++){
            fread(&pixel, 3, 1, fp);
            printf("Pixel %d: %d, %d, %d\n",p, pixel.b,pixel.g,pixel.r);
            p++;
        }
        fseek(fp, padding_row, SEEK_CUR);
    }
    printf("\n");
    
    /*fseek(fp, bmp_header.offset, SEEK_SET);
    for(int i = 1; i<=dib_header.height; i++){
        for (int j = 1; j<= pixels_row; j++){
            fread(&pixel, -3, 1, fp);
            float avg = pixel.r*0.33 + pixel.g*0.5 + pixel.b*0.16;
            //float avg = (pixel.r + pixel.g + pixel.b)/3;
            if(avg<25){
                printf(" ");
            }
            else if(avg<50){
                printf(".");
            }
            else if(avg<50){
                printf(":");
            }
            else if(avg<75){
                printf("-");
            }
            else if(avg<100){
                printf("=");
            }
            else if(avg<125){
                printf("+");
            }
            else if(avg<150){
                printf("*");
            }
            else if(avg<175){
                printf("#");
            }
            else if(avg<200){
                printf("%");
            }
            else if(avg<225){
                printf("@");
            }
            else{
                printf("$");
            }
        }
        printf("\n");
        fseek(fp, padding_row, SEEK_CUR);
    }*/

}
    
int main(void){

    if(sizeof(char) == 1 && sizeof(short int) == 2 && sizeof(int) == 4){
        print_info("small.bmp");
        return EXIT_SUCCESS;
    }
    else{
        printf("\nTaille des variables pas adaptée");
        return EXIT_FAILURE;
    }
}