
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[]) {
    int creation = 0, affichage = 0;
    int dimension = 0, binaire = 1;
    char *nomfichier = NULL;
    int **matrice;
    FILE *fichier;
    int i, j;

    /* Lecture des options */
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0)
            creation = 1;
        else if (strcmp(argv[i], "-a") == 0)
            affichage = 1;
        else if (strcmp(argv[i], "-b") == 0)
            binaire = 1;
        else if (strcmp(argv[i], "-t") == 0)
            binaire = 0;
        else if (strcmp(argv[i], "-d") == 0 && i + 1 < argc)
            dimension = atoi(argv[++i]);
        else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc)
            nomfichier = argv[++i];
        else {
            printf("Option inconnue ou argument manquant : %s\n",
                   argv[i]);
            return 1;
        }
    }

    if ((!creation && !affichage) ||
        (creation && affichage) ||
        nomfichier == NULL) {
        printf("Usage :\n");
        printf("./matrix -c -d 10 -f fichiermatrice.bin [-b|-t]\n");
        printf("./matrix -a -f fichiermatrice.bin [-b|-t]\n");
        return 1;
    }

    if (creation && dimension <= 0) {
        printf("La dimension doit etre positive.\n");
        return 1;
    }

    /* Ouverture du fichier */
    if (creation)
        fichier = fopen(nomfichier, binaire ? "wb" : "w");
    else
        fichier = fopen(nomfichier, binaire ? "rb" : "r");

    if (fichier == NULL) {
        perror("Erreur d'ouverture du fichier");
        return 1;
    }

    /* Creation de la matrice */
    if (creation) {
        srand((unsigned int) time(NULL));

        /* Allocation dynamique */
        matrice = malloc(dimension * sizeof(int *));
        if (matrice == NULL) {
            fclose(fichier);
            return 1;
        }

        for (i = 0; i < dimension; i++) {
            matrice[i] = malloc(dimension * sizeof(int));
            if (matrice[i] == NULL) {
                while (i > 0)
                    free(matrice[--i]);
                free(matrice);
                fclose(fichier);
                return 1;
            }
        }

        /* Remplissage aleatoire */
        for (i = 0; i < dimension; i++) {
            for (j = 0; j < dimension; j++)
                matrice[i][j] = rand() % 100;
        }

        /* Enregistrement dans le fichier */
        if (binaire) {
            fwrite(&dimension, sizeof(int), 1, fichier);

            for (i = 0; i < dimension; i++)
                fwrite(matrice[i], sizeof(int), dimension, fichier);
        } else {
            fprintf(fichier, "%d\n", dimension);

            for (i = 0; i < dimension; i++) {
                for (j = 0; j < dimension; j++)
                    fprintf(fichier, "%d ", matrice[i][j]);
                fprintf(fichier, "\n");
            }
        }

        printf("Matrice enregistree dans %s\n", nomfichier);

        for (i = 0; i < dimension; i++)
            free(matrice[i]);
        free(matrice);
    }

    /* Lecture et affichage de la matrice */
    else {
        if (binaire) {
            if (fread(&dimension, sizeof(int), 1, fichier) != 1) {
                printf("Fichier binaire invalide.\n");
                fclose(fichier);
                return 1;
            }
        } else {
            if (fscanf(fichier, "%d", &dimension) != 1 ||
                dimension <= 0) {
                printf("Fichier texte invalide.\n");
                fclose(fichier);
                return 1;
            }
        }

        if (dimension <= 0 || dimension > 10000) {
            printf("Dimension invalide.\n");
            fclose(fichier);
            return 1;
        }

        matrice = malloc(dimension * sizeof(int *));
        if (matrice == NULL) {
            fclose(fichier);
            return 1;
        }

        for (i = 0; i < dimension; i++) {
            matrice[i] = malloc(dimension * sizeof(int));
            if (matrice[i] == NULL) {
                while (i > 0)
                    free(matrice[--i]);
                free(matrice);
                fclose(fichier);
                return 1;
            }
        }

        int erreur = 0;

        if (binaire) {
            for (i = 0; i < dimension; i++) {
                if (fread(matrice[i], sizeof(int),
                          dimension, fichier) != (size_t) dimension) {
                    erreur = 1;
                    break;
                }
            }
        } else {
            for (i = 0; i < dimension && !erreur; i++) {
                for (j = 0; j < dimension; j++) {
                    if (fscanf(fichier, "%d", &matrice[i][j]) != 1) {
                        erreur = 1;
                        break;
                    }
                }
            }
        }

        if (erreur) {
            printf("Erreur de lecture de la matrice.\n");
        } else {
            printf("Matrice %d x %d :\n", dimension, dimension);

            for (i = 0; i < dimension; i++) {
                for (j = 0; j < dimension; j++)
                    printf("%4d", matrice[i][j]);
                printf("\n");
            }
        }

        for (i = 0; i < dimension; i++)
            free(matrice[i]);
        free(matrice);
    }

    fclose(fichier);
    return 0;
}
