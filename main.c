#include "tp3.h"

int main() {
    t_processus* tableau = NULL;
    int nb_processus = 0;
    int choix;
    char nom_fichier[256];

    do {
        printf("\n===== Menu =====\n");
        printf("1. Charger processus depuis un fichier\n");
        printf("2. Simuler FCFS\n");
        printf("3. Simuler SJF\n");
        printf("4. Afficher le diagramme de Gantt\n");
        printf("5. Quitter\n");
        printf("Votre choix : ");
        scanf("%d", &choix);

        switch (choix) {
        case 1:
            printf("Nom du fichier : ");
            scanf("%s", nom_fichier);

            if (tableau != NULL) {
                free(tableau);
                tableau = NULL;
                nb_processus = 0;
            }

            tableau = charger_processus(nom_fichier, &nb_processus);
            if (tableau == NULL) {
                printf("Echec du chargement.\n");
            }
            break;

        case 2:
            if (tableau == NULL) {
                printf("Veuillez charger un fichier d'abord.\n");
            } else {
                simuler_fcfs(tableau, nb_processus);
            }
            break;

        case 3:
            if (tableau == NULL) {
                printf("Veuillez charger un fichier d'abord.\n");
            } else {
                simuler_sjf(tableau, nb_processus);
            }
            break;

        case 4:
            if (tableau == NULL) {
                printf("Veuillez charger un fichier d'abord.\n");
            } else {
                afficher_gantt(tableau, nb_processus);
            }
            break;

        case 5:
            printf("Au revoir !\n");
            break;

        default:
            printf("Choix invalide.\n");
        }

    } while (choix != 5);

    if (tableau != NULL) {
        free(tableau);
    }

    return 0;
}
