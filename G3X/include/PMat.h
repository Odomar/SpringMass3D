/*=============================================================*/
/*= E.Incerti (incerti@univ-eiffel.fr)                        =*/
/*= M2/IMAC3 - ANIMATION / MOTEUR PHYSIQUE                    =*/
/*= Systeme Masses/Ressort en dimension 1 : module materiel   =*/
/*=============================================================*/

#ifndef  _PMAT_
  #define _PMAT_
  
  #include <g3x.h>

  #define _POINTFIXE 0
  #define _PARTICULE 1

  typedef struct _pmat
  {
    int    type;     /* type pour usages divers            */     
    /*-----------------------------------------------------*/           
    double m;        /* parametre de masse                 */
	double x;        /* position courante                  */
	double y;        /* position courante                  */
	double z;        /* position courante                  */
    double vit_x;    /* vitesse  courante                  */
    double vit_y;    /* vitesse  courante                  */
    double vit_z;    /* vitesse  courante                  */
    double frc_x;    /* buffer de force                    */
    double frc_y;    /* buffer de force                    */
    double frc_z;    /* buffer de force                    */
    void (*setup)(struct _pmat*, double h); /* integrateur */
    /*-----------------------------------------------------*/
    G3Xcolor col;   /* couleur RGBA (4 float)              */
    void (*draw)(struct _pmat*);     /* fonction de dessin */
  } PMat;
  
/*! Creation d'une particule mobile !*/
  /* avec l'integrateur LeapFrog */
  void MassLF(PMat* M, double x, double y, double z, double m);
  /* variante, avec l'integrateur Euler Explicite */
  void MassEE(PMat* M, double x, double y, double z, double m);

/*! Creation d'une masse fixe !*/
  void Fixe(PMat *M, double x, double y, double z);

#endif
