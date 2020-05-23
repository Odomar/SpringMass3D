/*=============================================================*/
/*= E.Incerti (incerti@univ-eiffel.fr)                        =*/
/*= M2/IMAC3 - ANIMATION / MOTEUR PHYSIQUE                    =*/
/*= Systeme Masses/Ressort en dimension 1 : module materiel   =*/
/*=============================================================*/

#include <PMat.h>


/*! Algorithme de la masse libre 2D (2e loi de Newton)    !*/
/*! integration : leapfrog                                !*/
static void leapfrog(PMat* M, double h)
{	
	M->vit_x += h*M->frc_x/M->m;   /* 1e integration : vitesse   V(n+1)=V(n)+h*F(n)/m - EXplicite */
	M->x += h*M->vit_x;        /* 2e integration : position  X(n+1)=X(n)+h*V(n+1) - IMplicite */
	M->frc_x = 0.;              /* on vide le buffer de force */

	M->vit_y += h*M->frc_y/M->m;   /* 1e integration : vitesse   V(n+1)=V(n)+h*F(n)/m - EXplicite */
	M->y += h*M->vit_y;        /* 2e integration : position  X(n+1)=X(n)+h*V(n+1) - IMplicite */
	M->frc_y = 0.;              /* on vide le buffer de force */

	M->vit_z += h*M->frc_z/M->m;   /* 1e integration : vitesse   V(n+1)=V(n)+h*F(n)/m - EXplicite */
	M->z += h*M->vit_z;        /* 2e integration : position  X(n+1)=X(n)+h*V(n+1) - IMplicite */
	M->frc_z = 0.;              /* on vide le buffer de force */
}
/*! integration : Euler Explicite                         !*/
static void eulerexp(PMat* M, double h)
{	
	M->x += h*M->vit_x;        /* 1e integration : position  X(n+1)=X(n)+h*V(n)   - EXplicite */
	M->vit_x += h*M->frc_x/M->m;   /* 2e integration : vitesse   V(n+1)=V(n)+h*F(n)/m - EXplicite */
	M->frc_x = 0.;              /* on vide le buffer de force */

	M->y += h*M->vit_y;        /* 1e integration : position  X(n+1)=X(n)+h*V(n)   - EXplicite */
	M->vit_y += h*M->frc_y/M->m;   /* 2e integration : vitesse   V(n+1)=V(n)+h*F(n)/m - EXplicite */
	M->frc_y = 0.;              /* on vide le buffer de force */

	M->z += h*M->vit_z;        /* 1e integration : position  X(n+1)=X(n)+h*V(n)   - EXplicite */
	M->vit_z += h*M->frc_z/M->m;   /* 2e integration : vitesse   V(n+1)=V(n)+h*F(n)/m - EXplicite */
	M->frc_z = 0.;              /* on vide le buffer de force */
}

/*! Algorithme du point fixe (position constante)         !*/
static void pointfixe(PMat* M, double h)
{
	M->frc_x = 0.;
	M->vit_x = 0.;

	M->frc_y = 0.;
	M->vit_y = 0.;

	M->frc_z = 0.;
	M->vit_z = 0.;
}

static void drawcirc(PMat *M)
{ /* rayon indexe sur la masse */
	/*g3x_Material(rouge,ambi,diff,spec,shin,1.);
	glutSolidSphere(2,M->x,M->pos);
	g2x_Circle(M->x,M->pos,0.2*M->m,M->col,2);*/

}

static void drawdot(PMat *M)
{ /* rayon indexe sur la masse */
  /*g3x_FillSphere(M->x,M->pos,0.2*M->m,M->col);*/
}


/*! Creation d'une masse libre !*/
/*  Creation d'une particule libre : attribution des parametres de position et masse (vitesse nulle)  */
/*  avec l'integrateur LeapFrog */
extern void MassLF(PMat* M, double x, double y, double z, double m)
{
	M->type  = _PARTICULE;
	/* parametres pour le moteur physique */
	M->x   = x;
	M->y   = y;
	M->z   = z;
	M->vit_x   = 0.;
	M->vit_y   = 0.;
	M->vit_z   = 0.;
	M->frc_x   = 0.;
	M->frc_y   = 0.;
	M->frc_z   = 0.;
	M->m     = m;
	M->setup = &leapfrog;
	/* parametres graphiques */
	memcpy(M->col,G3Xb ,sizeof(G3Xcolor));
	M->draw  = &drawdot;
}

/* variante, avec l'integrateur Euler Explicite */
extern void MassEE(PMat* M, double x, double y, double z, double m)
{
	M->type  = _PARTICULE;
	/* parametres pour le moteur physique */
	M->x   = x;
	M->y   = y;
	M->z   = z;
	M->vit_x   = 0.;
	M->vit_y   = 0.;
	M->vit_z   = 0.;
	M->frc_x   = 0.;
	M->frc_y   = 0.;
	M->frc_z   = 0.;
	M->m     = m;
	M->setup = &eulerexp;
	/* parametres graphiques */
	memcpy(M->col,G3Xg ,sizeof(G3Xcolor));
	M->draw  = &drawcirc;
}

/*! Creation d'une masse fixe !*/
extern void Fixe(PMat *M, double x, double y, double z)
{
	M->type  = _POINTFIXE;
	M->x   = x;
	M->y   = y;
	M->z   = z;
	M->vit_x   = 0.;
	M->vit_y   = 0.;
	M->vit_z   = 0.;
	M->frc_x   = 0.;
	M->frc_y   = 0.;
	M->frc_z   = 0.;
	M->m    = 1.; /* juste pour le dessin */
	M->setup = &pointfixe;
	/* parametres graphiques */
	memcpy(M->col,G3Xr ,sizeof(G3Xcolor));
	M->draw  = &drawcirc;
}

