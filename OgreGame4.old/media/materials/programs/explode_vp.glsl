/*
  Our explosion vertex program that just passes the alpha value of the vertex
  on to the fragment program
*/
uniform mat4 worldViewProj;
 
void main()
{
   gl_FrontColor = gl_Color;
   gl_TexCoord[0] = gl_MultiTexCoord0;
   gl_Position = ftransform();
}