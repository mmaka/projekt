#version 330 core

layout(location = 0) in vec3 polozenie_in;
//layout(location = 1) in vec3 normalna_in;
layout(location = 2) in vec2 wspTekstur_in;
layout(location = 3) in vec4 kolor_in;

uniform bool UwzglednijKolorWerteksu = true;
uniform vec4 Kolor = vec4(1.0,1.0,0.0,1.0);

uniform float przezroczystosc = 1.0f;

const mat4 macierzJednostkowa = mat4(1.0);
//uniform mat4 macierzSwiata = macierzJednostkowa;
//uniform mat4 macierzWidoku=macierzJednostkowa;
//uniform mat4 macierzRzutowania=macierzJednostkowa;
uniform mat4 mvp = macierzJednostkowa;
//mat4 macierzMVP = macierzRzutowania*macierzWidoku*macierzSwiata;
//uniform mat4 macierzNormalnych = macierzJednostkowa;

out vec4 polozenie;
out vec4 kolor;
out vec2 wspTekstur;
out vec3 polozenie_scena;
//out vec3 normalna_scena;

void main(void) {

	polozenie = vec4(polozenie_in, 1.0);
	//gl_Position = macierzMVP*polozenie;	
	gl_Position = mvp*polozenie;	
	kolor = vec4(kolor_in.r,kolor_in.g,kolor_in.b,przezroczystosc);
//	normalna_scena = mat3(macierzNormalnych)*normalna_in;
	wspTekstur = wspTekstur_in;
	
}
