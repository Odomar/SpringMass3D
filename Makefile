#
# Les variables d'environnement libG2X, incG2X
# sont definies dans le fichier ~/.bashrc par le script ../install.sh
#
#compilateur
CC = gcc
#compil en mode 'debug' ou optmisée (-O2)
DBG = no

ifeq ($(DBG),yes) #en mode debug
  CFLAGS = -g -Wpointer-arith -Wall -ansi
else              #en mode normal
  CFLAGS = -O2 -ansi
endif

# assemblage des infos de lib. et inc.
lib =   $(libG2X)
# fichiers *.c locaux
src = src/
# fichiers *.h locaux et lib.
inc = -I./include $(incG2X) 
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
	
SimpleOsc : $(bin)PMat.o $(bin)Link.o $(bin)SimpleOsc.o	
	@echo "edition de lien $^ -> $@"
	@$(CC) $^ $(lib) -o $@

Corde1 : $(bin)Mat.o $(bin)Link.o $(bin)Corde1.o	
	@echo "edition de lien $^ -> $@"
	@$(CC) $^ $(lib) -o $@

Corde2 : $(bin)PMat.o $(bin)Link.o $(bin)Corde2.o	
	@echo "edition de lien $^ -> $@"
	@$(CC) $^ $(lib) -o $@

.PHONY : clean cleanall

clean : 
	rm -f $(bin)*.o .tmp*
cleanall : 
	rm -f $(bin)*.o .tmp* $(EXEC)
