/*=============================================================*/
/*= E.Incerti (incerti@univ-eiffel.fr)                        =*/
/*= M2/IMAC3 - ANIMATION / MOTEUR PHYSIQUE                    =*/
/*= Systeme Masses/Ressort en dimension 1 : module liaison    =*/
/*=============================================================*/

#include <g3x.h>

#include <PMat.h>
#include <Link.h>

static double alpha= 0.5;
static double ambi = 0.2;
static double diff = 0.3;
static double spec = 0.4;
static double shin = 0.5;

/*================================================*/
/*=      ALGORITHMES DES MODULES DE LIAISON      =*/
/*================================================*/

/*! Algorithme Force Constante             !*/
/*  1 seul point mat. M1 ou M2, au choix... */
static void AlgoFrcConst(Link* L)
{
  PMat* M=(L->M1?L->M1:L->M2);
  M->frc_x += L->frc_x;
  M->frc_y += L->frc_y;
  M->frc_z += L->frc_z;
}

/*! Algo. ressort+frein         !*/
/*combine les 2 algos precedents */
static void AlgoRessortFrein(Link* L)
{
	double d = L->M2->x-L->M1->x;
	double f = L->k*(d-L->l)+L->v*(L->M2->vit_x-L->M1->vit_x);
	L->M1->frc_x += f;
	L->M2->frc_x -= f;

	d = L->M2->y-L->M1->y;
	f = L->k*(d-L->l)+L->v*(L->M2->vit_y-L->M1->vit_y);
	L->M1->frc_y += f;
	L->M2->frc_y -= f;

	d = L->M2->z-L->M1->z;
	f = L->k*(d-L->l)+L->v*(L->M2->vit_z-L->M1->vit_z);
	L->M1->frc_z += f;
	L->M2->frc_z -= f;
}

/* quelques couleurs (cf. <g3x_Colors.h>) */
static G3Xcolor Lcols[8]={G3Xr,G3Xo,G3Xy,G3Xg,G3Xb,G3Xc,G3Xm,G3Xk};

static void DrawM1M2(Link* L)
{
	g3x_Material(Lcols[L->type],ambi,diff,spec,shin,1.);
	glBegin(GL_LINES);
	glVertex3f(L->M1->x,L->M1->y,L->M1->z);
	glVertex3f(L->M2->x,L->M2->y,L->M2->z);
	glEnd();
}


static void DrawFrc(Link* L)
{
	PMat* M=(L->M1!=NULL?L->M1:L->M2);
	g3x_Material(Lcols[L->type],ambi,diff,spec,shin,1.);
	glBegin(GL_LINES);
	glVertex3f(M->x,M->y,M->z);
	glVertex3f(M->x+0.001*L->frc_x,M->y+0.001*L->frc_y,M->z+0.001*L->frc_z);
	glEnd();
}

/*================================================*/
/*= FONCTIONS DE CREATION DES MODULES DE LIAISON =*/
/*================================================*/

/*! Creation d'un module Force Constante (exp. Gravite) !*/
extern void FrcConst(Link* L, double fc_x, double fc_y, double fc_z)
{
  L->type   = _FRC_CONST;
  /* parametres pour le moteur physique */
  L->frc_x  = fc_x;
  L->frc_y  = fc_y;
  L->frc_z  = fc_z;
  L->k      = 0.;
  L->v      = 0.;
  L->s      = 0.;
  L->l      = 0.;
  L->on_off = true;
  L->setup  = &AlgoFrcConst;
  /* parametres graphiques              */
  L->draw   = &DrawFrc;
}

/*! Creation d'un ressort+frein !*/
extern void RessortFrein(Link* L, double k, double z)
{
  L->type   = _RESSORT_FREIN;
  /* parametres pour le moteur physique */
  L->k      = k ;
  L->v      = z ;
  L->s      = 0.;
  L->l      = 0.;
  L->on_off = true;
  L->setup  = &AlgoRessortFrein;
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
  /* length is enclidean distance */
  L->l = sqrt((M2->x-M1->x) * (M2->x-M1->x) + (M2->y-M1->y) * (M2->y-M1->y) + (M2->z-M1->z) * (M2->z-M1->z));
}
