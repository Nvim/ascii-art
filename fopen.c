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

struct info{
    int rowsize; //taille en octets de chaque row
    int pixels_nb; //nombre total de pixels sur l'image
    int padding_total; //nombre total d'octets de padding: pixelarray_size - le nombre d'octets alloués a des pixels
    int padding_row; //nombre d'octets de padding par row: nombre d'octets de padding total / nombre de rows
    int pixels_row; //nombre total de pixels par row
};

struct pixel{
    unsigned char b, g, r;
};
typedef struct bmp_header bmp_header;
typedef struct dib_header dib_header;
typedef struct info info;
typedef struct pixel pixel;

//lit les infos du header BMP(14 premiers octets du fichier), et retourne les valeurs trouvées sous forme de structure:
bmp_header read_bmp(char *img, bmp_header bmp_header){
    FILE * fp = fopen(img,"rb");
    if (fp == NULL){
        printf("\nerreur");
        exit(EXIT_FAILURE);
    }
    //printf("\nOuverture reussie: \n");

    //on lit pas tout le struct d'un coup a cause du nom en char
    fread(bmp_header.name,2,1,fp);
    //on devrait lire tout ça d'un coup avec 2*sizeof(int)+2*sizeof(short int) = 3*sizeof(int) ?
    fread(&bmp_header.size,4,1,fp);
    fread(&bmp_header.app1,2,1,fp);
    fread(&bmp_header.app2,2,1,fp);
    fread(&bmp_header.offset,4,1,fp);

    fclose(fp);
    return(bmp_header);
}

//lit les infos du header DIB(octets 14 à 53), et retourne les valeurs trouvées dans une structure:
dib_header read_dib(char *img, dib_header dib_header){
    FILE * fp = fopen(img,"rb");
    if (fp == NULL){
        printf("\nerreur");
        exit(EXIT_FAILURE);
    }
    //printf("\nOuverture reussie: \n");
    fseek(fp, 14, SEEK_SET);
    fread(&dib_header,sizeof(struct dib_header),1,fp);

    fclose(fp);
    return(dib_header);
} 

//infos utiles sur le pixel array et le padding (essentielles pour lire les valeurs des pixels):
info get_info(bmp_header bmp_header, dib_header dib_header){
    struct info info;

    info.rowsize = ((dib_header.bpp*dib_header.width+31)/32)*4; //taille en octets de chaque row
    info.pixels_nb = dib_header.width * dib_header.height;  //nombre total de pixels sur l'image
    info.padding_total = dib_header.pixelarray_size - (info.pixels_nb*3); //nombre total d'octets de padding: pixelarray_size - le nombre d'octets alloués a des pixels
    info.padding_row = info.padding_total / dib_header.height; //nombre d'octets de padding par row: nombre d'octets de padding total / nombre de rows
    info.pixels_row = (info.rowsize - info.padding_row)/3;
    return(info);
}

//affiche les infos des 2 headers et du struct info (debug):
void print_info(bmp_header bmp_header, dib_header dib_header, info info){
    //BMP:
    printf("\nType d'image: %c%c", bmp_header.name[0],bmp_header.name[1]);
    printf("\nTaille de l'image (header inclus): %d\nOffset (nombre d'octets avant le premier pixel): %d\n",bmp_header.size,bmp_header.offset);

    //DIB:
    printf("\nTaille du header DIB: %d\nLargeur: %dpx\nHauteur: %dpx\nColor Planes: %d\nBits par pixel: %d\nMethode de compression: %d\nTaille de l'image sans header(PixelArraySize): %d\n", dib_header.header_size, dib_header.width,dib_header.height,dib_header.colorplanes,dib_header.bpp,dib_header.compression,dib_header.pixelarray_size);

    //info:
    printf("\nTaille de chaque row: %d\nNombre total de pixels: %d\nNombre d'octets de padding: %d\nPadding par row: %d\nPixels par row: %d\n", info.rowsize,info.pixels_nb,info.padding_total,info.padding_row,info.pixels_row);
}

//affiche le pixel array octet par octet:
void print_array(char *img, bmp_header bmp_header, dib_header dib_header, info info, pixel pixel){
    FILE * fp = fopen(img,"rb");
    if (fp == NULL){
        printf("\nerreur");
        exit(EXIT_FAILURE);
    }

    fseek(fp, bmp_header.offset, SEEK_SET);
    printf("\nPixel Array: \n"); //affichage des octets 1 par 1
    for(int i = 1; i<=dib_header.height; i++){  //ligne i
        for (int j = 1; j<= info.pixels_row; j++){  //pixel j
            fread(&pixel, 3, 1, fp);
            printf("%d | %d | %d | ", pixel.b,pixel.g,pixel.r);
        }
        fseek(fp, info.padding_row, SEEK_CUR);//padding en fin de ligne
        for(int k = 0; k<2; k++){
            printf("* | ");
        }
    }
    fclose(fp);
}

//Affiche le pixel array octet par octet depuis la fin (l'affichage en ascii est calqué sur cette fonction):
void print_array_reverse(char *img, bmp_header bmp_header, dib_header dib_header, info info, pixel pixel){
    FILE * fp = fopen(img,"rb");
    if (fp == NULL){
        printf("\nerreur");
        exit(EXIT_FAILURE);
    }

    fseek(fp, 0, SEEK_END);
    printf("Pixel Array reverse: \n");
    for(int i = 1; i<=dib_header.height; i++){
        fseek(fp, -info.padding_row, SEEK_CUR);
        for(int k = 0; k<info.padding_row; k++){
            printf("* | ");
        }
        for(int j = 1; j<=info.pixels_row;j++){
            fseek(fp, -3, SEEK_CUR);
            fread(&pixel, 3, 1, fp);
            printf("%d | %d | %d | ", pixel.r,pixel.g,pixel.b);
            fseek(fp, -3, SEEK_CUR);
        }
    }
    fclose(fp);
}

//affiche le pixel array pixel par pixel:
void print_pixels(char *img, bmp_header bmp_header, dib_header dib_header, info info, pixel pixel){
    FILE * fp = fopen(img,"rb");
    if (fp == NULL){
        printf("\nerreur");
        exit(EXIT_FAILURE);
    }
    
    fseek(fp, bmp_header.offset, SEEK_SET);
    printf("\n\nAffichage par pixel:\n");
    int p = 1;
    for(int i = 1; i<=dib_header.height; i++){
        for (int j = 1; j<= info.pixels_row; j++){
            fread(&pixel, 3, 1, fp);
            printf("Pixel %d: %d, %d, %d\n",p, pixel.b,pixel.g,pixel.r);
            p++;
        }
        fseek(fp, info.padding_row, SEEK_CUR);
    }
    fclose(fp);
}

//Affiche un caractère par pixel:
void print_ascii(char *img, bmp_header bmp_header, dib_header dib_header, info info, pixel pixel){
    FILE * fp = fopen(img,"rb");
    if (fp == NULL){
        printf("\nerreur");
        exit(EXIT_FAILURE);
    }
    printf("\n");
    fseek(fp, 0, SEEK_END);
    for(int i = 1; i<=dib_header.height; i++){
        fseek(fp, -info.padding_row, SEEK_CUR);
        for(int j = 1; j<=info.pixels_row;j++){
            fseek(fp, -3, SEEK_CUR);
            fread(&pixel, 3, 1, fp);
            float avg = pixel.r*0.33 + pixel.g*0.5 + pixel.b*0.16;
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
            fseek(fp, -3, SEEK_CUR);
        }
        printf("\n");
    }
    fclose(fp);
}

int main(void){

    bmp_header bmp_header;
    dib_header dib_header; 
    info info;
    pixel pixel;

    if(sizeof(char) == 1 && sizeof(short int) == 2 && sizeof(int) == 4){
        
        //On lit les valeurs des headers et on calcule les infos utiles grace aux fonctions:
        bmp_header = read_bmp("zoro.bmp",bmp_header);
        dib_header = read_dib("zoro.bmp", dib_header);
        info = get_info(bmp_header, dib_header);

        print_info(bmp_header, dib_header, info);
        printf("\n");
        //print_array("oui.bmp", bmp_header, dib_header, info, pixel);
        //print_pixels("oui.bmp", bmp_header, dib_header, info, pixel);
        //print_array_reverse("oui.bmp", bmp_header, dib_header, info, pixel);
        print_ascii("zoro.bmp", bmp_header, dib_header, info, pixel);

        return EXIT_SUCCESS;
    }
    else{
        printf("\nTaille des variables pas adaptée");
        return EXIT_FAILURE;
    }
}