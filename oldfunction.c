#include <stdio.h>
#include <stdlib.h>

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
    int rowsize;
    int pixels_nb;
    int padding_total;
    int padding_row;
    int pixels_row;
};

struct pixel{
    unsigned char b, g, r;
};
typedef struct bmp_header bmp_header;
typedef struct dib_header dib_header;
typedef struct info info;
typedef struct pixel pixel;



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
        for (int j = 0; j < dib_header.width; j++){ // pixel j
            fread(&pixel, 3, 1, fp);
            matrice[i][j] = pixel;
        }
        fseek(fp, info.padding_row, SEEK_CUR); // padding à la fin de chaque ligne, on avance le curseur de padding_row pour préparer la prochaine lecture
    }
    fclose(fp);
    return matrice;
}

//affiche une matrice:
void print_matrice(char *img, bmp_header bmp_header, dib_header dib_header, info info, pixel pixel, struct pixel ** matrice){
    matrice = fill_matrix(img, bmp_header, dib_header, info, pixel, matrice);
    int i,j;

    for (i = 0; i<dib_header.height; i++){
        for(j = 0; j<dib_header.width; j++){
            printf("%d %d %d | ", matrice[i][j].b, matrice[i][j].g, matrice[i][j].r);
        }
        printf("\n");
    }
}












void printt_info(char *img){
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
    
    /*fseek(fp, -padding_row, SEEK_END);
    fseek(fp, -3, SEEK_CUR);
    fread(&pixel, 3, 1, fp);
    printf("\n%d | %d | %d | ", pixel.b,pixel.g,pixel.r);


    fseek(fp, -6, SEEK_CUR);
    fread(&pixel, 3, 1, fp);
    printf("\n%d | %d | %d | ", pixel.b,pixel.g,pixel.r);

    fseek(fp, -padding_row, SEEK_CUR);
    fseek(fp, -6, SEEK_CUR);
    fread(&pixel, 3, 1, fp);
    printf("\n%d | %d | %d | ", pixel.b,pixel.g,pixel.r);

    fseek(fp, -6, SEEK_CUR);
    fread(&pixel, 3, 1, fp);
    printf("\n%d | %d | %d | ", pixel.b,pixel.g,pixel.r);*/


    //Affichage octet par octet depuis la fin
    fseek(fp, 0, SEEK_END);
    for(int i = 1; i<=dib_header.height; i++){
        fseek(fp, -padding_row, SEEK_CUR);
        for(int k = 0; k<padding_row; k++){
            printf("* | ");
        }
        for(int j = 1; j<=pixels_row;j++){
            fseek(fp, -3, SEEK_CUR);
            fread(&pixel, 3, 1, fp);
            printf("%d | %d | %d | ", pixel.b,pixel.g,pixel.r);
            fseek(fp, -3, SEEK_CUR);
        }
    }
    
    printf("\n");
    //ascii dans l'ordre:
    fseek(fp, 0, SEEK_END);
    for(int i = 1; i<=dib_header.height; i++){
        fseek(fp, -padding_row, SEEK_CUR);
        for(int j = 1; j<=pixels_row;j++){
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
    
    fseek(fp, bmp_header.offset, SEEK_SET);
    for(int i = 1; i<=dib_header.height; i++){
        for (int j = 1; j<= pixels_row; j++){
            fread(&pixel, 3, 1, fp);
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
    }
}


// Affiche un caractère par pixel:
//(même fonction que print_array_reverse mais on print un caractère en fonction de la clarté d'un pixel au lieu de ses valeurs de rgb)
void print_ascii_old(char *img, bmp_header bmp_header, dib_header dib_header, info info, pixel pixel)
{
    FILE *fp = fopen(img, "rb");
    if (fp == NULL)
    {
        printf("\nerreur");
        exit(EXIT_FAILURE);
    }
    printf("\n");
    float avg; // clarté du pixel
    fseek(fp, 0, SEEK_END);
    for (int i = 1; i <= dib_header.height; i++)
    {
        fseek(fp, -info.padding_row, SEEK_CUR);
        for (int j = 1; j <= info.pixels_row; j++)
        {
            fseek(fp, -3, SEEK_CUR);
            fread(&pixel, 3, 1, fp);
            avg = pixel.r * 0.33 + pixel.g * 0.5 + pixel.b * 0.16; // calcul de la luminosité/clarté du pixel
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

void ascii_10char(dib_header dib_header, pixel **matrice)
{
    int i, j, h, w;
    unsigned char charr;
    char ascii[] = {'@', '%', '#', '*', '+', '=', '-', ':', '.', ' '};

    for (i = 0; i < dib_header.height; i++)
    {
        for (j = dib_header.width - 1; j >= 0; j--)
        {
            charr = gray_pixel(matrice[i][j]);
            printf("%c", ascii[9 - charr / 26]);
        }
        printf("\n");
    }
}

void ascii_16char(dib_header dib_header, pixel **matrice)
{
    int i, j, h, w;
    unsigned char charr;
    char ascii[] = {'$', '8', '#', 'h', 'Z', 'L', 'Y', 'v', 'r', '/', '{', '_', '>', 'I', '^', ' '};
    printf("\nReduire la hauteur? (1: non; 2: 2px; 3: 4px)\n");
    scanf("%d", &h);
    if (h != 1 && h != 2 && h != 3)
    {
        printf("\n*****\tReponse incorrecte, veuillez entrer une valeur reconnue!\t*****\n\n");
        do
        {
            printf("\nReduire la hauteur? (1: non; 2: 2px; 3: 4px)\n");
            scanf("%d", &h);
        } while (h != 1 && h != 2 && h != 3);
    }

    printf("\nReduire la largeur? (1: non; 2: 2px; 3: 4px)\n");
    scanf("%d", &w);
    if (w != 1 && w != 2 && w != 3)
    {
        printf("\n*****\tReponse incorrecte, veuillez entrer une valeur reconnue!\t*****\n\n");
        do
        {
            printf("\nReduire la largeur? (1: non; 2: 2px; 3: 4px)\n");
            scanf("%d", &h);
        } while (w != 1 && w != 2 && w != 3);
    }
    if (h == 3)
        h = 4;
    if (w == 3)
        w == 4;

    for (i = 0; i < dib_header.height; i = i + h)
    {
        for (j = dib_header.width - 1; j >= 0; j = j - w)
        {
            charr = gray_pixel(matrice[i][j]);
            printf("%c", ascii[9 - charr / 16]);
        }
        printf("\n");
    }
}

void ascii_64char(dib_header dib_header, pixel **matrice)
{
    int i, j, h, w;
    unsigned char charr;
    char ascii[] = {'$', '@', 'B', '%', '8', '&', 'W', 'M', '#', '*', 'o', 'a', 'h', 'k', 'b', 'd', 'p', 'q', 'w', 'm', 'Z', 'O', '0', 'Q', 'L', 'C', 'J', 'U', 'Y', 'X', 'z', 'c', 'v', 'u', 'n', 'x', 'r', 'j', 'f', 't', '/', '|', '(', '1', '{', '[', '?', '-', '_', '+', '~', '<', '>', 'i', '!', 'l', 'I', ';', ':', '"', '^', '`', '.', ' '};

    printf("\nReduire la hauteur? (1: non; 2: 2px; 3: 4px)\n");
    scanf("%d", &h);
    if (h != 1 && h != 2 && h != 3)
    {
        printf("\n*****\tReponse incorrecte, veuillez entrer une valeur reconnue!\t*****\n\n");
        do
        {
            printf("\nReduire la hauteur? (1: non; 2: 2px; 3: 4px)\n");
            scanf("%d", &h);
        } while (h != 1 && h != 2 && h != 3);
    }

    printf("\nReduire la largeur? (1: non; 2: 2px; 3: 4px)\n");
    scanf("%d", &w);
    if (w != 1 && w != 2 && w != 3)
    {
        printf("\n*****\tReponse incorrecte, veuillez entrer une valeur reconnue!\t*****\n\n");
        do
        {
            printf("\nReduire la largeur? (1: non; 2: 2px; 3: 4px)\n");
            scanf("%d", &h);
        } while (w != 1 && w != 2 && w != 3);
    }
    if (h == 3)
        h = 4;
    if (w == 3)
        w == 4;

    for (i = 0; i < dib_header.height; i = i + h)
    {
        for (j = dib_header.width - 1; j >= 0; j = j - w)
        {
            charr = gray_pixel(matrice[i][j]);
            printf("%c", ascii[63 - charr / 4]);
        }
        printf("\n");
    }
}


















// alloue dynamiquement une matrice de struct pixels (3 octets par case):
pixel **matrix(dib_header dib_header)
{
    int i, j;

    pixel **matrice;
    matrice = (pixel **)malloc(dib_header.width * sizeof(pixel *));
    for (i = 0; i < dib_header.width; i++)
    {
        matrice[i] = (pixel *)malloc(dib_header.height * sizeof(pixel));
    }
    return matrice;
}

// retourne le pixel array sous forme de matrice de pixels (chaque case = 1 pixel)
pixel **fill_matrix(char *img, bmp_header bmp_header, dib_header dib_header, info info)
{
    int i, j;
    pixel px;
    pixel **matrice = matrix(dib_header);

    FILE *fp = fopen(img, "rb");
    if (fp == NULL)
    {
        printf("\nerreur");
        exit(EXIT_FAILURE);
    }

    fseek(fp, bmp_header.offset, SEEK_CUR);
    for (i = 0; i < dib_header.height; i++)
    { // ligne i -> i= hauteur
        for (j = 0; j < dib_header.width; j++)
        { // pixel j -> j = largeur
            fread(&px, 3, 1, fp);
            matrice[i][j] = px;
            printf("(%d,%d): %d-%d-%d | ", i, j, matrice[i][j].b, matrice[i][j].g, matrice[i][j].r);
            if (i == 5) printf("%d", dib_header.height);
        }
        fseek(fp, info.padding_row, SEEK_CUR);
        printf("\n");
    }

    printf("\nOK\n%d %d %d\n\n", matrice[0][13].b, matrice[0][13].g, matrice[0][13].r);

    for (i = 0; i < dib_header.height; i++)
    { // ligne i -> i= hauteur
        for (j = 0; j < dib_header.width; j++)
        { // pixel j -> j = largeur
            printf("(%d,%d): %d-%d-%d | ", i, j, matrice[i][j].b, matrice[i][j].g, matrice[i][j].r);
        }
        printf("\n");
    }

    fclose(fp);
    return matrice;
}


// affiche le pixel array sous forme de matrice de pixels (chaque case = 1 pixel)
void print_matrix(char *img, bmp_header bmp_header, dib_header dib_header, info info, pixel **matrice)
{
    int i, j;
    //matrice = fill_matrix(img, bmp_header, dib_header, info);

    for (int i = 1; i <= dib_header.height; i++)
    { // ligne i
        for (int j = 1; j <= info.pixels_row; j++)
        { // pixel j
            printf("(%d,%d,%d) ", matrice[i][j].b, matrice[i][j].g, matrice[i][j].r);
        }
        printf("\n");
    }
}






// applique gray_pixel a tous les pixels de la matrice:
pixel **gray_matrice(char *img, dib_header dib_header, pixel **matrice)
{
    int i, j;
    for (i = 0; i < dib_header.height; i++)
    {
        for (j = 0; j < dib_header.width; j++)
        {
            matrice[i][j].r = gray_pixel(matrice[i][j]);
            matrice[i][j].g = gray_pixel(matrice[i][j]);
            matrice[i][j].b = gray_pixel(matrice[i][j]);
        }
    }
    return matrice;
}

void gray_image(char *img, bmp_header bmp_header, dib_header dib_header, info info, struct pixel **matrice)
{

    FILE *fw = fopen("griss.bmp", "w");
    if (fw == NULL)
    {
        printf("\nerreurz\n\n");
        exit(EXIT_FAILURE);
    }

    unsigned char pad = 0;

    // ecriture header bmp puis dib:
    fwrite(bmp_header.name, 2, 1, fw);
    fwrite(&bmp_header, 3 * (sizeof(int)), 1, fw);
    fwrite(&dib_header, dib_header.header_size, 1, fw);

    gray_matrice(img, dib_header, matrice); // on met tout le pixel array en noir et blanc

    for (int i = dib_header.height - 1; i > 0; i--)
    {
        for (int j = dib_header.width - 1; j > 0; j--)
        {
            fwrite(&matrice[i][j], 3, 1, fw);
        }
        /*for(int k = 0; k<info.padding_row; k++){
            fwrite(&pad, 1, 1, fw);
        }*/
    }
    fclose(fw);
}


void ascii_8char(dib_header dib_header, pixel **matrice)
{
    int i, j, h, w;
    unsigned char charr;
    char ascii[] = {'@', '$', '#', '+', ':', '-', '.', ' '};

    printf("\nReduire la hauteur? (1: non; 2: 2px; 3: 4px)\n");
    scanf("%d", &h);
    if (h != 1 && h != 2 && h != 3)
    {
        printf("\n*****\tReponse incorrecte, veuillez entrer une valeur reconnue!\t*****\n\n");
        do
        {
            printf("\nReduire la hauteur? (1: non; 2: 2px; 3: 4px)\n");
            scanf("%d", &h);
        } while (h != 1 && h != 2 && h != 3);
    }

    printf("\nReduire la largeur? (1: non; 2: 2px; 3: 4px)\n");
    scanf("%d", &w);
    if (w != 1 && w != 2 && w != 3)
    {
        printf("\n*****\tReponse incorrecte, veuillez entrer une valeur reconnue!\t*****\n\n");
        do
        {
            printf("\nReduire la largeur? (1: non; 2: 2px; 3: 4px)\n");
            scanf("%d", &h);
        } while (w != 1 && w != 2 && w != 3);
    }
    if (h == 3)
        h = 4;
    if (w == 3)
        w == 4;

    for (i = 0; i < dib_header.height; i = i + h)
    {
        for (j = dib_header.width - 1; j >= 0; j = j - w)
        {
            charr = gray_pixel(matrice[i][j]);
            printf("%c", ascii[7 - charr / 32]);
        }
        printf("\n");
    }
}

void ascii_8char_negative(dib_header dib_header, pixel **matrice)
{
    int i, j, h, w;
    unsigned char charr;
    char ascii[] = {'@', '$', '#', '+', ':', '-', '.', ' '};

    printf("\nReduire la hauteur? (1: non; 2: 2px; 3: 4px)\n");
    scanf("%d", &h);
    if (h != 1 && h != 2 && h != 3)
    {
        printf("\n*****\tReponse incorrecte, veuillez entrer une valeur reconnue!\t*****\n\n");
        do
        {
            printf("\nReduire la hauteur? (1: non; 2: 2px; 3: 4px)\n");
            scanf("%d", &h);
        } while (h != 1 && h != 2 && h != 3);
    }

    printf("\nReduire la largeur? (1: non; 2: 2px; 3: 4px)\n");
    scanf("%d", &w);
    if (w != 1 && w != 2 && w != 3)
    {
        printf("\n*****\tReponse incorrecte, veuillez entrer une valeur reconnue!\t*****\n\n");
        do
        {
            printf("\nReduire la largeur? (1: non; 2: 2px; 3: 4px)\n");
            scanf("%d", &h);
        } while (w != 1 && w != 2 && w != 3);
    }
    if (h == 3)
        h = 4;
    if (w == 3)
        w == 4;

    for (i = 0; i < dib_header.height; i = i + h)
    {
        for (j = dib_header.width - 1; j >= 0; j = j - w)
        {
            charr = gray_pixel(matrice[i][j]);
            printf("%c", ascii[charr / 32]);
        }
        printf("\n");
    }
}



int main(){
    printt_info("oui.bmp");
}