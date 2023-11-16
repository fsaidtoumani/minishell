/*
  Projet minishell - Licence 3 Info - PSI 2023
 
  Nom :
  Prénom :
  Num. étudiant :
  Groupe de projet :
  Date :
 
  Gestion des processus (implémentation).
 
 */

#include "cmd.h"

int exec_cmd(cmd_t* p) {
  if ((p->pid = fork()) == 0)// si je suis le fils
  {
    dup2(p->stdin,0);
    dup2(p->stdout,1);
    dup2(p->stderr,2);
    int j = 0;
    while (p->fdclose[j] != -1)
    {
        close(p->fdclose[j]); // on ferme les descripteurs
        j++;
    }
    execvp(p->path, p->argv);
    
    exit(1);
  }
  return 0;
}

int init_cmd(cmd_t* p) {
  p->stdin = STDIN_FILENO;
  p->stdout = STDOUT_FILENO;
  p->stderr = STDERR_FILENO;
  p->wait = 1; // attendre la fin de la commande
  p->next = NULL;
  p->next_success = NULL;
  p->next_failure = NULL;
  return 0;
}

int parse_cmd(char* tokens[], cmd_t* cmds, size_t max) {
    int nbc = 0;   // le nombre de commande qu'on a
  int idxfd = 0; // l'index des descripteurs qu'on va fermer
  for (size_t i = 0; i < max; i++)
  {
    cmds[nbc].path = tokens[i];

    while (i < max && strcmp(tokens[i], "2>") != 0 && strcmp(tokens[i], "2>>") != 0 && strcmp(tokens[i], ">") != 0 && strcmp(tokens[i], ">>") != 0 && strcmp(tokens[i], "<") != 0 && strcmp(tokens[i], "||") != 0 && strcmp(tokens[i], "&&") != 0 && strcmp(tokens[i], "&") != 0 && strcmp(tokens[i], "|") != 0 && strcmp(tokens[i], ";") != 0 && tokens[i] != NULL) // on peut remplacer par *(cmdtoks+i)
    {
      cmds[nbc].argv[idxfd] = tokens[i];
      idxfd++;
      i++;
    }
    if(i < max  && (strcmp(tokens[i], "2>") == 0 || strcmp(tokens[i], "2>>") == 0 || strcmp(tokens[i], ">") == 0 || strcmp(tokens[i], ">>") == 0)) i+=2; // pour ne pas prendre le nom du  fichier
    cmds[nbc].argv[idxfd] = NULL;
    init_cmd(&cmds[nbc]);
    idxfd = 0;
    nbc++;
  }
  return nbc;
  
}

/*
Fonction init_pipe
Paramètre cmdtoks : tableau de caractère réprésentant nos commande
Paramètre processes : tableau de processus pour tous les processus de notre programmme 
Paramètre max : c'est la taille max qu'on a 
Paramètre fdclosep : réprésente le tableau de descripteur à fermé
Retourne un entier, qui permet de savoir si tout c'est bien passé 
on initialise les pipes, les redirection,........
*/

int init_pipe(char* cmdtoks[],cmd_t* p, int * fdclosep, size_t max,size_t nbp){

  int idxp = 0; // index de notre processus et en même temps le nombre de processus
  int cpt = 0;  // pour le compter de fdclose su pere
  for (size_t i = 0; i < max; i++)
  {
    if(strcmp(cmdtoks[i], "&&") == 0)
    {
      //p[idxp].wait = 0;// il n'attends pas la fin 
      p[idxp].next_success = &p[idxp + 1];
      idxp++;
      continue;
    }

    if(strcmp(cmdtoks[i], "||") == 0)
    {
      //p[idxp].wait = 0;// il n'attends pas la fin 
      p[idxp].next_failure = &p[idxp + 1];
      idxp++;
      continue;
    }
    if (strcmp(cmdtoks[i], "&") == 0)
    {
      p[idxp].wait = 0;
      p[idxp].next = &p[idxp + 1];
      idxp++;
      continue;
    }
    // si on tombe sur un ';' on va au processus suivant donc on met un -2 pour savoir qu'on change complement de processus
    if (strcmp(cmdtoks[i], ";") == 0)
    {
      fdclosep[cpt] = -2;// -2 pour montrer qu on a fini une commande et que on va lancer une autre commande
      cpt++;
      p[idxp].next = &p[idxp + 1];
      idxp++;
      continue;
    }
    // printf("ok1 ");
    if (strcmp(cmdtoks[i], "|") == 0)
    {
      int tube[2];
      int pere= pipe(tube);
      if(pere == -1)
      {
        perror("Erreur de pipe :");
        return -1;
      }
      p[idxp].stdout = tube[1];// il ecrit dans le tube
      p[idxp + 1].stdin = tube[0];// il va lire dans le tube
      p[idxp].wait = 0;// il n'attends pas la fin 
      p[idxp].next = &p[idxp + 1];

      fdclosep[cpt] = tube[0];cpt++;
      fdclosep[cpt] = tube[1];cpt++;
      idxp++;
      continue;
    }
    if (strcmp(cmdtoks[i], ">") == 0)
    {
      // celui la concatene au debut du fichier
      //fdclosep[cpt] = -2; cpt++;

      //processes[idxp].wait = 0;
      int fdout = open(cmdtoks[i + 1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
      if (fdout==-1) {
        perror("Erreur de redirection :");
        return -1;
      }
      p[idxp].stdout = fdout;
      // retourner 1 si on arrive pas à ouvrir le fichier en ecriture
      fdclosep[cpt] = p[idxp].stdout;
      cpt++;
      i++;// pour ne pas prendre le nom dufichier
      continue;
    }
    if (strcmp(cmdtoks[i], ">>") == 0)
    {
      // celui la concatene au debut du fichier
      //fdclosep[cpt] = -2; cpt++;
      //p[idxp].wait = 0;
      p[idxp].stdout = open(cmdtoks[i + 1], O_CREAT| O_WRONLY  | O_APPEND, 0644);
      if (p[idxp].stdout == -1)
      { perror("Erreur de redirection :");
        return -1;
        //exit(1);
      }
      // retourner 1 si on arrive pas à ouvrir le fichier en ecriture
      fdclosep[cpt] = p[idxp].stdout;
      cpt++;
      i++; // pour ne pas à reprendre le nom du fichier
      continue;
    }
    
    if (strcmp(cmdtoks[i], "2>") == 0)
    {
      // celui la concatene au debut du fichier
      //fdclosep[cpt] = -2; cpt++;
      //processes[idxp].wait = 0;
      int fdout = open(cmdtoks[i + 1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
      if (fdout==-1) {
        perror("Erreur de redirection :");
        return -1;
      }
      p[idxp].stderr = fdout;
      // retourner 1 si on arrive pas à ouvrir le fichier en ecriture
      fdclosep[cpt] = fdout;
      cpt++;
      i++;// pour ne pas prendre le nom dufichier
      continue;
    }
    if (strcmp(cmdtoks[i], "2>>") == 0)
    {
      // celui la concatene au debut du fichier
      //fdclosep[cpt] = -2; cpt++;
      //p[idxp].wait = 0;
      p[idxp].stderr = open(cmdtoks[i + 1], O_CREAT| O_WRONLY  | O_APPEND, 0644);
      if (p[idxp].stdout == -1)
      { perror("Erreur de redirection :");
        return -1;
        //exit(1);
      }
      // retourner 1 si on arrive pas à ouvrir le fichier en ecriture
      fdclosep[cpt] = p[idxp].stderr;
      cpt++;
      i++; // pour ne pas à reprendre le nom du fichier
      continue;
    }
    if (strcmp(cmdtoks[i], "<") == 0)
    {
      p[idxp].wait = 0;
      p[idxp].stdin = open(cmdtoks[i + 1], O_RDONLY | O_CREAT , 0640);
      if (p[idxp].stdout == -1) exit(1);
      fdclosep[cpt] = p[idxp].stdin;
      cpt++;
      p[idxp].next = &p[idxp + 1];
      idxp++;
      i++; // pour ne pas à reprendre le nom du fichier
      continue;
    }
  }
  fdclosep[cpt] = -1; // on remet le dernier descripteur à -1 pour le pere


  /*

  */

  for (size_t i = 0; i < nbp; i++)
  {
    int ind = 0;
    while (fdclosep[ind] != -1)
    {
      p[i].fdclose[ind] = fdclosep[ind];
      ind++;
    }
    p[i].fdclose[ind] = -1; // on remet le -1 à la fin de pour chaque processus
  }
  return 0;
}
