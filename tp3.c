#include "tp3.h"

/* ========== FONCTIONS COMMUNES ========== */

/* Complexite : O(1) */
t_processus* creer_processus(int pid, int arrivee, int duree){
    t_processus* p;
    p = malloc(sizeof(t_processus));
    if (p==NULL){
        fprintf(stderr, "Erreur allocation memoire\n");
        return NULL;
    }
    p->pid = pid;
    p->arrivee = arrivee;
    p->duree = duree;
    p->suivant = NULL;
    return p;
}

/* Complexite : O(1) */
void free_processus(t_processus* p){
    if (p!=NULL){
        free(p);
    }
}

/* ========== CHARGEMENT DEPUIS FICHIER ========== */

/* Complexite : O(n) ou n = nombre de processus */
t_processus* charger_processus(char* nom_fichier, int* nb_processus){
    FILE* f;
    int count,i;
    int pid,arrivee,duree;
    t_processus* tableau;
    
    f = fopen(nom_fichier, "r");
    if (f==NULL){
        fprintf(stderr, "Erreur ouverture fichier %s\n", nom_fichier);
        return NULL;
    }

    /* Compter le nombre de lignes */
    count = 0;
    while (fscanf(f, "%d %d %d", &pid, &arrivee, &duree)==3){
        count++;
    }

    if (count==0){
        fclose(f);
        *nb_processus = 0;
        return NULL;
    }

    /* Allouer le tableau */
    tableau = malloc(count*sizeof(t_processus));
    if (tableau==NULL){
        fprintf(stderr, "Erreur allocation memoire\n");
        fclose(f);
        return NULL;
    }

    /* Recharger les donnees */
    rewind(f);
    i = 0;
    while (fscanf(f, "%d %d %d", &pid, &arrivee, &duree)==3){
        tableau[i].pid = pid;
        tableau[i].arrivee = arrivee;
        tableau[i].duree = duree;
        tableau[i].suivant = NULL;
        i++;
    }

    fclose(f);
    *nb_processus = count;
    printf("Charge %d processus depuis %s\n", count, nom_fichier);
    return tableau;
}

/* ========== FONCTIONS FIFO (FCFS) ========== */

/* Complexite : O(1) */
t_processus* fifo_init(){
    return NULL;
}

/* Complexite : O(n) ou n = nombre d'elements dans la file */
t_processus* fifo_clear(t_processus* file){
    t_processus* current;
    t_processus* next;
    
    current = file;
    while (current!=NULL){
        next = current->suivant;
        free_processus(current);
        current = next;
    }
    return NULL;
}

/* Complexite : O(1) */
int fifo_vide(t_processus* file){
    return (file==NULL) ? 1 : 0;
}

/* Complexite : O(n) pour atteindre la fin de la file */
t_processus* fifo_enfiler(t_processus* file, t_processus* p){
    t_processus* current;
    
    if (p==NULL) return file;

    p->suivant = NULL;

    if (file==NULL){
        return p;
    }

    /* Parcourir jusqu'a la fin */
    current = file;
    while (current->suivant!=NULL){
        current = current->suivant;
    }
    current->suivant = p;

    return file;
}

/* Complexite : O(1) */
t_processus* fifo_defiler(t_processus** file){
    t_processus* p;
    
    if (file==NULL || *file==NULL){
        return NULL;
    }

    p = *file;
    *file = (*file)->suivant;
    p->suivant = NULL;
    return p;
}

/* Complexite : O(n*m) ou n = duree totale, m = nombre de processus */
void simuler_fcfs(t_processus* tableau, int nb_processus){
    t_processus* file;
    t_processus* processus_courant;
    t_processus* nouveau;
    int temps_restant,t,processus_termines,idx_prochain;
    
    printf("\n=== Simulation FCFS ===\n");

    file = fifo_init();
    processus_courant = NULL;
    temps_restant = 0;
    t = 0;
    processus_termines = 0;
    idx_prochain = 0;

    while (processus_termines<nb_processus){
        /* Admettre les processus arrives */
        while (idx_prochain<nb_processus && tableau[idx_prochain].arrivee==t){
            nouveau = creer_processus(tableau[idx_prochain].pid,
                                     tableau[idx_prochain].arrivee,
                                     tableau[idx_prochain].duree);
            printf("t=%d : arrivee P%d (duree=%d)\n", t, nouveau->pid, nouveau->duree);
            file = fifo_enfiler(file, nouveau);
            idx_prochain++;
        }

        /* Si le processeur est libre, prendre le prochain */
        if (processus_courant==NULL && !fifo_vide(file)){
            processus_courant = fifo_defiler(&file);
            temps_restant = processus_courant->duree;
            printf("t=%d : run P%d (duree=%d)\n", t, processus_courant->pid, temps_restant);
        }

        /* Executer le processus courant */
        if (processus_courant!=NULL){
            temps_restant--;
            if (temps_restant==0){
                t++;
                printf("t=%d : fin P%d\n", t, processus_courant->pid);
                free_processus(processus_courant);
                processus_courant = NULL;
                processus_termines++;
                continue;
            }
        }

        t++;
    }

    printf("\n=== Fin de la simulation FCFS ===\n");
    fifo_clear(file);
}

/* ========== FONCTIONS LISTE TRIEE (SJF) ========== */

/* Complexite : O(1) */
t_processus* lsorted_init(){
    return NULL;
}

/* Complexite : O(n) ou n = nombre d'elements dans la liste */
t_processus* lsorted_clear(t_processus* liste){
    t_processus* current;
    t_processus* next;
    
    current = liste;
    while (current!=NULL){
        next = current->suivant;
        free_processus(current);
        current = next;
    }
    return NULL;
}

/* Complexite : O(1) */
int lsorted_vide(t_processus* liste){
    return (liste==NULL) ? 1 : 0;
}

/* Complexite : O(n) ou n = nombre d'elements dans la liste */
t_processus* lsorted_inserer_trie(t_processus* liste, t_processus* p){
    t_processus* current;
    
    if (p==NULL) return liste;

    p->suivant = NULL;

    /* Liste vide ou insertion en tete */
    if (liste==NULL || p->duree<liste->duree){
        p->suivant = liste;
        return p;
    }

    /* Chercher la position d'insertion */
    current = liste;
    while (current->suivant!=NULL && current->suivant->duree<=p->duree){
        current = current->suivant;
    }

    p->suivant = current->suivant;
    current->suivant = p;

    return liste;
}

/* Complexite : O(1) */
t_processus* lsorted_extraire_premier(t_processus** liste){
    t_processus* p;
    
    if (liste==NULL || *liste==NULL){
        return NULL;
    }

    p = *liste;
    *liste = (*liste)->suivant;
    p->suivant = NULL;
    return p;
}

/* Complexite : O(n*m) ou n = duree totale, m = nombre de processus */
void simuler_sjf(t_processus* tableau, int nb_processus){
    t_processus* liste;
    t_processus* processus_courant;
    t_processus* nouveau;
    int temps_restant,t,processus_termines,idx_prochain;
    
    printf("\n=== Simulation SJF ===\n");

    liste = lsorted_init();
    processus_courant = NULL;
    temps_restant = 0;
    t = 0;
    processus_termines = 0;
    idx_prochain = 0;

    while (processus_termines<nb_processus){
        /* Admettre les processus arrives */
        while (idx_prochain<nb_processus && tableau[idx_prochain].arrivee==t){
            nouveau = creer_processus(tableau[idx_prochain].pid,
                                     tableau[idx_prochain].arrivee,
                                     tableau[idx_prochain].duree);
            printf("t=%d : arrivee P%d (duree=%d)\n", t, nouveau->pid, nouveau->duree);
            liste = lsorted_inserer_trie(liste, nouveau);
            idx_prochain++;
        }

        /* Si le processeur est libre, prendre le plus court */
        if (processus_courant==NULL && !lsorted_vide(liste)){
            processus_courant = lsorted_extraire_premier(&liste);
            temps_restant = processus_courant->duree;
            printf("t=%d : run P%d (duree=%d)\n", t, processus_courant->pid, temps_restant);
        }

        /* Executer le processus courant */
        if (processus_courant!=NULL){
            temps_restant--;
            if (temps_restant==0){
                t++;
                printf("t=%d : fin P%d\n", t, processus_courant->pid);
                free_processus(processus_courant);
                processus_courant = NULL;
                processus_termines++;
                continue;
            }
        }

        t++;
    }

    printf("\n=== Fin de la simulation SJF ===\n");
    lsorted_clear(liste);
}

/* ========== FONCTIONS POUR LE DIAGRAMME DE GANTT ========== */

/* Simulation FCFS pour collecter les donnees d'execution */
int simuler_fcfs_gantt(t_processus* tableau, int nb_processus, t_execution* exec){
    t_processus* file;
    t_processus* processus_courant;
    t_processus* nouveau;
    int temps_restant,t,processus_termines,idx_prochain,nb_exec,temps_debut;
    
    file = fifo_init();
    processus_courant = NULL;
    temps_restant = 0;
    t = 0;
    processus_termines = 0;
    idx_prochain = 0;
    nb_exec = 0;
    temps_debut = 0;

    while (processus_termines<nb_processus){
        while (idx_prochain<nb_processus && tableau[idx_prochain].arrivee==t){
            nouveau = creer_processus(tableau[idx_prochain].pid,
                                     tableau[idx_prochain].arrivee,
                                     tableau[idx_prochain].duree);
            file = fifo_enfiler(file, nouveau);
            idx_prochain++;
        }

        if (processus_courant==NULL && !fifo_vide(file)){
            processus_courant = fifo_defiler(&file);
            temps_restant = processus_courant->duree;
            temps_debut = t;
        }

        if (processus_courant!=NULL){
            temps_restant--;
            if (temps_restant==0){
                t++;
                exec[nb_exec].pid = processus_courant->pid;
                exec[nb_exec].debut = temps_debut;
                exec[nb_exec].fin = t;
                nb_exec++;
                free_processus(processus_courant);
                processus_courant = NULL;
                processus_termines++;
                continue;
            }
        }
        t++;
    }

    fifo_clear(file);
    return nb_exec;
}

/* Simulation SJF pour collecter les donnees d'execution */
int simuler_sjf_gantt(t_processus* tableau, int nb_processus, t_execution* exec){
    t_processus* liste;
    t_processus* processus_courant;
    t_processus* nouveau;
    int temps_restant,t,processus_termines,idx_prochain,nb_exec,temps_debut;
    
    liste = lsorted_init();
    processus_courant = NULL;
    temps_restant = 0;
    t = 0;
    processus_termines = 0;
    idx_prochain = 0;
    nb_exec = 0;
    temps_debut = 0;

    while (processus_termines<nb_processus){
        while (idx_prochain<nb_processus && tableau[idx_prochain].arrivee==t){
            nouveau = creer_processus(tableau[idx_prochain].pid,
                                     tableau[idx_prochain].arrivee,
                                     tableau[idx_prochain].duree);
            liste = lsorted_inserer_trie(liste, nouveau);
            idx_prochain++;
        }

        if (processus_courant==NULL && !lsorted_vide(liste)){
            processus_courant = lsorted_extraire_premier(&liste);
            temps_restant = processus_courant->duree;
            temps_debut = t;
        }

        if (processus_courant!=NULL){
            temps_restant--;
            if (temps_restant==0){
                t++;
                exec[nb_exec].pid = processus_courant->pid;
                exec[nb_exec].debut = temps_debut;
                exec[nb_exec].fin = t;
                nb_exec++;
                free_processus(processus_courant);
                processus_courant = NULL;
                processus_termines++;
                continue;
            }
        }
        t++;
    }

    lsorted_clear(liste);
    return nb_exec;
}

/* Affichage du diagramme de Gantt comparatif */
void afficher_gantt(t_processus* tableau, int nb_processus){
    t_execution exec_fcfs[100];
    t_execution exec_sjf[100];
    int nb_exec_fcfs,nb_exec_sjf,temps_max,i,j,k,trouve;
    
    printf("\n=== Comparaison des algorithmes FCFS et SJF ===\n");
    
    /* Collecter les donnees */
    nb_exec_fcfs = simuler_fcfs_gantt(tableau, nb_processus, exec_fcfs);
    nb_exec_sjf = simuler_sjf_gantt(tableau, nb_processus, exec_sjf);
    
    /* Determiner le temps maximum */
    temps_max = exec_fcfs[nb_exec_fcfs-1].fin;
    if (exec_sjf[nb_exec_sjf-1].fin > temps_max){
        temps_max = exec_sjf[nb_exec_sjf-1].fin;
    }
    
    /* Afficher la ligne FCFS */
    printf("FCFS : ");
    for (i=0; i<temps_max; i++){
        trouve = 0;
        for (j=0; j<nb_exec_fcfs; j++){
            if (i>=exec_fcfs[j].debut && i<exec_fcfs[j].fin){
                printf("\033[0;3%dmP%d\033[0m", (exec_fcfs[j].pid%6)+1, exec_fcfs[j].pid);
                trouve = 1;
                break;
            }
        }
        if (!trouve) printf("   ");
    }
    printf("\n");
    
    /* Afficher la ligne SJF */
    printf("SJF  : ");
    for (i=0; i<temps_max; i++){
        trouve = 0;
        for (j=0; j<nb_exec_sjf; j++){
            if (i>=exec_sjf[j].debut && i<exec_sjf[j].fin){
                printf("\033[0;3%dmP%d\033[0m", (exec_sjf[j].pid%6)+1, exec_sjf[j].pid);
                trouve = 1;
                break;
            }
        }
        if (!trouve) printf("   ");
    }
    printf("\n");
    
    /* Afficher la ligne des arrivees */
    printf("Arr. : ");
    for (i=0; i<temps_max; i++){
        trouve = 0;
        for (k=0; k<nb_processus; k++){
            if (tableau[k].arrivee==i){
                printf("^  ");
                trouve = 1;
                break;
            }
        }
        if (!trouve) printf("   ");
    }
    printf("\n");
    
    /* Afficher la legende */
    printf("\nLegende : ");
    for (i=0; i<nb_processus; i++){
        printf("\033[0;3%dmP%d\033[0m ", (tableau[i].pid%6)+1, tableau[i].pid);
    }
    printf("\n");
}
