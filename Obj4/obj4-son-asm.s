
	thumb

TIM3_CCR3	equ	0x4000043C	; adresse registre PWM

	area  	madata, data, readwrite
	import	min
	import	max

	area  	moncode, code, readonly
	export 	sound_callback
	get	etat.inc
	import	compos_cont
	import	fact_asm
	import	etat
	export	recup_min_max
	
;

recup_min_max proc
	
	PUSH 	{R4}
	
	LDR 	R0, =etat			; etat puis Son[i]
	LDR 	R1, [R0, #E_TAI]		; taille
	LDR	R2, [R0, #E_SON]		; son[]
	LDRSH 	R3, [R2]				; Valeur du MIN
	LDRSH 	R12, [R2]				; Valeur du MAX
	MOV 	R4, #0				; i
	
boucle 
	
	LDRSH 	R0, [R2,R4, LSL #1]
	
 	CMP 	R3,R0				; if (min < Son[i])
	BLE 	calcul_max
	MOV 	R3, R0
	
calcul_max
	
	CMP 	R12,R0				; if (max > Son[i])
	BGE	fin_boucle
	MOV 	R12, R0

fin_boucle
	ADD 	R4, #1
	CMP 	R1, R4
	BNE 	boucle

	LDR 	R0, =min
	LDR 	R1, =max
	
	STR 	R3,[R0]
	STR	R12, [R1]

	POP 	{R4}

	BX 	LR

	ENDP
		
sound_callback	proc

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

	CMP	R1, R2 ; if (position >= taille)
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
