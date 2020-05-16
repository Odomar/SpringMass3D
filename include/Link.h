/*=============================================================*/
/*= E.Incerti (incerti@univ-eiffel.fr)                        =*/
/*= M2/IMAC3 - ANIMATION / MOTEUR PHYSIQUE                    =*/
/*= Systeme Masses/Ressort en dimension 1 : module liaison    =*/
/*=============================================================*/

#ifndef  _LINK_
  #define _LINK_

  #include <PMat.h>
  
  /* identificateurs de type                                                 */
  
  /* Module produisant une force constante -- 'equivalent' du point fixe     */
  #define _FRC_CONST     0
  /* Ressort lineaire (de Hook) - force proportionnelle a l'elongation       */
  #define _RESSORT       1
  /* Frein lineaire - force proportionnelle a la vitesse relative des points */
  #define _FREIN         2
  /* Ressort+Frein integres                                                  */
  #define _RESSORT_FREIN 3
  /* Liaison condtionnelle de rupture -- condition sur l'elong. ou la force  */
  #define _CONDIT_RF     4  
  /* Butee conditionelle : ressort frein inactif au-dela d'un seuil de dist. */
  #define _RF_BUTEE      5  

  typedef struct _link
  {
    int       type;      /* type pour usages divers         */     
    /*------------------------------------------------------*/
    double    k;      /* raideur pour les ressorts          */
    double    l;      /* longueur a vide pour les ressorts  */
    double    z;      /* viscosite pour les freins          */
    double    s;      /* seuil pour les liens conditionnels */ 
    double    frc;    /* force a distribuer sur M1 et M2    */
    PMat     *M1,*M2; /* les 2 PMat de branchement          */
    bool      on_off;                /* flag d'activite     */
    void    (*setup)(struct _link*); /* l'algo de calcul    */
    /*------------------------------------------------------*/
    void    (*draw)(struct _link*);  /* fonction de dessin  */
  } 
  Link;

/* les  constructeurs */
/*! Creation d'un module Force Constante (exp. Gravite)                                  !*/
  void FrcConst(Link* L, double frc);

/*! Creation d'un ressort lineaire (de Hook)                                             !*/
  void Ressort(Link* L, double k);

/*! Creation d'un frein cinetique lineaire                                               !*/
  void Frein(Link* L, double z);

/*! Creation d'un ressort+frein : les 2 precedents combines                              !*/
  void RessortFrein(Link* L, double k, double z);

/*! Creation d'une butee visco-elastique seuillee, non lineaire                          !*/
  void RF_Butee(Link *L, double k, double z, double s);
  
/*! Creation d'une liaison de rupture avec condition sur l'elongation                    !*/
  void RF_CondPos(Link *L, double k, double z, double s);

/*! Connexion d'une Liaison entre 2 points Mat                                           !*/
  void Connect(PMat *M1, Link *L, PMat *M2);

#endif
