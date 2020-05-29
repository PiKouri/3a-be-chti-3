
	thumb

TIM3_CCR3	equ	0x4000043C	; adresse registre PWM

	area  	moncode, code, readonly
	export 	timer_callback
	get	etat.inc
	import	compos_cont
	import	fact_asm
	import	etat
	
;		
timer_callback	proc

;	R0 : adresse etat
;	R1 : position
;	R2 : taille | puis utilisée pour avoir Son[position] et résultat final 
;	R3 : Son[]
;	R4 : compos_cont
;	R5 : fact_asm
;	R12 : TIM3_CCR3

	PUSH	{R4,R5}

	LDR	R0, =etat
	LDR	R1, [R0, #E_POS]
	LDR	R2, [R0, #E_TAI]
	LDR	R3, [R0, #E_SON]
	LDR	R4, =compos_cont
	LDR	R4, [R4]
	LDR	R5, =fact_asm
	LDR	R5, [R5]
	LDR	R12, =TIM3_CCR3

	CMP	R1, R2 ; if (position > taille)
	BLE	SUITE

	LDR	R2, [R0, #E_RES]
	LSR	R2, #1
	B	FIN
	
SUITE	

	LDRSH	R2, [R3, R1, LSL #1] ; R2 = Son[position] (adresse = R3 + R1*2)

;	Incremente position
	ADD	R1, #1
	STR	R1, [R0, #E_POS]

;	Calcul de la valeur à envoyer
	ADD	R2, R4
	MUL	R2, R5
	LSR	R2, #16
	
FIN	STR	R2, [R12]
	
	POP	{R4,R5}
	BX	LR

	endp
;
	end
