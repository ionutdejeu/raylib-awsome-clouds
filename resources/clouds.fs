#version 100
#define TAU 6.28318530718
#define MAX_SPOTS 3
uniform float iTime;
const vec2 iResolution = vec2(800,600);
const vec3 BackColor	= vec3(0.0, 0.4, 0.58);
const vec3 CloudColor	= vec3(0.18,0.70,0.87);

struct Spot {
    vec2 pos;        // window coords of spot
    float inner;    // inner fully transparent centre radius
    float radius;    // alpha fades out to this radius
	int isActive;
};

uniform Spot spots[MAX_SPOTS];  // Spotlight positions array
uniform vec3 tintColor;

float Func(float pX)
{
	return 0.6*(0.5*sin(0.1*pX) + 0.5*sin(0.553*pX) + 0.7*sin(1.2*pX));
}


float FuncR(float pX)
{
	return 0.5 + 0.25*(1.0 + sin(mod(40.0*pX, TAU)));
}


float Layer(vec2 pQ, float pT)
{
	vec2 Qt = 3.5*pQ;
	pT *= 0.5;
	Qt.x += pT;

	float Xi = floor(Qt.x);
	float Xf = Qt.x - Xi -0.5;

	vec2 C;
	float Yi;
	float D = 1.0 - step(Qt.y,  Func(Qt.x));

	// Disk:
	Yi = Func(Xi + 0.5);
	C = vec2(Xf, Qt.y - Yi ); 
	D =  min(D, length(C) - FuncR(Xi+ pT/80.0));

	// Previous disk:
	Yi = Func(Xi+1.0 + 0.5);
	C = vec2(Xf-1.0, Qt.y - Yi ); 
	D =  min(D, length(C) - FuncR(Xi+1.0+ pT/80.0));

	// Next Disk:
	Yi = Func(Xi-1.0 + 0.5);
	C = vec2(Xf+1.0, Qt.y - Yi ); 
	D =  min(D, length(C) - FuncR(Xi-1.0+ pT/80.0));

	return min(1.0, D);
}



void main()
{
	float alpha = 1.0;
	// Setup:
	vec2 UV = 2.0*(gl_FragCoord.xy - iResolution.xy/2.0) / min(iResolution.x, iResolution.y);	
	// Get the position of the current fragment (screen coordinates!)
    vec2 pos = vec2(gl_FragCoord.x, gl_FragCoord.y);

	// Render:
	vec3 Color= mix(BackColor,tintColor,0.1);
	float a = 1.0;
	for(float J=0.4; J<=1.0; J+=0.2)
	{
		// Cloud Layer: 
		float Lt =  iTime*(0.5  + 2.0*J)*(1.0 + 0.1*sin(226.0*J)) + 17.0*J;
		vec2 Lp = vec2(0.0, 0.3+1.5*( J - 0.5));
		float L = Layer(UV + Lp, Lt);
		// Blur and color:
		float Blur = 4.0*(0.5*abs(2.0 - 5.0*J))/(11.0 - 5.0*J);

		float V = mix( 0.0, 1.0, 1.0 - smoothstep( 0.0, 0.01 +0.2*Blur, L ) );
		vec3 Lc=  mix(CloudColor, vec3(1.0), J);
		 
		a= min(a,L);
		Color = mix(Color, Lc,  V);
	}

	gl_FragColor = vec4(Color,1.0-a);
}