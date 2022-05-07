#include<stdio.h>

char * remplir_tableau(char * tab, char * tab2, int taille){
    int i;
    for(i = 0; i<taille;i++){
        tab[i] = tab2[i];
    }
    return tab;
}

int main(){

    int nb_char = 64;
    char ascii[nb_char];
    char ascii8[] = {'@', '$', '#', '+', ':', '-', '.', ' '};
    char ascii16[] =  {'$', '8', '#', 'h', 'Z', 'L', 'Y', 'v', 'r', '/', '{', '_', '^', ':', '.', ' '};
    char ascii64[] = {'$', '@', 'B', '%', '8', '&', 'W', 'M', '#', '*', 'o', 'a', 'h', 'k', 'b', 'd', 'p', 'q', 'w', 'm', 'Z', 'O', '0', 'Q', 'L', 'C', 'J', 'U', 'Y', 'X', 'z', 'c', 'v', 'u', 'n', 'x', 'r', 'j', 'f', 't', '/', '|', '(', '1', '{', '[', '?', '-', '_', '+', '~', '<', '>', 'i', '!', 'l', 'I', ';', ':', '"', '^', '`', '.', ' '};

    remplir_tableau(ascii, ascii64, nb_char);

    int res;
    do{
        printf("\nreponse: ");
        scanf("%d", &res);
        if (res != 33) printf("\nMauvaise reponse\n");
    }while (res != 33);

    return 0;
}

