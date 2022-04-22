#include <stdio.h>
#include <stdlib.h>

struct bmp_header
{                            // correspond aux 14 premiers octets
    unsigned char name[2];   // BM
    unsigned int size;       // taille de l'image au total (headers inclus)
    unsigned short int app1; // on s'en fout mais c'est 0
    unsigned short int app2; // pareil
    unsigned int offset;     // octet du premier pixel= 54 pour les 24bits(16 777 216 couleurs), 1078 pour les 8bits(256 couleurs) 118 pour les 4bits(16 couleurs)
};

struct dib_header
{                             // correspond aux 40 octets suivants
    unsigned int header_size; // taille du header dib (40 octets)
    unsigned int width;
    unsigned int height;
    unsigned short int colorplanes; // pas compris mais c'est 1
    unsigned short int bpp;         // bits par pixel, 8 = noir et blanc 24 = RGB
    unsigned int compression;       // méthode de compression utilisée
    unsigned int pixelarray_size;   // taille de l'image (headers exclus) = pixelarraysize
};

struct info
{
    int rowsize;       // taille en octets de chaque row
    int pixels_nb;     // nombre total de pixels sur l'image
    int padding_total; // nombre total d'octets de padding: pixelarray_size - le nombre d'octets alloués a des pixels
    int padding_row;   // nombre d'octets de padding par row: nombre d'octets de padding total / nombre de rows
    int pixels_row;    // nombre total de pixels par row
};

struct pixel
{
    unsigned char b, g, r;
};
typedef struct bmp_header bmp_header;
typedef struct dib_header dib_header;
typedef struct info info;
typedef struct pixel pixel;

// lit les infos du header BMP(14 premiers octets du fichier), et retourne les valeurs trouvées sous forme de structure:
bmp_header read_bmp(char *img, bmp_header bmp_header)
{
    FILE *fp = fopen(img, "rb");
    if (fp == NULL)
    {
        printf("\nerreur");
        exit(EXIT_FAILURE);
    }
    // printf("\nOuverture reussie: \n");

    // on lit pas tout le struct d'un coup a cause du nom en char
    fread(bmp_header.name, 2, 1, fp);
    // on devrait lire tout ça d'un coup avec 2*sizeof(int)+2*sizeof(short int) = 3*sizeof(int) ?
    fread(&bmp_header.size, 4, 1, fp);
    fread(&bmp_header.app1, 2, 1, fp);
    fread(&bmp_header.app2, 2, 1, fp);
    fread(&bmp_header.offset, 4, 1, fp);

    fclose(fp);
    return (bmp_header);
}

// lit les infos du header DIB(octets 14 à 53), et retourne les valeurs trouvées dans une structure:
dib_header read_dib(char *img, dib_header dib_header)
{
    FILE *fp = fopen(img, "rb");
    if (fp == NULL)
    {
        printf("\nerreur");
        exit(EXIT_FAILURE);
    }
    // printf("\nOuverture reussie: \n");
    fseek(fp, 14, SEEK_SET);                              // on place le curseur au 14eme octet avant la lecture des valeurs
    fread(&dib_header, sizeof(struct dib_header), 1, fp); // on lit tout le struct d'un coup avec 1 seul fread

    fclose(fp);
    return (dib_header);
}

// infos utiles sur le pixel array et le padding (essentielles pour lire les valeurs des pixels):
info get_info(bmp_header bmp_header, dib_header dib_header)
{
    struct info info;

    info.rowsize = ((dib_header.bpp * dib_header.width + 31) / 32) * 4;     // taille en octets de chaque row
    info.pixels_nb = dib_header.width * dib_header.height;                  // nombre total de pixels sur l'image
    info.padding_total = dib_header.pixelarray_size - (info.pixels_nb * 3); // nombre total d'octets de padding: pixelarray_size - le nombre d'octets alloués a des pixels
    info.padding_row = info.padding_total / dib_header.height;              // nombre d'octets de padding par row: nombre d'octets de padding total / nombre de rows
    info.pixels_row = (info.rowsize - info.padding_row) / 3;
    return (info);
}

// affiche les infos des 2 headers et du struct info (debug):
void print_info(bmp_header bmp_header, dib_header dib_header, info info)
{
    // BMP:
    printf("\nType d'image: %c%c", bmp_header.name[0], bmp_header.name[1]);
    printf("\nTaille de l'image (headers inclus): %d\nOffset (nombre d'octets avant le premier pixel): %d\n", bmp_header.size, bmp_header.offset);

    // DIB:
    printf("\nTaille du header DIB: %d\nLargeur: %dpx\nHauteur: %dpx\nColor Planes: %d\nBits par pixel: %d\nMethode de compression: %d\nTaille de l'image sans headers(PixelArraySize): %d\n", dib_header.header_size, dib_header.width, dib_header.height, dib_header.colorplanes, dib_header.bpp, dib_header.compression, dib_header.pixelarray_size);

    // info:
    printf("\nTaille de chaque row: %d\nNombre total de pixels: %d\nNombre d'octets de padding: %d\nPadding par row: %d\nPixels par row: %d\n", info.rowsize, info.pixels_nb, info.padding_total, info.padding_row, info.pixels_row);
}

// affiche le pixel array octet par octet:
void print_array(char *img, bmp_header bmp_header, dib_header dib_header, info info, pixel pixel)
{
    FILE *fp = fopen(img, "rb");
    if (fp == NULL)
    {
        printf("\nerreur");
        exit(EXIT_FAILURE);
    }

    fseek(fp, bmp_header.offset, SEEK_SET);
    printf("\nPixel Array: \n"); // affichage des octets 1 par 1
    for (int i = 1; i <= dib_header.height; i++)
    { // ligne i
        for (int j = 1; j <= info.pixels_row; j++)
        { // pixel j
            fread(&pixel, 3, 1, fp);
            printf("%d | %d | %d | ", pixel.b, pixel.g, pixel.r);
        }
        fseek(fp, info.padding_row, SEEK_CUR); // padding à la fin de chaque ligne, on avance le curseur de padding_row pour préparer la prochaine lecture et on print autant de 0 qu'il y a de padding_row
        for (int k = 0; k < info.padding_row; k++)
        {
            printf("* | ");
        }
    }
    fclose(fp);
}

// Affiche le pixel array octet par octet depuis la fin (l'affichage en ascii est calqué sur cette fonction):
void print_array_reverse(char *img, bmp_header bmp_header, dib_header dib_header, info info, pixel pixel)
{
    FILE *fp = fopen(img, "rb");
    if (fp == NULL)
    {
        printf("\nerreur");
        exit(EXIT_FAILURE);
    }

    fseek(fp, 0, SEEK_END);//on commence avec le curseur au dernier octet du fichier (pixel en haut a droite)
    printf("Pixel Array reverse: \n");
    for (int i = 1; i <= dib_header.height; i++)// chaque tour de cette boucle print tous les pixels d'une ligne et le padding, on arrete donc i à dib_header.height, le nombre de lignes
    {
        fseek(fp, -info.padding_row, SEEK_CUR); //on déplace le curseur de -padding_row car on part de la fin et que le padding est en fin de ligne
        for (int k = 0; k < info.padding_row; k++) //on print les octets de padding
        {
            printf("* | ");
        }
        for (int j = 1; j <= info.pixels_row; j++)//cette boucle affiche tous les pixels d'une ligne, chaque tour affiche un pixel, il y a donc autant de tours que de pixels par ligne
        {
            fseek(fp, -3, SEEK_CUR);//on recule le curseur de 3 octets pour lire le pixel
            fread(&pixel, 3, 1, fp); //on lit le pixel normalement
            printf("%d | %d | %d | ", pixel.r, pixel.g, pixel.b); //on affiche les 3 octets du pixel dans l'ordre RGB au lieu de BGR car on affiche le pixel array à l'envers
            fseek(fp, -3, SEEK_CUR); //on recule le curseur de 3 octets car fread le fait avancer, le curseur est alors à l'endroit ou il était avant la lecture du pixel
        }
    }
    fclose(fp);
}

// affiche le pixel array pixel par pixel (utilisée pour les tests sur petites images):
void print_pixels(char *img, bmp_header bmp_header, dib_header dib_header, info info, pixel pixel)
{
    FILE *fp = fopen(img, "rb");
    if (fp == NULL)
    {
        printf("\nerreur");
        exit(EXIT_FAILURE);
    }

    fseek(fp, bmp_header.offset, SEEK_SET);//on place le curseur au début du pixel array
    printf("\n\nAffichage par pixel:\n");
    int p = 1; //variable externe aux boucles pour compter le nombre de pixels lus et affichés
    for (int i = 1; i <= dib_header.height; i++)//chaque tour affiche tous les pixels d'une ligne et avance le curseur pour sauter les octets de padding, il y a donc autant de tours que de lignes
    {
        for (int j = 1; j <= info.pixels_row; j++) //chaque tour affiche tous les pixels d'une ligne
        {
            fread(&pixel, 3, 1, fp);
            printf("Pixel %d: %d, %d, %d\n", p, pixel.b, pixel.g, pixel.r); //on affiche le p-ème pixel
            p++; //on incrémente p après chaque pixel
        }
        fseek(fp, info.padding_row, SEEK_CUR);//après l'affichage d'une ligne, on déplace le curseur pour sauter les octets de padding 
    }
    fclose(fp);
}

//initialise une matrice
pixel** matrix(dib_header dib_header){
    int i, j;

    pixel ** matrice = (pixel **)malloc(dib_header.width*sizeof(pixel));
        for (i = 0; i<dib_header.width; i++){
            matrice[i] = (pixel *)malloc(dib_header.height*sizeof(pixel));
        }
    return matrice;
}

//remplit la matrice avec les valeurs des pixels:
pixel ** fill_matrix(char *img, bmp_header bmp_header, dib_header dib_header, info info, pixel pixel, struct pixel ** matrice){

    FILE *fp = fopen(img, "rb");
    if (fp == NULL)
    {
        printf("\nerreur");
        exit(EXIT_FAILURE);
    }

    matrice = matrix(dib_header);

    fseek(fp, bmp_header.offset, SEEK_SET);
    for (int i = 0; i < dib_header.height; i++){ // ligne i
        for (int j = 0; j < info.pixels_row; j++){ // pixel j
            fread(&pixel, 3, 1, fp);
            matrice [i][j] = pixel;
        }
        fseek(fp, info.padding_row, SEEK_CUR); // padding à la fin de chaque ligne, on avance le curseur de padding_row pour préparer la prochaine lecture et on print autant de 0 qu'il y a de padding_row
    }
    fclose(fp);
    return matrice;
}

//affiche une matrice:
void print_matrice(char *img, bmp_header bmp_header, dib_header dib_header, info info, pixel pixel, struct pixel ** matrice){
    matrice = fill_matrix(img, bmp_header, dib_header, info, pixel, matrice);
    int i,j;

    for (i = 0; i<dib_header.width; i++){
        for(j = 0; j<dib_header.height; j++){
            printf("%d %d %d | ", matrice[i][j].b, matrice[i][j].g, matrice[i][j].r);
        }
        printf("\n");
    }
}


// Affiche un caractère par pixel:
//(même fonction que print_array_reverse mais on print un caractère en fonction de la clarté d'un pixel au lieu de ses valeurs de rgb)
void print_ascii(char *img, bmp_header bmp_header, dib_header dib_header, info info, pixel pixel)
{
    FILE *fp = fopen(img, "rb");
    if (fp == NULL)
    {
        printf("\nerreur");
        exit(EXIT_FAILURE);
    }
    printf("\n");
    float avg; //clarté du pixel
    fseek(fp, 0, SEEK_END);
    for (int i = 1; i <= dib_header.height; i++)
    {
        fseek(fp, -info.padding_row, SEEK_CUR);
        for (int j = 1; j <= info.pixels_row; j++)
        {
            fseek(fp, -3, SEEK_CUR);
            fread(&pixel, 3, 1, fp);
            avg = pixel.r * 0.33 + pixel.g * 0.5 + pixel.b * 0.16; //calcul de la luminosité/clarté du pixel
            if (avg < 25)
            {
                printf(" ");
            }
            else if (avg < 50)
            {
                printf(".");
            }
            else if (avg < 50)
            {
                printf(":");
            }
            else if (avg < 75)
            {
                printf("-");
            }
            else if (avg < 100)
            {
                printf("=");
            }
            else if (avg < 125)
            {
                printf("+");
            }
            else if (avg < 150)
            {
                printf("*");
            }
            else if (avg < 175)
            {
                printf("#");
            }
            else if (avg < 200)
            {
                printf("%");
            }
            else if (avg < 225)
            {
                printf("@");
            }
            else
            {
                printf("$");
            }
            fseek(fp, -3, SEEK_CUR);
        }
        printf("\n");
    }
    fclose(fp);
}

//retourne un pixel en noir et blanc:
pixel gray_pixel(pixel pixel){
    pixel.r = pixel.r * 0.33 + pixel.g * 0.5 + pixel.b * 0.16;
    pixel.b = pixel.r * 0.33 + pixel.g * 0.5 + pixel.b * 0.16;
    pixel.g = pixel.r * 0.33 + pixel.g * 0.5 + pixel.b * 0.16;

    return pixel;
}
void gray_img(char *img, bmp_header bmp_header, dib_header dib_header, info info, pixel pixel, struct pixel ** matrice){
    int i,j;
    matrice = fill_matrix(img, bmp_header, dib_header, info, pixel, matrice);
    for(i = 0; i<dib_header.width; i++){
        //gray_pixel à chaque pixel
    }

}



int main(void)
{
    //déclaration des 4 structures:
    bmp_header bmp_header;
    dib_header dib_header;
    info info;
    pixel pixel;

    if (sizeof(char) == 1 && sizeof(short int) == 2 && sizeof(int) == 4)
    {

        // On remplit les structures grâce aux fonctions:
        bmp_header = read_bmp("small.bmp", bmp_header);
        dib_header = read_dib("small.bmp", dib_header);
        info = get_info(bmp_header, dib_header);

        //print_info(bmp_header, dib_header, info); //affichage des infos des 2 headers et du struct info (debug)
        printf("\n");
        struct pixel ** matrice = matrix(dib_header);
        fill_matrix("small.bmp", bmp_header, dib_header, info, pixel, matrice);
        print_matrice("small.bmp", bmp_header, dib_header, info, pixel, matrice);
        // prints_array("oui.bmp", bmp_header, dib_header, info, pixel);
        // print_pixels("oui.bmp", bmp_header, dib_header, info, pixel);
        // print_array_reverse("oui.bmp", bmp_header, dib_header, info, pixel);
        //print_ascii("musashi.bmp", bmp_header, dib_header, info, pixel);

        return EXIT_SUCCESS;
    }
    else
    {
        printf("\nTaille des variables pas adaptée");
        return EXIT_FAILURE;
    }
}