varying vec4 color;
varying vec4 position;
varying vec3 normal;
varying vec2 texCoord;

attribute vec4 vPosition;
attribute vec3 vNormal;
attribute vec2 vTextureCoordinates;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct,Light1Position,Light2Position;
uniform int pointSource,directionalSource,shading,reflection;
uniform mat4 ModelView,Projection;
uniform float Shininess;


vec4 gouraudcolor( vec4 LightPosition ){
// Transform vertex position into camera coord.

	vec3 pos = (ModelView * vPosition).xyz;

	vec3 L = normalize( LightPosition.xyz - pos); //  assume point light source and compute direction 
	vec3 V = normalize( -pos ); // viewer direction // viewer direction (camera is at origin)
	vec3 H = normalize( L + V ); // halfway vector

	// Transform vertex normal into camera coord
	vec3 N = normalize( ModelView * vec4(vNormal, 0.0) ).xyz;
	vec3 R = 2.0 * dot(L, N) * N - L;	//

	// Compute terms in the illumination equation

	vec4 ambient = AmbientProduct;
	float Kd = max( dot(L, N), 0.0 ); //set diffuse to 0 if light is behind the surface point
	vec4  diffuse = Kd * DiffuseProduct;
	float Ks = pow( max(dot(V, R), 0.0), Shininess );	 // ignore specular component if viewer behind 
	
	vec4  specular = Ks * SpecularProduct;

	//ignore specular component if light is behind the surface point
	if( dot(L, N) < 0.0 ) {
		specular = vec4(0.0, 0.0, 0.0, 1.0);
	}

	return ambient + diffuse + specular;
}

void main(){
	// Gouraud

	if (shading == 1){
		color = vec4(0);
		color += gouraudcolor(Light1Position)*pointSource+gouraudcolor(Light2Position)*directionalSource; //if lights are turned on
		color.a = 1.0;
	}
	

	normal = vNormal;
	texCoord = vTextureCoordinates;
	position = ModelView * vPosition;
	gl_Position = Projection * position;

}