/*
  Projet minishell - Licence 3 Info - PSI 2023

  Nom : SAID TOUMANI | SCHMITT | LOPEZ
  Prénom :Fahardine | Lucas | Baptiste
  Num. étudiant :22000032 | 22000511 | 22001792 ;
  Groupe de projet : Binôme 4
  Date :24/11/4023
 
  Gestion des commandes internes du minishell (implémentation).
 
 */

#include "cmd.h"
#include "localvariables.h"

int is_builtin(const char* cmd) {
  if(strcmp(cmd,"cd") == 0 || strcmp(cmd,"export") == 0 || strcmp(cmd,"exit") == 0 || strcmp(cmd,"unset") == 0 || strchr(cmd,'=') != NULL)
    return 1;
  return 0;
}


int cd(const char* path, int fderr) {
    if (chdir(path) != 0) {
        char error_message[256];
        snprintf(error_message, sizeof(error_message), "Minishell: cd: %s: Aucun fichier ou dossier de ce type\n", path);
        int trash=write(fderr, error_message, strlen(error_message));
        (void)trash; // pour éviter le warning
        return 1; // Échec du changement de répertoire
    }
    return 0; // Succès
}


int export_(const char* var, const char* value, int fderr) {
  if(setenv(var, value,1) != 0) 
  {
    ssize_t trash = write(fderr,"Erreur : export failed\n",22);
    (void)trash;
    return 1;
  }
  //printf("name:%s et value:%s\n", var, value);
  return 0;
}

int unset_(const char* var, int fderr) {
  if(unsetenv(var) != 0)
  {
    ssize_t trash = write(fderr,"Erreur : unset failed\n",22);
    (void)trash;
    return 1;
  }
  return 0;
}

int exit_shell(int ret, int fderr) {
  exit(ret);
  return 0;// on retourn un code d'erreur
}


int builtin(cmd_t* cmd,AVLvariables** variableslocal) {
  if(strcmp(cmd->path,"cd") == 0)
  {
    if(cmd->argv[1]!= NULL)
    {
      if(cd(cmd->argv[1],cmd->stderr) == 0)// si tout c'est bien passé
        return 0;
      return -1;
    }
    else
    {
      if(cd("/",cmd->stderr) == 0)// si tout c'est bien passé
        return 0;
      return -1;
    }
    
  }

  if(strcmp(cmd->path,"exit") == 0)
  {
    int coderetour = 0;
    if(cmd->argv[1] != NULL)
      coderetour = atoi(cmd->argv[1]);
      
    if(exit_shell(coderetour,cmd->stderr) == 0)
      return 0;
    return -1;
  }

  if (strcmp(cmd->path,"export") == 0)
  {
    int i=1;
    char *pos=NULL;
    char *name;
	  char *value;
    value = "";
    while(cmd->argv[i] != NULL)
    {
      pos = strchr(cmd->argv[i], '=');
      if (pos)// premier occurrence de =
      {
        name = strndup(cmd->argv[i],(pos - cmd->argv[i]));
        value = strndup(pos+1,strlen(pos+1));
      }
      if(!pos)// si y'a par le caractère '=' , on appelle la fonction avec le nom seulement et la valeur vide
        export_(cmd->argv[i],value,cmd->stderr);
      else
      {
        export_(name,value,cmd->stderr);
        free(name);
        free(value);
      }
      i++;
    }
  }

  if(strcmp(cmd->path,"unset") == 0)
  {
    int i=1;
    while(cmd->argv[i] != NULL)
    {
      if(unsetenv(cmd->argv[i]) != 0)
      {
        ssize_t trash = write(cmd->stderr,"Erreur : unset failed\n",22);
        (void)trash;
        return -1;
      }
      *variableslocal=insertAVLvariables(*variableslocal,cmd->argv[i],"");
      i++;
    }
  }
  
  //Affection des variables locales
  if(strchr(cmd->path,'=') != NULL)
  {
    int i=0;
    char *pos=NULL;
    char *name=cmd->path;
    char *value=NULL;
    value = "";
    while (cmd->path[i] != '=')
    {
      i++;
    }
    pos = strchr(cmd->path, '=');
    if (pos)// premier occurrence de =
    {
      name = strndup(cmd->path,(pos - cmd->path));
      value = strndup(pos+1,strlen(pos+1));

      //on ajoute la variable dans l'arbre
      *variableslocal = insertAVLvariables(*variableslocal,name,value);


      free(name);
      free(value);
    }
    
  }
  return 0;
}