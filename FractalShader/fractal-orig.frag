uniform sampler2D texDOMPOS, texSO, texORI;

#define semillainicial 0.5

// para rango 4
#define auxiliar1 64.0		// 256/rango
#define auxiliar2 0.015625	// 1/auxiliar1

// para rango 2
//#define auxiliar1 128.0		// 256/rango
//#define auxiliar2 0.0078125	// 1/auxiliar1

/*#pragma optimize(on)
#pragma debug(off)*/

void main (void)
{
	vec2 posaux = gl_TexCoord[0].st;
	int deep;// = 1;
	//float prec = 0.5;
	float color_ultimo = 255.0;
	//float color_penultimo;
	float factor0 = 1.0;
	float factoracum = 0.0;
	float factorultimo = 0.0;
	float sem = semillainicial;
	float a,b,c,d,e,f;
	
	/*float a2[8];
	float b2[8];
	float c2[8];
	float d2[8];
	float e2[8];
	float f2[8];
	a2[0]=2.0;a2[1]=0.0;a2[2]=-2.0;a2[3]=0.0;a2[4]=-2.0;a2[5]=0.0;a2[6]=2.0;a2[7]=0.0;
	b2[0]=0.0;b2[1]=2.0;b2[2]=0.0;b2[3]=-2.0;b2[4]=0.0;b2[5]=-2.0;b2[6]=0.0;b2[7]=2.0;
	c2[0]=0.0;c2[1]=-2.0;c2[2]=0.0;c2[3]=2.0;c2[4]=0.0;c2[5]=-2.0;c2[6]=0.0;c2[7]=2.0;
	d2[0]=2.0;d2[1]=0.0;d2[2]=-2.0;d2[3]=0.0;d2[4]=2.0;d2[5]=0.0;d2[6]=-2.0;d2[7]=0.0;
	e2[0]=0.0;e2[1]=1.0;e2[2]=1.0;e2[3]=0.0;e2[4]=1.0;e2[5]=1.0;e2[6]=0.0;e2[7]=0.0;
	f2[0]=0.0;f2[1]=0.0;f2[2]=1.0;f2[3]=1.0;f2[4]=0.0;f2[5]=1.0;f2[6]=1.0;f2[7]=0.0;*/
	
	for (deep=0; deep<5; deep++)
	//do
	{
		/*if (posaux.x<0.0 || posaux.x > 1.0 || posaux.y < 0.0 || posaux.y > 1.0)
		{
		  color_ultimo=0.0;
		  break;
		}
		else
		{*/
			// esto lo hace el CLAMP
			/*if (posaux.x<0.0) posaux.x=0.0;
			if (posaux.x>1.0) posaux.x=1.0;
			if (posaux.y<0.0) posaux.y=0.0;
			if (posaux.y>1.0) posaux.y=1.0;*/
		
			vec4 colSO = texture2D(texSO,posaux)*2.0-1.0;
    
			/*if (colSO.x!=0.0)
			{ */  
				vec4 colORI = texture2D(texORI,posaux);
				colORI.x *= 255.0;

				vec4 colDOMPOS = texture2D(texDOMPOS,posaux); 
				//colDOMPOS *= 255.0; //ANTES

				if (colORI.x<16.0)		{a=2.0;	b=0.0;	c=0.0;	d=2.0;	e=0.0;f=0.0;}
				else if (colORI.x<48.0)	{a=0.0;	b=2.0;	c=-2.0;	d=0.0;	e=1.0;f=0.0;}
				else if (colORI.x<80.0)	{a=-2.0;b=0.0;	c=0.0;	d=-2.0;	e=1.0;f=1.0;}
				else if (colORI.x<112.0){a=0.0;	b=-2.0;	c=2.0;	d=0.0;	e=0.0;f=1.0;}
				else if (colORI.x<144.0){a=-2.0;b=0.0;	c=0.0;	d=2.0;	e=1.0;f=0.0;} 
				else if (colORI.x<176.0){a=0.0;	b=-2.0;	c=-2.0;	d=0.0;	e=1.0;f=1.0;} 
				else if (colORI.x<208.0){a=2.0;	b=0.0;	c=0.0;	d=-2.0;	e=0.0;f=1.0;}
				else					{a=0.0;	b=2.0;	c=2.0;	d=0.0;	e=0.0;f=0.0;}
	
				// AHORA
				vec2 rel=posaux-((vec2(e,f)+floor(posaux*auxiliar1))*auxiliar2); 
				posaux=(vec2(a*rel.x+b*rel.y,c*rel.x+d*rel.y)+colDOMPOS.xw);
				
				/*if (colORI.x<16.0)		
				{
					posaux=(posaux-(floor(posaux*auxiliar1)*auxiliar2))*2.0+colDOMPOS.xw; 
				}
				else if (colORI.x<48.0)	
				{
					vec2 rel=posaux-((vec2(1.0,0.0)+floor(posaux*auxiliar1))*auxiliar2); 
					posaux=(vec2(rel.y,-rel.x)*2.0+colDOMPOS.xw);
				}
				else if (colORI.x<80.0)	
				{
					posaux=(posaux-(floor(posaux*auxiliar1)*auxiliar2)+1)*-2.0+colDOMPOS.xw; 
				}
				else if (colORI.x<112.0)
				{
					vec2 rel=posaux-((vec2(0.0,1.0)+floor(posaux*auxiliar1))*auxiliar2); 
					posaux=(vec2(-rel.y,rel.x)*2.0+colDOMPOS.xw);
				}
				else if (colORI.x<144.0)
				{
					vec2 rel=posaux-((vec2(1.0,0.0)+floor(posaux*auxiliar1))*auxiliar2); 
					posaux=(vec2(-rel.x,rel.y)*2.0+colDOMPOS.xw);
				} 
				else if (colORI.x<176.0)
				{
					vec2 rel=posaux-((vec2(1.0,0.0)+floor(posaux*auxiliar1))*auxiliar2); 
					posaux=(vec2(rel.y,rel.x)*-2.0+colDOMPOS.xw);
				} 
				else if (colORI.x<208.0)
				{
					vec2 rel=posaux-((vec2(0.0,1.0)+floor(posaux*auxiliar1))*auxiliar2); 
					posaux=(vec2(rel.x,-rel.y)*2.0+colDOMPOS.xw);
				}
				else					
				{
					vec2 rel=posaux-((floor(posaux*auxiliar1))*auxiliar2); 
					posaux=(vec2(rel.y,rel.x)*2.0+colDOMPOS.xw);
				}*/
				

				//color_penultimo = color_ultimo;

				factoracum += factorultimo;
				factorultimo = factor0*colSO.w;
				factor0*=colSO.x;

				color_ultimo = sem * factor0 + factoracum + factorultimo;            
			/*}
			else
			{
				color_ultimo=colSO.w;
				break;
			}*/
		//}
	} 
	//while (deep<4 || (deep<10 && abs(color_penultimo-color_ultimo)>prec));

	gl_FragColor = vec4(color_ultimo);
	gl_FragColor.a = 1.0;
}