#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.MCB1700::Board Support:Graphic LCD
#include "stdio.h"
#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include "LPC17xx.h"
#include "GPIO.h" 

extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;
extern ARM_DRIVER_USART Driver_USART0;


#define LIDAR_STOP              0x25        // byte d'arret du scan
#define LIDAR_RESET             0x40        // byte de reinitialisation 
#define LIDAR_SCAN              0x20        // byte de demarage du scan
#define EXPRESS_SCAN            0x82        // byte de demarage du scan express
#define GET_INFO                0x50        // byte de reception des infos 
#define GET_HEALTH              0x52        // byte de reception de l'état de santé
#define GET_SAMPLERATE          0x29        // byte de réception de la fréquence d'échantillonage 
#define START_FLAG              0xA5        // byte necessaire a l'envoi des trames

//tableau de stockage des signaux de commande

char Macros[8]={LIDAR_STOP,LIDAR_RESET,LIDAR_SCAN ,EXPRESS_SCAN, GET_INFO, GET_HEALTH, GET_SAMPLERATE,START_FLAG };

//tableaux de récupération des données du LIDAR
char SCAN[5];
char INFO[20];
char HEALTH[3];
char SAMPLE[4];
char DATARESPONSE[7];

//initialisation
void Init_UART0(void);
void InitLidar(void);

//fonctions d'envoi
void debutScan(void);
void stopScan(void);
void sante(void);

//fonctions de réception
char statut(void);
void etatDeSante(void);

//affichage des infos
char quality;
char angle;
char distance;

int main()
{	
	InitLidar();
	stopScan();
	
	while(1)
	{	
		sante();
		if(statut() == 1)
		{
			debutScan();
			if((SCAN[0] & 0x02) != (SCAN[0] &0x01))
			{
				quality = SCAN[0] >> 2;
			
			}
			angle = (SCAN[1] >> 1) || (SCAN[2] << 7);
			distance = (SCAN[3] || SCAN[4] << 8);
		}

	}


return 0;
}

void InitLidar(void)
{
	Initialise_GPIO();
	Init_UART0();

	GLCD_Initialize();
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_16x24);

}
void Init_UART0(void)
{
	Driver_USART0.Initialize(NULL);
	Driver_USART0.PowerControl(ARM_POWER_FULL);
	Driver_USART0.Control(	ARM_USART_MODE_ASYNCHRONOUS |
							ARM_USART_DATA_BITS_8		|
							ARM_USART_STOP_BITS_1		|
							ARM_USART_PARITY_NONE		|
							ARM_USART_FLOW_CONTROL_NONE,
							115200);
	Driver_USART0.Control(ARM_USART_CONTROL_TX,1);
	Driver_USART0.Control(ARM_USART_CONTROL_RX,1);
}



void debutScan(void)
{	
	while (Driver_USART0.GetStatus().tx_busy==1);
	Driver_USART0.Send(&Macros[7], 1); //envoi du flag
	while (Driver_USART0.GetStatus().tx_busy==1);
	Driver_USART0.Send(&Macros[2], 1);

}

void stopScan(void)
{	
	while (Driver_USART0.GetStatus().tx_busy==1);
	Driver_USART0.Send(&Macros[7], 1); //envoi du flag
	while (Driver_USART0.GetStatus().tx_busy==1);
	Driver_USART0.Send(&Macros[0], 1);

}

void sante(void)
{	
	while (Driver_USART0.GetStatus().tx_busy==1);
	Driver_USART0.Send(&Macros[7], 1); //envoi du flag
	while (Driver_USART0.GetStatus().tx_busy==1);
	Driver_USART0.Send(&Macros[5], 1);
	
}

char statut(void)
{	
	Driver_USART0.Receive(DATARESPONSE,7);
	while(Driver_USART0.GetRxCount()<1);
	Driver_USART0.Receive(&HEALTH[0],1);
	while(Driver_USART0.GetRxCount()<1);
	if (HEALTH[0] == 0) return 1;
	else return 0;
	
}		
	
void 	data(void)
{
	Driver_USART0.Receive(DATARESPONSE,7);
	while(Driver_USART0.GetRxCount()<1);
	if((DATARESPONSE[0] == 0xA5) && (DATARESPONSE[1] == 0x5A) && (DATARESPONSE[2] == 0x05) && (DATARESPONSE[3] == 0x00) && (DATARESPONSE[4] == 0x00) && (DATARESPONSE[5] == 0x40) && (DATARESPONSE[6] == 0x81))
	{
		Driver_USART0.Receive(SCAN,5);
		while(Driver_USART0.GetRxCount()<1);
	}
	
}





















void etatDeSante(void) 
{ 

	char Verif_connexion[10]={0};
	char etat_de_sante[10];
  int test_time_out;
 
  char i;
	char tab[1];
	while(Driver_USART0.GetStatus().tx_busy==1);
	Driver_USART0.Send(&Macros[7] ,1);

  while(Driver_USART0.GetStatus().tx_busy==1);
	Driver_USART0.Send(&Macros[5],1);

	Driver_USART0.Receive(Verif_connexion,10);
	while(Driver_USART0.GetRxCount()<1);
	
//sprintf(etat_de_sante,"Rep req %s",Verif_connexion);	
		GLCD_DrawString(100,100,Verif_connexion);	
													 
/*	if (Verif_connexion[0]==0){//sprintf(etat_de_sante,"0: Good");
		                         GLCD_DrawString(100,100,etat_de_sante);}	
	
else if (Verif_connexion[0]==1){ sprintf(etat_de_sante,"1: Warning");	
	                               	GLCD_DrawString(100,100,etat_de_sante);	}	

else {                          sprintf(etat_de_sante,"2: Error");	
		                            GLCD_DrawString(100,100,etat_de_sante);	}	
*/

//	Driver_USART1.Receive(etat_de_sante,3);
//	while(Driver_USART1.GetRxCount()<1);
// 


//                                
// // *pointeur_etat_sante = etat_de_sante[0];


// //  sprintf(Verif_connexion,"Blabla %c",Verif_connexion[0]);	
//		GLCD_DrawString(100,130,etat_de_sante);
//													 

}








             

																			 