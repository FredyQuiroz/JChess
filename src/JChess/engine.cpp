
//Tipo de datos que contiene un puntero a una linea de una pantalla
typedef WORD *ScreenLines;


//Estructura de datos que contiene una pantalla
typedef struct{
                  WORD AX,AY; //Alto y Ancho
                  ScreenLines *Lines; //Puntero a las lineas de la pantalla
                } Screen;


//Contiene la pantalla fisica.
Screen pantallafisica;


//Es un puntero a la pantalla que se esta usando actualmente
Screen *pantallaenuso;


//Numero maximo de pantallas.
#define maxpantallas 1000
//Espacio reservado para las pantallas que se van a usar.
Screen pantalla[maxpantallas];


//Funcion que avisa de un error producido en alguna de las rutinas
//nos ayudara a depurar el codigo.
void Error(char *c,char *buf)
{
 MessageBox( hwnd, buf, c, MB_OK );
 exit(-1);
}


//Esta funcion reserva la memoria para un pantalla que contendra los
//punteros a cada una de las lineas de la pantalla fisica.
void NewScreenWin(Screen& SCR,WORD AX,WORD AY)
{
 //Se reserva la memoria para los punteros a lineas.
 //Si no se puede reservar la memoria, se genera un error.   
 if ((SCR.Lines=new ScreenLines[AY])==NULL)
 {
  Error("no hay memoria en newscreenwin","");
 }
 //Estas variables contienen el tamaño de la pantalla, por lo general
 //sera 640*480
 else
 {
  SCR.AX=AX;
  SCR.AY=AY;
 }
}


//Esta funcion reserva la memoria para una pantalla. A diferencia de la
//funcion NewScreenWin, aqui es necesario reservar la memoria para cada una
//de las lineas, ya que las pantallas creadas en esta funcion estan
//contenidas en la memoria del ordenador y no se corresponden con ninguna
//pantalla fisica.
void NewScreen(Screen& SCR,WORD AX,WORD AY)
{
 WORD Y;
 //Se reserva la memoria para los punteros a lineas.
 //Si no se puede reservar la memoria, se genera un error.   
 if ((SCR.Lines=new ScreenLines[AY])==NULL)
 {
  Error("no hay memoria en newscreen","");
 }
 //Estas variables contienen el tamaño de la pantalla, por lo general
 //sera 640*480
 else
 {
  SCR.AX=AX;
  SCR.AY=AY;
  //Se recorren todas la lineas.
  for (Y=0;Y<AY;Y++)
  {
   //Se reserva la memoria para cada una de las lineas.
   SCR.Lines[Y]=new WORD[AX];
   //Se rellena cada linea con el valor 0, que es el color negro.
   //El numero de bytes a rellenar es el ancho*2 (porque cada pixel
   //emplea un total de 2 bytes).
   _fmemset(SCR.Lines[Y],0,AX*2);
  }
 }
}


//Esta funcion sirve para borrar una pantalla que ha sido creada previamente
//con la funcion NewScreen. Su utilidad es liberar memoria por si dicha pantalla
//va a ser usada de nuevo.
//NOTA: Intentar borrar una pantalla que no ha sido previamente creada puede
//ocasionar un error en la aplicacion.
void ClearScreen(Screen& SCR)
{
 WORD Y;

 //Se recorren todas la lineas.
 for (Y=0;Y<SCR.AY;Y++)
  //Se libera la memoria de cada una de las lineas.
  if (SCR.Lines[Y]!=NULL)
   delete SCR.Lines[Y];

 //Se libera la memoria de los punteros a lineas.
 if (SCR.Lines!=NULL)
  delete SCR.Lines;
}


//Esta funcion es muy potente, copia un trozo de una pantalla a otra.
//La primera pantalla es el origen, y la segunda el destino.
//Se copia un rectangulo contenido entre las coordenadas (Xo,Yo)(Xo1,Yo1)
//de la pantalla origen a las coordendas (Xd,Yd) de la pantalla destino.
//Nota: Si parte de la pantalla que se esta copiando queda fuera, se
//generara un error de proteccion de memoria.
void ScreenCopy(Screen& Sc,Screen& Dt,int Xo,int Yo,int Xo1,int Yo1,
                int Xd,int Yd)
{
 //Variables usadas en la funcion.
 WORD A,AY=Yo1-Yo+1;
 //En lugar de copiar la pantalla pixel a pixel, se hace con funciones
 //de copiar memoria, que es varias veces mas rapido.
 //La copia se realizara linea a linea. El tamaño en bytes de cada linea
 //que se copia es su ancho*2 (cada pixel son dos bytes).
 int aux=(abs(Xo1-Xo)+1)*2;
 //Se recorren todas la lineas, copiandose el contenido de cada una
 //desde la pantalla origen a la de destino.
 for(A=0;A<AY;A++)
  _fmemcpy(Dt.Lines[Yd+A]+Xd,Sc.Lines[Yo+A]+Xo,aux);
}


//Es similar ScreenCopy, con la diferencia que el color 0 no se copia.
//Debido a que tiene que hacer una comprobacion por cada pixel, es
//bastante mas lento que ScreenCopy.
void ScreenCopyTrans(Screen& Sc,Screen& Dt,int Xo,int Yo,int Xo1,int Yo1,
                int Xd,int Yd)
{
 //Variables usadas en la funcion.
 WORD A,B,AY=Yo1-Yo+1;
 //Dos punteros a WORD temporales.
 WORD *p_o,*p_d;
 //La copia se realizara linea a linea. 
 int aux=(abs(Xo1-Xo)+1);
 //Se recorren todas la lineas.
 for(A=0;A<AY;A++)
 {
  //Se asigna a cada puntero temporal el primer pixel de la linea
  //que se va a copiar de origen a destino.
  p_o=Sc.Lines[Yo+A]+Xo;
  p_d=Dt.Lines[Yd+A]+Xd;
  //Se recorren todos los puntos de cada linea.
  for(B=0;B<aux;B++)
  {
   //Si el pixel origen es distinto de cero, lo copia, en otro caso no.
   //realiza la copia.
   if(p_o[B]!=0)
    p_d[B]=p_o[B];
  }
 }
}


//Definimos el valor solido como valor 0 
#define SOLIDO 0
//Definimos el valor transparente como valor 1.
#define TRANSPARENTE 1


//Esta funcion es una mejora para la funcion ScreenCopy. Evita los errores
//que se generan si alguna parte del area a copiar queda fuera de alguna
//de las dos pantallas.
//Lo que realmente hace es "recortar" el area a copiar a los limites de
//cada pantalla, y luego llama a ScreenCopy con los valores recortados.
//
//Se le ha añadido el parametro de la transparencia.
//Si Trans=SOLIDO, realiza la copia con ScreenCopy.
//Si Trans=TRANSPARENTE, realiza la copia con ScreenCopyTrans.
void ScreenCopyClip(Screen& Sc,Screen& Sd,int Xo,int Yo,int Xo1,int Yo1,
                int Xd,int Yd,int Trans)
{
 //Valores auxiliares de ayuda.
 int Xd1=Xd+(Xo1-Xo),Yd1=Yd+(Yo1-Yo);

 //Primer y ultimo pixel valido de cada pantalla en sus coordenadas X e Y.
 int ScClipX=0,ScClipX1=Sc.AX-1;
 int ScClipY=0,ScClipY1=Sc.AY-1;
 int SdClipX=0,SdClipX1=Sd.AX-1;
 int SdClipY=0,SdClipY1=Sd.AY-1;

 //Si el area a copiar esta totalmente fuera de la pantalla de origen
 //no se copia nada.
 if( (Xo>ScClipX1)||(Yo>ScClipY1)||(Xo1<ScClipX)||(Yo1<ScClipY) )
  return;
 //Si el area a copiar esta totalmente fuera de la pantalla de destino
 //tampoco se copia nada.
 if( (Xd>SdClipX1)||(Yd>SdClipY1)||(Xd1<SdClipX)||(Yd1<SdClipY) )
  return;

 //Se recorta el area a copiar a los limites de la pantalla de origen.
 if(Xo<ScClipX)
 {
  Xd+=(ScClipX-Xo);
  Xo=ScClipX;
 }
 if(Xo1>ScClipX1)
 {
  Xd1-=(Xo1-ScClipX1);
  Xo1=ScClipX1;
 }
 if(Yo<ScClipY)
 {
  Yd+=(ScClipY-Yo);
  Yo=ScClipY;
 }
 if(Yo1>ScClipY1)
 {
  Yd1-=(Yo1-ScClipY1);
  Yo1=ScClipY1;
 }

 //Se recorta el area a copiar a los limites de la pantalla de destino.
 if(Xd<SdClipX)
 {
  Xo+=(SdClipX-Xd);
  Xd=SdClipX;
 }
 if(Xd1>SdClipX1)
 {
  Xo1-=(Xd1-SdClipX1);
  Xd1=SdClipX1;
 }
 if(Yd<SdClipY)
 {
  Yo+=(SdClipY-Yd);
  Yd=SdClipY;
 }
 if(Yd1>SdClipY1)
 {
  Yo1-=(Yd1-SdClipY1);
  Yd1=SdClipY1;
 }

 //De nuevo se comprueba si el area queda totalmente fuera de la pantalla
 //origen.
 if( (Xo>ScClipX1)||(Yo>ScClipY1)||(Xo1<ScClipX)||(Yo1<ScClipY) )
  return;
 //Se hace la misma comprobacion para la pantalla de destino.
 if( (Xd>SdClipX1)||(Yd>SdClipY1)||(Xd1<SdClipX)||(Yd1<SdClipY) )
  return;

 //Se realiza la copia con el area recortada a dichos limites.
 if(Trans==0)
  //copia normal (solido).
  ScreenCopy(Sc,Sd,Xo,Yo,Xo1,Yo1,Xd,Yd);
 else
  //copia con color 0 transparente.
  ScreenCopyTrans(Sc,Sd,Xo,Yo,Xo1,Yo1,Xd,Yd);
}


//Esta funcion pinta un rectangulo relleno de un color en la pantalla en uso.
//Se pinta el rectangulo contenido entre las coordenadas (Xo,Yo)(Xo1,Yo1)
//Nota: Si parte del rectangulo que se esta pintando queda fuera, se
//generara un error de proteccion de memoria.
void Box(int X,int Y,int X1,int Y1,WORD Color)
{
 //Variables usadas en la funcion.
 WORD A;
 //En lugar de pintar la pantalla pixel a pixel, se hace con funciones
 //de relleno de la memoria, que es varias veces mas rapido.
 //El relleno se realizara linea a linea. El tamaño en bytes de cada linea
 //que se copia es su ancho*2 (cada pixel son dos bytes).
 int aux=(X1-X+1)*2;       
 //Se recorren todas la lineas, rellenandose con el valor Color.
 for (A=Y;A<=Y1;A++)
 {
  _fmemset(pantallaenuso->Lines[A]+X,Color,aux);
 }    
}


//Esta funcion tiene un uso muy concreto. Su objetivo es facilitar el
//acceso a los puntos de la pantalla. Genera una lista de 480 punteros
//contenidos en la estructura de datos "Screen", cada uno de los cuales
//contiene la direccion de memoria donde esta el principio de cada linea
//de la pantalla.
//El parametro "pointer" es la direccion del primer pixel de la pantalla.
void UpdateScreenPointers(Screen& SCR,WORD *pointer)
{
 int Y;

 //Recorre todas las lineas de la pantalla.
 for (Y=0;Y<SCR.AY;Y++)
 {
  //Asigna a cada puntero de la pantalla el principio de una linea.
  SCR.Lines[Y]=pointer;
  //Incrementa el puntero a la siguiente linea.
  pointer+=(long)(SCR.AX);
 }
}


//Esta funcion prepara el BackSurface para que se puedan pintar graficos
//en el. Para poder tener acceso es necesario hacerle un "Lock".
void PrepareRealScreen()
{
 //Esta variable contiene el puntero a la pantalla donde se dibujan
 //los graficos.
 WORD *screenpointer;

 //Se hace un "Lock" al BackSurface, a partir de ahora se puede
 //tener acceso a el.
 lpDDSBack->Lock(NULL, &ddsd, 0, NULL);
 //Con esta linea se obtiene la direccion de memoria en la cual empieza
 //el contenido del BackSurface. Es la direccion del primer pixel de la
 //pantalla de fondo.
 screenpointer=(WORD *)ddsd.lpSurface;

 //Se actualizan los punteros del principio de cada linea de
 //la pantalla fisica.
 UpdateScreenPointers(pantallafisica,screenpointer);
}


//En esta funcion, el contenido del BackSurface o pantalla de fondo es copiado
//al FrontSurface o pantalla fisica. Todo lo que se hubiera pintado en el
//BackSurface pasa ahora a verse en la pantalla del ordenador.
void UpdateRealScreen()
{
 //Se crean algunas estructuras de datos necesarias
 HRESULT     ddrval;
 RECT        rcRect;
 LPDIRECTDRAWSURFACE pdds;

 //Es necesario hacer un "Unlock" al BackSurface, ya no se puede
 //pintar en el.
 lpDDSBack->Unlock(ddsd.lpSurface);

 //Se crea un rectangulo con el tamaño de la pantalla que se va
 //a copiar. En este caso se trabaja con una resolucion de 640*480.
 rcRect.left = 0;
 rcRect.top = 0;
 rcRect.right = 640;
 rcRect.bottom = 480;

 //Se asigna el BackSurface a este puntero.
 pdds = lpDDSBack;
 while(1)
 {
  //Se efectua la copia del BackSurface al FrontSurface   
  ddrval = lpDDSPrimary->Blt(&rcRect, pdds, &rcRect, 0, 0);
  //En caso de que la operacion se haya realizado con exito,
  //se termina la ejecucion de esta funcion
  if(ddrval == DD_OK)
  {
   break; 
  }    
 }   
}


//Sirve para asignar en cual pantalla se pintaran los graficos (pantalla activa)
void SetActiveScreen(Screen &s)
{
 pantallaenuso=&s;
}


//Esta funcion pone un pixel en la pantalla activa
//en las coordenadas indicadas por X e Y.
void PutPixel(WORD x,WORD y,WORD color)
{
 (*(pantallaenuso->Lines[y]+x))=color;
}


//Esta funcion retorna el valor de un pixel
//en las coordenadas indicadas por X e Y.
WORD GetPixel(WORD x,WORD y)
{
 return (*(pantallaenuso->Lines[y]+x));
}


//Esta funcion indica el numero de bits que usa un determinado numero
//Por ejemplo, el numero binario 00101101 usara un total de 6 bits.
long GetBits(long n)
{
 long cont=0;
 while(n>0)
 {
  n=n/2;
  cont++;
 }
 return cont;
}



//+++++++++++++++++++++++++++++++++++++++++++++++++++
//
//                  FORMATO TGA
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++


//Estructura de datos que contiene la cabecera de un fichero grafico
//TGA que esta sin comprimir y que no usa mapa de colores.
typedef struct
{
 BYTE idsize; //Normamente 0.
 BYTE colormaptype; //Tiene que ser 0.
 BYTE image; //Tiene que ser 2.
 BYTE colormapspec[5]; //Se ignora.
 WORD Xorigin; //Origen X.
 WORD Yorigin; //Origen Y.
 WORD AX; //Ancho.
 WORD AY; //Alto.
 BYTE bitsperpixel; //Tiene que ser 24 o 32.
 BYTE imagedesc; //Se ignora.
} TGA;


//Esta funcion carga una pantalla TGA que este sin comprimir y en
//formato de 24 o 32 bits sobre la pantalla en uso a partir de las
//coordenadas indicadas por X e Y.
void LoadTGA(char *name,long x,long y)
{
 //Cabecera del TGA.
 TGA h;
 //Manejador del fichero.
 FILE *f;
 //Variables de uso interno en la funcion.
 long pixels,size,xx,yy;
 WORD rojo,verde,azul,color;
 BYTE *buffer,*pointer,id[256];

 //En caso de no encontrarse el fichero, se genera un error.
 if ((f=fopen(name,"rb"))==NULL)
  Error("fichero no encontrado en LoadTGA:",name);

 //Se leen los 18 bits que forman la cabecera del fichero. 
 fread(&h,18,1,f);

 //En este caso, el formato TGA no esta soportado y se genera un error.
 if( (h.colormaptype!=0)||(h.image!=2) )
  Error("error al leer un TGA",name);

 //Si la pantalla TGA no esta en 24 bits ni en 32 bits, se genera un error.
 if( (h.bitsperpixel!=24)&&(h.bitsperpixel!=32) )
  Error("error al leer un TGA, numero incorrecto de bits",name);

 //En caso de que exista una descripcion del fichero TGA, se lee.
 if(h.idsize!=0)
  fread(id,h.idsize,1,f);

 //Se calcula el numero de pixeles que tiene la pantalla (ancho por alto).
 pixels=((long)h.AX)*((long)h.AY);

 //Se calcula el numero de bytes que usa la pantalla (pixeles*bits_por_pixel/8).
 size=pixels*((long)h.bitsperpixel)/8;

 //Se reserva la memoria para contener la pantalla en si.
 if ((buffer=new BYTE[size])==NULL)
 {
  Error("no hay memoria en LoadTGA","");
 }

 //Se lee la totalidad de la pantalla dentro de buffer.
 fread(buffer,size,1,f);

 //Pointer es un puntero temporal que es actualizado con el valor de buffer.
 pointer=buffer;

 //Con dos bucles anidados se recorren todos los pixeles de la pantalla.
 for(yy=0;yy<h.AY;yy++)
  for(xx=0;xx<h.AX;xx++)
  {
   //Se obtienen los colores azul, rojo y verde en valores comprendidos
   //entre 0 (minimo) y 255 (maximo).
   azul=(WORD)(*pointer++);
   verde=(WORD)(*pointer++);
   rojo=(WORD)(*pointer++);

   //Los valores de rojo, verde y azul son divididos entre cuatro para
   //facilitar su conversion (0=minimo, 63=maximo).
   azul=(azul>>2);
   verde=(verde>>2);
   rojo=(rojo>>2);

   //Ahora la variable color va a contener el resultado de convertir los
   //tres colores a un solo valor de 16 bits. Este valor tendra que ser
   //calculado de manera diferente si la tarjeta grafica usa 6 bits o
   //5 bits de color verde.
   if(bitsverde==6) //6 bits de verde.
    color=(azul>>1)+(verde<<5)+((rojo>>1)<<11);
   else
   if(bitsverde==5) //5 bits de verde.
    color=(azul>>1)+((verde>>1)<<5)+((rojo>>1)<<10);

   //Ahora se imprime cada pixel en pantalla.
   //Si el bit 5 de h.imagedesc es igual a 0, la imagen estara almacenada
   //de abajo a arriba. En el otro caso, la imagen esta almacenada de arriba
   //a abajo.
   if((h.imagedesc&&0x20)==0)
    PutPixel(xx+x,(h.AY-yy-1)+y,color);
   else
    PutPixel(xx+x,yy+y,color);

   //Si la pantalla esta en modo de 32 bits, el cuarto y ultimo byte de cada
   //pixel tiene que ser saltado, ya que no contiene ningun color.
   if(h.bitsperpixel==32)
    pointer++;
  }

 //Se libera la memoria usada por el buffer temporal.
 delete buffer;
 //Se cierra el fichero.
 fclose(f);
}



