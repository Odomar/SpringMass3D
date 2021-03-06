/*=============================================================*/
/*= E.Incerti (incerti@univ-eiffel.fr)                        =*/
/*= M2/IMAC3 - ANIMATION / MOTEUR PHYSIQUE                    =*/
/*= Systeme Masses/Ressort en dimension 1 : Corde             =*/
/*= Modele 2 : avec force exterieure ("gravite")              =*/
/*=============================================================*/

#include <g3x.h>

#include <PMat.h>
#include <Link.h>

/*! Parametres de la fenetre graphique !*/
static int      pixwidth=640,pixheight=640;
static double   xmin=-6.,ymin=-6.,xmax=+6.,ymax=+6.;

/*! VARIABLES DE SIMULATION !*/
static int      Fe;        /* frequence d'echantillonnage        */
static double   h;         /* pas d'echantillonnage 1./Fech      */
static int      Fa=1;      /* on affiche 1 echantillon sur Faff  */
static double   m,k,z,g;   /* parametres physiques               */
static double   tempo=0.02;/* temporisation de la simul          */ 

/*! systeme "Masses-Ressorts" : les particules et les liaisons !*/
static int      nbm;
static PMat    *TabM=NULL;
static int      nbl;
static Link    *TabL=NULL;

/*=================================================================*/
/*= sic.                                                          =*/
/*=================================================================*/
void reset(void)
{	
  int i;
  for (i=0;i<nbm;i++) TabM[i].z=TabM[i].vit=TabM[i].frc=0.;
}


/*=-------------------------=*/
/*=  Constructon du modele  =*/
/*=-------------------------=*/
bool Modeleur(void)
{
  /*! le Modele : un tableau de particules, un tableau de liaisons !*/
  nbm = 11;              /* 9 particules et 2 points fixes     */
  if (!(TabM=(PMat*)calloc(nbm,sizeof(PMat)))) return false;
  nbl = (nbm-1)+(nbm-2); /* 10 ressorts-freins + 9 "gravites"  */
  if (!(TabL=(Link*)calloc(nbl,sizeof(Link)))) return false;

  Fe= 100;           /* parametre du simulateur Fe=1/h                  */ 
  h = 1./Fe;
  m = 1.;            /* la plupart du temps, toutes les masses sont a 1 */
  /* avec les parametres ci-dessous : limite de stabilite               */
  k = 0.866*SQR(Fe); /* raideurs   -> a multiplier par mb*Fe^2          */
  z = 0.08*Fe;       /* viscosires -> a multiplier par mb*Fe            */
  g = -5.*Fe;        /* la gravite : elle aussi a calibrer avec Fe      */
  

  /*! les particules !*/
  int i;
  PMat* M=TabM;
  Fixe(M++,0.,-5.);
  for (i=1;i<nbm-1;i++) MassLF(M++,0.,-5.+i,m); 
  Fixe(M++,0.,+5.);

  /*! les liaisons !*/  
  Link* L=TabL;
  for (i=0;i<nbm-1;i++) RessortFrein(L++,k,z);
  for (i=0;i<nbm-2;i++) FrcConst    (L++,g  ); 
  
  /*! les connections => topologie fixe !*/
  L=TabL;
  /* les ressorts inter-particules */
  M=TabM;
  while (L<TabL+nbm-1) 
  {
    Connect(M,L,M+1);
    M++;
    L++;
  }
  /* les "gravites" individuelles  */
  M=TabM+1; /* 1e particule mobile */
  while (L<TabL+nbl) 
  {
    Connect(M,L,NULL);
    M++;
    L++;
  }

  reset();
  return true;
}


/*=------------------------------=*/
/*=        Initialisation        =*/
/*=------------------------------=*/
void init(void)
{
  /* construction du modele   */
	if (!Modeleur()) exit(1);
  
  /* les parametres reglables */
	g3x_CreateScrollv_i("Fa",&Fa,1,20,1,"Fa");
	g3x_CreateScrollv_d("tmp",&tempo,0.,.1,1.,"tempo");
	g3x_CreateScrollh_d("k",&k,k*0.01,k*5.,1,"k");
	g3x_CreateScrollh_d("z",&z,z*0.01,z*5.,1,"z");
	g3x_CreatePopUp("reset",reset,"reset");
}

/*=------------------------------=*/
/*=          L'Affichage         =*/
/*=------------------------------=*/
void dessin(void)
{
	/* frequence d'affichage reglable */
	g3x_SetRefreshFreq(Fa);
  Link *L=TabL;  
  while (L<TabL+nbl)  { L->draw(L); L->k=k; L->v=z; ++L; } /* mise a jour des parametres => scrollbar */
}


/*=------------------------------=*/
/*=        Le Simulateur         =*/
/*=------------------------------=*/
void Moteur_Physique(void)
{  
  PMat *M=TabM;
  while (M<TabM+nbm) { M->setup(M,h); ++M; } 
  Link *L=TabL;
  while (L<TabL+nbl) { L->setup(L)  ; ++L; } 
  g3x_tempo(tempo); /* temporisation, si ca va trop vite */
}


/*=-------------------------=*/
/*= Fonction de liberation  =*/
/*= appelee a la fin.       =*/
/*=-------------------------=*/
void quit(void)
{
  if (TabM!=NULL) free(TabM); 
  if (TabL!=NULL) free(TabL); 
}

/*=-------------------------=*/
/*=                         =*/
/*=-------------------------=*/
int main(int argc, char* argv[])
{
  g3x_InitWindow(*argv,pixwidth,pixheight);
	/* paramètres caméra */
	/* param. géométrique de la caméra. cf. gluLookAt(...) */
	g3x_SetPerspective(40.f, 100.f, 1.f);
	/* zPosition, orientation de la caméra */
	g3x_SetCameraSpheric(0.25 * PI, +0.25 * PI, 6., (G3Xpoint){0., 0., 0.});

	/* fixe les param. colorimétriques du spot lumineux */
	/* lumiere blanche (c'est les valeurs par defaut)   */
	g3x_SetLightAmbient(1.f, 1.f, 1.f);
	g3x_SetLightDiffuse(1.f, 1.f, 1.f);
	g3x_SetLightSpecular(1.f, 1.f, 1.f);

	/* fixe la zPosition et la direction du spot lumineux */
	/* (c'est les valeurs par defaut)                    */
	g3x_SetLightPosition(10.f, 10.f, 10.f);
	/*g3x_SetLightDirection(0.f, 0.f, 0.f);*/

  g3x_SetInitFunction(init);
  g3x_SetAnimFunction(Moteur_Physique);
  g3x_SetDrawFunction(dessin);
  g3x_SetExitFunction(quit);

  return g3x_MainStart();
}
