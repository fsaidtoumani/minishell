/*
  Projet minishell - Licence 3 Info - PSI 2023

  Nom : SAID TOUMANI | SCHMITT | LOPEZ
  Prénom :Fahardine | Lucas | Baptiste
  Num. étudiant :22000032 | 22000511 | 22001792 ;
  Groupe de projet : Binôme 4
  Date :24/11/4023
 
  Gestion des processus.
 
 */

#ifndef _PROCESSUS_H
#define _PROCESSUS_H

#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parser.h"

/*
  Structure de représentation d'une commande à lancer.

  pid: le PID du processus fils
  status: le code de retour du processus fils
  stdin, stdout, stderr: les descripteurs de fichiers
                         pour les entrées/sorties
  wait: attendre la fin de la commande
  fdclose: descripteurs de fichiers à fermer
  next: commande suivante
  next_success: commande à lancer en cas de succès
  next_failure: commande à lancer en cas d'échec
*/

typedef struct cmd_t {
  pid_t pid;
  int status;
  int stdin, stdout, stderr;
  int wait;
  char* path;
  char* argv[MAX_CMD_SIZE];
  int fdclose[MAX_CMD_SIZE];
  struct cmd_t* next;
  struct cmd_t* next_success;
  struct cmd_t* next_failure;
} cmd_t;

/*
  Lancer la commande en fonction des attributs de
  la structure et initialiser les champs manquants.
 
  On crée un nouveau processus, on détourne
  éventuellement les entrée/sorties.
  On conserve le PID dans la structure du processus
  appelant (le minishell).
  On attend si la commande est lancée en "avant-plan"
  et on initialise le code de retour.
  On rend la main immédiatement sinon.
 project_new
  p : un pointeur sur la structure contenant les
      informations pour l'exécution de la commande.
  variableslocal : un pointeur sur la structure contenant les
      variables locales.
  Retourne 0 ou un code d'erreur.
 */

int exec_cmd(cmd_t* p,AVLvariables * variableslocal);

/*
  Initialiser une structure cmd_t avec les
  valeurs par défaut.
 
  p : un pointeur sur la structure à initialiser
 
  Retourne 0 ou un code d'erreur.
*/

int init_cmd(cmd_t* p);

/*
  Remplit le tableau de commandes en fonction du contenu
  de tokens.
    Ex : {"ls", "-l", "|", "grep", "^a", NULL} =>
         {{path = "ls",
           argv = {"ls", "-l", NULL},
           bg = 1,
           ...},
          {path = "grep",
           argv = {"grep", "^a", NULL},
           bg = 0,
           ...}}
  tokens : le tableau des éléments de la ligne de
           commandes (peut être modifié)
  cmds : le tableau dans lequel sont stockés les
         différentes structures représentant le
         commandes.
  max : le nombre maximum de commandes dans le tableau

  Retourne 0 ou un code d'erreur.
 */
int parse_cmd(char* tokens[], cmd_t* cmds, size_t max);


//=================== ICI , LES FONCTION QUE J'AI EU À AJOUTER DANS LE CODE =================

/*
Cette fonction permet de faire les différents pipe et redirection
que nous aurons a faire dans notre programme
Car c'etait plus simple pour moi de faire une fonction qui fait tout

cmdstoks : le tableau des éléments de la ligne de
           commandes (peut être modifié)

p : le tableau dans lequel sont stockés les
    différentes structures représentant le
    commandes.

fdcloseep : tableau d'entier représentant les descripteurs de sortie et d'entrée

max : le nombre max de "mots" dans cmdtoks

nbp : le nombre de commande
*/
int init_pipe(char* cmdtoks[],cmd_t* p, int * fdclosep, size_t max,size_t nbp);

#endif

