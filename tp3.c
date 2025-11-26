#include "tp3.h"

/* ========== FONCTIONS COMMUNES ========== */

/* Complexite : O(1) */
t_processus* creer_processus(int pid, int arrivee, int duree) {
    t_processus* p = malloc(sizeof(t_processus));
    if (p == NULL) {
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
void free_processus(t_processus* p) {
    if (p != NULL) {
        free(p);
    }
}

/* ========== CHARGEMENT DEPUIS FICHIER - OPTIMISE ========== */

/* Complexite : O(n) - UN SEUL PARCOURS !
 * Au passage,on utiliise realloc pour allocation dynamique
 */
t_processus* charger_processus(char* nom_fichier, int* nb_processus) {
    FILE* f = fopen(nom_fichier, "r");
    if (f == NULL) {
        fprintf(stderr, "Erreur ouverture fichier %s\n", nom_fichier);
        return NULL;
    }

    /* Capacite initiale */
    int capacite = 10;
    t_processus* tableau = malloc(capacite * sizeof(t_processus));
    if (tableau == NULL) {
        fprintf(stderr, "Erreur allocation memoire\n");
        fclose(f);
        return NULL;
    }

    int count = 0;
    int pid, arrivee, duree;
    
    /* UNE SEULE BOUCLE - lecture directe */
    while (fscanf(f, "%d %d %d", &pid, &arrivee, &duree) == 3) {
        /* Agrandir si necessaire - realloc amortized O(1) */
        if (count >= capacite) {
            capacite *= 2;  // Doubler la capacite
            t_processus* nouveau = realloc(tableau, capacite * sizeof(t_processus));
            if (nouveau == NULL) {
                fprintf(stderr, "Erreur reallocation memoire\n");
                free(tableau);
                fclose(f);
                return NULL;
            }
            tableau = nouveau;
        }
        
        /* Remplir directement */
        tableau[count].pid = pid;
        tableau[count].arrivee = arrivee;
        tableau[count].duree = duree;
        tableau[count].suivant = NULL;
        count++;
    }

    fclose(f);

    if (count == 0) {
        free(tableau);
        *nb_processus = 0;
        return NULL;
    }

    /* Ajuster la taille finale (optionnel mais propre) */
    if (count < capacite) {
        t_processus* final = realloc(tableau, count * sizeof(t_processus));
        if (final != NULL) {
            tableau = final;
        }
        /* Si realloc echoue, on garde l'ancien tableau (plus grand mais ok) */
    }

    *nb_processus = count;
    printf("Charge %d processus depuis %s\n", count, nom_fichier);
    return tableau;
}

/* ========== FONCTIONS FIFO (FCFS) ========== */

/* Complexite : O(1) */
t_file* fifo_init() {
    t_file* file = malloc(sizeof(t_file));
    if (file == NULL) {
        fprintf(stderr, "Erreur allocation memoire\n");
        return NULL;
    }
    file->tete = NULL;
    file->queue = NULL;
    file->taille = 0;
    return file;
}

/* Complexite : O(n) */
void fifo_clear(t_file* file) {
    if (file == NULL) return;
    
    t_processus* current = file->tete;
    while (current != NULL) {
        t_processus* next = current->suivant;
        free_processus(current);
        current = next;
    }
    
    file->tete = NULL;
    file->queue = NULL;
    file->taille = 0;
}

/* Complexite : O(1) */
int fifo_vide(t_file* file) {
    return (file == NULL || file->taille == 0);
}

/* Complexite : O(1) */
void fifo_enfiler(t_file* file, t_processus* p) {
    if (file == NULL || p == NULL) return;
    
    p->suivant = NULL;
    
    if (file->queue == NULL) {
        file->tete = p;
        file->queue = p;
    } else {
        file->queue->suivant = p;
        file->queue = p;
    }
    
    file->taille++;
}

/* Complexite : O(1) */
t_processus* fifo_defiler(t_file* file) {
    if (file == NULL || file->tete == NULL) {
        return NULL;
    }
    
    t_processus* p = file->tete;
    file->tete = file->tete->suivant;
    
    if (file->tete == NULL) {
        file->queue = NULL;
    }
    
    file->taille--;
    p->suivant = NULL;
    return p;
}

/* Complexite : O(n + T) */
void simuler_fcfs(t_processus* tableau, int nb_processus) {
    printf("\n=== Simulation FCFS ===\n");

    t_file* file = fifo_init();
    int temps_actuel = 0;
    int idx = 0;

    while (idx < nb_processus || !fifo_vide(file)) {
        /* Admettre tous les processus arrives */
        while (idx < nb_processus && tableau[idx].arrivee <= temps_actuel) {
            t_processus* nouveau = creer_processus(
                tableau[idx].pid,
                tableau[idx].arrivee,
                tableau[idx].duree
            );
            printf("t=%d : arrivee P%d (duree=%d)\n", 
                   tableau[idx].arrivee, nouveau->pid, nouveau->duree);
            fifo_enfiler(file, nouveau);
            idx++;
        }

        if (!fifo_vide(file)) {
            t_processus* p = fifo_defiler(file);
            int debut = temps_actuel;
            int fin = debut + p->duree;
            
            printf("t=%d : run P%d (duree=%d)\n", debut, p->pid, p->duree);
            printf("t=%d : fin P%d\n", fin, p->pid);
            
            temps_actuel = fin;
            free_processus(p);
        } else if (idx < nb_processus) {
            temps_actuel = tableau[idx].arrivee;
        }
    }

    printf("\n=== Fin de la simulation FCFS ===\n");
    fifo_clear(file);
    free(file);
}

/* ========== FONCTIONS LISTE TRIEE (SJF) ========== */

/* Complexite : O(1) */
t_processus* lsorted_init() {
    return NULL;
}

/* Complexite : O(n) */
t_processus* lsorted_clear(t_processus* liste) {
    t_processus* current = liste;
    while (current != NULL) {
        t_processus* next = current->suivant;
        free_processus(current);
        current = next;
    }
    return NULL;
}

/* Complexite : O(1) */
int lsorted_vide(t_processus* liste) {
    return (liste == NULL);
}

/* Complexite : O(k) ou k = taille actuelle de la liste
 * Pire cas : O(n) */
t_processus* lsorted_inserer_trie(t_processus* liste, t_processus* p) {
    if (p == NULL) return liste;
    
    p->suivant = NULL;

    if (liste == NULL || p->duree < liste->duree) {
        p->suivant = liste;
        return p;
    }

    t_processus* current = liste;
    while (current->suivant != NULL && 
           current->suivant->duree <= p->duree) {
        current = current->suivant;
    }

    p->suivant = current->suivant;
    current->suivant = p;

    return liste;
}

/* Complexite : O(1) */
t_processus* lsorted_extraire_premier(t_processus** liste) {
    if (liste == NULL || *liste == NULL) {
        return NULL;
    }

    t_processus* p = *liste;
    *liste = (*liste)->suivant;
    p->suivant = NULL;
    return p;
}

/* Complexite : O(n^2 + T) */
void simuler_sjf(t_processus* tableau, int nb_processus) {
    printf("\n=== Simulation SJF ===\n");

    t_processus* liste = lsorted_init();
    int temps_actuel = 0;
    int idx = 0;

    while (idx < nb_processus || !lsorted_vide(liste)) {
        /* Admettre tous les processus arrives */
        while (idx < nb_processus && tableau[idx].arrivee <= temps_actuel) {
            t_processus* nouveau = creer_processus(
                tableau[idx].pid,
                tableau[idx].arrivee,
                tableau[idx].duree
            );
            printf("t=%d : arrivee P%d (duree=%d)\n", 
                   tableau[idx].arrivee, nouveau->pid, nouveau->duree);
            liste = lsorted_inserer_trie(liste, nouveau);
            idx++;
        }

        if (!lsorted_vide(liste)) {
            t_processus* p = lsorted_extraire_premier(&liste);
            int debut = temps_actuel;
            int fin = debut + p->duree;
            
            printf("t=%d : run P%d (duree=%d)\n", debut, p->pid, p->duree);
            printf("t=%d : fin P%d\n", fin, p->pid);
            
            temps_actuel = fin;
            free_processus(p);
        } else if (idx < nb_processus) {
            temps_actuel = tableau[idx].arrivee;
        }
    }

    printf("\n=== Fin de la simulation SJF ===\n");
    lsorted_clear(liste);
}

/* ========== FONCTIONS GANTT ========== */

/* Complexite : O(n + T) */
int simuler_fcfs_gantt(t_processus* tableau, int nb_processus, 
                       t_execution* exec) {
    t_file* file = fifo_init();
    int temps_actuel = 0;
    int idx = 0;
    int nb_exec = 0;

    while (idx < nb_processus || !fifo_vide(file)) {
        while (idx < nb_processus && tableau[idx].arrivee <= temps_actuel) {
            t_processus* nouveau = creer_processus(
                tableau[idx].pid,
                tableau[idx].arrivee,
                tableau[idx].duree
            );
            fifo_enfiler(file, nouveau);
            idx++;
        }

        if (!fifo_vide(file)) {
            t_processus* p = fifo_defiler(file);
            exec[nb_exec].pid = p->pid;
            exec[nb_exec].debut = temps_actuel;
            exec[nb_exec].fin = temps_actuel + p->duree;
            
            temps_actuel = exec[nb_exec].fin;
            nb_exec++;
            free_processus(p);
        } else if (idx < nb_processus) {
            temps_actuel = tableau[idx].arrivee;
        }
    }

    fifo_clear(file);
    free(file);
    return nb_exec;
}

/* Complexite : O(n^2 + T) */
int simuler_sjf_gantt(t_processus* tableau, int nb_processus, 
                      t_execution* exec) {
    t_processus* liste = lsorted_init();
    int temps_actuel = 0;
    int idx = 0;
    int nb_exec = 0;

    while (idx < nb_processus || !lsorted_vide(liste)) {
        while (idx < nb_processus && tableau[idx].arrivee <= temps_actuel) {
            t_processus* nouveau = creer_processus(
                tableau[idx].pid,
                tableau[idx].arrivee,
                tableau[idx].duree
            );
            liste = lsorted_inserer_trie(liste, nouveau);
            idx++;
        }

        if (!lsorted_vide(liste)) {
            t_processus* p = lsorted_extraire_premier(&liste);
            exec[nb_exec].pid = p->pid;
            exec[nb_exec].debut = temps_actuel;
            exec[nb_exec].fin = temps_actuel + p->duree;
            
            temps_actuel = exec[nb_exec].fin;
            nb_exec++;
            free_processus(p);
        } else if (idx < nb_processus) {
            temps_actuel = tableau[idx].arrivee;
        }
    }

    lsorted_clear(liste);
    return nb_exec;
}



/* Complexite : O(n^2 + T) */
void afficher_gantt(t_processus* tableau, int nb_processus) {
    // Allocation dynamique
    
    // Ici, on a un historique des executions
    t_execution* exec_fcfs = malloc(nb_processus * sizeof(t_execution));
    t_execution* exec_sjf = malloc(nb_processus * sizeof(t_execution));

    if (exec_fcfs == NULL || exec_sjf == NULL) {
        fprintf(stderr, "Erreur allocation memoire (exec)\n");
        free(exec_fcfs); // On free au cas où l'un des deux a marché
        free(exec_sjf);
        return;
    }

    
    printf("\n=== Comparaison FCFS vs SJF ===\n");
    
    int nb_exec_fcfs = simuler_fcfs_gantt(tableau, nb_processus, exec_fcfs);
    int nb_exec_sjf = simuler_sjf_gantt(tableau, nb_processus, exec_sjf);
    
    // Sécurité : si aucun processus, on évite de lire exec[-1]
    if (nb_exec_fcfs == 0 && nb_exec_sjf == 0) {
        free(exec_fcfs);
        free(exec_sjf);
        return;
    }

    int temps_max = 0;
    if (nb_exec_fcfs > 0) temps_max = exec_fcfs[nb_exec_fcfs - 1].fin;
    if (nb_exec_sjf > 0 && exec_sjf[nb_exec_sjf - 1].fin > temps_max) {
        temps_max = exec_sjf[nb_exec_sjf - 1].fin;
    }
    
    int* fcfs_display = calloc(temps_max, sizeof(int));
    int* sjf_display = calloc(temps_max, sizeof(int));
    int* arr_display = calloc(temps_max, sizeof(int));
    
    if (!fcfs_display || !sjf_display || !arr_display) {
        fprintf(stderr, "Erreur allocation memoire (display)\n");
        // On n'oublie pas de tout libérer
        free(exec_fcfs);
        free(exec_sjf);
        free(fcfs_display);
        free(sjf_display);
        free(arr_display);
        return;
    }
    
    
    for (int j = 0; j < nb_exec_fcfs; j++) {
        for (int t = exec_fcfs[j].debut; t < exec_fcfs[j].fin; t++) {
            if (t < temps_max) fcfs_display[t] = exec_fcfs[j].pid;
        }
    }
    
    for (int j = 0; j < nb_exec_sjf; j++) {
        for (int t = exec_sjf[j].debut; t < exec_sjf[j].fin; t++) {
            if (t < temps_max) sjf_display[t] = exec_sjf[j].pid;
        }
    }
    
    for (int k = 0; k < nb_processus; k++) {
        if (tableau[k].arrivee < temps_max) {
            arr_display[tableau[k].arrivee] = 1; // Marque juste la présence
        }
    }
    

    // Affichage de la ligne des arrivées
    printf("\nArr. : ");
    for (int i = 0; i < temps_max; i++) {
        if (arr_display[i]) {
            printf("^  "); // "^" + 2 espaces = 3 caractères
        } else {
            printf("   "); // 3 espaces
        }
    }
    printf("\n");
    
    // Affichage FCFS
    printf("FCFS : ");
    for (int i = 0; i < temps_max; i++) {
        if (fcfs_display[i] > 0) {
            printf("\033[0;3%dmP%d\033[0m", (fcfs_display[i] % 6) + 1, fcfs_display[i]);
            printf(" "); // Ajouter 1 espace après pour avoir 3 caractères au total
        } else {
            printf("   "); // 3 espaces
        }
    }
    printf("\n");
    
    // Affichage SJF
    printf("SJF  : ");
    for (int i = 0; i < temps_max; i++) {
        if (sjf_display[i] > 0) {
            printf("\033[0;3%dmP%d\033[0m", (sjf_display[i] % 6) + 1, sjf_display[i]);
            printf(" "); // Ajouter 1 espace après pour avoir 3 caractères au total
        } else {
            printf("   "); // 3 espaces
        }
    }
    printf("\n");
    
    printf("\nLegende : ");
    for (int i = 0; i < nb_processus; i++) {
        printf("\033[0;3%dmP%d\033[0m ", (tableau[i].pid % 6) + 1, tableau[i].pid);
    }
    printf("\n");
    
    // ET ici, on oublie pas de tut libérer
    free(exec_fcfs);
    free(exec_sjf);
    
    free(fcfs_display);
    free(sjf_display);
    free(arr_display);
}



