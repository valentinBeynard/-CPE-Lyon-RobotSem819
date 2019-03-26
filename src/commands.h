#ifndef COMMAND_H

#define COMMAND_H


#define byte unsigned char
#define signed_byte char

/*
typedef unsigned char byte;
typedef char signed_byte;
*/

#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))

// Taille max du buffer pour un seul mot de commande/argument
#define ARGS_BUFFER_SIZE  20

// Nombre max de mots composants une commande que l'on peut récupérer avec le buffer lors du parse
#define MAX_COMMAND_WORD  10


///*****************************************************************************************************************************************
// Structure de donn�es pour les commandes transmises par la centrale ce commande
// Les commandes d�cod�es par FO-M1 seront stock�es dans une structure de ce type pour �tre utilis�es par FO-M5
//*****************************************************************************************************************************************

enum Epreuve {Epreuve_non, epreuve1, epreuve2, epreuve3, epreuve4, epreuve5, epreuve6, epreuve7, epreuve8, Fin_Epreuve, Stop_Urgence};
enum Mouvement {Mouvement_non, Avancer, Reculer, Stopper, Rot_90D, Rot_90G, Rot_180D, Rot_180G, Rot_AngD, Rot_AngG, Depl_Coord};
enum ACQ_Son {ACQ_non, ACQ_oui};
enum DCT_Obst {DCT_non, oui_180, oui_360};
enum Lumiere {Lumiere_non, Allumer, Eteindre};
enum Servo {Servo_non, Servo_H, Servo_V};
enum Energie{Energie_non,Mesure_I,Mesure_E};
enum Position{Position_non, Init_Position, Demande_Position};
enum Photo{Photo_non,Photo_1,Photo_Multiple,Photo_continue,Photo_stop};

typedef struct COMMANDES       // Cette structure contient toutes les commandes envoy�es par la centrale de commande
                               // Les variables de type enum correspondent aux commandes
                               // Les variables de type byte, UNsigned_byte ou int correspondent aux param�tres

{
   enum  Epreuve Etat_Epreuve;     //Contient le num�ro d'�preuve en cours - Commandes "D" "E" "Q"

   enum  Mouvement Etat_Mouvement; //Contient le type de mouvement demand� - Commandes "A" "B" "S" "RD" "RG" "RC" "RA" "G"
                                   // Param�tres des commandes pr�c�dentes:
   byte  Vitesse;                  // Param�tre vitesse - Peut aussi �tre modifi� par la commande "TV"
   byte  Coord_X;	                 // Param�tre coordonn�e X
   byte  Coord_Y;                  // Param�tre coordonn�e Y
   int   Angle;                    // Param�tre angle

   enum  ACQ_Son Etat_ACQ_Son;     // Contient la commande d'acquisition de signaux sonores "ASS"
                                   // Param�tre de la commande pr�c�dente:
   byte  ACQ_Duree;                // Param�tre dur�e de la s�quence d'acquisition

   enum  DCT_Obst Etat_DCT_Obst;   // Contient la commande de d�tection d'obstacle "MOB D" "MOB"
                                   // Param�tre de la commandes pr�c�dente:
   byte  DCT_Obst_Resolution;      // Param�tre r�solution angulaire de la d�tection d'obstacle

   enum  Lumiere Etat_Lumiere;     // Contient la commande d'allumage du pointeur lumineux "L" "LS"
                                   // Param�tres de la commande pr�c�dente:
   byte  Lumiere_Intensite;        // Param�tre intensit� lumineuse
   byte  Lumiere_Duree;            // Param�tre dur�e d'allumage
   byte  Lumire_Extinction;        // Param�tre dur�e d'extinction
   byte  Lumiere_Nbre;             // Param�tre nombre de cycles d'allumage/extinction

   enum  Servo Etat_Servo;        // Contient la commande de positionnement d'un servo "CS C" "CS H" "CS V"
                                  // Param�tre de la commande pr�c�dente:
   signed_byte  Servo_Angle;      // Param�tre angle de rotation
   enum  Energie Etat_Energie;    // Contient la commande d'envoi du courant instantan� ou de l'energie consomm�e - Commandes "MI" "ME"
   enum  Position Etat_Position;  // Contient les commandes de gestion de la position, initialisation ou demande d'envoi
		                              // de la position - Commandes: "IPO" "POS"
   byte  Pos_Coord_X;	            // Param�tre coordonn�e X pour initialisation de la position
   byte  Pos_Coord_Y;             // Param�tre coordonn�e Y pour initialisation de la position
   int   Pos_Angle;               // Param�tre angle pour initialisation de la position
	 enum  Photo Etat_Photo;        // Contient les commandes de la gestion de la prise de photo - Commandes "PPH" "SPH"
	 byte  Photo_Duree;	            // Param�tre dur�e entre 2 photos
   byte  Photo_Nbre;              // Param�tre nombre de photos

}OUT_M1;



//*****************************************************************************************************************************************
// Structure de donn�es pour les informations � emettre vers la centrale ce commande
// Les donn�es "Informations" produites par FO-M5  seront stock�es dans une structure de ce type pour �tre utilis�es par FO-M1 qui devra
// les encoder en messages ASCII
//*****************************************************************************************************************************************

enum Invite {Invite_non, Invite_oui};
enum BUT_Mouvement {BUT_Atteint_non, BUT_Atteint_oui};
enum BUT_Servo {BUT_Servo_non, BUT_Servo_H, BUT_Servo_V, BUT_Servo_C};
enum RESULT_Courant{RESULT_Courant_non,RESULT_Courant_oui};
enum RESULT_Energie{RESULT_Energie_non,RESULT_Energie_oui};
enum RESULT_Position{RESULT_Position_non,RESULT_Position_oui};
enum RESULT_DCT_Obst {DCT_Obst_non, DCT_Obst_180_oui, DCT_Obst_360_oui};
enum Aux {Aux_non, Aux_oui};

typedef struct INFORMATIONS       // Cette structure contient toutes les informations susceptibles d'�tre transmises � la centrale de commande
	                                // Les variables de type enum correspondent aux types des informations
                                  // Les variables de type byte, UNsigned_byte ou int correspondent aux param�tres

{
   enum  Invite Etat_Invite;      // Contient l'identificateur information d'invite "I"
                                  // Param�tre de l'information pr�c�dente:
   char  *MSG_Invit;	            //Pointeur sur une chaine de caract�re d'invite

   enum  BUT_Mouvement Etat_BUT_Mouvement; // Contient l'identificateur information Arriv�e au point sp�cifi� "B"
	                                         // Pas de param�tre associ� pour l'information pr�c�dente:
   enum  BUT_Servo Etat_BUT_Servo;         // Contient l'identificateur information Servomoteur positionn� "AS C" "AS H" "AS H"
	                                         // Pas de param�tre associ� pour l'information pr�c�dente:

   enum  RESULT_DCT_Obst Etat_DCT_Obst;    // Contient l'identificateur information D�tection des obstacles "KOB"
                                           // Param�tre de l'information pr�c�dente:
	 int   *Tab_Val_Obst;                    //Pointeur sur un tableau de valeurs de distance
	 byte	 Nbre_Val_obst;                    //Nombre de points dans le tableau

	 enum RESULT_Courant Etat_RESULT_Courant; // Contient un idendicateur d'information "courant mesur�"
	 unsigned int Mesure_Courant;            // Valeur du courant mesur� en mA

	 enum RESULT_Energie Etat_RESULT_Energie; // Contient un idendicateur d'information "Energie Consomm�e"
	 unsigned int Mesure_Energie;            // Valeur de l'�nergie consomm�e depuis le d�but de l'�preuve

	 enum RESULT_Position Etat_RESULT_Position; // Contient un idendicateur d'information "Position"
   byte  Pos_Coord_X;	                     // Information Param�tre coordonn�e X
   byte  Pos_Coord_Y;                      // Information Param�tre coordonn�e Y
   int   Pos_Angle;                        // Information Param�tre angle

   enum   Aux Etat_Aux;                    // Contient l'identificateur information auxiliaires "IA"
                                           // Param�tre de l'information pr�c�dente:
   char  *MSG_Aux;	                       //Pointeur sur une chaine de caract�re d'informations auxiliaires
} IN_M1;
// *******************************************************************************************

typedef struct
{
	char* commands_data;
	byte cmd_size;
	OUT_M1* commands;
}CMD_PACKET;

typedef struct
{
  const char* name;
  byte(*process)(CMD_PACKET*);
}CMD_;


byte default_process(CMD_PACKET* cmd_packet);


/*
  Commande de démarage de l'épreuve : D [Numéro Epreuve]
  Si aucun paramètre rentré, alors on démarre à l'épreuve 1
*/
byte epreuve_cmd(CMD_PACKET* cmd_packet);
byte epreuve_stop(CMD_PACKET* cmd_packet);

byte safety_break_cmd(CMD_PACKET* cmd_packet);
byte set_default_speed_cmd(CMD_PACKET* cmd_packet);
byte move_forward_cmd(CMD_PACKET* cmd_packet);
byte move_backward_cmd(CMD_PACKET* cmd_packet);
byte move_stop_cmd(CMD_PACKET* cmd_packet);
byte rigth_rotation_cmd(CMD_PACKET* cmd_packet);
byte left_rotation_cmd(CMD_PACKET* cmd_packet);
byte complete_rotation_cmd(CMD_PACKET* cmd_packet);
byte angle_rotation_cmd(CMD_PACKET* cmd_packet);


byte detecte_obstacle(CMD_PACKET* cmd_packet);

#else

//#error Multiple define !

#endif