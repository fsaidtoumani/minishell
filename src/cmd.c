/*
  Projet minishell - Licence 3 Info - PSI 2023

  Nom : SAID TOUMANI | SCHMITT | LOPEZ
  Prénom :Fahardine | Lucas | Baptiste
  Num. étudiant :22000032 | 22000511 | 22001792 ;
  Groupe de projet : Binôme 4
  Date :24/11/4023

  Gestion des processus (implémentation).

 */

#include "cmd.h"
#include "localvariables.h"

int exec_cmd(cmd_t *p, AVLvariables *variableslocal)
{
  // On fait la substitution des variables d'environnement
  for (size_t i = 0; p->argv[i] != NULL; i++)
  {

    substenv(p->argv[i], MAX_CMD_SIZE, variableslocal);

    // On verifi si ca ne commenc pas par " sinon on les enleves
    if (p->argv[i][0] == '"')
    {
      // On decalle la chaine de 1 vers le debut et on decal le deniere caractere de -1 aussi
      memmove(p->argv[i], p->argv[i] + 1, strlen(p->argv[i]));
      if (p->argv[i][strlen(p->argv[i]) - 1] == '"')
        p->argv[i][strlen(p->argv[i]) - 1] = '\0';
      // On verifie si il y a pas un echapateur dans la chaine "\" et """
      for (size_t j = 0; j < strlen(p->argv[i]); j++)
      {
        char c = p->argv[i][j];
        if (c == '\\' || c == '"')
        {
          // On decalle la chaine de 1 vers le debut et on decal le deniere caractere de -1 aussi
          memmove(p->argv[i] + j, p->argv[i] + j + 1, strlen(p->argv[i]) - j);
          j++;
        }
      }
    }
    // On verifi si ca ne commenc pas par ' sinon on les enleves
    if (p->argv[i][0] == '\'')
    {
      // On decalle la chaine de 1 vers le debut et on decal le deniere caractere de -1 aussi
      memmove(p->argv[i], p->argv[i] + 1, strlen(p->argv[i]));
      if (p->argv[i][strlen(p->argv[i]) - 1] == '\'')
        p->argv[i][strlen(p->argv[i]) - 1] = '\0';
      // On verifie si il y a pas un echapateur dans la chaine "\" et """
      for (size_t j = 0; j < strlen(p->argv[i]); j++)
      {
        char c = p->argv[i][j];
        if (c == '\\' || c == '\'')
        {
          // On decalle la chaine de 1 vers le debut et on decal le deniere caractere de -1 aussi
          memmove(p->argv[i] + j, p->argv[i] + j + 1, strlen(p->argv[i]) - j);
          j++;
        }
      }
    }
  }

  if ((p->pid = fork()) == 0) // si je suis le fils
  {
    dup2(p->stdin, 0);
    dup2(p->stdout, 1);
    dup2(p->stderr, 2);
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

int init_cmd(cmd_t *p)
{
  p->stdin = STDIN_FILENO;
  p->stdout = STDOUT_FILENO;
  p->stderr = STDERR_FILENO;
  p->wait = 1; // attendre la fin de la commande
  p->next = NULL;
  p->next_success = NULL;
  p->next_failure = NULL;
  return 0;
}

int parse_cmd(char *tokens[], cmd_t *cmds, size_t max)
{
  int nbc = 0;   // le nombre de commande qu'on a
  int idxfd = 0; // l'index des descripteurs qu'on va fermer
  for (size_t i = 0; i < max; i++)
  {
    cmds[nbc].path = tokens[i];
    // On reduit les tests par rapport au 2 carractere car on aura deja traiter les formes de redirection
    while (i < max &&
           strncmp(tokens[i], "0>", 2) != 0 &&
           strncmp(tokens[i], "1>", 2) != 0 &&
           strncmp(tokens[i], "2>", 2) != 0 &&
           strncmp(tokens[i], ">", 1) != 0 &&
           strcmp(tokens[i], "<") != 0 &&
           strncmp(tokens[i], "&", 1) != 0 &&
           strncmp(tokens[i], "|", 1) != 0 &&
           strcmp(tokens[i], ";") != 0 &&
           tokens[i] != NULL)
    {
      // On verifi si ca ne commenc pas par " sinon on les enleves
      if (tokens[i][0] == '"')
      {
        // On decalle la chaine de 1 vers le debut et on decal le deniere caractere de -1 aussi
        memmove(tokens[i], tokens[i] + 1, strlen(tokens[i]));
        if (tokens[i][strlen(tokens[i]) - 1] == '"')
          tokens[i][strlen(tokens[i]) - 1] = '\0';
        // On verifie si il y a pas un echapateur dans la chaine "\" et """
        for (size_t j = 0; j < strlen(tokens[i]); j++)
        {
          char c = tokens[i][j];
          if (c == '\\' || c == '"')
          {
            // On decalle la chaine de 1 vers le debut et on decal le deniere caractere de -1 aussi
            memmove(tokens[i] + j, tokens[i] + j + 1, strlen(tokens[i]) - j);
            j++;
          }
        }
      }
      // On verifi si ca ne commenc pas par ' sinon on les enleves
      if (tokens[i][0] == '\'')
      {
        // On decalle la chaine de 1 vers le debut et on decal le deniere caractere de -1 aussi
        memmove(tokens[i], tokens[i] + 1, strlen(tokens[i]));
        if (tokens[i][strlen(tokens[i]) - 1] == '\'')
          tokens[i][strlen(tokens[i]) - 1] = '\0';
        // On verifie si il y a pas un echapateur dans la chaine "\" et """
        for (size_t j = 0; j < strlen(tokens[i]); j++)
        {
          char c = tokens[i][j];
          if (c == '\\' || c == '\'')
          {
            // On decalle la chaine de 1 vers le debut et on decal le deniere caractere de -1 aussi
            memmove(tokens[i] + j, tokens[i] + j + 1, strlen(tokens[i]) - j);
            j++;
          }
        }
      }

      // Si ya un echapement on le supprime
      char *pos = strchr(tokens[i], '\\');
      if (pos != NULL)
      {
        memmove(pos, pos + 1, strlen(pos));
      }

      cmds[nbc].argv[idxfd] = tokens[i];
      idxfd++;
      i++;
    }
    if (i < max &&
        (strcmp(tokens[i], "2>") == 0 ||
         strcmp(tokens[i], "2>>") == 0 ||
         strcmp(tokens[i], ">") == 0 ||
         strcmp(tokens[i], ">>") == 0 ||
         strncmp(tokens[i], "0>", 2) == 0 ||
         strncmp(tokens[i], "1>", 2) == 0 ||
         strncmp(tokens[i], "2>", 2) == 0))
      i += 2; // pour ne pas prendre le nom du  fichier
    cmds[nbc].argv[idxfd] = NULL;
    init_cmd(&cmds[nbc]);
    idxfd = 0;
    nbc++;
  }
  return nbc;
}

int init_pipe(char *cmdtoks[], cmd_t *cmds, int *fdclosep, size_t max, size_t nbp)
{

  int idxp = 0; // index de notre processus et en même temps le nombre de processus
  int cpt = 0;  // pour le compter de fdclose su pere
  for (size_t i = 0; i < max; i++)
  {
    if (strcmp(cmdtoks[i], "&&") == 0)
    {
      // p[idxp].wait = 0;// il n'attends pas la fin
      if(cmdtoks[i+1] == NULL)
        cmds[idxp].next_success = NULL;
      else
        cmds[idxp].next_success = &cmds[idxp + 1];
      idxp++;
      continue;
    }

    if (strcmp(cmdtoks[i], "||") == 0)
    {
      if(cmdtoks[i+1] == NULL)
        cmds[idxp].next_failure = NULL;
      else
        cmds[idxp].next_failure = &cmds[idxp + 1];
      idxp++;
      continue;
    }
    if (strcmp(cmdtoks[i], "&") == 0)
    {
      cmds[idxp].wait = 0; // il n'attends pas la fin
      if(cmdtoks[i+1] == NULL)
        cmds[idxp].next = NULL;
      else
        cmds[idxp].next = &cmds[idxp + 1];
      idxp++;
      continue;
    }
    // si on tombe sur un ';' on va au processus suivant donc on met un -2 pour savoir qu'on change complement de processus
    if (strcmp(cmdtoks[i], ";") == 0)
    {
      fdclosep[cpt] = -2; // -2 pour montrer qu on a fini une commande et que on va lancer une autre commande
      cpt++;
      if(cmdtoks[i+1] == NULL)
        cmds[idxp].next = NULL;
      else
        cmds[idxp].next = &cmds[idxp + 1];
      idxp++;
      continue;
    }

    if (strcmp(cmdtoks[i], "|") == 0)
    {
      int tube[2];
      int pere = pipe(tube);
      if (pere == -1)
      {
        perror("Erreur de pipe :");
        return -1;
      }
      cmds[idxp].stdout = tube[1];    // il ecrit dans le tube
      cmds[idxp + 1].stdin = tube[0]; // il va lire dans le tube
      cmds[idxp].wait = 0;            // il n'attends pas la fin
      if(cmdtoks[i+1] == NULL)
        cmds[idxp].next = NULL;
      else
        cmds[idxp].next = &cmds[idxp + 1];

      fdclosep[cpt] = tube[0];
      cpt++;
      fdclosep[cpt] = tube[1];
      cpt++;

      idxp++;
      continue;
    }

    // Si c'est une redirection de sortie cas de base
    if (cmdtoks[i][0] == '>')
    {
      // On va traiter le cas de la redirection de sortie

      // Redirection de l'entrée standard vers la sortie standard en mode écrasement
      if (strcmp(cmdtoks[i], ">") == 0)
      {
        int fdout = open(cmdtoks[i + 1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
        if (fdout == -1)
        {
          perror("Erreur de redirection :");
          return -1;
        }
        cmds[idxp].stdout = fdout;
        // retourner 1 si on arrive pas à ouvrir le fichier en ecriture
        fdclosep[cpt] = cmds[idxp].stdout;
        cpt++;
        i++; // pour ne pas prendre le nom dufichier
        continue;
      }

      // Redirection de l'entrée standard vers la sortie standard en mode concaténation
      if (strcmp(cmdtoks[i], ">>") == 0)
      {
        cmds[idxp].stdout = open(cmdtoks[i + 1], O_CREAT | O_WRONLY | O_APPEND, 0644);
        if (cmds[idxp].stdout == -1)
        {
          perror("Erreur de redirection :");
          return -1;
          // exit(1);
        }
        // retourner 1 si on arrive pas à ouvrir le fichier en ecriture
        fdclosep[cpt] = cmds[idxp].stdout;
        cpt++;
        i++; // pour ne pas à reprendre le nom du fichier
        continue;
      }

      // Redirection de l'entrée standard vers la sortie d'erreur
      if (strcmp(cmdtoks[i], ">&2") == 0)
      {
        cmds[idxp].stdout = cmds[idxp].stderr;
        // retourner 1 si on arrive pas à ouvrir le fichier en ecriture
        fdclosep[cpt] = cmds[idxp].stdout;
        cpt++;
        continue;
      }
      // Redirection de l'entrée standard vers la sortie standard
      if (strcmp(cmdtoks[i], ">&1") == 0)
      {
        cmds[idxp].stdout = cmds[idxp].stdout;
        // retourner 1 si on arrive pas à ouvrir le fichier en ecriture
        fdclosep[cpt] = cmds[idxp].stdout;
        cpt++;
        continue;
      }

      // Redirection de l'entrée standard vers la entré standard
      if (strcmp(cmdtoks[i], ">&0") == 0)
      {

        cmds[idxp].stdout = cmds[idxp].stdin;
        // retourner 1 si on arrive pas à ouvrir le fichier en ecriture
        fdclosep[cpt] = cmds[idxp].stdout;
        cpt++;
        continue;
      }
    }
    // Si c'est une redirection d'entrée cas de base
    if (cmdtoks[i][0] == '<')
    {

      // Redirection de l'entrée standard vers la sortie standard en mode écrasement
      if (strcmp(cmdtoks[i], "<") == 0)
      {
        cmds[idxp].wait = 0;
        cmds[idxp].stdin = open(cmdtoks[i + 1], O_RDONLY);
        if (cmds[idxp].stdout == -1)
          exit(1);
        fdclosep[cpt] = cmds[idxp].stdin;
        cpt++;
        cmds[idxp].next = &cmds[idxp + 1];
        idxp++;
        i++; // pour ne pas à reprendre le nom du fichier
        continue;
      }

      // Redirection de l'entrée standard vers la sortie standard e
      if (strcmp(cmdtoks[i], "<&1") == 0)
      {
        cmds[idxp].stdin = cmds[idxp].stdout;
        // retourner 1 si on arrive pas à ouvrir le fichier en ecriture
        fdclosep[cpt] = cmds[idxp].stdin;
        cpt++;
        continue;
      }

      // Redirection de l'entrée standard vers la sortie d'erreur
      if (strcmp(cmdtoks[i], "<&2") == 0)
      {
        cmds[idxp].stdin = cmds[idxp].stderr;
        // retourner 1 si on arrive pas à ouvrir le fichier en ecriture
        fdclosep[cpt] = cmds[idxp].stdin;
        cpt++;
        continue;
      }

      // Redirection de l'entrée standard vers la entré standard
      if (strcmp(cmdtoks[i], "<&0") == 0)
      {
        cmds[idxp].stdin = cmds[idxp].stdin;
        // retourner 1 si on arrive pas à ouvrir le fichier en ecriture
        fdclosep[cpt] = cmds[idxp].stdin;
        cpt++;
        continue;
      }
    }

    // Traitement des redirections par entrée standard
    switch (cmdtoks[i][0])
    {
    case '0': // on traite les cas de redirection d'entrée
      if (strcmp(cmdtoks[i], "0>") == 0)
      {
        int fdout = open(cmdtoks[i + 1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
        if (fdout == -1)
        {
          perror("Erreur de redirection :");
          return -1;
        }
        cmds[idxp].stdin = fdout;
        // retourner 1 si on arrive pas à ouvrir le fichier en ecriture
        fdclosep[cpt] = fdout;
        cpt++;
        i++; // pour ne pas prendre le nom dufichier
        break;
      }

      // Redirection de l'entrée standard vers la sortie d'erreur en mode concaténation
      if (strcmp(cmdtoks[i], "0>>") == 0)
      {
        cmds[idxp].stdin = open(cmdtoks[i + 1], O_CREAT | O_WRONLY | O_APPEND, 0644);
        if (cmds[idxp].stdout == -1)
        {
          perror("Erreur de redirection :");
          return -1;
        }
        // retourner 1 si on arrive pas à ouvrir le fichier en ecriture
        fdclosep[cpt] = cmds[idxp].stdin;
        cpt++;
        i++; // pour ne pas à reprendre le nom du fichier
        break;
      }

      // Redirection de la sortie standard d'erreur vers la sortie standard
      if (strcmp(cmdtoks[i], "0>&1") == 0)
      {
        cmds[idxp].stdin = cmds[idxp].stdout;
        // retourner 1 si on arrive pas à ouvrir le fichier en ecriture
        fdclosep[cpt] = cmds[idxp].stdin;
        cpt++;
        break;
      }

      // Redirection de la sortie standard d'erreur vers la sortie d'erreur
      if (strcmp(cmdtoks[i], "0>&0") == 0)
      {
        cmds[idxp].stdin = cmds[idxp].stdin;
        // retourner 1 si on arrive pas à ouvrir le fichier en ecriture
        fdclosep[cpt] = cmds[idxp].stdin;
        cpt++;
        break;
      }

      // Redirection de la sortie standard d'erreur vers la sortie standard d'erreur
      if (strcmp(cmdtoks[i], "0>&2") == 0)
      {
        cmds[idxp].stdin = cmds[idxp].stderr;
        // retourner 1 si on arrive pas à ouvrir le fichier en ecriture
        fdclosep[cpt] = cmds[idxp].stdin;
        cpt++;
        break;
      }
      break;
    case '1': // on traite les cas de redirection de sortie
      // Redirection de l'entrée standard vers la sortie d'erreur en mode écrasement
      if (strcmp(cmdtoks[i], "1>") == 0)
      {
        int fdout = open(cmdtoks[i + 1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
        if (fdout == -1)
        {
          perror("Erreur de redirection :");
          return -1;
        }
        cmds[idxp].stdout = fdout;
        // retourner 1 si on arrive pas à ouvrir le fichier en ecriture
        fdclosep[cpt] = cmds[idxp].stdout;
        cpt++;
        i++; // po
      }

      // Redirection de l'entrée standard vers la sortie d'erreur en mode concaténation
      if (strcmp(cmdtoks[i], "1>>") == 0)
      {
        cmds[idxp].stdout = open(cmdtoks[i + 1], O_CREAT | O_WRONLY | O_APPEND, 0644);
        if (cmds[idxp].stdout == -1)
        {
          perror("Erreur de redirection :");
          return -1;
        }
        // retourner 1 si on arrive pas à ouvrir le fichier en ecriture
        fdclosep[cpt] = cmds[idxp].stdout;
        cpt++;
        i++; // pour ne pas à reprendre le nom du fichier
        break;
      }

      // Redirection de la sortie standard vers la sortie standard
      if (strcmp(cmdtoks[i], "1>&1") == 0)
      {
        cmds[idxp].stdout = cmds[idxp].stdout;
        // retourner 1 si on arrive pas à ouvrir le fichier en ecriture
        // fdclosep[cpt] =cmds[idxp].stdout;
        cpt++;
        break;
      }

      // Redirection de la sortie standard d'erreur vers la sortie d'erreur
      if (strcmp(cmdtoks[i], "1>&0") == 0)
      {
        cmds[idxp].stdout = cmds[idxp].stdin;
        // retourner 1 si on arrive pas à ouvrir le fichier en ecriture
        fdclosep[cpt] = cmds[idxp].stdout;
        cpt++;
        break;
      }

      // Redirection de la sortie standard d'erreur vers la sortie standard d'erreur
      if (strcmp(cmdtoks[i], "1>&2") == 0)
      {
        cmds[idxp].stdout = cmds[idxp].stderr;
        // retourner 1 si on arrive pas à ouvrir le fichier en ecriture
        fdclosep[cpt] = cmds[idxp].stdout;
        cpt++;
        break;
      }
      break;
    case '2': // on traite les cas de redirection d'erreur
      // Redirection de l'entrée standard vers la sortie d'erreur en mode écrasement
      if (strcmp(cmdtoks[i], "2>") == 0)
      {
        int fdout = open(cmdtoks[i + 1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
        if (fdout == -1)
        {
          perror("Erreur de redirection :");
          return -1;
        }
        cmds[idxp].stderr = fdout;
        // retourner 1 si on arrive pas à ouvrir le fichier en ecriture
        fdclosep[cpt] = fdout;
        cpt++;
        i++; // pour ne pas prendre le nom dufichier
        break;
      }

      // Redirection de l'entrée standard vers la sortie d'erreur en mode concaténation
      if (strcmp(cmdtoks[i], "2>>") == 0)
      {
        cmds[idxp].stderr = open(cmdtoks[i + 1], O_CREAT | O_WRONLY | O_APPEND, 0644);
        if (cmds[idxp].stdout == -1)
        {
          perror("Erreur de redirection :");
          return -1;
        }
        // retourner 1 si on arrive pas à ouvrir le fichier en ecriture
        fdclosep[cpt] = cmds[idxp].stderr;
        cpt++;
        i++; // pour ne pas à reprendre le nom du fichier
        break;
      }

      // Redirection de la sortie standard d'erreur vers la sortie standard
      if (strcmp(cmdtoks[i], "2>&1") == 0)
      {
        if (cmds[idxp].stdout < 0)
        {
          perror("stdout n'est pas un descripteur valide");
          return -1;
        }
        cmds[idxp].stderr = cmds[idxp].stdout;

        break;
      }

      // Redirection de la sortie standard d'erreur vers la sortie d'erreur
      if (strcmp(cmdtoks[i], "2>&0") == 0)
      {
        cmds[idxp].stderr = cmds[idxp].stdin;
        // retourner 1 si on arrive pas à ouvrir le fichier en ecriture
        // fdclosep[cpt] =cmds[idxp].stderr;
        break;
      }

      // Redirection de la sortie standard d'erreur vers la sortie standard d'erreur
      if (strcmp(cmdtoks[i], "2>&2") == 0)
      {
        cmds[idxp].stderr = cmds[idxp].stderr;
        // retourner 1 si on arrive pas à ouvrir le fichier en ecriture
        fdclosep[cpt] = cmds[idxp].stderr;
        break;
      }
      break;
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
      cmds[i].fdclose[ind] = fdclosep[ind];
      ind++;
    }
    cmds[i].fdclose[ind] = -1; // on remet le -1 à la fin de pour chaque processus
  }
  return 0;
}