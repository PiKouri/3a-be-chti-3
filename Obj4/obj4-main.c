#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "gassp72.h"
#include "etat.h"

int32_t calculM2TIR() {
	return  pow(50 * 1.241 * 64 /2, 2)/4;
}

int32_t dma_buf [64];
int occurences [6];
int occurences_min = 3; // 3, arbitraire
int score [6];
int valeurk [6] = {17, 18, 19, 20, 23, 24};
int32_t M2TIR;
int SYSTICK_PER = 14400; // Pour avoir une période de 5 ms

int32_t M2(int32_t* adresse_signal, int k);
extern int32_t TabSig;

int laser_detecte = 0;

// Gestion du son

void sound_callback(void);
int Periode_en_Tck=72000;
int Periode_PWM_en_Tck = 720;
extern short Son[];
extern int LongueurSon;
extern int PeriodeSonMicroSec;
type_etat etat;
int compos_cont;
short min;
short max;
float fact_echelle;
int fact_asm;
int son_joue = 0;
void recup_min_max(void);

void sys_callback() { // Gestion du laser
	// Démarrage DMA pour 64 points
	Start_DMA1(64);
	Wait_On_End_Of_DMA1();
	Stop_DMA1;
	for (int i=0; i<6; i++) {
		if (M2(dma_buf, valeurk[i]) >= M2TIR) occurences[i]++;
		else occurences[i] = 0;
		if (occurences[i] == occurences_min) {
			etat.position = 0; // Joue le son
			score[i]++;
		}
	}
	int i=0;
	laser_detecte = 0;
	while (i<6 && !laser_detecte) { // Vérifie qu'1 laser est allumé au moins
		laser_detecte = occurences[i]>=occurences_min;
		i++;
	}
	/*if (laser_detecte && !son_joue) {
		son_joue = 1;
		etat.position = 0; // Joue le son
	}*/
	if (laser_detecte) GPIO_Set(GPIOB, 14); // Allume la LED
	else { 
		GPIO_Clear(GPIOB, 14);
		/*son_joue = 0;
		etat.position = etat.taille;*/
	}
}

int main(void) {
M2TIR = calculM2TIR();
GPIO_Clear(GPIOB, 14); // Eteint la LED

// Gestion du son

// config port PB0 pour être utilisé par TIM3-CH3
GPIO_Configure(GPIOB, 0, OUTPUT, ALT_PPULL);
// config TIM3-CH3 en mode PWM
etat.resolution = PWM_Init_ff( TIM3, 3, Periode_PWM_en_Tck );
etat.taille = LongueurSon;
etat.periode_ticks = PeriodeSonMicroSec*72;
etat.son = Son;

etat.position = etat.taille ; // Ne joue pas le son tant qu'on n'a pas mis la position à 0

/*max = Son[0];
min = Son[0];
for (int i=0; i<LongueurSon; i++) {
	if (Son[i] > max) max = Son[i];
	else if (Son[i] < min) min = Son[i];
}*/
recup_min_max();
compos_cont = ((min < 0) ? -(float)min : 0);
fact_echelle = etat.resolution/((float)max + compos_cont) ;
fact_asm = fact_echelle * pow(2, 16);

// activation de la PLL qui multiplie la fréquence du quartz par 9
 CLOCK_Configure();
// initialisation du timer 4
// Periode_en_Tck doit fournir la durée entre interruptions,
// exprimée en périodes Tck de l'horloge principale du STM32 (72 MHz)
Timer_1234_Init_ff( TIM4, etat.periode_ticks );
// enregistrement de la fonction de traitement de l'interruption timer
// ici le 2 est la priorité, timer_callback est l'adresse de cette fonction, a créér en asm,
// cette fonction doit être conforme à l'AAPCS
Active_IT_Debordement_Timer( TIM4, 2, sound_callback );
// lancement du timer
Run_Timer( TIM4 );

while	(1)
	{
		// activation de la PLL qui multiplie la fréquence du quartz par 9
		CLOCK_Configure();
		// PA2 (ADC voie 2) = entrée analog
		GPIO_Configure(GPIOA, 2, INPUT, ANALOG);
		// PB1 = sortie pour profilage à l'oscillo
		GPIO_Configure(GPIOB, 1, OUTPUT, OUTPUT_PPULL);
		// PB14 = sortie pour LED
		GPIO_Configure(GPIOB, 14, OUTPUT, OUTPUT_PPULL);

		// activation ADC, sampling time 1us
		Init_TimingADC_ActiveADC_ff( ADC1, 82 ); // 72 par défaut
		Single_Channel_ADC( ADC1, 2 );
		// Déclenchement ADC par timer2, periode (72MHz/320kHz)ticks
		Init_Conversion_On_Trig_Timer_ff( ADC1, TIM2_CC2, 225 );
		// Config DMA pour utilisation du buffer dma_buf (a créér)
		Init_ADC1_DMA1( 0, (vu16 *) dma_buf);
		
		// Config Timer, période exprimée en périodes horloge CPU (72 MHz)
		Systick_Period_ff( SYSTICK_PER );
		// enregistrement de la fonction de traitement de l'interruption timer
		// ici le 3 est la priorité, sys_callback est l'adresse de cette fonction, a créér en C
		Systick_Prio_IT( 3, sys_callback );
		SysTick_On;
		SysTick_Enable_IT;
		
	}
}
