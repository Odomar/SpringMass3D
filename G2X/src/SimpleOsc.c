/*=============================================================*/
/*= E.Incerti (incerti@univ-eiffel.fr)                        =*/
/*= M2/IMAC3 - ANIMATION / MOTEUR PHYSIQUE                    =*/
/*= Systeme Masses/Ressort en dimension 1 : Oscillateur       =*/
/*= Modele 0 : juste une masse, un point fixe et un ressort   =*/
/*= Rupture d'equilibre par impulsion de force ou deplacement =*/  
/*=============================================================*/
#include <g2x.h>

#include <PMat.h>
#include <Link.h>

/*! Parametres de la fenetre graphique !*/
static int    pixwidth=640,pixheight=640;
static double xmin=-6.,ymin=-6.,xmax=+6.,ymax=+6.;

/*! VARIABLES DE SIMULATION !*/
static double   h;          /* pas d'echantillonnage 1./Fech      */
static int      Fe;         /* frequence d'echantillonnage        */
static int      Fa=1;       /* on affiche 1 echantillon sur Faff  */
static double   m,k,z;      /* parametres physiques               */ 
static char     PouF = 'f'; /* type de perturbation initiale      */
static double   tempo=0.02; /* temporisation de la simul          */ 

/*! systeme de particules  !*/
static PMat     M,S;
static Link     L;

/*=================================================================*/
/*= sic.                                                          =*/
/*=================================================================*/
void reset(void)
{	
  M.pos=M.vit=M.frc=0.;
  switch (PouF)
  {
    case 'P' : case 'p' : /* contrainte initiale : particule deplacee */
      M.pos=.5;
      break;
    case 'F' : case 'f' : /* Autre forme de contrainte initiale : impulsion de force */
      M.frc=1.*SQR(Fe);   /* notez le calibrage par Fe^2 */
      break;
  }
}

/*=-------------------------=*/
/*=  Constructon du modele  =*/
/*=-------------------------=*/
void Modeleur(void)
{
  Fe= 100;           /* parametre du simulateur Fe=1/h                  */ 
  h = 1./Fe;
  m = 1.;            /* la plupart du temps, toutes les masses sont a 1 */
  k = 0.10*SQR(Fe);  /* raideurs   -> a multiplier par mb*Fe^2                   */
  z = 0.005*Fe;      /* viscosires -> a multiplier par mb*Fe                     */

  /*! les objets : une masse mobile (M) reliee a un pt. fixe (S) par un ressort-frein (L) !*/
  Fixe(&S,0.,-2.);
  MassLF(&M,0.,+2.,m); 
/* la meme chose, mais avec l'integrateur Euler Explicite */
/*  MassEE(&M,0.,+2.,m); */
  
  RessortFrein(&L,k,z);
  /* connection du ressort entre les 2 masses */
  Connect(&M,&L,&S);
  
  reset();
}

/*=------------------------------=*/
/*=        Initialisation        =*/
/*=------------------------------=*/
void init(void)
{
  /* construction du modele   */
	Modeleur();
  
  /* les parametres reglables */
  g2x_CreateScrollv_i("Fa",&Fa,1,20,1,"Fa");
  g2x_CreateScrollv_d("tmp",&tempo,0.,.1,1.,"tempo");
	g2x_CreateScrollh_d("k",&k,k*0.01,k*5.,1,"k");
	g2x_CreateScrollh_d("z",&z,z*0.01,z*5.,1,"z");
	g2x_CreatePopUp("reset",reset,"reset");
}


/*=------------------------------=*/
/*=          L'Affichage         =*/
/*=------------------------------=*/
void dessin(void)
{ /* frequence d'affichage reglable */
	g2x_SetRefreshFreq(Fa);
  S.draw(&S);
  M.draw(&M);
  L.draw(&L);
  L.k=k; L.z=z; /* mise a jour des parametres => scrollbar */
}


/*=------------------------------=*/
/*=        Le Simulateur         =*/
/*=------------------------------=*/
void Moteur_Physique(void)
{  
  /* d'abord les PMat */
  M.setup(&M,h);
  S.setup(&S,h);
  /* ensuite les Link */
  L.setup(&L); 

  g2x_tempo(tempo); /* temporisation, si ca va trop vite */
}


/*=-------------------------=*/
/*= Fonction de liberation  =*/
/*= appelee a la fin.       =*/
/*=-------------------------=*/
void quit(void)
{
  /* rien ici */
}

/*=-------------------------=*/
/*=                         =*/
/*=-------------------------=*/
int main(int argc, char* argv[])
{
  g2x_InitWindow(*argv,pixwidth,pixheight);  
  g2x_SetWindowCoord(xmin,ymin,xmax,ymax);

  g2x_SetInitFunction(init);
  g2x_SetAnimFunction(Moteur_Physique);
  g2x_SetDrawFunction(dessin);          
  g2x_SetExitFunction(quit);      

  return g2x_MainStart();
}
