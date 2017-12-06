#version 330 core

out vec4 kolor_out;

in vec4 polozenie;
in vec4 kolor;
in vec3 normalna_scena;
in vec2 wspTekstur;
in vec3 polozenie_scena;

uniform bool Teksturowanie = true;
uniform bool flaga = true;
uniform sampler2D ProbnikTekstury;

void main(void){
	
	vec4 teksel = vec4(1.0f,1.0f,1.0f,1.0f);
	
	if(Teksturowanie) {
	
		teksel = texture2D(ProbnikTekstury,wspTekstur);
		
	}
	teksel.a = kolor.a;
	kolor_out = teksel;
	kolor_out = vec4(1.0f-kolor_out.r,1.0f-kolor_out.g,1.0f-kolor_out.b,kolor_out.a);

}