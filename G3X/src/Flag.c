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
static double   m,k,z,g,v; /* parametres physiques               */
static double   tempo=0.02;/* temporisation de la simul          */

/*! systeme "Masses-Ressorts" : les particules et les liaisons !*/
static int nbm;
static PMat ** TabM=NULL;
static int nbl;
static Link * TabL=NULL;
static int width, height;

/*=================================================================*/
/*= sic.                                                          =*/
/*=================================================================*/
void reset(void)
{
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++){
			TabM[i][j].x = i;
			TabM[i][j].y = 0.;
			TabM[i][j].z = j;
			TabM[i][j].vit_x = 0.;
			TabM[i][j].vit_y = 0.;
			TabM[i][j].vit_z = 0.;
			TabM[i][j].frc_x = 0.;
			TabM[i][j].frc_y = 0.;
			TabM[i][j].frc_z = 0.;
		}
	}
}


/*=-------------------------=*/
/*=  Constructon du modele  =*/
/*=-------------------------=*/
bool createMasses(){
	nbm = width * height;
	if ((TabM=(PMat**)calloc(width, sizeof(PMat*))) == NULL)
		return false;

	for (int i = 0; i < width; i++) {
		if ((TabM[i]=(PMat*)calloc(height, sizeof(PMat))) == NULL)
			return false;
	}

	// Add fixed points to first column
	for (int i = 0; i < height; i++) {
		Fixe(&TabM[0][i], 0., 0., i);
	}

	for (int i = 1; i < width; i++) {
		for(int j = 0; j < height; j++) {
			MassLF(&TabM[i][j], i, 0., j, m);
		}
	}

	return true;
}

bool createLinks(){
	// number of links :
	// (w-1)*h horizontal direct neighbors
	// w*(h-1) vertical direct neighbors
	// (w-2)*h horizontal bridges
	// w*(h-2) vertical bridges
	// (w-1)*(h-1) first diagonal
	// (w-1)*(h-1) second diagonal
	// w*h gravity
	// w*h wind
	// quick maths : 8 * (w*h) - 5w - 5h + 2
	nbl = 8 * (width * height) - 5 * width - 5 * height + 2;

	if ((TabL = (Link*)calloc(nbl, sizeof(Link))) == NULL)
		return false;

	// index to loop over all links
	int idx = 0;

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			// direct horizontal neighbors
			if (i < width - 1) {
				RessortFrein(&TabL[idx], k, z);
				Connect(&TabM[i][j], &TabL[idx], &TabM[i + 1][j]);
				idx++;
			}
			// direct vertical neighbors
			if (j < height - 1) {
				RessortFrein(&TabL[idx], k, z);
				Connect(&TabM[i][j], &TabL[idx], &TabM[i][j + 1]);
				idx++;
			}

			// horizontal bridges
			if (i < width - 2) {
				RessortFrein(&TabL[idx], k, z);
				Connect(&TabM[i][j], &TabL[idx], &TabM[i + 2][j]);
				idx++;
			}
			// vertical bridges
			if (j < height - 2) {
				RessortFrein(&TabL[idx], k, z);
				Connect(&TabM[i][j], &TabL[idx], &TabM[i][j + 2]);
				idx++;
			}

			// first diagonal
			if (i < width - 1 && j < height - 1) {
				RessortFrein(&TabL[idx], k, z);
				Connect(&TabM[i][j], &TabL[idx], &TabM[i + 1][j + 1]);
				idx++;
			}
			// second diagonal
			if (i < width - 1 && j > 0) {
				RessortFrein(&TabL[idx], k, z);
				Connect(&TabM[i][j], &TabL[idx], &TabM[i + 1][j - 1]);
				idx++;
			}

			// Gravity
			FrcConst(&TabL[idx], 0, 0, g);
			Connect(&TabM[i][j], &TabL[idx], NULL);
			idx++;

			// Wind
			FrcConst(&TabL[idx], 0, v, 0);
			Connect(&TabM[i][j], &TabL[idx], NULL);
			idx++;
		}
	}

	return true;
}

bool Modeleur(void)
{
	Fe= 100;           /* parametre du simulateur Fe=1/h                  */
	h = 1./Fe;
	m = 1.;            /* la plupart du temps, toutes les masses sont a 1 */
	/* avec les parametres ci-dessous : limite de stabilite               */
	k = 0.0866*SQR(Fe); /* raideurs   -> a multiplier par mb*Fe^2          */
	z = 0.08*Fe;       /* viscosires -> a multiplier par mb*Fe            */
	g = -5.*Fe;        /* la gravite : elle aussi a calibrer avec Fe      */
	v = -2.*Fe;        /* le vent : lui aussi a calibrer avec Fe          */

	width = 32;
	height = 32;

	if(!createMasses())
		return false;

	if(!createLinks())
		return false;

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
	while (L<TabL+nbl)  {
		/* mise a jour des parametres => scrollbar */
//		fprintf(stderr, "success test\n");
		L->draw(L);
		L->k=k;
		L->v=z;
		++L;
	}
}


/*=------------------------------=*/
/*=        Le Simulateur         =*/
/*=------------------------------=*/
void Moteur_Physique(void)
{
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++){
			TabM[i][j].setup(&TabM[i][j], h);
		}
	}
	Link *L=TabL;
	while (L<TabL+nbl) {
		L->setup(L);
		++L;
	}
	g3x_tempo(tempo); /* temporisation, si ca va trop vite */
}


/*=-------------------------=*/
/*= Fonction de liberation  =*/
/*= appelee a la fin.       =*/
/*=-------------------------=*/
void quit(void)
{
	if (TabM!=NULL){
		for (int i = 0; i < width; i++){
			free(TabM[i]);
		}
		free(TabM);
	}
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
