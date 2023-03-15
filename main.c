#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct bmp_header
{                            // correspond aux 14 premiers octets
    unsigned char name[2];   // BM
    unsigned int size;       // taille de l'image au total (headers inclus)
    unsigned short int app1; // toujours 0
    unsigned short int app2; // pareil
    unsigned int offset;     // octet du premier pixel= 54 pour les 24bits(16 777 216 couleurs), 1078 pour les 8bits(256 couleurs) 118 pour les 4bits(16 couleurs)
};

struct dib_header
{                             // correspond aux 40 octets suivants
    unsigned int header_size; // taille du header dib (40 octets)
    unsigned int width;
    unsigned int height;
    unsigned short int colorplanes; // Color planes: toujours 1
    unsigned short int bpp;         // bits par pixel, ici 24
    unsigned int compression;       // méthode de compression utilisée
    unsigned int pixelarray_size;   // taille de l'image (headers exclus) = pixelarraysize
};

struct info
{
    int rowsize;       // taille en octets de chaque row (ligne)
    int pixels_nb;     // nombre total de pixels sur l'image
    int padding_total; // nombre total d'octets de padding: pixelarray_size - le nombre d'octets alloués a des pixels
    int padding_row;   // nombre d'octets de padding par row: nombre d'octets de padding total / nombre de rows
    int pixels_row;    // nombre total de pixels par row
};

struct pixel
{
    unsigned char b;
    unsigned char g;
    unsigned char r;
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
        printf("\nErreur: image introuvable, verifiez que l'image que vous voulez manipuler existe\n");
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
        printf("\nErreur: image introuvable verifiez que l'image que vous voulez manipluer existe\n");
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

// Affiche le pixel array octet par octet depuis la fin (la première version de l'affichage en ascii est calqué sur cette fonction):
void print_array_reverse(char *img, bmp_header bmp_header, dib_header dib_header, info info, pixel pixel)
{
    FILE *fp = fopen(img, "rb");
    if (fp == NULL)
    {
        printf("\nerreur");
        exit(EXIT_FAILURE);
    }

    fseek(fp, 0, SEEK_END); // on commence avec le curseur au dernier octet du fichier (pixel en haut a droite)
    printf("Pixel Array reverse: \n");
    for (int i = 1; i <= dib_header.height; i++) // chaque tour de cette boucle print tous les pixels d'une ligne et le padding, on arrete donc i à dib_header.height, le nombre de lignes
    {
        fseek(fp, -info.padding_row, SEEK_CUR);    // on déplace le curseur de -padding_row car on part de la fin et que le padding est en fin de ligne
        for (int k = 0; k < info.padding_row; k++) // on print les octets de padding
        {
            printf("* | ");
        }
        for (int j = 1; j <= info.pixels_row; j++) // cette boucle affiche tous les pixels d'une ligne, chaque tour affiche un pixel, il y a donc autant de tours que de pixels par ligne
        {
            fseek(fp, -3, SEEK_CUR);                              // on recule le curseur de 3 octets pour lire le pixel
            fread(&pixel, 3, 1, fp);                              // on lit le pixel normalement
            printf("%d | %d | %d | ", pixel.r, pixel.g, pixel.b); // on affiche les 3 octets du pixel dans l'ordre RGB au lieu de BGR car on affiche le pixel array à l'envers
            fseek(fp, -3, SEEK_CUR);                              // on recule le curseur de 3 octets car fread le fait avancer, le curseur est alors à l'endroit ou il était avant la lecture du pixel
        }
    }
    fclose(fp);
}

// affiche le pixel array pixel par pixel (utilisée pour les tests sur petites images):
void print_pixels(char *img, bmp_header bmp_header, dib_header dib_header, info info)
{
    struct pixel pixel;
    FILE *fp = fopen(img, "rb");
    if (fp == NULL)
    {
        printf("\nImpossible d'ouvrir l'image (fp == NULL)\n");
        exit(EXIT_FAILURE);
    }

    fseek(fp, bmp_header.offset, SEEK_SET);      // on place le curseur au début du pixel array
    int p = 1;                                   // variable externe aux boucles pour compter le nombre de pixels lus et affichés
    for (int i = 1; i <= dib_header.height; i++) // chaque tour affiche tous les pixels d'une ligne et avance le curseur pour sauter les octets de padding, il y a donc autant de tours que de lignes
    {
        for (int j = 1; j <= info.pixels_row; j++) // chaque tour affiche tous les pixels d'une ligne
        {
            fread(&pixel, 3, 1, fp);
            printf("Pixel %d: %d, %d, %d\n", p, pixel.b, pixel.g, pixel.r); // on affiche le p-ème pixel
            p++;                                                            // on incrémente p après chaque pixel
        }
        fseek(fp, info.padding_row, SEEK_CUR); // après l'affichage d'une ligne, on déplace le curseur pour sauter les octets de padding
    }
    fclose(fp);
}

// alloue dynamiquement une matrice aux dimensions du pixel array:
pixel **matrix(dib_header dib_header)
{

    pixel **matrice;
    int i;
    matrice = (pixel **)malloc(dib_header.height * sizeof(pixel *));
    for (i = 0; i < dib_header.height; i++)
    {
        matrice[i] = (pixel *)malloc(dib_header.width * sizeof(pixel));
    }
    return matrice;
}

// libere la mémoire allouée à la matrice:
void free_matrix(pixel **matrice, dib_header dib_header)
{
    int i, j;
    for (int i = 0; i < dib_header.height; i++)
    {
        free(matrice[i]);
    }
    free(matrice);
}

// remplit la matrice avec les valeurs du pixel array:
pixel **fill_matrix(char *img, bmp_header bmp_header, dib_header dib_header, info info)
{

    // déclarations et allocation de la matrice:
    int i, j;
    pixel px;
    pixel **matrice;
    FILE *fp;

    fp = fopen(img, "rb");
    if (fp == NULL)
    {
        printf("\nImpossible d'ouvrir l'image (fp == NULL)\n");
    }
    matrice = matrix(dib_header);

    // remplissage de la matrice:
    fseek(fp, bmp_header.offset, SEEK_SET);
    for (i = dib_header.height - 1; i >= 0; i--)
    {
        for (j = dib_header.width - 1; j >= 0; j--)
        {
            fread(&px, 3, 1, fp);
            matrice[i][j].b = px.b;
            matrice[i][j].g = px.g;
            matrice[i][j].r = px.r;
        }
        fseek(fp, info.padding_row, SEEK_CUR);
    }

    fclose(fp);
    return matrice;
}

// affiche ligne par ligne le contenu d'une matrice de dimensions height*width:
void print_matrix(dib_header dib_header, pixel **matrice)
{

    int i, j;
    printf("\nAffichage Matrice:\n\n");

    for (i = 0; i < dib_header.height; i++)
    {
        for (j = 0; j < dib_header.width; j++)
        {
            printf("(%d,%d): %d,%d,%d  ", i, j, matrice[i][j].b, matrice[i][j].g, matrice[i][j].r);
        }
        printf("\n");
    }
}

// retourne un pixel en noir et blanc:
unsigned char gray_pixel(pixel pixel)
{
    return pixel.r * 0.299 + pixel.g * 0.587 + pixel.b * 0.114;
}

void ascii_title(dib_header dib_header, pixel **matrice)
{
    int i, j, h = 2, w = 1;
    unsigned char charr;
    char ascii[] = {'@', '$', '#', '+', ':', '-', '.', ' '};

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

// remplit le premier tableau avec les valeurs du deuxieme
char *remplir_tableau(char *tab, char *tab2, int taille)
{
    int i;
    for (i = 0; i < taille; i++)
    {
        tab[i] = tab2[i];
    }
    return tab;
}

// Paramètres: h = réduction de la hauteur, w = réduction de la largeur, nb_chars = nombre de caractères a afficher (8,16,64)
void print_ascii(dib_header dib_header, pixel **matrice, int h, int w, int nb_chars, int negatif)
{
    int i, j;
    unsigned char charr;
    char ascii8[] = {'@', '$', '#', '+', ':', '-', '.', ' '};
    char ascii16[] = {'$', '8', '#', 'h', 'Z', 'L', 'Y', 'v', 'r', '/', '{', '_', '^', ':', '.', ' '};
    char ascii64[] = {'$', '@', 'B', '%', '8', '&', 'W', 'M', '#', '*', 'o', 'a', 'h', 'k', 'b', 'd', 'p', 'q', 'w', 'm', 'Z', 'O', '0', 'Q', 'L', 'C', 'J', 'U', 'Y', 'X', 'z', 'c', 'v', 'u', 'n', 'x', 'r', 'j', 'f', 't', '/', '|', '(', '1', '{', '[', '?', '-', '_', '+', '~', '<', '>', 'i', '!', 'l', 'I', ';', ':', '"', '^', '`', '.', ' '};
    char ascii[nb_chars];

    switch (nb_chars)
    {

    case 8:
        remplir_tableau(ascii, ascii8, nb_chars);
        break;

    case 16:
        remplir_tableau(ascii, ascii16, nb_chars);
        break;

    case 64:
        remplir_tableau(ascii, ascii64, nb_chars);
        break;
    }

    if (negatif == 1)
    {
        for (i = 0; i < dib_header.height; i = i + h)
        {
            for (j = dib_header.width - 1; j >= 0; j = j - w)
            {
                charr = gray_pixel(matrice[i][j]);
                //printf("%c", ascii8[7 - charr / 32]);
                printf("%c", ascii[(nb_chars - 1) - charr / (256 / nb_chars)]);
            }

            printf("\n");
        }
    }
    else
    {
        for (i = 0; i < dib_header.height; i = i + h)
        {
            for (j = dib_header.width - 1; j >= 0; j = j - w)
            {
                charr = gray_pixel(matrice[i][j]);
                // printf("%c", ascii[7 - charr / 32]);
                printf("%c", ascii[charr / (256 / nb_chars)]);
            }

            printf("\n");
        }
    }
}

void ascii(int fond)
{
    // déclarations:
    bmp_header bmp_header;
    dib_header dib_header;
    info info;
    pixel **matrice;
    char img[20];
    int nb_chars, reponse, h, w;

    while (true)
    {
        printf("\n\t~Affichage ASCII~\n\nChoisir une image: ");
        scanf("%s", &img);

        // On remplit les structures grâce aux fonctions et on initialise une matrice avec les pixels de l'image:
        bmp_header = read_bmp(img, bmp_header);
        dib_header = read_dib(img, dib_header);
        info = get_info(bmp_header, dib_header);
        matrice = fill_matrix(img, bmp_header, dib_header, info);

        // Choix du nombre de caractères:
        
        do
        {
            printf("\nCombien de caracteres? (1: 8; 2:16; 3: 64)\n");
            scanf("%d", &nb_chars);
            if (nb_chars != 1 && nb_chars != 2 && nb_chars != 3)
            {
                printf("\n*****\tReponse incorrecte, veuillez entrer une valeur reconnue!\t*****\n\n");
            }
        } while (nb_chars != 1 && nb_chars != 2 && nb_chars != 3);


        if (nb_chars == 1)
        {
            nb_chars = 8;
        }
        if (nb_chars == 2)
        {
            nb_chars = 16;
        }
        if (nb_chars == 3)
        {
            nb_chars = 64;
        }

        // Réduire la hauteur

        do
        {
            printf("\nReduire la hauteur: Afficher 1 pixel sur combien?\n(1: Non)\n");
            scanf("%d", &h);
            if (h<=0)
            {
                printf("\n*****\tReponse incorrecte, veuillez entrer une valeur reconnue!\t*****\n\n");
            }
        } while (h<=0);

        // Réduire la largeur

        do
        {
            printf("\n\nReduire la largeur: Afficher 1 pixel sur combien?\n(1: Non)\n");
            scanf("%d", &w);
            if (w<=0)
            {
                printf("\n*****\tReponse incorrecte, veuillez entrer une valeur reconnue!\t*****\n\n");
            }
        } while (w<=0);


        print_ascii(dib_header, matrice, h, w, nb_chars, fond);
        
        do
        {
            printf("\nContinuer? (1: oui; 2: non\n");
            scanf("%d", &reponse);
            if (reponse != 1 && reponse != 2)
        {
            printf("\n*****\tReponse incorrecte, veuillez entrer une valeur reconnue!\t*****\n\n");
        }
        } while (reponse != 1 && reponse != 2);

        free_matrix(matrice, dib_header);
        if (reponse == 2)
            break;
    }
}

void debug()
{

    bmp_header bmp_header;
    dib_header dib_header;
    info info;
    pixel **matrice;
    char img[20];
    int reponse, reponse2;

    while (true)
    {
        printf("\n\t~Autres Fonctions~\n\nChoisir une image: ");
        scanf("%s", &img);

        // On remplit les structures grâce aux fonctions et on déclare une matrice de pixels:
        bmp_header = read_bmp(img, bmp_header);
        dib_header = read_dib(img, dib_header);
        info = get_info(bmp_header, dib_header);
        matrice = fill_matrix(img, bmp_header, dib_header, info);

        printf("\n\n\t~Que voulez-vous faire?~\n\n1: Afficher les infos de l'image\n2: Afficher la matrice\n3: Afficher les valeurs de chaque pixel, pixel par pixel\n");
        scanf("%d", &reponse);

        if (reponse != 1 && reponse != 2 && reponse != 3)
        {
            printf("\n*****\tReponse incorrecte, veuillez entrer une valeur reconnue!\t*****\n\n");
            do
            {
                printf("\n\n1: Afficher les infos de l'image\n2: Afficher la matrice\n3: Afficher les valeurs de chaque pixel, pixel par pixel\n");
                scanf("%d", &reponse);
            } while (reponse != 1 && reponse != 2 && reponse != 3);
        }

        switch (reponse)
        {
        case 1:
            printf("\n\tInfos:\n\n");
            print_info(bmp_header, dib_header, info);
            break;

        case 2:
            printf("\n\tMatrice:\n\n");
            print_matrix(dib_header, matrice);
            break;
        case 3:
            printf("\n\tPixels:\n\n");
            print_pixels(img, bmp_header, dib_header, info);
            break;
        }

        printf("\nContinuer? (1: oui; 2: non)\n");
        scanf("%d", &reponse2);
        if (reponse2 != 1 && reponse2 != 2)
        {
            printf("\n*****\tReponse incorrecte, veuillez entrer une valeur reconnue!\t*****\n\n");
            do
            {
                printf("\nContinuer? (1: oui; 2: non");
                scanf("%d", &reponse2);
            } while (reponse2 != 1 && reponse2 != 2);
        }

        free_matrix(matrice, dib_header);
        if (reponse2 == 2)
            break;
    }
}

int main(void)
{
    bmp_header bmp_header;
    dib_header dib_header;
    info info;
    pixel **matrice;
    int reponse, fond;

    if (sizeof(char) == 1 && sizeof(short int) == 2 && sizeof(int) == 4)
    {
        char img[20] = "titre.bmp";
        bmp_header = read_bmp(img, bmp_header);
        dib_header = read_dib(img, dib_header);
        info = get_info(bmp_header, dib_header);
        matrice = fill_matrix(img, bmp_header, dib_header, info);
        ascii_title(dib_header, matrice);
        free_matrix(matrice, dib_header);

        printf("\n\tNaim Chefirat et Gauvain Crevot");

        while (true)
        {
            printf("\n\n\t\t****MENU PRINCIPAL****");
            printf("\n\n\t~Que voulez-vous faire?~\n\n1: Affichage ascii\n2: Autres fonctions\n3: Quitter\n");
            scanf("%d", &reponse);

            if (reponse != 1 && reponse != 2 && reponse != 3)
            {
                printf("\n*****\tReponse incorrecte, veuillez entrer une valeur reconnue!\t*****\n\n");
                do
                {
                    printf("\n\n1: Affichage ascii\n2: Autres fonctions\n3: Quitter\n");
                    scanf("%d", &reponse);
                } while (reponse != 1 && reponse != 2 && reponse != 3);
            }

            switch (reponse)
            {
            case 1:
                printf("\nQuelle est la couleur de l'arriere-plan? (1: noir; 2: blanc)\n");
                scanf("%d", &fond);
                if (fond != 1 && fond != 2)
                {
                    printf("\n*****\tReponse incorrecte, veuillez entrer une valeur reconnue!\t*****\n\n");
                    do
                    {
                        printf("\nQuelle est la couleur de l'arrière-plan? (1: noir; 2: blanc)\n");
                        scanf("%d", &fond);
                    } while (fond != 1 && fond != 2);
                }
                ascii(fond);
                printf("Retour au menu principal");
                break;

            case 2:
                debug();
                printf("Retour au menu principal");
                break;

                break;
            case 3:
                break;
            }
            if (reponse == 3)
                break;
        }
        printf("\nMerci d'avoir utilise notre programme");
        return EXIT_SUCCESS;
    }
    else
    {
        printf("\nTaille des variables pas adaptée");
        return EXIT_FAILURE;
    }
}
