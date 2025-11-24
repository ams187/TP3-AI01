#include "tp3.h"

int main(){
    t_processus* tableau;
    int nb_processus;
    int choix;
    char nom_fichier[256];
    
    tableau = NULL;
    nb_processus = 0;

    do {
        printf("\n===== Menu =====\n");
        printf("1. Charger processus depuis un fichier\n");
        printf("2. Simuler FCFS\n");
        printf("3. Simuler SJF\n");
        printf("4. Afficher le Gantt\n");
        printf("5. Quitter\n");
        printf("Votre choix : ");
        scanf("%d", &choix);

        switch (choix){
        case 1: /* Charger processus */
            printf("Nom du fichier : ");
            scanf("%s", nom_fichier);

            /* Liberer l'ancien tableau si existant */
            if (tableau!=NULL){
                free(tableau);
                tableau = NULL;
                nb_processus = 0;
            }

            /* Charger le nouveau */
            tableau = charger_processus(nom_fichier, &nb_processus);
            if (tableau==NULL){
                printf("Echec du chargement.\n");
            }
            break;

        case 2: /* Simuler FCFS */
            if (tableau==NULL){
                printf("Veuillez charger un fichier d'abord.\n");
            } else {
                simuler_fcfs(tableau, nb_processus);
            }
            break;

        case 3: /* Simuler SJF */
            if (tableau==NULL){
                printf("Veuillez charger un fichier d'abord.\n");
            } else {
                simuler_sjf(tableau, nb_processus);
            }
            break;

        case 4: /* Afficher le Gantt */
            if (tableau==NULL){
                printf("Veuillez charger un fichier d'abord.\n");
            } else {
                afficher_gantt(tableau, nb_processus);
            }
            break;

        case 5: /* Quitter */
            printf("Au revoir !\n");
            break;

        default:
            printf("Choix invalide.\n");
        }

    } while (choix!=5);

    /* Nettoyage memoire avant sortie */
    if (tableau!=NULL){
        free(tableau);
    }

    return 0;
}
