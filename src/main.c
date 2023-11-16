/*
  Projet minishell - Licence 3 Info - PSI 2023
 
  Nom : SAID TOUMANI
  Prénom : Fahardine
  Num. étudiant : 22000032
  Groupe de projet :
  Date : 10/11/2023
 
  Interface du minishell.
 
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#include "parser.h"
#include "cmd.h"
#include "builtin.h"

int main(int argc, char* argv[]) {
  char cmdline[MAX_LINE_SIZE]; // buffer des lignes de commandes
  char* cmdtoks[MAX_CMD_SIZE]; // "mots" de la ligne de commandes
  cmd_t cmds[MAX_CMD_SIZE];
  cmd_t* current;
  int fdclosep[MAX_CMD_SIZE];// le fdclose du père.
  
  while (1) {
    // Effacer les contenus de cmdline, cmdtoks et cmds
    // Initialiser les valeurs par défaut dans cmds (stdin, stdout, stderr, ...)
    
    // Afficher un prompt
    printf("$ ");
    
    // Lire une ligne dans cmdline - Attention fgets enregistre le \n final
    if (fgets(cmdline, MAX_LINE_SIZE, stdin)==NULL) break;
    cmdline[strlen(cmdline)-1]='\0';

    //Quand l'utilisateur tape '\n' on continue la boucle
    if (strcmp(cmdline, "\n") == 0) continue;

    // Traiter la ligne de commande
    //   - supprimer les espaces en début et en fin de ligne
    //   - ajouter d'éventuels espaces autour de ; ! || && & ...
    //   - supprimer les doublons d'espaces
    //   - traiter les variables d'environnement
    trim(cmdline);
    clean(cmdline);
    separate_s(cmdline, "!|&<>;", MAX_CMD_SIZE);


    // Découper la ligne dans cmdtoks
    // Initialisation des cmdtoks à NULL
    for (int j = 0; j < MAX_CMD_SIZE; j++) {
      cmdtoks[j] = NULL;
    }

    // Découper la ligne dans cmdtoks
    int nb_token = strcut(cmdline, ' ', cmdtoks, MAX_CMD_SIZE);  //nb_token est le nombre de token qu'on a
    int nbc = parse_cmd(cmdtoks, cmds, nb_token); // nbc est le nombre de commande qu'on a
    //On va traiter les redirections
    init_pipe(cmdtoks, cmds, fdclosep,nb_token, nbc);
    
    // Traduire la ligne en structures cmd_t dans cmds
    // Les commandes sont chaînées en fonction des séparateurs
    //   - next -> exécution inconditionnelle
    //   - next_success -> exécution si la commande précédente réussit
    //   - next_failure -> exécution si la commande précédente échoue

    // Exécuter les commandes dans l'ordre en fonction des opérateurs
    // de flux
   
    int indxfdclose = 0;
    int indxfdcloseforEtandOU = 0;
    for (current=cmds; current!=NULL; ){
      if(is_builtin(current->path))
        builtin(current);
      else
        exec_cmd(current);
      //le pere doit attendre le fils si jamais il y a un wait un pipe ou une redirection
      if(current->wait == 1){
        indxfdcloseforEtandOU = 0;
        while(fdclosep[indxfdclose]!= -1 && fdclosep[indxfdclose]!=-2){
          //printf("bon");
          // on ferme les fdclose du père
          close(fdclosep[indxfdclose]);
          indxfdclose++;
        }
        if(fdclosep[indxfdclose]==-2) {
          indxfdclose++; // pour ne pas prendre la case du -2
          indxfdcloseforEtandOU = 1;// 1 pour dire q'on a encore une commande à exécuter
        }
        else 
          indxfdclose = 0;
        waitpid(current->pid,&current->status,0);// ou WUNTRACED | WCONTINUED pour les signaux
      }
      if (current->next!=NULL) {
      // Exécution inconditionnelle
        current=current->next;
        continue;
      }
      if (current->next_success!=NULL) {
        if(WEXITSTATUS(current->status) == 0) // Si la commande a réussi (c'est dans le cas du &&)
          current = current->next_success;
          else 
            if(current->next_success != NULL)// si jamais y' un eventuelle ';' le dernier process qui est précédé de  &&
            {
              while (current->next_success != NULL)// on arrive sur le dernier process qui est précédé de  &&
              {
                current = current->next_success;
              }
              // si jamais le &&(et) échou et il a un eventuelle ||(ou) après
              if(current->next_failure != NULL)// si jamais y'a un eventuelle && apres le dernier ||, on exécute la commande du &&
                current = current->next_failure;
                else 
                  if(indxfdcloseforEtandOU)// si on a un ; et une commande après, on exécute celle ci
                  {
                    current = current->next;// on prend pour le dernier, le -> next. next = null si plus de processus
                  }
                  else 
                  current = NULL;// on prend pour le dernier, le -> next. next = null si plus de processus
              }
              //else current = NULL; // Si la commande echoue
              continue;
      }
      if (current->next_failure!=NULL) {
        if(WEXITSTATUS(current->status)) // Si la commande a échoué ( c'est dans le cas du || )
          current = current->next_failure;
        else 
          if(current->next_failure != NULL)// si la commande reussi
          {
            while (current->next_failure != NULL)// on arrive sur le dernier process qui est précédé de ||
            {
              current = current->next_failure;
            }
            // si jamais le ou reussi et il a un eventuelle && après
            if(current->next_success != NULL)// si jamais y'a un eventuelle && apres le dernier ||, on exécute la commande du &&
              current = current->next_success;
            else 
              if(indxfdcloseforEtandOU)// si on a un ; et une commande après, on exécute celle ci
              {
                current = current->next;// on prend pour le dernier, le -> next. next = null si plus de processus
              }
              else // si non, on met à null
                current = NULL;
          }
            //else current = NULL; // si la commande a réussi
            continue;
      }
      current = NULL;
    }
    // c'est ici , on fait le fdclose pour le pere
    // on ferme maintenant dans le père 
    int j = 0;
    while(fdclosep[j]!= -1)
    {
      close(fdclosep[j]);
      j++;
    }
    /* Libérer la mémoire allouée pour les tokens
    for (int i = 0; i < nb_token; i++) {
      if (cmdtoks[i] != NULL) {
        free(cmdtoks[i]); // Libérer la mémoire allouée pour le token
      }
    }*/
  }
  
  fprintf(stderr, "\nGood bye!\n");
  return 0;
}
