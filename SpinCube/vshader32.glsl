attribute  vec4 vPosition;
attribute  vec4 vColor;
varying vec4 color;
uniform mat4 Projection;
uniform mat4 ModelView;

void main() 
{
  gl_Position =  Projection * ModelView * vPosition;
  color = vColor;
} 