/*=============================================================*/
/*= E.Incerti (incerti@univ-eiffel.fr)                        =*/
/*= M2/IMAC3 - ANIMATION / MOTEUR PHYSIQUE                    =*/
/*= Systeme Masses/Ressort en dimension 1 : module liaison    =*/
/*=============================================================*/

#include <g2x.h>

#include <PMat.h>
#include <Link.h>

/*================================================*/
/*=      ALGORITHMES DES MODULES DE LIAISON      =*/
/*================================================*/

/*! Algorithme Force Constante             !*/
/*  1 seul point mat. M1 ou M2, au choix... */
static void AlgoFrcConst(Link* L)
{
  PMat* M=(L->M1?L->M1:L->M2);
  M->frc += L->frc;
}

/*! Algo. ressort de Hook (lineaire)                            !*/
/* produit une force : F=k*(d-l0).AB                             */
/* ou (d-l0) est l'allongement et AB le vecteur inter particules */
static void AlgoRessort(Link* L)
{
  double d = L->M2->pos-L->M1->pos;
  double f = L->k*(d-L->l);
  L->M1->frc += f;
  L->M2->frc -= f;
}
  
/*! Algo. frein cinetique                           !*/
/* produit une force : F=z*(V2-V1)                   */
/* ou (V2-V1) est la vitesse relative des particules */
static void AlgoFrein(Link* L)
{
  double f = L->z*(L->M2->vit-L->M1->vit);
  L->M1->frc += f;
  L->M2->frc -= f;
}

/*! Algo. ressort+frein         !*/
/*combine les 2 algos precedents */
static void AlgoRessortFrein(Link* L)
{ 
  double d = L->M2->pos-L->M1->pos;
  double f = L->k*(d-L->l)+L->z*(L->M2->vit-L->M1->vit);
  L->M1->frc += f;
  L->M2->frc -= f;
}  

/*! Algo. butee visco-elastique                    !*/
/*! active uniquement si dist. < seuil             !*/
/* comme precedemment mais avec un seuil d'activite */
static void AlgoRF_Butee(Link* L)
{ 
  double d = L->M2->pos-L->M1->pos;
  if (d>L->s) return;

  double f = L->k*(d-L->l)+L->z*(L->M2->vit-L->M1->vit);
  L->M1->frc += f;
  L->M2->frc -= f;
}  


/*! Algo. lien visco-elastique "cassable"          !*/
/*! si d>seuil, la liaison devient inactive        !*/
static void AlgoRF_CondPos(Link* L)
{ /* si la liaison est deja cassee : rien */
  if (!L->on_off) return;

  double d = L->M2->pos-L->M1->pos;
  /* si l'allongement est trop fort : la liaison casse */
  if (d>L->s) { L->on_off=0; return; }

  double f = L->k*(d-L->l)+L->z*(L->M2->vit-L->M1->vit);
  L->M1->frc += f;
  L->M2->frc -= f;
}

/* quelques couleurs (cf. <g2x_Colors.h>) */
static G2Xcolor Lcols[8]={G2Xr,G2Xo,G2Xy,G2Xg,G2Xb,G2Xc,G2Xm,G2Xk};

static void DrawM1M2(Link* L)
{
  g2x_Line(L->M1->x,L->M1->pos,L->M2->x,L->M2->pos,Lcols[L->type],1);
}


static void DrawFrc(Link* L)
{
  PMat* M=(L->M1!=NULL?L->M1:L->M2);
  g2x_Line(M->x,M->pos,M->x,M->pos+0.001*L->frc,Lcols[L->type],1);
}

/*================================================*/
/*= FONCTIONS DE CREATION DES MODULES DE LIAISON =*/
/*================================================*/

/*! Creation d'un module Force Constante (exp. Gravite) !*/
extern void FrcConst(Link* L, double force_const)
{
  L->type   = _FRC_CONST;
  /* parametres pour le moteur physique */
  L->frc    = force_const;
  L->k      = 0.;
  L->z      = 0.;
  L->s      = 0.;
  L->l      = 0.;
  L->on_off = true;
  L->setup  = &AlgoFrcConst;
  /* parametres graphiques              */
  L->draw   = &DrawFrc;
}

/*! Creation d'un ressort lineaire (de Hook) !*/
extern void Ressort(Link* L, double k)
{
  L->type   = _RESSORT;
  /* parametres pour le moteur physique */
  L->k      = k ;
  L->z      = 0.;
  L->s      = 0.;
  L->l      = 0.;
  L->on_off = true;
  L->setup  = &AlgoRessort;
  /* parametres graphiques              */
  L->draw   = &DrawM1M2;
}
  
/*! Creation d'un frein cinetique lineaire !*/
extern void Frein(Link* L, double z)
{
  L->type   = _FREIN;
  /* parametres pour le moteur physique */
  L->k      = 0.;
  L->z      = z ;
  L->s      = 0.;
  L->on_off = true;
  L->setup  = &AlgoFrein;
  /* parametres graphiques              */
  L->draw   = &DrawM1M2;
}

/*! Creation d'un ressort+frein !*/
extern void RessortFrein(Link* L, double k, double z)
{
  L->type   = _RESSORT_FREIN;
  /* parametres pour le moteur physique */
  L->k      = k ;
  L->z      = z ;
  L->s      = 0.;
  L->l      = 0.;
  L->on_off = true;
  L->setup  = &AlgoRessortFrein;
  /* parametres graphiques              */
  L->draw   = &DrawM1M2;
}

/*! Creation d'une butee visco-elastique seuillee !*/
extern void RF_Butee(Link *L, double k, double z, double s)
{
  L->type   = _RF_BUTEE;
  /* parametres pour le moteur physique */
  L->k      = k ;
  L->z      = z ;
  L->s      = s ; /* seuil de distance pour detachement   */
  L->l      = 0.;
  L->on_off = true;
  L->setup  = &AlgoRF_Butee;
  /* parametres graphiques              */
  L->draw   = &DrawM1M2;
}


/*! Creation d'une liaison de rupture avec condition sur l'elongation !*/
extern void RF_CondPos(Link *L, double k, double z, double s)
{
  L->type   = _CONDIT_RF;
  /* parametres pour le moteur physique */
  L->k      = k ;
  L->z      = z ;
  L->s      = s ; /* seuil d'elongation pour rupture      */
  L->l      = 0.;
  L->on_off = true;
  L->setup  = &AlgoRF_CondPos;
  /* parametres graphiques              */
  L->draw   = &DrawM1M2;
}


/*! Connexion d'une Liaison entre 2 points Mat !*/
/* pas utilise dans systemes de particules       */
/* ca sert surtout pour les topologies fixes    */
extern void Connect(PMat *M1, Link *L, PMat *M2)
{
  L->M1=M1;
  L->M2=M2;
  if (M1==NULL || M2==NULL) return;
  L->l = (M2->pos-M1->pos);
}
