#include <reg51.h>
//#include "LED_lab03.c"

sbit RS = P3^6;
sbit EN = P3^7;



//define linhas do teclado
sbit linha1 = P0^7;
sbit linha2 = P0^6;
sbit linha3 = P0^5;
sbit linha4 = P0^4;

//define colunas do teclado
sbit coluna1 = P0^0;
sbit coluna2 = P0^1;
sbit coluna3 = P0^2;


//define o pulso de leitura do osciloscópio
sbit pulso = P3^5;



#define NUMERO_HOTEIS 6 // Considerando 6 hotéis
#define init_state 20

// Lista de nomes de cada hotel cadastrado
unsigned char code NOMES[NUMERO_HOTEIS][16] = {
    "Hotel Classic",
    "Master Plaza",
    "Pousada Feliz",
    "Aparts Hotel",
    "Casa Verde",
    "Hotel Imperador"};

// Coordenadas x e y em metros de cada hotel
unsigned short code POSICOES[NUMERO_HOTEIS][2] = {
    308, 190,
    220, 590,
    230, 170,
    710, 230,
    930, 260,
    230, 190};

// Lista de RESULT
		
unsigned char xdata RESULT[NUMERO_HOTEIS][16];
xdata unsigned char ordem[NUMERO_HOTEIS];
xdata unsigned long distancia[NUMERO_HOTEIS];
xdata long   posx, posy;
xdata unsigned short btn_ant_state;




void atraso()
{
	unsigned char c;
	for(c=0;c<50;c++);
}

void atraso_debouncing()
{
	
	pulso = 1;//Liga pino osciloscopio
	TMOD = 0x01; //modo 16bits ("65536") do Timer 1
	TR0=1;  //Timer 1 Run (Liga o timer)
	TF0=0; //Zera a Flag
	TH0=0xF8; //Parte alta do Timer 1
	TL0=0xF9; //Parte baixa do Timer 1
	while(!TF0); //Repete até dar flag diferente de 0
	pulso = 0;//Desliga pino osciloscopio
}

void set_LCD()
{
	RS = 0;
	EN = 1;
	P1 = 0x38;
	EN = 0;
	atraso();
	
	RS = 0;
	EN = 1;
	P1 = 0x0c;
	EN = 0;
	atraso();
	
	RS = 0;
	EN = 1;
	P1 = 0x06;
	EN = 0;
	atraso();
}

void limpa_DISPLAY()
{
	RS = 0;
	EN = 1;
	P1 = 0x80;
	EN = 0;
	atraso();
}

void escreve_DISPLAY(char mensagem)
{
	RS = 1;
	EN = 1;
	P1 = mensagem;
	EN = 0;
	atraso();
}


void calcula_distancia()
{
	xdata long distx,disty;
	xdata unsigned short i=0;
	for(i=0;i<NUMERO_HOTEIS;i++)
	{
		distx = (POSICOES[i][0] - posx);		
		disty = (POSICOES[i][1] - posy);
		
		distancia[i] = distx*distx+disty*disty;
		ordem[i] = i;		
	}
	
}

void ordena_hoteis()
{
	xdata short menor;
	xdata unsigned long aux;
	xdata unsigned short i=0,j=0;
	
	for(i=0;i<NUMERO_HOTEIS-1;i++)
	{
		menor = i;
		for(j=(i+1);j<(NUMERO_HOTEIS);j++)
		{
			if(distancia[j] < distancia[menor])
			{
				menor = j;
			}
		}
		if(menor != i)
		{
			aux = distancia[i];
			distancia[i] = distancia[menor];
			distancia[menor] = aux;
			
			aux = ordem[i];
			ordem[i] = ordem[menor];
			ordem[menor] = aux;
			
		}
	}
	
	
}

void calcula_raio()
{
	xdata unsigned short i,k;
	for(i=0;i<=NUMERO_HOTEIS;i++)
	{
		if(distancia[i] < 640000)
		{
			for(k=0;k<16;k++)
			{
				RESULT[i][k] = NOMES[ordem[i]][k];
			}
		}
	}

}

void show_nearst_HOTEL()
{
	xdata unsigned short m;
	for(m=0;m<16;m++)
	{
		escreve_DISPLAY(RESULT[0][m]);
	}
}



unsigned short testa_tecla(unsigned short tecla)
{
	
	if(tecla == 1)
	{
				linha1 = 0;
				if(coluna1 == 0)
				{
					linha1 = 1;
					return 1;
				}
				else
				{
					linha1 = 1;
					return 0;
				}
				
	}	

	if(tecla == 2)
	{
			linha1 = 0;
			if(coluna2 == 0)
			{
				linha1 = 1;
				return 2;
			}
			else
			{
				linha1 = 1;
				return 0;
			}			
	}
			
			
	if(tecla == 3)
	{
			linha1 = 0;
			if(coluna3 == 0)
			{
				linha1 = 1;
				return 3;
			}
			else
			{
				linha1 = 1;
				return 0;
			}
			
	}
	

}

unsigned short deboucing(unsigned short keynow)
{
	
	xdata unsigned short count = 0;	
	xdata unsigned short keylast = keynow;
	
	
	while(count<7)
	{
		atraso_debouncing();
		
		keynow = testa_tecla(keynow);
		if(keynow == 0)
		{
			return 0;
		}
		if(keynow == keylast)
		{
			count +=1;			
		}
		else
		{
			return 0;
		}

		
	}
	
	return 1;
}

void set_postion(unsigned short bt)
{
	if(bt == 1)
	{
		posx = 100;
		posy = 100;
	}
	if(bt == 2)
	{
		posx = 200;
		posy = 200;
	}
	if(bt == 3)
	{
		posx = 300;
		posy = 300;
	}
}


void main () 
{	
	xdata unsigned short z;
	xdata unsigned short atualiza;
	xdata unsigned short btn = init_state;
	btn_ant_state = init_state;
		
	set_LCD();
	limpa_DISPLAY();
	
	while(1)
	{
		
		for(z=1;z<=3;z++)
		{
			atualiza = deboucing(z);
			if(atualiza == 1)
			{
				btn = z;				
				z = 500;
			}
		}
		
		
		

		if(btn != init_state)
		{
			set_postion(btn);
		}
		

		
		if(btn_ant_state != btn)
		{
			limpa_DISPLAY();
			calcula_distancia();
			ordena_hoteis();
			calcula_raio();			
			
			show_nearst_HOTEL();
			
			btn_ant_state = btn;
		}
		
	}

}