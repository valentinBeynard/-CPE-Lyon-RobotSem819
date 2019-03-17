#ifndef SERIALIZER_H

#define SERIALIZER_H

#include "c8051F020.h"
#include "commands.h"

#define byte unsigned char
#define SIGNED_BYTE char

#define MOGO_CMD_SIZE	20

#define DIGO_CMD_SIZE	40


#define ENC_RSLT_SIZE	10

#define PID_RSLT_SIZE	10

// Byte retourné par le sérializer à la fin de son retour
#define END_RSLT_BYTE	0x3E

/* Diametre des roues */
#define WHEEL_DIAMETER	60

/* Nombre de Ticks d'un encoder pour 1 tour de roue */
#define TICK_PER_CYCLE	624

#define M_PI 3.14159265358979323846

#define ENC_2_MM(ticks)	( ticks * (M_PI * WHEEL_DIAMETER) / TICK_PER_CYCLE)

#define ANGLE_2_DIST(angle)	(TICK_PER_CYCLE * angle)/(2.0 * M_PI)

#define TURN_SPEED	20

/*
#define ABS(X)	(X >= 0 ? X : (-1) * X)
*/


typedef enum
{
	IDLE = 0,
	TRANSLATE,
	ROTATE,
	NAVIGATE,
	STOP
}SERIALIZER_STATE;

typedef enum
{
	LEFT = 0,
	RIGHT
}ENCODER_ID;

typedef struct
{
	char x;
	char y;
	float angle;
	byte speed;
}PTS_2DA;


typedef struct
{
  SERIALIZER_STATE state_name;
  void(*state_process)(PTS_2DA*);
}SERIALIZER_FSM_PROCESS;


/*
typedef unsigned char byte;
typedef char SIGNED_BYTE;
*/
//*****************************************************************************************************************************************
// Structure de donn?es pour les commandes ? envoyer ? la carte Serializer
// Les commandes produites par FO-M5 (? priori) seront stock?es dans une structure de ce type pour ?tre utilis?es par FO-M2
// Cette structure ne couvre pas toutes les commandes compr?hensibles par la carte Serializer. 
// Seules les commande potentiellement utiles au projet SEM815 sont trait?es 
// Commandes Trait?es:  Reset - getenc - clrenc - mogo - vpid - digo - dpid - rpid - pids - stop - vel - restore
//*****************************************************************************************************************************************

enum Commande {Commande_non, Reset, Getenc_1, Getenc_2, Getenc_1_2, Clrenc_1, Clrenc_2, Clrenc_1_2, mogo_1, mogo_2, mogo_1_2, Vpid_set, Vpid_read,
               digo_1, digo_2, digo_1_2, Dpid_set, Dpid_read, Rpid_Stinger, Pids, Stop, Vel, Restore };


typedef struct COMMANDES_SERIALIZER  // Cette structure contient toutes les commandes ? envoyer au serializer
	                                   // Les variables de type enum correspondent aux commandes
                                    // Les variables de type BYTE, UNSIGNED_BYTE ou int corrspondent aux param?tres
                
{           
   enum  Commande Etat_Commande;   // Contient la commande ? transmettre "Reset" "getenc 1" "getenc 2" "getenc 1 2" "clrenc 1" "clrenc 2"
                                   // "clrenc 1 2" "mogo 1" "mogo 1" "mogo 2" "mogo 1 2" "vpid" "digo 1" "digo 2" "digo 1 2" "dpid"
									                 // "rpid s"  "pids" "stop" "vel" "restore"
               
   int   Set_P;                    // param?tre commande vpid dpid
   int   Set_I;                    // param?tre commande vpid dpid
   int   Set_D;                    // param?tre commande vpid dpid
   int   Set_L_A;                  // param?tre commande vpid (L)  dpid (A)
   int   Ticks_mot1;               // param?tre Distance - Encoder Ticks moteur 1 (digo)
   SIGNED_BYTE  Vitesse_Mot1;      // param?tre de vitesse de rotation du moteur 1  (mogo digo)
   int   Ticks_mot2;               // param?tre Distance - Encoder Ticks moteur 2 (digo)
   SIGNED_BYTE  Vitesse_Mot2;      // param?tre de vitesse de rotation du moteur 2 (mogo digo)
} IN_M2;


//*****************************************************************************************************************************************
// Structure de donn?es pour les informations transmises par la carte Serializer
// Les informations Serializer d?cod?es par FO-M2 seront stock?es dans une structure de ce type pour ?tre utilis?es par FO-M5
// Cette structure ne couvre pas toutes les informations transmissibles par la carte Serializer. 
// Seules les informations potentiellement utiles au projet SEM815 sont trait?es 
//*****************************************************************************************************************************************

enum Reponse {Reponse_non, Rep_getenc, Rep_vpid, Rep_dpid, Rep_pids, Rep_vel};


typedef struct INFORMATIONS_SERIALIZER // Cette structure contient toutes les informations envoy?es par la carte Serialize
	                                     // Les variables de type enum correspondent aux commandes
                                       // Les variables de type BYTE, UNSIGNED_BYTE ou int corrspondent aux param?tres
                
{  
   enum  Reponse Etat_Response;   	   // Contient le type de r?ponse re?ue ? une des commandes suivantes:  "vpid" "dpid" "pids" "vel" 
   int   Read_Val_enc_1;               // information commande getenc1
   int   Read_Val_enc_2;               // information commande getenc2
   
   int   Read_P;                       //   information commande vpid dpid
   int   Read_I;                       //   information commande vpid dpid
   int   Read_D;                       //   information commande vpid dpid
   int   Read_L_A;                     //   information commande vpid (L)  dpid (A)
   byte  Read_Pids;                    //   information commande pids
   int   Read_Vitesse_mot1;            //   information commande "velocity" moteur 1
   int   Read_Vitesse_mot2;            //   information commande "velocity" moteur 2
   
} OUT_M2;

byte init_serializer_UART1();
void serializer_receive(byte* read_byte);
void serializer_send(byte ch);
void serializer_print(char* str);
void serializer_clear_serial();
void serializer_init_serial();

void serializer_process(OUT_M1* cmd);
void idle_next_state(OUT_M1* cmd, PTS_2DA* pts);

void setMotors(int mtr_speed_1, int mtr_speed_2);
void moveAngle(float angle);

int getRawEncoders(ENCODER_ID encoder_id);
int getEncoderDistance(ENCODER_ID encoder_id);

char is_PID_processing();

void idle(PTS_2DA* pts);
void translate(PTS_2DA* pts);
void rotate(PTS_2DA* pts);
void stop(PTS_2DA* pts);

#else

#endif
