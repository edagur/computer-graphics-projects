varying vec4 position;
varying vec3 normal;
varying vec2 texCoord;
varying vec4 color;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct,Light1Position,Light2Position;
uniform int displayType,shading,reflection,pointSource,directionalSource;
uniform mat4 ModelView;
uniform float Shininess;

uniform sampler2D texture;

vec4 fragmentcolor( vec4 LightPosition ){
	vec3 pos = position.xyz;

	
	vec3 L = normalize( LightPosition.xyz - pos);  //  assume point light source and compute direction 
	vec3 V = normalize( -pos ); // viewer direction // viewer direction (camera is at origin)
	vec3 H = normalize( L + V ); // halfway vector

	// Transform vertex normal into eye coordinates

	vec3 N = normalize( ModelView * vec4(normal, 0.0) ).xyz;
	vec3 R = 2.0 * dot(L, N) * N - L;

	// Compute terms in the illumination equation

	vec4 ambient = AmbientProduct;
	float Kd = max( dot(L, N), 0.0 ); //set diffuse to 0 if light is behind the surface point
	vec4 diffuse = Kd * DiffuseProduct;
	float Ks;
	if(reflection == 1) Ks = pow( max(dot(V, R), 0.0), Shininess ); //phong  ignore specular component if viewer behind
	if(reflection == 2) Ks = pow( max(dot(N, H), 0.0), Shininess );	// Modified Phong 
	vec4 specular = Ks * SpecularProduct;

	//ignore specular component if light is behind the surface point

	if( dot(L, N) < 0.0 ) {
		specular = vec4(0.0, 0.0, 0.0, 1.0);
	}

		return ambient + diffuse + specular;
}

void main(){
	
	if(displayType==1){gl_FragColor = vec4(1.0,0.0,0.0,1.0);}
	else if(displayType == 2){
	
		if(shading == 1){   //Groud shading is per vertex, so no detail with the fragment shader.
			gl_FragColor = color;}
		
		else{
			vec4 color = vec4(0);		
			color += fragmentcolor(Light1Position)*pointSource+fragmentcolor(Light2Position)*directionalSource;//if lights are turned on
			color.a = 1.0;
			gl_FragColor = color;
		}}
	else{
		if(shading == 1){   //Groud shading is per vertex, so no detail with the fragment shader.
			gl_FragColor = color+texture2D(texture, texCoord);}
			else{
	
		vec4 color = vec4(0);
		color += fragmentcolor(Light1Position)*pointSource+fragmentcolor(Light2Position)*directionalSource;//if lights are turned on
		gl_FragColor = color+texture2D(texture, texCoord);

	
	}
		}
}