#ifndef TP3_H
#define TP3_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Structure de donnees */
typedef struct s_processus s_processus;
struct s_processus {
    int pid;
    int arrivee;
    int duree;
    struct s_processus *suivant;
};

typedef s_processus t_processus;

/* Structure pour stocker une execution */
typedef struct {
    int pid;
    int debut;
    int fin;
} t_execution;

/* Fonctions communes */
t_processus* creer_processus(int pid, int arrivee, int duree);
void free_processus(t_processus* p);

/* Chargement depuis fichier */
t_processus* charger_processus(char* nom_fichier, int* nb_processus);

/* Fonctions FIFO (FCFS) */
t_processus* fifo_init();
t_processus* fifo_clear(t_processus* file);
int fifo_vide(t_processus* file);
t_processus* fifo_enfiler(t_processus* file, t_processus* p);
t_processus* fifo_defiler(t_processus** file);
void simuler_fcfs(t_processus* tableau, int nb_processus);

/* Fonctions liste triee (SJF) */
t_processus* lsorted_init();
t_processus* lsorted_clear(t_processus* liste);
int lsorted_vide(t_processus* liste);
t_processus* lsorted_inserer_trie(t_processus* liste, t_processus* p);
t_processus* lsorted_extraire_premier(t_processus** liste);
void simuler_sjf(t_processus* tableau, int nb_processus);

/* Fonctions pour le diagramme de Gantt */
int simuler_fcfs_gantt(t_processus* tableau, int nb_processus, t_execution* exec);
int simuler_sjf_gantt(t_processus* tableau, int nb_processus, t_execution* exec);
void afficher_gantt(t_processus* tableau, int nb_processus);

#endif
