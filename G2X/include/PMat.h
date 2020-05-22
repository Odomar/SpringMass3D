/*=============================================================*/
/*= E.Incerti (incerti@univ-eiffel.fr)                        =*/
/*= M2/IMAC3 - ANIMATION / MOTEUR PHYSIQUE                    =*/
/*= Systeme Masses/Ressort en dimension 1 : module materiel   =*/
/*=============================================================*/

#ifndef  _PMAT_
  #define _PMAT_
  
  #include <g2x.h>

  #define _POINTFIXE 0
  #define _PARTICULE 1

  typedef struct _pmat
  {
    int    type;     /* type pour usages divers            */     
    /*-----------------------------------------------------*/           
    double m;        /* parametre de masse                 */
    double pos;      /* position courante                  */
    double vit;      /* vitesse  courante                  */
    double frc;      /* buffer de force                    */
    void (*setup)(struct _pmat*, double h); /* integrateur */
    /*-----------------------------------------------------*/
    G2Xcolor col;   /* couleur RGBA (4 float)              */   
    double   x;     /* 2e coord. pour positionnement 2D    */
    void (*draw)(struct _pmat*);     /* fonction de dessin */    
  } PMat;
  
/*! Creation d'une particule mobile !*/
  /* avec l'integrateur LeapFrog */
  void MassLF(PMat* M, double pos, double x, double m);
  /* variante, avec l'integrateur Euler Explicite */
  void MassEE(PMat* M, double pos, double x, double m);

/*! Creation d'une masse fixe !*/
  void Fixe(PMat *M, double pos, double x);

#endif
