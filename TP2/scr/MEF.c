#define F_CPU 16000000UL
#include <MEF.h>
#include <Keypad.h>
#include <stdlib.h>
#include <util/delay.h>
#include <Timer.h>
#include <math.h>

typedef enum {INIT, MOSTRARPALABRA, ESCRIBIR,
DERROTA, VICTORIA} eMEF_state;


static eMEF_state MEF_state;
static uint8_t key_code;

//definir valor de cada switch
static const char keymap[16] = {
	'1','2','3','A',
	'4','5','6','B',
	'7','8','9','C',
	'*','0','#','D'
};

//palabras a repetir
const char* palabras[] = {
	"Arbol", "Boton", "CDyMC", "ClavE", "Facil", "Gafas", "Hojas", "LiBro",
	"Lanza", "Nieve", "PeRro", "PecES", "PiAno", "PrYKe", "RUEDa",
	"SERIE", "SalUd", "Salud", "Silla", "Tecla", "Valor", "Verde",
	"YnHRz", "hARdD", "silla"
};

int num_palabras = sizeof(palabras) / sizeof(palabras[0]);
int indice_random;
uint8_t ascci_char = 0;
int idx_letra = 0;
int index = 0;
int sigChar = 0;
int errores = 0;
char buffer[4];
int seed = 0; //semilla para randomizar
uint64_t tiempoJuego = 0; // Sera para el tiempo total
uint64_t tiempoInicio = 0; // Indica en que tiempo del timer comenzo a jugar
int timerTicks = 0; 
int mostrar = 0;  // Para no imprimir varias veces lo mismo
int perdio = 0;  // Para indicar si perdio en la partida actual

//Inicializa LCD
void MEF_init(void){
	MEF_state = INIT; //Cambia estado a INIT
	LCDinit();
	LCDclr();
	LCDGotoXY(0, 0);
	LCDstring("PRESIONE *",10);
	srand(time(NULL));
}

void MEF_update(void){
	
	switch(MEF_state){
		
		//Espera que se presione switch "*"
		case INIT:
			seed++;
			if (KEYPAD_Scan(&key_code)){
				if(keymap[key_code] == '*'){
					srand(seed);
					tiempoJuego = 0;
					MEF_state = MOSTRARPALABRA; //Cambia estado a MOSTRARPALABRA
				}
			}
		break;
		
		//Muestra palabra aleatoria por 2 segundos, y limpia LCD
		case MOSTRARPALABRA:
			if(!mostrar){
				indice_random = rand() % num_palabras;
				LCDclr();
				LCDGotoXY(0, 0);
				LCDstring(palabras[indice_random],5);	
				mostrar = 1;
			}
			if (timerTicks != 2000){
				timerTicks++;
			}else{
				LCDclr();
				LCDGotoXY(0, 1);
				ascci_char = 0;
				idx_letra = 0;
				index = 0;
				sigChar = 0;
				errores = 0;
				MEF_state = ESCRIBIR; //Cambia estado a ESCRIBIR
				if (perdio == 0){
					tiempoInicio=get_tiempoTimer();
				}
				timerTicks = 0;
				mostrar = 0;
			}
		break;
		
		//Verifica si la palabra es la correcta
		case ESCRIBIR:
		
			if (KEYPAD_Scan(&key_code)){
				
				//al oprimir "#" finaliza string equivalente a un caracter en ascii
				if(keymap[key_code] == '#'){
					LCDGotoXY(idx_letra, 0);
					buffer[index] = '\0';           // finaliza string
					int ascii = atoi(buffer);       // convierte a número
					index = 0;
					
					//Si letra es correcta, la imprime en pantalla, sino suma error
					if(palabras[indice_random][sigChar] == ascii){
						LCDsendChar(ascii);
						LCDGotoXY(0, 1);
						LCDstring("                ",16);
						LCDGotoXY(0, 1);
						sigChar++;
						idx_letra++;
					}else{
						LCDGotoXY(0, 1);
						LCDstring("                ",16);
						LCDGotoXY(0, 1);
						errores++;
					}
					
					//A los tres errores cambia estado a DERROTA
					if(errores == 3){
						MEF_state = DERROTA;
					}
					
					//Al tener todas las letras correctas cambia estado a VICTORIA
					if(sigChar == 5){
						MEF_state = VICTORIA;
					}
				}
				
				//Solo lee carater numerico. No toma switch de letras
				if (keymap[key_code]>='0' && keymap[key_code]<='9' && index < 3) {
					LCDsendChar(keymap[key_code]);
					buffer[index++] = keymap[key_code];
				}
			}
		break;
		
		//Imprime derrota en LCD durante 5 segundos y sigue el juego
		case DERROTA:
			if (!mostrar){
				LCDclr();
				LCDGotoXY(0, 0);
				LCDstring("derrota",7);
				mostrar = 1;
				perdio = 1;
			}
			if (timerTicks != 5000){
				timerTicks++;
			}else{
				MEF_state =	MOSTRARPALABRA; //cambia estado a MOSTRARPALABRA
				timerTicks = 0;
				mostrar = 0;
			}
			
		break;
		
		//Imprime victoria en LCD durante 5 segundos y reinicia juego
		case VICTORIA:
			
			if (!mostrar){
				LCDclr();
				LCDGotoXY(0, 0);
				LCDstring("victoria",8);
				LCDGotoXY(0, 1);
				tiempoJuego = get_tiempoTimer() - tiempoInicio; //tiempo de fin de juego menos tiempo en que arranco
				tiempoJuego = tiempoJuego / 1000; //Paso de milisegundos a segundos
				int aux = (int)tiempoJuego;
				int i = 0;
				//Buscamos saber cuantos digitos tiene el tiempo
				while(aux){
					i++;
					aux = aux/10;
				}
				LCDGotoXY(i, 1); //impresion mensaje
				LCDstring(" segs",5); 
				aux = (int)tiempoJuego;
				uint8_t ch;
				int iaux = i;
				//imprimo tiempo digito por digito
				while(i>0){
					LCDGotoXY(i-1, 1);
					ch=(aux%10 +'0');
					LCDsendChar(ch);
					aux=aux / 10;
					i--;
				}
				mostrar = 1;
				perdio = 0;
				LCDGotoXY(iaux + 5, 1);
			}
			//timer de 5 segundos y luego cambia a INIT
			if (timerTicks != 5000){ 
				timerTicks++;
				
				}else{
					
				LCDclr();
				LCDGotoXY(0, 0);
				LCDstring("PRESIONE *",10);
				MEF_state = INIT; //cambia estado a INIT
				timerTicks = 0;
				mostrar = 0;
			}
		break;
	}
	
	
}
