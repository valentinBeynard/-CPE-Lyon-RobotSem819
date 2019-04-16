#ifndef SLAVE_CMD_H

#define SLAVE_CMD_H


#define byte unsigned char
#define signed_byte char
	

///*****************************************************************************************************************************************
// Structure de donn?es pour les commandes transmises par la centrale ce commande
// Les commandes d?cod?es par FO-M1 seront stock?es dans une structure de ce type pour ?tre utilis?es par FO-M5
//*****************************************************************************************************************************************
enum ACQ_Son {ACQ_non, ACQ_oui};
enum GEN_Son {GEN_non, GEN_oui};
enum Lumiere {Lumiere_non, Allumer, Eteindre};
enum Servo {Servo_non, Servo_oui};
enum Photo{Photo_non,Photo_1,Photo_Multiple,Photo_continue,Photo_stop};

typedef struct COMMANDES                              
{
   enum  ACQ_Son Etat_ACQ_Son;     // Contient la commande d'acquisition de signaux sonores "ASS"
                                   // Param?tre de la commande pr?c?dente:
   byte  ACQ_Duree;                // Param?tre dur?e de la s?quence d'acquisition
	
   enum	 GEN_Son Etat_GEN_Son;		 // Contient la commande de génération de signaux sonores "ASS"

   byte  GEN_freq_code;
	 byte	 GEN_son_Duree;
	 byte	 GEN_silence_Duree;
	 byte	 GEN_nbr_bip;

   enum  Lumiere Etat_Lumiere;     // Contient la commande d'allumage du pointeur lumineux "L" "LS"
                                   // Param?tres de la commande pr?c?dente:
   byte  Lumiere_Intensite;        // Param?tre intensit? lumineuse
   byte  Lumiere_Duree;            // Param?tre dur?e d'allumage
   byte  Lumire_Extinction;        // Param?tre dur?e d'extinction
   byte  Lumiere_Nbre;             // Param?tre nombre de cycles d'allumage/extinction

   enum  Servo Etat_Servo;        // Contient la commande de positionnement d'un servo "CS C" "CS H" "CS V"
                                  // Param?tre de la commande pr?c?dente:
   signed_byte  Servo_Angle;      // Param?tre angle de rotation

	 enum  Photo Etat_Photo;        // Contient les commandes de la gestion de la prise de photo - Commandes "PPH" "SPH"
	 byte  Photo_Duree;	            // Param?tre dur?e entre 2 photos
   byte  Photo_Nbre;              // Param?tre nombre de photos

}OUT_M2;

#endif