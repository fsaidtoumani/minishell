/*
  Projet minishell - Licence 3 Info - PSI 2023
 
  Nom :
  Prénom :
  Num. étudiant :
  Groupe de projet :
  Date :
 
  Gestion des commandes internes du minishell (implémentation).
 
 */

#include "cmd.h"

int is_builtin(const char* cmd) {
  if(strcmp(cmd,"cd") == 0 || strcmp(cmd,"export") == 0 || strcmp(cmd,"exit") == 0)
    return 1;
  return 0;
}


int cd(const char* path, int fderr) {
  if(chdir(path) != 0){
      ssize_t trash=write(fderr,"Erreur : cd failed",18);
      (void)trash;
      return 1;
    }
  return 0;// tout c'est bien passé
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

int exit_shell(int ret, int fderr) {
  exit(ret);
  return 2;// on retourn un code d'erreur
}


int builtin(cmd_t* cmd) {
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
    if(exit_shell(0,cmd->stderr) ==0)
      return 0;
    return -1;
  }
  if (strcmp(cmd->path,"export") == 0)
  {
    int i=1;
    char *pos=NULL;
    char			*name;
	  char			*value;
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
  return 0;
}