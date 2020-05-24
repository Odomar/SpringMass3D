#
# Les variables d'environnement libG3X, incG3X
# sont definies dans le fichier ~/.bashrc par le script ../install.sh
#
#compilateur
CC = gcc
#compil en mode 'debug' ou optmisée (-O2)
DBG = no

ifeq ($(DBG),yes) #en mode debug
  CFLAGS = -g -Wpointer-arith -Wall
else              #en mode normal
  CFLAGS = -O2
endif

# assemblage des infos de lib. et inc.
lib =   $(libG3X)
# fichiers *.c locaux
src = src/
# fichiers *.h locaux et lib.
inc = -I./include $(incG3X)
# fichiers générés
bin = bin/

# règle de compilation générique des objets
$(bin)%.o : $(src)%.c
	@echo "module $@"
	@$(CC) $(CFLAGS) $(inc) -c $< -o $@
	
# règle de compilation générique
% : $(bin)%.o 
	@echo "edition de lien $^ -> $@"
	@$(CC) $^ $(lib) -o $@

all: Flag

Corde : $(bin)PMat.o $(bin)Link.o $(bin)Corde.o	
	@echo "edition de lien $^ -> $@"
	@$(CC) $^ $(lib) -o $@

Flag : $(bin)PMat.o $(bin)Link.o $(bin)Flag.o
	@echo "edition de lien $^ -> $@"
	@$(CC) $^ $(lib) -o $@

.PHONY : clean cleanall

clean : 
	rm -f $(bin)*.o .tmp*
cleanall : 
	rm -f $(bin)*.o .tmp* $(EXEC)
